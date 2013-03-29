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
#include "../Can_msg_processing.h"

#define LOG_PREFIX "BI: "
#define COMPONENT_log(log_level, format, ...) OUTPUTLOG_printf(OUTPUT_LOG_COMPONENT_BALLINFLATER, log_level, LOG_PREFIX format, ## __VA_ARGS__)

#include "../QS/QS_timer.h"
#if (!defined(BALLINFLATER_TIMER_ID) || BALLINFLATER_TIMER_ID > 3 || BALLINFLATER_TIMER_ID < 1) && !defined(AX12_USE_WATCHDOG)
		#error "AX12: BALLINFLATER_TIMER_ID non d�fini ou invalide, vous devez choisir le num�ro du timer entre 1 et 3 inclus, ou utiliser le watchdog avec AX12_USE_WATCHDOG"
#else
	//Pour plus d'info sur la concat�nation de variable dans des macros, voir http://stackoverflow.com/questions/1489932/c-preprocessor-and-concatenation
	#define BALLINFLATER_TEMP_CONCAT_WITH_PREPROCESS(a,b,c) a##b##c
	#define BALLINFLATER_TEMP_CONCAT(a,b,c) BALLINFLATER_TEMP_CONCAT_WITH_PREPROCESS(a,b,c)
	#define BALLINFLATER_TIMER_interrupt _ISR BALLINFLATER_TEMP_CONCAT(_T, BALLINFLATER_TIMER_ID, Interrupt)
	#define BALLINFLATER_TIMER_resetFlag() BALLINFLATER_TEMP_CONCAT(IFS0bits.T, BALLINFLATER_TIMER_ID, IF) = 0
	#define BALLINFLATER_TIMER_init() TIMER_init()
	#define BALLINFLATER_TIMER_start(period_us) BALLINFLATER_TEMP_CONCAT(TIMER, BALLINFLATER_TIMER_ID, _run_us)(period_us)
	//#define BALLINFLATER_TIMER_stop() BALLINFLATER_TEMP_CONCAT(TIMER, BALLINFLATER_TIMER_ID, _stop)()
#endif

static bool_e ballinflater_state = FALSE;

static void BALLINFLATER_run_command(queue_id_t queueId, bool_e init);

void BALLINFLATER_init() {
	static bool_e initialized = FALSE;

	if(initialized)
		return;
	initialized = TRUE;

	BALLINFLATER_TIMER_init();
	BALLINFLATER_TIMER_start(100);
}

static bool_e BALLINFLATER_emerg_stop_inflater = FALSE;

bool_e BALLINFLATER_CAN_process_msg(CAN_msg_t* msg) {
	if(msg->sid == ACT_BALLINFLATER) {
		switch(msg->data[0]) {
			case ACT_BALLINFLATER_START:
				CAN_push_operation_from_msg(msg, QUEUE_ACT_BallInflater, &BALLINFLATER_run_command, msg->data[1]);
				break;

			case ACT_BALLINFLATER_STOP:
				BALLINFLATER_emerg_stop_inflater = TRUE;
				ballinflater_state = BALLINFLATER_OFF;
				BALLINFLATER_PIN = BALLINFLATER_OFF;
				COMPONENT_log(LOG_LEVEL_Debug, "gonfleur stopp� !\n");
				break;

			default:
				COMPONENT_log(LOG_LEVEL_Warning, "invalid CAN msg data[0]=%u !\n", msg->data[0]);
		}
		return TRUE;
	}

	return FALSE;
}

static void BALLINFLATER_run_command(queue_id_t queueId, bool_e init) {
	if(QUEUE_get_act(queueId) == QUEUE_ACT_BallInflater) {
		if(init == TRUE && !QUEUE_has_error(queueId)) {
			Uint8 command = QUEUE_get_arg(queueId)->canCommand;
			//CAN_msg_t resultMsg = {ACT_RESULT, {ACT_BALLINFLATER & 0xFF, command, ACT_RESULT_DONE, ACT_RESULT_ERROR_OK}, 4};

			switch(command) {
				case ACT_BALLINFLATER_START:
					BALLINFLATER_emerg_stop_inflater = FALSE;
					ballinflater_state = BALLINFLATER_ON;
					COMPONENT_log(LOG_LEVEL_Debug, "Gonfleur d�marr�\n");
					//On ne passe pas direct a la commande suivant, on fait une v�rification du temps pour arr�ter le gonflage apr�s le temps demand�
					break;

				case ACT_BALLINFLATER_STOP: //La queue n'est pas utilis�e pour cette commande
					QUEUE_behead(queueId);
					break;

				default: {
//						CAN_msg_t errorMsg = {ACT_RESULT, {ACT_BALLINFLATER & 0xFF, command, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_LOGIC}, 4};
//						CAN_send(&errorMsg);
						CAN_sendResultWithLine(ACT_BALLINFLATER, command, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_LOGIC);
						COMPONENT_log(LOG_LEVEL_Error, "Invalid command: %u, code is broken !\n", command);
						QUEUE_set_error(queueId);
						QUEUE_behead(queueId);
						return;
					}
			}

			//La commande ne peut pas fail (on a aucun retour sur ce qu'il se passe avec le ballon)
			//CAN_send(&resultMsg);
			CAN_sendResultWithLine(ACT_BALLINFLATER, command, ACT_RESULT_DONE, ACT_RESULT_ERROR_OK);
		} else {
			if(QUEUE_has_error(queueId)) {
//				CAN_msg_t resultMsg;
//				resultMsg.data[0] = ACT_BALLINFLATER & 0xFF;
//				resultMsg.data[1] = QUEUE_get_arg(queueId)->canCommand;
//				resultMsg.data[2] = ACT_RESULT_NOT_HANDLED;
//				resultMsg.data[3] = ACT_RESULT_ERROR_OTHER;
//				resultMsg.size = 4;
//				CAN_send(&resultMsg);
				CAN_sendResultWithLine(ACT_BALLINFLATER, QUEUE_get_arg(queueId)->canCommand, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_OTHER);
				QUEUE_behead(queueId);
				return;
			}

			//Si on doit arreter le gonflage maintenant ou si le temps s'est �coul� compl�tement
			if(BALLINFLATER_emerg_stop_inflater == TRUE ||
					CLOCK_get_time() >= (QUEUE_get_arg(queueId)->param*10 + QUEUE_get_initial_time(queueId)) )
			{
				ballinflater_state = BALLINFLATER_OFF;
				COMPONENT_log(LOG_LEVEL_Debug, "Gonfleur stopp�\n");
				QUEUE_behead(queueId);	//gestion termin�e
			}

			//Pas de message de retour ici, il a �t� envoy� pendant l'init
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

