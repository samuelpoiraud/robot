/*  Club Robot ESEO 2014 - 2015
 *	SMALL
 *
 *	Fichier : Pop_drop_left_Wood.c
 *	Package : Carte actionneur
 *	Description : Gestion du bras pour faire tomber les pop corns dans le gobelet gauche
 *  Auteur : Valentin
 *  Version 2015
 *  Robot : SMALL
 */



#include "Pop_drop_left_Wood.h"

// POP_DROP_LEFT_WOOD d'un actionneur standart avec AX12

// Ajout l'actionneur dans QS_CANmsgList.h ainsi que toutes ses différentes position
// Ajout d'une valeur dans l'énumération de la queue dans config_(big/small)/config_global_vars_types.h
// Formatage : QUEUE_ACT_AX12_POP_DROP_LEFT_WOOD
// Ajout de la déclaration de l'actionneur dans ActManager dans le tableau actionneurs
// Ajout de la verbosité dans le fichier act_queue_utils.c dans la fonction ACTQ_internal_printResult
// Ajout du selftest dans le fichier selftest.c dans la fonction SELFTEST_done_test
// Ajout du selftest dans le fichier QS_CANmsgList (dans l'énumération SELFEST)
// Ajout du pilotage via terminal dans le fichier term_io.c dans le tableau terminal_motor
// Un define POP_DROP_LEFT_WOOD_AX12_ID doit avoir été ajouté au fichier config_big/config_pin.h // config_small/config_pin.h

// En stratégie
// ajout des fonctions actionneurs dans act_functions.c/h
// ajout des fonctions actionneurs dans act_can.c (fonction ACT_process_result)
// ajout des fonctions actionneurs dans act_avoidance.c/h si l'actionneur modifie l'évitement du robot
// ajout du verbose du selftest dans Supervision/Selftest.c (tableau SELFTEST_getError_string, fonction SELFTEST_print_errors)
// ajout de la verbosité dans Supervision/Verbose_can_msg.c/h (fonction VERBOSE_CAN_MSG_sprint)

// If def à mettre si l'actionneur est seulement présent sur le petit robot (I_AM_ROBOT_SMALL) ou le gros (I_AM_ROBOT_BIG)
#ifdef I_AM_ROBOT_SMALL

// Les différents includes nécessaires...
#include "../QS/QS_CANmsgList.h"
#include "../QS/QS_ax12.h"
#include "../act_queue_utils.h"
#include "../selftest.h"
#include "../ActManager.h"

#include "Pop_drop_left_Wood_config.h"

// Les différents define pour le verbose sur uart
#define LOG_PREFIX "Pop_drop_left_Wood.c : "
#define LOG_COMPONENT OUTPUT_LOG_COMPONENT_POP_DROP_LEFT_WOOD
#include "../QS/QS_outputlog.h"

// Les fonctions internes au fonctionnement de l'actionneur
static void POP_DROP_LEFT_WOOD_command_run(queue_id_t queueId);
static void POP_DROP_LEFT_WOOD_initAX12();
static void POP_DROP_LEFT_WOOD_command_init(queue_id_t queueId);
static void POP_DROP_LEFT_WOOD_config(CAN_msg_t* msg);

// Booléen contenant l'état actuel de l'initialisation de l'AX12 (Plusieurs booléens si plusieurs servo dans l'actionneur)
static bool_e ax12_is_initialized = FALSE;

// Fonction appellée au lancement de la carte (via ActManager)
void POP_DROP_LEFT_WOOD_init() {
	static bool_e initialized = FALSE;

	if(initialized)
		return;
	initialized = TRUE;

	AX12_init();
	POP_DROP_LEFT_WOOD_initAX12();
}

// Fonction appellée si la carte IHM a détecté une grosse chutte de la tension d'alimentation des servos
// Pour éviter les problèmes d'utilisation de servo non initialisé
void POP_DROP_LEFT_WOOD_reset_config(){
	ax12_is_initialized = FALSE;
	POP_DROP_LEFT_WOOD_initAX12();
}

//Initialise l'AX12 s'il n'était pas alimenté lors d'initialisations précédentes, si déjà initialisé, ne fait rien
static void POP_DROP_LEFT_WOOD_initAX12() {
	if(ax12_is_initialized == FALSE && AX12_is_ready(POP_DROP_LEFT_WOOD_AX12_ID) == TRUE) {
		ax12_is_initialized = TRUE;
		AX12_config_set_highest_voltage(POP_DROP_LEFT_WOOD_AX12_ID, 136);
		AX12_config_set_lowest_voltage(POP_DROP_LEFT_WOOD_AX12_ID, 70);
		AX12_config_set_maximum_torque_percentage(POP_DROP_LEFT_WOOD_AX12_ID, POP_DROP_LEFT_WOOD_AX12_MAX_TORQUE_PERCENT);

		AX12_config_set_maximal_angle(POP_DROP_LEFT_WOOD_AX12_ID, POP_DROP_LEFT_WOOD_AX12_MAX_VALUE);
		AX12_config_set_minimal_angle(POP_DROP_LEFT_WOOD_AX12_ID, POP_DROP_LEFT_WOOD_AX12_MIN_VALUE);

		AX12_config_set_error_before_led(POP_DROP_LEFT_WOOD_AX12_ID, AX12_ERROR_ANGLE | AX12_ERROR_CHECKSUM | AX12_ERROR_INSTRUCTION | AX12_ERROR_OVERHEATING | AX12_ERROR_OVERLOAD | AX12_ERROR_RANGE);
		AX12_config_set_error_before_shutdown(POP_DROP_LEFT_WOOD_AX12_ID, AX12_ERROR_OVERHEATING);
		debug_printf("Init config DONE\n");
	}else if(ax12_is_initialized == FALSE)
		debug_printf("Init config FAIL\n");
}


// Fonction appellée pour la modification des configurations de l'ax12 telle que la vitesse et le couple (via ActManager)
// Dans le cas de multiple actionneur appartenant à un même actionneur, ajouter des defines dans QS_CANmsgList.h afin de pouvoir les choisirs facilement depuis la stratégie
void POP_DROP_LEFT_WOOD_config(CAN_msg_t* msg){
	switch(msg->data[1]){
		case 0 : // Premier élement de l'actionneur
			ACTMGR_config_AX12(POP_DROP_LEFT_WOOD_AX12_ID, msg);
			break;

		default :
			warn_printf("invalid CAN msg data[1]=%u (sous actionneur inexistant)!\n", msg->data[1]);
	}
}

// Fonction appellée pour l'initialisation en position de l'AX12 dés l'arrivé de l'alimentation (via ActManager)
void POP_DROP_LEFT_WOOD_init_pos(){
	POP_DROP_LEFT_WOOD_initAX12();

	if(ax12_is_initialized == FALSE)
		return;

	debug_printf("Init pos : \n");
	if(!AX12_set_position(POP_DROP_LEFT_WOOD_AX12_ID, POP_DROP_LEFT_WOOD_AX12_INIT_POS))
		debug_printf("   L'AX12 n°%d n'est pas là\n", POP_DROP_LEFT_WOOD_AX12_ID);
	else
		debug_printf("   L'AX12 n°%d a été initialisé en position\n", POP_DROP_LEFT_WOOD_AX12_ID);
}

// Fonction appellée à la fin du match (via ActManager)
void POP_DROP_LEFT_WOOD_stop(){
	AX12_set_torque_enabled(POP_DROP_LEFT_WOOD_AX12_ID, FALSE); //Stopper l'asservissement de l'AX12
}

// fonction appellée à la réception d'un message CAN (via ActManager)
bool_e POP_DROP_LEFT_WOOD_CAN_process_msg(CAN_msg_t* msg) {
	if(msg->sid == ACT_POP_DROP_LEFT_WOOD){
		POP_DROP_LEFT_WOOD_initAX12();
		switch(msg->data[0]) {
			// Listing de toutes les positions de l'actionneur possible
			case ACT_POP_DROP_LEFT_WOOD_OPEN :
			case ACT_POP_DROP_LEFT_WOOD_CLOSED :
			case ACT_POP_DROP_LEFT_WOOD_STOP :
				ACTQ_push_operation_from_msg(msg, QUEUE_ACT_AX12_POP_DROP_LEFT_WOOD, &POP_DROP_LEFT_WOOD_run_command, 0,TRUE);
				break;

			case ACT_CONFIG :
				POP_DROP_LEFT_WOOD_config(msg);
				break;


			default:
				component_printf(LOG_LEVEL_Warning, "invalid CAN msg data[0]=%u !\n", msg->data[0]);
		}
		return TRUE;
	}else if(msg->sid == ACT_DO_SELFTEST){
		// Lister les différents états que l'actionneur doit réaliser pour réussir le selftest
		SELFTEST_set_actions(&POP_DROP_LEFT_WOOD_run_command, 9, 3, (SELFTEST_action_t[]){
								 {ACT_POP_DROP_LEFT_WOOD_CLOSED,		0,  QUEUE_ACT_AX12_POP_DROP_LEFT_WOOD},
								 {ACT_POP_DROP_LEFT_WOOD_OPEN,		    0,  QUEUE_ACT_AX12_POP_DROP_LEFT_WOOD},
								 {ACT_POP_DROP_LEFT_WOOD_CLOSED,		0,  QUEUE_ACT_AX12_POP_DROP_LEFT_WOOD}
							 });
	}
	return FALSE;
}

// Fonction appellée par la queue pendant tout le temps de la commande en cours (le booléen init est à TRUE au premier lancement de la commande)
void POP_DROP_LEFT_WOOD_run_command(queue_id_t queueId, bool_e init) {
	if(QUEUE_has_error(queueId)) {
		QUEUE_behead(queueId);
		return;
	}

	if(QUEUE_get_act(queueId) == QUEUE_ACT_AX12_POP_DROP_LEFT_WOOD) {    // Gestion des mouvements de POP_DROP_LEFT_WOOD
		if(init)
			POP_DROP_LEFT_WOOD_command_init(queueId);
		else
			POP_DROP_LEFT_WOOD_command_run(queueId);
	}
}

//Initialise une commande
static void POP_DROP_LEFT_WOOD_command_init(queue_id_t queueId) {
	Uint8 command = QUEUE_get_arg(queueId)->canCommand;
	Sint16* ax12_goalPosition = &QUEUE_get_arg(queueId)->param;

	*ax12_goalPosition = 0xFFFF;
	POP_DROP_LEFT_WOOD_initAX12();

	switch(command) {
		// Listing de toutes les positions de l'actionneur possible avec les valeurs de position associées
		case ACT_POP_DROP_LEFT_WOOD_OPEN : *ax12_goalPosition = POP_DROP_LEFT_WOOD_AX12_OPEN_POS; break;
		case ACT_POP_DROP_LEFT_WOOD_CLOSED : *ax12_goalPosition = POP_DROP_LEFT_WOOD_AX12_CLOSE_POS; break;

		case ACT_POP_DROP_LEFT_WOOD_STOP :
			AX12_set_torque_enabled(POP_DROP_LEFT_WOOD_AX12_ID, FALSE); //Stopper l'asservissement de l'AX12
			QUEUE_next(queueId, ACT_POP_DROP_LEFT_WOOD, ACT_RESULT_DONE, ACT_RESULT_ERROR_OK, __LINE__);
			return;

		default: {
			error_printf("Invalid POP_DROP_LEFT_WOOD command: %u, code is broken !\n", command);
			QUEUE_next(queueId, ACT_POP_DROP_LEFT_WOOD, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_LOGIC, __LINE__);
			return;
		}
	}

	if(ax12_is_initialized == FALSE){
		error_printf("Impossible de mettre l'actionneur en position il n'est pas initialisé\n");
		QUEUE_next(queueId, ACT_POP_DROP_LEFT_WOOD, ACT_RESULT_FAILED, ACT_RESULT_ERROR_NOT_HERE, __LINE__);
		return;
	}

	if(*ax12_goalPosition == 0xFFFF) {
		error_printf("Invalid ax12 position for command: %u, code is broken !\n", command);
		QUEUE_next(queueId, ACT_POP_DROP_LEFT_WOOD, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_LOGIC, __LINE__);
		return;
	}

	AX12_reset_last_error(POP_DROP_LEFT_WOOD_AX12_ID); //Sécurité anti terroriste. Nous les parano on aime pas voir des erreurs là ou il n'y en a pas.
	if(!AX12_set_position(POP_DROP_LEFT_WOOD_AX12_ID, *ax12_goalPosition)) {	//Si la commande n'a pas été envoyée correctement et/ou que l'AX12 ne répond pas a cet envoi, on l'indique à la carte stratégie
		error_printf("AX12_set_position error: 0x%x\n", AX12_get_last_error(POP_DROP_LEFT_WOOD_AX12_ID).error);
		QUEUE_next(queueId, ACT_POP_DROP_LEFT_WOOD, ACT_RESULT_FAILED, ACT_RESULT_ERROR_NOT_HERE, __LINE__);
		return;
	}
	//La commande a été envoyée et l'AX12 l'a bien reçu
	debug_printf("Move POP_DROP_LEFT_WOOD ax12 to %d\n", *ax12_goalPosition);
}

//Gère les états pendant le mouvement de l'AX12
static void POP_DROP_LEFT_WOOD_command_run(queue_id_t queueId) {
	Uint8 result, errorCode;
	Uint16 line;

	if(ACTQ_check_status_ax12(queueId, POP_DROP_LEFT_WOOD_AX12_ID, QUEUE_get_arg(queueId)->param, POP_DROP_LEFT_WOOD_AX12_ASSER_POS_EPSILON, POP_DROP_LEFT_WOOD_AX12_ASSER_TIMEOUT, POP_DROP_LEFT_WOOD_AX12_ASSER_POS_LARGE_EPSILON, &result, &errorCode, &line))
		QUEUE_next(queueId, ACT_POP_DROP_LEFT_WOOD, result, errorCode, line);
}



#endif
