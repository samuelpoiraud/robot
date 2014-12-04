/*  Club Robot ESEO 2013 - 2014
 *	SMALL
 *
 *	Fichier : Pince_Gauche.c
 *	Package : Carte actionneur
 *	Description : Gestion de l'ax12 de la pince gauche
 *  Auteur : Valentin
 *  Version 20130219
 *  Robot : SMALL
 */

#include "Pince_gauche.h"

#ifdef I_AM_ROBOT_SMALL

// Les différents includes nécessaires...
#include "../QS/QS_CANmsgList.h"
#include "../QS/QS_ax12.h"
#include "../act_queue_utils.h"
#include "../selftest.h"
#include "../ActManager.h"

#include "pince_gauche_config.h"

// Les différents define pour le verbose sur uart
#define LOG_PREFIX "Pince_gauche.c : "
#define LOG_COMPONENT OUTPUT_LOG_COMPONENT_PINCE_GAUCHE
#include "../QS/QS_outputlog.h"

// Les fonctions internes au fonctionnement de l'actionneur
static void PINCE_GAUCHE_command_run(queue_id_t queueId);
static void PINCE_GAUCHE_initAX12();
static void PINCE_GAUCHE_command_init(queue_id_t queueId);
static void PINCE_GAUCHE_config(CAN_msg_t* msg);

// Booléen contenant l'état actuel de l'initialisation de l'AX12 (Plusieurs booléens si plusieurs servo dans l'actionneur)
static bool_e ax12_is_initialized = FALSE;

// Fonction appellée au lancement de la carte (via ActManager)
void PINCE_GAUCHE_init() {
	static bool_e initialized = FALSE;

	if(initialized)
		return;
	initialized = TRUE;

	AX12_init();
	PINCE_GAUCHE_initAX12();
}

// Fonction appellée si la carte IHM a détecté une grosse chutte de la tension d'alimentation des servos
// Pour éviter les problèmes d'utilisation de servo non initialisé
void PINCE_GAUCHE_reset_config(){
	ax12_is_initialized = FALSE;
	PINCE_GAUCHE_initAX12();
}

//Initialise l'AX12 s'il n'était pas alimenté lors d'initialisations précédentes, si déjà initialisé, ne fait rien
static void PINCE_GAUCHE_initAX12() {
	if(ax12_is_initialized == FALSE && AX12_is_ready(PINCE_GAUCHE_AX12_ID) == TRUE) {
		ax12_is_initialized = TRUE;
		AX12_config_set_highest_voltage(PINCE_GAUCHE_AX12_ID, 136);
		AX12_config_set_lowest_voltage(PINCE_GAUCHE_AX12_ID, 70);
		AX12_config_set_maximum_torque_percentage(PINCE_GAUCHE_AX12_ID, PINCE_GAUCHE_AX12_MAX_TORQUE_PERCENT);

		AX12_config_set_maximal_angle(PINCE_GAUCHE_AX12_ID, PINCE_GAUCHE_AX12_MAX_VALUE);
		AX12_config_set_minimal_angle(PINCE_GAUCHE_AX12_ID, PINCE_GAUCHE_AX12_MIN_VALUE);

		AX12_config_set_error_before_led(PINCE_GAUCHE_AX12_ID, AX12_ERROR_ANGLE | AX12_ERROR_CHECKSUM | AX12_ERROR_INSTRUCTION | AX12_ERROR_OVERHEATING | AX12_ERROR_OVERLOAD | AX12_ERROR_RANGE);
		AX12_config_set_error_before_shutdown(PINCE_GAUCHE_AX12_ID, AX12_ERROR_OVERHEATING);
		debug_printf("Init config DONE\n");
	}else if(ax12_is_initialized == FALSE)
		debug_printf("Init config FAIL\n");
}


// Fonction appellée pour la modification des configurations de l'ax12 telle que la vitesse et le couple (via ActManager)
// Dans le cas de multiple actionneur appartenant à un même actionneur, ajouter des defines dans QS_CANmsgList.h afin de pouvoir les choisirs facilement depuis la stratégie
void PINCE_GAUCHE_config(CAN_msg_t* msg){
	switch(msg->data[1]){
		case 0 : // Premier élement de l'actionneur
			ACTMGR_config_AX12(PINCE_GAUCHE_AX12_ID, msg);
			break;

		default :
			warn_printf("invalid CAN msg data[1]=%u (sous actionneur inexistant)!\n", msg->data[1]);
	}
}

// Fonction appellée pour l'initialisation en position de l'AX12 dés l'arrivé de l'alimentation (via ActManager)
void PINCE_GAUCHE_init_pos(){
	PINCE_GAUCHE_initAX12();

	if(ax12_is_initialized == FALSE)
		return;

	debug_printf("Init pos : \n");
	if(!AX12_set_position(PINCE_GAUCHE_AX12_ID, PINCE_GAUCHE_AX12_INIT_POS))
		debug_printf("   L'AX12 n°%d n'est pas là\n", PINCE_GAUCHE_AX12_ID);
	else
		debug_printf("   L'AX12 n°%d a été initialisé en position\n", PINCE_GAUCHE_AX12_ID);
}

// Fonction appellée à la fin du match (via ActManager)
void PINCE_GAUCHE_stop(){
	AX12_set_torque_enabled(PINCE_GAUCHE_AX12_ID, FALSE); //Stopper l'asservissement de l'AX12
}

// fonction appellée à la réception d'un message CAN (via ActManager)
bool_e PINCE_GAUCHE_CAN_process_msg(CAN_msg_t* msg) {
	if(msg->sid == ACT_PINCE_GAUCHE){
		PINCE_GAUCHE_initAX12();
		switch(msg->data[0]) {
			// Listing de toutes les positions de l'actionneur possible
			case ACT_PINCE_GAUCHE_CLOSED :
			case ACT_PINCE_GAUCHE_OPEN :
			case ACT_PINCE_GAUCHE_MID_POS :
			case ACT_PINCE_GAUCHE_STOP :
				ACTQ_push_operation_from_msg(msg, QUEUE_ACT_AX12_PINCE_GAUCHE, &PINCE_GAUCHE_run_command, 0,TRUE);
				break;

			case ACT_CONFIG :
				PINCE_GAUCHE_config(msg);
				break;


			default:
				component_printf(LOG_LEVEL_Warning, "invalid CAN msg data[0]=%u !\n", msg->data[0]);
		}
		return TRUE;
	}else if(msg->sid == ACT_DO_SELFTEST){
		// Lister les différents états que l'actionneur doit réaliser pour réussir le selftest
		SELFTEST_set_actions(&PINCE_GAUCHE_run_command, 3, (SELFTEST_action_t[]){
								 {ACT_PINCE_GAUCHE_CLOSED,		0,  QUEUE_ACT_AX12_PINCE_GAUCHE},
								 {ACT_PINCE_GAUCHE_OPEN,		0,  QUEUE_ACT_AX12_PINCE_GAUCHE},
								 {ACT_PINCE_GAUCHE_CLOSED,		0,  QUEUE_ACT_AX12_PINCE_GAUCHE}
							 });
	}
	return FALSE;
}

// Fonction appellée par la queue pendant tout le temps de la commande en cours (le booléen init est à TRUE au premier lancement de la commande)
void PINCE_GAUCHE_run_command(queue_id_t queueId, bool_e init) {
	if(QUEUE_has_error(queueId)) {
		QUEUE_behead(queueId);
		return;
	}

	if(QUEUE_get_act(queueId) == QUEUE_ACT_AX12_PINCE_GAUCHE) {    // Gestion des mouvements de PINCE_GAUCHE
		if(init)
			PINCE_GAUCHE_command_init(queueId);
		else
			PINCE_GAUCHE_command_run(queueId);
	}
}

//Initialise une commande
static void PINCE_GAUCHE_command_init(queue_id_t queueId) {
	Uint8 command = QUEUE_get_arg(queueId)->canCommand;
	Uint16* ax12_goalPosition = &QUEUE_get_arg(queueId)->param;

	*ax12_goalPosition = 0xFFFF;
	PINCE_GAUCHE_initAX12();

	switch(command) {
		// Listing de toutes les positions de l'actionneur possible avec les valeurs de position associées
		case ACT_PINCE_GAUCHE_CLOSED : *ax12_goalPosition = PINCE_GAUCHE_AX12_IDLE_POS; break;
		case ACT_PINCE_GAUCHE_OPEN : *ax12_goalPosition = PINCE_GAUCHE_AX12_DEPLOYED_POS; break;
		case ACT_PINCE_GAUCHE_MID_POS : *ax12_goalPosition = PINCE_GAUCHE_AX12_MID_POS; break;

		case ACT_PINCE_GAUCHE_STOP :
			AX12_set_torque_enabled(PINCE_GAUCHE_AX12_ID, FALSE); //Stopper l'asservissement de l'AX12
			QUEUE_next(queueId, ACT_PINCE_GAUCHE, ACT_RESULT_DONE, ACT_RESULT_ERROR_OK, __LINE__);
			return;

		default: {
			error_printf("Invalid PINCE_GAUCHE command: %u, code is broken !\n", command);
			QUEUE_next(queueId, ACT_PINCE_GAUCHE, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_LOGIC, __LINE__);
			return;
		}
	}

	if(ax12_is_initialized == FALSE){
		error_printf("Impossible de mettre l'actionneur en position il n'est pas initialisé\n");
		QUEUE_next(queueId, ACT_PINCE_GAUCHE, ACT_RESULT_FAILED, ACT_RESULT_ERROR_NOT_HERE, __LINE__);
		return;
	}

	if(*ax12_goalPosition == 0xFFFF) {
		error_printf("Invalid ax12 position for command: %u, code is broken !\n", command);
		QUEUE_next(queueId, ACT_PINCE_GAUCHE, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_LOGIC, __LINE__);
		return;
	}

	AX12_reset_last_error(PINCE_GAUCHE_AX12_ID); //Sécurité anti terroriste. Nous les parano on aime pas voir des erreurs là ou il n'y en a pas.
	if(!AX12_set_position(PINCE_GAUCHE_AX12_ID, *ax12_goalPosition)) {	//Si la commande n'a pas été envoyée correctement et/ou que l'AX12 ne répond pas a cet envoi, on l'indique à la carte stratégie
		error_printf("AX12_set_position error: 0x%x\n", AX12_get_last_error(PINCE_GAUCHE_AX12_ID).error);
		QUEUE_next(queueId, ACT_PINCE_GAUCHE, ACT_RESULT_FAILED, ACT_RESULT_ERROR_NOT_HERE, __LINE__);
		return;
	}
	//La commande a été envoyée et l'AX12 l'a bien reçu
	debug_printf("Move Pince gauche ax12 to %d\n", *ax12_goalPosition);
}

//Gère les états pendant le mouvement de l'AX12
static void PINCE_GAUCHE_command_run(queue_id_t queueId) {
	Uint8 result, errorCode;
	Uint16 line;

	if(ACTQ_check_status_ax12(queueId, PINCE_GAUCHE_AX12_ID, QUEUE_get_arg(queueId)->param, PINCE_GAUCHE_AX12_ASSER_POS_EPSILON, PINCE_GAUCHE_AX12_ASSER_TIMEOUT, PINCE_GAUCHE_AX12_ASSER_POS_LARGE_EPSILON, &result, &errorCode, &line))
		QUEUE_next(queueId, ACT_PINCE_GAUCHE, result, errorCode, line);
}

#endif
