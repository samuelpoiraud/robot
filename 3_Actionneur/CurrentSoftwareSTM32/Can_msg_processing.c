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


#include "Can_msg_processing.h"
#include "QS/QS_DCMotor2.h"
#include "QS/QS_can.h"
#include "QS/QS_CANmsgList.h"
#include "QS/QS_can_verbose.h"
#include "QS/QS_IHM.h"
#include "queue.h"

#include "ActManager.h"

#define LOG_PREFIX "CANProcess: "
#define LOG_COMPONENT OUTPUT_LOG_COMPONENT_CANPROCESSMSG
#include "QS/QS_outputlog.h"
#include "QS/QS_who_am_i.h"

static void CAN_send_callback(CAN_msg_t* msg);

void CAN_process_init(){
	CAN_init();
	CAN_set_send_callback(CAN_send_callback);
}

void CAN_process_msg(CAN_msg_t* msg) {
	CAN_msg_t answer;
	if(ACTMGR_process_msg(msg)) {
		component_printf(LOG_LEVEL_Debug, "Act Msg SID: 0x%03x, cmd: 0x%x(%u), size: %d\n", msg->sid, msg->data[0], msg->data[0], msg->size);
		return;  //Le message a d�ja �t� g�r�
	}


	// Traitement des autres messages re�us
	switch (msg->sid)
	{
		case BROADCAST_RESET:
			NVIC_SystemReset();
			break;

		//Fin de la partie
		case BROADCAST_STOP_ALL :
			global.match_started = FALSE;
			global.match_over = TRUE;
			QUEUE_flush_all();
			ACTMGR_stop();
			#ifdef USE_DCMOTOR2
				DCM_stop_all();
			#endif
			break;

		//Reprise de la partie
		case BROADCAST_START :
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
				global.alim = FALSE;
			}else if(msg->data[0] == ALIM_ON){
				static bool_e first_on = TRUE;
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

		case ACT_ASK_SENSOR:{
			bool_e found = TRUE;
			answer.sid = STRAT_INFORM_CAPTEUR;
			answer.size = 2;
			answer.data[0] = msg->data[0];
			switch(msg->data[0]){
#ifdef I_AM_ROBOT_BIG
				case GOBELET_HOLLY:
					if(PRESENCE_GOBELET_CENTRAL)
						answer.data[1] = STRAT_INFORM_CAPTEUR_PRESENT;
					else
						answer.data[1] = STRAT_INFORM_CAPTEUR_ABSENT;
					break;
#else
				case PINCE_GOBELET_DROITE:
					if(WT100_GOBELET_RIGHT)
						answer.data[1] = STRAT_INFORM_CAPTEUR_PRESENT;
					else
						answer.data[1] = STRAT_INFORM_CAPTEUR_ABSENT;
					break;

				case PINCE_GOBELET_GAUCHE:
					if(WT100_GOBELET_LEFT)
						answer.data[1] = STRAT_INFORM_CAPTEUR_PRESENT;
					else
						answer.data[1] = STRAT_INFORM_CAPTEUR_ABSENT;
					break;

				case GOBELET_DEVANT:
					if(WT100_GOBELET_FRONT)
						answer.data[1] = STRAT_INFORM_CAPTEUR_PRESENT;
					else
						answer.data[1] = STRAT_INFORM_CAPTEUR_ABSENT;
					break;
#endif
				default:
					found = FALSE;
					break;
			}
			if(found)
				CAN_send(&answer);
		}break;

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

static void CAN_send_callback(CAN_msg_t* msg){
	UNUSED_VAR(msg);
	#ifdef CAN_VERBOSE_MODE
		QS_CAN_VERBOSE_can_msg_print(msg, VERB_OUTPUT_MSG);
	#endif
}
