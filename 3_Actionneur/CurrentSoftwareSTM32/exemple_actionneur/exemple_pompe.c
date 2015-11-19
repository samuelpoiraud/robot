/*  Club Robot ESEO 2015 - 2016
 *
 *	Fichier : exemple_pompe.c
 *	Package : Carte actionneur
 *	Description : Gestion de exemple pompe
 *  Auteur :
 *  Version 2016
 *  Robot : BIG
 */

#include "exemple_pompe.h"

// Exemple d'une pompe

// Ajout l'actionneur dans QS_CANmsgList.h ainsi que toutes ses différentes position
// Ajout d'une valeur dans l'énumération de la queue dans config_(big/small)/config_global_vars_types.h
// Formatage : QUEUE_ACT_AX12_EXEMPLE
// Ajout de la déclaration de l'actionneur dans ActManager dans le tableau actionneurs
// Ajout de la verbosité dans le fichier act_queue_utils.c dans la fonction ACTQ_internal_printResult
// Ajout du pilotage via terminal dans le fichier term_io.c dans le tableau terminal_motor du type : ACT_DECLARE(EXEMPLE)
// Un define EXEMPLE_AX12_ID doit avoir été ajouté au fichier config_big/config_pin.h // config_small/config_pin.h
// Ajout des postions dans QS_types.h dans l'énum ACT_order_e (avec "ACT_" et sans "_POS" à la fin)

// Optionnel:
// Ajout du selftest dans le fichier selftest.c dans la fonction SELFTEST_done_test
// Ajout du selftest dans le fichier QS_CANmsgList (dans l'énumération SELFTEST)

// En stratégie
// ajout d'une d'une valeur dans le tableau act_link_SID_Queue du fichier act_functions.c/h
// ajout des fonctions actionneurs dans act_avoidance.c/h si l'actionneur modifie l'évitement du robot

// En stratégie Optionnel
// ajout du verbose du selftest dans Supervision/Selftest.c (tableau SELFTEST_getError_string, fonction SELFTEST_print_errors)
// ajout de la verbosité dans Supervision/Verbose_can_msg.c/h (fonction VERBOSE_CAN_MSG_sprint)

#if 0

#ifdef I_AM_ROBOT_BIG

#include "../QS/QS_CANmsgList.h"
#include "../QS/QS_ax12.h"
#include "../QS/QS_pwm.h"
#include "../QS/QS_ports.h"
#include "../QS/QS_can.h"
#include "../act_queue_utils.h"
#include "../selftest.h"

#define LOG_PREFIX "exemple_pompe : "
#define LOG_COMPONENT OUTPUT_LOG_COMPONENT_EXEMPLE_POMPE
#include "../QS/QS_outputlog.h"

static void EXEMPLE_POMPE_initDCM();
static void EXEMPLE_POMPE_command_init(queue_id_t queueId);
static void EXEMPLE_POMPE_command_run(queue_id_t queueId);
static void EXEMPLE_POMPE_do_order(Uint8 command, Uint8 param);
static void EXEMPLE_POMPE_run_command(queue_id_t queueId, bool_e init);

void EXEMPLE_POMPE_init() {
	static bool_e initialized = FALSE;

	if(initialized)
		return;
	initialized = TRUE;

	EXEMPLE_POMPE_initDCM();
}

void EXEMPLE_POMPE_reset_config(){}

// Initialisation du moteur, si init ne fait rien
static void EXEMPLE_POMPE_initDCM() {
	static bool_e initialized = FALSE;

	if(initialized)
		return;
	initialized = TRUE;

	PORTS_pwm_init();
	PWM_init();
	PWM_stop(EXEMPLE_POMPE_PWM_NUM);

	info_printf("initialisée (DCM) \n");
}

void EXEMPLE_POMPE_stop() {
	PWM_stop(EXEMPLE_POMPE_PWM_NUM);
}

void EXEMPLE_POMPE_init_pos(){
	PWM_stop(EXEMPLE_POMPE_PWM_NUM);
}

bool_e EXEMPLE_POMPE_CAN_process_msg(CAN_msg_t* msg) {
	if(msg->sid == ACT_EXEMPLE_POMPE) {
		EXEMPLE_POMPE_initDCM();
		switch(msg->data.act_msg.order) {
			case ACT_EXEMPLE_POMPE_NORMAL:
			case ACT_EXEMPLE_POMPE_REVERSE:
			case ACT_EXEMPLE_POMPE_STOP:
				ACTQ_push_operation_from_msg(msg, QUEUE_ACT_EXEMPLE_POMPE, &EXEMPLE_POMPE_run_command, msg->data.act_msg.act_data.act_optionnal_data[0], TRUE);
				break;

			default:
				component_printf(LOG_LEVEL_Warning, "invalid CAN msg data[0]=%u !\n", msg->data.act_msg.order);
		}
		return TRUE;
	}else if(msg->sid == ACT_DO_SELFTEST) {
		SELFTEST_set_actions(&EXEMPLE_POMPE_run_command, 27, 3, (SELFTEST_action_t[]){
								 {ACT_EXEMPLE_POMPE_NORMAL,  100, QUEUE_ACT_EXEMPLE_POMPE},
								 {ACT_EXEMPLE_POMPE_REVERSE, 100, QUEUE_ACT_EXEMPLE_POMPE},
								 {ACT_EXEMPLE_POMPE_STOP, 0, QUEUE_ACT_EXEMPLE_POMPE}
							 });
	}

	return FALSE;
}

static void EXEMPLE_POMPE_run_command(queue_id_t queueId, bool_e init) {
	if(QUEUE_has_error(queueId)) {
		QUEUE_behead(queueId);
		return;
	}

	if(QUEUE_get_act(queueId) == QUEUE_ACT_EXEMPLE_POMPE) {
			if(init)
				EXEMPLE_POMPE_command_init(queueId);
			else
				EXEMPLE_POMPE_command_run(queueId);
	}
}

//Initialise une commande
static void EXEMPLE_POMPE_command_init(queue_id_t queueId) {
	Uint8 command = QUEUE_get_arg(queueId)->canCommand;
	Uint8 param = QUEUE_get_arg(queueId)->param;

	switch(command) {
		case ACT_EXEMPLE_POMPE_NORMAL:
		case ACT_EXEMPLE_POMPE_REVERSE:
			EXEMPLE_POMPE_do_order(command, param);
			break;

		case ACT_EXEMPLE_POMPE_STOP:
			PWM_stop(EXEMPLE_POMPE_PWM_NUM);
			return;

		default: {
			error_printf("Invalid EXEMPLE_POMPE command: %u, code is broken !\n", command);
			QUEUE_next(queueId, ACT_EXEMPLE_POMPE, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_LOGIC, __LINE__);
			return;
		}
	}
}

static void EXEMPLE_POMPE_command_run(queue_id_t queueId){
	QUEUE_next(queueId, ACT_EXEMPLE_POMPE, ACT_RESULT_DONE, ACT_RESULT_ERROR_OK, 0);
}

static void EXEMPLE_POMPE_do_order(Uint8 command, Uint8 param){
	if(command == ACT_EXEMPLE_POMPE_NORMAL)
		GPIO_ResetBits(EXEMPLE_POMPE_SENS);
	else if(command == ACT_EXEMPLE_POMPE_REVERSE)
		GPIO_SetBits(EXEMPLE_POMPE_SENS);
	else{
		debug_printf("commande envoyée à EXEMPLE_POMPE_do_order inconnue -> %d	0x%x\n", command, command);
		PWM_stop(EXEMPLE_POMPE_PWM_NUM);
		return;
	}

	param = (param > 100) ? 100 : param;
	PWM_run(param, EXEMPLE_POMPE_PWM_NUM);
}

#endif

#endif
