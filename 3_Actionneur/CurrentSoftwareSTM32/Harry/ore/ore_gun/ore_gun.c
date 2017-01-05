/*  Club Robot ESEO 2016 - 2017
*
*	Fichier : ORE_GUN_rx24.c
*	Package : Carte actionneur
*	Description : Gestion de l'actionneur RX24 ORE_GUN
*	Auteur :
*	Version 2017
*	Robot : BIG
*/

#include "ore_gun.h"


// ORE_GUN d'un actionneur standart avec RX24

// 1) Ajout l'actionneur dans QS_CANmsgList.h (tout � la fin du fichier)
// 2) Ajout d'une valeur dans l'�num�ration de la queue dans config_(big/small)/config_global_vars_types.h
//		Formatage : QUEUE_ACT_RX24_ORE_GUN
// 3) Ajout de la d�claration de l'actionneur dans ActManager dans le tableau actionneurs
// 4) Ajout de la verbosit� dans le fichier act_queue_utils.c dans la fonction ACTQ_internal_printResult
// 5) Ajout du pilotage via terminal dans le fichier term_io.c dans le tableau terminal_motor du type : ACT_DECLARE(ORE_GUN)
// 6) Un #define ORE_GUN_RX24_ID doit avoir �t� ajout� au fichier config_big/config_pin.h ou config_small/config_pin.h
// 7) Ajout des postions dans QS_types.h dans l'�num ACT_order_e (avec "ACT_" et sans "_POS" � la fin)
// 8) Mise � jour de config/config_debug.h : mettre LOG_PRINT_On

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
#ifdef I_AM_ROBOT_BIG

// Les diff�rents includes n�cessaires...
#include "../QS/QS_CANmsgList.h"
#include "../QS/QS_rx24.h"
#include "../act_queue_utils.h"
#include "../selftest.h"
#include "../ActManager.h"

#include "ore_gun_config.h"

// Les diff�rents define pour le verbose sur uart
#define LOG_PREFIX "ORE_GUN.c : "
#define LOG_COMPONENT OUTPUT_LOG_COMPONENT_ORE_GUN
#include "../QS/QS_outputlog.h"

// Les fonctions internes au fonctionnement de l'actionneur
static void ORE_GUN_command_run(queue_id_t queueId);
static void ORE_GUN_initRX24();
static void ORE_GUN_command_init(queue_id_t queueId);
static void ORE_GUN_config(CAN_msg_t* msg);

// Bool�en contenant l'�tat actuel de l'initialisation du RX24 (Plusieurs bool�ens si plusieurs servo dans l'actionneur)
static bool_e rx24_is_initialized = FALSE;

// Fonction appell�e au lancement de la carte (via ActManager)
void ORE_GUN_init() {
	static bool_e initialized = FALSE;

	if(initialized)
		return;
	initialized = TRUE;

	RX24_init();
	ORE_GUN_initRX24();
}

// Fonction appell�e si la carte IHM a d�tect� une grosse chutte de la tension d'alimentation des servos
// Pour �viter les probl�mes d'utilisation de servo non initialis�
void ORE_GUN_reset_config(){
	rx24_is_initialized = FALSE;
	ORE_GUN_initRX24();
}

//Initialise le RX24 s'il n'�tait pas aliment� lors d'initialisations pr�c�dentes, si d�j� initialis�, ne fait rien
static void ORE_GUN_initRX24() {
	if(rx24_is_initialized == FALSE && RX24_is_ready(ORE_GUN_RX24_ID) == TRUE) {
		rx24_is_initialized = TRUE;
		RX24_config_set_lowest_voltage(ORE_GUN_RX24_ID, RX24_MIN_VOLTAGE);
		RX24_config_set_highest_voltage(ORE_GUN_RX24_ID, RX24_MAX_VOLTAGE);
		RX24_set_torque_limit(ORE_GUN_RX24_ID, ORE_GUN_RX24_MAX_TORQUE_PERCENT);
		RX24_config_set_temperature_limit(ORE_GUN_RX24_ID, ORE_GUN_RX24_MAX_TEMPERATURE);

		RX24_config_set_maximal_angle(ORE_GUN_RX24_ID, ORE_GUN_RX24_MAX_VALUE);
		RX24_config_set_minimal_angle(ORE_GUN_RX24_ID, ORE_GUN_RX24_MIN_VALUE);

		RX24_config_set_error_before_led(ORE_GUN_RX24_ID, RX24_BEFORE_LED);
		RX24_config_set_error_before_shutdown(ORE_GUN_RX24_ID, RX24_BEFORE_SHUTDOWN);
		debug_printf("Init config DONE\n");
	}else if(rx24_is_initialized == FALSE)
		debug_printf("Init config FAIL\n");
}


// Fonction appell�e pour la modification des configurations du rx24 telle que la vitesse et le couple (via ActManager)
// Dans le cas de multiple actionneur appartenant � un m�me actionneur, ajouter des defines dans QS_CANmsgList.h afin de pouvoir les choisirs facilement depuis la strat�gie
void ORE_GUN_config(CAN_msg_t* msg){
	switch(msg->data.act_msg.act_data.act_config.sub_act_id){
		case DEFAULT_MONO_ACT : // Premier �lement de l'actionneur
			ACTMGR_config_RX24(ORE_GUN_RX24_ID, msg);
			break;

		default :
			warn_printf("invalid CAN msg data[1]=%u (sous actionneur inexistant)!\n", msg->data.act_msg.act_data.act_config.sub_act_id);
	}
}

// Fonction appell�e pour l'initialisation en position du rx24 d�s l'arriv� de l'alimentation (via ActManager)
void ORE_GUN_init_pos(){
	ORE_GUN_initRX24();

	if(rx24_is_initialized == FALSE)
		return;

	debug_printf("Init pos : \n");
	if(!RX24_set_position(ORE_GUN_RX24_ID, ORE_GUN_RX24_INIT_POS))
		debug_printf("   Le RX24 n�%d n'est pas l�\n", ORE_GUN_RX24_ID);
	else
		debug_printf("   Le RX24 n�%d a �t� initialis� en position\n", ORE_GUN_RX24_ID);
}

// Fonction appell�e � la fin du match (via ActManager)
void ORE_GUN_stop(){
	RX24_set_torque_enabled(ORE_GUN_RX24_ID, FALSE); //Stopper l'asservissement du RX24
}

// fonction appell�e � la r�ception d'un message CAN (via ActManager)
bool_e ORE_GUN_CAN_process_msg(CAN_msg_t* msg) {
	if(msg->sid == ACT_ORE_GUN){
		ORE_GUN_initRX24();
		switch(msg->data.act_msg.order) {
			// Listing de toutes les positions de l'actionneur possible
			case ACT_ORE_GUN_IDLE :
			case ACT_ORE_GUN_LOCK :
			case ACT_ORE_GUN_UNLOCK :
			case ACT_ORE_GUN_STOP :
				ACTQ_push_operation_from_msg(msg, QUEUE_ACT_RX24_ORE_GUN, &ORE_GUN_run_command, 0,TRUE);
				break;

			case ACT_CONFIG :
				ORE_GUN_config(msg);
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
void ORE_GUN_run_command(queue_id_t queueId, bool_e init) {
	if(QUEUE_has_error(queueId)) {
		QUEUE_behead(queueId);
		return;
	}

	if(QUEUE_get_act(queueId) == QUEUE_ACT_RX24_ORE_GUN) {    // Gestion des mouvements de ORE_GUN
		if(init)
			ORE_GUN_command_init(queueId);
		else
			ORE_GUN_command_run(queueId);
	}
}

//Initialise une commande
static void ORE_GUN_command_init(queue_id_t queueId) {
	Uint8 command = QUEUE_get_arg(queueId)->canCommand;
	Sint16* rx24_goalPosition = &QUEUE_get_arg(queueId)->param;

	*rx24_goalPosition = 0xFFFF;
	ORE_GUN_initRX24();

	switch(command) {
		// Listing de toutes les positions de l'actionneur possible avec les valeurs de position associ�es
		case ACT_ORE_GUN_IDLE : *rx24_goalPosition = ORE_GUN_RX24_IDLE_POS; break;
		case ACT_ORE_GUN_LOCK : *rx24_goalPosition = ORE_GUN_RX24_LOCK_POS; break;
		case ACT_ORE_GUN_UNLOCK : *rx24_goalPosition = ORE_GUN_RX24_UNLOCK_POS; break;

		case ACT_ORE_GUN_STOP :
			RX24_set_torque_enabled(ORE_GUN_RX24_ID, FALSE); //Stopper l'asservissement du RX24
			QUEUE_next(queueId, ACT_ORE_GUN, ACT_RESULT_DONE, ACT_RESULT_ERROR_OK, __LINE__);
			return;

		default: {
			error_printf("Invalid ORE_GUN command: %u, code is broken !\n", command);
			QUEUE_next(queueId, ACT_ORE_GUN, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_LOGIC, __LINE__);
			return;
		}
	}

	if(rx24_is_initialized == FALSE){
		error_printf("Impossible de mettre l'actionneur en position il n'est pas initialis�\n");
		QUEUE_next(queueId, ACT_ORE_GUN, ACT_RESULT_FAILED, ACT_RESULT_ERROR_NOT_HERE, __LINE__);
		return;
	}

	if(*rx24_goalPosition == 0xFFFF) {
		error_printf("Invalid rx24 position for command: %u, code is broken !\n", command);
		QUEUE_next(queueId, ACT_ORE_GUN, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_LOGIC, __LINE__);
		return;
	}

	RX24_reset_last_error(ORE_GUN_RX24_ID); //S�curit� anti terroriste. Nous les parano on aime pas voir des erreurs l� ou il n'y en a pas.
	if(!RX24_set_position(ORE_GUN_RX24_ID, *rx24_goalPosition)) {	//Si la commande n'a pas �t� envoy�e correctement et/ou que le RX24 ne r�pond pas a cet envoi, on l'indique � la carte strat�gie
		error_printf("RX24_set_position error: 0x%x\n", RX24_get_last_error(ORE_GUN_RX24_ID).error);
		QUEUE_next(queueId, ACT_ORE_GUN, ACT_RESULT_FAILED, ACT_RESULT_ERROR_NOT_HERE, __LINE__);
		return;
	}
	//La commande a �t� envoy�e et le RX24 l'a bien re�u
	debug_printf("Move ORE_GUN rx24 to %d\n", *rx24_goalPosition);
}

//G�re les �tats pendant le mouvement du RX24
static void ORE_GUN_command_run(queue_id_t queueId) {
	Uint8 result, errorCode;
	Uint16 line;

	if(ACTQ_check_status_rx24(queueId, ORE_GUN_RX24_ID, QUEUE_get_arg(queueId)->param, ORE_GUN_RX24_ASSER_POS_EPSILON, ORE_GUN_RX24_ASSER_TIMEOUT, ORE_GUN_RX24_ASSER_POS_LARGE_EPSILON, &result, &errorCode, &line))
		QUEUE_next(queueId, ACT_ORE_GUN, result, errorCode, line);
}

#endif