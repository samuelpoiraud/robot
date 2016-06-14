/*
 *  Club Robot ESEO 2015
 *
 *  Fichier : it.c
 *  Package : Balise émettrice
 *  Description : Fonctions de traitement des messages CAN
 *  Auteur : Arnaud
 *  Version 200904
 */

#include "can_msg_processing.h"
#include "QS/QS_can.h"
#include "QS/QS_CANmsgList.h"

#define LOG_PREFIX "CANProcess: "
#define LOG_COMPONENT OUTPUT_LOG_COMPONENT_CANPROCESSMSG
#include "QS/QS_outputlog.h"


void CAN_process_msg(CAN_msg_t* msg) {

	// Traitement des messages reçus
	switch (msg->sid){

		default:
			component_printf(LOG_LEVEL_Trace, "Msg SID: 0x%03x(%u)\n", msg->sid, msg->sid);
			break;
	}
}
