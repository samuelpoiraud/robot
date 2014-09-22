/*
 *	Club Robot ESEO 2014
 *
 *	Fichier : Can_msg_processing.c
 *	Package : Carte actionneur
 *	Description : Fonctions de traitement des messages CAN
 *  Auteur : Anthony
 *  Version 20110225
 */


#define CAN_MSG_PROCESSING_C

#include "Can_msg_processing.h"
#include "QS/QS_can.h"
#include "QS/QS_CANmsgList.h"


#define LOG_PREFIX "CANProcess: "
#define LOG_COMPONENT OUTPUT_LOG_COMPONENT_CANPROCESSMSG
#include "QS/QS_outputlog.h"
#include "QS/QS_who_am_i.h"

void CAN_process_msg(CAN_msg_t* msg) {
	CAN_msg_t answer;

	// Traitement des autres messages reçus
	switch (msg->sid)
	{
		//Fin de la partie
		case BROADCAST_STOP_ALL :
			break;

		//Reprise de la partie
		case BROADCAST_START :
			break;

		case BROADCAST_POSITION_ROBOT:
			break;
		case BROADCAST_BEACON_ADVERSARY_POSITION_IR:
			break;
		case BROADCAST_ALIM:
			break;

		case ACT_PING:
//			answer.sid = STRAT_ACT_PONG;
//			answer.size = 1;
//			#ifdef I_AM_ROBOT_BIG
//				answer.data[0] = BIG_ROBOT;
//			#else
//				answer.data[0] = SMALL_ROBOT;
//			#endif
//			CAN_send(&answer);
			break;

		default:
			component_printf(LOG_LEVEL_Trace, "Msg SID: 0x%03x(%u)\n", msg->sid, msg->sid);
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
