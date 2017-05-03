/*  Club Robot ESEO 2016 - 2017
*
*	Fichier : small_pompe_prise.c
*	Package : Carte actionneur
*	Description : Gestion de la pompe SMALL_POMPE_PRISE
*	Auteur :
*	Version 2017
*	Robot : BIG
*/

#include "small_pompe_prise.h"

// Exemple d'une pompe

// Ajout l'actionneur dans QS_CANmsgList.h
// Ajout d'une valeur dans l'énumération de la queue dans config_(big/small)/config_global_vars_types.h
// Formatage : QUEUE_ACT_SMALL_POMPE_PRISE
// Ajout de la déclaration de l'actionneur dans ActManager dans le tableau actionneurs
// Ajout de la verbosité dans le fichier act_queue_utils.c dans la fonction ACTQ_internal_printResult
// Un define EXEMPLE_PIN doit avoir été ajouté au fichier config_big/config_pin.h // config_small/config_pin.h
// Ajout des postions dans QS_types.h dans l'énum ACT_order_e (avec "ACT_" et sans "_POS" à la fin)
// Mise à jour de config/config_debug.h

// Optionnel:
// Ajout du selftest dans le fichier selftest.c dans la fonction SELFTEST_done_test
// Ajout du selftest dans le fichier QS_CANmsgList (dans l'énumération SELFTEST)

// En stratégie
// ajout d'une d'une valeur dans le tableau act_link_SID_Queue du fichier act_functions.c/h
// ajout des fonctions actionneurs dans act_avoidance.c/h si l'actionneur modifie l'évitement du robot

// En stratégie Optionnel
// ajout du verbose du selftest dans Supervision/Selftest.c (tableau SELFTEST_getError_string, fonction SELFTEST_print_errors)
// ajout de la verbosité dans Supervision/Verbose_can_msg.c/h (fonction VERBOSE_CAN_MSG_sprint)

#ifdef I_AM_ROBOT_SMALL

#include "../QS/QS_CANmsgList.h"
#include "../QS/QS_ax12.h"
#include "../QS/QS_pwm.h"
#include "../QS/QS_ports.h"
#include "../QS/QS_can.h"
#include "../act_queue_utils.h"
#include "../selftest.h"

#define LOG_PREFIX "small_pompe_prise.c : "
#define LOG_COMPONENT OUTPUT_LOG_COMPONENT_SMALL_POMPE_PRISE
#include "../QS/QS_outputlog.h"

static void SMALL_POMPE_PRISE_init_pwm();
static void SMALL_POMPE_PRISE_command_init(queue_id_t queueId);
static void SMALL_POMPE_PRISE_command_run(queue_id_t queueId);
static void SMALL_POMPE_PRISE_do_order(Uint8 command, Uint8 param);

void SMALL_POMPE_PRISE_init() {
	static bool_e initialized = FALSE;

	SMALL_POMPE_PRISE_init_pwm();

	if(initialized)
			return;
	initialized = TRUE;
}

void SMALL_POMPE_PRISE_reset_config(){}

// Initialisation du moteur, si init ne fait rien
static void SMALL_POMPE_PRISE_init_pwm() {
	PORTS_pwm_init();
	PWM_init();
	PWM_stop(SMALL_POMPE_PRISE_PWM_NUM);

	info_printf("Pompe SMALL_POMPE_PRISE initialisée\n");
}

void SMALL_POMPE_PRISE_stop() {
	PWM_stop(SMALL_POMPE_PRISE_PWM_NUM);
}

void SMALL_POMPE_PRISE_init_pos(){
	PWM_stop(SMALL_POMPE_PRISE_PWM_NUM);
}

bool_e SMALL_POMPE_PRISE_CAN_process_msg(CAN_msg_t* msg) {
	if(msg->sid == ACT_SMALL_POMPE_PRISE) {
		switch(msg->data.act_msg.order) {
			case ACT_POMPE_NORMAL:
			case ACT_POMPE_REVERSE:
			case ACT_POMPE_STOP:
				ACTQ_push_operation_from_msg(msg, QUEUE_ACT_SMALL_POMPE_PRISE, &SMALL_POMPE_PRISE_run_command, msg->data.act_msg.act_data.act_optionnal_data[0],TRUE);
				break;

			default:
				component_printf(LOG_LEVEL_Warning, "invalid CAN msg data[0]=%u !\n", msg->data.act_msg.order);
		}
		return TRUE;
	}else if(msg->sid == ACT_DO_SELFTEST) {

	}

	return FALSE;
}

void SMALL_POMPE_PRISE_run_command(queue_id_t queueId, bool_e init) {
	if(QUEUE_has_error(queueId)) {
		QUEUE_behead(queueId);
		return;
	}

	if(QUEUE_get_act(queueId) == QUEUE_ACT_SMALL_POMPE_PRISE) {
			if(init)
				SMALL_POMPE_PRISE_command_init(queueId);
			else
				SMALL_POMPE_PRISE_command_run(queueId);
	}
}

//Initialise une commande
static void SMALL_POMPE_PRISE_command_init(queue_id_t queueId) {
	Uint8 command = QUEUE_get_arg(queueId)->canCommand;
	Uint8 param = QUEUE_get_arg(queueId)->param;

	switch(command) {
		case ACT_POMPE_NORMAL:
		case ACT_POMPE_REVERSE:
			SMALL_POMPE_PRISE_do_order(command, param);
			break;

		case ACT_POMPE_STOP:
			PWM_stop(SMALL_POMPE_PRISE_PWM_NUM);
			return;

		default: {
			error_printf("Invalid SMALL_POMPE_PRISE command: %u, code is broken !\n", command);
			QUEUE_next(queueId, ACT_SMALL_POMPE_PRISE, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_LOGIC, __LINE__);
			return;
		}
	}
}

static void SMALL_POMPE_PRISE_command_run(queue_id_t queueId){
	QUEUE_next(queueId, ACT_SMALL_POMPE_PRISE, ACT_RESULT_DONE, ACT_RESULT_ERROR_OK, 0);
}

static void SMALL_POMPE_PRISE_do_order(Uint8 command, Uint8 param){
	if(command == ACT_POMPE_NORMAL)
		GPIO_SetBits(SMALL_POMPE_PRISE_SENS);
	else if(command == ACT_POMPE_REVERSE)
		GPIO_ResetBits(SMALL_POMPE_PRISE_SENS);
	else{
		debug_printf("commande envoyée à SMALL_POMPE_PRISE_do_order inconnue -> %d	%x\n", command, command);
		PWM_stop(SMALL_POMPE_PRISE_PWM_NUM);
		return;
	}

	param = (param > 100) ? 100 : param;
	PWM_run(param, SMALL_POMPE_PRISE_PWM_NUM);
}

#endif


