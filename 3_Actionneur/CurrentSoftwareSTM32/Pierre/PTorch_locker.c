/*  Club Robot ESEO 2013 - 2014
 *
 *	Fichier : PTorch_locker.c
 *	Package : Carte actionneur
 *	Description : Gestion du système de vérouillage des torches
 *  Auteur : Arnaud
 *  Version 20130219
 *  Robot : BIG
 */

#include "PTorch_locker.h"
#ifdef I_AM_ROBOT_BIG

#include "PTorch_locker_config.h"

#include "../QS/QS_CANmsgList.h"
#include "../QS/QS_ax12.h"
#include "../act_queue_utils.h"
#include "../selftest.h"
#include "../config/config_pin.h"
#include "../QS/QS_watchdog.h"

#include "config_debug.h"
#define LOG_PREFIX "Torch locker.c : "
#define LOG_COMPONENT OUTPUT_LOG_COMPONENT_SMALL_ARM
#include "../QS/QS_outputlog.h"


static void TORCH_LOCKER_command_run(queue_id_t queueId);
static void TORCH_LOCKER_initAX12();
static void TORCH_LOCKER_command_init(queue_id_t queueId);


static Uint16 ax12_1_goalPosition = 0xFFFF;
static Uint16 ax12_2_goalPosition = 0xFFFF;
static Uint8 last_command;
static watchdog_id_t watchdog_PTorch;

void TORCH_LOCKER_init() {
	static bool_e initialized = FALSE;

	if(initialized)
		return;
	initialized = TRUE;

	AX12_init();
	TORCH_LOCKER_initAX12();
}

//Initialise l'AX12 du TORCH_LOCKER s'il n'était pas alimenté lors d'initialisations précédentes, si déjà initialisé, ne fait rien
static void TORCH_LOCKER_initAX12() {
	static bool_e ax12_1_is_initialized = FALSE;
	static bool_e ax12_2_is_initialized = FALSE;
	if(ax12_1_is_initialized == FALSE && AX12_is_ready(TORCH_LOCKER_AX12_1_ID) == TRUE) {
		ax12_1_is_initialized = TRUE;
		AX12_config_set_highest_voltage(TORCH_LOCKER_AX12_1_ID, 136);
		AX12_config_set_lowest_voltage(TORCH_LOCKER_AX12_1_ID, 70);
		AX12_config_set_maximum_torque_percentage(TORCH_LOCKER_AX12_1_ID, TORCH_LOCKER_AX12_1_MAX_TORQUE_PERCENT);

		AX12_config_set_maximal_angle(TORCH_LOCKER_AX12_1_ID, TORCH_LOCKER_AX12_1_MAX_VALUE);
		AX12_config_set_minimal_angle(TORCH_LOCKER_AX12_1_ID, TORCH_LOCKER_AX12_1_MIN_VALUE);

		AX12_config_set_error_before_led(TORCH_LOCKER_AX12_1_ID, AX12_ERROR_ANGLE | AX12_ERROR_CHECKSUM | AX12_ERROR_INSTRUCTION | AX12_ERROR_OVERHEATING | AX12_ERROR_OVERLOAD | AX12_ERROR_RANGE);
		AX12_config_set_error_before_shutdown(TORCH_LOCKER_AX12_1_ID, AX12_ERROR_OVERHEATING);
	}

	if(ax12_2_is_initialized == FALSE && AX12_is_ready(TORCH_LOCKER_AX12_2_ID) == TRUE) {
		ax12_2_is_initialized = TRUE;
		AX12_config_set_highest_voltage(TORCH_LOCKER_AX12_2_ID, 136);
		AX12_config_set_lowest_voltage(TORCH_LOCKER_AX12_2_ID, 70);
		AX12_config_set_maximum_torque_percentage(TORCH_LOCKER_AX12_2_ID, TORCH_LOCKER_AX12_2_MAX_TORQUE_PERCENT);

		AX12_config_set_maximal_angle(TORCH_LOCKER_AX12_2_ID, TORCH_LOCKER_AX12_2_MAX_VALUE);
		AX12_config_set_minimal_angle(TORCH_LOCKER_AX12_2_ID, TORCH_LOCKER_AX12_2_MIN_VALUE);

		AX12_config_set_error_before_led(TORCH_LOCKER_AX12_2_ID, AX12_ERROR_ANGLE | AX12_ERROR_CHECKSUM | AX12_ERROR_INSTRUCTION | AX12_ERROR_OVERHEATING | AX12_ERROR_OVERLOAD | AX12_ERROR_RANGE);
		AX12_config_set_error_before_shutdown(TORCH_LOCKER_AX12_2_ID, AX12_ERROR_OVERHEATING);
	}

	debug_printf("Torch locker init config %s  (%s %s)\n", (ax12_1_is_initialized && ax12_2_is_initialized) ? "DONE" : "FAIL", ax12_1_is_initialized ? "DONE" : "FAIL", ax12_2_is_initialized ? "DONE" : "FAIL");
}

void TORCH_LOCKER_init_pos(){
	TORCH_LOCKER_initAX12();
	debug_printf("Torch locker init pos : \n");
	if(!AX12_set_position(TORCH_LOCKER_AX12_1_ID, TORCH_LOCKER_AX12_1_INIT_POS))
		debug_printf("   L'AX12 n°%d n'est pas là\n", TORCH_LOCKER_AX12_1_ID);
	if(!AX12_set_position(TORCH_LOCKER_AX12_2_ID, TORCH_LOCKER_AX12_2_INIT_POS))
		debug_printf("   L'AX12 n°%d n'est pas là\n", TORCH_LOCKER_AX12_2_ID);
}

void TORCH_LOCKER_stop(){

}

void PTORCH_process_main(){
	static bool_e flag_watchDog = FALSE;
	static bool_e init = FALSE;

	if(flag_watchDog)
		display(flag_watchDog);

	if(init == FALSE){
		watchdog_PTorch = WATCHDOG_create_flag(3000,&flag_watchDog);
		init = TRUE;
		return;
	}

	if(!flag_watchDog) // Retourne si le flag n'est pas encore levé
		return;

	flag_watchDog = FALSE;
	init = FALSE;
	CAN_msg_t msg;
	msg.size = 1;
	msg.sid = ACT_TORCH_LOCKER;
	msg.data[0] = last_command;

	ACTQ_push_operation_from_msg(&msg, QUEUE_ACT_Torch_Locker, &TORCH_LOCKER_run_command, 0,FALSE);
	global.PTorchErrorLastCode = FALSE;
}

bool_e TORCH_LOCKER_CAN_process_msg(CAN_msg_t* msg) {
	if(msg->sid == ACT_TORCH_LOCKER) {
		TORCH_LOCKER_initAX12();
		switch(msg->data[0]) {
			case ACT_TORCH_LOCKER_LOCK :
			case ACT_TORCH_LOCKER_UNLOCK :
			case ACT_TORCH_LOCKER_INSIDE :
			case ACT_TORCH_LOCKER_STOP :
				ACTQ_push_operation_from_msg(msg, QUEUE_ACT_Torch_Locker, &TORCH_LOCKER_run_command, 0,TRUE);
				break;

			default:
				component_printf(LOG_LEVEL_Warning, "invalid CAN msg data[0]=%u !\n", msg->data[0]);
		}
		return TRUE;
	} else if(msg->sid == ACT_DO_SELFTEST) {
		SELFTEST_set_actions(&TORCH_LOCKER_run_command, 3, (SELFTEST_action_t[]){
								 {ACT_TORCH_LOCKER_INSIDE,     0,  QUEUE_ACT_Torch_Locker},
								 {ACT_TORCH_LOCKER_UNLOCK,   0,  QUEUE_ACT_Torch_Locker},
								 {ACT_TORCH_LOCKER_INSIDE,   0,  QUEUE_ACT_Torch_Locker}
								 // Vérifier présence SMALL_ARM
							 });
	}

	return FALSE;
}

void TORCH_LOCKER_run_command(queue_id_t queueId, bool_e init) {
	if(QUEUE_has_error(queueId)) {
		QUEUE_behead(queueId);
		return;
	}

	if(QUEUE_get_act(queueId) == QUEUE_ACT_Torch_Locker) {    // Gestion des mouvements de TORCH_LOCKER
		if(init)
			TORCH_LOCKER_command_init(queueId);
		else
			TORCH_LOCKER_command_run(queueId);
	}
}

//Initialise une commande
static void TORCH_LOCKER_command_init(queue_id_t queueId) {
	Uint8 command = QUEUE_get_arg(queueId)->canCommand;

	switch(command) {
		case ACT_TORCH_LOCKER_LOCK : ax12_1_goalPosition = TORCH_LOCKER_AX12_1_LOCK_POS; ax12_2_goalPosition = TORCH_LOCKER_AX12_2_LOCK_POS; break;
		case ACT_TORCH_LOCKER_UNLOCK : ax12_1_goalPosition = TORCH_LOCKER_AX12_1_UNLOCK_POS;ax12_2_goalPosition = TORCH_LOCKER_AX12_2_UNLOCK_POS; break;
		case ACT_TORCH_LOCKER_INSIDE : ax12_1_goalPosition = TORCH_LOCKER_AX12_1_INSIDE_POS; ax12_2_goalPosition = TORCH_LOCKER_AX12_2_INSIDE_POS; break;

		case ACT_SMALL_ARM_STOP :
			AX12_set_torque_enabled(TORCH_LOCKER_AX12_1_ID, FALSE); //Stopper l'asservissement de l'AX12 qui gère le TORCH_LOCKER
			AX12_set_torque_enabled(TORCH_LOCKER_AX12_2_ID, FALSE);
			QUEUE_next(queueId, ACT_TORCH_LOCKER, ACT_RESULT_DONE, ACT_RESULT_ERROR_OK, __LINE__);
			return;

		default: {
			error_printf("Invalid plier command: %u, code is broken !\n", command);
			QUEUE_next(queueId, ACT_TORCH_LOCKER, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_LOGIC, __LINE__);
			return;
		}
	}

	if(ax12_1_goalPosition == 0xFFFF) {
		error_printf("Invalid ax12 %d position for command: %u, code is broken !\n", TORCH_LOCKER_AX12_1_ID, command);
		QUEUE_next(queueId, ACT_TORCH_LOCKER, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_LOGIC, __LINE__);
		return;
	}else if(ax12_2_goalPosition == 0xFFFF) {
		error_printf("Invalid ax12 %d position for command: %u, code is broken !\n", TORCH_LOCKER_AX12_2_ID, command);
		QUEUE_next(queueId, ACT_TORCH_LOCKER, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_LOGIC, __LINE__);
		return;
	}

	debug_printf("Move TORCH_LOCKER ax12_1 %d to %d\n", TORCH_LOCKER_AX12_1_ID, ax12_1_goalPosition);
	debug_printf("Move TORCH_LOCKER ax12_2 %d to %d\n", TORCH_LOCKER_AX12_2_ID, ax12_2_goalPosition);
	AX12_reset_last_error(TORCH_LOCKER_AX12_1_ID); //Sécurité anti terroriste. Nous les parano on aime pas voir des erreurs là ou il n'y en a pas.
	AX12_reset_last_error(TORCH_LOCKER_AX12_2_ID); //Sécurité anti terroriste. Nous les parano on aime pas voir des erreurs là ou il n'y en a pas.
	if(!AX12_set_position(TORCH_LOCKER_AX12_1_ID, ax12_1_goalPosition)) {	//Si la commande n'a pas été envoyée correctement et/ou que l'AX12 ne répond pas a cet envoi, on l'indique à la carte stratégie
		error_printf("AX12_set_position id : %d error: 0x%x\n", TORCH_LOCKER_AX12_1_ID, AX12_get_last_error(TORCH_LOCKER_AX12_1_ID).error);
		QUEUE_next(queueId, ACT_TORCH_LOCKER, ACT_RESULT_FAILED, ACT_RESULT_ERROR_NOT_HERE, __LINE__);
		return;
	}else if(!AX12_set_position(TORCH_LOCKER_AX12_2_ID, ax12_2_goalPosition)) {	//Si la commande n'a pas été envoyée correctement et/ou que l'AX12 ne répond pas a cet envoi, on l'indique à la carte stratégie
			 error_printf("AX12_set_position id : %d error: 0x%x\n", TORCH_LOCKER_AX12_2_ID, AX12_get_last_error(TORCH_LOCKER_AX12_2_ID).error);
			 QUEUE_next(queueId, ACT_TORCH_LOCKER, ACT_RESULT_FAILED, ACT_RESULT_ERROR_NOT_HERE, __LINE__);
			 return;
		 }
	//La commande a été envoyée et l'AX12 l'a bien reçu
}

//Gère les états pendant le mouvement de l'AX12 déclenchant le TORCH_LOCKER
static void TORCH_LOCKER_command_run(queue_id_t queueId) {
	Uint8 result1, result2, errorCode1, errorCode2;
	Uint16 line1, line2;
	static bool_e check_1 = FALSE, check_2 = FALSE;
	last_command = QUEUE_get_arg(queueId)->canCommand;

	if(watchdog_PTorch != 0xFF)
		WATCHDOG_stop(watchdog_PTorch);


	if(!check_1)
		check_1 = ACTQ_check_status_ax12(queueId, TORCH_LOCKER_AX12_1_ID, ax12_1_goalPosition, TORCH_LOCKER_AX12_1_ASSER_POS_EPSILON, TORCH_LOCKER_AX12_1_ASSER_TIMEOUT, TORCH_LOCKER_AX12_1_ASSER_POS_LARGE_EPSILON, &result1, &errorCode1, &line1);

	if(!check_2)
		check_2 = ACTQ_check_status_ax12(queueId, TORCH_LOCKER_AX12_2_ID, ax12_2_goalPosition,TORCH_LOCKER_AX12_2_ASSER_POS_EPSILON, TORCH_LOCKER_AX12_2_ASSER_TIMEOUT, TORCH_LOCKER_AX12_2_ASSER_POS_LARGE_EPSILON, &result2, &errorCode2, &line2);

	if(check_1 && check_2){
		check_1 = FALSE;
		check_2 = FALSE;
		if(result1 != ACT_RESULT_DONE)
			QUEUE_next(queueId, ACT_TORCH_LOCKER, result1, errorCode1, line1);
		else
			QUEUE_next(queueId, ACT_TORCH_LOCKER, result2, errorCode2, line2);

		if(errorCode1 != ACT_RESULT_ERROR_OK || errorCode2 != ACT_RESULT_ERROR_OK)
			global.PTorchErrorLastCode = TRUE;
	}
}

#endif
