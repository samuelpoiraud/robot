/*  Club Robot ESEO 2013 - 2014
 *
 *	Fichier : pompe.c
 *	Package : Carte actionneur
 *	Description : Gestion de la pompe
 *  Auteur : Arnaud
 *  Version 20130219
 *  Robot : Les deux
 */

#include "Pompe.h"
#include "../QS/QS_CANmsgList.h"
#include "../QS/QS_ax12.h"
#include "../QS/QS_pwm.h"
#include "../QS/QS_ports.h"
#include "../act_queue_utils.h"
#include "../selftest.h"
#include "config_pin.h"
#include "Pompe_config.h"

#include "config_debug.h"
#define LOG_PREFIX "FR: "
#define LOG_COMPONENT OUTPUT_LOG_COMPONENT_POMPE
#include "../QS/QS_outputlog.h"


static void POMPE_initDCM();
static void POMPE_command_init(queue_id_t queueId);
static void POMPE_command_run(queue_id_t queueId);
static void POMPE_do_order(Uint8 command, Uint8 param);

void POMPE_init() {
	static bool_e initialized = FALSE;

	if(initialized)
		return;
	initialized = TRUE;

	POMPE_initDCM();
	info_printf("Pompe initialisée\n");
}

// Initialisation du moteur, si init ne fait rien
static void POMPE_initDCM() {
	static bool_e initialized = FALSE;

	if(initialized && !global.alim)
		return;
	initialized = TRUE;

	PORTS_pwm_init();
	PWM_stop(POMPE_PWM_NUM);

	info_printf("VERIN POMPE initialisé (pompe) \n");
}

void POMPE_stop() {
	PWM_stop(POMPE_PWM_NUM);
}

void POMPE_init_pos(){
	PWM_stop(POMPE_PWM_NUM);
}

bool_e POMPE_CAN_process_msg(CAN_msg_t* msg) {

	if(msg->sid == ACT_POMPE) {
		switch(msg->data[0]) {
			case ACT_POMPE_NORMAL:
			case ACT_POMPE_REVERSE:
			case ACT_POMPE_STOP:
				ACTQ_push_operation_from_msg(msg, QUEUE_ACT_Pompe, &POMPE_run_command, msg->data[1]);
				break;

			default:
				component_printf(LOG_LEVEL_Warning, "invalid CAN msg data[0]=%u !\n", msg->data[0]);
		}
		return TRUE;
	}else if(msg->sid == ACT_DO_SELFTEST) {
		SELFTEST_set_actions(&POMPE_run_command, 3, (SELFTEST_action_t[]){
								 {ACT_POMPE_NORMAL,  100, QUEUE_ACT_Pompe},
								 {ACT_POMPE_REVERSE, 100, QUEUE_ACT_Pompe},
								 {ACT_POMPE_STOP, 0, QUEUE_ACT_Pompe}
							 });
	}

	return FALSE;
}

void POMPE_run_command(queue_id_t queueId, bool_e init) {
	if(QUEUE_has_error(queueId)) {
		QUEUE_behead(queueId);
		return;
	}

	if(QUEUE_get_act(queueId) == QUEUE_ACT_Pompe) {
			if(init)
				POMPE_command_init(queueId);
			else
				POMPE_command_run(queueId);
	}
}

//Initialise une commande
static void POMPE_command_init(queue_id_t queueId) {
	Uint8 command = QUEUE_get_arg(queueId)->canCommand;
	Uint8 param = QUEUE_get_arg(queueId)->param;

	switch(command) {
		case ACT_POMPE_NORMAL:
		case ACT_POMPE_REVERSE:
			POMPE_do_order(command, param);
			break;

		case ACT_POMPE_STOP:
			PWM_stop(POMPE_PWM_NUM);
			return;

		default: {
			error_printf("Invalid pompe command: %u, code is broken !\n", command);
			QUEUE_next(queueId, ACT_POMPE, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_LOGIC, __LINE__);
			return;
		}
	}
}

static void POMPE_command_run(queue_id_t queueId){
	QUEUE_next(queueId, ACT_POMPE, ACT_RESULT_DONE, ACT_RESULT_ERROR_OK, 0);
}

static void POMPE_do_order(Uint8 command, Uint8 param){
	if(command == ACT_POMPE_NORMAL)
		POMPE_SENS = 1;
	else if(command == ACT_POMPE_REVERSE)
		POMPE_SENS = 0;
	else{
		debug_printf("commande envoyée à POMPE_do_order inconnue -> %d	%x", command, command);
		PWM_stop(POMPE_PWM_NUM);
		return;
	}

	param = (param > 100) ? 100 : param;
	PWM_run(param, POMPE_PWM_NUM);
}
