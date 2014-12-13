/*  Club Robot ESEO 2014 - 2015
 *
 *	Fichier : spot_pompe_right.c
 *	Package : Carte actionneur
 *	Description : Gestion de la SPOT_POMPE_RIGHT pour attraper les spots arrière droite
 *  Auteur : Arnaud
 *  Version 20130219
 *  Robot : HOLLY
 */

#include "spot_pompe_right.h"

#ifdef I_AM_ROBOT_BIG

#include "../QS/QS_CANmsgList.h"
#include "../QS/QS_ax12.h"
#include "../QS/QS_pwm.h"
#include "../QS/QS_ports.h"
#include "../QS/QS_can.h"
#include "../act_queue_utils.h"
#include "../selftest.h"

#define LOG_PREFIX "spot_pompe_left : "
#define LOG_COMPONENT OUTPUT_LOG_COMPONENT_SPOT_POMPE_LEFT
#include "../QS/QS_outputlog.h"

static void SPOT_POMPE_LEFT_initDCM();
static void SPOT_POMPE_LEFT_command_init(queue_id_t queueId);
static void SPOT_POMPE_LEFT_command_run(queue_id_t queueId);
static void SPOT_POMPE_LEFT_do_order(Uint8 command, Uint8 param);
static void SPOT_POMPE_LEFT_run_command(queue_id_t queueId, bool_e init);

void SPOT_POMPE_LEFT_init() {
	static bool_e initialized = FALSE;

	if(initialized)
		return;
	initialized = TRUE;

	SPOT_POMPE_LEFT_initDCM();
}

void SPOT_POMPE_LEFT_reset_config(){}

// Initialisation du moteur, si init ne fait rien
static void SPOT_POMPE_LEFT_initDCM() {
	static bool_e initialized = FALSE;

	if(initialized)
		return;
	initialized = TRUE;

	PORTS_pwm_init();
	PWM_init();
	PWM_stop(SPOT_POMPE_LEFT_PWM_NUM);

	info_printf("initialisée (DCM) \n");
}

void SPOT_POMPE_LEFT_stop() {
	PWM_stop(SPOT_POMPE_LEFT_PWM_NUM);
}

void SPOT_POMPE_LEFT_init_pos(){
	PWM_stop(SPOT_POMPE_LEFT_PWM_NUM);
}

bool_e SPOT_POMPE_LEFT_CAN_process_msg(CAN_msg_t* msg) {
	if(msg->sid == ACT_SPOT_POMPE_LEFT) {
		SPOT_POMPE_LEFT_initDCM();
		switch(msg->data[0]) {
			case ACT_SPOT_POMPE_LEFT_NORMAL:
			case ACT_SPOT_POMPE_LEFT_REVERSE:
			case ACT_SPOT_POMPE_LEFT_STOP:
				ACTQ_push_operation_from_msg(msg, QUEUE_ACT_SPOT_POMPE_LEFT, &SPOT_POMPE_LEFT_run_command, msg->data[1],TRUE);
				break;

			default:
				component_printf(LOG_LEVEL_Warning, "invalid CAN msg data[0]=%u !\n", msg->data[0]);
		}
		return TRUE;
	}else if(msg->sid == ACT_DO_SELFTEST) {
		SELFTEST_set_actions(&SPOT_POMPE_LEFT_run_command, 3, (SELFTEST_action_t[]){
								 {ACT_SPOT_POMPE_LEFT_NORMAL,  100, QUEUE_ACT_SPOT_POMPE_LEFT},
								 {ACT_SPOT_POMPE_LEFT_REVERSE, 100, QUEUE_ACT_SPOT_POMPE_LEFT},
								 {ACT_SPOT_POMPE_LEFT_STOP, 0, QUEUE_ACT_SPOT_POMPE_LEFT}
							 });
	}

	return FALSE;
}

static void SPOT_POMPE_LEFT_run_command(queue_id_t queueId, bool_e init) {
	if(QUEUE_has_error(queueId)) {
		QUEUE_behead(queueId);
		return;
	}

	if(QUEUE_get_act(queueId) == QUEUE_ACT_SPOT_POMPE_LEFT) {
			if(init)
				SPOT_POMPE_LEFT_command_init(queueId);
			else
				SPOT_POMPE_LEFT_command_run(queueId);
	}
}

//Initialise une commande
static void SPOT_POMPE_LEFT_command_init(queue_id_t queueId) {
	Uint8 command = QUEUE_get_arg(queueId)->canCommand;
	Uint8 param = QUEUE_get_arg(queueId)->param;

	switch(command) {
		case ACT_SPOT_POMPE_LEFT_NORMAL:
		case ACT_SPOT_POMPE_LEFT_REVERSE:
			SPOT_POMPE_LEFT_do_order(command, param);
			break;

		case ACT_SPOT_POMPE_LEFT_STOP:
			PWM_stop(SPOT_POMPE_LEFT_PWM_NUM);
			return;

		default: {
			error_printf("Invalid SPOT_POMPE_LEFT command: %u, code is broken !\n", command);
			QUEUE_next(queueId, ACT_SPOT_POMPE_LEFT, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_LOGIC, __LINE__);
			return;
		}
	}
}

static void SPOT_POMPE_LEFT_command_run(queue_id_t queueId){
	QUEUE_next(queueId, ACT_SPOT_POMPE_LEFT, ACT_RESULT_DONE, ACT_RESULT_ERROR_OK, 0);
}

static void SPOT_POMPE_LEFT_do_order(Uint8 command, Uint8 param){
	if(command == ACT_SPOT_POMPE_LEFT_NORMAL)
		GPIO_ResetBits(SPOT_POMPE_LEFT_SENS);
	else if(command == ACT_SPOT_POMPE_LEFT_REVERSE)
		GPIO_SetBits(SPOT_POMPE_LEFT_SENS);
	else{
		debug_printf("commande envoyée à SPOT_POMPE_LEFT_do_order inconnue -> %d	%x\n", command, command);
		PWM_stop(SPOT_POMPE_LEFT_PWM_NUM);
		return;
	}

	param = (param > 100) ? 100 : param;
	PWM_run(param, SPOT_POMPE_LEFT_PWM_NUM);
}

#endif
