/*  Club Robot ESEO 2012 - 2013
 *	Krusty
 *
 *	Fichier : Lance_Launcher.c
 *	Package : Carte actionneur
 *	Description : Gestion du lanceur de lance
 *  Auteur : amaury
 *  Version 1
 *  Robot : Krusty
 */

#include "Klance_launcher.h"
#ifdef I_AM_ROBOT_KRUSTY

#include "../QS/QS_CANmsgList.h"
//#include "../QS/QS_can.h"
#include "../act_queue_utils.h"
#include "config_pin.h"

#include "config_debug.h"
#define LOG_PREFIX "LL: "
#define LOG_COMPONENT OUTPUT_LOG_COMPONENT_LANCELAUNCHER
#include "../QS/QS_outputlog.h"

#ifdef LANCELAUNCHER_TIMER_ID
	#define TIMER_SRC_TIMER_ID LANCELAUNCHER_TIMER_ID
#endif
#ifdef LANCELAUNCHER_TIMER_USE_WATCHDOG
	#define TIMER_SRC_USE_WATCHDOG
#endif

#include "../QS/QS_setTimerSource.h"

static Uint8 lance_launcher_last_launch;

void LANCE_LAUNCHER_init() {
	static bool_e initialized = FALSE;

	if(initialized)
		return;
	initialized = TRUE;
	LANCELAUNCHER_PIN_1 = 0;
	LANCELAUNCHER_PIN_2 = 0;
	LANCELAUNCHER_PIN_3 = 0;
	LANCELAUNCHER_PIN_4 = 0;
	LANCELAUNCHER_PIN_5 = 0;
	LANCELAUNCHER_PIN_6 = 0;
	lance_launcher_last_launch = 0;
	TIMER_SRC_TIMER_init();
	component_printf(LOG_LEVEL_Debug, "Lance  launcher init !\n");
}

void LANCE_LAUNCHER_stop() {
	lance_launcher_last_launch = 0;
	LANCELAUNCHER_PIN_1 = 0;
	LANCELAUNCHER_PIN_2 = 0;
	LANCELAUNCHER_PIN_3 = 0;
	LANCELAUNCHER_PIN_4 = 0;
	LANCELAUNCHER_PIN_5 = 0;
	LANCELAUNCHER_PIN_6 = 0;
}


bool_e LANCE_LAUNCHER_CAN_process_msg(CAN_msg_t* msg) {
	 LANCE_LAUNCHER_init();
	if(msg->sid == ACT_LANCELAUNCHER) {
		switch(msg->data[0]) {
			case ACT_LANCELAUNCHER_RUN:
				ACTQ_push_operation_from_msg(msg, QUEUE_ACT_lancelauncher, &LANCE_LAUNCHER_run_command, 0);  //param en centaine de ms, data[1] en sec
				TIMER_SRC_TIMER_start_ms(100);
				break;
			case ACT_LANCELAUNCHER_RUN_2:
				ACTQ_push_operation_from_msg(msg, QUEUE_ACT_lancelauncher, &LANCE_LAUNCHER_run_command, 0);  //param en centaine de ms, data[1] en sec
				TIMER_SRC_TIMER_start_ms(100);
				break;
			case ACT_LANCELAUNCHER_RUN_3:
				ACTQ_push_operation_from_msg(msg, QUEUE_ACT_lancelauncher, &LANCE_LAUNCHER_run_command, 0);  //param en centaine de ms, data[1] en sec
				TIMER_SRC_TIMER_start_ms(100);
				break;
			case ACT_LANCELAUNCHER_RUN_4:
				ACTQ_push_operation_from_msg(msg, QUEUE_ACT_lancelauncher, &LANCE_LAUNCHER_run_command, 0);  //param en centaine de ms, data[1] en sec
				TIMER_SRC_TIMER_start_ms(100);
				break;
			case ACT_LANCELAUNCHER_RUN_5:
				ACTQ_push_operation_from_msg(msg, QUEUE_ACT_lancelauncher, &LANCE_LAUNCHER_run_command, 0);  //param en centaine de ms, data[1] en sec
				TIMER_SRC_TIMER_start_ms(100);
				break;
			case ACT_LANCELAUNCHER_RUN_6:
				ACTQ_push_operation_from_msg(msg, QUEUE_ACT_lancelauncher, &LANCE_LAUNCHER_run_command, 0);  //param en centaine de ms, data[1] en sec
				TIMER_SRC_TIMER_start_ms(100);
				break;

			case ACT_LANCELAUNCHER_STOP:
				LANCE_LAUNCHER_stop();
				component_printf(LOG_LEVEL_Debug, "lanceur stoppé !\n");
				break;

			default:
				component_printf(LOG_LEVEL_Warning, "invalid CAN msg data[0]=%u !\n", msg->data[0]);
		}
		return TRUE;
	}

	return FALSE;
}

void LANCE_LAUNCHER_run_command(queue_id_t queueId, bool_e init) {
	if(QUEUE_get_act(queueId) == QUEUE_ACT_lancelauncher) {
		if(QUEUE_has_error(queueId)) {
			QUEUE_behead(queueId);
			return;
		}

		if(init == TRUE && lance_launcher_last_launch ==0) { //on verifie qu'aucun lanceur n'est activé (car seul un seul pour l'être à la fois)
			Uint8 command = QUEUE_get_arg(queueId)->canCommand;
			//CAN_msg_t resultMsg = {ACT_RESULT, {ACT_BALLINFLATER & 0xFF, command, ACT_RESULT_DONE, ACT_RESULT_ERROR_OK}, 4};

			switch(command) {
				case ACT_LANCELAUNCHER_RUN:
					lance_launcher_last_launch = 1;
					LANCELAUNCHER_PIN_6 = 1;
					component_printf(LOG_LEVEL_Debug, "lanceur 6 démarré\n");
					//On ne passe pas direct a la commande suivant, on fait une vérification du temps pour arrêter le gonflage après le temps demandé
					break;

				case ACT_LANCELAUNCHER_RUN_2:
					lance_launcher_last_launch = 2;
					LANCELAUNCHER_PIN_5 = 1;
					component_printf(LOG_LEVEL_Debug, "lanceur 5 démarré\n");
					//On ne passe pas direct a la commande suivant, on fait une vérification du temps pour arrêter le gonflage après le temps demandé
					break;

				case ACT_LANCELAUNCHER_RUN_3:
					lance_launcher_last_launch = 3;
					LANCELAUNCHER_PIN_4 = 1;
					component_printf(LOG_LEVEL_Debug, "lanceur 4 démarré\n");
					//On ne passe pas direct a la commande suivant, on fait une vérification du temps pour arrêter le gonflage après le temps demandé
					break;

				case ACT_LANCELAUNCHER_RUN_4:
					lance_launcher_last_launch = 4;
					LANCELAUNCHER_PIN_3 = 1;
					component_printf(LOG_LEVEL_Debug, "lanceur 3 démarré\n");
					//On ne passe pas direct a la commande suivant, on fait une vérification du temps pour arrêter le gonflage après le temps demandé
					break;

				case ACT_LANCELAUNCHER_RUN_5:
					lance_launcher_last_launch = 5;
					LANCELAUNCHER_PIN_2 = 1;
					component_printf(LOG_LEVEL_Debug, "lanceur 2 démarré\n");
					//On ne passe pas direct a la commande suivant, on fait une vérification du temps pour arrêter le gonflage après le temps demandé
					break;

				case ACT_LANCELAUNCHER_RUN_6:
					lance_launcher_last_launch = 6;
					LANCELAUNCHER_PIN_1 = 1;
					component_printf(LOG_LEVEL_Debug, "lanceur 1 démarré\n");
					//On ne passe pas direct a la commande suivant, on fait une vérification du temps pour arrêter le gonflage après le temps demandé
					break;


				case ACT_LANCELAUNCHER_STOP: //La queue n'est pas utilisée pour cette commande
					QUEUE_behead(queueId);
					break;

				default: {
						component_printf(LOG_LEVEL_Error, "Invalid command: %u, code is broken !\n", command);
						QUEUE_next(queueId, ACT_LANCELAUNCHER, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_LOGIC, __LINE__);
						return;
					}
			}
		} else {
			QUEUE_behead(queueId);
		}
	}
}

void TIMER_SRC_TIMER_interrupt() {
	/* pour avoir une activation d'une seconde pour les lanceurs de lances*/
	static Uint8 lance_launcher_timer=0;
	lance_launcher_timer++;
	if (lance_launcher_timer>=10){
		lance_launcher_last_launch--;  //le lancé étant déjà fait on post décrémente lance_launcher_last_launch
		switch(lance_launcher_last_launch){
			case 0 :
				LANCELAUNCHER_PIN_1 = 0; //quoi qu'il arrive on coupe toujours tout les lanceurs (par sécurité)
				LANCELAUNCHER_PIN_2 = 0;
				LANCELAUNCHER_PIN_3 = 0;
				LANCELAUNCHER_PIN_4 = 0;
				LANCELAUNCHER_PIN_5 = 0;
				LANCELAUNCHER_PIN_6 = 0;
				component_printf(LOG_LEVEL_Debug, "FIN des lancés\n");
				TIMER_SRC_TIMER_stop();
				break;
			case 1 :
				LANCELAUNCHER_PIN_1 = 0; //quoi qu'il arrive on coupe toujours tout les lanceurs (par sécurité)
				LANCELAUNCHER_PIN_2 = 0;
				LANCELAUNCHER_PIN_3 = 0;
				LANCELAUNCHER_PIN_4 = 0;
				LANCELAUNCHER_PIN_5 = 0;
				LANCELAUNCHER_PIN_6 = 1;
				component_printf(LOG_LEVEL_Debug, "lanceur 6 démarré\n");
				break;
			case 2 :
				LANCELAUNCHER_PIN_1 = 0; //quoi qu'il arrive on coupe toujours tout les lanceurs (par sécurité)
				LANCELAUNCHER_PIN_2 = 0;
				LANCELAUNCHER_PIN_3 = 0;
				LANCELAUNCHER_PIN_4 = 0;
				LANCELAUNCHER_PIN_5 = 1;
				LANCELAUNCHER_PIN_6 = 0;
				component_printf(LOG_LEVEL_Debug, "lanceur 5 démarré\n");
				break;
			case 3 :
				LANCELAUNCHER_PIN_1 = 0; //quoi qu'il arrive on coupe toujours tout les lanceurs (par sécurité)
				LANCELAUNCHER_PIN_2 = 0;
				LANCELAUNCHER_PIN_3 = 0;
				LANCELAUNCHER_PIN_4 = 1;
				LANCELAUNCHER_PIN_5 = 0;
				LANCELAUNCHER_PIN_6 = 0;
				component_printf(LOG_LEVEL_Debug, "lanceur 4 démarré\n");
				break;
			case 4 :
				LANCELAUNCHER_PIN_1 = 0; //quoi qu'il arrive on coupe toujours tout les lanceurs (par sécurité)
				LANCELAUNCHER_PIN_2 = 0;
				LANCELAUNCHER_PIN_3 = 1;
				LANCELAUNCHER_PIN_4 = 0;
				LANCELAUNCHER_PIN_5 = 0;
				LANCELAUNCHER_PIN_6 = 0;
				component_printf(LOG_LEVEL_Debug, "lanceur 3 démarré\n");
				break;
			case 5 :
				LANCELAUNCHER_PIN_1 = 0; //quoi qu'il arrive on coupe toujours tout les lanceurs (par sécurité)
				LANCELAUNCHER_PIN_2 = 1;
				LANCELAUNCHER_PIN_3 = 0;
				LANCELAUNCHER_PIN_4 = 0;
				LANCELAUNCHER_PIN_5 = 0;
				LANCELAUNCHER_PIN_6 = 0;
				component_printf(LOG_LEVEL_Debug, "lanceur 2 démarré\n");
				break;
		}

		lance_launcher_timer=0;
	}
	TIMER_SRC_TIMER_resetFlag();
}

#endif	//I_AM_ROBOT_TINY

