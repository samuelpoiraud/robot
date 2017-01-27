/*  Club Robot ESEO 2016 - 2017
 *
 *	Fichier : SMALL_BALL_FRONT_LEFT_ax12.c
 *	Package : Carte actionneur
 *	Description : Gestion de l'actionneur AX12 SMALL_BALL_FRONT_LEFT
 *  Auteur :
 *  Version 2017
 *  Robot : BIG
 */

#include "small_ball_front_left.h"


// SMALL_BALL_FRONT_LEFT d'un actionneur standart avec AX12

// 1) Ajout l'actionneur dans QS_CANmsgList.h (tout � la fin du fichier)
// 2) Ajout d'une valeur dans l'�num�ration de la queue dans config_(big/small)/config_global_vars_types.h
//		Formatage : QUEUE_ACT_AX12_SMALL_BALL_FRONT_LEFT
// 3) Ajout de la d�claration de l'actionneur dans ActManager dans le tableau actionneurs
// 4) Ajout de la verbosit� dans le fichier act_queue_utils.c dans la fonction ACTQ_internal_printResult
// 5) Ajout du pilotage via terminal dans le fichier term_io.c dans le tableau terminal_motor du type : ACT_DECLARE(SMALL_BALL_FRONT_LEFT)
// 6) Un #define SMALL_BALL_FRONT_LEFT_AX12_ID doit avoir �t� ajout� au fichier config_big/config_pin.h ou config_small/config_pin.h
// 7) Ajout des postions dans QS_types.h dans l'�num ACT_order_e (avec "ACT_" et sans "_POS" � la fin)
// 8) Mise � jour de config/config_debug.h : mettre LOG_PRINT_On
// 9) Include le .h dans act_manager et dans termi_io
// 10) Enlever le #IF 0 qui se trouve quelques lignes

// Optionnel:
// Ajout du selftest dans le fichier selftest.c dans la fonction SELFTEST_done_test
// Ajout du selftest dans le fichier QS_CANmsgList (dans l'�num�ration SELFTEST)

// En strat�gie
// 1) ajout d'une d'une valeur dans le tableau act_link_SID_Queue du fichier act_functions.c/h
// 2) ajout des fonctions actionneurs dans act_avoidance.c/h si l'actionneur modifie l'�vitement du robot

// En strat�gie Optionnel
// ajout du verbose du selftest dans Supervision/Selftest.c (tableau SELFTEST_getError_string, fonction SELFTEST_print_errors)
// ajout de la verbosit� dans Supervision/Verbose_can_msg.c/h (fonction VERBOSE_CAN_MSG_sprint)



// If def � mettre si l'actionneur est seulement pr�sent sur le petit robot (I_AM_ROBOT_SMALL) ou le gros (I_AM_ROBOT_BIG)
#ifdef I_AM_ROBOT_SMALL

// Les diff�rents includes n�cessaires...
#include "../QS/QS_CANmsgList.h"
#include "../QS/QS_ax12.h"
#include "../act_queue_utils.h"
#include "../selftest.h"
#include "../ActManager.h"

#include "small_ball_front_left_config.h"

// Les diff�rents define pour le verbose sur uart
#define LOG_PREFIX "SMALL_BALL_FRONT_LEFT.c : "
#define LOG_COMPONENT OUTPUT_LOG_COMPONENT_SMALL_BALL_FRONT_LEFT
#include "../QS/QS_outputlog.h"

// Les fonctions internes au fonctionnement de l'actionneur
static void SMALL_BALL_FRONT_LEFT_command_run(queue_id_t queueId);
static void SMALL_BALL_FRONT_LEFT_initAX12();
static void SMALL_BALL_FRONT_LEFT_command_init(queue_id_t queueId);
static void SMALL_BALL_FRONT_LEFT_config(CAN_msg_t* msg);

// Bool�en contenant l'�tat actuel de l'initialisation de l'AX12 (Plusieurs bool�ens si plusieurs servo dans l'actionneur)
static bool_e ax12_is_initialized = FALSE;

// Fonction appell�e au lancement de la carte (via ActManager)
void SMALL_BALL_FRONT_LEFT_init() {
	static bool_e initialized = FALSE;

	if(initialized)
		return;
	initialized = TRUE;

	AX12_init();
	SMALL_BALL_FRONT_LEFT_initAX12();
}

// Fonction appell�e si la carte IHM a d�tect� une grosse chutte de la tension d'alimentation des servos
// Pour �viter les probl�mes d'utilisation de servo non initialis�
void SMALL_BALL_FRONT_LEFT_reset_config(){
	ax12_is_initialized = FALSE;
	SMALL_BALL_FRONT_LEFT_initAX12();
}

//Initialise l'AX12 s'il n'�tait pas aliment� lors d'initialisations pr�c�dentes, si d�j� initialis�, ne fait rien
static void SMALL_BALL_FRONT_LEFT_initAX12() {
	if(ax12_is_initialized == FALSE && AX12_is_ready(SMALL_BALL_FRONT_LEFT_AX12_ID) == TRUE) {
		ax12_is_initialized = TRUE;
		AX12_config_set_lowest_voltage(SMALL_BALL_FRONT_LEFT_AX12_ID, AX12_MIN_VOLTAGE);
		AX12_config_set_highest_voltage(SMALL_BALL_FRONT_LEFT_AX12_ID, AX12_MAX_VOLTAGE);
		AX12_set_torque_limit(SMALL_BALL_FRONT_LEFT_AX12_ID, SMALL_BALL_FRONT_LEFT_AX12_MAX_TORQUE_PERCENT);
		AX12_config_set_temperature_limit(SMALL_BALL_FRONT_LEFT_AX12_ID, SMALL_BALL_FRONT_LEFT_AX12_MAX_TEMPERATURE);

		AX12_config_set_maximal_angle(SMALL_BALL_FRONT_LEFT_AX12_ID, SMALL_BALL_FRONT_LEFT_AX12_MAX_VALUE);
		AX12_config_set_minimal_angle(SMALL_BALL_FRONT_LEFT_AX12_ID, SMALL_BALL_FRONT_LEFT_AX12_MIN_VALUE);

		AX12_config_set_error_before_led(SMALL_BALL_FRONT_LEFT_AX12_ID, AX12_BEFORE_LED);
		AX12_config_set_error_before_shutdown(SMALL_BALL_FRONT_LEFT_AX12_ID, AX12_BEFORE_SHUTDOWN);
		debug_printf("Init config DONE\n");
	}else if(ax12_is_initialized == FALSE)
		debug_printf("Init config FAIL\n");
}


// Fonction appell�e pour la modification des configurations de l'ax12 telle que la vitesse et le couple (via ActManager)
// Dans le cas de multiple actionneur appartenant � un m�me actionneur, ajouter des defines dans QS_CANmsgList.h afin de pouvoir les choisirs facilement depuis la strat�gie
void SMALL_BALL_FRONT_LEFT_config(CAN_msg_t* msg){
	switch(msg->data.act_msg.act_data.act_config.sub_act_id){
		case DEFAULT_MONO_ACT : // Premier �lement de l'actionneur
			ACTMGR_config_AX12(SMALL_BALL_FRONT_LEFT_AX12_ID, msg);
			break;

		default :
			warn_printf("invalid CAN msg data[1]=%u (sous actionneur inexistant)!\n", msg->data.act_msg.act_data.act_config.sub_act_id);
	}
}

// Fonction appell�e pour l'initialisation en position de l'AX12 d�s l'arriv� de l'alimentation (via ActManager)
void SMALL_BALL_FRONT_LEFT_init_pos(){
	SMALL_BALL_FRONT_LEFT_initAX12();

	if(ax12_is_initialized == FALSE)
		return;

	debug_printf("Init pos : \n");
	if(!AX12_set_position(SMALL_BALL_FRONT_LEFT_AX12_ID, SMALL_BALL_FRONT_LEFT_AX12_INIT_POS))
		debug_printf("   L'AX12 n�%d n'est pas l�\n", SMALL_BALL_FRONT_LEFT_AX12_ID);
	else
		debug_printf("   L'AX12 n�%d a �t� initialis� en position\n", SMALL_BALL_FRONT_LEFT_AX12_ID);
}

// Fonction appell�e � la fin du match (via ActManager)
void SMALL_BALL_FRONT_LEFT_stop(){
	AX12_set_torque_enabled(SMALL_BALL_FRONT_LEFT_AX12_ID, FALSE); //Stopper l'asservissement de l'AX12
}

// fonction appell�e � la r�ception d'un message CAN (via ActManager)
bool_e SMALL_BALL_FRONT_LEFT_CAN_process_msg(CAN_msg_t* msg) {
	if(msg->sid == ACT_SMALL_BALL_FRONT_LEFT){
		SMALL_BALL_FRONT_LEFT_initAX12();
		switch(msg->data.act_msg.order) {
			// Listing de toutes les positions de l'actionneur possible
			case ACT_SMALL_BALL_FRONT_LEFT_IDLE :
			case ACT_SMALL_BALL_FRONT_LEFT_LOCK :
			case ACT_SMALL_BALL_FRONT_LEFT_UNLOCK :
			case ACT_SMALL_BALL_FRONT_LEFT_STOP :
				ACTQ_push_operation_from_msg(msg, QUEUE_ACT_AX12_SMALL_BALL_FRONT_LEFT, &SMALL_BALL_FRONT_LEFT_run_command, 0,TRUE);
				break;

			case ACT_CONFIG :
				SMALL_BALL_FRONT_LEFT_config(msg);
				break;


			default:
				component_printf(LOG_LEVEL_Warning, "invalid CAN msg data[0]=%u !\n", msg->data.act_msg.order);
		}
		return TRUE;
	}else if(msg->sid == ACT_DO_SELFTEST){

	}
	return FALSE;
}

// Fonction appell�e par la queue pendant tout le temps de la commande en cours (le bool�en init est � TRUE au premier lancement de la commande)
void SMALL_BALL_FRONT_LEFT_run_command(queue_id_t queueId, bool_e init) {
	if(QUEUE_has_error(queueId)) {
		QUEUE_behead(queueId);
		return;
	}

	if(QUEUE_get_act(queueId) == QUEUE_ACT_AX12_SMALL_BALL_FRONT_LEFT) {    // Gestion des mouvements de SMALL_BALL_FRONT_LEFT
		if(init)
			SMALL_BALL_FRONT_LEFT_command_init(queueId);
		else
			SMALL_BALL_FRONT_LEFT_command_run(queueId);
	}
}

//Initialise une commande
static void SMALL_BALL_FRONT_LEFT_command_init(queue_id_t queueId) {
	Uint8 command = QUEUE_get_arg(queueId)->canCommand;
	Sint16* ax12_goalPosition = &QUEUE_get_arg(queueId)->param;

	*ax12_goalPosition = 0xFFFF;
	SMALL_BALL_FRONT_LEFT_initAX12();

	switch(command) {
		// Listing de toutes les positions de l'actionneur possible avec les valeurs de position associ�es
		case ACT_SMALL_BALL_FRONT_LEFT_IDLE : *ax12_goalPosition = SMALL_BALL_FRONT_LEFT_AX12_IDLE_POS; break;
		case ACT_SMALL_BALL_FRONT_LEFT_LOCK : *ax12_goalPosition = SMALL_BALL_FRONT_LEFT_AX12_LOCK_POS; break;
		case ACT_SMALL_BALL_FRONT_LEFT_UNLOCK : *ax12_goalPosition = SMALL_BALL_FRONT_LEFT_AX12_UNLOCK_POS; break;

		case ACT_SMALL_BALL_FRONT_LEFT_STOP :
			AX12_set_torque_enabled(SMALL_BALL_FRONT_LEFT_AX12_ID, FALSE); //Stopper l'asservissement de l'AX12
			QUEUE_next(queueId, ACT_SMALL_BALL_FRONT_LEFT, ACT_RESULT_DONE, ACT_RESULT_ERROR_OK, __LINE__);
			return;

		default: {
			error_printf("Invalid SMALL_BALL_FRONT_LEFT command: %u, code is broken !\n", command);
			QUEUE_next(queueId, ACT_SMALL_BALL_FRONT_LEFT, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_LOGIC, __LINE__);
			return;
		}
	}

	if(ax12_is_initialized == FALSE){
		error_printf("Impossible de mettre l'actionneur en position il n'est pas initialis�\n");
		QUEUE_next(queueId, ACT_SMALL_BALL_FRONT_LEFT, ACT_RESULT_FAILED, ACT_RESULT_ERROR_NOT_HERE, __LINE__);
		return;
	}

	if(*ax12_goalPosition == 0xFFFF) {
		error_printf("Invalid ax12 position for command: %u, code is broken !\n", command);
		QUEUE_next(queueId, ACT_SMALL_BALL_FRONT_LEFT, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_LOGIC, __LINE__);
		return;
	}

	AX12_reset_last_error(SMALL_BALL_FRONT_LEFT_AX12_ID); //S�curit� anti terroriste. Nous les parano on aime pas voir des erreurs l� ou il n'y en a pas.
	if(!AX12_set_position(SMALL_BALL_FRONT_LEFT_AX12_ID, *ax12_goalPosition)) {	//Si la commande n'a pas �t� envoy�e correctement et/ou que l'AX12 ne r�pond pas a cet envoi, on l'indique � la carte strat�gie
		error_printf("AX12_set_position error: 0x%x\n", AX12_get_last_error(SMALL_BALL_FRONT_LEFT_AX12_ID).error);
		QUEUE_next(queueId, ACT_SMALL_BALL_FRONT_LEFT, ACT_RESULT_FAILED, ACT_RESULT_ERROR_NOT_HERE, __LINE__);
		return;
	}
	//La commande a �t� envoy�e et l'AX12 l'a bien re�u
	debug_printf("Move SMALL_BALL_FRONT_LEFT ax12 to %d\n", *ax12_goalPosition);
}

//G�re les �tats pendant le mouvement de l'AX12
static void SMALL_BALL_FRONT_LEFT_command_run(queue_id_t queueId) {
	Uint8 result, errorCode;
	Uint16 line;

	if(ACTQ_check_status_ax12(queueId, SMALL_BALL_FRONT_LEFT_AX12_ID, QUEUE_get_arg(queueId)->param, SMALL_BALL_FRONT_LEFT_AX12_ASSER_POS_EPSILON, SMALL_BALL_FRONT_LEFT_AX12_ASSER_TIMEOUT, SMALL_BALL_FRONT_LEFT_AX12_ASSER_POS_LARGE_EPSILON, &result, &errorCode, &line))
		QUEUE_next(queueId, ACT_SMALL_BALL_FRONT_LEFT, result, errorCode, line);
}
#endif


