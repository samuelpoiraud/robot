/*
 *	Club Robot ESEO 2010 - 2011
 *	Check Norris
 *
 *	Fichier : Can_msg_processing.c
 *	Package : Carte actionneur
 *	Description : Fonctions de traitement des messages CAN
 *  Auteur : Aurélien
 *  Version 20110225
 */


#define CAN_MSG_PROCESSING_C

#include "Can_msg_processing.h"
#include "QS/QS_DCMotor2.h"
#include "QS/QS_can.h"
#include "QS/QS_CANmsgList.h"
#include "queue.h"

#include "BIG/BActManager.h"
#include "SMALL/SActManager.h"

#include "config_debug.h"
#define LOG_PREFIX "CANProcess: "
#define LOG_COMPONENT OUTPUT_LOG_COMPONENT_CANPROCESSMSG
#include "QS/QS_outputlog.h"
#include "QS/QS_who_am_i.h"

void CAN_process_msg(CAN_msg_t* msg) {
	CAN_msg_t answer;
	if(ACTMGR_process_msg(msg)) {
		component_printf(LOG_LEVEL_Debug, "Act Msg SID: 0x%x, cmd: 0x%x(%u), size: %d\n", msg->sid, msg->data[0], msg->data[0], msg->size);
		return;  //Le message a déja été géré
	}


	// Traitement des autres messages reçus
	switch (msg->sid)
	{
		//Fin de la partie
		case BROADCAST_STOP_ALL :
			global.match_started = FALSE;
			component_printf(LOG_LEVEL_Info, "C:BROADCAST_STOP_ALL\n");
			QUEUE_flush_all();
			ACTMGR_stop();
			DCM_stop_all();
			break;

		//Reprise de la partie
		case BROADCAST_START :
			component_printf(LOG_LEVEL_Info, "C:BROADCAST_START\n");
			global.match_started = TRUE;
			break;

		case BROADCAST_POSITION_ROBOT:
			//Rien, mais pas inclus dans le cas default où l'on peut afficher le sid...
			break;

		case BROADCAST_ALIM:
			if(msg->data[0] == ALIM_OFF){
				component_printf(LOG_LEVEL_Info, "C:BROADCAST_ALIM -> ALIM_OFF\n");
				global.alim = FALSE;
			}else if(msg->data[0] == ALIM_ON){
				component_printf(LOG_LEVEL_Info, "C:BROADCAST_ALIM -> ALIM_ON\n");
				global.alim = TRUE;
			}
			global.alim_value = (((Uint16)(msg->data[1]) << 8) & 0xFF00) | ((Uint16)(msg->data[2]) & 0x00FF);
			break;

		case ACT_PING:
			answer.sid = STRAT_ACT_PONG;
			answer.size = 1;
			#ifdef I_AM_ROBOT_BIG
				answer.data[0] = BIG_ROBOT;
			#else
				answer.data[0] = SMALL_ROBOT;
			#endif
			CAN_send(&answer);
			break;
		default:
			//debug_printf("SID:%x\r\n",msg->sid);
			component_printf(LOG_LEVEL_Trace, "Msg SID: 0x%x(%u)\n", msg->sid, msg->sid);
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
