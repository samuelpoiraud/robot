/*  Club Robot ESEO 2015 - 2016
 *
 *	Fichier : pompe_pendulum.c
 *	Package : Carte actionneur
 *	Description : Gestion de la pompe PENDULUM
 *  Auteur : Valentin
 *  Version 2016
 *  Robot : BIG
 */

#include "pompe_pendulum.h"


#define POMPE_PENDULUM_PIN 0,0

#ifdef I_AM_ROBOT_BIG

#include "../QS/QS_CANmsgList.h"
#include "../QS/QS_ax12.h"
#include "../QS/QS_pwm.h"
#include "../QS/QS_ports.h"
#include "../QS/QS_can.h"
#include "../act_queue_utils.h"
#include "../selftest.h"

#define LOG_PREFIX "pompe_pendulum : "
#define LOG_COMPONENT OUTPUT_LOG_COMPONENT_POMPE_PENDULUM
#include "../QS/QS_outputlog.h"

static void POMPE_PENDULUM_command_init(queue_id_t queueId);
static void POMPE_PENDULUM_command_run(queue_id_t queueId);
static void POMPE_PENDULUM_do_order(Uint8 command);

void POMPE_PENDULUM_init() {
	static bool_e initialized = FALSE;

	if(initialized)
		return;
	initialized = TRUE;
}

void POMPE_PENDULUM_reset_config(){}


void POMPE_PENDULUM_stop() {
	GPIO_ResetBits(POMPE_PENDULUM_PIN);
}

void POMPE_PENDULUM_init_pos(){
	GPIO_ResetBits(POMPE_PENDULUM_PIN);
}

bool_e POMPE_PENDULUM_CAN_process_msg(CAN_msg_t* msg) {
	if(msg->sid == ACT_POMPE_PENDULUM) {
		switch(msg->data.act_msg.order) {
			case ACT_POMPE_PENDULUM_NORMAL:
			case ACT_POMPE_PENDULUM_STOP:
				ACTQ_push_operation_from_msg(msg, QUEUE_ACT_POMPE_PENDULUM, &POMPE_PENDULUM_run_command, msg->data.act_msg.act_data.act_optionnal_data[0], TRUE);
				break;

			default:
				component_printf(LOG_LEVEL_Warning, "invalid CAN msg data[0]=%u !\n", msg->data.act_msg.order);
		}
		return TRUE;
	}else if(msg->sid == ACT_DO_SELFTEST) {
		/*SELFTEST_set_actions(&POMPE_PENDULUM_run_command,  2, (SELFTEST_action_t[]){
								 {ACT_POMPE_PENDULUM_NORMAL,  100, QUEUE_ACT_POMPE_PENDULUM},
								 {ACT_POMPE_PENDULUM_STOP, 0, QUEUE_ACT_POMPE_PENDULUM}
							 });*/
	}

	return FALSE;
}

void POMPE_PENDULUM_run_command(queue_id_t queueId, bool_e init) {
	if(QUEUE_has_error(queueId)) {
		QUEUE_behead(queueId);
		return;
	}

	if(QUEUE_get_act(queueId) == QUEUE_ACT_POMPE_PENDULUM) {
			if(init)
				POMPE_PENDULUM_command_init(queueId);
			else
				POMPE_PENDULUM_command_run(queueId);
	}
}

//Initialise une commande
static void POMPE_PENDULUM_command_init(queue_id_t queueId) {
	Uint8 command = QUEUE_get_arg(queueId)->canCommand;

	switch(command) {
		case ACT_POMPE_PENDULUM_NORMAL:
			POMPE_PENDULUM_do_order(command);
			break;

		case ACT_POMPE_PENDULUM_STOP:
			GPIO_ResetBits(POMPE_PENDULUM_PIN);
			return;

		default: {
			error_printf("Invalid POMPE_PENDULUM command: %u, code is broken !\n", command);
			QUEUE_next(queueId, ACT_POMPE_PENDULUM, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_LOGIC, __LINE__);
			return;
		}
	}
}

static void POMPE_PENDULUM_command_run(queue_id_t queueId){
	QUEUE_next(queueId, ACT_POMPE_PENDULUM, ACT_RESULT_DONE, ACT_RESULT_ERROR_OK, 0);
}

static void POMPE_PENDULUM_do_order(Uint8 command){
	if(command == ACT_POMPE_PENDULUM_NORMAL)
		GPIO_SetBits(POMPE_PENDULUM_PIN);
	else{
		debug_printf("commande envoy�e � POMPE_PENDULUM_do_order inconnue -> %d	0x%x\n", command, command);
		GPIO_ResetBits(POMPE_PENDULUM_PIN);
		return;
	}
}

#endif


