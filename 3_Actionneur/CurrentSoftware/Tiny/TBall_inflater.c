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
#include "../QS/QS_can.h"
#include "../output_log.h"
#include "../Can_msg_processing.h"

#define LOG_PREFIX "BI: "

static void BALLINFLATER_run_command(queue_id_t queueId, bool_e init);

void BALLINFLATER_init() {
	static bool_e initialized = FALSE;

	if(initialized)
		return;
	initialized = TRUE;
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
				BALLINFLATER_PIN = BALLINFLATER_OFF;
				OUTPUTLOG_printf(LOG_LEVEL_Debug, LOG_PREFIX"gonfleur stoppé !\n");
				break;

			default:
				OUTPUTLOG_printf(LOG_LEVEL_Warning, LOG_PREFIX"invalid CAN msg data[0]=%u !\n", msg->data[0]);
		}
		return TRUE;
	}

	return FALSE;
}

static void BALLINFLATER_run_command(queue_id_t queueId, bool_e init) {
	if(QUEUE_get_act(queueId) == QUEUE_ACT_BallInflater) {
		if(init == TRUE && !QUEUE_has_error(queueId)) {
			Uint8 command = QUEUE_get_arg(queueId)->canCommand;
			CAN_msg_t resultMsg = {ACT_RESULT, {ACT_BALLINFLATER & 0xFF, command, ACT_RESULT_DONE, ACT_RESULT_ERROR_OK}, 4};

			switch(command) {
				case ACT_BALLINFLATER_START:
					BALLINFLATER_emerg_stop_inflater = FALSE;
					BALLINFLATER_PIN = BALLINFLATER_ON;
					OUTPUTLOG_printf(LOG_LEVEL_Debug, LOG_PREFIX"gonfleur démarré\n");
					//On ne passe pas direct a la commande suivant, on fait une vérification du temps pour arrêter le gonflage après le temps demandé
					break;

				case ACT_BALLINFLATER_STOP:
					QUEUE_behead(queueId);
					break;

				default: {
						CAN_msg_t errorMsg = {ACT_RESULT, {ACT_BALLINFLATER & 0xFF, command, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_LOGIC}, 4};
						CAN_send(&errorMsg);
						OUTPUTLOG_printf(LOG_LEVEL_Error, LOG_PREFIX"invalid command: %u, code is broken !\n", command);
						QUEUE_set_error(queueId);
						QUEUE_behead(queueId);
						return;
					}
			}

			//La commande ne peut pas fail (on a aucun retour sur ce qu'il se passe avec le ballon)
			CAN_send(&resultMsg);
		} else {
			if(QUEUE_has_error(queueId)) {
				CAN_msg_t resultMsg;
				resultMsg.data[0] = ACT_BALLINFLATER & 0xFF;
				resultMsg.data[1] = QUEUE_get_arg(queueId)->canCommand;
				resultMsg.data[2] = ACT_RESULT_NOT_HANDLED;
				resultMsg.data[3] = ACT_RESULT_ERROR_OTHER;
				resultMsg.size = 4;
				CAN_send(&resultMsg);
				QUEUE_behead(queueId);
				return;
			}

			//Si on doit arreter le gonflage maintenant ou si le temps s'est écoulé complètement
			if(BALLINFLATER_emerg_stop_inflater == TRUE ||
					CLOCK_get_time() >= (QUEUE_get_arg(queueId)->param*10 + QUEUE_get_initial_time(queueId)) )
			{
				BALLINFLATER_PIN = BALLINFLATER_OFF;
				QUEUE_behead(queueId);	//gestion terminée
			}

			//Pas de message de retour ici, il a été envoyé pendant l'init
		}
	}
}

#endif	//I_AM_ROBOT_TINY

