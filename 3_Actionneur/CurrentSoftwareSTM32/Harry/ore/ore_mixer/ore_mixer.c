/*  Club Robot ESEO 2016 - 2017
 *
 *	Fichier : ORE_MIXER_ax12.c
 *	Package : Carte actionneur
 *	Description : Gestion de l'actionneur AX12 ORE_MIXER
 *  Auteur :
 *  Version 2017
 *  Robot : BIG
 */

#include "ore_mixer.h"


// ORE_MIXER d'un actionneur standart avec AX12

// 1) Ajout l'actionneur dans QS_CANmsgList.h (tout à la fin du fichier)
// 2) Ajout d'une valeur dans l'énumération de la queue dans config_(big/small)/config_global_vars_types.h
//		Formatage : QUEUE_ACT_AX12_ORE_MIXER
// 3) Ajout de la déclaration de l'actionneur dans ActManager dans le tableau actionneurs
// 4) Ajout de la verbosité dans le fichier act_queue_utils.c dans la fonction ACTQ_internal_printResult
// 5) Ajout du pilotage via terminal dans le fichier term_io.c dans le tableau terminal_motor du type : ACT_DECLARE(ORE_MIXER)
// 6) Un #define ORE_MIXER_AX12_ID doit avoir été ajouté au fichier config_big/config_pin.h ou config_small/config_pin.h
// 7) Ajout des postions dans QS_types.h dans l'énum ACT_order_e (avec "ACT_" et sans "_POS" à la fin)
// 8) Mise à jour de config/config_debug.h : mettre LOG_PRINT_On
// 9) Include le .h dans act_manager et dans termi_io
// 10) Enlever le #IF 0 qui se trouve quelques lignes

// Optionnel:
// Ajout du selftest dans le fichier selftest.c dans la fonction SELFTEST_done_test
// Ajout du selftest dans le fichier QS_CANmsgList (dans l'énumération SELFTEST)

// En stratégie
// 1) ajout d'une d'une valeur dans le tableau act_link_SID_Queue du fichier act_functions.c/h
// 2) ajout des fonctions actionneurs dans act_avoidance.c/h si l'actionneur modifie l'évitement du robot

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

#include "ore_mixer_config.h"

// Les différents define pour le verbose sur uart
#define LOG_PREFIX "ORE_MIXER.c : "
#define LOG_COMPONENT OUTPUT_LOG_COMPONENT_ORE_MIXER
#include "../QS/QS_outputlog.h"

// Les fonctions internes au fonctionnement de l'actionneur
static void ORE_MIXER_command_run(queue_id_t queueId);
static void ORE_MIXER_initAX12();
static void ORE_MIXER_command_init(queue_id_t queueId);
static void ORE_MIXER_config(CAN_msg_t* msg);

// Booléen contenant l'état actuel de l'initialisation de l'AX12 (Plusieurs booléens si plusieurs servo dans l'actionneur)
static bool_e ax12_is_initialized = FALSE;

// Fonction appellée au lancement de la carte (via ActManager)
void ORE_MIXER_init() {
	static bool_e initialized = FALSE;

	if(initialized)
		return;
	initialized = TRUE;

	AX12_init();
	ORE_MIXER_initAX12();
}

// Fonction appellée si la carte IHM a détecté une grosse chutte de la tension d'alimentation des servos
// Pour éviter les problèmes d'utilisation de servo non initialisé
void ORE_MIXER_reset_config(){
	ax12_is_initialized = FALSE;
	ORE_MIXER_initAX12();
}

//Initialise l'AX12 s'il n'était pas alimenté lors d'initialisations précédentes, si déjà initialisé, ne fait rien
static void ORE_MIXER_initAX12() {
	if(ax12_is_initialized == FALSE && AX12_is_ready(ORE_MIXER_AX12_ID) == TRUE) {
		ax12_is_initialized = TRUE;
		AX12_config_set_lowest_voltage(ORE_MIXER_AX12_ID, AX12_MIN_VOLTAGE);
		AX12_config_set_highest_voltage(ORE_MIXER_AX12_ID, AX12_MAX_VOLTAGE);
		AX12_set_torque_limit(ORE_MIXER_AX12_ID, ORE_MIXER_AX12_MAX_TORQUE_PERCENT);
		AX12_config_set_temperature_limit(ORE_MIXER_AX12_ID, ORE_MIXER_AX12_MAX_TEMPERATURE);

		AX12_config_set_maximal_angle(ORE_MIXER_AX12_ID, ORE_MIXER_AX12_MAX_VALUE);
		AX12_config_set_minimal_angle(ORE_MIXER_AX12_ID, ORE_MIXER_AX12_MIN_VALUE);

		AX12_config_set_error_before_led(ORE_MIXER_AX12_ID, AX12_BEFORE_LED);
		AX12_config_set_error_before_shutdown(ORE_MIXER_AX12_ID, AX12_BEFORE_SHUTDOWN);
		debug_printf("Init config DONE\n");
	}else if(ax12_is_initialized == FALSE)
		debug_printf("Init config FAIL\n");
}


// Fonction appellée pour la modification des configurations de l'ax12 telle que la vitesse et le couple (via ActManager)
// Dans le cas de multiple actionneur appartenant à un même actionneur, ajouter des defines dans QS_CANmsgList.h afin de pouvoir les choisirs facilement depuis la stratégie
void ORE_MIXER_config(CAN_msg_t* msg){
	switch(msg->data.act_msg.act_data.act_config.sub_act_id){
		case DEFAULT_MONO_ACT : // Premier élement de l'actionneur
			ACTMGR_config_AX12(ORE_MIXER_AX12_ID, msg);
			break;

		default :
			warn_printf("invalid CAN msg data[1]=%u (sous actionneur inexistant)!\n", msg->data.act_msg.act_data.act_config.sub_act_id);
	}
}

// Fonction appellée pour l'initialisation en position de l'AX12 dés l'arrivé de l'alimentation (via ActManager)
void ORE_MIXER_init_pos(){
	ORE_MIXER_initAX12();

	if(ax12_is_initialized == FALSE)
		return;

	debug_printf("Init pos : \n");
	if(!AX12_set_position(ORE_MIXER_AX12_ID, ORE_MIXER_AX12_INIT_POS))
		debug_printf("   L'AX12 n°%d n'est pas là\n", ORE_MIXER_AX12_ID);
	else
		debug_printf("   L'AX12 n°%d a été initialisé en position\n", ORE_MIXER_AX12_ID);
}

// Fonction appellée à la fin du match (via ActManager)
void ORE_MIXER_stop(){
	AX12_set_torque_enabled(ORE_MIXER_AX12_ID, FALSE); //Stopper l'asservissement de l'AX12
}

// fonction appellée à la réception d'un message CAN (via ActManager)
bool_e ORE_MIXER_CAN_process_msg(CAN_msg_t* msg) {
	if(msg->sid == ACT_ORE_MIXER){
		ORE_MIXER_initAX12();
		switch(msg->data.act_msg.order) {
			// Listing de toutes les positions de l'actionneur possible
			case ACT_ORE_MIXER_IDLE :
			case ACT_ORE_MIXER_LOCK :
			case ACT_ORE_MIXER_UNLOCK :
			case ACT_ORE_MIXER_STOP :
				ACTQ_push_operation_from_msg(msg, QUEUE_ACT_AX12_ORE_MIXER, &ORE_MIXER_run_command, 0,TRUE);
				break;

			case ACT_CONFIG :
				ORE_MIXER_config(msg);
				break;


			default:
				component_printf(LOG_LEVEL_Warning, "invalid CAN msg data[0]=%u !\n", msg->data.act_msg.order);
		}
		return TRUE;
	}else if(msg->sid == ACT_DO_SELFTEST){

	}
	return FALSE;
}

// Fonction appellée par la queue pendant tout le temps de la commande en cours (le booléen init est à TRUE au premier lancement de la commande)
void ORE_MIXER_run_command(queue_id_t queueId, bool_e init) {
	if(QUEUE_has_error(queueId)) {
		QUEUE_behead(queueId);
		return;
	}

	if(QUEUE_get_act(queueId) == QUEUE_ACT_AX12_ORE_MIXER) {    // Gestion des mouvements de ORE_MIXER
		if(init)
			ORE_MIXER_command_init(queueId);
		else
			ORE_MIXER_command_run(queueId);
	}
}

//Initialise une commande
static void ORE_MIXER_command_init(queue_id_t queueId) {
	Uint8 command = QUEUE_get_arg(queueId)->canCommand;
	Sint16* ax12_goalPosition = &QUEUE_get_arg(queueId)->param;

	*ax12_goalPosition = 0xFFFF;
	ORE_MIXER_initAX12();

	switch(command) {
		// Listing de toutes les positions de l'actionneur possible avec les valeurs de position associées
		case ACT_ORE_MIXER_IDLE : *ax12_goalPosition = ORE_MIXER_AX12_IDLE_POS; break;
		case ACT_ORE_MIXER_LOCK : *ax12_goalPosition = ORE_MIXER_AX12_LOCK_POS; break;
		case ACT_ORE_MIXER_UNLOCK : *ax12_goalPosition = ORE_MIXER_AX12_UNLOCK_POS; break;

		case ACT_ORE_MIXER_STOP :
			AX12_set_torque_enabled(ORE_MIXER_AX12_ID, FALSE); //Stopper l'asservissement de l'AX12
			QUEUE_next(queueId, ACT_ORE_MIXER, ACT_RESULT_DONE, ACT_RESULT_ERROR_OK, __LINE__);
			return;

		default: {
			error_printf("Invalid ORE_MIXER command: %u, code is broken !\n", command);
			QUEUE_next(queueId, ACT_ORE_MIXER, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_LOGIC, __LINE__);
			return;
		}
	}

	if(ax12_is_initialized == FALSE){
		error_printf("Impossible de mettre l'actionneur en position il n'est pas initialisé\n");
		QUEUE_next(queueId, ACT_ORE_MIXER, ACT_RESULT_FAILED, ACT_RESULT_ERROR_NOT_HERE, __LINE__);
		return;
	}

	if(*ax12_goalPosition == 0xFFFF) {
		error_printf("Invalid ax12 position for command: %u, code is broken !\n", command);
		QUEUE_next(queueId, ACT_ORE_MIXER, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_LOGIC, __LINE__);
		return;
	}

	AX12_reset_last_error(ORE_MIXER_AX12_ID); //Sécurité anti terroriste. Nous les parano on aime pas voir des erreurs là ou il n'y en a pas.
	if(!AX12_set_position(ORE_MIXER_AX12_ID, *ax12_goalPosition)) {	//Si la commande n'a pas été envoyée correctement et/ou que l'AX12 ne répond pas a cet envoi, on l'indique à la carte stratégie
		error_printf("AX12_set_position error: 0x%x\n", AX12_get_last_error(ORE_MIXER_AX12_ID).error);
		QUEUE_next(queueId, ACT_ORE_MIXER, ACT_RESULT_FAILED, ACT_RESULT_ERROR_NOT_HERE, __LINE__);
		return;
	}
	//La commande a été envoyée et l'AX12 l'a bien reçu
	debug_printf("Move ORE_MIXER ax12 to %d\n", *ax12_goalPosition);
}

//Gère les états pendant le mouvement de l'AX12
static void ORE_MIXER_command_run(queue_id_t queueId) {
	Uint8 result, errorCode;
	Uint16 line;

	if(ACTQ_check_status_ax12(queueId, ORE_MIXER_AX12_ID, QUEUE_get_arg(queueId)->param, ORE_MIXER_AX12_ASSER_POS_EPSILON, ORE_MIXER_AX12_ASSER_TIMEOUT, ORE_MIXER_AX12_ASSER_POS_LARGE_EPSILON, &result, &errorCode, &line))
		QUEUE_next(queueId, ACT_ORE_MIXER, result, errorCode, line);
}
#endif
