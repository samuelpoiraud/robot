/*  Club Robot ESEO 2016 - 2017
*
*	Fichier : small_pompe_dispose.c
*	Package : Carte actionneur
*	Description : Gestion de la pompe small_pompe_dispose
*	Auteur :
*	Version 2017
*	Robot : BIG
*/

#include "small_pompe_dispose.h"

#ifdef I_AM_ROBOT_SMALL

#include "../QS/QS_CANmsgList.h"
#include "../QS/QS_ax12.h"
#include "../QS/QS_pwm.h"
#include "../QS/QS_ports.h"
#include "../QS/QS_can.h"
#include "../act_queue_utils.h"
#include "../selftest.h"

#define LOG_PREFIX "small_pompe_dispose.c: "
#define LOG_COMPONENT OUTPUT_LOG_COMPONENT_SMALL_POMPE_DISPOSE
#include "../QS/QS_outputlog.h"

static void SMALL_POMPE_DISPOSE_init_pwm();
static void SMALL_POMPE_DISPOSE_command_init(queue_id_t queueId);
static void SMALL_POMPE_DISPOSE_command_run(queue_id_t queueId);
static void SMALL_POMPE_DISPOSE_do_order(Uint8 command, Uint8 param);

void SMALL_POMPE_DISPOSE_init() {
	static bool_e initialized = FALSE;

	SMALL_POMPE_DISPOSE_init_pwm();

	if(initialized)
		return;
	initialized = TRUE;
}

void SMALL_POMPE_DISPOSE_reset_config(){}

// Initialisation du moteur, si init ne fait rien
static void SMALL_POMPE_DISPOSE_init_pwm() {
	PORTS_pwm_init();
	PWM_init();
	PWM_stop(SMALL_POMPE_DISPOSE_PWM_NUM);

	info_printf("Pompe SMALL_POMPE_DISPOSE initialisée\n");
}

void SMALL_POMPE_DISPOSE_stop() {
	PWM_stop(SMALL_POMPE_DISPOSE_PWM_NUM);
}

void SMALL_POMPE_DISPOSE_init_pos(){
	PWM_stop(SMALL_POMPE_DISPOSE_PWM_NUM);
}

bool_e SMALL_POMPE_DISPOSE_CAN_process_msg(CAN_msg_t* msg) {
	if(msg->sid == ACT_SMALL_POMPE_DISPOSE) {
		SMALL_POMPE_DISPOSE_initDCM();
		switch(msg->data.act_msg.order) {
			case ACT_POMPE_NORMAL:
			case ACT_POMPE_REVERSE:
			case ACT_POMPE_STOP:
				ACTQ_push_operation_from_msg(msg, QUEUE_ACT_SMALL_POMPE_DISPOSE, &SMALL_POMPE_DISPOSE_run_command, msg->data.act_msg.act_data.act_optionnal_data[0],TRUE);
				break;

			default:
				component_printf(LOG_LEVEL_Warning, "invalid CAN msg data[0]=%u !\n", msg->data.act_msg.order);
		}
		return TRUE;
	}else if(msg->sid == ACT_DO_SELFTEST) {

	}

	return FALSE;
}

void SMALL_POMPE_DISPOSE_run_command(queue_id_t queueId, bool_e init) {
	if(QUEUE_has_error(queueId)) {
		QUEUE_behead(queueId);
		return;
	}

	if(QUEUE_get_act(queueId) == QUEUE_ACT_SMALL_POMPE_DISPOSE) {
			if(init)
				SMALL_POMPE_DISPOSE_command_init(queueId);
			else
				SMALL_POMPE_DISPOSE_command_run(queueId);
	}
}

//Initialise une commande
static void SMALL_POMPE_DISPOSE_command_init(queue_id_t queueId) {
	Uint8 command = QUEUE_get_arg(queueId)->canCommand;
	Uint8 param = QUEUE_get_arg(queueId)->param;

	switch(command) {
		case ACT_POMPE_NORMAL:
		case ACT_POMPE_REVERSE:
			SMALL_POMPE_DISPOSE_do_order(command, param);
			break;

		case ACT_POMPE_STOP:
			PWM_stop(SMALL_POMPE_DISPOSE_PWM_NUM);
			return;

		default: {
			error_printf("Invalid SMALL_POMPE_DISPOSE command: %u, code is broken !\n", command);
			QUEUE_next(queueId, ACT_SMALL_POMPE_DISPOSE, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_LOGIC, __LINE__);
			return;
		}
	}
}

static void SMALL_POMPE_DISPOSE_command_run(queue_id_t queueId){
	QUEUE_next(queueId, ACT_SMALL_POMPE_DISPOSE, ACT_RESULT_DONE, ACT_RESULT_ERROR_OK, 0);
}

static void SMALL_POMPE_DISPOSE_do_order(Uint8 command, Uint8 param){
	if(command == ACT_POMPE_NORMAL)
		GPIO_SetBits(SMALL_POMPE_DISPOSE_SENS);
	else if(command == ACT_POMPE_REVERSE)
		GPIO_ResetBits(SMALL_POMPE_DISPOSE_SENS);
	else{
		debug_printf("commande envoyée à SMALL_POMPE_DISPOSE_do_order inconnue -> %d	%x\n", command, command);
		PWM_stop(SMALL_POMPE_DISPOSE_PWM_NUM);
		return;
	}

	param = (param > 100) ? 100 : param;
	PWM_run(param, SMALL_POMPE_DISPOSE_PWM_NUM);
}

#endif
