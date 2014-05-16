/*  Club Robot ESEO 2013 - 2014
 *	BIG
 *
 *	Fichier : PGACHE.c
 *	Package : Carte actionneur
 *	Description : Gestion du GACHE
 *  Auteur : Arnaud
 *  Version 20130219
 *  Robot : BIG
 */
#include "PGache.h"
#ifdef I_AM_ROBOT_BIG

#include "PGache_config.h"

#include "../QS/QS_CANmsgList.h"
#include "../QS/QS_ax12.h"
#include "../QS/QS_watchdog.h"
#include "../QS/QS_can.h"
#include "../act_queue_utils.h"
#include "../selftest.h"
#include "../config/config_pin.h"
#include "../Can_msg_processing.h"


#include "config_debug.h"
#define LOG_PREFIX "PGache.c : "
#define LOG_COMPONENT OUTPUT_LOG_COMPONENT_GACHE
#include "../QS/QS_outputlog.h"

static void GACHE_initAX12();
static void GACHE_command_init(queue_id_t queueId);
static void GACHE_command_run(queue_id_t queueId);


void GACHE_init() {
	static bool_e initialized = FALSE;

	if(initialized)
		return;
	initialized = TRUE;

	AX12_init();
	GACHE_initAX12();
}

//Initialise l'AX12 du GACHE s'il n'était pas alimenté lors d'initialisations précédentes, si déjà initialisé, ne fait rien
static void GACHE_initAX12() {
	static bool_e ax12_is_initialized = FALSE;
	if(ax12_is_initialized == FALSE && AX12_is_ready(GACHE_AX12_ID) == TRUE) {
		ax12_is_initialized = TRUE;
		AX12_config_set_highest_voltage(GACHE_AX12_ID, 136);
		AX12_config_set_lowest_voltage(GACHE_AX12_ID, 70);
		AX12_config_set_maximum_torque_percentage(GACHE_AX12_ID, GACHE_AX12_MAX_TORQUE_PERCENT);

		AX12_config_set_maximal_angle(GACHE_AX12_ID, GACHE_AX12_MAX_VALUE);
		AX12_config_set_minimal_angle(GACHE_AX12_ID, GACHE_AX12_MIN_VALUE);

		AX12_config_set_error_before_led(GACHE_AX12_ID, AX12_ERROR_ANGLE | AX12_ERROR_CHECKSUM | AX12_ERROR_INSTRUCTION | AX12_ERROR_OVERHEATING | AX12_ERROR_OVERLOAD | AX12_ERROR_RANGE);
		AX12_config_set_error_before_shutdown(GACHE_AX12_ID, AX12_ERROR_OVERHEATING);
	}
	debug_printf("Gache init config %s\n", ax12_is_initialized ? "DONE" : "FAIL");
}

void GACHE_stop() {
}

void GACHE_init_pos(){
	GACHE_initAX12();
	debug_printf("Gache init pos : \n");
	if(!AX12_set_position(GACHE_AX12_ID, GACHE_AX12_IDLE_POS))
		debug_printf("   L'AX12 n°%d n'est pas la\n", GACHE_AX12_ID);
}


bool_e GACHE_CAN_process_msg(CAN_msg_t* msg) {
	queue_id_t queueId;
	if(msg->sid == ACT_GACHE){
		GACHE_initAX12();
		switch(msg->data[0]){

			//Même chose pour les 2 actions
			case ACT_GACHE_IDLE :
			case ACT_GACHE_LAUNCHED :
			case ACT_GACHE_STOP :
				ACTQ_push_operation_from_msg(msg, QUEUE_ACT_AX12_Gache, &GACHE_run_command, 0,TRUE);
				break;

			default:
				component_printf(LOG_LEVEL_Warning, "invalid CAN msg data[0]=%u !\n", msg->data[0]);
		}
		return TRUE;
	}else if(msg->sid == ACT_DO_SELFTEST){
		queueId = QUEUE_create();
		if(queueId != QUEUE_CREATE_FAILED){ // Selftest différent des autres actionneurs car il ne faut pas bouger l'actionneur juste regarder son état
			SELFTEST_set_actions(&GACHE_run_command, 3, (SELFTEST_action_t[]){
									 {ACT_GACHE_IDLE,   0, QUEUE_ACT_AX12_Gache},
									 {ACT_GACHE_LAUNCHED,  0, QUEUE_ACT_AX12_Gache},
									 {ACT_GACHE_IDLE,  0, QUEUE_ACT_AX12_Gache}
								 });
		}
	}

	return FALSE;
}

void GACHE_run_command(queue_id_t queueId, bool_e init) {
	if(QUEUE_has_error(queueId)) {
		QUEUE_behead(queueId);
		return;
	}

	if(QUEUE_get_act(queueId) == QUEUE_ACT_AX12_Gache) {    // Gestion des mouvements du GACHE
		if(init)
			GACHE_command_init(queueId);
		else
			GACHE_command_run(queueId);
	}
}


//Initialise une commande
static void GACHE_command_init(queue_id_t queueId) {
	Uint8 command = QUEUE_get_arg(queueId)->canCommand;
	Uint16* ax12_goalPosition = &QUEUE_get_arg(queueId)->param;

	*ax12_goalPosition = 0xFFFF;

	switch(command) {
		case ACT_GACHE_IDLE :  *ax12_goalPosition = GACHE_AX12_IDLE_POS; break;
		case ACT_GACHE_LAUNCHED : *ax12_goalPosition = GACHE_AX12_LAUNCHED_POS; break;

		case ACT_GACHE_STOP :
			AX12_set_torque_enabled(GACHE_AX12_ID, FALSE); //Stopper l'asservissement de l'AX12 qui gère le GACHE
			QUEUE_next(queueId, ACT_GACHE, ACT_RESULT_DONE, ACT_RESULT_ERROR_OK, __LINE__);
			return;

		default: {
				error_printf("Invalid command: %u, code is broken !\n", command);
				QUEUE_next(queueId, ACT_GACHE, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_LOGIC, __LINE__);
				return;
			}
	}
	if(*ax12_goalPosition == 0xFFFF) {
		error_printf("Invalid position for command: %u, code is broken !\n", command);
		QUEUE_next(queueId, ACT_GACHE, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_LOGIC, __LINE__);
		return;
	}

	debug_printf("Move GACHE ax12 to %d\n", *ax12_goalPosition);
	AX12_reset_last_error(GACHE_AX12_ID); //Sécurité anti terroriste. Nous les parano on aime pas voir des erreurs là ou il n'y en a pas.

	if(!AX12_set_position(GACHE_AX12_ID, *ax12_goalPosition)) {	//Si la commande n'a pas été envoyée correctement et/ou que l'AX12 ne répond pas a cet envoi, on l'indique à la carte stratégie
		error_printf("AX12_set_position error: 0x%x\n", AX12_get_last_error(GACHE_AX12_ID).error);
		QUEUE_next(queueId, ACT_GACHE, ACT_RESULT_FAILED, ACT_RESULT_ERROR_NOT_HERE, __LINE__);
		return;
	}

	//La commande a été envoyée et l'AX12 l'a bien reçu
}

//Gère les états pendant le mouvement de l'AX12 déclenchant le GACHE
static void GACHE_command_run(queue_id_t queueId) {
	Uint8 result, errorCode;
	Uint16 line;

	if(ACTQ_check_status_ax12(queueId, GACHE_AX12_ID, QUEUE_get_arg(queueId)->param, GACHE_AX12_ASSER_POS_EPSILON, GACHE_AX12_ASSER_TIMEOUT, 0, &result, &errorCode, &line))
		QUEUE_next(queueId, ACT_GACHE, result, errorCode, line);
}

#endif /* I_AM_BIG_ROBOT */
