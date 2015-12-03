/*  Club Robot ESEO 2015 - 2016
 *	BIG
 *
 *	Fichier : TOP_PLATE_LEFT_RX24.c
 *	Package : Carte actionneur
 *	Description : Gestion de l'actionneur TOP_PLATE_LEFT
 *  Auteur : Julien
 *  Version 2016
 *  Robot : BIG
 */

#include "top_plate_left_rx24.h"


// TOP_PLATE_LEFT d'un actionneur standart avec AX12

// Ajout l'actionneur dans QS_CANmsgList.h
// Ajout d'une valeur dans l'énumération de la queue dans config_(big/small)/config_global_vars_types.h
// Formatage : QUEUE_ACT_AX12_TOP_PLATE_LEFT
// Ajout de la déclaration de l'actionneur dans ActManager dans le tableau actionneurs
// Ajout de la verbosité dans le fichier act_queue_utils.c dans la fonction ACTQ_internal_printResult
// Ajout du pilotage via terminal dans le fichier term_io.c dans le tableau terminal_motor du type : ACT_DECLARE(TOP_PLATE_LEFT)
// Un define TOP_PLATE_LEFT_AX12_ID doit avoir été ajouté au fichier config_big/config_pin.h // config_small/config_pin.h
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


// If def à mettre si l'actionneur est seulement présent sur le petit robot (I_AM_ROBOT_SMALL) ou le gros (I_AM_ROBOT_BIG)
#ifdef I_AM_ROBOT_BIG

// Les différents includes nécessaires...
#include "../QS/QS_CANmsgList.h"
#include "../QS/QS_ax12.h"
#include "../act_queue_utils.h"
#include "../selftest.h"
#include "../ActManager.h"

#include "top_plate_left_rx24_config.h"

// Les différents define pour le verbose sur uart
#define LOG_PREFIX "top_plate_left_rx24.c : "
#define LOG_COMPONENT OUTPUT_LOG_COMPONENT_TOP_PLATE_LEFT
#include "../QS/QS_outputlog.h"

// Les fonctions internes au fonctionnement de l'actionneur
static void TOP_PLATE_LEFT_command_run(queue_id_t queueId);
static void TOP_PLATE_LEFT_initRX24();
static void TOP_PLATE_LEFT_command_init(queue_id_t queueId);
static void TOP_PLATE_LEFT_config(CAN_msg_t* msg);

// Booléen contenant l'état actuel de l'initialisation du RX24 (Plusieurs booléens si plusieurs servo dans l'actionneur)
static bool_e rx24_is_initialized = FALSE;

// Fonction appellée au lancement de la carte (via ActManager)
void TOP_PLATE_LEFT_init() {
	static bool_e initialized = FALSE;

	if(initialized)
		return;
	initialized = TRUE;

	AX12_init();
	TOP_PLATE_LEFT_initRX24();
}

// Fonction appellée si la carte IHM a détecté une grosse chutte de la tension d'alimentation des servos
// Pour éviter les problèmes d'utilisation de servo non initialisé
void TOP_PLATE_LEFT_reset_config(){
	rx24_is_initialized = FALSE;
	TOP_PLATE_LEFT_initRX24();
}

//Initialise le RX24 s'il n'était pas alimenté lors d'initialisations précédentes, si déjà initialisé, ne fait rien
static void TOP_PLATE_LEFT_initRX24() {
	if(rx24_is_initialized == FALSE && AX12_is_ready(TOP_PLATE_LEFT_AX12_RX24_ID) == TRUE) {
		rx24_is_initialized = TRUE;
		AX12_config_set_lowest_voltage(TOP_PLATE_LEFT_AX12_RX24_ID, AX12_MIN_VOLTAGE);
		AX12_config_set_highest_voltage(TOP_PLATE_LEFT_AX12_RX24_ID, AX12_MAX_VOLTAGE);
		AX12_set_torque_limit(TOP_PLATE_LEFT_AX12_RX24_ID, TOP_PLATE_LEFT_AX12_RX24_MAX_TORQUE_PERCENT);
		AX12_config_set_temperature_limit(TOP_PLATE_LEFT_AX12_RX24_ID, TOP_PLATE_LEFT_AX12_RX24_MAX_TEMPERATURE);

		AX12_config_set_maximal_angle(TOP_PLATE_LEFT_AX12_RX24_ID, TOP_PLATE_LEFT_AX12_RX24_MAX_VALUE);
		AX12_config_set_minimal_angle(TOP_PLATE_LEFT_AX12_RX24_ID, TOP_PLATE_LEFT_AX12_RX24_MIN_VALUE);

		AX12_config_set_error_before_led(TOP_PLATE_LEFT_AX12_RX24_ID, AX12_BEFORE_LED);
		AX12_config_set_error_before_shutdown(TOP_PLATE_LEFT_AX12_RX24_ID, AX12_BEFORE_SHUTDOWN);
		debug_printf("Init config DONE\n");
	}else if(rx24_is_initialized == FALSE)
		debug_printf("Init config FAIL\n");
}


// Fonction appellée pour la modification des configurations du rx24 telle que la vitesse et le couple (via ActManager)
// Dans le cas de multiple actionneur appartenant à un même actionneur, ajouter des defines dans QS_CANmsgList.h afin de pouvoir les choisirs facilement depuis la stratégie
void TOP_PLATE_LEFT_config(CAN_msg_t* msg){
	switch(msg->data.act_msg.act_data.act_config.sub_act_id){
		case DEFAULT_MONO_ACT : // Premier élement de l'actionneur
			ACTMGR_config_AX12(TOP_PLATE_LEFT_AX12_RX24_ID, msg);
			break;

		default :
			warn_printf("invalid CAN msg data[1]=%u (sous actionneur inexistant)!\n", msg->data.act_msg.act_data.act_config.sub_act_id);
	}
}

// Fonction appellée pour l'initialisation en position du rx24 dés l'arrivé de l'alimentation (via ActManager)
void TOP_PLATE_LEFT_init_pos(){
	TOP_PLATE_LEFT_initRX24();

	if(rx24_is_initialized == FALSE)
		return;

	debug_printf("Init pos : \n");
	if(!AX12_set_position(TOP_PLATE_LEFT_AX12_RX24_ID, TOP_PLATE_LEFT_AX12_RX24_INIT_POS))
		debug_printf("   Le RX24 n°%d n'est pas là\n", TOP_PLATE_LEFT_AX12_RX24_ID);
	else
		debug_printf("   Le RX24 n°%d a été initialisé en position\n", TOP_PLATE_LEFT_AX12_RX24_ID);
}

// Fonction appellée à la fin du match (via ActManager)
void TOP_PLATE_LEFT_stop(){
	AX12_set_torque_enabled(TOP_PLATE_LEFT_AX12_RX24_ID, FALSE); //Stopper l'asservissement du RX24
}

// fonction appellée à la réception d'un message CAN (via ActManager)
bool_e TOP_PLATE_LEFT_CAN_process_msg(CAN_msg_t* msg) {
	if(msg->sid == ACT_TOP_PLATE_LEFT){
		TOP_PLATE_LEFT_initRX24();
		switch(msg->data.act_msg.order) {
			// Listing de toutes les positions de l'actionneur possible
			case ACT_TOP_PLATE_LEFT_IDLE :
			case ACT_TOP_PLATE_LEFT_OPEN :
			case ACT_TOP_PLATE_LEFT_CLOSE :
			case ACT_TOP_PLATE_LEFT_STOP :
				ACTQ_push_operation_from_msg(msg, QUEUE_ACT_AX12_RX24_TOP_PLATE_LEFT, &TOP_PLATE_LEFT_run_command, 0,TRUE);
				break;

			case ACT_CONFIG :
				TOP_PLATE_LEFT_config(msg);
				break;


			default:
				component_printf(LOG_LEVEL_Warning, "invalid CAN msg data[0]=%u !\n", msg->data.act_msg.order);
		}
		return TRUE;
	}else if(msg->sid == ACT_DO_SELFTEST){
		// Lister les différents états que l'actionneur doit réaliser pour réussir le selftest
		SELFTEST_set_actions(&TOP_PLATE_LEFT_run_command, 3, 3, (SELFTEST_action_t[]){
								 {ACT_TOP_PLATE_LEFT_IDLE,		0,  QUEUE_ACT_AX12_RX24_TOP_PLATE_LEFT},
								 {ACT_TOP_PLATE_LEFT_OPEN,       0,  QUEUE_ACT_AX12_RX24_TOP_PLATE_LEFT},
								 {ACT_TOP_PLATE_LEFT_IDLE,		0,  QUEUE_ACT_AX12_RX24_TOP_PLATE_LEFT}
							 });
	}
	return FALSE;
}

// Fonction appellée par la queue pendant tout le temps de la commande en cours (le booléen init est à TRUE au premier lancement de la commande)
void TOP_PLATE_LEFT_run_command(queue_id_t queueId, bool_e init) {
	if(QUEUE_has_error(queueId)) {
		QUEUE_behead(queueId);
		return;
	}

	if(QUEUE_get_act(queueId) == QUEUE_ACT_AX12_RX24_TOP_PLATE_LEFT) {    // Gestion des mouvements de TOP_PLATE_LEFT
		if(init)
			TOP_PLATE_LEFT_command_init(queueId);
		else
			TOP_PLATE_LEFT_command_run(queueId);
	}
}

//Initialise une commande
static void TOP_PLATE_LEFT_command_init(queue_id_t queueId) {
	Uint8 command = QUEUE_get_arg(queueId)->canCommand;
	Sint16* rx24_goalPosition = &QUEUE_get_arg(queueId)->param;

	*rx24_goalPosition = 0xFFFF;
	TOP_PLATE_LEFT_initRX24();

	switch(command) {
		// Listing de toutes les positions de l'actionneur possible avec les valeurs de position associées
		case ACT_TOP_PLATE_LEFT_IDLE : *rx24_goalPosition = TOP_PLATE_LEFT_AX12_RX24_IDLE_POS; break;
		case ACT_TOP_PLATE_LEFT_CLOSE : *rx24_goalPosition = TOP_PLATE_LEFT_AX12_RX24_CLOSE_POS; break;
		case ACT_TOP_PLATE_LEFT_OPEN : *rx24_goalPosition = TOP_PLATE_LEFT_AX12_RX24_OPEN_POS; break;

		case ACT_TOP_PLATE_LEFT_STOP :
			AX12_set_torque_enabled(TOP_PLATE_LEFT_AX12_RX24_ID, FALSE); //Stopper l'asservissement du RX24
			QUEUE_next(queueId, ACT_TOP_PLATE_LEFT, ACT_RESULT_DONE, ACT_RESULT_ERROR_OK, __LINE__);
			return;

		default: {
			error_printf("Invalid TOP_PLATE_LEFT command: %u, code is broken !\n", command);
			QUEUE_next(queueId, ACT_TOP_PLATE_LEFT, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_LOGIC, __LINE__);
			return;
		}
	}

	if(rx24_is_initialized == FALSE){
		error_printf("Impossible de mettre l'actionneur en position il n'est pas initialisé\n");
		QUEUE_next(queueId, ACT_TOP_PLATE_LEFT, ACT_RESULT_FAILED, ACT_RESULT_ERROR_NOT_HERE, __LINE__);
		return;
	}

	if(*rx24_goalPosition == 0xFFFF) {
		error_printf("Invalid rx24 position for command: %u, code is broken !\n", command);
		QUEUE_next(queueId, ACT_TOP_PLATE_LEFT, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_LOGIC, __LINE__);
		return;
	}

	AX12_reset_last_error(TOP_PLATE_LEFT_AX12_RX24_ID); //Sécurité anti terroriste. Nous les parano on aime pas voir des erreurs là ou il n'y en a pas.
	if(!AX12_set_position(TOP_PLATE_LEFT_AX12_RX24_ID, *rx24_goalPosition)) {	//Si la commande n'a pas été envoyée correctement et/ou que le RX24 ne répond pas a cet envoi, on l'indique à la carte stratégie
		error_printf("AX12_set_position error: 0x%x\n", AX12_get_last_error(TOP_PLATE_LEFT_AX12_RX24_ID).error);
		QUEUE_next(queueId, ACT_TOP_PLATE_LEFT, ACT_RESULT_FAILED, ACT_RESULT_ERROR_NOT_HERE, __LINE__);
		return;
	}
	//La commande a été envoyée et le RX24 l'a bien reçu
	debug_printf("Move TOP_PLATE_LEFT rx24 to %d\n", *rx24_goalPosition);
}

//Gère les états pendant le mouvement du RX24
static void TOP_PLATE_LEFT_command_run(queue_id_t queueId) {
	Uint8 result, errorCode;
	Uint16 line;

	if(ACTQ_check_status_ax12(queueId, TOP_PLATE_LEFT_AX12_RX24_ID, QUEUE_get_arg(queueId)->param, TOP_PLATE_LEFT_AX12_RX24_ASSER_POS_EPSILON, TOP_PLATE_LEFT_AX12_RX24_ASSER_TIMEOUT, TOP_PLATE_LEFT_AX12_RX24_ASSER_POS_LARGE_EPSILON, &result, &errorCode, &line))
		QUEUE_next(queueId, ACT_TOP_PLATE_LEFT, result, errorCode, line);
}


#endif
