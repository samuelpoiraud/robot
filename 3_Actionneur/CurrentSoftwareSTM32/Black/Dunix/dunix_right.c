/*  Club Robot ESEO 2015 - 2016
 *	BIG
 *
 *	Fichier : dunix_right.c
 *	Package : Carte actionneur
 *	Description : Gestion de l'actionneur RX24 DUNIX_RIGHT
 *  Auteur : Valentin
 *  Version 2016
 *  Robot : BIG
 */

#include "dunix_right.h"


// DUNIX_RIGHT d'un actionneur standart avec RX24

// Ajout l'actionneur dans QS_CANmsgList.h
// Ajout d'une valeur dans l'énumération de la queue dans config_(big/small)/config_global_vars_types.h
// Formatage : QUEUE_ACT_RX24_DUNIX_RIGHT
// Ajout de la déclaration de l'actionneur dans ActManager dans le tableau actionneurs
// Ajout de la verbosité dans le fichier act_queue_utils.c dans la fonction ACTQ_internal_printResult
// Ajout du pilotage via terminal dans le fichier term_io.c dans le tableau terminal_motor du type : ACT_DECLARE(DUNIX_RIGHT)
// Un define DUNIX_RIGHT_RX24_ID doit avoir été ajouté au fichier config_big/config_pin.h // config_small/config_pin.h
// Ajout des postions dans QS_types.h dans l'énum ACT_order_e (avec "ACT_" et sans "_POS" à la fin)
// Mise à jour de config/config_debug.h

// Optionnel:
// Ajout du selftest dans le fichier selftest.c dans la fonction SELFTEST_done_test
// Ajout du selftest dans le fichier QS_CANmsgList (dans l'énumération SELFTEST)

// En stratégie
// ajout d'une d'une valeur dans le tableau act_link_SID_Queue du fichier act_functions.c/h
// ajout des fonctions actionneurs dans act_avoidance.c/h si l'actionneur modifie l'évitement du robot

// En stratégie Optionnel
// ajout du verbose du selftest dans Supervision/Selftest.c (tableau SELFTEST_getError_string, fonction SELFTEST_print_errors)
// ajout de la verbosité dans Supervision/Verbose_can_msg.c/h (fonction VERBOSE_CAN_MSG_sprint)


// If def à mettre si l'actionneur est seulement présent sur le petit robot (I_AM_ROBOT_SMALL) ou le gros (I_AM_ROBOT_BIG)
#ifdef I_AM_ROBOT_BIG

// Les différents includes nécessaires...
#include "../QS/QS_CANmsgList.h"
#include "../QS/QS_rx24.h"
#include "../act_queue_utils.h"
#include "../selftest.h"
#include "../ActManager.h"

#include "DUNIX_RIGHT_config.h"

// Les différents define pour le verbose sur uart
#define LOG_PREFIX "dunix_right.c : "
#define LOG_COMPONENT OUTPUT_LOG_COMPONENT_DUNIX_RIGHT
#include "../QS/QS_outputlog.h"

// Les fonctions internes au fonctionnement de l'actionneur
static void DUNIX_RIGHT_command_run(queue_id_t queueId);
static void DUNIX_RIGHT_initRX24();
static void DUNIX_RIGHT_command_init(queue_id_t queueId);
static void DUNIX_RIGHT_config(CAN_msg_t* msg);

// Booléen contenant l'état actuel de l'initialisation du RX24 (Plusieurs booléens si plusieurs servo dans l'actionneur)
static bool_e rx24_is_initialized = FALSE;

// Fonction appellée au lancement de la carte (via ActManager)
void DUNIX_RIGHT_init() {
	static bool_e initialized = FALSE;

	if(initialized)
		return;
	initialized = TRUE;

	RX24_init();
	DUNIX_RIGHT_initRX24();
}

// Fonction appellée si la carte IHM a détecté une grosse chutte de la tension d'alimentation des servos
// Pour éviter les problèmes d'utilisation de servo non initialisé
void DUNIX_RIGHT_reset_config(){
	rx24_is_initialized = FALSE;
	DUNIX_RIGHT_initRX24();
}

//Initialise le RX24 s'il n'était pas alimenté lors d'initialisations précédentes, si déjà initialisé, ne fait rien
static void DUNIX_RIGHT_initRX24() {
	if(rx24_is_initialized == FALSE && RX24_is_ready(DUNIX_RIGHT_RX24_ID) == TRUE) {
		rx24_is_initialized = TRUE;
		RX24_config_set_lowest_voltage(DUNIX_RIGHT_RX24_ID, RX24_MIN_VOLTAGE);
		RX24_config_set_highest_voltage(DUNIX_RIGHT_RX24_ID, RX24_MAX_VOLTAGE);
		RX24_set_torque_limit(DUNIX_RIGHT_RX24_ID, DUNIX_RIGHT_RX24_MAX_TORQUE_PERCENT);
		RX24_config_set_temperature_limit(DUNIX_RIGHT_RX24_ID, DUNIX_RIGHT_RX24_MAX_TEMPERATURE);

		RX24_config_set_maximal_angle(DUNIX_RIGHT_RX24_ID, DUNIX_RIGHT_RX24_MAX_VALUE);
		RX24_config_set_minimal_angle(DUNIX_RIGHT_RX24_ID, DUNIX_RIGHT_RX24_MIN_VALUE);

		RX24_config_set_error_before_led(DUNIX_RIGHT_RX24_ID, RX24_BEFORE_LED);
		RX24_config_set_error_before_shutdown(DUNIX_RIGHT_RX24_ID, RX24_BEFORE_SHUTDOWN);
		debug_printf("Init config DONE\n");
	}else if(rx24_is_initialized == FALSE)
		debug_printf("Init config FAIL\n");
}


// Fonction appellée pour la modification des configurations du rx24 telle que la vitesse et le couple (via ActManager)
// Dans le cas de multiple actionneur appartenant à un même actionneur, ajouter des defines dans QS_CANmsgList.h afin de pouvoir les choisirs facilement depuis la stratégie
void DUNIX_RIGHT_config(CAN_msg_t* msg){
	switch(msg->data.act_msg.act_data.act_config.sub_act_id){
		case DEFAULT_MONO_ACT : // Premier élement de l'actionneur
			ACTMGR_config_RX24(DUNIX_RIGHT_RX24_ID, msg);
			break;

		default :
			warn_printf("invalid CAN msg data[1]=%u (sous actionneur inexistant)!\n", msg->data.act_msg.act_data.act_config.sub_act_id);
	}
}

// Fonction appellée pour l'initialisation en position du rx24 dés l'arrivé de l'alimentation (via ActManager)
void DUNIX_RIGHT_init_pos(){
	DUNIX_RIGHT_initRX24();

	if(rx24_is_initialized == FALSE)
		return;

	debug_printf("Init pos : \n");
	if(!RX24_set_position(DUNIX_RIGHT_RX24_ID, DUNIX_RIGHT_RX24_INIT_POS))
		debug_printf("   Le RX24 n°%d n'est pas là\n", DUNIX_RIGHT_RX24_ID);
	else
		debug_printf("   Le RX24 n°%d a été initialisé en position\n", DUNIX_RIGHT_RX24_ID);
}

// Fonction appellée à la fin du match (via ActManager)
void DUNIX_RIGHT_stop(){
	RX24_set_torque_enabled(DUNIX_RIGHT_RX24_ID, FALSE); //Stopper l'asservissement du RX24
}

// fonction appellée à la réception d'un message CAN (via ActManager)
bool_e DUNIX_RIGHT_CAN_process_msg(CAN_msg_t* msg) {
	if(msg->sid == ACT_DUNIX_RIGHT){
		DUNIX_RIGHT_initRX24();
		switch(msg->data.act_msg.order) {
			// Listing de toutes les positions de l'actionneur possible
			case ACT_DUNIX_RIGHT_IDLE :
			case ACT_DUNIX_RIGHT_OPEN :
			case ACT_DUNIX_RIGHT_CLOSE :
			case ACT_DUNIX_RIGHT_STOP :
				ACTQ_push_operation_from_msg(msg, QUEUE_ACT_RX24_DUNIX_RIGHT, &DUNIX_RIGHT_run_command, 0,TRUE);
				break;

			case ACT_CONFIG :
				DUNIX_RIGHT_config(msg);
				break;


			default:
				component_printf(LOG_LEVEL_Warning, "invalid CAN msg data[0]=%u !\n", msg->data.act_msg.order);
		}
		return TRUE;
	}else if(msg->sid == ACT_DO_SELFTEST){
		// Lister les différents états que l'actionneur doit réaliser pour réussir le selftest
		/*SELFTEST_set_actions(&DUNIX_RIGHT_run_command,  3, (SELFTEST_action_t[]){
								 {ACT_DUNIX_RIGHT_IDLE,		0,  QUEUE_ACT_RX24_DUNIX_RIGHT},
								 {ACT_DUNIX_RIGHT_OPEN,       0,  QUEUE_ACT_RX24_DUNIX_RIGHT},
								 {ACT_DUNIX_RIGHT_IDLE,		0,  QUEUE_ACT_RX24_DUNIX_RIGHT}
							 });*/
	}
	return FALSE;
}

// Fonction appellée par la queue pendant tout le temps de la commande en cours (le booléen init est à TRUE au premier lancement de la commande)
void DUNIX_RIGHT_run_command(queue_id_t queueId, bool_e init) {
	if(QUEUE_has_error(queueId)) {
		QUEUE_behead(queueId);
		return;
	}

	if(QUEUE_get_act(queueId) == QUEUE_ACT_RX24_DUNIX_RIGHT) {    // Gestion des mouvements de DUNIX_RIGHT
		if(init)
			DUNIX_RIGHT_command_init(queueId);
		else
			DUNIX_RIGHT_command_run(queueId);
	}
}

//Initialise une commande
static void DUNIX_RIGHT_command_init(queue_id_t queueId) {
	Uint8 command = QUEUE_get_arg(queueId)->canCommand;
	Sint16* rx24_goalPosition = &QUEUE_get_arg(queueId)->param;

	*rx24_goalPosition = 0xFFFF;
	DUNIX_RIGHT_initRX24();

	switch(command) {
		// Listing de toutes les positions de l'actionneur possible avec les valeurs de position associées
		case ACT_DUNIX_RIGHT_IDLE : *rx24_goalPosition = DUNIX_RIGHT_RX24_IDLE_POS; break;
		case ACT_DUNIX_RIGHT_CLOSE : *rx24_goalPosition = DUNIX_RIGHT_RX24_CLOSE_POS; break;
		case ACT_DUNIX_RIGHT_OPEN : *rx24_goalPosition = DUNIX_RIGHT_RX24_OPEN_POS; break;

		case ACT_DUNIX_RIGHT_STOP :
			RX24_set_torque_enabled(DUNIX_RIGHT_RX24_ID, FALSE); //Stopper l'asservissement du RX24
			QUEUE_next(queueId, ACT_DUNIX_RIGHT, ACT_RESULT_DONE, ACT_RESULT_ERROR_OK, __LINE__);
			return;

		default: {
			error_printf("Invalid DUNIX_RIGHT command: %u, code is broken !\n", command);
			QUEUE_next(queueId, ACT_DUNIX_RIGHT, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_LOGIC, __LINE__);
			return;
		}
	}

	if(rx24_is_initialized == FALSE){
		error_printf("Impossible de mettre l'actionneur en position il n'est pas initialisé\n");
		QUEUE_next(queueId, ACT_DUNIX_RIGHT, ACT_RESULT_FAILED, ACT_RESULT_ERROR_NOT_HERE, __LINE__);
		return;
	}

	if(*rx24_goalPosition == 0xFFFF) {
		error_printf("Invalid rx24 position for command: %u, code is broken !\n", command);
		QUEUE_next(queueId, ACT_DUNIX_RIGHT, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_LOGIC, __LINE__);
		return;
	}

	RX24_reset_last_error(DUNIX_RIGHT_RX24_ID); //Sécurité anti terroriste. Nous les parano on aime pas voir des erreurs là ou il n'y en a pas.
	if(!RX24_set_position(DUNIX_RIGHT_RX24_ID, *rx24_goalPosition)) {	//Si la commande n'a pas été envoyée correctement et/ou que le RX24 ne répond pas a cet envoi, on l'indique à la carte stratégie
		error_printf("RX24_set_position error: 0x%x\n", RX24_get_last_error(DUNIX_RIGHT_RX24_ID).error);
		QUEUE_next(queueId, ACT_DUNIX_RIGHT, ACT_RESULT_FAILED, ACT_RESULT_ERROR_NOT_HERE, __LINE__);
		return;
	}
	//La commande a été envoyée et le RX24 l'a bien reçu
	debug_printf("Move DUNIX_RIGHT rx24 to %d\n", *rx24_goalPosition);
}

//Gère les états pendant le mouvement du RX24
static void DUNIX_RIGHT_command_run(queue_id_t queueId) {
	Uint8 result, errorCode;
	Uint16 line;

	if(ACTQ_check_status_rx24(queueId, DUNIX_RIGHT_RX24_ID, QUEUE_get_arg(queueId)->param, DUNIX_RIGHT_RX24_ASSER_POS_EPSILON, DUNIX_RIGHT_RX24_ASSER_TIMEOUT, DUNIX_RIGHT_RX24_ASSER_POS_LARGE_EPSILON, &result, &errorCode, &line))
		QUEUE_next(queueId, ACT_DUNIX_RIGHT, result, errorCode, line);
}

#endif

