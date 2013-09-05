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

#define TIMEOUT_SELFTEST_ACT 		20000	// en ms
#define TIMEOUT_SELFTEST_PROP 		10000	// en ms
#define TIMEOUT_SELFTEST_BEACON_IR 	1000	// en ms
#define TIMEOUT_SELFTEST_BEACON_US 	1000	// en ms
#define MAX_ERRORS_NUMBER 20

volatile bool_e ask_launch_selftest = FALSE;

SELFTEST_error_code_e errors[MAX_ERRORS_NUMBER];
volatile Uint8 errors_index = 0;

volatile bool_e selftest_state = FALSE;

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
bool_e SELFTEST_read_answer(CAN_msg_t * msg)
{
	bool_e ret;
	Uint8 i;
	if(msg->size == 0)	//Tout va bien...
		ret = TRUE;
	else
	{
		for(i=0;i<msg->size;i++)
		{
			//Récupération des codes d'erreurs envoyés.
			errors[errors_index] = msg->data[i];
			errors_index++;
			if(errors_index >= MAX_ERRORS_NUMBER)
				errors_index = MAX_ERRORS_NUMBER - 1;
		}
		ret = FALSE;
	}
	return ret;
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
			if(!global.env.match_started)
			{
				if(ask_launch_selftest)
				{
					debug_printf("\r\n_________________________ SELFTEST __________________________\r\n\r\n");
					//lcd_printf_last_line("Selftest : launch\n");
					errors_index = 0;	//On REMET le compteur d'erreur à 0.
					state = SELFTEST_ACT;
					selftest_state = TRUE;	//On suppose que le selftest sera réussi. (a la moindre erreur, on le remettra à FALSE).
					flag_timeout = FALSE;
				}
			}	
			break;
		case SELFTEST_ACT:
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
					selftest_state &= SELFTEST_read_answer(CAN_msg_received);
					WATCHDOG_stop(watchdog_id);
					state = SELFTEST_PROP;
				}
			if(flag_timeout)	//Timeout
			{
				state = SELFTEST_PROP;
			}
			break;
		case SELFTEST_PROP:
			if(entrance)
			{
				CAN_send_sid(ACT_DO_SELFTEST);
				flag_timeout = FALSE;
				watchdog_id = WATCHDOG_create_flag(TIMEOUT_SELFTEST_PROP, (bool_e*) &(flag_timeout));
			}
			if(CAN_msg_received != NULL)
				if(CAN_msg_received->sid == STRAT_ACT_SELFTEST_DONE)
				{
					//Retour de la carte Propulsion
					selftest_state &= SELFTEST_read_answer(CAN_msg_received);
					WATCHDOG_stop(watchdog_id);
					state = SELFTEST_BEACON_IR;
				}
			if(flag_timeout)	//Timeout
			{
				state = SELFTEST_BEACON_IR;
			}
			break;
		case SELFTEST_BEACON_IR:
			if(entrance)
			{
				CAN_send_sid(ACT_DO_SELFTEST);
				flag_timeout = FALSE;
				watchdog_id = WATCHDOG_create_flag(TIMEOUT_SELFTEST_BEACON_IR, (bool_e*) &(flag_timeout));
			}
			if(CAN_msg_received != NULL)
				if(CAN_msg_received->sid == STRAT_ACT_SELFTEST_DONE)
				{
					//Retour de la carte Beacon IR
					selftest_state &= SELFTEST_read_answer(CAN_msg_received);
					WATCHDOG_stop(watchdog_id);
					state = SELFTEST_BEACON_US;
				}
			if(flag_timeout)	//Timeout
			{
				state = SELFTEST_BEACON_US;
			}
			break;
		case SELFTEST_BEACON_US:
			if(entrance)
			{
				CAN_send_sid(ACT_DO_SELFTEST);
				flag_timeout = FALSE;
				watchdog_id = WATCHDOG_create_flag(TIMEOUT_SELFTEST_BEACON_US, (bool_e*) &(flag_timeout));
			}
			if(CAN_msg_received != NULL)
				if(CAN_msg_received->sid == STRAT_ACT_SELFTEST_DONE)
				{
					//Retour de la carte Beacon US
					selftest_state &= SELFTEST_read_answer(CAN_msg_received);
					WATCHDOG_stop(watchdog_id);
					state = SELFTEST_END;
				}
			if(flag_timeout)	//Timeout
			{
				state = SELFTEST_END;
			}
			break;
		case SELFTEST_END:
			ask_launch_selftest = FALSE;	//Fin du selftest (et du clignotement de la led selftest).
			if(selftest_state)//Tout s'est bien passé
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

void SELFTEST_print_errors(SELFTEST_error_code_e * tab_errors, Uint8 size)
{
	Uint8 i;
	for(i=0;i<size;i++)
	{
		if(tab_errors[i] != SELFTEST_NO_ERROR)
		{
			SD_printf("Selftest error %d : ",errors[i]);
			switch(errors[i])
			{
				case SELFTEST_NOT_DONE:						SD_printf("NOT_DONE");					break;
				case SELFTEST_FAIL_UNKNOW_REASON:			SD_printf("FAIL_UNKNOW_REASON");		break;
				case SELFTEST_NO_POWER:						SD_printf("NO_POWER");					break;
				case SELFTEST_TIMEOUT:						SD_printf("TIMEOUT");					break;
				case SELFTEST_PROP_LEFT_MOTOR:				SD_printf("PROP_LEFT_MOTOR");			break;
				case SELFTEST_PROP_LEFT_ENCODER:			SD_printf("PROP_LEFT_ENCODER");			break;
				case SELFTEST_PROP_RIGHT_MOTOR:				SD_printf("PROP_RIGHT_MOTOR");			break;
				case SELFTEST_PROP_RIGHT_ENCODER:			SD_printf("PROP_RIGHT_ENCODER");		break;
				case SELFTEST_STRAT_BIROUTE_NOT_IN_PLACE:	SD_printf("STRAT_BIROUTE_NOT_IN_PLACE");break;
				default:									SD_printf("UNKNOW_ERROR_CODE");			break;
			}
			SD_printf("\n");
		}
	}
}

//Flag qui permet de lancer la fonction SELFTEST_balise_update
volatile bool_e flag_1s = FALSE;

//Fonction qui leve le flag précédent appelée par le timer4: 250ms
void SELFTEST_process_1sec(){
	flag_1s = TRUE;
}


void led_us_update(selftest_beacon_e state){
		switch(state)
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
		}
}

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


void SELFTEST_balise_update(){
	static Uint8 counter = 8;
	
	if(flag_1s)
	{
		if(ask_launch_selftest)
			LED_SELFTEST = !LED_SELFTEST;	//clignotement de la led pendant le selftest.
		if(global.env.match_started == FALSE)
		{
			counter--;
			if(counter == 0)
			{
				counter = 200;
				//TODO : CAN_send_sid(BEACON_ENABLE_PERIODIC_SENDING);
			}
		}
		flag_1s = FALSE;
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

			/*
			if(beacon_error_report.beacon_error_ir_counter[msg->data[0]] < 0XFF)
				beacon_error_report.beacon_error_ir_counter[msg->data[0]]++;
			if(beacon_error_report.beacon_error_ir_counter[msg->data[4]] < 0XFF)
				beacon_error_report.beacon_error_ir_counter[msg->data[4]]++;
			 */
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


			/*
			if(beacon_error_report.beacon_error_us_counter[msg->data[0]] < 0xFF)
				beacon_error_report.beacon_error_us_counter[msg->data[0]]++;
			if(beacon_error_report.beacon_error_us_counter[msg->data[4]] < 0xFF)
				beacon_error_report.beacon_error_us_counter[msg->data[4]]++;*/
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
