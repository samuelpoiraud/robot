/*  Club Robot ESEO 2013 - 2014
 *	BIG
 *
 *	Fichier : TEST_SERVO.c
 *	Package : Carte actionneur
 *	Description : Gestion du TEST_SERVO
 *  Auteur : Arnaud
 *  Version 20130219
 *  Robot : BIG
 */
#include "TestServo.h"
#ifdef I_AM_ROBOT_BIG

#include "../QS/QS_CANmsgList.h"
#include "../QS/QS_servo.h"
#include "../act_queue_utils.h"
#include "../selftest.h"

#include "../config/config_debug.h"
#define LOG_PREFIX "TestServo.c : "
#define LOG_COMPONENT OUTPUT_LOG_COMPONENT_TEST_SERVO
#include "../QS/QS_outputlog.h"

#define TIME_BEFORE_COMMAND_VALIDATED	500		//ms

static time32_t last_time_command;


static void TEST_SERVO_command_init(queue_id_t queueId);
static void TEST_SERVO_command_run(queue_id_t queueId);


void TEST_SERVO_init() {
	static bool_e initialized = FALSE;

	if(initialized)
		return;
	initialized = TRUE;

	SERVO_init();
}

void TEST_SERVO_init_pos(){}

void TEST_SERVO_stop(){}

void TEST_SERVO_reset_config(){}


bool_e TEST_SERVO_CAN_process_msg(CAN_msg_t* msg){
	queue_id_t queueId;
	if(msg->sid == ACT_TEST_SERVO){
		switch(msg->data[0]){
			case ACT_TEST_SERVO_IDLE :
				ACTQ_push_operation_from_msg(msg, QUEUE_ACT_Test_Servo, &TEST_SERVO_run_command, 0,TRUE);
				break;

			default:
				component_printf(LOG_LEVEL_Warning, "invalid CAN msg data[0]=%u !\n", msg->data[0]);
		}
		return TRUE;
	}else if(msg->sid == ACT_DO_SELFTEST){
		queueId = QUEUE_create();
		if(queueId != QUEUE_CREATE_FAILED){
			SELFTEST_set_actions(&TEST_SERVO_run_command, 1, (SELFTEST_action_t[]){
									 {ACT_TEST_SERVO_IDLE,   0, QUEUE_ACT_Test_Servo}
								 });
		}
	}

	return FALSE;
}

void TEST_SERVO_run_command(queue_id_t queueId, bool_e init){
	if(QUEUE_has_error(queueId)){
		QUEUE_behead(queueId);
		return;
	}

	if(QUEUE_get_act(queueId) == QUEUE_ACT_Test_Servo){
		if(init)
			TEST_SERVO_command_init(queueId);
		else
			TEST_SERVO_command_run(queueId);
	}
}



//Initialise une commande
static void TEST_SERVO_command_init(queue_id_t queueId){
	Uint8 command = QUEUE_get_arg(queueId)->canCommand;
	Uint16* goalPosition = &QUEUE_get_arg(queueId)->param;

	*goalPosition = 0xFFFF;

	switch(command){
		case ACT_TEST_SERVO_IDLE :  *goalPosition = 0; break;
		case ACT_TEST_SERVO_STATE_1 :  *goalPosition = 1000; break;
		case ACT_TEST_SERVO_STATE_2 :  *goalPosition = 2000; break;

		case ACT_TEST_SERVO_STOP :
			// TODO : couper l'asservissement d'un servo (Ajouter contrôle de l'alimentation des servos via transistor ?)
			QUEUE_next(queueId, ACT_TEST_SERVO, ACT_RESULT_DONE, ACT_RESULT_ERROR_OK, __LINE__);
			return;

		default:
			error_printf("Invalid command: %u, code is broken !\n", command);
			QUEUE_next(queueId, ACT_TEST_SERVO, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_LOGIC, __LINE__);
			return;
	}

	if(*goalPosition == 0xFFFF){
		error_printf("Invalid position for command: %u, code is broken !\n", command);
		QUEUE_next(queueId, ACT_TEST_SERVO, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_LOGIC, __LINE__);
		return;
	}

	debug_printf("Move TEST_SERVO to %d\n", *goalPosition);

	SERVO_set_cmd(*goalPosition, TEST_SERVO_ID);

	last_time_command = global.absolute_time;

}

//Attente d'un temps donné avant de rendre la main sur le servo-moteur
static void TEST_SERVO_command_run(queue_id_t queueId){

	if(global.absolute_time - last_time_command > TIME_BEFORE_COMMAND_VALIDATED)
		QUEUE_next(queueId, ACT_TEST_SERVO, ACT_RESULT_DONE, ACT_RESULT_ERROR_OK, 0x0000);
}

#endif  /* I_AM_ROBOT_BIG */
