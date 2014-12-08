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

#include "ActManager.h"

#define LOG_PREFIX "CANProcess: "
#define LOG_COMPONENT OUTPUT_LOG_COMPONENT_CANPROCESSMSG
#include "QS/QS_outputlog.h"
#include "QS/QS_who_am_i.h"

void CAN_process_msg(CAN_msg_t* msg) {
	CAN_msg_t answer;
	if(ACTMGR_process_msg(msg)) {
		component_printf(LOG_LEVEL_Debug, "Act Msg SID: 0x%03x, cmd: 0x%x(%u), size: %d\n", msg->sid, msg->data[0], msg->data[0], msg->size);
		return;  //Le message a déja été géré
	}


	// Traitement des autres messages reçus
	switch (msg->sid)
	{
		case BROADCAST_RESET:
			NVIC_SystemReset();
			break;

		//Fin de la partie
		case BROADCAST_STOP_ALL :
			global.match_started = FALSE;
			global.match_over = TRUE;
			component_printf(LOG_LEVEL_Info, "C:BROADCAST_STOP_ALL\n");
			QUEUE_flush_all();
			ACTMGR_stop();
			#ifdef USE_DCMOTOR2
				DCM_stop_all();
			#endif
			break;

		//Reprise de la partie
		case BROADCAST_START :
			component_printf(LOG_LEVEL_Info, "C:BROADCAST_START\n");
			global.match_started = TRUE;
			break;

		case BROADCAST_POSITION_ROBOT:
				global.pos.x = U16FROMU8(msg->data[0],msg->data[1]) & 0x1FFF;
				global.pos.y = U16FROMU8(msg->data[2],msg->data[3]) & 0x1FFF;
				global.pos.angle = U16FROMU8(msg->data[4],msg->data[5]);
			break;

		case BROADCAST_BEACON_ADVERSARY_POSITION_IR:
			break;

		case BROADCAST_ADVERSARIES_POSITION:
			break;

		case BROADCAST_ALIM:
			if(msg->data[0] == ALIM_OFF){
				component_printf(LOG_LEVEL_Info, "C:BROADCAST_ALIM -> ALIM_OFF\n");
				global.alim = FALSE;
			}else if(msg->data[0] == ALIM_ON){
				static bool_e first_on = TRUE;
				component_printf(LOG_LEVEL_Info, "C:BROADCAST_ALIM -> ALIM_ON\n");
				global.alim = TRUE;
				if(first_on)
					first_on = FALSE;
				else
					ACTMGR_reset_config();
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

		case IHM_SWITCH_ALL:
		case IHM_BUTTON:
		case IHM_SWITCH:
		case IHM_POWER:
			IHM_process_main(msg);
			break;

		default:
			component_printf(LOG_LEVEL_Trace, "Msg SID: 0x%03x(%u)\n", msg->sid, msg->sid);
			break;
	}//End switch
}
