/*  Club Robot ESEO 2015 - 2016
 *	BIG
 *
 *	Fichier : fich_unstick_arm.c
 *	Package : Carte actionneur
 *	Description : Gestion de l'actionneur FISH_UNSTICK_ARM
 *  Auteur : Cailyn
 *  Version 2016
 *  Robot : BIG
 */

#include "fish_unstick_arm.h"

// If def à mettre si l'actionneur est seulement présent sur le petit robot (I_AM_ROBOT_SMALL) ou le gros (I_AM_ROBOT_BIG)
#ifdef I_AM_ROBOT_BIG

// Les différents includes nécessaires...
#include "../QS/QS_CANmsgList.h"
#include "../QS/QS_rx24.h"
#include "../act_queue_utils.h"
#include "../selftest.h"
#include "../ActManager.h"

#include "fish_unstick_arm_config.h"

// Les différents define pour le verbose sur uart
#define LOG_PREFIX "fish_unstick_arm.c : "
#define LOG_COMPONENT OUTPUT_LOG_COMPONENT_FISH_UNSTICK_ARM
#include "../QS/QS_outputlog.h"

// Les fonctions internes au fonctionnement de l'actionneur
static void FISH_UNSTICK_ARM_command_run(queue_id_t queueId);
static void FISH_UNSTICK_ARM_initRX24();
static void FISH_UNSTICK_ARM_command_init(queue_id_t queueId);
static void FISH_UNSTICK_ARM_config(CAN_msg_t* msg);

// Booléen contenant l'état actuel de l'initialisation du RX24 (Plusieurs booléens si plusieurs servo dans l'actionneur)
static bool_e rx24_is_initialized = FALSE;

// Fonction appellée au lancement de la carte (via ActManager)
void FISH_UNSTICK_ARM_init() {
	static bool_e initialized = FALSE;

	if(initialized)
		return;
	initialized = TRUE;

	RX24_init();
	FISH_UNSTICK_ARM_initRX24();
}

// Fonction appellée si la carte IHM a détecté une grosse chutte de la tension d'alimentation des servos
// Pour éviter les problèmes d'utilisation de servo non initialisé
void FISH_UNSTICK_ARM_reset_config(){
	rx24_is_initialized = FALSE;
	FISH_UNSTICK_ARM_initRX24();
}

//Initialise le RX24 s'il n'était pas alimenté lors d'initialisations précédentes, si déjà initialisé, ne fait rien
static void FISH_UNSTICK_ARM_initRX24() {
	if(rx24_is_initialized == FALSE && RX24_is_ready(FISH_UNSTICK_ARM_RX24_ID) == TRUE) {
		rx24_is_initialized = TRUE;
		RX24_config_set_lowest_voltage(FISH_UNSTICK_ARM_RX24_ID, RX24_MIN_VOLTAGE);
		RX24_config_set_highest_voltage(FISH_UNSTICK_ARM_RX24_ID, RX24_MAX_VOLTAGE);
		RX24_set_torque_limit(FISH_UNSTICK_ARM_RX24_ID, FISH_UNSTICK_ARM_RX24_MAX_TORQUE_PERCENT);
		RX24_config_set_temperature_limit(FISH_UNSTICK_ARM_RX24_ID, FISH_UNSTICK_ARM_RX24_MAX_TEMPERATURE);
		RX24_set_move_to_position_speed(FISH_UNSTICK_ARM_RX24_ID,FISH_UNSTICK_ARM_RX24_MAX_SPEED);

		RX24_config_set_maximal_angle(FISH_UNSTICK_ARM_RX24_ID, FISH_UNSTICK_ARM_RX24_MAX_VALUE);
		RX24_config_set_minimal_angle(FISH_UNSTICK_ARM_RX24_ID, FISH_UNSTICK_ARM_RX24_MIN_VALUE);

		RX24_config_set_error_before_led(FISH_UNSTICK_ARM_RX24_ID, RX24_BEFORE_LED);
		RX24_config_set_error_before_shutdown(FISH_UNSTICK_ARM_RX24_ID, RX24_BEFORE_SHUTDOWN);
		debug_printf("Init config DONE\n");
	}else if(rx24_is_initialized == FALSE)
		debug_printf("Init config FAIL\n");
}


// Fonction appellée pour la modification des configurations du rx24 telle que la vitesse et le couple (via ActManager)
// Dans le cas de multiple actionneur appartenant à un même actionneur, ajouter des defines dans QS_CANmsgList.h afin de pouvoir les choisirs facilement depuis la stratégie
void FISH_UNSTICK_ARM_config(CAN_msg_t* msg){
	switch(msg->data.act_msg.act_data.act_config.sub_act_id){
		case DEFAULT_MONO_ACT : // Premier élement de l'actionneur
			ACTMGR_config_RX24(FISH_UNSTICK_ARM_RX24_ID, msg);
			break;

		default :
			warn_printf("invalid CAN msg data[1]=%u (sous actionneur inexistant)!\n", msg->data.act_msg.act_data.act_config.sub_act_id);
	}
}

// Fonction appellée pour l'initialisation en position du rx24 dés l'arrivé de l'alimentation (via ActManager)
void FISH_UNSTICK_ARM_init_pos(){
	FISH_UNSTICK_ARM_initRX24();

	if(rx24_is_initialized == FALSE)
		return;

	debug_printf("Init pos : \n");
	if(!RX24_set_position(FISH_UNSTICK_ARM_RX24_ID, FISH_UNSTICK_ARM_RX24_INIT_POS))
		debug_printf("   Le RX24 n°%d n'est pas là\n", FISH_UNSTICK_ARM_RX24_ID);
	else
		debug_printf("   Le RX24 n°%d a été initialisé en position\n", FISH_UNSTICK_ARM_RX24_ID);
}

// Fonction appellée à la fin du match (via ActManager)
void FISH_UNSTICK_ARM_stop(){
	RX24_set_torque_enabled(FISH_UNSTICK_ARM_RX24_ID, FALSE); //Stopper l'asservissement du RX24
}

// fonction appellée à la réception d'un message CAN (via ActManager)
bool_e FISH_UNSTICK_ARM_CAN_process_msg(CAN_msg_t* msg) {
	if(msg->sid == ACT_FISH_UNSTICK_ARM){
		FISH_UNSTICK_ARM_initRX24();
		switch(msg->data.act_msg.order) {
			// Listing de toutes les positions de l'actionneur possible
			case ACT_FISH_UNSTICK_ARM_IDLE :
			case ACT_FISH_UNSTICK_ARM_OPEN :
			case ACT_FISH_UNSTICK_ARM_CLOSE :
			case ACT_FISH_UNSTICK_ARM_STOP :
				ACTQ_push_operation_from_msg(msg, QUEUE_ACT_RX24_FISH_UNSTICK_ARM, &FISH_UNSTICK_ARM_run_command, 0,TRUE);
				break;

			case ACT_CONFIG :
				FISH_UNSTICK_ARM_config(msg);
				break;


			default:
				component_printf(LOG_LEVEL_Warning, "invalid CAN msg data[0]=%u !\n", msg->data.act_msg.order);
		}
		return TRUE;
	}else if(msg->sid == ACT_DO_SELFTEST){
		// Lister les différents états que l'actionneur doit réaliser pour réussir le selftest
		/*SELFTEST_set_actions(&FISH_UNSTICK_ARM_run_command, 3, (SELFTEST_action_t[]){
								 {ACT_FISH_UNSTICK_ARM_IDLE,		0,  QUEUE_ACT_RX24_FISH_UNSTICK_ARM},
								 {ACT_FISH_UNSTICK_ARM_OPEN,        0,  QUEUE_ACT_RX24_FISH_UNSTICK_ARM},
								 {ACT_FISH_UNSTICK_ARM_IDLE,		0,  QUEUE_ACT_RX24_FISH_UNSTICK_ARM}
							 });*/
	}
	return FALSE;
}

// Fonction appellée par la queue pendant tout le temps de la commande en cours (le booléen init est à TRUE au premier lancement de la commande)
void FISH_UNSTICK_ARM_run_command(queue_id_t queueId, bool_e init) {
	if(QUEUE_has_error(queueId)) {
		QUEUE_behead(queueId);
		return;
	}

	if(QUEUE_get_act(queueId) == QUEUE_ACT_RX24_FISH_UNSTICK_ARM) {    // Gestion des mouvements de FISH_UNSTICK_ARM
		if(init)
			FISH_UNSTICK_ARM_command_init(queueId);
		else
			FISH_UNSTICK_ARM_command_run(queueId);
	}
}

//Initialise une commande
static void FISH_UNSTICK_ARM_command_init(queue_id_t queueId) {
	Uint8 command = QUEUE_get_arg(queueId)->canCommand;
	Sint16* rx24_goalPosition = &QUEUE_get_arg(queueId)->param;

	*rx24_goalPosition = 0xFFFF;
	FISH_UNSTICK_ARM_initRX24();

	switch(command) {
		// Listing de toutes les positions de l'actionneur possible avec les valeurs de position associées
		case ACT_FISH_UNSTICK_ARM_IDLE : *rx24_goalPosition = FISH_UNSTICK_ARM_RX24_IDLE_POS; break;
		case ACT_FISH_UNSTICK_ARM_CLOSE : *rx24_goalPosition = FISH_UNSTICK_ARM_RX24_CLOSE_POS; break;
		case ACT_FISH_UNSTICK_ARM_OPEN : *rx24_goalPosition = FISH_UNSTICK_ARM_RX24_OPEN_POS; break;

		case ACT_FISH_UNSTICK_ARM_STOP :
			RX24_set_torque_enabled(FISH_UNSTICK_ARM_RX24_ID, FALSE); //Stopper l'asservissement du RX24
			QUEUE_next(queueId, ACT_FISH_UNSTICK_ARM, ACT_RESULT_DONE, ACT_RESULT_ERROR_OK, __LINE__);
			return;

		default: {
			error_printf("Invalid FISH_UNSTICK_ARM command: %u, code is broken !\n", command);
			QUEUE_next(queueId, ACT_FISH_UNSTICK_ARM, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_LOGIC, __LINE__);
			return;
		}
	}

	if(rx24_is_initialized == FALSE){
		error_printf("Impossible de mettre l'actionneur en position il n'est pas initialisé\n");
		QUEUE_next(queueId, ACT_FISH_UNSTICK_ARM, ACT_RESULT_FAILED, ACT_RESULT_ERROR_NOT_HERE, __LINE__);
		return;
	}

	if(*rx24_goalPosition == 0xFFFF) {
		error_printf("Invalid rx24 position for command: %u, code is broken !\n", command);
		QUEUE_next(queueId, ACT_FISH_UNSTICK_ARM, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_LOGIC, __LINE__);
		return;
	}

	RX24_reset_last_error(FISH_UNSTICK_ARM_RX24_ID); //Sécurité anti terroriste. Nous les parano on aime pas voir des erreurs là ou il n'y en a pas.
	if(!RX24_set_position(FISH_UNSTICK_ARM_RX24_ID, *rx24_goalPosition)) {	//Si la commande n'a pas été envoyée correctement et/ou que l'RX24 ne répond pas a cet envoi, on l'indique à la carte stratégie
		error_printf("RX24_set_position error: 0x%x\n", RX24_get_last_error(FISH_UNSTICK_ARM_RX24_ID).error);
		QUEUE_next(queueId, ACT_FISH_UNSTICK_ARM, ACT_RESULT_FAILED, ACT_RESULT_ERROR_NOT_HERE, __LINE__);
		return;
	}
	//La commande a été envoyée et le  l'a bien reçu
	debug_printf("Move FISH_UNSTICK_ARM rx24 to %d\n", *rx24_goalPosition);
}

//Gère les états pendant le mouvement du RX24
static void FISH_UNSTICK_ARM_command_run(queue_id_t queueId) {
	Uint8 result, errorCode;
	Uint16 line;

	if(ACTQ_check_status_rx24(queueId, FISH_UNSTICK_ARM_RX24_ID, QUEUE_get_arg(queueId)->param, FISH_UNSTICK_ARM_RX24_ASSER_POS_EPSILON, FISH_UNSTICK_ARM_RX24_ASSER_TIMEOUT, FISH_UNSTICK_ARM_RX24_ASSER_POS_LARGE_EPSILON, &result, &errorCode, &line))
		QUEUE_next(queueId, ACT_FISH_UNSTICK_ARM, result, errorCode, line);
}

#endif
