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
#include "../QS/QS_WHO_AM_I.h"
#include "SD/Libraries/fat_sd/ff.h"
#include "RTC.h"

#define TIMEOUT_SELFTEST_ACT 		20000	// en ms
#define TIMEOUT_SELFTEST_PROP 		10000	// en ms
#define TIMEOUT_SELFTEST_STRAT		5000	// en ms
#define TIMEOUT_SELFTEST_BEACON_IR 	1000	// en ms
#define TIMEOUT_SELFTEST_BEACON_US 	1000	// en ms
#define MAX_ERRORS_NUMBER 20
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
SELFTEST_error_code_e errors[MAX_ERRORS_NUMBER];
volatile Uint8 errors_index = 0;
volatile Uint8 t500ms = 0;	//Minuteur [500ms]
typedef enum
{
	END_OK=0,
	IN_PROGRESS,
	NOT_HANDLED
}error_e;

error_e SELFTEST_strategy(bool_e reset);
void SELFTEST_update(CAN_msg_t* CAN_msg_received);
void SELFTEST_print_errors(SELFTEST_error_code_e * tab_errors, Uint8 size);
void SELFTEST_beacon_reask_periodic_sending(void);
Uint16 SELFTEST_measure24_mV(void);

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
			//Récupération des codes d'erreurs envoyés.
			errors[errors_index] = msg->data[i];
			if(errors_index < MAX_ERRORS_NUMBER - 1)
				errors_index++;
		}
	}
}

void SELFTEST_process_500ms(void)
{
	if(selftest_is_running)
		LED_SELFTEST = !LED_SELFTEST;	//On fait clignoter la led selftest à 2hz pendant le selftest.
	if(t500ms)
		t500ms--;
}

void SELFTEST_process_main(void)
{
	SELFTEST_update(NULL);
	SELFTEST_beacon_reask_periodic_sending();	//Toutes les 200 secondes, il faut redemander à la carte balise d'envoyer des messages périodiquement
	//TODO... ?
}

//Machine a état du selftest, doit être appelée : dans le process de tâche de fond ET à chaque réception d'un message can de selftest.
void SELFTEST_update(CAN_msg_t* CAN_msg_received)
{
	static watchdog_id_t watchdog_id;
	static bool_e flag_timeout;
	typedef enum
	{
		INIT = 0,
		WAIT_SELFTEST_LAUNCH,
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
			errors_index = 0;
			LED_SELFTEST = FALSE;
			state = WAIT_SELFTEST_LAUNCH;
		break;
		case WAIT_SELFTEST_LAUNCH:
			if(!global.env.match_started && ask_launch_selftest)	//Match non commencé et demande de selftest
			{
				debug_printf("\r\n_________________________ SELFTEST __________________________\r\n\r\n");
				//lcd_printf_last_line("Selftest : launch\n");
				errors_index = 0;	//On REMET le compteur d'erreur à 0.
				state = SELFTEST_STRAT;
				flag_timeout = FALSE;
				selftest_is_running = TRUE;
			}
			break;
		case SELFTEST_STRAT:
			if(entrance)
			{
				SELFTEST_strategy(TRUE);	//Reset de la sous-machine a états
				flag_timeout = FALSE;
				watchdog_id = WATCHDOG_create_flag(TIMEOUT_SELFTEST_STRAT, (bool_e*) &(flag_timeout));
			}
			if(SELFTEST_strategy(FALSE) != IN_PROGRESS)	//La fonction SELFTEST_strategy déclare elle-même ses erreurs.
				state = SELFTEST_ACT;
			if(flag_timeout)	//Timeout
				state = SELFTEST_ACT;
			break;
		case SELFTEST_ACT:
			/////////////////////////////TODO : faire un ping préalable pour vérifier que les autres cartes sont là avant d'entrer dans le timeout de 20 secondes !!
			if(entrance)
			{
				CAN_send_sid(ACT_DO_SELFTEST);
				flag_timeout = FALSE;
				watchdog_id = WATCHDOG_create_flag(TIMEOUT_SELFTEST_ACT, (bool_e*) &(flag_timeout));
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
				state = SELFTEST_PROP;
			break;
		case SELFTEST_PROP:
			if(entrance)
			{
				CAN_send_sid(PROP_DO_SELFTEST);
				flag_timeout = FALSE;
				watchdog_id = WATCHDOG_create_flag(TIMEOUT_SELFTEST_PROP, (bool_e*) &(flag_timeout));
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
				state = SELFTEST_BEACON_IR;
			break;
		case SELFTEST_BEACON_IR:
			if(entrance)
			{
				CAN_send_sid(BEACON_DO_SELFTEST);
				flag_timeout = FALSE;
				watchdog_id = WATCHDOG_create_flag(TIMEOUT_SELFTEST_BEACON_IR, (bool_e*) &(flag_timeout));
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
				state = SELFTEST_END;
			break;

		case SELFTEST_END:
			selftest_is_running = FALSE;
			ask_launch_selftest = FALSE;	//Fin du selftest (et du clignotement de la led selftest).
			if(errors_index > 0)//Tout s'est bien passé
			{
				LED_SELFTEST = TRUE;
				debug_printf("SELFTEST : OK !\n");
			}
			else	//Des problèmes ont été rencontrés
			{
				LED_SELFTEST = FALSE;
				SELFTEST_print_errors(errors, errors_index);
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
		TEST_WHO_AM_I_1,
		TEST_WHO_AM_I_2,
		TEST_BIROUTE,
		TEST_SD_CARD,
		FAIL,
		DONE
	}state_e;
	static state_e state = INIT;
	static state_e previous_state = INIT;
	bool_e entrance;
	error_e ret;
	static watchdog_id_t watchdog_id;
	static bool_e flag_timeout;
	static FIL file;
	static robot_id_e prop_robot_id;
	static robot_id_e act_robot_id;
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
				t500ms = 5;	//2,5 secondes
			LED_SELFTEST = (t500ms&1);	//Si t est impair, on allume toutes les leds.
			LED_ERROR 	= LED_SELFTEST;
			LED_CAN 	= LED_SELFTEST;
			LED_UART 	= LED_SELFTEST;
			LED_RUN 	= LED_SELFTEST;
			LED_USER 	= LED_SELFTEST;
			//if(t500ms&1)
			//	BUZZER_run(FREQ);//TODO lier avec le module BUZZER

			if(!t500ms)	//Lorsque T vaut 0 (et que les leds sont éteintes...)
			{
				//BUZZER_stop();
				state = TEST_RTC;
			}
			break;
		case TEST_RTC:
			status = RTC_get_local_time (&date);
			if(!status)
				SELFTEST_declare_errors(NULL,SELFTEST_STRAT_RTC);
			state = TEST_MEASURE24;
			break;
		case TEST_MEASURE24:
			battery_level = SELFTEST_measure24_mV();
			if(battery_level < THRESHOLD_BATTERY_OFF)
				SELFTEST_declare_errors(NULL,SELFTEST_STRAT_BATTERY_NO_24V);
			else if(battery_level < THRESHOLD_BATTERY_LOW)
				SELFTEST_declare_errors(NULL,SELFTEST_STRAT_BATTERY_LOW);
			break;
		case TEST_WHO_AM_I_1:
			prop_robot_id = QS_WHO_AM_I_get();
			act_robot_id = QS_WHO_AM_I_get();
			if(entrance)
			{
				//TODO msg can pour demander à la prop et à l'act qui elle sont
				t500ms = 2;
			}
			//TODO traiter les messages can entrants...
			if(!t500ms)
				state = TEST_WHO_AM_I_2;

			break;
		case TEST_WHO_AM_I_2:
			if(prop_robot_id != QS_WHO_AM_I_get() || act_robot_id != QS_WHO_AM_I_get())
				SELFTEST_declare_errors(NULL,SELFTEST_STRAT_WHO_AM_I_ARE_NOT_THE_SAME);
			state = TEST_BIROUTE;
			break;
		case TEST_BIROUTE:
			if(!BIROUTE)
				SELFTEST_declare_errors(NULL,SELFTEST_STRAT_BIROUTE_FORGOTTEN);
			state = TEST_SD_CARD;
			break;
		case TEST_SD_CARD:
			nb_written = 0;
			if(f_open(&file, "test.txt", FA_WRITE | FA_CREATE_ALWAYS | FA_OPEN_ALWAYS) == FR_OK)
			{
				f_write(&file, "test_string", 11, (unsigned int *)&nb_written);
				f_close(&file);
			}
			if(nb_written != 11)
				SELFTEST_declare_errors(NULL,SELFTEST_STRAT_SD_WRITE_FAIL);
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
	return (Uint16)((measure * 3000)/4096);	//3000 [mV] correspond à 4096 [ADC]
}

void SELFTEST_print_errors(SELFTEST_error_code_e * tab_errors, Uint8 size)
{
	Uint8 i;
	for(i=0;i<size;i++)
	{
		if(errors[i] != SELFTEST_NO_ERROR)
		{
			debug_printf("Selftest error %d : ",errors[i]);
			switch(errors[i])
			{
				case SELFTEST_NOT_DONE:						debug_printf("NOT_DONE");					break;
				case SELFTEST_FAIL_UNKNOW_REASON:			debug_printf("FAIL_UNKNOW_REASON");		break;
				case SELFTEST_NO_POWER:						debug_printf("NO_POWER");					break;
				case SELFTEST_TIMEOUT:						debug_printf("TIMEOUT");					break;
				case SELFTEST_PROP_LEFT_MOTOR:				debug_printf("PROP_LEFT_MOTOR");			break;
				case SELFTEST_PROP_LEFT_ENCODER:			debug_printf("PROP_LEFT_ENCODER");			break;
				case SELFTEST_PROP_RIGHT_MOTOR:				debug_printf("PROP_RIGHT_MOTOR");			break;
				case SELFTEST_PROP_RIGHT_ENCODER:			debug_printf("PROP_RIGHT_ENCODER");		break;
				case SELFTEST_STRAT_BIROUTE_NOT_IN_PLACE:	debug_printf("STRAT_BIROUTE_NOT_IN_PLACE");break;
				default:									debug_printf("UNKNOW_ERROR_CODE %d",errors[i]); break;
			}
			debug_printf("\n");
		}
	}
}

//Flag qui permet de lancer la fonction SELFTEST_beacon_reask_periodic_sending
volatile bool_e flag_500ms = FALSE;

//Fonction qui leve le flag précédent appelée par le timer4: 250ms
void SELFTEST_process_1sec(){
	flag_500ms = TRUE;
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

