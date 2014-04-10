/*  Club Robot ESEO 2013 - 2014
 *	SMALL
 *
 *	Fichier : PSmallArm.c
 *	Package : Carte actionneur
 *	Description : Gestion du petit bras
 *  Auteur : Arnaud
 *  Version 20130219
 *  Robot : SMALL
 */

#include "Small_arm.h"
#include "Small_arm_config.h"

#include "../QS/QS_CANmsgList.h"
#include "../QS/QS_ax12.h"
#include "../act_queue_utils.h"
#include "../selftest.h"
#include "../config/config_pin.h"

#include "config_debug.h"
#define LOG_PREFIX "Small_arm.c : "
#define LOG_COMPONENT OUTPUT_LOG_COMPONENT_SMALL_ARM
#include "../QS/QS_outputlog.h"


static void SMALL_ARM_command_run(queue_id_t queueId);
static void SMALL_ARM_initAX12();
static void SMALL_ARM_command_init(queue_id_t queueId);

void SMALL_ARM_init() {
	static bool_e initialized = FALSE;

	if(initialized)
		return;
	initialized = TRUE;

	AX12_init();
	SMALL_ARM_initAX12();
}

//Initialise l'AX12 du SMALL_ARM s'il n'était pas alimenté lors d'initialisations précédentes, si déjà initialisé, ne fait rien
static void SMALL_ARM_initAX12() {
	static bool_e ax12_is_initialized = FALSE;
	if(ax12_is_initialized == FALSE && AX12_is_ready(SMALL_ARM_AX12_ID) == TRUE) {
		ax12_is_initialized = TRUE;
		AX12_config_set_highest_voltage(SMALL_ARM_AX12_ID, 136);
		AX12_config_set_lowest_voltage(SMALL_ARM_AX12_ID, 70);
		AX12_config_set_maximum_torque_percentage(SMALL_ARM_AX12_ID, SMALL_ARM_AX12_MAX_TORQUE_PERCENT);

		AX12_config_set_maximal_angle(SMALL_ARM_AX12_ID, 300);
		AX12_config_set_minimal_angle(SMALL_ARM_AX12_ID, 0);

		AX12_config_set_error_before_led(SMALL_ARM_AX12_ID, AX12_ERROR_ANGLE | AX12_ERROR_CHECKSUM | AX12_ERROR_INSTRUCTION | AX12_ERROR_OVERHEATING | AX12_ERROR_OVERLOAD | AX12_ERROR_RANGE);
		AX12_config_set_error_before_shutdown(SMALL_ARM_AX12_ID, AX12_ERROR_OVERHEATING);
	}

	debug_printf("Small Arm init config %s\n", ax12_is_initialized ? "DONE" : "FAIL");
}

void SMALL_ARM_init_pos(){
	SMALL_ARM_initAX12();
	debug_printf("Small Arm init pos : \n");
	if(!AX12_set_position(SMALL_ARM_AX12_ID, SMALL_ARM_AX12_INIT_POS))
		debug_printf("   L'AX12 n°%d n'est pas là\n", SMALL_ARM_AX12_ID);
}

void SMALL_ARM_stop(){

}

bool_e SMALL_ARM_CAN_process_msg(CAN_msg_t* msg) {
	if(msg->sid == ACT_SMALL_ARM) {
		SMALL_ARM_initAX12();
		switch(msg->data[0]) {
			case ACT_SMALL_ARM_IDLE :
			case ACT_SMALL_ARM_MID :
			case ACT_SMALL_ARM_DEPLOYED :
			case ACT_SMALL_ARM_STOP :
				ACTQ_push_operation_from_msg(msg, QUEUE_ACT_AX12_Small_Arm, &SMALL_ARM_run_command, 0);
				break;


			default:
				component_printf(LOG_LEVEL_Warning, "invalid CAN msg data[0]=%u !\n", msg->data[0]);
		}
		return TRUE;
	} else if(msg->sid == ACT_DO_SELFTEST) {
		SELFTEST_set_actions(&SMALL_ARM_run_command, 5, (SELFTEST_action_t[]){
								 {ACT_SMALL_ARM_IDLE,     0,  QUEUE_ACT_AX12_Small_Arm},
								 {ACT_SMALL_ARM_MID,      0,  QUEUE_ACT_AX12_Small_Arm},
								 {ACT_SMALL_ARM_DEPLOYED, 0,  QUEUE_ACT_AX12_Small_Arm},
								 {ACT_SMALL_ARM_MID,      0,  QUEUE_ACT_AX12_Small_Arm},
								 {ACT_SMALL_ARM_IDLE,     0,  QUEUE_ACT_AX12_Small_Arm},
								 // Vérifier présence SMALL_ARM
							 });
	}

	return FALSE;
}

void SMALL_ARM_run_command(queue_id_t queueId, bool_e init) {
	if(QUEUE_has_error(queueId)) {
		QUEUE_behead(queueId);
		return;
	}

	if(QUEUE_get_act(queueId) == QUEUE_ACT_AX12_Small_Arm) {    // Gestion des mouvements du SMALL_ARM
		if(init)
			SMALL_ARM_command_init(queueId);
		else
			SMALL_ARM_command_run(queueId);
	}
}

//Initialise une commande
static void SMALL_ARM_command_init(queue_id_t queueId) {
	Uint8 command = QUEUE_get_arg(queueId)->canCommand;
	Uint16* ax12_goalPosition = &QUEUE_get_arg(queueId)->param;

	*ax12_goalPosition = 0xFFFF;

	switch(command) {
		case ACT_SMALL_ARM_IDLE :  *ax12_goalPosition = SMALL_ARM_AX12_IDLE_POS; break;
		case ACT_SMALL_ARM_MID : *ax12_goalPosition = SMALL_ARM_AX12_MID_POS; break;
		case ACT_SMALL_ARM_DEPLOYED : *ax12_goalPosition = SMALL_ARM_AX12_DEPLOYED_POS; break;

		case ACT_SMALL_ARM_STOP :
			AX12_set_torque_enabled(SMALL_ARM_AX12_ID, FALSE); //Stopper l'asservissement de l'AX12 qui gère le SMALL_ARM
			QUEUE_next(queueId, ACT_SMALL_ARM, ACT_RESULT_DONE, ACT_RESULT_ERROR_OK, __LINE__);
			return;

		default: {
			error_printf("Invalid plier command: %u, code is broken !\n", command);
			QUEUE_next(queueId, ACT_SMALL_ARM, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_LOGIC, __LINE__);
			return;
		}
	}
	if(*ax12_goalPosition == 0xFFFF) {
		error_printf("Invalid ax12 position for command: %u, code is broken !\n", command);
		QUEUE_next(queueId, ACT_SMALL_ARM, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_LOGIC, __LINE__);
		return;
	}

	debug_printf("Move SMALL_ARM ax12 to %d\n", *ax12_goalPosition);
	AX12_reset_last_error(SMALL_ARM_AX12_ID); //Sécurité anti terroriste. Nous les parano on aime pas voir des erreurs là ou il n'y en a pas.
	if(!AX12_set_position(SMALL_ARM_AX12_ID, *ax12_goalPosition)) {	//Si la commande n'a pas été envoyée correctement et/ou que l'AX12 ne répond pas a cet envoi, on l'indique à la carte stratégie
		error_printf("AX12_set_position error: 0x%x\n", AX12_get_last_error(SMALL_ARM_AX12_ID).error);
		QUEUE_next(queueId, ACT_SMALL_ARM, ACT_RESULT_FAILED, ACT_RESULT_ERROR_NOT_HERE, __LINE__);
		return;
	}
	//La commande a été envoyée et l'AX12 l'a bien reçu
}

//Gère les états pendant le mouvement de l'AX12 déclenchant le SMALL_ARM
static void SMALL_ARM_command_run(queue_id_t queueId) {
	Uint8 result, errorCode;
	Uint16 line;

	if(ACTQ_check_status_ax12(queueId, SMALL_ARM_AX12_ID, QUEUE_get_arg(queueId)->param, SMALL_ARM_AX12_ASSER_POS_EPSILON, SMALL_ARM_AX12_ASSER_TIMEOUT, SMALL_ARM_AX12_ASSER_POS_LARGE_EPSILON, &result, &errorCode, &line))
		QUEUE_next(queueId, ACT_SMALL_ARM, result, errorCode, line);
}


