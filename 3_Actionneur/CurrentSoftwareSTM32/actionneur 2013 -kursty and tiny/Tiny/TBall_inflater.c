/*  Club Robot ESEO 2012 - 2013
 *	Tiny
 *
 *	Fichier : Ball_inflater.c
 *	Package : Carte actionneur
 *	Description : Gestion du gonfleur du ballon
 *  Auteur : Alexis
 *  Version 20130312
 *  Robot : Tiny
 */

#include "TBall_inflater.h"
#ifdef I_AM_ROBOT_TINY

#include "../QS/QS_CANmsgList.h"
//#include "../QS/QS_can.h"
#include "../act_queue_utils.h"
#include "config_tiny/config_pin.h"

#include "config_debug.h"
#define LOG_PREFIX "BI: "
#define LOG_COMPONENT OUTPUT_LOG_COMPONENT_BALLINFLATER
#include "../QS/QS_outputlog.h"


#ifdef BALLINFLATER_TIMER_ID
	#define TIMER_SRC_TIMER_ID BALLINFLATER_TIMER_ID
#endif
#ifdef BALLINFLATER_TIMER_USE_WATCHDOG
	#define TIMER_SRC_USE_WATCHDOG
#endif

#include "../QS/QS_setTimerSource.h"

static bool_e ballinflater_state;
static bool_e BALLINFLATER_emerg_stop_inflater = FALSE;

void BALLINFLATER_init() {
	static bool_e initialized = FALSE;

	if(initialized)
		return;
	initialized = TRUE;

	ballinflater_state = BALLINFLATER_OFF;
	TIMER_SRC_TIMER_init();
	TIMER_SRC_TIMER_start_ms(100);
}

void BALLINFLATER_stop() {
	ballinflater_state = BALLINFLATER_OFF;
	//BALLINFLATER_emerg_stop_inflater = TRUE;
	BALLINFLATER_PIN = BALLINFLATER_OFF;
}


bool_e BALLINFLATER_CAN_process_msg(CAN_msg_t* msg) {
	if(msg->sid == ACT_BALLINFLATER) {
		switch(msg->data[0]) {
			case ACT_BALLINFLATER_START:
				ACTQ_push_operation_from_msg(msg, QUEUE_ACT_BallInflater, &BALLINFLATER_run_command, msg->data[1]*10);  //param en centaine de ms, data[1] en sec
				break;

			case ACT_BALLINFLATER_STOP:
				BALLINFLATER_emerg_stop_inflater = TRUE;
				ballinflater_state = BALLINFLATER_OFF;
				BALLINFLATER_PIN = BALLINFLATER_OFF;
				component_printf(LOG_LEVEL_Debug, "gonfleur stoppé !\n");
				break;

			default:
				component_printf(LOG_LEVEL_Warning, "invalid CAN msg data[0]=%u !\n", msg->data[0]);
		}
		return TRUE;
	}

	return FALSE;
}

void BALLINFLATER_run_command(queue_id_t queueId, bool_e init) {
	if(QUEUE_get_act(queueId) == QUEUE_ACT_BallInflater) {
		if(QUEUE_has_error(queueId)) {
			QUEUE_behead(queueId);
			return;
		}

		if(init == TRUE) {
			Uint8 command = QUEUE_get_arg(queueId)->canCommand;
			//CAN_msg_t resultMsg = {ACT_RESULT, {ACT_BALLINFLATER & 0xFF, command, ACT_RESULT_DONE, ACT_RESULT_ERROR_OK}, 4};

			switch(command) {
				case ACT_BALLINFLATER_START:
					BALLINFLATER_emerg_stop_inflater = FALSE;
					ballinflater_state = BALLINFLATER_ON;
					component_printf(LOG_LEVEL_Debug, "Gonfleur démarré\n");
					//On ne passe pas direct a la commande suivant, on fait une vérification du temps pour arrêter le gonflage après le temps demandé
					break;

				case ACT_BALLINFLATER_STOP: //La queue n'est pas utilisée pour cette commande
					QUEUE_behead(queueId);
					break;

				default: {
						component_printf(LOG_LEVEL_Error, "Invalid command: %u, code is broken !\n", command);
						QUEUE_next(queueId, ACT_BALLINFLATER, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_LOGIC, __LINE__);
						return;
					}
			}
		} else {
			//Si on doit arreter le gonflage maintenant ou si le temps s'est écoulé complètement
			if(BALLINFLATER_emerg_stop_inflater == TRUE ||
					CLOCK_get_time() >= (QUEUE_get_arg(queueId)->param + QUEUE_get_initial_time(queueId)) )
			{
				ballinflater_state = BALLINFLATER_OFF;
				component_printf(LOG_LEVEL_Debug, "Gonfleur stoppé\n");
				//La commande ne peut pas fail (on a aucun retour sur ce qu'il se passe avec le ballon)
				QUEUE_next(queueId, ACT_BALLINFLATER, ACT_RESULT_DONE, ACT_RESULT_ERROR_OK, __LINE__);
			}
		}
	}
}

void BALLINFLATER_TIMER_interrupt() {
	static Uint8 state = 0;
	static Uint8 pwm_percentage = 0;
	static Uint16 i = 0;

	if(ballinflater_state == BALLINFLATER_OFF) {
		i = 0;
		pwm_percentage = 25;
	}

	if(i > 800)
		pwm_percentage = 50;
	if(i > 1600)
		pwm_percentage = 75;
//	if(i > 2400)
//		pwm_percentage = 100;

	if(i < 32000)
		i++;

	switch(state) {
		case 0:
			if(pwm_percentage >= 25)
				BALLINFLATER_PIN = ballinflater_state;
			else BALLINFLATER_PIN = BALLINFLATER_OFF;
			state = 1;
			break;

		case 1:
			if(pwm_percentage >= 50)
				BALLINFLATER_PIN = ballinflater_state;
			else BALLINFLATER_PIN = BALLINFLATER_OFF;
			state = 2;
			break;

		case 2:
			if(pwm_percentage >= 75)
				BALLINFLATER_PIN = ballinflater_state;
			else BALLINFLATER_PIN = BALLINFLATER_OFF;
			state = 3;
			break;

		case 3:
		default:
			if(pwm_percentage >= 100)
				BALLINFLATER_PIN = ballinflater_state;
			else BALLINFLATER_PIN = BALLINFLATER_OFF;
			state = 0;
			break;
	}

	TIMER_SRC_TIMER_resetFlag();
}

#endif	//I_AM_ROBOT_TINY

