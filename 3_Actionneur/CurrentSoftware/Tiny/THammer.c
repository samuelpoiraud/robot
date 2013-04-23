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
#include "../output_log.h"
#include "../act_queue_utils.h"
#include "THammer_config.h"
#include "TCandle_color_sensor.h"

#define LOG_PREFIX "Ha: "
#define COMPONENT_log(log_level, format, ...) OUTPUTLOG_printf(OUTPUT_LOG_COMPONENT_HAMMER, log_level, LOG_PREFIX format, ## __VA_ARGS__)

#if DCMOTOR_NB_POS < 1
#error "Le nombre de position disponible dans l'asservissement DCMotor n'est pas suffisant"
#endif
#if DCM_NUMBER <= HAMMER_DCMOTOR_ID
#error "Le nombre de DCMotor disponible n'est pas suffisant, veuillez augmenter DCM_NUMBER"
#endif

static bool_e HAMMER_checkCandleColorFinished(queue_id_t queueId, Uint11 act_sid, Uint8 result, Uint8 error_code, Uint16 param);
static Sint16 HAMMER_get_position();


//Couleur correspondante a celle du robot
static Uint8 good_candle_color = ACT_CANDLECOLOR_COLOR_WHITE;

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

	COMPONENT_log(LOG_LEVEL_Info, "Hammer initialisé\n");
}

void HAMMER_stop() {
	DCM_stop(HAMMER_DCMOTOR_ID);
}

bool_e HAMMER_CAN_process_msg(CAN_msg_t* msg) {
	queue_id_t queueId;

	if(msg->sid == ACT_HAMMER) {
		switch(msg->data[0]) {
			case ACT_HAMMER_MOVE_TO:   //Position dans data[1] et data[2]
				ACTQ_push_operation_from_msg(msg, QUEUE_ACT_Hammer, &HAMMER_run_command, msg->data[1] | ((Uint16)(msg->data[2]) << 8));
				break;

			case ACT_HAMMER_BLOW_CANDLE:
				if(msg->data[1] == BLUE)
					good_candle_color = ACT_CANDLECOLOR_COLOR_BLUE;
				else good_candle_color = ACT_CANDLECOLOR_COLOR_RED;

				queueId = QUEUE_create();
				assert(queueId != QUEUE_CREATE_FAILED);
				if(queueId != QUEUE_CREATE_FAILED) {
					QUEUE_add(queueId, &QUEUE_take_sem, (QUEUE_arg_t){0, 0, NULL}, QUEUE_ACT_CandleColor);
					QUEUE_add(queueId, &CANDLECOLOR_run_command, (QUEUE_arg_t){ACT_CANDLECOLOR_GET_LOW, 0, &HAMMER_checkCandleColorFinished}, QUEUE_ACT_CandleColor);
					QUEUE_add(queueId, &QUEUE_give_sem, (QUEUE_arg_t){0, 0, NULL}, QUEUE_ACT_CandleColor);
				} else {	//on indique qu'on a pas géré la commande
					ACTQ_sendResultWithLine(msg->sid, msg->data[0], ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_NO_RESOURCES);
				}
				break;

			case ACT_HAMMER_STOP:	//Ne pas passer par la pile pour le cas d'urgence
				COMPONENT_log(LOG_LEVEL_Debug, "bras désasservi !\n");
				DCM_stop(HAMMER_DCMOTOR_ID); //S'il y a eu une commande en cours, elle se terminera par un idle et donc renvera un message identique au suivant (mais pour la commande d'asservissement)
				ACTQ_sendResultWithLine(msg->sid, msg->data[0], ACT_RESULT_DONE, ACT_RESULT_ERROR_OK);
				break;

			default:
				COMPONENT_log(LOG_LEVEL_Warning, "invalid CAN msg data[0]=%u !\n", msg->data[0]);
		}
		return TRUE;
	}

	return FALSE;
}

void HAMMER_run_command(queue_id_t queueId, bool_e init) {
	if(QUEUE_get_act(queueId) == QUEUE_ACT_Hammer) {
		if(QUEUE_has_error(queueId)) {
			QUEUE_behead(queueId);
			return;
		}

		if(init == TRUE) {
			//Send command
			Uint8 command = QUEUE_get_arg(queueId)->canCommand;
			Sint16 wantedPosition;
			Sint16 realPosition; //use potar units

			switch(command) {
				case ACT_HAMMER_BLOW_CANDLE: //pas de break, on utilise les mêmes paramètre (angle) pour le HAMMER_run_command (pas comme le msg can)
				case ACT_HAMMER_MOVE_TO: wantedPosition = QUEUE_get_arg(queueId)->param;   break;

				default: {
						COMPONENT_log(LOG_LEVEL_Error, "invalid rotation command: %u, code is broken !\n", command);
						QUEUE_next(queueId, ACT_HAMMER, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_LOGIC, __LINE__);
						return;
					}
			}

			realPosition = (((Sint32)wantedPosition)*HAMMER_UNITS_PER_128_DEGRE) / 128 + HAMMER_UNITS_AT_0_DEGRE;
			COMPONENT_log(LOG_LEVEL_Debug, "Rotation to: %d (angle: %d)\n", realPosition, wantedPosition);
			DCM_setPosValue(HAMMER_DCMOTOR_ID, 0, realPosition);
			DCM_goToPos(HAMMER_DCMOTOR_ID, 0);
			DCM_restart(HAMMER_DCMOTOR_ID);
		} else {
			Uint8 result, errorCode;
			Uint16 line;

			if(ACTQ_check_status_dcmotor(HAMMER_DCMOTOR_ID, TRUE, &result, &errorCode, &line))
				QUEUE_next(queueId, ACT_HAMMER, result, errorCode, line);
		}
	}
}

//Fonction appelée quand le module Candle_color_sensor à terminé de récupérer la couleur de la bougie
static bool_e HAMMER_checkCandleColorFinished(queue_id_t queueId, Uint11 act_sid, Uint8 result, Uint8 error_code, Uint16 param) {
	if(result == ACT_RESULT_DONE) {
		//Si la couleur de la bougie est bonne, souffler la bougie avec le bras
		if(param == ACT_CANDLECOLOR_COLOR_WHITE || param == good_candle_color)
		{
			QUEUE_add(queueId, &QUEUE_take_sem, (QUEUE_arg_t){0, 0, NULL}, QUEUE_ACT_Hammer);
			QUEUE_add(queueId, &HAMMER_run_command, (QUEUE_arg_t){ACT_HAMMER_BLOW_CANDLE, HAMMER_CANDLE_POS_BLOWING, &ACTQ_finish_SendResultIfFail}, QUEUE_ACT_Hammer);
			QUEUE_add(queueId, &HAMMER_run_command, (QUEUE_arg_t){ACT_HAMMER_BLOW_CANDLE, HAMMER_CANDLE_POS_UP     , &ACTQ_finish_SendResult}      , QUEUE_ACT_Hammer);
			QUEUE_add(queueId, &QUEUE_give_sem, (QUEUE_arg_t){0, 0, NULL}, QUEUE_ACT_Hammer);

			return TRUE;
		}
	}

	QUEUE_get_arg(queueId)->canCommand = ACT_HAMMER_BLOW_CANDLE;
	return ACTQ_finish_SendResult(queueId, ACT_HAMMER, result, error_code, param);
}

Uint16 HAMMER_get_pos() {
	Sint16 rawPos = HAMMER_get_position();
	return ((Sint32)(rawPos - HAMMER_UNITS_AT_0_DEGRE))*128/HAMMER_UNITS_PER_128_DEGRE;
}

static Sint16 HAMMER_get_position() {
	return -ADC_getValue(HAMMER_SENSOR_ADC_ID);
}

#endif	//I_AM_ROBOT_TINY

