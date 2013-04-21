/*
 *	Club Robot ESEO 2010 - 2011
 *	Check Norris
 *
 *	Fichier : Can_msg_processing.c
 *	Package : Carte actionneur
 *	Description : Fonctions de traitement des messages CAN
 *  Auteur : Aur�lien
 *  Version 20110225
 */


#define CAN_MSG_PROCESSING_C

#include "Can_msg_processing.h"
#include "QS/QS_DCMotor2.h"
#include "QS/QS_can.h"
#include "QS/QS_CANmsgList.h"
#include "queue.h"
#include "output_log.h"

#include "Krusty/KActManager.h"
#include "Tiny/TActManager.h"

#define LOG_PREFIX "CANProcess: "
#define COMPONENT_log(log_level, format, ...) OUTPUTLOG_printf(OUTPUT_LOG_COMPONENT_CANPROCESSMSG, log_level, LOG_PREFIX format, ## __VA_ARGS__)

void CAN_process_msg(CAN_msg_t* msg) {
	if(ACTMGR_process_msg(msg)) {
		COMPONENT_log(LOG_LEVEL_Debug, "Act Msg SID: 0x%x, cmd: 0x%x(%u)\n", msg->sid, msg->data[0], msg->data[0]);
		return;  //Le message a d�ja �t� g�r�
	}

	
	// Traitement des autres messages re�us
	switch (msg->sid)
	{
		//Fin de la partie
		case BROADCAST_STOP_ALL :
			global.match_started = FALSE;
			COMPONENT_log(LOG_LEVEL_Info, "C:BROADCAST_STOP_ALL\n");
			QUEUE_flush_all();
			ACTMGR_stop();
			DCM_stop_all();
			break;
		
		//Reprise de la partie
		case BROADCAST_START :
			COMPONENT_log(LOG_LEVEL_Info, "C:BROADCAST_START\n");
			global.match_started = TRUE;
			break;

		case BROADCAST_POSITION_ROBOT:
			//Rien, mais pas inclus dans le cas default o� l'on peut afficher le sid...
			break;
		
		default: 
			//debug_printf("SID:%x\r\n",msg->sid);
			COMPONENT_log(LOG_LEVEL_Trace, "Msg SID: 0x%x(%u)\n", msg->sid, msg->sid);
			break;
	}//End switch
}

//CAN is not enabled, declare functions so compilation won't fail
//FIXME: should be in QS_can.c ...
#ifndef USE_CAN
void CAN_send(CAN_msg_t* msg) {}

CAN_msg_t CAN_get_next_msg() {
	return ((CAN_msg_t){0});
}

bool_e CAN_data_ready() {
	return FALSE;
}

void CAN_set_direct_treatment_function(direct_treatment_function_pt func) {}
#endif
