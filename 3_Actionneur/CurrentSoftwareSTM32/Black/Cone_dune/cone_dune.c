/*  Club Robot ESEO 2015 - 2016
 *	BIG
 *
 *	Fichier : cone_dune.c
 *	Package : Carte actionneur
 *	Description : Gestion de l'actionneur RX24 CONE_DUNE
 *  Auteur : Valentin
 *  Version 2016
 *  Robot : BIG
 */

#include "cone_dune.h"

// If def à mettre si l'actionneur est seulement présent sur le petit robot (I_AM_ROBOT_SMALL) ou le gros (I_AM_ROBOT_BIG)
#ifdef I_AM_ROBOT_BIG

// Les différents includes nécessaires...
#include "../QS/QS_CANmsgList.h"
#include "../QS/QS_rx24.h"
#include "../act_queue_utils.h"
#include "../selftest.h"
#include "../ActManager.h"

#include "cone_dune_config.h"

// Les différents define pour le verbose sur uart
#define LOG_PREFIX "CONE_DUNE.c : "
#define LOG_COMPONENT OUTPUT_LOG_COMPONENT_CONE_DUNE
#include "../QS/QS_outputlog.h"

// Les fonctions internes au fonctionnement de l'actionneur
static void CONE_DUNE_command_run(queue_id_t queueId);
static void CONE_DUNE_initRX24();
static void CONE_DUNE_command_init(queue_id_t queueId);
static void CONE_DUNE_config(CAN_msg_t* msg);

// Booléen contenant l'état actuel de l'initialisation du RX24 (Plusieurs booléens si plusieurs servo dans l'actionneur)
static bool_e rx24_is_initialized = FALSE;

// Fonction appellée au lancement de la carte (via ActManager)
void CONE_DUNE_init() {
	static bool_e initialized = FALSE;

	if(initialized)
		return;
	initialized = TRUE;

	RX24_init();
	CONE_DUNE_initRX24();
}

// Fonction appellée si la carte IHM a détecté une grosse chutte de la tension d'alimentation des servos
// Pour éviter les problèmes d'utilisation de servo non initialisé
void CONE_DUNE_reset_config(){
	rx24_is_initialized = FALSE;
	CONE_DUNE_initRX24();
}

//Initialise le RX24 s'il n'était pas alimenté lors d'initialisations précédentes, si déjà initialisé, ne fait rien
static void CONE_DUNE_initRX24() {
	if(rx24_is_initialized == FALSE && RX24_is_ready(CONE_DUNE_RX24_ID) == TRUE) {
		time32_t local_time = global.absolute_time;
		while(global.absolute_time - local_time < 1000);

		rx24_is_initialized = TRUE;
		RX24_config_set_lowest_voltage(CONE_DUNE_RX24_ID, RX24_MIN_VOLTAGE);
		RX24_config_set_highest_voltage(CONE_DUNE_RX24_ID, RX24_MAX_VOLTAGE);
		RX24_set_torque_limit(CONE_DUNE_RX24_ID, CONE_DUNE_RX24_MAX_TORQUE_PERCENT);
		RX24_config_set_temperature_limit(CONE_DUNE_RX24_ID, CONE_DUNE_RX24_MAX_TEMPERATURE);

		RX24_config_set_maximal_angle(CONE_DUNE_RX24_ID, CONE_DUNE_RX24_MAX_VALUE);
		RX24_config_set_minimal_angle(CONE_DUNE_RX24_ID, CONE_DUNE_RX24_MIN_VALUE);

		RX24_config_set_error_before_led(CONE_DUNE_RX24_ID, RX24_BEFORE_LED);
		RX24_config_set_error_before_shutdown(CONE_DUNE_RX24_ID, RX24_BEFORE_SHUTDOWN);
		debug_printf("Init config DONE\n");
	}else if(rx24_is_initialized == FALSE)
		debug_printf("Init config FAIL\n");
}


// Fonction appellée pour la modification des configurations du rx24 telle que la vitesse et le couple (via ActManager)
// Dans le cas de multiple actionneur appartenant à un même actionneur, ajouter des defines dans QS_CANmsgList.h afin de pouvoir les choisirs facilement depuis la stratégie
void CONE_DUNE_config(CAN_msg_t* msg){
	switch(msg->data.act_msg.act_data.act_config.sub_act_id){
		case DEFAULT_MONO_ACT : // Premier élement de l'actionneur
			ACTMGR_config_RX24(CONE_DUNE_RX24_ID, msg);
			break;

		default :
			warn_printf("invalid CAN msg data[1]=%u (sous actionneur inexistant)!\n", msg->data.act_msg.act_data.act_config.sub_act_id);
	}
}

// Fonction appellée pour l'initialisation en position du rx24 dés l'arrivé de l'alimentation (via ActManager)
void CONE_DUNE_init_pos(){
	CONE_DUNE_initRX24();

	if(rx24_is_initialized == FALSE)
		return;

	debug_printf("Init pos : \n");
	if(!RX24_set_position(CONE_DUNE_RX24_ID, CONE_DUNE_RX24_INIT_POS))
		debug_printf("   Le RX24 n°%d n'est pas là\n", CONE_DUNE_RX24_ID);
	else
		debug_printf("   Le RX24 n°%d a été initialisé en position\n", CONE_DUNE_RX24_ID);
}

// Fonction appellée à la fin du match (via ActManager)
void CONE_DUNE_stop(){
	RX24_set_torque_enabled(CONE_DUNE_RX24_ID, FALSE); //Stopper l'asservissement du RX24
}

// fonction appellée à la réception d'un message CAN (via ActManager)
bool_e CONE_DUNE_CAN_process_msg(CAN_msg_t* msg) {
	if(msg->sid == ACT_CONE_DUNE){
		CONE_DUNE_initRX24();
		switch(msg->data.act_msg.order) {
			// Listing de toutes les positions de l'actionneur possible
			case ACT_CONE_DUNE_IDLE :
			case ACT_CONE_DUNE_LOCK :
			case ACT_CONE_DUNE_UNLOCK :
			case ACT_CONE_DUNE_STOP :
				ACTQ_push_operation_from_msg(msg, QUEUE_ACT_RX24_CONE_DUNE, &CONE_DUNE_run_command, 0,TRUE);
				break;

			case ACT_CONFIG :
				CONE_DUNE_config(msg);
				break;


			default:
				component_printf(LOG_LEVEL_Warning, "invalid CAN msg data[0]=%u !\n", msg->data.act_msg.order);
		}
		return TRUE;
	}else if(msg->sid == ACT_DO_SELFTEST){
		// Lister les différents états que l'actionneur doit réaliser pour réussir le selftest
		/*SELFTEST_set_actions(&CONE_DUNE_run_command,  3, (SELFTEST_action_t[]){
								 {ACT_CONE_DUNE_IDLE,		0,  QUEUE_ACT_RX24_CONE_DUNE},
								 {ACT_CONE_DUNE_OPEN,       0,  QUEUE_ACT_RX24_CONE_DUNE},
								 {ACT_CONE_DUNE_IDLE,		0,  QUEUE_ACT_RX24_CONE_DUNE}
							 });*/
	}
	return FALSE;
}

// Fonction appellée par la queue pendant tout le temps de la commande en cours (le booléen init est à TRUE au premier lancement de la commande)
void CONE_DUNE_run_command(queue_id_t queueId, bool_e init) {
	if(QUEUE_has_error(queueId)) {
		QUEUE_behead(queueId);
		return;
	}

	if(QUEUE_get_act(queueId) == QUEUE_ACT_RX24_CONE_DUNE) {    // Gestion des mouvements de CONE_DUNE
		if(init)
			CONE_DUNE_command_init(queueId);
		else
			CONE_DUNE_command_run(queueId);
	}
}

//Initialise une commande
static void CONE_DUNE_command_init(queue_id_t queueId) {
	Uint8 command = QUEUE_get_arg(queueId)->canCommand;
	Sint16* rx24_goalPosition = &QUEUE_get_arg(queueId)->param;

	*rx24_goalPosition = 0xFFFF;
	CONE_DUNE_initRX24();

	switch(command) {
		// Listing de toutes les positions de l'actionneur possible avec les valeurs de position associées
		case ACT_CONE_DUNE_IDLE : *rx24_goalPosition = CONE_DUNE_RX24_IDLE_POS; break;
		case ACT_CONE_DUNE_LOCK : *rx24_goalPosition = CONE_DUNE_RX24_LOCK_POS; break;
		case ACT_CONE_DUNE_UNLOCK : *rx24_goalPosition = CONE_DUNE_RX24_UNLOCK_POS; break;

		case ACT_CONE_DUNE_STOP :
			RX24_set_torque_enabled(CONE_DUNE_RX24_ID, FALSE); //Stopper l'asservissement du RX24
			QUEUE_next(queueId, ACT_CONE_DUNE, ACT_RESULT_DONE, ACT_RESULT_ERROR_OK, __LINE__);
			return;

		default: {
			error_printf("Invalid CONE_DUNE command: %u, code is broken !\n", command);
			QUEUE_next(queueId, ACT_CONE_DUNE, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_LOGIC, __LINE__);
			return;
		}
	}

	if(rx24_is_initialized == FALSE){
		error_printf("Impossible de mettre l'actionneur en position il n'est pas initialisé\n");
		QUEUE_next(queueId, ACT_CONE_DUNE, ACT_RESULT_FAILED, ACT_RESULT_ERROR_NOT_HERE, __LINE__);
		return;
	}

	if(*rx24_goalPosition == 0xFFFF) {
		error_printf("Invalid rx24 position for command: %u, code is broken !\n", command);
		QUEUE_next(queueId, ACT_CONE_DUNE, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_LOGIC, __LINE__);
		return;
	}

	RX24_reset_last_error(CONE_DUNE_RX24_ID); //Sécurité anti terroriste. Nous les parano on aime pas voir des erreurs là ou il n'y en a pas.
	if(!RX24_set_position(CONE_DUNE_RX24_ID, *rx24_goalPosition)) {	//Si la commande n'a pas été envoyée correctement et/ou que le RX24 ne répond pas a cet envoi, on l'indique à la carte stratégie
		error_printf("RX24_set_position error: 0x%x\n", RX24_get_last_error(CONE_DUNE_RX24_ID).error);
		QUEUE_next(queueId, ACT_CONE_DUNE, ACT_RESULT_FAILED, ACT_RESULT_ERROR_NOT_HERE, __LINE__);
		return;
	}
	//La commande a été envoyée et le RX24 l'a bien reçu
	debug_printf("Move CONE_DUNE rx24 to %d\n", *rx24_goalPosition);
}

//Gère les états pendant le mouvement du RX24
static void CONE_DUNE_command_run(queue_id_t queueId) {
	Uint8 result, errorCode;
	Uint16 line;

	if(ACTQ_check_status_rx24(queueId, CONE_DUNE_RX24_ID, QUEUE_get_arg(queueId)->param, CONE_DUNE_RX24_ASSER_POS_EPSILON, CONE_DUNE_RX24_ASSER_TIMEOUT, CONE_DUNE_RX24_ASSER_POS_LARGE_EPSILON, &result, &errorCode, &line))
		QUEUE_next(queueId, ACT_CONE_DUNE, result, errorCode, line);
}

#endif


