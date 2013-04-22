/*  Club Robot ESEO 2012 - 2013
 *	Tiny
 *
 *	Fichier : TActManager.c
 *	Package : Carte actionneur
 *	Description : Gestion des actionneurs de Tiny
 *  Auteur : Alexis
 *  Version 20130227
 *  Robot : Tiny
 */

#include "TActManager.h"
#ifdef I_AM_ROBOT_TINY

#include "THammer.h"
#include "TBall_inflater.h"
#include "TCandle_color_sensor.h"
#include "../QS/QS_CANmsgList.h"
#include "../QS/QS_ax12.h"
#include "../act_queue_utils.h"

static void ACTMGR_run_reset_act(queue_id_t queueId, bool_e init);

//Initialise les actionneurs
void ACTMGR_init() {
	HAMMER_init();
	BALLINFLATER_init();
	CANDLECOLOR_init();
	ACTMGR_reset_act();
}

//Met les actionneurs en position de départ
void ACTMGR_reset_act() {
	queue_id_t queueId;

	queueId = QUEUE_create();
	QUEUE_add(queueId, &ACTMGR_run_reset_act, (QUEUE_arg_t){0, 0, NULL}, 0);
}

//Gère les messages CAN des actionneurs. Si le message à été géré, cette fonction renvoie TRUE, sinon FALSE.
bool_e ACTMGR_process_msg(CAN_msg_t* msg) {
	if(HAMMER_CAN_process_msg(msg))
		return TRUE;
	if(BALLINFLATER_CAN_process_msg(msg))
		return TRUE;
	if(CANDLECOLOR_CAN_process_msg(msg))
		return TRUE;

	return FALSE;
}

//Stop tous les actionneurs
void ACTMGR_stop() {
	HAMMER_stop();
	BALLINFLATER_stop();
	CANDLECOLOR_stop();
}

static void ACTMGR_run_reset_act(queue_id_t queueId, bool_e init) {
	if(init) {
		//Init des actionneurs
	} else {
		if(AX12_is_ready(2)) {
			//Hammer
			QUEUE_add(queueId, &QUEUE_take_sem, (QUEUE_arg_t){0, 0, NULL}, QUEUE_ACT_Hammer);
			QUEUE_add(queueId, &HAMMER_run_command, (QUEUE_arg_t){ACT_HAMMER_MOVE_TO, HAMMER_ACT_MOVE_TO_INIT_POS, &ACTQ_finish_SendNothing}, QUEUE_ACT_Hammer);
			QUEUE_add(queueId, &QUEUE_give_sem, (QUEUE_arg_t){0, 0, NULL}, QUEUE_ACT_Hammer);
			QUEUE_behead(queueId);
		} else if(global.match_started == TRUE) {
			//Le match a démarré, on arrete d'essayer de bouger les actionneurs
			QUEUE_behead(queueId);
		}
	}
}

#endif  /* I_AM_ROBOT_TINY */
