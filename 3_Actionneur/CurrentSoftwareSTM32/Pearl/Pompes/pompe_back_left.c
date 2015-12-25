/*  Club Robot ESEO 2015 - 2016
 *
 *	Fichier : pompe_back_left.c
 *	Package : Carte actionneur
 *	Description : Gestion de la pompe arrière gauche permettant de ventouser un coquillage
 *  Auteur : Valentin
 *  Version 2016
 *  Robot : SMALL
 */

#include "pompe_back_left.h"

// Exemple d'une pompe

// Ajout l'actionneur dans QS_CANmsgList.h
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

#define POMPE_BACK_LEFT_PIN 0,0

#ifdef I_AM_ROBOT_SMALL

#include "../QS/QS_CANmsgList.h"
#include "../QS/QS_ax12.h"
#include "../QS/QS_pwm.h"
#include "../QS/QS_ports.h"
#include "../QS/QS_can.h"
#include "../act_queue_utils.h"
#include "../selftest.h"

#define LOG_PREFIX "pompe_back_left : "
#define LOG_COMPONENT OUTPUT_LOG_COMPONENT_POMPE_BACK_LEFT
#include "../QS/QS_outputlog.h"

static void POMPE_BACK_LEFT_initDCM();
static void POMPE_BACK_LEFT_command_init(queue_id_t queueId);
static void POMPE_BACK_LEFT_command_run(queue_id_t queueId);
static void POMPE_BACK_LEFT_do_order(Uint8 command);
static void POMPE_BACK_LEFT_run_command(queue_id_t queueId, bool_e init);

void POMPE_BACK_LEFT_init() {
	static bool_e initialized = FALSE;

	if(initialized)
		return;
	initialized = TRUE;

	POMPE_BACK_LEFT_initDCM();
}

void POMPE_BACK_LEFT_reset_config(){}

// Initialisation du moteur, si init ne fait rien
static void POMPE_BACK_LEFT_initDCM() {
	static bool_e initialized = FALSE;

	if(initialized)
		return;
	initialized = TRUE;
}

void POMPE_BACK_LEFT_stop() {
	GPIO_ResetBits(POMPE_BACK_LEFT_PIN);
}

void POMPE_BACK_LEFT_init_pos(){
	GPIO_ResetBits(POMPE_BACK_LEFT_PIN);
}

bool_e POMPE_BACK_LEFT_CAN_process_msg(CAN_msg_t* msg) {
	if(msg->sid == ACT_POMPE_BACK_LEFT) {
		POMPE_BACK_LEFT_initDCM();
		switch(msg->data.act_msg.order) {
			case ACT_POMPE_BACK_LEFT_NORMAL:
			case ACT_POMPE_BACK_LEFT_STOP:
				ACTQ_push_operation_from_msg(msg, QUEUE_ACT_POMPE_BACK_LEFT, &POMPE_BACK_LEFT_run_command, msg->data.act_msg.act_data.act_optionnal_data[0], TRUE);
				break;

			default:
				component_printf(LOG_LEVEL_Warning, "invalid CAN msg data[0]=%u !\n", msg->data.act_msg.order);
		}
		return TRUE;
	}else if(msg->sid == ACT_DO_SELFTEST) {
		/*SELFTEST_set_actions(&POMPE_BACK_LEFT_run_command,  2, (SELFTEST_action_t[]){
								 {ACT_POMPE_BACK_LEFT_NORMAL,  0, QUEUE_ACT_POMPE_BACK_LEFT},
								 {ACT_POMPE_BACK_LEFT_STOP, 0, QUEUE_ACT_POMPE_BACK_LEFT}
							 });*/
	}

	return FALSE;
}

static void POMPE_BACK_LEFT_run_command(queue_id_t queueId, bool_e init) {
	if(QUEUE_has_error(queueId)) {
		QUEUE_behead(queueId);
		return;
	}

	if(QUEUE_get_act(queueId) == QUEUE_ACT_POMPE_BACK_LEFT) {
			if(init)
				POMPE_BACK_LEFT_command_init(queueId);
			else
				POMPE_BACK_LEFT_command_run(queueId);
	}
}

//Initialise une commande
static void POMPE_BACK_LEFT_command_init(queue_id_t queueId) {
	Uint8 command = QUEUE_get_arg(queueId)->canCommand;

	switch(command) {
		case ACT_POMPE_BACK_LEFT_NORMAL:
			POMPE_BACK_LEFT_do_order(command);
			break;

		case ACT_POMPE_BACK_LEFT_STOP:
			GPIO_ResetBits(POMPE_BACK_LEFT_PIN);
			return;

		default: {
			error_printf("Invalid POMPE_BACK_LEFT command: %u, code is broken !\n", command);
			QUEUE_next(queueId, ACT_POMPE_BACK_LEFT, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_LOGIC, __LINE__);
			return;
		}
	}
}

static void POMPE_BACK_LEFT_command_run(queue_id_t queueId){
	QUEUE_next(queueId, ACT_POMPE_BACK_LEFT, ACT_RESULT_DONE, ACT_RESULT_ERROR_OK, 0);
}

static void POMPE_BACK_LEFT_do_order(Uint8 command){
	if(command == ACT_POMPE_BACK_LEFT_NORMAL)
		GPIO_SetBits(POMPE_BACK_LEFT_PIN);
	else{
		debug_printf("commande envoyée à POMPE_BACK_LEFT_do_order inconnue -> %d	0x%x\n", command, command);
		GPIO_ResetBits(POMPE_BACK_LEFT_PIN);
		return;
	}
}

#endif

#endif

