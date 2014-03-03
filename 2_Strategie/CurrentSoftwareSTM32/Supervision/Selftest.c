/*
 *	Club Robot ESEO 2010 - 2011
 *	Chek'Norris
 *
 *	Fichier : Selftest.c
 *	Package : Supervision
 *	Description : Envoi et réception des messages de selftest.
 *	Auteur : Ronan & Patman
 *	Version 20110120
 */


#include "Selftest.h"
#include "SD/SD.h"
#include "config_pin.h"
#include "../QS/QS_outputlog.h"
#include "../QS/QS_CANmsgList.h"
#include "../QS/QS_uart.h"
#include "../QS/QS_watchdog.h"
#include "../QS/QS_can_over_uart.h"
#include "../QS/QS_adc.h"
#include "../QS/QS_who_am_i.h"
#include "SD/Libraries/fat_sd/ff.h"
#include "RTC.h"
#include "Buzzer.h"
#include "LCD_interface.h"

#define TIMEOUT_SELFTEST_ACT 		20000	// en ms
#define TIMEOUT_SELFTEST_PROP 		10000	// en ms
#define TIMEOUT_SELFTEST_STRAT		5000	// en ms
#define TIMEOUT_SELFTEST_BEACON_IR 	1000	// en ms
#define TIMEOUT_SELFTEST_BEACON_US 	1000	// en ms
#define MAX_ERRORS_NUMBER 			200
#define THRESHOLD_BATTERY_OFF	15000	//[mV] En dessous cette valeur, on considère que la puissance est absente
#define THRESHOLD_BATTERY_LOW	22000	//[mV] Réglage du seuil de batterie faible

#define LED_ON	1
#define LED_OFF	0

#define FORWARD 0x01
#define	REAR 0x10

#define TEMPS_SYNCHRO 10
#define NOMBRE_TESTS_BALISE 2

volatile bool_e ask_launch_selftest = FALSE;
volatile bool_e selftest_is_running = FALSE;
volatile SELFTEST_error_code_e errors[MAX_ERRORS_NUMBER];
volatile Uint8 errors_index = 0;
volatile Uint8 t500ms = 0;	//Minuteur [500ms]
volatile bool_e flag_500ms = FALSE;	//Flag qui permet de lancer la fonction SELFTEST_beacon_reask_periodic_sending


error_e SELFTEST_strategy(bool_e reset);

void SELFTEST_print_errors(SELFTEST_error_code_e * tab_errors, Uint8 size);
void SELFTEST_beacon_reask_periodic_sending(void);
Uint16 SELFTEST_measure24_mV(void);
static void SELFTEST_check_alim();


void SELFTEST_init(void)
{
	Uint8 i;
	errors[0] = SELFTEST_NOT_DONE;	//Cette erreur initiale sera écrasée, puisque l'index est à 0.
	for(i=1; i<MAX_ERRORS_NUMBER; i++)
	{
		errors[i] = SELFTEST_NO_ERROR;
	}
	errors_index = 0;
}

void SELFTEST_ask_launch(void)
{
	//On accepte une demande de selftest seulement avant ou après le match. pas pendant.
	if((!global.env.match_started || global.env.match_over))
		ask_launch_selftest = TRUE;
}



//Fonction privée qui scrute le contenu des messages CAN de réponse au selftest.
void SELFTEST_declare_errors(CAN_msg_t * msg, SELFTEST_error_code_e error)
{
	Uint8 i;
	if(error != SELFTEST_NO_ERROR)
	{
		errors[errors_index] = error;
		if(errors_index < MAX_ERRORS_NUMBER - 1)
			errors_index++;
	}
	if(msg != NULL && msg->size != 0)	//L'ordre des arguments est important (pour ne pas accéder à msg si ce pointeur est NULL)
	{
		for(i=0;i<msg->size;i++)
		{
			if(msg->data[i] != SELFTEST_NO_ERROR)
			{
				//Récupération des codes d'erreurs envoyés.
				errors[errors_index] = msg->data[i];
				if(errors_index < MAX_ERRORS_NUMBER - 1)
					errors_index++;
			}
		}
	}
}

void SELFTEST_process_500ms(void)
{
	//debug_printf("%d\n",SELFTEST_measure24_mV());
	if(selftest_is_running)
		LED_SELFTEST = !LED_SELFTEST;	//On fait clignoter la led selftest à 2hz pendant le selftest.
	if(t500ms)
		t500ms--;
	flag_500ms = TRUE;
}

void SELFTEST_process_main(void)
{
	SELFTEST_update(NULL);
	SELFTEST_beacon_reask_periodic_sending();	//Toutes les 200 secondes, il faut redemander à la carte balise d'envoyer des messages périodiquement
	SELFTEST_check_alim();
}

//Machine a état du selftest, doit être appelée : dans le process de tâche de fond ET à chaque réception d'un message can de selftest.
void SELFTEST_update(CAN_msg_t* CAN_msg_received)
{
	static watchdog_id_t watchdog_id;
	static bool_e flag_timeout;
	static bool_e act_ping_ok = FALSE;
	static bool_e prop_ping_ok = FALSE;
	static bool_e beacon_ping_ok = FALSE;
	typedef enum
	{
		INIT = 0,
		WAIT_SELFTEST_LAUNCH,
		SELFTEST_PING_ACT_PROP,
		SELFTEST_ACT,
		SELFTEST_PROP,
		SELFTEST_BEACON_IR,
		SELFTEST_BEACON_US,
		SELFTEST_STRAT,
		SELFTEST_END
	}state_e;
	static state_e state = INIT;
	static state_e previous_state = INIT;
	bool_e entrance;

	entrance = (state != previous_state)?TRUE:FALSE;
	previous_state = state;

	switch(state)
	{
		case INIT:
			WATCHDOG_init();
			debug_printf("Mesure du 24V : %dmV\n",SELFTEST_measure24_mV());
			errors_index = 0;
			LED_SELFTEST = FALSE;
			state = WAIT_SELFTEST_LAUNCH;
		break;
		case WAIT_SELFTEST_LAUNCH:
			if(ask_launch_selftest)	//demande de lancement du selftest (PAS PENDANT LE MATCH !)
			{
				debug_printf("\r\n_________________________ SELFTEST __________________________\r\n\r\n");
				//lcd_printf_last_line("Selftest : launch\n");
				errors_index = 0;	//On REMET le compteur d'erreur à 0.
				state = SELFTEST_STRAT;
				flag_timeout = FALSE;
				selftest_is_running = TRUE;
				act_ping_ok = FALSE;
				prop_ping_ok = FALSE;
				beacon_ping_ok = FALSE;
			}
			break;
		case SELFTEST_STRAT:
			if(entrance)
			{
				SELFTEST_strategy(TRUE);	//Reset de la sous-machine a états
				flag_timeout = FALSE;
				watchdog_id = WATCHDOG_create_flag(TIMEOUT_SELFTEST_STRAT, (bool_e*) &(flag_timeout));
				debug_printf("SELFTEST STRATEGY\r\n");
			}
			if(SELFTEST_strategy(FALSE) != IN_PROGRESS)	//La fonction SELFTEST_strategy déclare elle-même ses erreurs.
			{
				WATCHDOG_stop(watchdog_id);
				state = SELFTEST_PING_ACT_PROP;
			}
			if(flag_timeout)	//Timeout
			{
				debug_printf("SELFTEST STRATEGY TIMEOUT\r\n");
				state = SELFTEST_PING_ACT_PROP;
			}
			break;
		case SELFTEST_PING_ACT_PROP:
			if(entrance)
			{
				flag_timeout = 0;
				watchdog_id = WATCHDOG_create_flag(200, (bool_e*) &(flag_timeout));	//timeout 200ms
				CAN_send_sid(ACT_PING);
				CAN_send_sid(PROP_PING);
				CAN_send_sid(BEACON_PING);
				debug_printf("SELFTEST PING OTHER BOARDS\r\n");
			}
			if(CAN_msg_received != NULL)
			{
				if(CAN_msg_received->sid == STRAT_ACT_PONG)
				{
					if(CAN_msg_received->data[0] != QS_WHO_AM_I_get())
						SELFTEST_declare_errors(NULL,SELFTEST_STRAT_WHO_AM_I_ARE_NOT_THE_SAME);
					act_ping_ok = TRUE;
				}
				if(CAN_msg_received->sid == STRAT_PROP_PONG)
				{
					if(CAN_msg_received->data[0] != QS_WHO_AM_I_get())
						SELFTEST_declare_errors(NULL,SELFTEST_STRAT_WHO_AM_I_ARE_NOT_THE_SAME);
					prop_ping_ok = TRUE;
				}
				if(CAN_msg_received->sid == STRAT_BEACON_PONG)
					beacon_ping_ok = TRUE;
			}
			if(act_ping_ok == TRUE && prop_ping_ok == TRUE && beacon_ping_ok == TRUE)
			{
				WATCHDOG_stop(watchdog_id);
				state = SELFTEST_ACT;
			}
			if(flag_timeout)
				state = SELFTEST_ACT;


			if(state == SELFTEST_ACT)	//Déclaration des erreurs à la sortie de l'état !
			{
				if(!act_ping_ok)
					SELFTEST_declare_errors(CAN_msg_received, SELFTEST_ACT_UNREACHABLE);
				if(!prop_ping_ok)
					SELFTEST_declare_errors(CAN_msg_received, SELFTEST_PROP_UNREACHABLE);
				if(!beacon_ping_ok)
					SELFTEST_declare_errors(CAN_msg_received, SELFTEST_BEACON_UNREACHABLE);
			}
			break;
		case SELFTEST_ACT:
			if(entrance)
			{
				flag_timeout = FALSE;
				if(act_ping_ok)
				{
					CAN_send_sid(ACT_DO_SELFTEST);
					watchdog_id = WATCHDOG_create_flag(TIMEOUT_SELFTEST_ACT, (bool_e*) &(flag_timeout));
					debug_printf("SELFTEST ACT\r\n");
				}
				else
					state = SELFTEST_PROP;
			}
			if(CAN_msg_received != NULL)
				if(CAN_msg_received->sid == STRAT_ACT_SELFTEST_DONE)
				{
					//Retour de la carte actionneur
					SELFTEST_declare_errors(CAN_msg_received, SELFTEST_NO_ERROR);
					WATCHDOG_stop(watchdog_id);
					state = SELFTEST_PROP;
				}
			if(flag_timeout)	//Timeout
			{
				debug_printf("SELFTEST ACT TIMEOUT\r\n");
				state = SELFTEST_PROP;
			}
			break;
		case SELFTEST_PROP:
			if(entrance)
			{
				flag_timeout = FALSE;
				if(prop_ping_ok)
				{
					CAN_send_sid(PROP_DO_SELFTEST);
					watchdog_id = WATCHDOG_create_flag(TIMEOUT_SELFTEST_PROP, (bool_e*) &(flag_timeout));
					debug_printf("SELFTEST PROP\r\n");
				}
				else
					state = SELFTEST_BEACON_IR;
			}
			if(CAN_msg_received != NULL)
				if(CAN_msg_received->sid == STRAT_PROP_SELFTEST_DONE)
				{
					//Retour de la carte Propulsion
					SELFTEST_declare_errors(CAN_msg_received, SELFTEST_NO_ERROR);
					WATCHDOG_stop(watchdog_id);
					state = SELFTEST_BEACON_IR;
				}
			if(flag_timeout)	//Timeout
			{
				debug_printf("SELFTEST PROP TIMEOUT\r\n");
				state = SELFTEST_BEACON_IR;
			}
			break;
		case SELFTEST_BEACON_IR:
			if(entrance)
			{
				flag_timeout = FALSE;
				if(beacon_ping_ok)
				{
					CAN_send_sid(BEACON_DO_SELFTEST);
					watchdog_id = WATCHDOG_create_flag(TIMEOUT_SELFTEST_BEACON_IR, (bool_e*) &(flag_timeout));
					debug_printf("SELFTEST BEACON\r\n");
				}
				else
					state = SELFTEST_END;
			}
			if(CAN_msg_received != NULL)
				if(CAN_msg_received->sid == STRAT_BEACON_IR_SELFTEST_DONE)
				{
					//Retour de la carte Beacon IR
					SELFTEST_declare_errors(CAN_msg_received, SELFTEST_NO_ERROR);
					WATCHDOG_stop(watchdog_id);
					state = SELFTEST_END;
				}
			if(flag_timeout)	//Timeout
			{
				debug_printf("SELFTEST BEACON TIMEOUT\r\n");
				state = SELFTEST_END;
			}
			break;

		case SELFTEST_END:
			selftest_is_running = FALSE;
			ask_launch_selftest = FALSE;	//Fin du selftest (et du clignotement de la led selftest).
			if(errors_index == 0)//Tout s'est bien passé
			{
				LED_SELFTEST = TRUE;
				debug_printf("SELFTEST : OK ! CHAMPOMY LES GARS !\n");
			}
			else	//Des problèmes ont été rencontrés
			{
				LED_SELFTEST = FALSE;
				SELFTEST_print_errors((SELFTEST_error_code_e *)errors, errors_index);
			}
			state = WAIT_SELFTEST_LAUNCH;
			break;
		default :
			break;
	}
}

error_e SELFTEST_strategy(bool_e reset)
{
	typedef enum
	{
		INIT = 0,
		TEST_LEDS_AND_BUZZER,
		TEST_RTC,
		TEST_MEASURE24,
		TEST_BIROUTE,
		TEST_SD_CARD,
		FAIL,
		DONE
	}state_e;
	static state_e state = INIT;
	static state_e previous_state = INIT;
	bool_e entrance;
	error_e ret;
	static FIL file;
	Uint8 nb_written;
	date_t date;
	Uint8 status;
	Uint16 battery_level;

	entrance = (state != previous_state)?TRUE:FALSE;
	previous_state = state;
	if(reset)
		state = INIT;

	ret = IN_PROGRESS;
	switch(state)
	{
		case INIT:
			state = TEST_LEDS_AND_BUZZER;
			break;
		case TEST_LEDS_AND_BUZZER:
			if(entrance)
			{
				t500ms = 3;	//1,5 secondes
				BUZZER_play(100, DEFAULT_NOTE, 4);	//4 buzzs de 100ms + 3 pauses de 100ms = O,7 secondes
			}
			LED_SELFTEST = (t500ms&1);	//Si t est impair, on allume toutes les leds.
			LED_ERROR 	= LED_SELFTEST;
			LED_CAN 	= LED_SELFTEST;
			LED_UART 	= LED_SELFTEST;
			LED_RUN 	= LED_SELFTEST;
			LED_USER 	= LED_SELFTEST;

			if(!t500ms)	//Lorsque T vaut 0 (et que les leds sont éteintes...)
				state = TEST_RTC;
			break;
		case TEST_RTC:
			status = RTC_get_local_time (&date);
			if(!status)
				SELFTEST_declare_errors(NULL,SELFTEST_STRAT_RTC);
			state = TEST_MEASURE24;
			break;
		case TEST_MEASURE24:
			battery_level = SELFTEST_measure24_mV();
			debug_printf("Mesure du 24V : %dmV\n",battery_level);
			if(battery_level < THRESHOLD_BATTERY_OFF)
				SELFTEST_declare_errors(NULL,SELFTEST_STRAT_BATTERY_NO_24V);
			else if(battery_level < THRESHOLD_BATTERY_LOW)
				SELFTEST_declare_errors(NULL,SELFTEST_STRAT_BATTERY_LOW);
			state = TEST_BIROUTE;
			break;
		case TEST_BIROUTE:
			if(!BIROUTE)
				SELFTEST_declare_errors(NULL,SELFTEST_STRAT_BIROUTE_FORGOTTEN);
			state = TEST_SD_CARD;
			break;
		case TEST_SD_CARD:
			nb_written = 0;
			if(f_open(&file, "selftest.txt", FA_WRITE | FA_CREATE_ALWAYS | FA_OPEN_ALWAYS) == FR_OK)
			{
				f_write(&file, "test_string", 11, (unsigned int *)&nb_written);
				f_close(&file);
			}
			if(nb_written != 11)
				SELFTEST_declare_errors(NULL,SELFTEST_STRAT_SD_WRITE_FAIL);
			state = DONE;
			break;
		case FAIL:
			ret = NOT_HANDLED;
			break;
		case DONE:
			ret = END_OK;
			break;
	}
	return ret;
}

//Fonction publique : renvoie la tension de la batterie en mV.
Uint16 SELFTEST_measure24_mV(void)
{
	Uint32 measure = (Uint32)ADC_getValue(ADC_CHANNEL_MEASURE24);
	return (Uint16)((measure * 3000*110/10)/1024);	//3000 [mV] correspond à 4096 [ADC]
}

void SELFTEST_print_errors(SELFTEST_error_code_e * tab_errors, Uint8 size)
{
	Uint8 i;
	char buf[20]; // Buffer qui contient la chaine de caractere pour l'écran LCD
	debug_printf("SELFTEST ENDED with %d error(s) :\n",size);
	sprintf(buf,"Stest ENDED: %2d ERR",size);
	LCD_free_line(buf,0);
	LCD_free_line(buf,1);
	BUZZER_play(200, DEFAULT_NOTE, size);	//Autant de bip que d'erreurs !
	for(i=0;i<size;i++)
	{
		if(errors[i] != SELFTEST_NO_ERROR)
		{
			debug_printf("\terror %3d : ",errors[i]);
			switch(errors[i])
			{
				case SELFTEST_NOT_DONE:							debug_printf("NOT_DONE");								break;
				case SELFTEST_FAIL_UNKNOW_REASON:				debug_printf("FAIL_UNKNOW_REASON");						break;
				case SELFTEST_NO_POWER:							debug_printf("NO_POWER");								break;
				case SELFTEST_TIMEOUT:							debug_printf("TIMEOUT");								break;
				case SELFTEST_PROP_FAILED:						debug_printf("PROP_FAILED");							break;
				case SELFTEST_PROP_HOKUYO_FAILED:				debug_printf("SELFTEST_PROP_HOKUYO_FAILED");			break;
				case SELFTEST_PROP_DT10_1_FAILED:				debug_printf("PROP_DT10_1_FAILED");						break;
				case SELFTEST_PROP_DT10_2_FAILED:				debug_printf("PROP_DT10_2_FAILED");						break;
				case SELFTEST_PROP_DT10_3_FAILED:				debug_printf("PROP_DT10_3_FAILED");						break;
				case SELFTEST_PROP_IN_SIMULATION_MODE:			debug_printf("SELFTEST_PROP_IN_SIMULATION_MODE");		break;
				case SELFTEST_PROP_IN_LCD_TOUCH_MODE:			debug_printf("SELFTEST_PROP_IN_LCD_TOUCH_MODE");		break;
				case SELFTEST_STRAT_BIROUTE_NOT_IN_PLACE:		debug_printf("STRAT_BIROUTE_NOT_IN_PLACE");				break;
				case SELFTEST_STRAT_RTC:						debug_printf("SELFTEST_STRAT_RTC");						break;
				case SELFTEST_STRAT_BATTERY_NO_24V:				debug_printf("SELFTEST_STRAT_BATTERY_NO_24V");			break;
				case SELFTEST_STRAT_BATTERY_LOW:				debug_printf("SELFTEST_STRAT_BATTERY_LOW");				break;
				case SELFTEST_STRAT_WHO_AM_I_ARE_NOT_THE_SAME:	debug_printf("SELFTEST_STRAT_WHO_AM_I_ARE_NOT_THE_SAME");break;
				case SELFTEST_STRAT_BIROUTE_FORGOTTEN:			debug_printf("SELFTEST_STRAT_BIROUTE_FORGOTTEN");		break;
				case SELFTEST_STRAT_SD_WRITE_FAIL:				debug_printf("SELFTEST_STRAT_SD_WRITE_FAIL");			break;
				case SELFTEST_ACT_UNREACHABLE:					debug_printf("SELFTEST_ACT_UNREACHABLE");				break;
				case SELFTEST_PROP_UNREACHABLE:					debug_printf("SELFTEST_PROP_UNREACHABLE");				break;
				case SELFTEST_BEACON_UNREACHABLE:				debug_printf("SELFTEST_BEACON_UNREACHABLE");			break;
					// Actionneurs


				case SELFTEST_ACT_MISSING_TEST:					debug_printf("SELFTEST_ACT_MISSING_TEST");				break;	//Test manquant après un timeout du selftest actionneur, certains actionneur n'ont pas le selftest d'implémenté ou n'ont pas terminé leur action (ou plus rarement, la pile était pleine et le selftest n'a pas pu se faire)
				case SELFTEST_ACT_UNKNOWN_ACT:					debug_printf("SELFTEST_ACT_UNKNOWN_ACT");				break;	//Un actionneur inconnu a fail son selftest. Pour avoir le nom, ajoutez un SELFTEST_ACT_xxx ici et gérez l'actionneur dans selftest.c de la carte actionneur
				case SELFTEST_ACT_LANCELAUNCHER:				debug_printf("SELFTEST_ACT_LANCELAUNCHER");				break;
				case SELFTEST_ACT_FRUIT_MOUTH:					debug_printf("SELFTEST_ACT_FRUIT_MOUTH");				break;
				case SELFTEST_ACT_SMALL_ARM:					debug_printf("SELFTEST_ACT_SMALL_ARM");					break;
				case SELFTEST_ACT_ARM:							debug_printf("SELFTEST_ACT_ARM");						break;
				case SELFTEST_ACT_FILET:						debug_printf("SELFTEST_ACT_FILET");						break;
				case SELFTEST_POMPE:							debug_printf("SELFTEST_POMPE");							break;

				default:										debug_printf("UNKNOW_ERROR_CODE"); 						break;
			}
			debug_printf("\n");
		}
	}
}




//void led_us_update(selftest_beacon_e state){
		/*switch(state)
		{
			case BEACON_ERROR:
				LED_BEACON_US_GREEN = LED_OFF;
				LED_BEACON_US_RED = !LED_BEACON_US_RED;
				break;
			case BEACON_NEAR:
				LED_BEACON_US_GREEN = !LED_BEACON_US_GREEN;
				LED_BEACON_US_RED = LED_BEACON_US_GREEN;
				break;
			case BEACON_FAR:
				LED_BEACON_US_RED = LED_OFF;
				LED_BEACON_US_GREEN = !LED_BEACON_US_GREEN;
				break;
			default:
				LED_BEACON_US_RED = LED_ON;
				LED_BEACON_US_GREEN = LED_OFF;
				break;
		}*/
//}

void led_ir_update(selftest_beacon_e state)
	{
		switch(state)
		{
			case BEACON_ERROR:
				LED_BEACON_IR_GREEN = LED_OFF;
				LED_BEACON_IR_RED = !LED_BEACON_IR_RED;
				break;
			case BEACON_NEAR:
				LED_BEACON_IR_GREEN = !LED_BEACON_IR_GREEN;
				LED_BEACON_IR_RED = LED_BEACON_IR_GREEN;
				break;
			case BEACON_FAR:
				LED_BEACON_IR_RED = LED_OFF;
				LED_BEACON_IR_GREEN = !LED_BEACON_IR_GREEN;
				break;
			default:
				LED_BEACON_IR_RED = LED_ON;
				LED_BEACON_IR_GREEN = LED_OFF;
				break;
		}
}


void SELFTEST_beacon_reask_periodic_sending(void)
{
	static Uint16 counter = 1;
	if(flag_500ms)
	{
		if(global.env.match_started == FALSE)	//Tant que le match n'est pas commencé, on demande régulièrement aux balises des envois périodiques
		{
			counter--;
			if(counter == 0)
			{
				counter = 400;	//200 secondes
				CAN_send_sid(BEACON_ENABLE_PERIODIC_SENDING);
			}
		}
		flag_500ms = FALSE;
	}
}





volatile static struct {
	Uint8 beacon_error_ir_counter[8];
	Uint8 beacon_error_us_counter[8];
	Uint16 report_counter;
}beacon_error_report = {{0},{0}, 0};

void SELFTEST_beacon_counter_init(){
	Uint8 i;
	for(i=0;i<8;i++)
		beacon_error_report.beacon_error_ir_counter[i] = 0;
	for(i=0;i<8;i++)
		beacon_error_report.beacon_error_us_counter[i] = 0;
	beacon_error_report.report_counter = 0;
}

void error_counters_update(CAN_msg_t * msg){
	switch(msg->sid){
		case BEACON_ADVERSARY_POSITION_IR:
			if(msg->data[0] & 0b0)
				beacon_error_report.beacon_error_ir_counter[msg->data[0]]++;
			if(msg->data[0] & 0b00000001)
				beacon_error_report.beacon_error_ir_counter[msg->data[1]]++;
			if(msg->data[0] & 0b00000010)
				beacon_error_report.beacon_error_ir_counter[msg->data[2]]++;
			if(msg->data[0] & 0b00000100)
				beacon_error_report.beacon_error_ir_counter[msg->data[3]]++;
			if(msg->data[0] & 0b00001000)
				beacon_error_report.beacon_error_ir_counter[msg->data[4]]++;
			if(msg->data[0] & 0b00010000)
				beacon_error_report.beacon_error_ir_counter[msg->data[5]]++;
			if(msg->data[0] & 0b10000000)
				beacon_error_report.beacon_error_ir_counter[msg->data[6]]++;

			if(msg->data[4] & 0b0)
				beacon_error_report.beacon_error_ir_counter[msg->data[0]]++;
			if(msg->data[4] & 0b00000001)
				beacon_error_report.beacon_error_ir_counter[msg->data[1]]++;
			if(msg->data[4] & 0b00000010)
				beacon_error_report.beacon_error_ir_counter[msg->data[2]]++;
			if(msg->data[4] & 0b00000100)
				beacon_error_report.beacon_error_ir_counter[msg->data[3]]++;
			if(msg->data[4] & 0b00001000)
				beacon_error_report.beacon_error_ir_counter[msg->data[4]]++;
			if(msg->data[4] & 0b00010000)
				beacon_error_report.beacon_error_ir_counter[msg->data[5]]++;
			if(msg->data[4] & 0b10000000)
				beacon_error_report.beacon_error_ir_counter[msg->data[6]]++;


			beacon_error_report.report_counter++;
			break;
		case BEACON_ADVERSARY_POSITION_US:
			if(msg->data[0] & 0b0)
				beacon_error_report.beacon_error_us_counter[msg->data[0]]++;
			if(msg->data[0] & 0b00000001)
				beacon_error_report.beacon_error_us_counter[msg->data[1]]++;
			if(msg->data[0] & 0b00000010)
				beacon_error_report.beacon_error_us_counter[msg->data[2]]++;
			if(msg->data[0] & 0b00000100)
				beacon_error_report.beacon_error_us_counter[msg->data[3]]++;
			if(msg->data[0] & 0b00001000)
				beacon_error_report.beacon_error_us_counter[msg->data[4]]++;
			if(msg->data[0] & 0b00010000)
				beacon_error_report.beacon_error_us_counter[msg->data[5]]++;
			if(msg->data[0] & 0b10000000)
				beacon_error_report.beacon_error_us_counter[msg->data[6]]++;

			if(msg->data[4] & 0b0)
				beacon_error_report.beacon_error_us_counter[msg->data[0]]++;
			if(msg->data[4] & 0b00000001)
				beacon_error_report.beacon_error_us_counter[msg->data[1]]++;
			if(msg->data[4] & 0b00000010)
				beacon_error_report.beacon_error_us_counter[msg->data[2]]++;
			if(msg->data[4] & 0b00000100)
				beacon_error_report.beacon_error_us_counter[msg->data[3]]++;
			if(msg->data[4] & 0b00001000)
				beacon_error_report.beacon_error_us_counter[msg->data[4]]++;
			if(msg->data[4] & 0b00010000)
				beacon_error_report.beacon_error_us_counter[msg->data[5]]++;
			if(msg->data[4] & 0b10000000)
				beacon_error_report.beacon_error_us_counter[msg->data[6]]++;



			beacon_error_report.report_counter++;
			break;
		default:
			break;
	}
}

void SELFTEST_get_match_report_IR(CAN_msg_t * msg){
	int i;
	msg->sid = IR_ERROR_RESULT;
	msg->size = 8;
	for(i=0;i<8;i++)
		msg->data[i] = beacon_error_report.beacon_error_ir_counter[i];
}

void SELFTEST_get_match_report_US(CAN_msg_t * msg){
	int i;
	msg->sid = US_ERROR_RESULT;
	msg->size = 8;
	for(i=0;i<8;i++)
		msg->data[i] = beacon_error_report.beacon_error_us_counter[i];
}


void SELFTEST_update_led_beacon(CAN_msg_t * can_msg)
{
	switch(can_msg->sid)
	{
		case BEACON_ADVERSARY_POSITION_IR:
			if(global.env.match_started == TRUE)
				//Enregistrement du type d'erreur
				error_counters_update(can_msg);
			//Si le message d'erreur n'est pas nul autrement dit si il y a une erreur quelconque
			if(can_msg->data[0] || can_msg->data[4])
				led_ir_update(BEACON_ERROR);
			else if(can_msg->data[3] < 102 || can_msg->data[7] < 102) //Distance IR en cm
				led_ir_update(BEACON_NEAR);
			else
				led_ir_update(BEACON_FAR);
			break;

	/*	case BEACON_ADVERSARY_POSITION_US:
			if(global.env.match_started == TRUE)
				//Enregistrement du type d'erreur
				error_counters_update(can_msg);
			//Si le message d'erreur n'est pas nul autrement dit si il y a une erreur quelconque
			if(can_msg->data[0] || can_msg->data[4])
				led_us_update(BEACON_ERROR);
			else if(can_msg->data[1] < 4 || can_msg->data[5] < 4) //Distance US en mm on test seulement le poids fort autrement dit 1024mm
				led_us_update(BEACON_NEAR);
			else
				led_us_update(BEACON_FAR);
			break;*/
		default:
			break;
	}
}

void SELFTEST_check_alim(){
	typedef enum{
		ALIM_Off = 0,
		ALIM_On
	}state_e;
	static state_e state = ALIM_Off;
	static bool_e says = FALSE;
	global.env.alim_value = SELFTEST_measure24_mV();
	CAN_msg_t msg;

	if(global.env.alim_value > THRESHOLD_BATTERY_OFF && state != ALIM_On){
		msg.sid = BROADCAST_ALIM;
		msg.data[0] = ALIM_ON;
		msg.data[1] = (Uint8)((global.env.alim_value >> 8) & 0x00FF);
		msg.data[2] = (Uint8)(global.env.alim_value & 0x00FF);
		msg.size = 3;
		CAN_send(&msg);
		state = ALIM_On;
		global.env.alim = TRUE;
	}else if(global.env.alim_value < THRESHOLD_BATTERY_OFF && state != ALIM_Off){
		msg.sid = BROADCAST_ALIM;
		msg.data[0] = ALIM_OFF;
		msg.data[1] = (Uint8)((global.env.alim_value >> 8) & 0x00FF);
		msg.data[2] = (Uint8)(global.env.alim_value & 0x00FF);
		msg.size = 3;
		CAN_send(&msg);
		state = ALIM_Off;
		global.env.alim = FALSE;
	}

	if(global.env.alim_value > 5000 && global.env.alim_value < 20000 && !says){
		//debug_printf("Batterie instable -> %d mV\n", global.env.alim_value);
		says = TRUE;
	}else
		says = FALSE;
}
