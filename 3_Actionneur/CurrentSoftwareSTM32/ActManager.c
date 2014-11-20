/*  Club Robot ESEO 2012 - 2013
 *
 *	Fichier : ActManager.c
 *	Package : Carte actionneur
 *	Description : Gestion des actionneurs
 *  Auteur : Alexis, remake Arnaud
 *  Version 20130227
 */
#include "ActManager.h"

#ifdef I_AM_ROBOT_BIG
	#include "Pierre/TestServo.h"
#else

#endif

#include "QS/QS_CANmsgList.h"
#include "QS/QS_ax12.h"
#include "QS/QS_outputlog.h"
#include "act_queue_utils.h"
#include "selftest.h"
#include "Wood/Pince_gauche.h"
#include "Wood/Pince_gauche_config.h"
#include "Wood/Pince_droite.h"
#include "Wood/Pince_droite_config.h"

static void ACTMGR_run_reset_act(queue_id_t queueId, bool_e init);

#define ACT_DECLARE(prefix) {&prefix##_init, &prefix##_init_pos, &prefix##_stop, &prefix##_reset_config, &prefix##_CAN_process_msg}

static ACTQ_functions_t actionneurs[] = {
	#if 0
		ACT_DECLARE(EXEMPLE),
	#endif

	#ifdef I_AM_ROBOT_BIG
		ACT_DECLARE(TEST_SERVO),
	#else
		ACT_DECLARE(PINCE_GAUCHE),
		ACT_DECLARE(PINCE_DROITE)
	#endif
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

void ACTMGR_reset_config() {
	Uint8 i;
	debug_printf("Re-init de %d actionneurs\n", NB_ACTIONNEURS);
	for(i = 0; i < NB_ACTIONNEURS; i++) {
		if(actionneurs[i].onResetConfig != NULL)
			actionneurs[i].onResetConfig();
	}
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
	} else {
		bool_e isReady = FALSE, responseReceived = FALSE;
		/*#ifdef I_AM_ROBOT_BIG
				responseReceived = AX12_async_is_ready(TORCH_LOCKER_AX12_1_ID, &isReady);
		#else
				responseReceived = AX12_async_is_ready(SMALL_ARM_AX12_ID, &isReady);
		#endif*/

		if((responseReceived && isReady) || global.alim) { // Si il y a le +12/24V (on laisse le AX12_is_ready si on utilise le FDP hors robot sous 12V mais l'initialisation peut ne pas marcher si l'ax12 test� n'est pas pr�sent)
			debug_printf("Init pos\n");
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

void ACTMGR_config_AX12(Uint8 id_servo, CAN_msg_t* msg){
	switch(msg->data[1]){
		case AX12_SPEED_CONFIG : // Configuration de la vitesse
			if(AX12_is_wheel_mode_enabled(id_servo)){
				AX12_set_speed_percentage(id_servo, msg->data[2]);
				debug_printf("Configuration de la vitesse (wheel mode) de l'AX12 %d avec une valeur de %d\n", id_servo, msg->data[2]);
			}else{
				AX12_set_move_to_position_speed(id_servo, U16FROMU8(msg->data[3], msg->data[2]));
				debug_printf("Configuration de la vitesse (position mode) de l'AX12 %d avec une valeur de %d\n", id_servo, U16FROMU8(msg->data[3], msg->data[2]));
			}

			break;

		case AX12_TORQUE_CONFIG : // Configuration du couple
			AX12_set_torque_limit(id_servo, msg->data[3]);
			debug_printf("Configuration du couple de l'AX12 %d avec une valeur de %d\n", id_servo, msg->data[2]);
			break;

		default :
			warn_printf("invalid CAN msg data[2]=%u (configuration impossible)!\n", msg->data[1]);
	}
}
