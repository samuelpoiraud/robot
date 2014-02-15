/*  Club Robot ESEO 2012 - 2013
 *	SMALL
 *
 *	Fichier : SActManager.c
 *	Package : Carte actionneur
 *	Description : Gestion des actionneurs de SMALL
 *  Auteur : Alexis
 *  Version 20130227
 *  Robot : SMALL
 */

#include "SActManager.h"
#ifdef I_AM_ROBOT_SMALL


#include "../QS/QS_CANmsgList.h"
#include "../QS/QS_ax12.h"
#include "../act_queue_utils.h"
#include "config_pin.h"
#include "../Common/Arm.h"

static void ACTMGR_run_reset_act(queue_id_t queueId, bool_e init);

//Initialise les actionneurs
void ACTMGR_init() {

	ARM_init();

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

	if(ARM_CAN_process_msg(msg))
		return TRUE;

	return FALSE;
}

//Stop tous les actionneurs
void ACTMGR_stop() {

}

static void ACTMGR_run_reset_act(queue_id_t queueId, bool_e init) {
	if(init) {
		//Init des actionneurs
	} else {
		if(AX12_is_ready(PLIER_LEFT_AX12_ID)) {
			queue_id_t subQueue;


			subQueue = QUEUE_create();

			/*
			QUEUE_add(subQueue, &QUEUE_take_sem, (QUEUE_arg_t){0, 0, NULL}, QUEUE_ACT_PlierLeft);
			QUEUE_add(subQueue, &PLIER_run_command, (QUEUE_arg_t){ACT_PLIER_CLOSE, PLIER_CS_CloseLeftAX12,  &ACTQ_finish_SendNothing}, QUEUE_ACT_PlierLeft);
			QUEUE_add(subQueue, &QUEUE_give_sem, (QUEUE_arg_t){0, 0, NULL}, QUEUE_ACT_PlierLeft);
			*/

			QUEUE_behead(queueId);

		} else if(global.match_started == TRUE) {
			//Le match a démarré, on arrete d'essayer de bouger les actionneurs
			QUEUE_behead(queueId);
		}
	}
}

#endif  /* I_AM_ROBOT_SMALL */
