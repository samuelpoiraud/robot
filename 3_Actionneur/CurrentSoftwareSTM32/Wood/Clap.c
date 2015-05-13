/*  Club Robot ESEO 2014 - 2015
 *	SMALL
 *
 *	Fichier : Clap.c
 *	Package : Carte actionneur
 *	Description : Gestion de l'ax12 du clap
 *  Auteur : Valentin
 *  Version 20130219
 *  Robot : SMALL
 */

#include "Clap.h"

#ifdef I_AM_ROBOT_SMALL

// Les différents includes nécessaires...
#include "../QS/QS_CANmsgList.h"
#include "../QS/QS_ax12.h"
#include "../act_queue_utils.h"
#include "../selftest.h"
#include "../config/config_pin.h"
#include "../ActManager.h"

#include "Clap_config.h"

// Les différents define pour le verbose sur uart
#include "../config/config_debug.h"
#define LOG_PREFIX "Clap.c : "
#define LOG_COMPONENT OUTPUT_LOG_COMPONENT_CLAP
#include "../QS/QS_outputlog.h"

// Les fonctions internes au fonctionnement de l'actionneur
static void CLAP_command_run(queue_id_t queueId);
static void CLAP_initAX12();
static void CLAP_command_init(queue_id_t queueId);
static void CLAP_config(CAN_msg_t* msg);

// Booléen contenant l'état actuel de l'initialisation de l'AX12 (Plusieurs booléens si plusieurs servo dans l'actionneur)
static bool_e ax12_is_initialized = FALSE;

// Fonction appellée au lancement de la carte (via ActManager)
void CLAP_init() {
	static bool_e initialized = FALSE;

	if(initialized)
		return;
	initialized = TRUE;

	AX12_init();
	CLAP_initAX12();
}

// Fonction appellée si la carte IHM a détecté une grosse chutte de la tension d'alimentation des servos
// Pour éviter les problèmes d'utilisation de servo non initialisé
void CLAP_reset_config(){
	ax12_is_initialized = FALSE;
	CLAP_initAX12();
}

//Initialise l'AX12 s'il n'était pas alimenté lors d'initialisations précédentes, si déjà initialisé, ne fait rien
static void CLAP_initAX12() {
	if(ax12_is_initialized == FALSE && AX12_is_ready(CLAP_AX12_ID) == TRUE) {
		ax12_is_initialized = TRUE;
		AX12_config_set_lowest_voltage(CLAP_AX12_ID, AX12_MIN_VOLTAGE);
		AX12_config_set_highest_voltage(CLAP_AX12_ID, AX12_MAX_VOLTAGE);
		AX12_set_torque_limit(CLAP_AX12_ID, CLAP_AX12_MAX_TORQUE_PERCENT);

		AX12_config_set_maximal_angle(CLAP_AX12_ID, CLAP_AX12_MAX_VALUE);
		AX12_config_set_minimal_angle(CLAP_AX12_ID, CLAP_AX12_MIN_VALUE);

		AX12_config_set_error_before_led(CLAP_AX12_ID, AX12_BEFORE_LED);
		AX12_config_set_error_before_shutdown(CLAP_AX12_ID, AX12_BEFORE_SHUTDOWN);
		debug_printf("Init config DONE\n");
	}else if(ax12_is_initialized == FALSE)
		debug_printf("Init config FAIL\n");
}


// Fonction appellée pour la modification des configurations de l'ax12 telle que la vitesse et le couple (via ActManager)
// Dans le cas de multiple actionneur appartenant à un même actionneur, ajouter des defines dans QS_CANmsgList.h afin de pouvoir les choisirs facilement depuis la stratégie
void CLAP_config(CAN_msg_t* msg){
	switch(msg->data[1]){
		case 0 : // Premier élement de l'actionneur
			ACTMGR_config_AX12(CLAP_AX12_ID, msg);
			break;

		default :
			warn_printf("invalid CAN msg data[1]=%u (sous actionneur inexistant)!\n", msg->data[1]);
	}
}

// Fonction appellée pour l'initialisation en position de l'AX12 dés l'arrivé de l'alimentation (via ActManager)
void CLAP_init_pos(){
	CLAP_initAX12();

	if(ax12_is_initialized == FALSE)
		return;

	debug_printf("Init pos : \n");

	if(!AX12_set_position(CLAP_AX12_ID, CLAP_AX12_INIT_POS))
		debug_printf("   L'AX12 n°%d n'est pas là\n", CLAP_AX12_ID);
	else
		debug_printf("   L'AX12 n°%d a été initialisé en position\n", CLAP_AX12_ID);
}

// Fonction appellée à la fin du match (via ActManager)
void CLAP_stop(){
	AX12_set_torque_enabled(CLAP_AX12_ID, FALSE); //Stopper l'asservissement de l'AX12
}

// fonction appellée à la réception d'un message CAN (via ActManager)
bool_e CLAP_CAN_process_msg(CAN_msg_t* msg) {
	if(msg->sid == ACT_CLAP){
		CLAP_initAX12();
		switch(msg->data[0]) {
			// Listing de toutes les positions de l'actionneur possible
			case ACT_CLAP_OPEN :
			case ACT_CLAP_CLOSED:
			case ACT_CLAP_TAKE_CUP:
			case ACT_CLAP_STOP :
				ACTQ_push_operation_from_msg(msg, QUEUE_ACT_AX12_CLAP, &CLAP_run_command, 0,TRUE);
				break;

			case ACT_CONFIG :
				CLAP_config(msg);
				break;


			default:
				component_printf(LOG_LEVEL_Warning, "invalid CAN msg data[0]=%u !\n", msg->data[0]);
		}
		return TRUE;
	}else if(msg->sid == ACT_DO_SELFTEST){
		// Lister les différents états que l'actionneur doit réaliser pour réussir le selftest
		SELFTEST_set_actions(&CLAP_run_command, 0, 3, (SELFTEST_action_t[]){
								 {ACT_CLAP_CLOSED,		0,  QUEUE_ACT_AX12_CLAP},
								 {ACT_CLAP_OPEN,		0,  QUEUE_ACT_AX12_CLAP},
								 {ACT_CLAP_TAKE_CUP,	0,  QUEUE_ACT_AX12_CLAP},
								 {ACT_CLAP_CLOSED,		0,  QUEUE_ACT_AX12_CLAP}
							 });
	}
	return FALSE;
}

// Fonction appellée par la queue pendant tout le temps de la commande en cours (le booléen init est à TRUE au premier lancement de la commande)
void CLAP_run_command(queue_id_t queueId, bool_e init) {
	if(QUEUE_has_error(queueId)) {
		QUEUE_behead(queueId);
		return;
	}

	if(QUEUE_get_act(queueId) == QUEUE_ACT_AX12_CLAP) {    // Gestion des mouvements de CLAP
		if(init)
			CLAP_command_init(queueId);
		else
			CLAP_command_run(queueId);
	}
}

//Initialise une commande
static void CLAP_command_init(queue_id_t queueId) {
	Uint8 command = QUEUE_get_arg(queueId)->canCommand;
	Sint16* ax12_goalPosition = &QUEUE_get_arg(queueId)->param;

	*ax12_goalPosition = 0xFFFF;
	CLAP_initAX12();

	switch(command) {
		// Listing de toutes les positions de l'actionneur possible avec les valeurs de position associées
		case ACT_CLAP_CLOSED : *ax12_goalPosition = CLAP_AX12_CLOSE_POS; break;
		case ACT_CLAP_OPEN : *ax12_goalPosition = CLAP_AX12_OPEN_POS; break;
		case ACT_CLAP_TAKE_CUP : *ax12_goalPosition = CLAP_AX12_TAKE_CUP_POS; break;

		case ACT_CLAP_STOP :
			AX12_set_torque_enabled(CLAP_AX12_ID, FALSE); //Stopper l'asservissement de l'AX12
			QUEUE_next(queueId, ACT_CLAP, ACT_RESULT_DONE, ACT_RESULT_ERROR_OK, __LINE__);
			return;

		default: {
			error_printf("Invalid CLAP command: %u, code is broken !\n", command);
			QUEUE_next(queueId, ACT_CLAP, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_LOGIC, __LINE__);
			return;
		}
	}

	if(ax12_is_initialized == FALSE){
		error_printf("Impossible de mettre l'actionneur en position il n'est pas initialisé\n");
		QUEUE_next(queueId, ACT_CLAP, ACT_RESULT_FAILED, ACT_RESULT_ERROR_NOT_HERE, __LINE__);
		return;
	}

	if(*ax12_goalPosition == 0xFFFF) {
		error_printf("Invalid ax12 position for command: %u, code is broken !\n", command);
		QUEUE_next(queueId, ACT_CLAP, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_LOGIC, __LINE__);
		return;
	}

	AX12_reset_last_error(CLAP_AX12_ID); //Sécurité anti terroriste. Nous les parano on aime pas voir des erreurs là ou il n'y en a pas.
	if(!AX12_set_position(CLAP_AX12_ID, *ax12_goalPosition)) {	//Si la commande n'a pas été envoyée correctement et/ou que l'AX12 ne répond pas a cet envoi, on l'indique à la carte stratégie
		error_printf("AX12_set_position error: 0x%x\n", AX12_get_last_error(CLAP_AX12_ID).error);
		QUEUE_next(queueId, ACT_CLAP, ACT_RESULT_FAILED, ACT_RESULT_ERROR_NOT_HERE, __LINE__);
		return;
	}
	//La commande a été envoyée et l'AX12 l'a bien reçu
	debug_printf("Move CLAP ax12 to %d\n", *ax12_goalPosition);
}

//Gère les états pendant le mouvement de l'AX12
static void CLAP_command_run(queue_id_t queueId) {
	Uint8 result, errorCode;
	Uint16 line;

	if(ACTQ_check_status_ax12(queueId, CLAP_AX12_ID, QUEUE_get_arg(queueId)->param, CLAP_AX12_ASSER_POS_EPSILON, CLAP_AX12_ASSER_TIMEOUT, CLAP_AX12_ASSER_POS_LARGE_EPSILON, &result, &errorCode, &line))
		QUEUE_next(queueId, ACT_CLAP, result, errorCode, line);
}

#endif
