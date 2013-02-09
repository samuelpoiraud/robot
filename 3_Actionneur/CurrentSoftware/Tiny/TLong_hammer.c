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
#ifdef I_AM_ROBOT_TINY

#include "../QS/QS_DCMotor.h"
#include "../QS/QS_adc.h"
#include "../QS/QS_CANmsgList.h"
#include "../QS/QS_can.h"

typedef enum {
	LH_CMD_GoDown = 0,
	LH_CMD_GoUp = 1,
	LH_CMD_Park = 2,
	LH_CMD_StopAsser
} LONGHAMMER_command_e;

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
	long_hammer_config.timeout = 2000; //FIXME: Utiliser une valeur correcte (en ms)
}

void LONGHAMMER_CAN_process_msg(CAN_msg_t* msg) {
	queue_id_t queueId;
	if(msg->sid == ACT_LONGHAMMER) {
		switch(msg->data[0]) {
			case ACT_LONGHAMMER_GO_DOWN:
				 queueId = QUEUE_create();
				 assert(queueId != QUEUE_CREATE_FAILED);
				 QUEUE_add(queueId, QUEUE_take_sem, 0, QUEUE_ACT_LongHammer);
				 QUEUE_add(queueId, &LONGHAMMER_run_command, LH_CMD_GoDown, QUEUE_ACT_LongHammer);
				 QUEUE_add(queueId, QUEUE_give_sem, 0, QUEUE_ACT_LongHammer);
				 break;

			case ACT_LONGHAMMER_GO_UP:
				 queueId = QUEUE_create();
				 assert(queueId != QUEUE_CREATE_FAILED);
				 QUEUE_add(queueId, QUEUE_take_sem, 0, QUEUE_ACT_LongHammer);
				 QUEUE_add(queueId, &LONGHAMMER_run_command, LH_CMD_GoUp, QUEUE_ACT_LongHammer);
				 QUEUE_add(queueId, QUEUE_give_sem, 0, QUEUE_ACT_LongHammer);
				 break;

			case ACT_LONGHAMMER_GO_PARK:
				 queueId = QUEUE_create();
				 assert(queueId != QUEUE_CREATE_FAILED);
				 QUEUE_add(queueId, QUEUE_take_sem, 0, QUEUE_ACT_LongHammer);
				 QUEUE_add(queueId, &LONGHAMMER_run_command, LH_CMD_Park, QUEUE_ACT_LongHammer);
				 QUEUE_add(queueId, QUEUE_give_sem, 0, QUEUE_ACT_LongHammer);
				 break;

			case ACT_LONGHAMMER_GO_STOP:
				 queueId = QUEUE_create();
				 assert(queueId != QUEUE_CREATE_FAILED);
				 QUEUE_add(queueId, QUEUE_take_sem, 0, QUEUE_ACT_LongHammer);
				 QUEUE_add(queueId, &LONGHAMMER_run_command, LH_CMD_StopAsser, QUEUE_ACT_LongHammer);
				 QUEUE_add(queueId, QUEUE_give_sem, 0, QUEUE_ACT_LongHammer);
				 break;

			default:
				debug_printf("LH: invalid CAN msg data[0]=%d !\n", msg->data[0]);
		}
	}
}

void LONGHAMMER_run_command(queue_id_t queueId, bool_e init) {
	if(QUEUE_get_act(queueId) == QUEUE_ACT_LongHammer) {
		if(init == TRUE) {
			//Send command
			LONGHAMMER_command_e cmd = QUEUE_get_arg(queueId);
			if(cmd == LH_CMD_GoUp || cmd == LH_CMD_GoDown || cmd == LH_CMD_Park)
				DCM_goToPos(LONGHAMMER_DCMOTOR_ID, (Uint8)cmd);	// 2 == Park, 1 == GoUp, 0 == GoDown, voir .h
			else {	//LH_CMD_StopAsser
				debug_printf("LH: bras désasservi !\n");
				DCM_stop(LONGHAMMER_DCMOTOR_ID);
				QUEUE_behead(queueId);	//gestion terminée
			}
		} else {
			DCM_working_state_e asserState = DCM_get_state(LONGHAMMER_DCMOTOR_ID);
			if(asserState == DCM_TIMEOUT) {

				//Envoi du message de timeout
				/*
				CAN_msg_t timeoutMsg;
				timeoutMsg.sid = ACT_LONGHAMMER_RESULT;
				timeoutMsg.data[0] = ACT_LONGHAMMER_TIMEOUT;
				timeoutMsg.size = 1;
				CAN_send(&timeoutMsg);		//*/
				
				QUEUE_behead(queueId);	//gestion terminée
			} else if(asserState == DCM_IDLE) {
				QUEUE_behead(queueId);	//gestion terminée, le bras est à sa position
			}
		}
	}
}

static Sint16 LONGHAMMER_get_position() {
	return ADC_getValue(LONGHAMMER_SENSOR_ADC_ID);
}

#endif	//I_AM_ROBOT_TINY
