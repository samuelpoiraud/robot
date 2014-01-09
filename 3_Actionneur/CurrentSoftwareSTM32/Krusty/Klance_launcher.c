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

static bool_e lance_launcher_state;

void LANCE_LAUNCHER_init() {
	static bool_e initialized = FALSE;

	if(initialized)
		return;
	initialized = TRUE;
	LANCELAUNCHER_PIN_1 = 0;
	lance_launcher_state = 0;
	TIMER_SRC_TIMER_init();
	component_printf(LOG_LEVEL_Debug, "Lance  launcher init !\n");
}

void LANCE_LAUNCHER_stop() {
	lance_launcher_state = 0;
	LANCELAUNCHER_PIN_1 = 0;
}


bool_e LANCE_LAUNCHER_CAN_process_msg(CAN_msg_t* msg) {
	 LANCE_LAUNCHER_init();
	if(msg->sid == ACT_LANCELAUNCHER) {
		switch(msg->data[0]) {
			case ACT_LANCELAUNCHER_RUN:
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

		if(init == TRUE) {
			Uint8 command = QUEUE_get_arg(queueId)->canCommand;
			//CAN_msg_t resultMsg = {ACT_RESULT, {ACT_BALLINFLATER & 0xFF, command, ACT_RESULT_DONE, ACT_RESULT_ERROR_OK}, 4};

			switch(command) {
				case ACT_LANCELAUNCHER_RUN:
					lance_launcher_state = 1;
					LANCELAUNCHER_PIN_1 = 1;
					component_printf(LOG_LEVEL_Debug, "lanceur démarré\n");
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
	/* pour avoir une activation d'une seconde pour les lanceurs de lance*/
	static Uint8 lance_launcher_timer=0;
	lance_launcher_timer++;
	if (lance_launcher_timer>=10){
		LANCELAUNCHER_PIN_1 = 0;
		lance_launcher_timer=0;
		TIMER_SRC_TIMER_stop();
	}
	TIMER_SRC_TIMER_resetFlag();
}

#endif	//I_AM_ROBOT_TINY

