/*  Club Robot ESEO 2012 - 2013
 *	Krusty & Tiny
 *
 *	Fichier : Long_hammer.c
 *	Package : Carte actionneur
 *	Description : Gestion du marteau appuyant sur les bougies du haut
 *  Auteur : Alexis
 *  Version 20130207
 */

#include "Long_hammer.h"
#include "QS/QS_DCMotor.h"
#include "QS/QS_adc.h"

static DCMotor_config_t long_hammer_config;

static Sint16 LONGHAMMER_get_position();

void LONGHAMMER_init() {
	static bool_e initialized = FALSE;
	if(initialized)
		return;
	initialized = TRUE;

	DCM_init();

	long_hammer_config.sensor_read = &LONGHAMMER_get_position;
	long_hammer_config.Kp = LONGHAMMER_ASSER_KP*1024;
	long_hammer_config.Ki = LONGHAMMER_ASSER_KI*1048576;
	long_hammer_config.Kd = LONGHAMMER_ASSER_KD*1024;
	long_hammer_config.pos[0] = LONGHAMMER_TARGET_POS_DOWN;
	long_hammer_config.pos[1] = LONGHAMMER_TARGET_POS_UP;
	long_hammer_config.pos[2] = LONGHAMMER_TARGET_POS_PARKED;
	long_hammer_config.pwm_number = LONGHAMMER_DCMOTOR_PWM_NUM;
	long_hammer_config.way_latch = (Uint16*)&LONGHAMMER_DCMOTOR_PORT_WAY;
	long_hammer_config.way_bit_number = LONGHAMMER_DCMOTOR_PORT_WAY_BIT;
	long_hammer_config.way0_max_duty = LONGHAMMER_DCMOTOR_MAX_PWM_WAY0;
	long_hammer_config.way1_max_duty = LONGHAMMER_DCMOTOR_MAX_PWM_WAY1;
}

void LONGHAMMER_run_command(queue_id_t queueId, bool_e init) {
	if(init == TRUE) {
		//Send command
		LONGHAMMER_command_e cmd = QUEUE_get_arg(queueId);
		if(cmd == LH_CMD_GoUp || cmd == LH_CMD_GoDown || cmd == LH_CMD_Park)
			DCM_goToPos(LONGHAMMER_DCMOTOR_ID, (Uint8)cmd);	// 2 == Park, 1 == GoUp, 0 == GoDown, voir .h
		else {
			DCM_stop(LONGHAMMER_DCMOTOR_ID);
			QUEUE_behead(queueId);	//gestion terminée
		}
	} else {
		DCM_working_state_e asserState = DCM_get_state(LONGHAMMER_DCMOTOR_ID);
		if(asserState == DCM_TIMEOUT) {
			//TODO: Réponse CAN que le bras n'a pas pu être asservi
			QUEUE_behead(queueId);	//gestion terminée
		} else if(asserState == DCM_IDLE) {
			QUEUE_behead(queueId);	//gestion terminée, le bras est à sa position
		}
	}
}

static Sint16 LONGHAMMER_get_position() {
	return ADC_getValue(LONGHAMMER_SENSOR_ADC_ID);
}
