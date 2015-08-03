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


#include "Can_msg_processing.h"
#include "QS/QS_DCMotor2.h"
#include "QS/QS_can.h"
#include "QS/QS_can_verbose.h"
#include "QS/QS_watchdog.h"
#include "QS/QS_IHM.h"
#include "queue.h"

#include "ActManager.h"

#define LOG_PREFIX "CANProcess: "
#define LOG_COMPONENT OUTPUT_LOG_COMPONENT_CANPROCESSMSG
#include "QS/QS_outputlog.h"
#include "QS/QS_who_am_i.h"

static void CAN_send_callback(CAN_msg_t* msg);
static void break_asser_end_match(void);

void CAN_process_init(){
	CAN_init();
	CAN_set_send_callback(CAN_send_callback);
}

void CAN_process_msg(CAN_msg_t* msg) {
	CAN_msg_t answer;
	if(ACTMGR_process_msg(msg)) {
		component_printf(LOG_LEVEL_Debug, "Act Msg SID: 0x%03x, cmd: 0x%x(%u), size: %d\n", msg->sid, msg->data.act_msg.order, msg->data.act_msg.order, msg->size);
		return;  //Le message a déja été géré
	}


	// Traitement des autres messages reçus
	switch (msg->sid)
	{
		case BROADCAST_RESET:
			NVIC_SystemReset();
			break;

		//Fin de la partie
		case BROADCAST_STOP_ALL :{
			global.flags.match_started = FALSE;
			global.flags.match_over = TRUE;
			#ifdef USE_DCMOTOR2
				DCM_stop_all();
			#endif
			QUEUE_flush_all();

			CAN_msg_t msg;
			msg.size = 1;

			msg.sid = ACT_STOCK_RIGHT;
			msg.data.act_msg.order = ACT_STOCK_RIGHT_OPEN;
			CAN_process_msg(&msg);


			msg.sid = ACT_STOCK_LEFT;
			msg.data.act_msg.order = ACT_STOCK_LEFT_OPEN;
			CAN_process_msg(&msg);


			msg.sid = ACT_PINCEMI_LEFT;
			msg.data.act_msg.order = ACT_PINCEMI_LEFT_OPEN;
			CAN_process_msg(&msg);


			msg.sid = ACT_PINCEMI_RIGHT;
			msg.data.act_msg.order = ACT_PINCEMI_RIGHT_OPEN;
			CAN_process_msg(&msg);

			WATCHDOG_create(500, &break_asser_end_match, FALSE);
			}break;

		//Reprise de la partie
		case BROADCAST_START :
			global.flags.match_started = TRUE;
			break;

		case BROADCAST_POSITION_ROBOT:
				global.pos.x = msg->data.broadcast_position_robot.x;
				global.pos.y = msg->data.broadcast_position_robot.y;
				global.pos.angle = msg->data.broadcast_position_robot.angle;
			break;

		case BROADCAST_BEACON_ADVERSARY_POSITION_IR:
			break;

		case BROADCAST_ADVERSARIES_POSITION:
			break;

		case BROADCAST_ALIM:
			if(msg->data.broadcast_alim.state == FALSE){
				global.flags.alim = FALSE;
			}else{
#ifdef USE_DCMOTOR2
				DCM_reset_integrator();
#endif
				static bool_e first_on = TRUE;
				global.flags.alim = TRUE;
				if(first_on)
					first_on = FALSE;
				else
					ACTMGR_reset_config();
			}
			global.alim_value = msg->data.broadcast_alim.value;
			break;

		case ACT_PING:
			answer.sid = STRAT_ACT_PONG;
			answer.size = SIZE_STRAT_ACT_PONG;
			#ifdef I_AM_ROBOT_BIG
				answer.data.strat_act_pong.robot_id = BIG_ROBOT;
			#else
				answer.data.strat_act_pong.robot_id = SMALL_ROBOT;
			#endif
			CAN_send(&answer);
			break;

		case ACT_ASK_SENSOR:{
			bool_e found = TRUE;
			answer.sid = STRAT_INFORM_CAPTEUR;
			answer.size = SIZE_STRAT_INFORM_CAPTEUR;
			answer.data.strat_inform_capteur.sensor_id = msg->data.act_ask_sensor.act_sensor_id;
			switch(msg->data.act_ask_sensor.act_sensor_id){
#ifdef I_AM_ROBOT_BIG

#else
				case ACT_SENSOR_ID_PINCE_GOBELET_DROITE:
					answer.data.strat_inform_capteur.present = WT100_GOBELET_RIGHT;
					break;

				case ACT_SENSOR_ID_PINCE_GOBELET_GAUCHE:
					answer.data.strat_inform_capteur.present = WT100_GOBELET_LEFT;
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

static void break_asser_end_match(void){
	ACTMGR_stop();
}
