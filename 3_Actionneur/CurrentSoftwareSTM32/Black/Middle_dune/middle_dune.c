/*  Club Robot ESEO 2015 - 2016
 *	BIG
 *
 *	Fichier : middle_dune.c
 *	Package : Carte actionneur
 *	Description : Gestion de l'actionneur RX24 MIDDLE_DUNE
 *  Auteur : Julien
 *  Version 2016
 *  Robot : BIG
 */

#include "middle_dune.h"


// If def � mettre si l'actionneur est seulement pr�sent sur le petit robot (I_AM_ROBOT_SMALL) ou le gros (I_AM_ROBOT_BIG)
#ifdef I_AM_ROBOT_BIG

// Les diff�rents includes n�cessaires...
#include "../QS/QS_CANmsgList.h"
#include "../QS/QS_rx24.h"
#include "../act_queue_utils.h"
#include "../selftest.h"
#include "../ActManager.h"

#include "middle_dune_config.h"

// Les diff�rents define pour le verbose sur uart
#define LOG_PREFIX "Middle_dune_rx24.c : "
#define LOG_COMPONENT OUTPUT_LOG_COMPONENT_MIDDLE_DUNE
#include "../QS/QS_outputlog.h"

// Les fonctions internes au fonctionnement de l'actionneur
static void MIDDLE_DUNE_command_run(queue_id_t queueId);
static void MIDDLE_DUNE_initRX24();
static void MIDDLE_DUNE_command_init(queue_id_t queueId);
static void MIDDLE_DUNE_config(CAN_msg_t* msg);
static void MIDDLE_DUNE_get_position(QUEUE_act_e act_id, Uint8 command, Uint16 *right_pos, Uint16 *left_pos);

// Bool�en contenant l'�tat actuel de l'initialisation du RX24 (Plusieurs bool�ens si plusieurs servo dans l'actionneur)
static bool_e rx24_is_initialized_left = FALSE;
static bool_e rx24_is_initialized_right = FALSE;

// Fonction appell�e au lancement de la carte (via ActManager)
void MIDDLE_DUNE_init() {
	static bool_e initialized = FALSE;

	if(initialized)
		return;
	initialized = TRUE;

	RX24_init();
	MIDDLE_DUNE_initRX24();
}

// Fonction appell�e si la carte IHM a d�tect� une grosse chutte de la tension d'alimentation des servos
// Pour �viter les probl�mes d'utilisation de servo non initialis�
void MIDDLE_DUNE_reset_config(){
	rx24_is_initialized_left = FALSE;
	rx24_is_initialized_right = FALSE;
	MIDDLE_DUNE_initRX24();
}

//Initialise le RX24 s'il n'�tait pas aliment� lors d'initialisations pr�c�dentes, si d�j� initialis�, ne fait rien
static void MIDDLE_DUNE_initRX24() {
	//MIDDLE_DUNE_LEFT
	if(rx24_is_initialized_left == FALSE && RX24_is_ready(MIDDLE_DUNE_LEFT_RX24_ID) == TRUE) {
		rx24_is_initialized_left = TRUE;

		RX24_config_set_lowest_voltage(MIDDLE_DUNE_LEFT_RX24_ID, RX24_MIN_VOLTAGE);
		RX24_config_set_highest_voltage(MIDDLE_DUNE_LEFT_RX24_ID, RX24_MAX_VOLTAGE);
		RX24_set_torque_limit(MIDDLE_DUNE_LEFT_RX24_ID, MIDDLE_DUNE_RX24_MAX_TORQUE_PERCENT);
		RX24_config_set_temperature_limit(MIDDLE_DUNE_LEFT_RX24_ID, MIDDLE_DUNE_RX24_MAX_TEMPERATURE);
		RX24_set_move_to_position_speed(MIDDLE_DUNE_LEFT_RX24_ID,MIDDLE_DUNE_RX24_MAX_SPEED);

		RX24_config_set_maximal_angle(MIDDLE_DUNE_LEFT_RX24_ID, MIDDLE_DUNE_RX24_MAX_VALUE);
		RX24_config_set_minimal_angle(MIDDLE_DUNE_LEFT_RX24_ID, MIDDLE_DUNE_RX24_MIN_VALUE);

		RX24_config_set_error_before_led(MIDDLE_DUNE_LEFT_RX24_ID, RX24_BEFORE_LED);
		RX24_config_set_error_before_shutdown(MIDDLE_DUNE_LEFT_RX24_ID, RX24_BEFORE_SHUTDOWN);

		debug_printf("Init config DONE\n");
	}else if(rx24_is_initialized_left == FALSE)
		debug_printf("Init config FAIL\n");

	//MIDDLE_DUNE_RIGHT
	if(rx24_is_initialized_right == FALSE && RX24_is_ready(MIDDLE_DUNE_RIGHT_RX24_ID) == TRUE) {
		rx24_is_initialized_right = TRUE;

		RX24_config_set_lowest_voltage(MIDDLE_DUNE_RIGHT_RX24_ID, RX24_MIN_VOLTAGE);
		RX24_config_set_highest_voltage(MIDDLE_DUNE_RIGHT_RX24_ID, RX24_MAX_VOLTAGE);
		RX24_set_torque_limit(MIDDLE_DUNE_RIGHT_RX24_ID, MIDDLE_DUNE_RX24_MAX_TORQUE_PERCENT);
		RX24_config_set_temperature_limit(MIDDLE_DUNE_RIGHT_RX24_ID, MIDDLE_DUNE_RX24_MAX_TEMPERATURE);

		RX24_config_set_maximal_angle(MIDDLE_DUNE_RIGHT_RX24_ID, MIDDLE_DUNE_RX24_MAX_VALUE);
		RX24_config_set_minimal_angle(MIDDLE_DUNE_RIGHT_RX24_ID, MIDDLE_DUNE_RX24_MIN_VALUE);

		RX24_config_set_error_before_led(MIDDLE_DUNE_RIGHT_RX24_ID, RX24_BEFORE_LED);
		RX24_config_set_error_before_shutdown(MIDDLE_DUNE_RIGHT_RX24_ID, RX24_BEFORE_SHUTDOWN);
		debug_printf("Init config DONE\n");
	}else if(rx24_is_initialized_right == FALSE)
		debug_printf("Init config FAIL\n");
}


// Fonction appell�e pour la modification des configurations du rx24 telle que la vitesse et le couple (via ActManager)
// Dans le cas de multiple actionneur appartenant � un m�me actionneur, ajouter des defines dans QS_CANmsgList.h afin de pouvoir les choisirs facilement depuis la strat�gie
void MIDDLE_DUNE_config(CAN_msg_t* msg){
	switch(msg->data.act_msg.act_data.act_config.sub_act_id){
		case DEFAULT_MONO_ACT : // Premier �lement de l'actionneur
			ACTMGR_config_RX24(MIDDLE_DUNE_LEFT_RX24_ID, msg);
			ACTMGR_config_RX24(MIDDLE_DUNE_RIGHT_RX24_ID, msg);
			break;

		default :
			warn_printf("invalid CAN msg data[1]=%u (sous actionneur inexistant)!\n", msg->data.act_msg.act_data.act_config.sub_act_id);
	}
}

// Fonction appell�e pour l'initialisation en position du rx24 d�s l'arriv� de l'alimentation (via ActManager)
void MIDDLE_DUNE_init_pos(){
	MIDDLE_DUNE_initRX24();

	if(rx24_is_initialized_left == TRUE){
		debug_printf("Init pos : \n");
		if(!RX24_set_position(MIDDLE_DUNE_LEFT_RX24_ID, MIDDLE_DUNE_L_RX24_INIT_POS))
			debug_printf("   Le RX24 n�%d n'est pas l�\n", MIDDLE_DUNE_LEFT_RX24_ID);
		else
			debug_printf("   Le RX24 n�%d a �t� initialis� en position\n", MIDDLE_DUNE_LEFT_RX24_ID);
	}

	if(rx24_is_initialized_right == TRUE){
		debug_printf("Init pos : \n");
		if(!RX24_set_position(MIDDLE_DUNE_RIGHT_RX24_ID, MIDDLE_DUNE_R_RX24_INIT_POS))
			debug_printf("   Le RX24 n�%d n'est pas l�\n", MIDDLE_DUNE_RIGHT_RX24_ID);
		else
			debug_printf("   Le RX24 n�%d a �t� initialis� en position\n", MIDDLE_DUNE_RIGHT_RX24_ID);
	}
}

// Fonction appell�e � la fin du match (via ActManager)
void MIDDLE_DUNE_stop(){
	RX24_set_torque_enabled(MIDDLE_DUNE_LEFT_RX24_ID, FALSE); //Stopper l'asservissement du RX24
	RX24_set_torque_enabled(MIDDLE_DUNE_RIGHT_RX24_ID, FALSE); //Stopper l'asservissement du RX24
}

// fonction appell�e � la r�ception d'un message CAN (via ActManager)
bool_e MIDDLE_DUNE_CAN_process_msg(CAN_msg_t* msg) {
	if(msg->sid == ACT_MIDDLE_DUNE){
		MIDDLE_DUNE_initRX24();
		switch(msg->data.act_msg.order) {
			// Listing de toutes les positions de l'actionneur possible
			case ACT_MIDDLE_DUNE_IDLE :
			case ACT_MIDDLE_DUNE_LOCK :
			case ACT_MIDDLE_DUNE_UNLOCK :
			case ACT_MIDDLE_DUNE_STOP :
				ACTQ_push_operation_from_msg(msg, QUEUE_ACT_RX24_MIDDLE_DUNE, &MIDDLE_DUNE_run_command, 0,TRUE);
				break;

			case ACT_CONFIG :
				MIDDLE_DUNE_config(msg);
				break;


			default:
				component_printf(LOG_LEVEL_Warning, "invalid CAN msg data[0]=%u !\n", msg->data.act_msg.order);
		}
		return TRUE;
	}else if(msg->sid == ACT_DO_SELFTEST){
		// Lister les diff�rents �tats que l'actionneur doit r�aliser pour r�ussir le selftest
		/*SELFTEST_set_actions(&MIDDLE_DUNE_run_command, 3, (SELFTEST_action_t[]){
								 {ACT_MIDDLE_DUNE_IDLE,		0,  QUEUE_ACT_RX24_MIDDLE_DUNE},
								 {ACT_MIDDLE_DUNE_OPEN,       0,  QUEUE_ACT_RX24_MIDDLE_DUNE},
								 {ACT_MIDDLE_DUNE_IDLE,		0,  QUEUE_ACT_RX24_MIDDLE_DUNE}
							 });*/
	}
	return FALSE;
}

// Fonction appell�e par la queue pendant tout le temps de la commande en cours (le bool�en init est � TRUE au premier lancement de la commande)
void MIDDLE_DUNE_run_command(queue_id_t queueId, bool_e init) {
	if(QUEUE_has_error(queueId)) {
		QUEUE_behead(queueId);
		return;
	}

	if(QUEUE_get_act(queueId) == QUEUE_ACT_RX24_MIDDLE_DUNE) {    // Gestion des mouvements de MIDDLE_DUNE
		if(init)
			MIDDLE_DUNE_command_init(queueId);
		else
			MIDDLE_DUNE_command_run(queueId);
	}
}

//Initialise une commande
static void MIDDLE_DUNE_command_init(queue_id_t queueId) {
	Uint8 command = QUEUE_get_arg(queueId)->canCommand;
	Uint16 rx24_goalPosition_left = 0xFFFF, rx24_goalPosition_right = 0xFFFF;
	bool_e result = TRUE;

	MIDDLE_DUNE_initRX24();

	switch(command) {
		// Listing de toutes les positions de l'actionneur possible avec les valeurs de position associ�es
		case ACT_MIDDLE_DUNE_IDLE :
		case ACT_MIDDLE_DUNE_LOCK :
		case ACT_MIDDLE_DUNE_UNLOCK :
			MIDDLE_DUNE_get_position(QUEUE_get_act(queueId), QUEUE_get_arg(queueId)->canCommand, &rx24_goalPosition_right, &rx24_goalPosition_left);
			break;

		case ACT_MIDDLE_DUNE_STOP :
			RX24_set_torque_enabled(MIDDLE_DUNE_LEFT_RX24_ID, FALSE); //Stopper l'asservissement du RX24
			RX24_set_torque_enabled(MIDDLE_DUNE_RIGHT_RX24_ID, FALSE); //Stopper l'asservissement du RX24
			QUEUE_next(queueId, ACT_MIDDLE_DUNE, ACT_RESULT_DONE, ACT_RESULT_ERROR_OK, __LINE__);
			return;

		default: {
			error_printf("Invalid MIDDLE_DUNE command: %u, code is broken !\n", command);
			QUEUE_next(queueId, ACT_MIDDLE_DUNE, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_LOGIC, __LINE__);
			return;
		}
	}

	if(rx24_is_initialized_left == FALSE || rx24_is_initialized_right == FALSE ){
		error_printf("Impossible de mettre l'actionneur en position il n'est pas initialis�\n");
		QUEUE_next(queueId, ACT_MIDDLE_DUNE, ACT_RESULT_FAILED, ACT_RESULT_ERROR_NOT_HERE, __LINE__);
		return;
	}

	if(rx24_goalPosition_left == 0xFFFF || rx24_goalPosition_left == 0xFFFF) {
		error_printf("Invalid rx24 position for command: %u, code is broken !\n", command);
		QUEUE_next(queueId, ACT_MIDDLE_DUNE, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_LOGIC, __LINE__);
		return;
	}

	RX24_reset_last_error(MIDDLE_DUNE_LEFT_RX24_ID); //S�curit� anti terroriste. Nous les parano on aime pas voir des erreurs l� ou il n'y en a pas.
	RX24_reset_last_error(MIDDLE_DUNE_RIGHT_RX24_ID); //S�curit� anti terroriste. Nous les parano on aime pas voir des erreurs l� ou il n'y en a pas.

	if(!RX24_set_position(MIDDLE_DUNE_LEFT_RX24_ID, rx24_goalPosition_left)) {	//Si la commande n'a pas �t� envoy�e correctement et/ou que le RX24 ne r�pond pas a cet envoi, on l'indique � la carte strat�gie
		error_printf("RX24_set_position error: 0x%x\n", RX24_get_last_error(MIDDLE_DUNE_LEFT_RX24_ID).error);
		result = FALSE;
	}else
		debug_printf("Move MIDDLE_DUNE rx24(%d) to %d\n", MIDDLE_DUNE_LEFT_RX24_ID, rx24_goalPosition_left);

	if(!RX24_set_position(MIDDLE_DUNE_RIGHT_RX24_ID, rx24_goalPosition_right)) {	//Si la commande n'a pas �t� envoy�e correctement et/ou que le RX24 ne r�pond pas a cet envoi, on l'indique � la carte strat�gie
		error_printf("RX24_set_position error: 0x%x\n", RX24_get_last_error(MIDDLE_DUNE_LEFT_RX24_ID).error);
		result = FALSE;
	}else
		debug_printf("Move MIDDLE_DUNE rx24(%d) to %d\n", MIDDLE_DUNE_LEFT_RX24_ID, rx24_goalPosition_right);

	if(result == FALSE){
		QUEUE_next(queueId, ACT_MIDDLE_DUNE, ACT_RESULT_FAILED, ACT_RESULT_ERROR_NOT_HERE, __LINE__);
		return;
	}

}

//G�re les �tats pendant le mouvement du RX24
static void MIDDLE_DUNE_command_run(queue_id_t queueId) {
	Uint8 result_left, result_right, errorCode_left, errorCode_right;
	Uint16 line_left, line_right;
	Uint16 rx24_goalPosition_left = 0xFFFF, rx24_goalPosition_right = 0xFFFF;
	static bool_e done_left=FALSE, done_right=FALSE;

	MIDDLE_DUNE_get_position(QUEUE_get_act(queueId), QUEUE_get_arg(queueId)->canCommand, &rx24_goalPosition_right, &rx24_goalPosition_left);

	if(done_right == FALSE)
		done_right = ACTQ_check_status_rx24(queueId, MIDDLE_DUNE_RIGHT_RX24_ID, rx24_goalPosition_right,
				MIDDLE_DUNE_RX24_ASSER_POS_EPSILON, MIDDLE_DUNE_RX24_ASSER_TIMEOUT, MIDDLE_DUNE_RX24_ASSER_POS_LARGE_EPSILON, &result_right, &errorCode_right, &line_right);

	if(done_left == FALSE)
		done_left = ACTQ_check_status_rx24(queueId, MIDDLE_DUNE_LEFT_RX24_ID, rx24_goalPosition_left,
				MIDDLE_DUNE_RX24_ASSER_POS_EPSILON,MIDDLE_DUNE_RX24_ASSER_TIMEOUT, MIDDLE_DUNE_RX24_ASSER_POS_LARGE_EPSILON, &result_left, &errorCode_left, &line_left);

	if(done_right && done_left){
		done_right = FALSE;
		done_left = FALSE;
		if(result_right == ACT_RESULT_DONE && result_left == ACT_RESULT_DONE)
			QUEUE_next(queueId, ACT_MIDDLE_DUNE, ACT_RESULT_DONE, ACT_RESULT_ERROR_OK, 0x0100);
		else if(result_right != ACT_RESULT_DONE)
			QUEUE_next(queueId, ACT_MIDDLE_DUNE, result_right, errorCode_right, line_right);
		else
			QUEUE_next(queueId, ACT_MIDDLE_DUNE, result_left, errorCode_left, line_left);
	}
}

static void MIDDLE_DUNE_get_position(QUEUE_act_e act_id, Uint8 command, Uint16 *right_pos, Uint16 *left_pos){
	if(act_id == QUEUE_ACT_RX24_MIDDLE_DUNE){
		switch(command){
			case ACT_MIDDLE_DUNE_IDLE :
				*right_pos = MIDDLE_DUNE_R_RX24_IDLE_POS;
				*left_pos = MIDDLE_DUNE_L_RX24_IDLE_POS;
				break;

			case ACT_MIDDLE_DUNE_LOCK :
				*right_pos = MIDDLE_DUNE_R_RX24_LOCK_POS;
				*left_pos = MIDDLE_DUNE_L_RX24_LOCK_POS;
				break;

			case ACT_MIDDLE_DUNE_UNLOCK :
				*right_pos = MIDDLE_DUNE_R_RX24_UNLOCK_POS;
				*left_pos = MIDDLE_DUNE_L_RX24_UNLOCK_POS;
				break;

			default:
				*right_pos = 0xFFFF;
				*left_pos = 0xFFFF;
				break;
		}
	}else{
		*right_pos = 0xFFFF;
		*left_pos = 0xFFFF;
	}
}
#endif
