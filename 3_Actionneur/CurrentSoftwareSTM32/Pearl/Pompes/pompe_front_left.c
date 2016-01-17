/*  Club Robot ESEO 2015 - 2016
 *
 *	Fichier : pompe_front_left.c
 *	Package : Carte actionneur
 *	Description : Gestion de la pompe avant gauche permettant de ventouser les blocs de sable ou un coquillage
 *  Auteur : Valentin
 *  Version 2016
 *  Robot : SMALL
 */

#include "pompe_front_left.h"

// Exemple d'une pompe

// Ajout l'actionneur dans QS_CANmsgList.h
// Ajout d'une valeur dans l'�num�ration de la queue dans config_(big/small)/config_global_vars_types.h
// Formatage : QUEUE_ACT_POMPE_FRONT_LEFT
// Ajout de la d�claration de l'actionneur dans ActManager dans le tableau actionneurs
// Ajout de la verbosit� dans le fichier act_queue_utils.c dans la fonction ACTQ_internal_printResult
// Un define EXEMPLE_PIN doit avoir �t� ajout� au fichier config_big/config_pin.h // config_small/config_pin.h
// Ajout des postions dans QS_types.h dans l'�num ACT_order_e (avec "ACT_" et sans "_POS" � la fin)

// Optionnel:
// Ajout du selftest dans le fichier selftest.c dans la fonction SELFTEST_done_test
// Ajout du selftest dans le fichier QS_CANmsgList (dans l'�num�ration SELFTEST)

// En strat�gie
// ajout d'une d'une valeur dans le tableau act_link_SID_Queue du fichier act_functions.c/h
// ajout des fonctions actionneurs dans act_avoidance.c/h si l'actionneur modifie l'�vitement du robot

// En strat�gie Optionnel
// ajout du verbose du selftest dans Supervision/Selftest.c (tableau SELFTEST_getError_string, fonction SELFTEST_print_errors)
// ajout de la verbosit� dans Supervision/Verbose_can_msg.c/h (fonction VERBOSE_CAN_MSG_sprint)



#define POMPE_FRONT_LEFT_PIN 0,0

#ifdef I_AM_ROBOT_SMALL

#include "../QS/QS_CANmsgList.h"
#include "../QS/QS_ax12.h"
#include "../QS/QS_pwm.h"
#include "../QS/QS_ports.h"
#include "../QS/QS_can.h"
#include "../act_queue_utils.h"
#include "../selftest.h"

#define LOG_PREFIX "pompe_front_left : "
#define LOG_COMPONENT OUTPUT_LOG_COMPONENT_POMPE_FRONT_LEFT
#include "../QS/QS_outputlog.h"

static void POMPE_FRONT_LEFT_command_init(queue_id_t queueId);
static void POMPE_FRONT_LEFT_command_run(queue_id_t queueId);
static void POMPE_FRONT_LEFT_do_order(Uint8 command);

void POMPE_FRONT_LEFT_init() {
	static bool_e initialized = FALSE;

	if(initialized)
		return;
	initialized = TRUE;

}

void POMPE_FRONT_LEFT_reset_config(){}


void POMPE_FRONT_LEFT_stop() {
	GPIO_ResetBits(POMPE_FRONT_LEFT_PIN);
}

void POMPE_FRONT_LEFT_init_pos(){
	GPIO_ResetBits(POMPE_FRONT_LEFT_PIN);
}

bool_e POMPE_FRONT_LEFT_CAN_process_msg(CAN_msg_t* msg) {
	if(msg->sid == ACT_POMPE_FRONT_LEFT) {
		switch(msg->data.act_msg.order) {
			case ACT_POMPE_FRONT_LEFT_NORMAL:
			case ACT_POMPE_FRONT_LEFT_STOP:
				ACTQ_push_operation_from_msg(msg, QUEUE_ACT_POMPE_FRONT_LEFT, &POMPE_FRONT_LEFT_run_command, msg->data.act_msg.act_data.act_optionnal_data[0], TRUE);
				break;

			default:
				component_printf(LOG_LEVEL_Warning, "invalid CAN msg data[0]=%u !\n", msg->data.act_msg.order);
		}
		return TRUE;
	}else if(msg->sid == ACT_DO_SELFTEST) {
		/*SELFTEST_set_actions(&POMPE_FRONT_LEFT_run_command, 3, (SELFTEST_action_t[]){
								 {ACT_POMPE_FRONT_LEFT_NORMAL,  0, QUEUE_ACT_POMPE_FRONT_LEFT},
								 {ACT_POMPE_FRONT_LEFT_REVERSE, 0, QUEUE_ACT_POMPE_FRONT_LEFT},
								 {ACT_POMPE_FRONT_LEFT_STOP, 0, QUEUE_ACT_POMPE_FRONT_LEFT}
							 });*/
	}

	return FALSE;
}

void POMPE_FRONT_LEFT_run_command(queue_id_t queueId, bool_e init) {
	if(QUEUE_has_error(queueId)) {
		QUEUE_behead(queueId);
		return;
	}

	if(QUEUE_get_act(queueId) == QUEUE_ACT_POMPE_FRONT_LEFT) {
			if(init)
				POMPE_FRONT_LEFT_command_init(queueId);
			else
				POMPE_FRONT_LEFT_command_run(queueId);
	}
}

//Initialise une commande
static void POMPE_FRONT_LEFT_command_init(queue_id_t queueId) {
	Uint8 command = QUEUE_get_arg(queueId)->canCommand;

	switch(command) {
		case ACT_POMPE_FRONT_LEFT_NORMAL:
		case ACT_POMPE_FRONT_LEFT_REVERSE:
			POMPE_FRONT_LEFT_do_order(command);
			break;

		case ACT_POMPE_FRONT_LEFT_STOP:
			GPIO_ResetBits(POMPE_FRONT_LEFT_PIN);
			return;

		default: {
			error_printf("Invalid POMPE_FRONT_LEFT command: %u, code is broken !\n", command);
			QUEUE_next(queueId, ACT_POMPE_FRONT_LEFT, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_LOGIC, __LINE__);
			return;
		}
	}
}

static void POMPE_FRONT_LEFT_command_run(queue_id_t queueId){
	QUEUE_next(queueId, ACT_POMPE_FRONT_LEFT, ACT_RESULT_DONE, ACT_RESULT_ERROR_OK, 0);
}

static void POMPE_FRONT_LEFT_do_order(Uint8 command){
	if(command == ACT_POMPE_FRONT_LEFT_NORMAL)
		GPIO_SetBits(POMPE_FRONT_LEFT_PIN);
	else if(command == ACT_POMPE_FRONT_LEFT_REVERSE)
		GPIO_ResetBits(POMPE_FRONT_LEFT_PIN);
	else{
		debug_printf("commande envoy�e � POMPE_FRONT_LEFT_do_order inconnue -> %d	0x%x\n", command, command);
		GPIO_ResetBits(POMPE_FRONT_LEFT_PIN);
		return;
	}
}

#endif


