/*  Club Robot ESEO 2013 - 2014
 *	SMALL
 *
 *	Fichier : exemple.c
 *	Package : Carte actionneur
 *	Description : Gestion exemple ax12
 *  Auteur : Arnaud
 *  Version 20130219
 *  Robot : SMALL
 */

#if 0

#include "exemple.h"

// Exemple d'un actionneur standart avec AX12

// Ajout l'actionneur dans QS_CANmsgList.h ainsi que toutes ses diff�rentes position
// Ajout d'une valeur dans l'�num�ration de la queue dans config_(big/small)/config_global_vars_types.h
// Formatage : QUEUE_ACT_AX12_EXEMPLE
// Ajout de la d�claration de l'actionneur dans ActManager dans le tableau actionneurs
// Ajout de la verbosit� dans le fichier act_queue_utils.c dans la fonction ACTQ_internal_printResult
// Ajout du selftest dans le fichier selftest.c dans la fonction SELFTEST_done_test
// Ajout du selftest dans le fichier QS_CANmsgList (dans l'�num�ration SELFEST)
// Ajout du pilotage via terminal dans le fichier term_io.c dans le tableau terminal_motor
// Un define EXEMPLE_AX12_ID doit avoir �t� ajout� au fichier config_big/config_pin.h // config_small/config_pin.h

// En strat�gie
// ajout des fonctions actionneurs dans act_functions.c/h
// ajout des fonctions actionneurs dans act_can.c (fonction ACT_process_result)
// ajout des fonctions actionneurs dans act_avoidance.c/h si l'actionneur modifie l'�vitement du robot
// ajout du verbose du selftest dans Supervision/Selftest.c (tableau SELFTEST_getError_string, fonction SELFTEST_print_errors)
// ajout de la verbosit� dans Supervision/Verbose_can_msg.c/h (fonction VERBOSE_CAN_MSG_sprint)

// If def � mettre si l'actionneur est seulement pr�sent sur le petit robot (I_AM_ROBOT_SMALL) ou le gros (I_AM_ROBOT_BIG)
#ifdef I_AM_ROBOT_BIG

// Les diff�rents includes n�cessaires...
#include "../QS/QS_CANmsgList.h"
#include "../QS/QS_ax12.h"
#include "../act_queue_utils.h"
#include "../selftest.h"
#include "../config/config_pin.h"
#include "../ActManager.h"

#include "exemple_config.h"

// Les diff�rents define pour le verbose sur uart
#include "../config/config_debug.h"
#define LOG_PREFIX "exemple.c : "
#define LOG_COMPONENT OUTPUT_LOG_COMPONENT_EXEMPLE
#include "../QS/QS_outputlog.h"

// Les fonctions internes au fonctionnement de l'actionneur
static void EXEMPLE_command_run(queue_id_t queueId);
static void EXEMPLE_initAX12();
static void EXEMPLE_command_init(queue_id_t queueId);
static void EXEMPLE_config(CAN_msg_t* msg);

// Bool�en contenant l'�tat actuel de l'initialisation de l'AX12 (Plusieurs bool�ens si plusieurs servo dans l'actionneur)
static bool_e ax12_is_initialized = FALSE;

// Fonction appell�e au lancement de la carte (via ActManager)
void EXEMPLE_init() {
	static bool_e initialized = FALSE;

	if(initialized)
		return;
	initialized = TRUE;

	AX12_init();
	EXEMPLE_initAX12();
}

// Fonction appell�e si la carte IHM a d�tect� une grosse chutte de la tension d'alimentation des servos
// Pour �viter les probl�mes d'utilisation de servo non initialis�
void EXEMPLE_reset_config(){
	ax12_is_initialized = FALSE;
	EXEMPLE_initAX12();
}

//Initialise l'AX12 s'il n'�tait pas aliment� lors d'initialisations pr�c�dentes, si d�j� initialis�, ne fait rien
static void EXEMPLE_initAX12() {
	if(ax12_is_initialized == FALSE && AX12_is_ready(EXEMPLE_AX12_ID) == TRUE) {
		ax12_is_initialized = TRUE;
		AX12_config_set_highest_voltage(EXEMPLE_AX12_ID, 136);
		AX12_config_set_lowest_voltage(EXEMPLE_AX12_ID, 70);
		AX12_config_set_maximum_torque_percentage(EXEMPLE_AX12_ID, SMALL_ARM_AX12_MAX_TORQUE_PERCENT);

		AX12_config_set_maximal_angle(EXEMPLE_AX12_ID, EXEMPLE_AX12_MAX_VALUE);
		AX12_config_set_minimal_angle(EXEMPLE_AX12_ID, EXEMPLE_AX12_MIN_VALUE);

		AX12_config_set_error_before_led(SMALL_ARM_AX12_ID, AX12_ERROR_ANGLE | AX12_ERROR_CHECKSUM | AX12_ERROR_INSTRUCTION | AX12_ERROR_OVERHEATING | AX12_ERROR_OVERLOAD | AX12_ERROR_RANGE);
		AX12_config_set_error_before_shutdown(SMALL_ARM_AX12_ID, AX12_ERROR_OVERHEATING);
		debug_printf("Exemple init config DONE\n");
	}else if(ax12_is_initialized == FALSE)
		debug_printf("Exemple init config FAIL\n");
}


// Fonction appell�e pour la modification des configurations de l'ax12 telle que la vitesse et le couple (via ActManager)
// Dans le cas de multiple actionneur appartenant � un m�me actionneur, ajouter des defines dans QS_CANmsgList.h afin de pouvoir les choisirs facilement depuis la strat�gie
void EXEMPLE_config(CAN_msg_t* msg){
	switch(msg->data[1]){
		case 0 : // Premier �lement de l'actionneur
			ACTMGR_config_AX12(EXEMPLE_AX12_ID, msg);
			break;

		default :
			warn_printf("invalid CAN msg data[1]=%u (sous actionneur inexistant)!\n", msg->data[1]);
	}
}

// Fonction appell�e pour l'initialisation en position de l'AX12 d�s l'arriv� de l'alimentation (via ActManager)
void EXEMPLE_init_pos(){
	EXEMPLE_initAX12();

	if(ax12_is_initialized == FALSE)
		return;

	debug_printf("Exemple init pos : \n");
	if(!AX12_set_position(EXEMPLE_AX12_ID, EXEMPLE_AX12_INIT_POS))
		debug_printf("   L'AX12 n�%d n'est pas l�\n", EXEMPLE_AX12_ID);
	else
		debug_printf("   L'AX12 n�%d a �t� initialis� en position\n", EXEMPLE_AX12_ID);
}

// Fonction appell�e � la fin du match (via ActManager)
void EXEMPLE_stop(){
	AX12_set_torque_enabled(EXEMPLE_AX12_ID, FALSE); //Stopper l'asservissement de l'AX12
}

// fonction appell�e � la r�ception d'un message CAN (via ActManager)
bool_e EXEMPLE_CAN_process_msg(CAN_msg_t* msg) {
	if(msg->sid == ACT_EXEMPLE){
		EXEMPLE_initAX12();
		switch(msg->data[0]) {
			// Listing de toutes les positions de l'actionneur possible
			case ACT_EXEMPLE_POS_1 :
			case ACT_EXEMPLE_POS_2 :
			case ACT_EXEMPLE_STOP :
				ACTQ_push_operation_from_msg(msg, QUEUE_ACT_AX12_EXEMPLE, &EXEMPLE_run_command, 0,TRUE);
				break;

			case ACT_CONFIG :
				EXEMPLE_config(msg);
				break;


			default:
				component_printf(LOG_LEVEL_Warning, "invalid CAN msg data[0]=%u !\n", msg->data[0]);
		}
		return TRUE;
	}else if(msg->sid == ACT_DO_SELFTEST){
		// Lister les diff�rents �tats que l'actionneur doit r�aliser pour r�ussir le selftest
		SELFTEST_set_actions(&EXEMPLE_run_command, 3, (SELFTEST_action_t[]){
								 {ACT_EXEMPLE_POS_1,		0,  QUEUE_ACT_AX12_EXEMPLE},
								 {ACT_EXEMPLE_POS_2,		0,  QUEUE_ACT_AX12_EXEMPLE},
								 {ACT_EXEMPLE_POS_1,		0,  QUEUE_ACT_AX12_EXEMPLE}
							 });
	}
	return FALSE;
}

// Fonction appell�e par la queue pendant tout le temps de la commande en cours (le bool�en init est � TRUE au premier lancement de la commande)
void EXEMPLE_run_command(queue_id_t queueId, bool_e init) {
	if(QUEUE_has_error(queueId)) {
		QUEUE_behead(queueId);
		return;
	}

	if(QUEUE_get_act(queueId) == QUEUE_ACT_AX12_EXEMPLE) {    // Gestion des mouvements de EXEMPLE
		if(init)
			EXEMPLE_command_init(queueId);
		else
			EXEMPLE_command_run(queueId);
	}
}

//Initialise une commande
static void EXEMPLE_command_init(queue_id_t queueId) {
	Uint8 command = QUEUE_get_arg(queueId)->canCommand;
	Uint16* ax12_goalPosition = &QUEUE_get_arg(queueId)->param;

	*ax12_goalPosition = 0xFFFF;
	EXEMPLE_initAX12();

	switch(command) {
		// Listing de toutes les positions de l'actionneur possible avec les valeurs de position associ�es
		case ACT_EXEMPLE_POS_1 : *ax12_goalPosition = EXEMPLE_AX12_POS_1_POS; break;
		case ACT_EXEMPLE_POS_2 : *ax12_goalPosition = EXEMPLE_AX12_POS_2_POS; break;

		case ACT_EXEMPLE_STOP :
			AX12_set_torque_enabled(EXEMPLE_AX12_ID, FALSE); //Stopper l'asservissement de l'AX12
			QUEUE_next(queueId, ACT_EXEMPLE, ACT_RESULT_DONE, ACT_RESULT_ERROR_OK, __LINE__);
			return;

		default: {
			error_printf("Invalid exemple command: %u, code is broken !\n", command);
			QUEUE_next(queueId, ACT_EXEMPLE, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_LOGIC, __LINE__);
			return;
		}
	}

	if(ax12_is_initialized == FALSE){
		error_printf("Impossible de mettre l'actionneur en position il n'est pas initialis�\n");
		QUEUE_next(queueId, ACT_EXEMPLE, ACT_RESULT_FAILED, ACT_RESULT_ERROR_NOT_HERE, __LINE__);
		return;
	}

	if(*ax12_goalPosition == 0xFFFF) {
		error_printf("Invalid ax12 position for command: %u, code is broken !\n", command);
		QUEUE_next(queueId, ACT_EXEMPLE, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_LOGIC, __LINE__);
		return;
	}

	AX12_reset_last_error(EXEMPLE_AX12_ID); //S�curit� anti terroriste. Nous les parano on aime pas voir des erreurs l� ou il n'y en a pas.
	if(!AX12_set_position(EXEMPLE_AX12_ID, *ax12_goalPosition)) {	//Si la commande n'a pas �t� envoy�e correctement et/ou que l'AX12 ne r�pond pas a cet envoi, on l'indique � la carte strat�gie
		error_printf("AX12_set_position error: 0x%x\n", AX12_get_last_error(EXEMPLE_AX12_ID).error);
		QUEUE_next(queueId, ACT_EXEMPLE, ACT_RESULT_FAILED, ACT_RESULT_ERROR_NOT_HERE, __LINE__);
		return;
	}
	//La commande a �t� envoy�e et l'AX12 l'a bien re�u
	debug_printf("Move SMALL_ARM ax12 to %d\n", *ax12_goalPosition);
}

//G�re les �tats pendant le mouvement de l'AX12
static void EXEMPLE_command_run(queue_id_t queueId) {
	Uint8 result, errorCode;
	Uint16 line;

	if(ACTQ_check_status_ax12(queueId, EXEMPLE_AX12_ID, QUEUE_get_arg(queueId)->param, EXEMPLE_AX12_ASSER_POS_EPSILON, EXEMPLE_AX12_ASSER_TIMEOUT, EXEMPLE_AX12_ASSER_POS_LARGE_EPSILON, &result, &errorCode, &line))
		QUEUE_next(queueId, ACT_EXEMPLE, result, errorCode, line);
}

#endif

#endif
