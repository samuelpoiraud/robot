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
#include "../stm32f4xx/stm32f4xx_gpio.h"

#include "switch.h"
#include "led.h"

static void CAN_send_callback(CAN_msg_t* msg);

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
		case IHM_SET_LED:
			LEDS_get_msg(msg);
			break;

		case IHM_LCD_BIT_RESET:
			GPIO_WriteBit(LCD_RESET_PORT, msg->data[0]);
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

