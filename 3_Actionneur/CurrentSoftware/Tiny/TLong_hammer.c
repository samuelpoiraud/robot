/*  Club Robot ESEO 2012 - 2013
 *	Tiny
 *
 *	Fichier : Long_hammer.c
 *	Package : Carte actionneur
 *	Description : Gestion du marteau appuyant sur les bougies du haut
 *  Auteur : Alexis
 *  Version 20130207
 *  Robot : Tiny
 */

#include "TLong_hammer.h"
//#ifdef I_AM_ROBOT_TINY
#if 0 //Long_hammer non utilisé

#include "../QS/QS_DCMotor2.h"
#include "../QS/QS_adc.h"
#include "../QS/QS_CANmsgList.h"
#include "../QS/QS_can.h"
#include "../output_log.h"
#include "../Can_msg_processing.h"

#define LOG_PREFIX "LH: "
#define COMPONENT_log(log_level, format, ...) OUTPUTLOG_printf(OUTPUT_LOG_COMPONENT_LONGHAMMER, log_level, LOG_PREFIX format, ## __VA_ARGS__)

#define LONGHAMMER_NUM_POS           3
	#define LONGHAMMER_PARKED_POS_ID 0
	#define LONGHAMMER_UP_POS_ID     1
	#define LONGHAMMER_DOWN_POS_ID   2


#if DCMOTOR_NB_POS < LONGHAMMER_NUM_POS
#error "Le nombre de position disponible dans l'asservissement DCMotor n'est pas suffisant"
#endif

static void LONGHAMMER_run_command(queue_id_t queueId, bool_e init);
static Sint16 LONGHAMMER_get_position();

void LONGHAMMER_init() {
	DCMotor_config_t long_hammer_config;
	static bool_e initialized = FALSE;

	if(initialized)
		return;
	initialized = TRUE;

	DCM_init();
	ADC_init();

	long_hammer_config.sensor_read = &LONGHAMMER_get_position;
	long_hammer_config.Kp = LONGHAMMER_ASSER_KP;
	long_hammer_config.Ki = LONGHAMMER_ASSER_KI;
	long_hammer_config.Kd = LONGHAMMER_ASSER_KD;
	long_hammer_config.pos[LONGHAMMER_PARKED_POS_ID] = LONGHAMMER_TARGET_POS_PARKED;
	long_hammer_config.pos[LONGHAMMER_UP_POS_ID] = LONGHAMMER_TARGET_POS_UP;
	long_hammer_config.pos[LONGHAMMER_DOWN_POS_ID] = LONGHAMMER_TARGET_POS_DOWN;
	long_hammer_config.pwm_number = LONGHAMMER_DCMOTOR_PWM_NUM;
	long_hammer_config.way_latch = &LONGHAMMER_DCMOTOR_PORT_WAY;
	long_hammer_config.way_bit_number = LONGHAMMER_DCMOTOR_PORT_WAY_BIT;
	long_hammer_config.way0_max_duty = LONGHAMMER_DCMOTOR_MAX_PWM_WAY0;
	long_hammer_config.way1_max_duty = LONGHAMMER_DCMOTOR_MAX_PWM_WAY1;
	long_hammer_config.timeout = LONGHAMMER_ASSER_TIMEOUT;
	long_hammer_config.epsilon = LONGHAMMER_ASSER_POS_EPSILON;
	DCM_config(LONGHAMMER_DCMOTOR_ID, &long_hammer_config);
	DCM_stop(LONGHAMMER_DCMOTOR_ID);
}

bool_e LONGHAMMER_CAN_process_msg(CAN_msg_t* msg) {
	if(msg->sid == ACT_LONGHAMMER) {
		switch(msg->data[0]) {
			case ACT_LONGHAMMER_GO_DOWN:
			case ACT_LONGHAMMER_GO_UP:
			case ACT_LONGHAMMER_GO_PARK:
				CAN_push_operation_from_msg(msg, QUEUE_ACT_LongHammer, &LONGHAMMER_run_command, 0);
				break;

			case ACT_LONGHAMMER_GO_STOP:	//Ne pas passer par la pile pour le cas d'urgence
				COMPONENT_log(LOG_LEVEL_Debug, "bras désasservi !\n");
				DCM_stop(LONGHAMMER_DCMOTOR_ID);
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

static void LONGHAMMER_run_command(queue_id_t queueId, bool_e init) {
	if(QUEUE_get_act(queueId) == QUEUE_ACT_LongHammer) {
		if(init == TRUE) {
			//Send command
			Uint8 command = QUEUE_get_arg(queueId)->canCommand;
			Uint8 wantedPosition;
			switch(command) {
				case ACT_LONGHAMMER_GO_DOWN: wantedPosition = LONGHAMMER_DOWN_POS_ID;   break;
				case ACT_LONGHAMMER_GO_UP:   wantedPosition = LONGHAMMER_UP_POS_ID;     break;
				case ACT_LONGHAMMER_GO_PARK: wantedPosition = LONGHAMMER_PARKED_POS_ID; break;
				case ACT_LONGHAMMER_GO_STOP:
					COMPONENT_log(LOG_LEVEL_Debug, "bras désasservi !\n");
					DCM_stop(LONGHAMMER_DCMOTOR_ID);
					return;

				default: {
						CAN_msg_t resultMsg = {ACT_RESULT, {ACT_LONGHAMMER & 0xFF, command, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_LOGIC}, 4};
						CAN_send(&resultMsg);
						COMPONENT_log(LOG_LEVEL_Error, "invalid rotation command: %u, code is broken !\n", command);
						QUEUE_set_error(queueId);
						QUEUE_behead(queueId);
						return;
					}
			}
			DCM_goToPos(LONGHAMMER_DCMOTOR_ID, wantedPosition);
		} else {
			DCM_working_state_e asserState = DCM_get_state(LONGHAMMER_DCMOTOR_ID);
			CAN_msg_t resultMsg;

			if(asserState == DCM_IDLE) {
				resultMsg.data[2] = ACT_RESULT_DONE;
				resultMsg.data[3] = ACT_RESULT_ERROR_OK;
			} else if(QUEUE_has_error(queueId)) {
				resultMsg.data[2] = ACT_RESULT_NOT_HANDLED;
				resultMsg.data[3] = ACT_RESULT_ERROR_OTHER;
			} else if(asserState == DCM_TIMEOUT) {
				resultMsg.data[2] = ACT_RESULT_FAILED;
				resultMsg.data[3] = ACT_RESULT_ERROR_TIMEOUT;
				QUEUE_set_error(queueId);
			} else return;	//Operation is not finished, do nothing

			resultMsg.sid = ACT_RESULT;
			resultMsg.data[0] = ACT_LONGHAMMER & 0xFF;
			resultMsg.data[1] = QUEUE_get_arg(queueId)->canCommand;
			resultMsg.size = 4;

			CAN_send(&resultMsg);
			QUEUE_behead(queueId);	//gestion terminée
		}
	}
}

static Sint16 LONGHAMMER_get_position() {
	return ADC_getValue(LONGHAMMER_SENSOR_ADC_ID);
}

#endif	//I_AM_ROBOT_TINY
