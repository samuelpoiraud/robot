/*
 *	Club Robot ESEO 2010 - 2011
 *	Chek'Norris
 *
 *	Fichier : Selftest.c
 *	Package : Supervision
 *	Description : Envoi et r�ception des messages de selftest.
 *	Auteur : Ronan & Patman
 *	Version 20110120
 */


#include "Selftest.h"
#include "../QS/QS_outputlog.h"
#include "../QS/QS_CANmsgList.h"
#include "../QS/QS_uart.h"
#include "../QS/QS_watchdog.h"
#include "../QS/QS_can_over_uart.h"
#include "../QS/QS_adc.h"
#include "../QS/QS_who_am_i.h"
#include "../QS/QS_can_over_xbee.h"
#include "../QS/QS_IHM.h"
#include "../QS/QS_IHM.h"
#include "../QS/QS_ports.h"
#include "../QS/QS_mosfet.h"
#include "../elements.h"
#include "../act_functions.h"
#include "../detection.h"
#include "SD/Libraries/fat_sd/ff.h"
#include "SD/SD.h"
#include "RTC.h"
#include "Buzzer.h"
#include "LCD_interface.h"
#include "Synchro_balises.h"

#define TIMEOUT_SELFTEST_ACT			20000	// en ms
#define TIMEOUT_SELFTEST_PROP			10000	// en ms
#define TIMEOUT_SELFTEST_STRAT			10000	// en ms
#define TIMEOUT_SELFTEST_BEACON_IR		1000	// en ms
#define TIMEOUT_SELFTEST_AVOIDANCE		5000	// en ms
#define TIMEOUT_SELFTEST_IHM			1000	// en ms
#define MAX_ERRORS_NUMBER				200

#define THRESHOLD_BATTERY_OFF	18000	//[mV] En dessous cette valeur, on consid�re que la puissance est absente
#define THRESHOLD_BATTERY_LOW	21300	//[mV] R�glage du seuil de batterie faible
#define NB_AVERAGED_VALUE		50
#define REFRESH_DISPLAY_BAT		500
#define TIME_TO_REFRESH_BAT		1000
#define TIME_TO_TAKE_VALUE		(TIME_TO_REFRESH_BAT/NB_AVERAGED_VALUE)

#define LED_ON	Bit_SET
#define LED_OFF	Bit_RESET

#define FORWARD 0x01
#define	REAR 0x10

#define TEMPS_SYNCHRO 10
#define NOMBRE_TESTS_BALISE 2

volatile bool_e ask_launch_selftest = FALSE;
volatile bool_e selftest_is_running = FALSE;
volatile bool_e selftest_is_over = FALSE;
volatile SELFTEST_error_code_e errors[MAX_ERRORS_NUMBER];
volatile Uint8 errors_index = 0;
volatile Uint8 t500ms = 0;	//Minuteur [500ms]
static bool_e warning_bat = FALSE;
static Uint16 hokuyo_lost_counter = 0;


error_e SELFTEST_strategy(bool_e reset);

void SELFTEST_print_errors(SELFTEST_error_code_e * tab_errors, Uint8 size);
Uint16 SELFTEST_measure24_mV(void);
static void SELFTEST_check_alim();
static void SELFTEST_check_hokuyo();


void SELFTEST_init(void)
{
	Uint8 i;
	errors[0] = SELFTEST_NOT_DONE;	//Cette erreur initiale sera �cras�e, puisque l'index est � 0.
	for(i=1; i<MAX_ERRORS_NUMBER; i++)
	{
		errors[i] = SELFTEST_NO_ERROR;
	}
	errors_index = 0;
}

void SELFTEST_ask_launch(void)
{
	//On accepte une demande de selftest seulement avant ou apr�s le match. pas pendant.
	if((!global.flags.match_started || global.flags.match_over))
	{
		ask_launch_selftest = TRUE;
		selftest_is_running = FALSE;
		selftest_is_over = FALSE;
	}
}



//Fonction priv�e qui scrute le contenu des messages CAN de r�ponse au selftest.
void SELFTEST_declare_errors(CAN_msg_t * msg, SELFTEST_error_code_e error)
{
	Uint8 i;
	if(error != SELFTEST_NO_ERROR)
	{
		errors[errors_index] = error;
		if(errors_index < MAX_ERRORS_NUMBER - 1)
			errors_index++;
	}
	if(msg != NULL && msg->size != 0)	//L'ordre des arguments est important (pour ne pas acc�der � msg si ce pointeur est NULL)
	{
		for(i=0;i<msg->size;i++)
		{
			if(msg->data.strat_prop_selftest_done.error_code[i] != SELFTEST_NO_ERROR)
			{
				//R�cup�ration des codes d'erreurs envoy�s.
				errors[errors_index] = msg->data.strat_prop_selftest_done.error_code[i];
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
		toggle_led(LED_SELFTEST);	//On fait clignoter la led selftest � 2hz pendant le selftest.
	if(t500ms)
		t500ms--;
}

void SELFTEST_process_main(void)
{
	SELFTEST_update(NULL);
	SELFTEST_check_alim();
	SELFTEST_check_hokuyo();
}

//Machine a �tat du selftest, doit �tre appel�e : dans le process de t�che de fond ET � chaque r�ception d'un message can de selftest.
void SELFTEST_update(CAN_msg_t* CAN_msg_received)
{
	static watchdog_id_t watchdog_id;
	static bool_e flag_timeout;
	static bool_e act_ping_ok = FALSE;
	static bool_e prop_ping_ok = FALSE;
	static bool_e beacon_ping_ok = FALSE;
	static bool_e ihm_ping_ok = FALSE;

	typedef enum
	{
		INIT = 0,
		WAIT_SELFTEST_LAUNCH,
		SELFTEST_PING_ACT_PROP,
		SELFTEST_ACT,
		SELFTEST_PROP,
		SELFTEST_IHM,
		SELFTEST_BEACON_IR,
		SELFTEST_STRAT,
		SELFTEST_BEACON_BATTERY,
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
			GPIO_ResetBits(LED_SELFTEST);
			state = WAIT_SELFTEST_LAUNCH;
		break;
		case WAIT_SELFTEST_LAUNCH:
			if(ask_launch_selftest)	//demande de lancement du selftest (PAS PENDANT LE MATCH !)
			{
				debug_printf("\r\n_________________________ SELFTEST __________________________\r\n\r\n");
				errors_index = 0;	//On REMET le compteur d'erreur � 0.
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
				SELFTEST_strategy(TRUE);	//Reset de la sous-machine a �tats
				flag_timeout = FALSE;
				watchdog_id = WATCHDOG_create_flag(TIMEOUT_SELFTEST_STRAT, (bool_e*) &(flag_timeout));
				debug_printf("SELFTEST STRATEGY\r\n");
			}
			if(SELFTEST_strategy(FALSE) != IN_PROGRESS)	//La fonction SELFTEST_strategy d�clare elle-m�me ses erreurs.
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
				CAN_send_sid(IHM_PING);
				debug_printf("SELFTEST PING OTHER BOARDS\r\n");
			}
			if(CAN_msg_received != NULL)
			{
				if(CAN_msg_received->sid == STRAT_ACT_PONG)
				{
					if(CAN_msg_received->data.strat_act_pong.robot_id != QS_WHO_AM_I_get())
						SELFTEST_declare_errors(NULL,SELFTEST_STRAT_WHO_AM_I_ARE_NOT_THE_SAME);
					act_ping_ok = TRUE;
				}
				if(CAN_msg_received->sid == STRAT_PROP_PONG)
				{
					if(CAN_msg_received->data.strat_prop_pong.robot_id != QS_WHO_AM_I_get())
						SELFTEST_declare_errors(NULL,SELFTEST_STRAT_WHO_AM_I_ARE_NOT_THE_SAME);
					prop_ping_ok = TRUE;
				}
				if(CAN_msg_received->sid == STRAT_IHM_PONG)
				{
					if(CAN_msg_received->data.strat_ihm_pong.robot_id != QS_WHO_AM_I_get())
						SELFTEST_declare_errors(NULL,SELFTEST_STRAT_WHO_AM_I_ARE_NOT_THE_SAME);
					ihm_ping_ok = TRUE;
				}
				if(CAN_msg_received->sid == STRAT_BEACON_PONG)
					beacon_ping_ok = TRUE;

			}
			if(act_ping_ok == TRUE && prop_ping_ok == TRUE && beacon_ping_ok == TRUE && ihm_ping_ok == TRUE)
			{
				if(!flag_timeout)
					WATCHDOG_stop(watchdog_id);
				state = SELFTEST_ACT;
			}
			if(flag_timeout)
				state = SELFTEST_ACT;


			if(state == SELFTEST_ACT)	//D�claration des erreurs � la sortie de l'�tat !
			{
				if(!act_ping_ok)
					SELFTEST_declare_errors(CAN_msg_received, SELFTEST_ACT_UNREACHABLE);
				if(!prop_ping_ok)
					SELFTEST_declare_errors(CAN_msg_received, SELFTEST_PROP_UNREACHABLE);
				if(!beacon_ping_ok)
					SELFTEST_declare_errors(CAN_msg_received, SELFTEST_BEACON_UNREACHABLE);
				if(!ihm_ping_ok){
					SELFTEST_declare_errors(CAN_msg_received, SELFTEST_IHM_UNREACHABLE);
				}
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
					if(!flag_timeout)
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
					state = SELFTEST_IHM;

				// Lever erreur si switch_asser n'est actif
				if(IHM_switchs_get(SWITCH18_DISABLE_ASSER))
					SELFTEST_declare_errors(NULL,SELFTEST_PROP_SWITCH_ASSER_DISABLE);
			}
			if(CAN_msg_received != NULL)
				if(CAN_msg_received->sid == STRAT_PROP_SELFTEST_DONE)
				{
					//Retour de la carte Propulsion
					SELFTEST_declare_errors(CAN_msg_received, SELFTEST_NO_ERROR);
					if(!flag_timeout)
						WATCHDOG_stop(watchdog_id);
					state = SELFTEST_IHM;
				}
			if(flag_timeout)	//Timeout
			{
				debug_printf("SELFTEST PROP TIMEOUT\r\n");
				state = SELFTEST_IHM;
			}
			break;

		case SELFTEST_IHM:
			if(entrance)
			{
				flag_timeout = FALSE;
				if(ihm_ping_ok)
				{
					CAN_send_sid(IHM_DO_SELFTEST);
					watchdog_id = WATCHDOG_create_flag(TIMEOUT_SELFTEST_IHM, (bool_e*) &(flag_timeout));
					debug_printf("SELFTEST IHM\r\n");
				}
				else
					state = SELFTEST_BEACON_IR;
			}

			if(CAN_msg_received != NULL)
				if(CAN_msg_received->sid == STRAT_IHM_SELFTEST_DONE)
				{
					//Retour de la carte IHM
					SELFTEST_declare_errors(CAN_msg_received, SELFTEST_NO_ERROR);
					if(!flag_timeout)
						WATCHDOG_stop(watchdog_id);
					state = SELFTEST_BEACON_IR;
				}

			if(flag_timeout)	//Timeout
			{
				debug_printf("SELFTEST IHM TIMEOUT\r\n");
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
					state = SELFTEST_BEACON_BATTERY;
			}
			if(CAN_msg_received != NULL)
				if(CAN_msg_received->sid == STRAT_BEACON_SELFTEST_DONE)
				{
					//Retour de la carte Beacon IR
					SELFTEST_declare_errors(CAN_msg_received, SELFTEST_NO_ERROR);
					if(!flag_timeout)
						WATCHDOG_stop(watchdog_id);
					state = SELFTEST_BEACON_BATTERY;
				}
			if(flag_timeout)	//Timeout
			{
				debug_printf("SELFTEST BEACON TIMEOUT\r\n");
				state = SELFTEST_BEACON_BATTERY;
			}
			break;

		case SELFTEST_BEACON_BATTERY:
			if(entrance)
				debug_printf("SELFTEST BEACON ADV\r\n");

			/*if(get_warner_foe1_is_rf_unreacheable())
				SELFTEST_declare_errors(NULL,SELFTEST_BEACON_ADV1_RF_UNREACHABLE);

			if(get_warner_foe2_is_rf_unreacheable())
				SELFTEST_declare_errors(NULL,SELFTEST_BEACON_ADV2_RF_UNREACHABLE);

			if(get_warner_low_battery_on_foe1())
				SELFTEST_declare_errors(NULL,SELFTEST_BEACON_ADV1_BATTERY_LOW);

			if(get_warner_low_battery_on_foe2())
				SELFTEST_declare_errors(NULL,SELFTEST_BEACON_ADV2_BATTERY_LOW);*/

			state = SELFTEST_END;

			break;

		case SELFTEST_END:
			selftest_is_running = FALSE;
			selftest_is_over = TRUE;
			ask_launch_selftest = FALSE;	//Fin du selftest (et du clignotement de la led selftest).
			if(errors_index == 0)//Tout s'est bien pass�
			{
				GPIO_SetBits(LED_SELFTEST);
				debug_printf("SELFTEST : OK ! CHAMPOMY LES GARS !\n");
			}
			else	//Des probl�mes ont �t� rencontr�s
			{
				GPIO_ResetBits(LED_SELFTEST);
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
		TEST_AVOIDANCE_SW,
		TEST_XBEE,
		TEST_RTC,
		TEST_SWITCHS,
		TEST_STRAT_MOSFETS,
		TEST_ACT_MOSFETS,
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
			GPIO_WriteBit(LED_SELFTEST, (t500ms&1)); //Si t est impair, on allume toutes les leds.
			GPIO_WriteBit(LED_ERROR, (t500ms&1));
			GPIO_WriteBit(LED_CAN, (t500ms&1));
			GPIO_WriteBit(LED_UART, (t500ms&1));
			GPIO_WriteBit(LED_RUN, (t500ms&1));
			GPIO_WriteBit(LED_USER, (t500ms&1));

			if(!t500ms)	//Lorsque T vaut 0 (et que les leds sont �teintes...)
				state = TEST_AVOIDANCE_SW;
			break;

		case TEST_AVOIDANCE_SW:
			if(IHM_switchs_get(SWITCH_EVIT) == FALSE)
				SELFTEST_declare_errors(NULL,SELFTEST_STRAT_AVOIDANCE_SWITCH_DISABLE);
			state = TEST_XBEE;
			break;
		case TEST_XBEE:
			if(IHM_switchs_get(SWITCH_XBEE) == FALSE)
				SELFTEST_declare_errors(NULL,SELFTEST_STRAT_XBEE_SWITCH_DISABLE);
			else
			{
				if(XBee_is_destination_reachable() == FALSE)
					SELFTEST_declare_errors(NULL,SELFTEST_STRAT_XBEE_DESTINATION_UNREACHABLE);
			}
			state = TEST_RTC;
			break;
		case TEST_RTC:
			status = RTC_get_local_time (&date);
			if(!status)
				SELFTEST_declare_errors(NULL,SELFTEST_STRAT_RTC);
			state = TEST_SWITCHS;
			break;

		case TEST_SWITCHS:
			if(QS_WHO_AM_I_get()==BIG_ROBOT){
				if(IHM_switchs_get(SWITCH_DISABLE_DUNE))		SELFTEST_declare_errors(NULL, SELFTEST_STRAT_SWITCH_DISABLE_DUNE);
				if(IHM_switchs_get(SWITCH_DISABLE_FISHS))		SELFTEST_declare_errors(NULL, SELFTEST_STRAT_SWITCH_DISABLE_FISHS);
				if(IHM_switchs_get(SWITCH_DISABLE_DUNIX))		SELFTEST_declare_errors(NULL, SELFTEST_STRAT_SWITCH_DISABLE_DUNIX);
			}
			if(IHM_switchs_get(SWITCH_DISABLE_SAND_BLOC))	SELFTEST_declare_errors(NULL, SELFTEST_STRAT_SWITCH_DISABLE_SAND_BLOC);
			state = TEST_STRAT_MOSFETS;
			break;

		case TEST_STRAT_MOSFETS:
			if(I_AM_BIG()){
				if(MOSFET_selftest_strat())
					state = TEST_ACT_MOSFETS;
			}else{
				state = TEST_ACT_MOSFETS;
			}
			break;

		case TEST_ACT_MOSFETS:
			if(I_AM_BIG()){
				if(MOSFET_selftest_act(8))  //huit mosfets � tester
					state = TEST_SD_CARD;
			}else{
				if(MOSFET_selftest_act(2))  //deux mosfets � tester
					state = TEST_SD_CARD;
			}
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
	return (Uint16)((measure * 3000*110/10)/1024);	//3000 [mV] correspond � 4096 [ADC]
}

void SELFTEST_print_errors(SELFTEST_error_code_e * tab_errors, Uint8 size)
{
	Uint8 i;
	debug_printf("SELFTEST ENDED with %d error(s) :\n",size);
	BUZZER_play(200, DEFAULT_NOTE, size);	//Autant de bip que d'erreurs !
	for(i=0;i<size;i++)
	{
		if(errors[i] != SELFTEST_NO_ERROR)
		{
			debug_printf("\terror %3d : ",errors[i]);
			switch(errors[i])
			{
				case SELFTEST_NOT_DONE:							debug_printf("NOT_DONE");										break;
				case SELFTEST_FAIL_UNKNOW_REASON:				debug_printf("FAIL_UNKNOW_REASON");								break;
				case SELFTEST_TIMEOUT:							debug_printf("TIMEOUT");										break;

				case SELFTEST_BEACON_ADV1_NOT_SEEN:				debug_printf("SELFTEST_BEACON_ADV1_NOT_SEEN");					break;
				case SELFTEST_BEACON_ADV2_NOT_SEEN:				debug_printf("SELFTEST_BEACON_ADV2_NOT_SEEN");					break;
				case SELFTEST_BEACON_SYNCHRO_NOT_RECEIVED:		debug_printf("SELFTEST_BEACON_SYNCHRO_NOT_RECEIVED");			break;
				case SELFTEST_BEACON_ADV1_BATTERY_LOW:			debug_printf("SELFTEST_BEACON_ADV1_BATTERY_LOW");				break;
				case SELFTEST_BEACON_ADV2_BATTERY_LOW:			debug_printf("SELFTEST_BEACON_ADV2_BATTERY_LOW");				break;
				case SELFTEST_BEACON_ADV1_RF_UNREACHABLE:		debug_printf("SELFTEST_BEACON_ADV1_RF_UNREACHABLE");			break;
				case SELFTEST_BEACON_ADV2_RF_UNREACHABLE:		debug_printf("SELFTEST_BEACON_ADV2_RF_UNREACHABLE");			break;

				case SELFTEST_PROP_FAILED:						debug_printf("PROP_FAILED");									break;
				case SELFTEST_PROP_HOKUYO_FAILED:				debug_printf("SELFTEST_PROP_HOKUYO_FAILED");					break;
				case SELFTEST_PROP_IN_SIMULATION_MODE:			debug_printf("SELFTEST_PROP_IN_SIMULATION_MODE");				break;
				case SELFTEST_PROP_SWITCH_ASSER_DISABLE:		debug_printf("SELFTEST_PROP_SWITCH_ASSER_DISABLE");				break;
				case SELFTEST_PROP_LASER_SENSOR_LEFT:		    debug_printf("SELFTEST_PROP_LASER_SENSOR_LEFT");				break;
				case SELFTEST_PROP_LASER_SENSOR_RIGHT:		    debug_printf("SELFTEST_PROP_LASER_SENSOR_RIGHT");			    break;

				case SELFTEST_STRAT_AVOIDANCE_SWITCH_DISABLE:	debug_printf("SELFTEST_STRAT_AVOIDANCE_SWITCH_DISABLE");		break;
				case SELFTEST_STRAT_XBEE_SWITCH_DISABLE:		debug_printf("SELFTEST_STRAT_XBEE_SWITCH_DISABLE");				break;
				case SELFTEST_STRAT_XBEE_DESTINATION_UNREACHABLE:debug_printf("SELFTEST_STRAT_XBEE_DESTINATION_UNREACHABLE");	break;
				case SELFTEST_STRAT_RTC:						debug_printf("SELFTEST_STRAT_RTC");								break;
				case SELFTEST_STRAT_BATTERY_NO_24V:				debug_printf("SELFTEST_STRAT_BATTERY_NO_24V");					break;
				case SELFTEST_STRAT_BATTERY_LOW:				debug_printf("SELFTEST_STRAT_BATTERY_LOW");						break;
				case SELFTEST_STRAT_WHO_AM_I_ARE_NOT_THE_SAME:	debug_printf("SELFTEST_STRAT_WHO_AM_I_ARE_NOT_THE_SAME");		break;
				case SELFTEST_STRAT_BIROUTE_FORGOTTEN:			debug_printf("SELFTEST_STRAT_BIROUTE_FORGOTTEN");				break;
				case SELFTEST_STRAT_SD_WRITE_FAIL:				debug_printf("SELFTEST_STRAT_SD_WRITE_FAIL");					break;
				case SELFTEST_STRAT_SWITCH_DISABLE_DUNE:		debug_printf("SELFTEST_STRAT_SWITCH_DISABLE_DUNE");			    break;
				case SELFTEST_STRAT_SWITCH_DISABLE_SAND_BLOC:	debug_printf("SELFTEST_STRAT_SWITCH_DISABLE_SAND_BLOC");		break;
				case SELFTEST_STRAT_SWITCH_DISABLE_FISHS:	    debug_printf("SELFTEST_STRAT_SWITCH_DISABLE_FISHS");		    break;
				case SELFTEST_STRAT_SWITCH_DISABLE_DUNIX:		debug_printf("SELFTEST_STRAT_SWITCH_DISABLE_DUNIX");			break;

				case SELFTEST_IHM_BATTERY_NO_24V:				debug_printf("SELFTEST_IHM_BATTERY_NO_24V");					break;
				case SELFTEST_IHM_BATTERY_LOW:					debug_printf("SELFTEST_IHM_BATTERY_LOW");						break;
				case SELFTEST_IHM_BIROUTE_FORGOTTEN:			debug_printf("SELFTEST_IHM_BIROUTE_FORGOTTEN");					break;
				case SELFTEST_IHM_POWER_HOKUYO_FAILED:			debug_printf("SELFTEST_IHM_POWER_HOKUYO_FAILED");				break;

				case SELFTEST_ACT_UNREACHABLE:					debug_printf("SELFTEST_ACT_UNREACHABLE");						break;
				case SELFTEST_PROP_UNREACHABLE:					debug_printf("SELFTEST_PROP_UNREACHABLE");						break;
				case SELFTEST_BEACON_UNREACHABLE:				debug_printf("SELFTEST_BEACON_UNREACHABLE");					break;
				case SELFTEST_IHM_UNREACHABLE:					debug_printf("SELFTEST_IHM_UNREACHABLE");						break;

				// Actionneurs
				case SELFTEST_ACT_RX24_FISH_MAGNETIC_ARM:       debug_printf("SELFTEST_ACT_RX24_FISH_MAGNETIC_ARM");            break;
				case SELFTEST_ACT_RX24_FISH_UNSTICK_ARM:		debug_printf("SELFTEST_ACT_RX24_FISH_UNSTICK_ARM");             break;
				case SELFTEST_ACT_RX24_BLACK_SAND_CIRCLE:		debug_printf("SELFTEST_ACT_RX24_BLACK_SAND_CIRCLE");            break;
				case SELFTEST_ACT_RX24_BOTTOM_DUNE:				debug_printf("SELFTEST_ACT_RX24_BOTTOM_DUNE");                  break;
				case SELFTEST_ACT_RX24_MIDDLE_DUNE:				debug_printf("SELFTEST_ACT_RX24_MIDDLE_DUNE");                  break;
				case SELFTEST_ACT_RX24_CONE_DUNE:				debug_printf("SELFTEST_ACT_RX24_CONE_DUNE");                    break;
				case SELFTEST_ACT_RX24_DUNIX_LEFT:				debug_printf("SELFTEST_ACT_RX24_DUNIX_LEFT");                   break;
				case SELFTEST_ACT_RX24_DUNIX_RIGHT:				debug_printf("SELFTEST_ACT_RX24_DUNIX_RIGHT");                  break;
				case SELFTEST_ACT_RX24_SAND_LOCKER_LEFT:		debug_printf("SELFTEST_ACT_RX24_SAND_LOCKER_LEFT");             break;
				case SELFTEST_ACT_RX24_SAND_LOCKER_RIGHT:		debug_printf("SELFTEST_ACT_RX24_SAND_LOCKER_RIGHT");            break;
				case SELFTEST_ACT_RX24_SHIFT_CYLINDER:			debug_printf("SELFTEST_ACT_RX24_SHIFT_CYLINDER");               break;
				case SELFTEST_ACT_RX24_PENDULUM:				debug_printf("SELFTEST_ACT_RX24_PENDULUM");                     break;

				case SELFTEST_ACT_AX12_LEFT_ARM:				debug_printf("SELFTEST_ACT_AX12_LEFT_ARM");                     break;
				case SELFTEST_ACT_AX12_RIGHT_ARM:				debug_printf("SELFTEST_ACT_AX12_RIGHT_ARM");                    break;
				case SELFTEST_ACT_AX12_SAND_CIRCLE:				debug_printf("SELFTEST_ACT_AX12_SAND_CIRCLE");                  break;
				case SELFTEST_ACT_AX12_PARASOL:					debug_printf("SELFTEST_ACT_AX12_PARASOL");                      break;
				case SELFTEST_ACT_POMPE_FRONT_LEFT:				debug_printf("SELFTEST_ACT_POMPE_FRONT_LEFT");                  break;
				case SELFTEST_ACT_POMPE_FRONT_RIGHT:			debug_printf("SELFTEST_ACT_POMPE_FRONT_RIGHT");                 break;

				case SELFTEST_ACT_MISSING_TEST:					debug_printf("SELFTEST_ACT_MISSING_TEST");						break;	//Test manquant apr�s un timeout du selftest actionneur, certains actionneur n'ont pas le selftest d'impl�ment� ou n'ont pas termin� leur action (ou plus rarement, la pile �tait pleine et le selftest n'a pas pu se faire)
				case SELFTEST_ACT_UNKNOWN_ACT:					debug_printf("SELFTEST_ACT_UNKNOWN_ACT");						break;	//Un actionneur inconnu a fail son selftest. Pour avoir le nom, ajoutez un SELFTEST_ACT_xxx ici et g�rez l'actionneur dans selftest.c de la carte actionneur
				break;


				default:										debug_printf("UNKNOW_ERROR_CODE");								break;
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

/*void led_ir_update(selftest_beacon_e state)
	{
		switch(state)
		{
			case BEACON_ERROR:
				GPIO_WriteBit(LED_BEACON_IR_GREEN, LED_OFF);
				toggle_led(LED_BEACON_IR_RED);
				break;
			case BEACON_NEAR:
				toggle_led(LED_BEACON_IR_GREEN);
				GPIO_WriteBit(LED_BEACON_IR_RED, GPIO_ReadOutputDataBit(LED_BEACON_IR_GREEN));
				break;
			case BEACON_FAR:
				GPIO_WriteBit(LED_BEACON_IR_RED, LED_OFF);
				toggle_led(LED_BEACON_IR_GREEN);
				break;
			default:
				GPIO_WriteBit(LED_BEACON_IR_RED, LED_ON);
				GPIO_WriteBit(LED_BEACON_IR_GREEN, LED_OFF);
				break;
		}
}*/






volatile static struct {
	Uint8 beacon_error_ir_counter[8];
	Uint16 report_counter;
}beacon_error_report = {{0}, 0};

void SELFTEST_beacon_counter_init(){
	Uint8 i;
	for(i=0;i<8;i++)
		beacon_error_report.beacon_error_ir_counter[i] = 0;
	beacon_error_report.report_counter = 0;
}

void error_counters_update(CAN_msg_t * msg){
	Uint8 i;
	switch(msg->sid){
		case BROADCAST_BEACON_ADVERSARY_POSITION_IR:

			for(i=0;i<2;i++){
				if(msg->data.broadcast_beacon_adversary_position_ir.adv[i].error == AUCUNE_ERREUR)
					beacon_error_report.beacon_error_ir_counter[0]++;
				if(msg->data.broadcast_beacon_adversary_position_ir.adv[i].error & AUCUN_SIGNAL)
					beacon_error_report.beacon_error_ir_counter[1]++;
				if(msg->data.broadcast_beacon_adversary_position_ir.adv[i].error & SIGNAL_INSUFFISANT)
					beacon_error_report.beacon_error_ir_counter[2]++;
				if(msg->data.broadcast_beacon_adversary_position_ir.adv[i].error & TACHE_TROP_GRANDE)
					beacon_error_report.beacon_error_ir_counter[3]++;
				if(msg->data.broadcast_beacon_adversary_position_ir.adv[i].error & TROP_DE_SIGNAL)
					beacon_error_report.beacon_error_ir_counter[4]++;
				if(msg->data.broadcast_beacon_adversary_position_ir.adv[i].error & ERREUR_POSITION_INCOHERENTE)
					beacon_error_report.beacon_error_ir_counter[5]++;
				if(msg->data.broadcast_beacon_adversary_position_ir.adv[i].error & OBSOLESCENCE)
					beacon_error_report.beacon_error_ir_counter[6]++;
			}
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
		msg->data.ir_error_result.error_counter[i] = beacon_error_report.beacon_error_ir_counter[i];
}

/*void SELFTEST_update_led_beacon(CAN_msg_t * can_msg)
{
	switch(can_msg->sid)
	{
		case BROADCAST_BEACON_ADVERSARY_POSITION_IR:
			if(global.flags.match_started == TRUE)
				//Enregistrement du type d'erreur
				error_counters_update(can_msg);
			//Si le message d'erreur n'est pas nul autrement dit si il y a une erreur quelconque
			if(can_msg->data.broadcast_beacon_adversary_position_ir.adv[0].error || can_msg->data.broadcast_beacon_adversary_position_ir.adv[1].error)
				led_ir_update(BEACON_ERROR);
			else if(can_msg->data.broadcast_beacon_adversary_position_ir.adv[0].dist < 102 || can_msg->data.broadcast_beacon_adversary_position_ir.adv[1].dist < 102) //Distance IR en cm
				led_ir_update(BEACON_NEAR);
			else
				led_ir_update(BEACON_FAR);
			break;

		default:
			break;
	}
}*/


void SELFTEST_check_alim(){
	typedef enum{
		ALIM_Off = 0,
		ALIM_On
	}state_e;
	static state_e state = ALIM_Off;
	static Uint16 values[NB_AVERAGED_VALUE] = {0};
	static time32_t begin_time = 0;
	static time32_t last_display_time = 0;

	Uint8 i;
	Uint32 average = 0;
	CAN_msg_t msg;

	if(begin_time == 0)
		begin_time = global.absolute_time;

	if((int)((global.absolute_time-begin_time)/TIME_TO_TAKE_VALUE) < NB_AVERAGED_VALUE)
		values[(int)((global.absolute_time-begin_time)/TIME_TO_TAKE_VALUE)] = SELFTEST_measure24_mV();

	for(i=0;i<NB_AVERAGED_VALUE;i++)
		average += values[i];

	average /= NB_AVERAGED_VALUE;
	global.alim_value = average;

	if(global.absolute_time-begin_time >= TIME_TO_REFRESH_BAT){
		begin_time = global.absolute_time;
	}

	if(global.absolute_time-last_display_time >= REFRESH_DISPLAY_BAT){
		last_display_time = global.absolute_time;

		if(state == ALIM_On && global.alim_value < THRESHOLD_BATTERY_LOW && global.alim_value > THRESHOLD_BATTERY_OFF){
			BUZZER_play(40, DEFAULT_NOTE, 10);
			LCD_printf(3, TRUE, TRUE, "CHANGE BAT : %d", global.alim_value);
			warning_bat = TRUE;
		}

		if(!warning_bat)
			LCD_printf(3, FALSE, FALSE, "VBAT : %d  N�%d", global.alim_value, SD_get_match_id());
	}

	if(global.alim_value > THRESHOLD_BATTERY_OFF && state != ALIM_On){
		msg.sid = BROADCAST_ALIM;
		msg.size = SIZE_BROADCAST_ALIM;
		msg.data.broadcast_alim.state = TRUE;
		msg.data.broadcast_alim.value = global.alim_value;
		CAN_send(&msg);
		state = ALIM_On;
		global.flags.alim = TRUE;
	}else if(global.alim_value < THRESHOLD_BATTERY_OFF && state != ALIM_Off){
		msg.sid = BROADCAST_ALIM;
		msg.size = SIZE_BROADCAST_ALIM;
		msg.data.broadcast_alim.state = FALSE;
		msg.data.broadcast_alim.value = global.alim_value;
		CAN_send(&msg);
		state = ALIM_Off;
		global.flags.alim = FALSE;
	}

}

void SELFTEST_check_hokuyo(){
	static bool_e alarmed = FALSE;
	time32_t delta_time = DETECTION_get_last_time_since_hokuyo_date();
	CAN_msg_t msg;
	msg.sid = IHM_SET_ERROR;
	msg.size = SIZE_IHM_SET_ERROR;
	msg.data.ihm_set_error.error = IHM_ERROR_ASSER;

	if(delta_time > 250 && alarmed == FALSE){
		BUZZER_play(15, DEFAULT_NOTE, 5);
		LCD_printf(3, FALSE, TRUE, "HOKUYO LOST !");
		hokuyo_lost_counter++;
		msg.data.ihm_set_error.state = TRUE;
		CAN_send(&msg);
		alarmed = TRUE;
	}else if(delta_time <= 250 && alarmed == TRUE){
		LCD_printf(3, FALSE, TRUE, "HOKUYO IS ALIVE !");
		msg.data.ihm_set_error.state = FALSE;
		CAN_send(&msg);
		alarmed = FALSE;
	}
}

SELFTEST_error_code_e SELFTEST_getError(Uint8 index)
{
	assert(index < MAX_ERRORS_NUMBER);
	return errors[index];
}

char * SELFTEST_getError_string(SELFTEST_error_code_e error_num){
	static char default_string[20];

	switch(error_num){
		case SELFTEST_NOT_DONE:							return "Not done"; 				break;
		case SELFTEST_FAIL_UNKNOW_REASON:				return "Error 404"; 			break;
		case SELFTEST_TIMEOUT:							return "Selftest Timeout";		break;

		case SELFTEST_BEACON_ADV1_NOT_SEEN:				return "IR Adv1 not seen";		break;
		case SELFTEST_BEACON_ADV2_NOT_SEEN:				return "IR Adv2 not seen";		break;
		case SELFTEST_BEACON_SYNCHRO_NOT_RECEIVED:		return "IR not synchronized";	break;
		case SELFTEST_BEACON_ADV1_BATTERY_LOW:			return "Beacon 1 bat low";		break;
		case SELFTEST_BEACON_ADV2_BATTERY_LOW:			return "Beacon 2 bat low";		break;
		case SELFTEST_BEACON_ADV1_RF_UNREACHABLE:		return "Beacon 1 RF unreach";	break;
		case SELFTEST_BEACON_ADV2_RF_UNREACHABLE:		return "Beacon 2 RF unreach";	break;

		case SELFTEST_PROP_FAILED:						return "PROP failed";			break;
		case SELFTEST_PROP_HOKUYO_FAILED:				return "Hokuyo failed";			break;
		case SELFTEST_PROP_IN_SIMULATION_MODE:			return "PROP in simu mode";		break;
		case SELFTEST_PROP_SWITCH_ASSER_DISABLE:		return "Asser Switch disable"; 	break;
		case SELFTEST_PROP_LASER_SENSOR_LEFT:			return "Laser sensor left";		break;
		case SELFTEST_PROP_LASER_SENSOR_RIGHT:			return "Laser sensor right";	break;

		case SELFTEST_STRAT_AVOIDANCE_SWITCH_DISABLE:	return "Evit Switch disable";	break;
		case SELFTEST_STRAT_XBEE_SWITCH_DISABLE:		return "XBee Switch disable";	break;
		case SELFTEST_STRAT_XBEE_DESTINATION_UNREACHABLE: return "XBee dest unreach";	break;
		case SELFTEST_STRAT_RTC:						return "RTC failed";			break;
		case SELFTEST_STRAT_BATTERY_NO_24V:				return "NO 24V";				break;
		case SELFTEST_STRAT_BATTERY_LOW:				return "BATTERY LOW";			break;
		case SELFTEST_STRAT_WHO_AM_I_ARE_NOT_THE_SAME:	return "WhoAmI error";			break;
		case SELFTEST_STRAT_BIROUTE_FORGOTTEN:			return "Biroute Forgotten"; 	break;
		case SELFTEST_STRAT_SD_WRITE_FAIL:				return "SD Write FAIL";			break;
		case SELFTEST_STRAT_SWITCH_DISABLE_DUNE:		return "Dune Disabled";			break;
		case SELFTEST_STRAT_SWITCH_DISABLE_SAND_BLOC:	return "Sand bloc Disabled";	break;
		case SELFTEST_STRAT_SWITCH_DISABLE_FISHS:	    return "Fishs Disabled";		break;
		case SELFTEST_STRAT_SWITCH_DISABLE_DUNIX:		return "Dunix Disabled";		break;

		case SELFTEST_IHM_BATTERY_NO_24V:				return "NO 24V";				break;
		case SELFTEST_IHM_BATTERY_LOW:					return "BATTERY LOW";			break;
		case SELFTEST_IHM_BIROUTE_FORGOTTEN:			return "Biroute Forgotten";		break;
		case SELFTEST_IHM_POWER_HOKUYO_FAILED:			return "Hokuyo power failed";	break;

		case SELFTEST_ACT_UNREACHABLE:					return "ACT Unreachable";		break;
		case SELFTEST_PROP_UNREACHABLE:					return "PROP Unreachable";		break;
		case SELFTEST_BEACON_UNREACHABLE:				return "BEACON Unreachable";	break;
		case SELFTEST_IHM_UNREACHABLE:					return "IHM Unreachable";		break;

		case SELFTEST_ACT_MISSING_TEST:					return "ACT Missing test";		break;
		case SELFTEST_ACT_UNKNOWN_ACT:					return "ACT Unknown ACT";		break;


		case SELFTEST_ERROR_NB:							return NULL;					break;
		case SELFTEST_NO_ERROR:							return NULL;					break;
		default:
			sprintf(default_string, "0x%2x Unknown ERR", error_num);
			return default_string;
		break;
	}
	return default_string;	// pour �viter un warning.. meme si on arrive jamais ici !
}

bool_e SELFTEST_is_running(void)
{
	return selftest_is_running;
}

bool_e SELFTEST_is_over(void)
{
	return selftest_is_over;
}

Uint8 SELFTEST_get_errors_number(void)
{
	return errors_index;
}

bool_e get_warning_bat(){
	return warning_bat;
}

void clean_warning_bat(){
	warning_bat = FALSE;
}

void Selftest_print_sd_hokuyo_lost(void){
	SD_printf("Hokuyo lost count : %d\n", hokuyo_lost_counter);
}

