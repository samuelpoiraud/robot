/*  Club Robot ESEO 2012 - 2013
 *	Krusty
 *
 *	Fichier : Long_launcher.c
 *	Package : Carte actionneur
 *	Description : Asservissement du lanceur de balle
 *  Auteur : Alexis
 *  Version 20130208
 *  Robot : Krusty
 */

#include "KBall_launcher.h"
#ifdef I_AM_ROBOT_KRUSTY

#include "../QS/QS_DCMotor.h"
#include "../QS/QS_CANmsgList.h"

static DCMotor_config_t ball_launcher_config;
static Sint16 count_from_last_tick;

static Sint16 BALLLAUNCHER_get_speed();

//Change la vitesse du lanceur de balle. Attention les vitesses peuvent être négative ou positive.
static void BALLLAUNCHER_set_target_speed(Sint16 tr_min);

void BALLLAUNCHER_init() {
	ball_launcher_config.sensor_read = &BALLLAUNCHER_get_speed;
	ball_launcher_config.Kp = BALLLAUNCHER_ASSER_KP*1024;
	ball_launcher_config.Ki = BALLLAUNCHER_ASSER_KI*1048576;
	ball_launcher_config.Kd = BALLLAUNCHER_ASSER_KD*1024;
	ball_launcher_config.pos[0] = 0;
	ball_launcher_config.pos[1] = (BALLLAUNCHER_TARGET_SPEED/60.0*DCM_TIMER_PERIOD/1000*BALLLAUNCHER_EDGE_PER_ROTATION);	//BALLLAUNCHER_TARGET_SPEED en tr/min
	ball_launcher_config.pwm_number = BALLLAUNCHER_DCMOTOR_PWM_NUM;
	ball_launcher_config.way_latch = (Uint16*)&BALLLAUNCHER_DCMOTOR_PORT_WAY;
	ball_launcher_config.way_bit_number = BALLLAUNCHER_DCMOTOR_PORT_WAY_BIT;
	ball_launcher_config.way0_max_duty = BALLLAUNCHER_DCMOTOR_MAX_PWM_WAY0;
	ball_launcher_config.way1_max_duty = BALLLAUNCHER_DCMOTOR_MAX_PWM_WAY1;
	ball_launcher_config.timeout = 2000; //FIXME: Mettre une valeur appropriée
	DCM_config(BALLLAUNCHER_DCMOTOR_ID, &ball_launcher_config);

	BALLLAUNCHER_HALLSENSOR_INT_PRIORITY = 4;
	BALLLAUNCHER_HALLSENSOR_INT_FLAG = 0;
	BALLLAUNCHER_HALLSENSOR_INT_ENABLE = 1;
}

void BALLLAUNCHER_CAN_process_msg(CAN_msg_t* msg) {
	queue_id_t queueId;
	if(msg->sid == ACT_BALLLAUNCHER) {
		switch(msg->data[0]) {
			case ACT_BALLLAUNCHER_ACTIVATE:
				 queueId = QUEUE_create();
				 assert(queueId != QUEUE_CREATE_FAILED);
				 QUEUE_add(queueId, QUEUE_take_sem, 0, QUEUE_ACT_BallLauncher);
				 QUEUE_add(queueId, &BALLLAUNCHER_run_command, msg->data[1] | (msg->data[2] << 8), QUEUE_ACT_BallLauncher);
				 QUEUE_add(queueId, QUEUE_give_sem, 0, QUEUE_ACT_BallLauncher);
				 break;

			case ACT_BALLLAUNCHER_STOP:
				 queueId = QUEUE_create();
				 assert(queueId != QUEUE_CREATE_FAILED);
				 QUEUE_add(queueId, QUEUE_take_sem, 0, QUEUE_ACT_BallLauncher);
				 QUEUE_add(queueId, &BALLLAUNCHER_run_command, 0, QUEUE_ACT_BallLauncher);
				 QUEUE_add(queueId, QUEUE_give_sem, 0, QUEUE_ACT_BallLauncher);
				 break;

			default:
				debug_printf("BL: invalid CAN msg data[0]=%d !\n", msg->data[0]);
		}
	}
}

void BALLLAUNCHER_run_command(queue_id_t queueId, bool_e init) {
	if(QUEUE_get_act(queueId) == QUEUE_ACT_BallLauncher) {
		if(init == TRUE) {
			//Send command
			Sint16 pos_speed = QUEUE_get_arg(queueId);
			if(pos_speed == 0)
				DCM_goToPos(BALLLAUNCHER_DCMOTOR_ID, 0);
			else if(pos_speed > 0) {
				BALLLAUNCHER_set_target_speed(pos_speed);
				DCM_goToPos(BALLLAUNCHER_DCMOTOR_ID, 1);
			} else debug_printf("BL: pos_speed invalide: %u\n", pos_speed);
			QUEUE_behead(queueId);	//gestion terminée
		}
	}
}

static Sint16 BALLLAUNCHER_get_speed() {
	Sint16 val = count_from_last_tick;
	count_from_last_tick = 0;
	return val;
}

static void BALLLAUNCHER_set_target_speed(Sint16 tr_min) {
	//On doit diviser, sinon le nombre a multiplier est < que 1 (donc c'est 0 pour un entier)
	DCM_setPosValue(BALLLAUNCHER_DCMOTOR_ID, 1, tr_min/(60.0/DCM_TIMER_PERIOD*1000/BALLLAUNCHER_EDGE_PER_ROTATION));
}

void BALLLAUNCHER_HALLSENSOR_INT_ISR() {
	//Pas de test d'overflow, si un overflow se produit, soit le code ne marche pas, soit le moteur tourne a plus de 3,8 millions de tours par minute (ça va vite)
	count_from_last_tick++;
	BALLLAUNCHER_HALLSENSOR_INT_FLAG = 0;
	BALLLAUNCHER_HALLSENSOR_INT_EDGE = !BALLLAUNCHER_HALLSENSOR_INT_EDGE;
}

#endif	//I_AM_ROBOT_KRUSTY
