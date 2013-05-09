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
#include "../output_log.h"
#include "../act_queue_utils.h"

#define LOG_PREFIX "BI: "
#define COMPONENT_log(log_level, format, ...) OUTPUTLOG_printf(OUTPUT_LOG_COMPONENT_BALLINFLATER, log_level, LOG_PREFIX format, ## __VA_ARGS__)

#include "../QS/QS_timer.h"
#if (!defined(BALLINFLATER_TIMER_ID) || BALLINFLATER_TIMER_ID > 3 || BALLINFLATER_TIMER_ID < 1) && !defined(AX12_USE_WATCHDOG)
		#error "AX12: BALLINFLATER_TIMER_ID non défini ou invalide, vous devez choisir le numéro du timer entre 1 et 3 inclus, ou utiliser le watchdog avec AX12_USE_WATCHDOG"
#else
	//Pour plus d'info sur la concaténation de variable dans des macros, voir http://stackoverflow.com/questions/1489932/c-preprocessor-and-concatenation
	#define BALLINFLATER_TEMP_CONCAT_WITH_PREPROCESS(a,b,c) a##b##c
	#define BALLINFLATER_TEMP_CONCAT(a,b,c) BALLINFLATER_TEMP_CONCAT_WITH_PREPROCESS(a,b,c)
	#define BALLINFLATER_TIMER_interrupt _ISR BALLINFLATER_TEMP_CONCAT(_T, BALLINFLATER_TIMER_ID, Interrupt)
	#define BALLINFLATER_TIMER_resetFlag() BALLINFLATER_TEMP_CONCAT(IFS0bits.T, BALLINFLATER_TIMER_ID, IF) = 0
	#define BALLINFLATER_TIMER_init() TIMER_init()
	#define BALLINFLATER_TIMER_start(period_us) BALLINFLATER_TEMP_CONCAT(TIMER, BALLINFLATER_TIMER_ID, _run_us)(period_us)
	#define BALLINFLATER_TIMER_stop() BALLINFLATER_TEMP_CONCAT(TIMER, BALLINFLATER_TIMER_ID, _stop)()
#endif

static bool_e ballinflater_state;
static bool_e BALLINFLATER_emerg_stop_inflater = FALSE;

void BALLINFLATER_init() {
	static bool_e initialized = FALSE;

	if(initialized)
		return;
	initialized = TRUE;

	ballinflater_state = BALLINFLATER_OFF;
	BALLINFLATER_TIMER_init();
	BALLINFLATER_TIMER_start(100);
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
				COMPONENT_log(LOG_LEVEL_Debug, "gonfleur stoppé !\n");
				break;

			default:
				COMPONENT_log(LOG_LEVEL_Warning, "invalid CAN msg data[0]=%u !\n", msg->data[0]);
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
					COMPONENT_log(LOG_LEVEL_Debug, "Gonfleur démarré\n");
					//On ne passe pas direct a la commande suivant, on fait une vérification du temps pour arrêter le gonflage après le temps demandé
					break;

				case ACT_BALLINFLATER_STOP: //La queue n'est pas utilisée pour cette commande
					QUEUE_behead(queueId);
					break;

				default: {
						COMPONENT_log(LOG_LEVEL_Error, "Invalid command: %u, code is broken !\n", command);
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
				COMPONENT_log(LOG_LEVEL_Debug, "Gonfleur stoppé\n");
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

	BALLINFLATER_TIMER_resetFlag();
}

#endif	//I_AM_ROBOT_TINY

