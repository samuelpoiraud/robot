/*  Club Robot ESEO 2015 - 2016
 *	Fichier : black_sand_circle.c
 *	Package : Carte actionneur
 *	Description : Gestion de l'actionneur AX12 BLACK_SAND_CIRCLE
 *  Auteur : Valentin
 *  Version 2016
 *  Robot : BIG
 */

#include "black_sand_circle.h"

// If def à mettre si l'actionneur est seulement présent sur le petit robot (I_AM_ROBOT_SMALL) ou le gros (I_AM_ROBOT_BIG)
#ifdef I_AM_ROBOT_BIG

// Les différents includes nécessaires...
#include "../QS/QS_CANmsgList.h"
#include "../QS/QS_ax12.h"
#include "../act_queue_utils.h"
#include "../selftest.h"
#include "../ActManager.h"

#include "black_sand_circle_config.h"

// Les différents define pour le verbose sur uart
#define LOG_PREFIX "black_sand_circle.c : "
#define LOG_COMPONENT OUTPUT_LOG_COMPONENT_BLACK_SAND_CIRCLE
#include "../QS/QS_outputlog.h"

// Les fonctions internes au fonctionnement de l'actionneur
static void BLACK_SAND_CIRCLE_command_run(queue_id_t queueId);
static void BLACK_SAND_CIRCLE_initAX12();
static void BLACK_SAND_CIRCLE_command_init(queue_id_t queueId);
static void BLACK_SAND_CIRCLE_config(CAN_msg_t* msg);

// Booléen contenant l'état actuel de l'initialisation de l'AX12 (Plusieurs booléens si plusieurs servo dans l'actionneur)
static bool_e ax12_is_initialized = FALSE;

// Fonction appellée au lancement de la carte (via ActManager)
void BLACK_SAND_CIRCLE_init() {
	static bool_e initialized = FALSE;

	if(initialized)
		return;
	initialized = TRUE;

	AX12_init();
	BLACK_SAND_CIRCLE_initAX12();
}

// Fonction appellée si la carte IHM a détecté une grosse chutte de la tension d'alimentation des servos
// Pour éviter les problèmes d'utilisation de servo non initialisé
void BLACK_SAND_CIRCLE_reset_config(){
	ax12_is_initialized = FALSE;
	BLACK_SAND_CIRCLE_initAX12();
}

//Initialise l'AX12 s'il n'était pas alimenté lors d'initialisations précédentes, si déjà initialisé, ne fait rien
static void BLACK_SAND_CIRCLE_initAX12() {
	if(ax12_is_initialized == FALSE && AX12_is_ready(BLACK_SAND_CIRCLE_AX12_ID) == TRUE) {
		ax12_is_initialized = TRUE;
		AX12_config_set_lowest_voltage(BLACK_SAND_CIRCLE_AX12_ID, AX12_MIN_VOLTAGE);
		AX12_config_set_highest_voltage(BLACK_SAND_CIRCLE_AX12_ID, AX12_MAX_VOLTAGE);
		AX12_set_torque_limit(BLACK_SAND_CIRCLE_AX12_ID, BLACK_SAND_CIRCLE_AX12_MAX_TORQUE_PERCENT);
		AX12_config_set_temperature_limit(BLACK_SAND_CIRCLE_AX12_ID, BLACK_SAND_CIRCLE_AX12_MAX_TEMPERATURE);

		AX12_config_set_maximal_angle(BLACK_SAND_CIRCLE_AX12_ID, BLACK_SAND_CIRCLE_AX12_MAX_VALUE);
		AX12_config_set_minimal_angle(BLACK_SAND_CIRCLE_AX12_ID, BLACK_SAND_CIRCLE_AX12_MIN_VALUE);

		AX12_config_set_error_before_led(BLACK_SAND_CIRCLE_AX12_ID, AX12_BEFORE_LED);
		AX12_config_set_error_before_shutdown(BLACK_SAND_CIRCLE_AX12_ID, AX12_BEFORE_SHUTDOWN);
		debug_printf("Init config DONE\n");
	}else if(ax12_is_initialized == FALSE)
		debug_printf("Init config FAIL\n");
}


// Fonction appellée pour la modification des configurations de l'ax12 telle que la vitesse et le couple (via ActManager)
// Dans le cas de multiple actionneur appartenant à un même actionneur, ajouter des defines dans QS_CANmsgList.h afin de pouvoir les choisirs facilement depuis la stratégie
void BLACK_SAND_CIRCLE_config(CAN_msg_t* msg){
	switch(msg->data.act_msg.act_data.act_config.sub_act_id){
		case DEFAULT_MONO_ACT : // Premier élement de l'actionneur
			ACTMGR_config_AX12(BLACK_SAND_CIRCLE_AX12_ID, msg);
			break;

		default :
			warn_printf("invalid CAN msg data[1]=%u (sous actionneur inexistant)!\n", msg->data.act_msg.act_data.act_config.sub_act_id);
	}
}

// Fonction appellée pour l'initialisation en position de l'AX12 dés l'arrivé de l'alimentation (via ActManager)
void BLACK_SAND_CIRCLE_init_pos(){
	BLACK_SAND_CIRCLE_initAX12();

	if(ax12_is_initialized == FALSE)
		return;

	debug_printf("Init pos : \n");
	if(!AX12_set_position(BLACK_SAND_CIRCLE_AX12_ID, BLACK_SAND_CIRCLE_AX12_INIT_POS))
		debug_printf("   L'AX12 n°%d n'est pas là\n", BLACK_SAND_CIRCLE_AX12_ID);
	else
		debug_printf("   L'AX12 n°%d a été initialisé en position\n", BLACK_SAND_CIRCLE_AX12_ID);
}

// Fonction appellée à la fin du match (via ActManager)
void BLACK_SAND_CIRCLE_stop(){
	AX12_set_torque_enabled(BLACK_SAND_CIRCLE_AX12_ID, FALSE); //Stopper l'asservissement de l'AX12
}

// fonction appellée à la réception d'un message CAN (via ActManager)
bool_e BLACK_SAND_CIRCLE_CAN_process_msg(CAN_msg_t* msg) {
	if(msg->sid == ACT_BLACK_SAND_CIRCLE){
		BLACK_SAND_CIRCLE_initAX12();
		switch(msg->data.act_msg.order) {
			// Listing de toutes les positions de l'actionneur possible
			case ACT_BLACK_SAND_CIRCLE_IDLE :
			case ACT_BLACK_SAND_CIRCLE_OPEN :
			case ACT_BLACK_SAND_CIRCLE_CLOSE :
			case ACT_BLACK_SAND_CIRCLE_STOP :
				ACTQ_push_operation_from_msg(msg, QUEUE_ACT_AX12_BLACK_SAND_CIRCLE, &BLACK_SAND_CIRCLE_run_command, 0,TRUE);
				break;

			case ACT_CONFIG :
				BLACK_SAND_CIRCLE_config(msg);
				break;


			default:
				component_printf(LOG_LEVEL_Warning, "invalid CAN msg data[0]=%u !\n", msg->data.act_msg.order);
		}
		return TRUE;
	}else if(msg->sid == ACT_DO_SELFTEST){
		// Lister les différents états que l'actionneur doit réaliser pour réussir le selftest
		/*SELFTEST_set_actions(&BLACK_SAND_CIRCLE_run_command, 3, (SELFTEST_action_t[]){
								 {ACT_BLACK_SAND_CIRCLE_IDLE,		0,  QUEUE_ACT_AX12_BLACK_SAND_CIRCLE},
								 {ACT_BLACK_SAND_CIRCLE_OPEN,       0,  QUEUE_ACT_AX12_BLACK_SAND_CIRCLE},
								 {ACT_BLACK_SAND_CIRCLE_IDLE,		0,  QUEUE_ACT_AX12_BLACK_SAND_CIRCLE}
							 });*/
	}
	return FALSE;
}

// Fonction appellée par la queue pendant tout le temps de la commande en cours (le booléen init est à TRUE au premier lancement de la commande)
void BLACK_SAND_CIRCLE_run_command(queue_id_t queueId, bool_e init) {
	if(QUEUE_has_error(queueId)) {
		QUEUE_behead(queueId);
		return;
	}

	if(QUEUE_get_act(queueId) == QUEUE_ACT_AX12_BLACK_SAND_CIRCLE) {    // Gestion des mouvements de BLACK_SAND_CIRCLE
		if(init)
			BLACK_SAND_CIRCLE_command_init(queueId);
		else
			BLACK_SAND_CIRCLE_command_run(queueId);
	}
}

//Initialise une commande
static void BLACK_SAND_CIRCLE_command_init(queue_id_t queueId) {
	Uint8 command = QUEUE_get_arg(queueId)->canCommand;
	Sint16* ax12_goalPosition = &QUEUE_get_arg(queueId)->param;

	*ax12_goalPosition = 0xFFFF;
	BLACK_SAND_CIRCLE_initAX12();

	switch(command) {
		// Listing de toutes les positions de l'actionneur possible avec les valeurs de position associées
		case ACT_BLACK_SAND_CIRCLE_IDLE : *ax12_goalPosition = BLACK_SAND_CIRCLE_AX12_IDLE_POS; break;
		case ACT_BLACK_SAND_CIRCLE_CLOSE : *ax12_goalPosition = BLACK_SAND_CIRCLE_AX12_CLOSE_POS; break;
		case ACT_BLACK_SAND_CIRCLE_OPEN : *ax12_goalPosition = BLACK_SAND_CIRCLE_AX12_OPEN_POS; break;

		case ACT_BLACK_SAND_CIRCLE_STOP :
			AX12_set_torque_enabled(BLACK_SAND_CIRCLE_AX12_ID, FALSE); //Stopper l'asservissement de l'AX12
			QUEUE_next(queueId, ACT_BLACK_SAND_CIRCLE, ACT_RESULT_DONE, ACT_RESULT_ERROR_OK, __LINE__);
			return;

		default: {
			error_printf("Invalid BLACK_SAND_CIRCLE command: %u, code is broken !\n", command);
			QUEUE_next(queueId, ACT_BLACK_SAND_CIRCLE, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_LOGIC, __LINE__);
			return;
		}
	}

	if(ax12_is_initialized == FALSE){
		error_printf("Impossible de mettre l'actionneur en position il n'est pas initialisé\n");
		QUEUE_next(queueId, ACT_BLACK_SAND_CIRCLE, ACT_RESULT_FAILED, ACT_RESULT_ERROR_NOT_HERE, __LINE__);
		return;
	}

	if(*ax12_goalPosition == 0xFFFF) {
		error_printf("Invalid ax12 position for command: %u, code is broken !\n", command);
		QUEUE_next(queueId, ACT_BLACK_SAND_CIRCLE, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_LOGIC, __LINE__);
		return;
	}

	AX12_reset_last_error(BLACK_SAND_CIRCLE_AX12_ID); //Sécurité anti terroriste. Nous les parano on aime pas voir des erreurs là ou il n'y en a pas.
	if(!AX12_set_position(BLACK_SAND_CIRCLE_AX12_ID, *ax12_goalPosition)) {	//Si la commande n'a pas été envoyée correctement et/ou que l'AX12 ne répond pas a cet envoi, on l'indique à la carte stratégie
		error_printf("AX12_set_position error: 0x%x\n", AX12_get_last_error(BLACK_SAND_CIRCLE_AX12_ID).error);
		QUEUE_next(queueId, ACT_BLACK_SAND_CIRCLE, ACT_RESULT_FAILED, ACT_RESULT_ERROR_NOT_HERE, __LINE__);
		return;
	}
	//La commande a été envoyée et l'AX12 l'a bien reçu
	debug_printf("Move BLACK_SAND_CIRCLE ax12 to %d\n", *ax12_goalPosition);
}

//Gère les états pendant le mouvement de l'AX12
static void BLACK_SAND_CIRCLE_command_run(queue_id_t queueId) {
	Uint8 result, errorCode;
	Uint16 line;

	if(ACTQ_check_status_ax12(queueId, BLACK_SAND_CIRCLE_AX12_ID, QUEUE_get_arg(queueId)->param, BLACK_SAND_CIRCLE_AX12_ASSER_POS_EPSILON, BLACK_SAND_CIRCLE_AX12_ASSER_TIMEOUT, BLACK_SAND_CIRCLE_AX12_ASSER_POS_LARGE_EPSILON, &result, &errorCode, &line))
		QUEUE_next(queueId, ACT_BLACK_SAND_CIRCLE, result, errorCode, line);
}

#endif


