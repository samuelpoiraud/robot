/*  Club Robot ESEO 2012 - 2013
 *	Krusty
 *
 *	Fichier : Plate.c
 *	Package : Carte actionneur
 *	Description : Gestion de l'assiette contenant les cerises
 *  Auteur : Alexis
 *  Version 20130219
 *  Robot : Krusty
 */

#include "KPlate.h"
#ifdef I_AM_ROBOT_KRUSTY

#include "../QS/QS_DCMotor2.h"
#include "../QS/QS_CANmsgList.h"

static DCMotor_config_t plate_rotation_config;

static Sint16 PLATE_getRotationAngle();

void PLATE_init() {
	plate_rotation_config.sensor_read = &PLATE_getRotationAngle;
	plate_rotation_config.Kp = PLATE_ASSER_KP;
	plate_rotation_config.Ki = PLATE_ASSER_KI;
	plate_rotation_config.Kd = PLATE_ASSER_KD;
	plate_rotation_config.pos[0] = PLATE_HORIZONTAL_POS;
	plate_rotation_config.pos[1] = PLATE_VERTICAL_POS;
	plate_rotation_config.pwm_number = PLATE_DCMOTOR_PWM_NUM;
	plate_rotation_config.way_latch = (Uint16*)&PLATE_DCMOTOR_PORT_WAY;
	plate_rotation_config.way_bit_number = PLATE_DCMOTOR_PORT_WAY_BIT;
	plate_rotation_config.way0_max_duty = PLATE_DCMOTOR_MAX_PWM_WAY0;
	plate_rotation_config.way1_max_duty = PLATE_DCMOTOR_MAX_PWM_WAY1;
	plate_rotation_config.timeout = 2000;
	plate_rotation_config.epsilon = 50;	//TODO: à ajuster
	DCM_config(PLATE_DCMOTOR_ID, &plate_rotation_config);
}

void PLATE_CAN_process_msg(CAN_msg_t* msg) {
	queue_id_t queueId;
	if(msg->sid == ACT_PLATE) {
		switch(msg->data[0]) {
			case ACT_PLATE_PLIER_CLOSE:
				 queueId = QUEUE_create();
				 assert(queueId != QUEUE_CREATE_FAILED);
				 QUEUE_add(queueId, QUEUE_take_sem, 0, QUEUE_ACT_Plate_AX12_Plier);
				 QUEUE_add(queueId, &PLATE_run_command, 0, QUEUE_ACT_Plate_AX12_Plier);
				 QUEUE_add(queueId, QUEUE_give_sem, 0, QUEUE_ACT_Plate_AX12_Plier);
				 break;

			case ACT_PLATE_PLIER_OPEN:
				 queueId = QUEUE_create();
				 assert(queueId != QUEUE_CREATE_FAILED);
				 QUEUE_add(queueId, QUEUE_take_sem, 0, QUEUE_ACT_Plate_AX12_Plier);
				 QUEUE_add(queueId, &PLATE_run_command, 1, QUEUE_ACT_Plate_AX12_Plier);
				 QUEUE_add(queueId, QUEUE_give_sem, 0, QUEUE_ACT_Plate_AX12_Plier);
				 break;

			case ACT_PLATE_ROTATE_HORIZONTALLY:
				 queueId = QUEUE_create();
				 assert(queueId != QUEUE_CREATE_FAILED);
				 QUEUE_add(queueId, QUEUE_take_sem, 0, QUEUE_ACT_Plate_Rotation);
				 QUEUE_add(queueId, &PLATE_run_command, 2, QUEUE_ACT_Plate_Rotation);
				 QUEUE_add(queueId, QUEUE_give_sem, 0, QUEUE_ACT_Plate_Rotation);
				 break;

			case ACT_PLATE_ROTATE_VERTICALLY:
				 queueId = QUEUE_create();
				 assert(queueId != QUEUE_CREATE_FAILED);
				 QUEUE_add(queueId, QUEUE_take_sem, 0, QUEUE_ACT_Plate_Rotation);
				 QUEUE_add(queueId, &PLATE_run_command, 3, QUEUE_ACT_Plate_Rotation);
				 QUEUE_add(queueId, QUEUE_give_sem, 0, QUEUE_ACT_Plate_Rotation);
				 break;

			case ACT_PLATE_PLIER_STOP:
				 queueId = QUEUE_create();
				 assert(queueId != QUEUE_CREATE_FAILED);
				 QUEUE_add(queueId, QUEUE_take_sem, 0, QUEUE_ACT_Plate_AX12_Plier);
				 QUEUE_add(queueId, &PLATE_run_command, 4, QUEUE_ACT_Plate_AX12_Plier);
				 QUEUE_add(queueId, QUEUE_give_sem, 0, QUEUE_ACT_Plate_AX12_Plier);
				 break;

			case ACT_PLATE_ROTATE_STOP:
				 queueId = QUEUE_create();
				 assert(queueId != QUEUE_CREATE_FAILED);
				 QUEUE_add(queueId, QUEUE_take_sem, 0, QUEUE_ACT_Plate_Rotation);
				 QUEUE_add(queueId, &PLATE_run_command, 5, QUEUE_ACT_Plate_Rotation);
				 QUEUE_add(queueId, QUEUE_give_sem, 0, QUEUE_ACT_Plate_Rotation);
				 break;

			default:
				debug_printf("PL: invalid CAN msg data[0]=%d !\n", msg->data[0]);
		}
	}
}

void PLATE_run_command(queue_id_t queueId, bool_e init) {
	
}

static Sint16 PLATE_getRotationAngle() {
	return 0;
}

#endif  /* I_AM_ROBOT_KRUSTY */
