/*  Club Robot ESEO 2012 - 2013
 *	Tiny
 *
 *	Fichier : Hammer.c
 *	Package : Carte actionneur
 *	Description : Gestion du marteau appuyant sur les bougies du bas
 *  Auteur : Alexis
 *  Version 20130207
 *  Robot : Tiny
 */

#include "THammer.h"
#ifdef I_AM_ROBOT_TINY

#include "../QS/QS_DCMotor2.h"
#include "../QS/QS_adc.h"
#include "../QS/QS_CANmsgList.h"
#include "../QS/QS_can.h"
#include "../output_log.h"
#include "../Can_msg_processing.h"

#define LOG_PREFIX "H: "
#define COMPONENT_log(log_level, format, ...) OUTPUTLOG_printf(OUTPUT_LOG_COMPONENT_HAMMER, log_level, LOG_PREFIX format, ## __VA_ARGS__)

#if DCMOTOR_NB_POS < 1
#error "Le nombre de position disponible dans l'asservissement DCMotor n'est pas suffisant"
#endif
#if DCM_NUMBER <= HAMMER_DCMOTOR_ID
#error "Le nombre de DCMotor disponible n'est pas suffisant, veuillez augmenter DCM_NUMBER"
#endif

static void HAMMER_run_command(queue_id_t queueId, bool_e init);
static Sint16 HAMMER_get_position();

void HAMMER_init() {
	DCMotor_config_t hammer_config;
	static bool_e initialized = FALSE;

	if(initialized)
		return;
	initialized = TRUE;

	DCM_init();
	ADC_init();

	hammer_config.sensor_read = &HAMMER_get_position;
	hammer_config.Kp = HAMMER_ASSER_KP;
	hammer_config.Ki = HAMMER_ASSER_KI;
	hammer_config.Kd = HAMMER_ASSER_KD;
	hammer_config.pos[0] = 0;   //Position dynamiques indiquées dans le message CAN
	hammer_config.pwm_number = HAMMER_DCMOTOR_PWM_NUM;
	hammer_config.way_latch = &HAMMER_DCMOTOR_PORT_WAY;
	hammer_config.way_bit_number = HAMMER_DCMOTOR_PORT_WAY_BIT;
	hammer_config.way0_max_duty = HAMMER_DCMOTOR_MAX_PWM_WAY0;
	hammer_config.way1_max_duty = HAMMER_DCMOTOR_MAX_PWM_WAY1;
	hammer_config.timeout = HAMMER_ASSER_TIMEOUT;
	hammer_config.epsilon = HAMMER_ASSER_POS_EPSILON;
	DCM_config(HAMMER_DCMOTOR_ID, &hammer_config);
	DCM_stop(HAMMER_DCMOTOR_ID);
}

bool_e HAMMER_CAN_process_msg(CAN_msg_t* msg) {
	if(msg->sid == ACT_HAMMER) {
		switch(msg->data[0]) {
			case ACT_HAMMER_MOVE_TO:   //Position dans data[1] et data[2]
				CAN_push_operation_from_msg(msg, QUEUE_ACT_Hammer, &HAMMER_run_command, msg->data[1] | (msg->data[2] << 8));
				break;

			case ACT_HAMMER_STOP:	//Ne pas passer par la pile pour le cas d'urgence
				COMPONENT_log(LOG_LEVEL_Debug, "bras désasservi !\n");
				DCM_stop(HAMMER_DCMOTOR_ID); //S'il y a eu une commande en cours, elle se terminera par un idle et donc renvera un message identique au suivant (mais pour la commande d'asservissement)
				CAN_msg_t resultMsg = {ACT_RESULT, {msg->sid & 0xFF, msg->data[0], ACT_RESULT_DONE, ACT_RESULT_ERROR_OK}, 4};
				CAN_send(&resultMsg);
				break;

			default:
				COMPONENT_log(LOG_LEVEL_Warning, "invalid CAN msg data[0]=%u !\n", msg->data[0]);
		}
		return TRUE;
	}

	return FALSE;
}

static void HAMMER_run_command(queue_id_t queueId, bool_e init) {
	if(QUEUE_get_act(queueId) == QUEUE_ACT_Hammer) {
		if(init == TRUE && !QUEUE_has_error(queueId)) {
			//Send command
			Uint8 command = QUEUE_get_arg(queueId)->canCommand;
			Sint16 wantedPosition;
			Sint16 realPosition; //use potar units

			switch(command) {
				case ACT_HAMMER_MOVE_TO: wantedPosition = QUEUE_get_arg(queueId)->param;   break;

				default: {
						CAN_msg_t resultMsg = {ACT_RESULT, {ACT_PLATE & 0xFF, command, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_LOGIC}, 4};
						CAN_send(&resultMsg);
						COMPONENT_log(LOG_LEVEL_Error, "invalid rotation command: %u, code is broken !\n", command);
						QUEUE_set_error(queueId);
						QUEUE_behead(queueId);
						return;
					}
			}

			realPosition = ((Sint32)wantedPosition)*HAMMER_UNITS_PER_128_DEGRE + HAMMER_UNITS_AT_0_DEGRE;
			DCM_setPosValue(HAMMER_DCMOTOR_ID, 0, realPosition);
			DCM_goToPos(HAMMER_DCMOTOR_ID, 0);
			DCM_restart(HAMMER_DCMOTOR_ID);
		} else {
			DCM_working_state_e asserState = DCM_get_state(HAMMER_DCMOTOR_ID);
			CAN_msg_t resultMsg;

			if(QUEUE_has_error(queueId)) {
				resultMsg.data[2] = ACT_RESULT_NOT_HANDLED;
				resultMsg.data[3] = ACT_RESULT_ERROR_OTHER;
			} else if(asserState == DCM_IDLE) {
				resultMsg.data[2] = ACT_RESULT_DONE;
				resultMsg.data[3] = ACT_RESULT_ERROR_OK;
			} else if(asserState == DCM_TIMEOUT) {
				resultMsg.data[2] = ACT_RESULT_FAILED;
				resultMsg.data[3] = ACT_RESULT_ERROR_TIMEOUT;
				QUEUE_set_error(queueId);
			} else return;	//Operation is not finished, do nothing

			resultMsg.sid = ACT_RESULT;
			resultMsg.data[0] = ACT_HAMMER & 0xFF;
			resultMsg.data[1] = QUEUE_get_arg(queueId)->canCommand;
			resultMsg.size = 4;

			CAN_send(&resultMsg);
			QUEUE_behead(queueId);	//gestion terminée
		}
	}
}

static Sint16 HAMMER_get_position() {
	return ADC_getValue(HAMMER_SENSOR_ADC_ID);
}

#endif	//I_AM_ROBOT_TINY

