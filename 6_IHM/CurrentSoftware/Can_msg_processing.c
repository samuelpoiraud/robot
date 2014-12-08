/*
 *	Club Robot ESEO 2014
 *
 *	Fichier : Can_msg_processing.c
 *	Package : Carte IHM
 *	Description : Fonctions de traitement des messages CAN
 *  Auteur : Anthony
 *  Version 20110225
 */


#define CAN_MSG_PROCESSING_C

#include "Can_msg_processing.h"
#include "QS/QS_can.h"
#include "QS/QS_CANmsgList.h"
#include "../stm32f4xx/stm32f4xx_gpio.h"

#define LOG_PREFIX "CANProcess: "
#define LOG_COMPONENT OUTPUT_LOG_COMPONENT_CANPROCESSMSG
#include "QS/QS_outputlog.h"
#include "switch.h"
#include "led.h"

void CAN_process_msg(CAN_msg_t* msg) {
	//CAN_msg_t answer;

	// Traitement des autres messages reçus
	switch (msg->sid){
		case BROADCAST_RESET:
			NVIC_SystemReset();
			break;
		case IHM_GET_SWITCH:
			debug_printf("Msg SID: IHM_GET_SWITCH\n");

			if(msg->size == 0)
				SWITCHS_send_all();
			else
				SWITCHS_answer(msg);
			break;
		case IHM_SET_LED:
			debug_printf("Msg SID: IHM_SET_LED\n");
			LEDS_get_msg(msg);
			break;

		case IHM_LCD_BIT_RESET:
			GPIO_WriteBit(LCD_RESET_PORT, msg->data[0]);
			break;

		default:
			debug_printf("Msg SID: 0x%03x(%u)\n", msg->sid, msg->sid);
			break;
	}//End switch
}
