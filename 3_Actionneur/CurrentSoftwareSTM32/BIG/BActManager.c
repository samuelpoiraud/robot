/*  Club Robot ESEO 2012 - 2013
 *	BIG
 *
 *	Fichier : BActManager.c
 *	Package : Carte actionneur
 *	Description : Gestion des actionneurs de BIG
 *  Auteur : Alexis
 *  Version 20130227
 *  Robot : BIG
 */
#include "BActManager.h"
#ifdef I_AM_ROBOT_BIG

#include "../Pierre/Plance_launcher.h"
#include "../Pierre/PFruit.h"
#include "../Pierre/PFilet.h"
#include "../Common/Arm.h"
#include "../Common/Small_arm.h"
#include "../Common/Pompe.h"

#include "../QS/QS_CANmsgList.h"
#include "../QS/QS_ax12.h"
#include "../QS/QS_outputlog.h"
#include "../act_queue_utils.h"
#include "../selftest.h"
#include "config_pin.h"

static void ACTMGR_run_reset_act(queue_id_t queueId, bool_e init);

#define ACT_DECLARE(prefix) {&prefix##_init, &prefix##_init_pos, &prefix##_stop, &prefix##_CAN_process_msg}

static ACTQ_functions_t actionneurs[] = {
	ACT_DECLARE(FRUIT),
	ACT_DECLARE(SMALL_ARM),
	ACT_DECLARE(POMPE),
	{&LANCE_LAUNCHER_init, NULL, &LANCE_LAUNCHER_stop, &LANCE_LAUNCHER_CAN_process_msg},
	{&FILET_init, NULL, NULL, &FILET_CAN_process_msg},
	{&ARM_init, NULL, &ARM_stop, &ARM_CAN_process_msg}
};

static const Uint8 NB_ACTIONNEURS = sizeof(actionneurs) / sizeof(ACTQ_functions_t);

void ACTMGR_init() {
	Uint8 i;
	debug_printf("Init de %d actionneurs\n", NB_ACTIONNEURS);
	for(i = 0; i < NB_ACTIONNEURS; i++) {
		if(actionneurs[i].onInit != NULL)
			actionneurs[i].onInit();
	}

	ACTMGR_reset_act();
}

//Met les actionneurs en position de d�part
void ACTMGR_reset_act() {
	queue_id_t queueId;

	queueId = QUEUE_create();
	QUEUE_add(queueId, &ACTMGR_run_reset_act, (QUEUE_arg_t){0, 0, NULL}, 0);
}

//G�re les messages CAN des actionneurs. Si le message � �t� g�r�, cette fonction renvoie TRUE, sinon FALSE.
bool_e ACTMGR_process_msg(CAN_msg_t* msg) {
	//Traitement des messages, si la fonction de traitement d'un actionneur retourne TRUE, c'est que le message a �t� trait� donc arr�te le passage du message dans chaque fonction de traitement. (les fonctions de traitement des actionneurs sont responsable de retourner FALSE lorsque le message n'est pas destin� qu'a un actionneur)

	Uint8 i;

	if(msg->sid == ACT_DO_SELFTEST) {
		if(SELFTEST_new_selftest(NB_ACTIONNEURS) == FALSE)
			return TRUE;  //si selftest d�j� en cours, ne pas le refaire
	}

	for(i = 0; i < NB_ACTIONNEURS; i++) {
		//Dans le cas du selftest, on fait le test pour tous les actionneurs, qu'ils g�rent ou non le message
		if(actionneurs[i].onCanMsg != NULL)
			if(actionneurs[i].onCanMsg(msg) && msg->sid != ACT_DO_SELFTEST)
				return TRUE;
	}

	return FALSE;
}

void ACTMGR_stop() {
	Uint8 i;
	for(i = 0; i < NB_ACTIONNEURS; i++) {
		if(actionneurs[i].onStop != NULL)
			actionneurs[i].onStop();
	}
}


static void ACTMGR_run_reset_act(queue_id_t queueId, bool_e init) {
	Uint8 i;
	if(init) {
		//Init des actionneurs
		AX12_async_is_ready(FILET_AX12_ID); //amorce des demandes de ping ax12
	} else {
		bool_e isReady = FALSE;
		Uint16 state = AX12_get_last_error(FILET_AX12_ID).error;
		switch(state) {
			case AX12_ERROR_IN_PROGRESS: //Si demande pas termin�e, on fait rien (isReady = FALSE)
				break;

			case AX12_ERROR_OK: //Si termin� + Ok, alors il est ready
				isReady = TRUE;
				break;

			default: //Si termin� mais pas ok, alors on refait une demande
				AX12_async_is_ready(FILET_AX12_ID);
				break;
		}

		if(isReady || global.alim) { // Si il y a le +12/24V (on laisse le AX12_is_ready si on utilise le FDP hors robot sous 12V mais l'initialisation peut ne pas marcher si l'ax12 test� n'est pas pr�sent)
			for(i = 0; i < NB_ACTIONNEURS; i++) {
				if(actionneurs[i].onInitPos != NULL)
					actionneurs[i].onInitPos();
			}
			QUEUE_behead(queueId);
		} else if(global.match_started == TRUE) {
			//Le match a d�marr�, on arrete d'essayer de bouger les actionneurs
			QUEUE_behead(queueId);
		}
	}
}


#endif  /* I_AM_ROBOT_BIG */
