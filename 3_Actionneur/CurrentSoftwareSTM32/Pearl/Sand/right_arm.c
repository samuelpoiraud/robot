/*  Club Robot ESEO 2015 - 2016
 *	SMALL
 *
 *	Fichier : right_arm.c
 *	Package : Carte actionneur
 *	Description : Gestion de l'actionneur RIGHT_ARM
 *  Auteur : Julien
 *  Version 2016
 *  Robot : SMALL
 */

#include "right_arm.h"

// If def � mettre si l'actionneur est seulement pr�sent sur le petit robot (I_AM_ROBOT_SMALL) ou le gros (I_AM_ROBOT_BIG)
#ifdef I_AM_ROBOT_SMALL

// Les diff�rents includes n�cessaires...
#include "../QS/QS_CANmsgList.h"
#include "../QS/QS_ax12.h"
#include "../act_queue_utils.h"
#include "../selftest.h"
#include "../ActManager.h"

#include "right_arm_config.h"

// Les diff�rents define pour le verbose sur uart
#define LOG_PREFIX "right_arm.c : "
#define LOG_COMPONENT OUTPUT_LOG_COMPONENT_RIGHT_ARM
#include "../QS/QS_outputlog.h"

// Les fonctions internes au fonctionnement de l'actionneur
static void RIGHT_ARM_command_run(queue_id_t queueId);
static void RIGHT_ARM_initAX12();
static void RIGHT_ARM_command_init(queue_id_t queueId);
static void RIGHT_ARM_config(CAN_msg_t* msg);

// Bool�en contenant l'�tat actuel de l'initialisation de l'AX12 (Plusieurs bool�ens si plusieurs servo dans l'actionneur)
static bool_e ax12_is_initialized = FALSE;

// Fonction appell�e au lancement de la carte (via ActManager)
void RIGHT_ARM_init() {
	static bool_e initialized = FALSE;

	if(initialized)
		return;
	initialized = TRUE;

	AX12_init();
	RIGHT_ARM_initAX12();
}

// Fonction appell�e si la carte IHM a d�tect� une grosse chutte de la tension d'alimentation des servos
// Pour �viter les probl�mes d'utilisation de servo non initialis�
void RIGHT_ARM_reset_config(){
	ax12_is_initialized = FALSE;
	RIGHT_ARM_initAX12();
}

//Initialise l'AX12 s'il n'�tait pas aliment� lors d'initialisations pr�c�dentes, si d�j� initialis�, ne fait rien
static void RIGHT_ARM_initAX12() {
	if(ax12_is_initialized == FALSE && AX12_is_ready(RIGHT_ARM_AX12_ID) == TRUE) {
		ax12_is_initialized = TRUE;
		AX12_config_set_lowest_voltage(RIGHT_ARM_AX12_ID, AX12_MIN_VOLTAGE);
		AX12_config_set_highest_voltage(RIGHT_ARM_AX12_ID, AX12_MAX_VOLTAGE);
		AX12_set_torque_limit(RIGHT_ARM_AX12_ID, RIGHT_ARM_AX12_MAX_TORQUE_PERCENT);
		AX12_config_set_temperature_limit(RIGHT_ARM_AX12_ID, RIGHT_ARM_AX12_MAX_TEMPERATURE);

		AX12_config_set_maximal_angle(RIGHT_ARM_AX12_ID, RIGHT_ARM_AX12_MAX_VALUE);
		AX12_config_set_minimal_angle(RIGHT_ARM_AX12_ID, RIGHT_ARM_AX12_MIN_VALUE);

		AX12_config_set_error_before_led(RIGHT_ARM_AX12_ID, AX12_BEFORE_LED);
		AX12_config_set_error_before_shutdown(RIGHT_ARM_AX12_ID, AX12_BEFORE_SHUTDOWN);
		debug_printf("Init config DONE\n");
	}else if(ax12_is_initialized == FALSE)
		debug_printf("Init config FAIL\n");
}


// Fonction appell�e pour la modification des configurations de l'ax12 telle que la vitesse et le couple (via ActManager)
// Dans le cas de multiple actionneur appartenant � un m�me actionneur, ajouter des defines dans QS_CANmsgList.h afin de pouvoir les choisirs facilement depuis la strat�gie
void RIGHT_ARM_config(CAN_msg_t* msg){
	switch(msg->data.act_msg.act_data.act_config.sub_act_id){
		case DEFAULT_MONO_ACT : // Premier �lement de l'actionneur
			ACTMGR_config_AX12(RIGHT_ARM_AX12_ID, msg);
			break;

		default :
			warn_printf("invalid CAN msg data[1]=%u (sous actionneur inexistant)!\n", msg->data.act_msg.act_data.act_config.sub_act_id);
	}
}

// Fonction appell�e pour l'initialisation en position de l'AX12 d�s l'arriv� de l'alimentation (via ActManager)
void RIGHT_ARM_init_pos(){
	RIGHT_ARM_initAX12();

	if(ax12_is_initialized == FALSE)
		return;

	debug_printf("Init pos : \n");
	if(!AX12_set_position(RIGHT_ARM_AX12_ID, RIGHT_ARM_AX12_INIT_POS))
		debug_printf("   L'AX12 n�%d n'est pas l�\n", RIGHT_ARM_AX12_ID);
	else
		debug_printf("   L'AX12 n�%d a �t� initialis� en position\n", RIGHT_ARM_AX12_ID);
}

// Fonction appell�e � la fin du match (via ActManager)
void RIGHT_ARM_stop(){
	AX12_set_torque_enabled(RIGHT_ARM_AX12_ID, FALSE); //Stopper l'asservissement de l'AX12
}

// fonction appell�e � la r�ception d'un message CAN (via ActManager)
bool_e RIGHT_ARM_CAN_process_msg(CAN_msg_t* msg) {
	if(msg->sid == ACT_RIGHT_ARM){
		RIGHT_ARM_initAX12();
		switch(msg->data.act_msg.order) {
			// Listing de toutes les positions de l'actionneur possible
			case ACT_RIGHT_ARM_IDLE :
			case ACT_RIGHT_ARM_OPEN :
			case ACT_RIGHT_ARM_CLOSE :
			case ACT_RIGHT_ARM_STOP :
				ACTQ_push_operation_from_msg(msg, QUEUE_ACT_AX12_RIGHT_ARM, &RIGHT_ARM_run_command, 0,TRUE);
				break;

			case ACT_CONFIG :
				RIGHT_ARM_config(msg);
				break;


			default:
				component_printf(LOG_LEVEL_Warning, "invalid CAN msg data[0]=%u !\n", msg->data.act_msg.order);
		}
		return TRUE;
	}else if(msg->sid == ACT_DO_SELFTEST){
		// Lister les diff�rents �tats que l'actionneur doit r�aliser pour r�ussir le selftest
		/*SELFTEST_set_actions(&RIGHT_ARM_run_command, 3, (SELFTEST_action_t[]){
								 {ACT_RIGHT_ARM_IDLE,		0,  QUEUE_ACT_AX12_RIGHT_ARM},
								 {ACT_RIGHT_ARM_OPEN,       0,  QUEUE_ACT_AX12_RIGHT_ARM},
								 {ACT_RIGHT_ARM_IDLE,		0,  QUEUE_ACT_AX12_RIGHT_ARM}
							 });*/
	}
	return FALSE;
}

// Fonction appell�e par la queue pendant tout le temps de la commande en cours (le bool�en init est � TRUE au premier lancement de la commande)
void RIGHT_ARM_run_command(queue_id_t queueId, bool_e init) {
	if(QUEUE_has_error(queueId)) {
		QUEUE_behead(queueId);
		return;
	}

	if(QUEUE_get_act(queueId) == QUEUE_ACT_AX12_RIGHT_ARM) {    // Gestion des mouvements de RIGHT_ARM
		if(init)
			RIGHT_ARM_command_init(queueId);
		else
			RIGHT_ARM_command_run(queueId);
	}
}

//Initialise une commande
static void RIGHT_ARM_command_init(queue_id_t queueId) {
	Uint8 command = QUEUE_get_arg(queueId)->canCommand;
	Sint16* ax12_goalPosition = &QUEUE_get_arg(queueId)->param;

	*ax12_goalPosition = 0xFFFF;
	RIGHT_ARM_initAX12();

	switch(command) {
		// Listing de toutes les positions de l'actionneur possible avec les valeurs de position associ�es
		case ACT_RIGHT_ARM_IDLE : *ax12_goalPosition = RIGHT_ARM_AX12_IDLE_POS; break;
		case ACT_RIGHT_ARM_CLOSE : *ax12_goalPosition = RIGHT_ARM_AX12_CLOSE_POS; break;
		case ACT_RIGHT_ARM_OPEN : *ax12_goalPosition = RIGHT_ARM_AX12_OPEN_POS; break;

		case ACT_RIGHT_ARM_STOP :
			AX12_set_torque_enabled(RIGHT_ARM_AX12_ID, FALSE); //Stopper l'asservissement de l'AX12
			QUEUE_next(queueId, ACT_RIGHT_ARM, ACT_RESULT_DONE, ACT_RESULT_ERROR_OK, __LINE__);
			return;

		default: {
			error_printf("Invalid RIGHT_ARM command: %u, code is broken !\n", command);
			QUEUE_next(queueId, ACT_RIGHT_ARM, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_LOGIC, __LINE__);
			return;
		}
	}

	if(ax12_is_initialized == FALSE){
		error_printf("Impossible de mettre l'actionneur en position il n'est pas initialis�\n");
		QUEUE_next(queueId, ACT_RIGHT_ARM, ACT_RESULT_FAILED, ACT_RESULT_ERROR_NOT_HERE, __LINE__);
		return;
	}

	if(*ax12_goalPosition == 0xFFFF) {
		error_printf("Invalid ax12 position for command: %u, code is broken !\n", command);
		QUEUE_next(queueId, ACT_RIGHT_ARM, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_LOGIC, __LINE__);
		return;
	}

	AX12_reset_last_error(RIGHT_ARM_AX12_ID); //S�curit� anti terroriste. Nous les parano on aime pas voir des erreurs l� ou il n'y en a pas.
	if(!AX12_set_position(RIGHT_ARM_AX12_ID, *ax12_goalPosition)) {	//Si la commande n'a pas �t� envoy�e correctement et/ou que l'AX12 ne r�pond pas a cet envoi, on l'indique � la carte strat�gie
		error_printf("AX12_set_position error: 0x%x\n", AX12_get_last_error(RIGHT_ARM_AX12_ID).error);
		QUEUE_next(queueId, ACT_RIGHT_ARM, ACT_RESULT_FAILED, ACT_RESULT_ERROR_NOT_HERE, __LINE__);
		return;
	}
	//La commande a �t� envoy�e et l'AX12 l'a bien re�u
	debug_printf("Move RIGHT_ARM ax12 to %d\n", *ax12_goalPosition);
}

//G�re les �tats pendant le mouvement de l'AX12
static void RIGHT_ARM_command_run(queue_id_t queueId) {
	Uint8 result, errorCode;
	Uint16 line;

	if(ACTQ_check_status_ax12(queueId, RIGHT_ARM_AX12_ID, QUEUE_get_arg(queueId)->param, RIGHT_ARM_AX12_ASSER_POS_EPSILON, RIGHT_ARM_AX12_ASSER_TIMEOUT, RIGHT_ARM_AX12_ASSER_POS_LARGE_EPSILON, &result, &errorCode, &line))
		QUEUE_next(queueId, ACT_RIGHT_ARM, result, errorCode, line);
}

#endif

