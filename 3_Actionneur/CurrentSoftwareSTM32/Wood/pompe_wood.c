/*  Club Robot ESEO 2014 - 2015
 *
 *	Fichier : spot_pompe_right.c
 *	Package : Carte actionneur
 *	Description : Gestion de la pompe pour attraper les spots arrière droite
 *  Auteur : Arnaud
 *  Version 20130219
 *  Robot : HOLLY
 */

#include "pompe_wood.h"

#ifdef I_AM_ROBOT_SMALL

#include "../QS/QS_CANmsgList.h"
#include "../QS/QS_ax12.h"
#include "../QS/QS_pwm.h"
#include "../QS/QS_ports.h"
#include "../QS/QS_can.h"
#include "../act_queue_utils.h"
#include "../selftest.h"

#define LOG_PREFIX "pompe_wood : "
#define LOG_COMPONENT OUTPUT_LOG_COMPONENT_POMPE_WOOD
#include "../QS/QS_outputlog.h"

static void POMPE_WOOD_initDCM();
static void POMPE_WOOD_command_init(queue_id_t queueId);
static void POMPE_WOOD_command_run(queue_id_t queueId);
static void POMPE_WOOD_do_order(Uint8 command, Uint8 param);
static void POMPE_WOOD_run_command(queue_id_t queueId, bool_e init);

void POMPE_WOOD_init() {
	static bool_e initialized = FALSE;

	if(initialized)
		return;
	initialized = TRUE;

	POMPE_WOOD_initDCM();
}

void POMPE_WOOD_reset_config(){}

// Initialisation du moteur, si init ne fait rien
static void POMPE_WOOD_initDCM() {
	static bool_e initialized = FALSE;

	if(initialized)
		return;
	initialized = TRUE;

	PORTS_pwm_init();
	PWM_init();
	PWM_stop(POMPE_WOOD_PWM_NUM);

	info_printf("initialisée (DCM) \n");
}

void POMPE_WOOD_stop() {
	PWM_stop(POMPE_WOOD_PWM_NUM);
}

void POMPE_WOOD_init_pos(){
	PWM_stop(POMPE_WOOD_PWM_NUM);
}

bool_e POMPE_WOOD_CAN_process_msg(CAN_msg_t* msg) {
	if(msg->sid == ACT_POMPE_WOOD) {
		POMPE_WOOD_initDCM();
		switch(msg->data.act_msg.order) {
			case ACT_POMPE_WOOD_NORMAL:
			case ACT_POMPE_WOOD_REVERSE:
			case ACT_POMPE_WOOD_STOP:
				ACTQ_push_operation_from_msg(msg, QUEUE_ACT_POMPE_WOOD, &POMPE_WOOD_run_command, msg->data.act_msg.act_data.act_optionnal_data[0],TRUE);
				break;

			default:
				component_printf(LOG_LEVEL_Warning, "invalid CAN msg data[0]=%u !\n", msg->data.act_msg.order);
		}
		return TRUE;
	}else if(msg->sid == ACT_DO_SELFTEST) {
		SELFTEST_set_actions(&POMPE_WOOD_run_command, 27, 3, (SELFTEST_action_t[]){
								 {ACT_POMPE_WOOD_NORMAL,  100, QUEUE_ACT_POMPE_WOOD},
								 {ACT_POMPE_WOOD_REVERSE, 100, QUEUE_ACT_POMPE_WOOD},
								 {ACT_POMPE_WOOD_STOP, 0, QUEUE_ACT_POMPE_WOOD}
							 });
	}

	return FALSE;
}

static void POMPE_WOOD_run_command(queue_id_t queueId, bool_e init) {
	if(QUEUE_has_error(queueId)) {
		QUEUE_behead(queueId);
		return;
	}

	if(QUEUE_get_act(queueId) == QUEUE_ACT_POMPE_WOOD) {
			if(init)
				POMPE_WOOD_command_init(queueId);
			else
				POMPE_WOOD_command_run(queueId);
	}
}

//Initialise une commande
static void POMPE_WOOD_command_init(queue_id_t queueId) {
	Uint8 command = QUEUE_get_arg(queueId)->canCommand;
	Uint8 param = QUEUE_get_arg(queueId)->param;

	switch(command) {
		case ACT_POMPE_WOOD_NORMAL:
		case ACT_POMPE_WOOD_REVERSE:
			POMPE_WOOD_do_order(command, param);
			break;

		case ACT_POMPE_WOOD_STOP:
			PWM_stop(POMPE_WOOD_PWM_NUM);
			return;

		default: {
			error_printf("Invalid POMPE_WOOD command: %u, code is broken !\n", command);
			QUEUE_next(queueId, ACT_POMPE_WOOD, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_LOGIC, __LINE__);
			return;
		}
	}
}

static void POMPE_WOOD_command_run(queue_id_t queueId){
	QUEUE_next(queueId, ACT_POMPE_WOOD, ACT_RESULT_DONE, ACT_RESULT_ERROR_OK, 0);
}

static void POMPE_WOOD_do_order(Uint8 command, Uint8 param){
	if(command == ACT_POMPE_WOOD_NORMAL)
		GPIO_ResetBits(POMPE_WOOD_SENS);
	else if(command == ACT_POMPE_WOOD_REVERSE)
		GPIO_SetBits(POMPE_WOOD_SENS);
	else{
		debug_printf("commande envoyée à POMPE_WOOD_do_order inconnue -> %d	%x\n", command, command);
		PWM_stop(POMPE_WOOD_PWM_NUM);
		return;
	}

	param = (param > 100) ? 100 : param;
	PWM_run(param, POMPE_WOOD_PWM_NUM);
}

#endif
