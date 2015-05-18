/*
 *	Club Robot ESEO 2014
 *
 *	Fichier : Can_msg_processing.c
 *	Package : Carte IHM
 *	Description : Fonctions de traitement des messages CAN
 *  Auteur : Anthony
 *  Version 20110225
 */


#include "Can_msg_processing.h"
#include "QS/QS_can.h"
#include "QS/QS_CANmsgList.h"
#include "QS/QS_can_verbose.h"
#include "QS/QS_who_am_i.h"
#include "../stm32f4xx/stm32f4xx_gpio.h"

#include "switch.h"
#include "led.h"
#include "voltage_measure.h"

static void CAN_send_callback(CAN_msg_t* msg);
static void CAN_send_pong(void);
static void CAN_send_selftest(void);

void CAN_process_init(){
	CAN_init();
	CAN_set_send_callback(CAN_send_callback);
}

void CAN_process_msg(CAN_msg_t* msg) {
	//CAN_msg_t answer;

	// Traitement des autres messages reçus
	switch (msg->sid){
		case BROADCAST_RESET:
			NVIC_SystemReset();
			break;

		case IHM_GET_SWITCH:
			if(msg->size == 0)
				SWITCHS_send_all();
			else
				SWITCHS_answer(msg);
			break;

		case IHM_SET_ERROR:
			LEDS_set_error(msg->data[0], msg->data[1]);
			break;

		case IHM_SET_LED:
			LEDS_get_msg(msg);
			break;

		case IHM_SET_LED_COLOR:
			LEDS_set_COLOR((led_color_e)msg->data[0]);
			break;

		case IHM_LCD_BIT_RESET:
			GPIO_WriteBit(LCD_RESET_PORT, msg->data[0]);
			break;

		case IHM_PING:
			CAN_send_pong();
			break;

		case IHM_DO_SELFTEST:
			CAN_send_selftest();
			break;

		default:
			break;
	}//End switch
}

static void CAN_send_callback(CAN_msg_t* msg){
	UNUSED_VAR(msg);
	#ifdef CAN_VERBOSE_MODE
		QS_CAN_VERBOSE_can_msg_print(msg, VERB_OUTPUT_MSG);
	#endif
}

static void CAN_send_pong(void){
	CAN_msg_t msg;
	msg.sid = STRAT_IHM_PONG;
	msg.size = 1;
	msg.data[0] = QS_WHO_AM_I_get();
	CAN_send(&msg);
}

static void CAN_send_selftest(void){
	Uint8 i = 0;
	CAN_msg_t msg;
	msg.sid = STRAT_IHM_SELFTEST_DONE;

	if(!SWITCHS_get(BIROUTE_IHM))
		msg.data[i++] = SELFTEST_IHM_BIROUTE_FORGOTTEN;

	if(!VOLTAGE_MEASURE_get_state_hokuyo())
		msg.data[i++] = SELFTEST_IHM_POWER_HOKUYO_FAILED;

	if(!VOLTAGE_MEASURE_get_state_battery())
		msg.data[i++] = SELFTEST_IHM_BATTERY_LOW;

	if(VOLTAGE_MEASURE_get_state_ARU())
		msg.data[i++] = SELFTEST_IHM_BATTERY_NO_24V;

	msg.data[i++] = SELFTEST_NO_ERROR;
	msg.size = i;
	CAN_send(&msg);
}
