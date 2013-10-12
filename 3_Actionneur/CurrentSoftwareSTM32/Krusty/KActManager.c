/*  Club Robot ESEO 2012 - 2013
 *	Krusty
 *
 *	Fichier : KActManager.c
 *	Package : Carte actionneur
 *	Description : Gestion des actionneurs de Krusty
 *  Auteur : Alexis
 *  Version 20130227
 *  Robot : Krusty
 */

#include "KActManager.h"
#ifdef I_AM_ROBOT_KRUSTY

#include "KBall_launcher.h"
#include "KPlate.h"
#include "KLift.h"
#include "KBall_sorter.h"
#include "../QS/QS_CANmsgList.h"
#include "../QS/QS_ax12.h"
#include "../act_queue_utils.h"
#include "config_pin.h"

static void ACTMGR_run_reset_act(queue_id_t queueId, bool_e init);

void ACTMGR_init() {
	BALLLAUNCHER_init();
	PLATE_init();
	LIFT_init();
	BALLSORTER_init();
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
	//Traitement des messages, si la fonction de traitement d'un actionneur retourne TRUE, c'est que le message a été traité donc arrête le passage du message dans chaque fonction de traitement. (les fonctions de traitement des actionneurs sont responsable de retourner FALSE lorsque le message n'est pas destiné qu'a un actionneur)
	if(BALLLAUNCHER_CAN_process_msg(msg))
		return TRUE;
	if(PLATE_CAN_process_msg(msg))
		return TRUE;
	if(LIFT_CAN_process_msg(msg))
		return TRUE;
	if(BALLSORTER_CAN_process_msg(msg))
		return TRUE;

	return FALSE;
}

void ACTMGR_stop() {
	BALLLAUNCHER_stop();
	PLATE_stop();
	LIFT_stop();
	BALLSORTER_stop();
}


static void ACTMGR_run_reset_act(queue_id_t queueId, bool_e init) {
	if(init) {
		//Init des actionneurs
	} else {
		if(AX12_is_ready(BALLSORTER_AX12_ID)) {
			queue_id_t subQueue;
			//BallSorter
			subQueue = QUEUE_create();
			QUEUE_add(subQueue, &QUEUE_take_sem, (QUEUE_arg_t){0, 0, NULL}, QUEUE_ACT_BallSorter);
			QUEUE_add(subQueue, &BALLSORTER_run_command, (QUEUE_arg_t){ACT_BALLSORTER_TAKE_NEXT_CHERRY, BALLSORTER_CS_TakeCherry, &ACTQ_finish_SendNothing}, QUEUE_ACT_BallSorter);
			QUEUE_add(subQueue, &QUEUE_give_sem, (QUEUE_arg_t){0, 0, NULL}, QUEUE_ACT_BallSorter);

			//Plate
			subQueue = QUEUE_create();
			QUEUE_add(subQueue, &QUEUE_take_sem, (QUEUE_arg_t){0, 0, NULL}, QUEUE_ACT_Plate_AX12_Plier);
			QUEUE_add(subQueue, &QUEUE_take_sem, (QUEUE_arg_t){0, 0, NULL}, QUEUE_ACT_Plate_Rotation);
			QUEUE_add(subQueue, &PLATE_run_command, (QUEUE_arg_t){ACT_PLATE_PLIER_CLOSE, 0, &ACTQ_finish_SendNothing}, QUEUE_ACT_Plate_AX12_Plier);
			QUEUE_add(subQueue, &PLATE_run_command, (QUEUE_arg_t){ACT_PLATE_ROTATE_VERTICALLY, 0, &ACTQ_finish_SendNothing}, QUEUE_ACT_Plate_Rotation);
			QUEUE_add(subQueue, &QUEUE_give_sem, (QUEUE_arg_t){0, 0, NULL}, QUEUE_ACT_Plate_Rotation);
			QUEUE_add(subQueue, &QUEUE_give_sem, (QUEUE_arg_t){0, 0, NULL}, QUEUE_ACT_Plate_AX12_Plier);

			//Lift
			subQueue = QUEUE_create();
			QUEUE_add(subQueue, &QUEUE_take_sem, (QUEUE_arg_t){0, 0, NULL}, QUEUE_ACT_Lift_Left_AX12_Plier);
			QUEUE_add(subQueue, &QUEUE_take_sem, (QUEUE_arg_t){0, 0, NULL}, QUEUE_ACT_Lift_Left_Translation);
			QUEUE_add(subQueue, &QUEUE_take_sem, (QUEUE_arg_t){0, 0, NULL}, QUEUE_ACT_Lift_Right_AX12_Plier);
			QUEUE_add(subQueue, &QUEUE_take_sem, (QUEUE_arg_t){0, 0, NULL}, QUEUE_ACT_Lift_Right_Translation);

			QUEUE_add(subQueue, &LIFT_run_command, (QUEUE_arg_t){ACT_LIFT_PLIER_OPEN, 0, &ACTQ_finish_SendNothing}, QUEUE_ACT_Lift_Left_AX12_Plier);
			QUEUE_add(subQueue, &LIFT_run_command, (QUEUE_arg_t){ACT_LIFT_PLIER_OPEN, 0, &ACTQ_finish_SendNothing}, QUEUE_ACT_Lift_Right_AX12_Plier);

			QUEUE_add(subQueue, &LIFT_run_command, (QUEUE_arg_t){ACT_LIFT_GO_DOWN, 0, &ACTQ_finish_SendNothing}, QUEUE_ACT_Lift_Left_Translation);
			QUEUE_add(subQueue, &LIFT_run_command, (QUEUE_arg_t){ACT_LIFT_GO_DOWN, 0, &ACTQ_finish_SendNothing}, QUEUE_ACT_Lift_Right_Translation);

			QUEUE_add(subQueue, &QUEUE_give_sem, (QUEUE_arg_t){0, 0, NULL}, QUEUE_ACT_Lift_Right_Translation);
			QUEUE_add(subQueue, &QUEUE_give_sem, (QUEUE_arg_t){0, 0, NULL}, QUEUE_ACT_Lift_Right_AX12_Plier);
			QUEUE_add(subQueue, &QUEUE_give_sem, (QUEUE_arg_t){0, 0, NULL}, QUEUE_ACT_Lift_Left_Translation);
			QUEUE_add(subQueue, &QUEUE_give_sem, (QUEUE_arg_t){0, 0, NULL}, QUEUE_ACT_Lift_Left_AX12_Plier);

			QUEUE_behead(queueId);
		} else if(global.match_started == TRUE) {
			//Le match a démarré, on arrete d'essayer de bouger les actionneurs
			QUEUE_behead(queueId);
		}
	}
}

static void ACTMGR_run_selftest_act(queue_id_t queueId, bool_e init) {
	if(init) {
		//Init des actionneurs
	} else {
		if(AX12_is_ready(BALLSORTER_AX12_ID)) {
			//BallSorter
			QUEUE_add(queueId, &QUEUE_take_sem, (QUEUE_arg_t){0, 0, NULL}, QUEUE_ACT_BallSorter);
			QUEUE_add(queueId, &BALLSORTER_run_command, (QUEUE_arg_t){ACT_BALLSORTER_TAKE_NEXT_CHERRY, BALLSORTER_CS_EjectCherry, &ACTQ_finish_SendNothing}, QUEUE_ACT_BallSorter);
			QUEUE_add(queueId, &BALLSORTER_run_command, (QUEUE_arg_t){ACT_BALLSORTER_TAKE_NEXT_CHERRY, BALLSORTER_CS_TakeCherry, &ACTQ_finish_SendNothing}, QUEUE_ACT_BallSorter);
			QUEUE_add(queueId, &QUEUE_give_sem, (QUEUE_arg_t){0, 0, NULL}, QUEUE_ACT_BallSorter);

			//Plate
			QUEUE_add(queueId, &QUEUE_take_sem, (QUEUE_arg_t){0, 0, NULL}, QUEUE_ACT_Plate_AX12_Plier);
			QUEUE_add(queueId, &QUEUE_take_sem, (QUEUE_arg_t){0, 0, NULL}, QUEUE_ACT_Plate_Rotation);
			QUEUE_add(queueId, &PLATE_run_command, (QUEUE_arg_t){ACT_PLATE_ROTATE_PREPARE, 0, &ACTQ_finish_SendNothing}, QUEUE_ACT_Plate_Rotation);
			QUEUE_add(queueId, &PLATE_run_command, (QUEUE_arg_t){ACT_PLATE_PLIER_OPEN, 0, &ACTQ_finish_SendNothing}, QUEUE_ACT_Plate_AX12_Plier);
			QUEUE_add(queueId, &PLATE_run_command, (QUEUE_arg_t){ACT_PLATE_PLIER_CLOSE, 0, &ACTQ_finish_SendNothing}, QUEUE_ACT_Plate_AX12_Plier);
			QUEUE_add(queueId, &PLATE_run_command, (QUEUE_arg_t){ACT_PLATE_ROTATE_VERTICALLY, 0, &ACTQ_finish_SendNothing}, QUEUE_ACT_Plate_Rotation);
			QUEUE_add(queueId, &QUEUE_give_sem, (QUEUE_arg_t){0, 0, NULL}, QUEUE_ACT_Plate_Rotation);
			QUEUE_add(queueId, &QUEUE_give_sem, (QUEUE_arg_t){0, 0, NULL}, QUEUE_ACT_Plate_AX12_Plier);

			//Lift
			QUEUE_add(queueId, &QUEUE_take_sem, (QUEUE_arg_t){0, 0, NULL}, QUEUE_ACT_Lift_Left_AX12_Plier);
			QUEUE_add(queueId, &QUEUE_take_sem, (QUEUE_arg_t){0, 0, NULL}, QUEUE_ACT_Lift_Left_Translation);
			QUEUE_add(queueId, &QUEUE_take_sem, (QUEUE_arg_t){0, 0, NULL}, QUEUE_ACT_Lift_Right_AX12_Plier);
			QUEUE_add(queueId, &QUEUE_take_sem, (QUEUE_arg_t){0, 0, NULL}, QUEUE_ACT_Lift_Right_Translation);

			QUEUE_add(queueId, &LIFT_run_command, (QUEUE_arg_t){ACT_LIFT_PLIER_CLOSE, 0, &ACTQ_finish_SendNothing}, QUEUE_ACT_Lift_Left_AX12_Plier);
			QUEUE_add(queueId, &LIFT_run_command, (QUEUE_arg_t){ACT_LIFT_PLIER_CLOSE, 0, &ACTQ_finish_SendNothing}, QUEUE_ACT_Lift_Right_AX12_Plier);

			QUEUE_add(queueId, &LIFT_run_command, (QUEUE_arg_t){ACT_LIFT_PLIER_OPEN, 0, &ACTQ_finish_SendNothing}, QUEUE_ACT_Lift_Left_AX12_Plier);
			QUEUE_add(queueId, &LIFT_run_command, (QUEUE_arg_t){ACT_LIFT_PLIER_OPEN, 0, &ACTQ_finish_SendNothing}, QUEUE_ACT_Lift_Right_AX12_Plier);

			QUEUE_add(queueId, &LIFT_run_command, (QUEUE_arg_t){ACT_LIFT_GO_MID, 0, &ACTQ_finish_SendNothing}, QUEUE_ACT_Lift_Left_Translation);
			QUEUE_add(queueId, &LIFT_run_command, (QUEUE_arg_t){ACT_LIFT_GO_MID, 0, &ACTQ_finish_SendNothing}, QUEUE_ACT_Lift_Right_Translation);

			QUEUE_add(queueId, &LIFT_run_command, (QUEUE_arg_t){ACT_LIFT_GO_DOWN, 0, &ACTQ_finish_SendNothing}, QUEUE_ACT_Lift_Left_Translation);
			QUEUE_add(queueId, &LIFT_run_command, (QUEUE_arg_t){ACT_LIFT_GO_DOWN, 0, &ACTQ_finish_SendNothing}, QUEUE_ACT_Lift_Right_Translation);

			QUEUE_add(queueId, &QUEUE_give_sem, (QUEUE_arg_t){0, 0, NULL}, QUEUE_ACT_Lift_Right_Translation);
			QUEUE_add(queueId, &QUEUE_give_sem, (QUEUE_arg_t){0, 0, NULL}, QUEUE_ACT_Lift_Right_AX12_Plier);
			QUEUE_add(queueId, &QUEUE_give_sem, (QUEUE_arg_t){0, 0, NULL}, QUEUE_ACT_Lift_Left_Translation);
			QUEUE_add(queueId, &QUEUE_give_sem, (QUEUE_arg_t){0, 0, NULL}, QUEUE_ACT_Lift_Left_AX12_Plier);

			//ATTENTION AU NOMBRE DE ADD vs QUEUE_SIZE dans queue.h !!!! (actuellement: 29 / 32
			QUEUE_behead(queueId);
		} else if(global.match_started == TRUE) {
			//Le match a démarré, on arrete d'essayer de bouger les actionneurs
			QUEUE_behead(queueId);
		}
	}
}

#endif  /* I_AM_ROBOT_KRUSTY */
