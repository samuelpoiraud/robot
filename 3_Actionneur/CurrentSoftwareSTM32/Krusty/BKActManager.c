/*  Club Robot ESEO 2012 - 2013
 *	BIG
 *
 *	Fichier : KActManager.c
 *	Package : Carte actionneur
 *	Description : Gestion des actionneurs de BIG
 *  Auteur : Alexis
 *  Version 20130227
 *  Robot : BIG
 */

#include "BActManager.h"
#ifdef I_AM_ROBOT_BIG

#include "KBall_launcher.h"
#include "Plance_launcher.h"
#include "KPlate.h"
#include "KLift.h"
#include "KBall_sorter.h"
#include "PFruit.h"
#include "../QS/QS_CANmsgList.h"
#include "../QS/QS_ax12.h"
#include "../act_queue_utils.h"
#include "config_pin.h"
#include "../Common/Arm.h"
#include "../QS/QS_outputlog.h"
#include "../selftest.h"

static void ACTMGR_run_reset_act(queue_id_t queueId, bool_e init);

#define ACT_DECLARE(prefix) {&prefix##_init, &prefix##_stop, &prefix##_CAN_process_msg}

static ACTQ_functions_t actionneurs[] = {
	{&FRUIT_init, &FRUIT_stop, &FRUIT_CAN_process_msg},
	{&LANCE_LAUNCHER_init, &LANCE_LAUNCHER_stop, &LANCE_LAUNCHER_CAN_process_msg},
	ACT_DECLARE(ARM)
};

static const Uint8 NB_ACTIONNEURS = sizeof(actionneurs) / sizeof(ACTQ_functions_t);

void ACTMGR_init() {
	Uint8 i;
	debug_printf("Init de %d actionneurs\n", NB_ACTIONNEURS);
	for(i = 0; i < NB_ACTIONNEURS; i++) {
		actionneurs[i].onInit();
	}

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

	Uint8 i;

	if(msg->sid == ACT_DO_SELFTEST) {
		if(SELFTEST_new_selftest(NB_ACTIONNEURS) == FALSE)
			return TRUE;  //si selftest déjà en cours, ne pas le refaire
	}

	for(i = 0; i < NB_ACTIONNEURS; i++) {
		//Dans le cas du selftest, on fait le test pour tous les actionneurs, qu'ils gèrent ou non le message
		if(actionneurs[i].onCanMsg(msg) && msg->sid != ACT_DO_SELFTEST)
			return TRUE;
	}

	return FALSE;
}

void ACTMGR_stop() {
	Uint8 i;
	for(i = 0; i < NB_ACTIONNEURS; i++) {
		actionneurs[i].onStop();
	}
}


static void ACTMGR_run_reset_act(queue_id_t queueId, bool_e init) {
	if(init) {
		//Init des actionneurs
	} else {
		if(AX12_is_ready(FRUIT_MOUTH_AX12_ID)) {
			queue_id_t subQueue;
		/*	//BallSorter
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
*/
			subQueue = QUEUE_create();
			QUEUE_add(subQueue, &QUEUE_take_sem, (QUEUE_arg_t){0, 0, NULL}, QUEUE_ACT_AX12_Fruit);
			QUEUE_add(subQueue, &FRUIT_run_command, (QUEUE_arg_t){ACT_FRUIT_MOUTH_CLOSE, 0,  &ACTQ_finish_SendNothing}, QUEUE_ACT_AX12_Fruit);
			QUEUE_add(subQueue, &QUEUE_give_sem, (QUEUE_arg_t){0, 0, NULL}, QUEUE_ACT_AX12_Fruit);

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
		if(AX12_is_ready(FRUIT_MOUTH_AX12_ID)) {
			//BallSorter
	/*		QUEUE_add(queueId, &QUEUE_take_sem, (QUEUE_arg_t){0, 0, NULL}, QUEUE_ACT_BallSorter);
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

*/
			//ATTENTION AU NOMBRE DE ADD vs QUEUE_SIZE dans queue.h !!!! (actuellement: 29 / 32

			QUEUE_add(queueId, &QUEUE_take_sem, (QUEUE_arg_t){0, 0, NULL}, QUEUE_ACT_AX12_Fruit);
			QUEUE_add(queueId, &FRUIT_run_command, (QUEUE_arg_t){ACT_FRUIT_MOUTH_CLOSE, 0,  &ACTQ_finish_SendNothing}, QUEUE_ACT_AX12_Fruit);
			QUEUE_add(queueId, &QUEUE_give_sem, (QUEUE_arg_t){0, 0, NULL}, QUEUE_ACT_AX12_Fruit);

			QUEUE_behead(queueId);
		} else if(global.match_started == TRUE) {
			//Le match a démarré, on arrete d'essayer de bouger les actionneurs
			QUEUE_behead(queueId);
		}
	}
}

#endif  /* I_AM_ROBOT_BIG */
