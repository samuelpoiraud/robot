/*  Club Robot ESEO 2014 - 2015
 *	HOLLY - WOOD
 *
 *	Fichier : stock_RX24.c
 *	Package : Carte actionneur
 *	Description : Gestion de l'ensemble des RX24 du stock de Holly
 *  Auteur : Arnaud
 *  Version 20130219
 *  Robot : HOLLY
 */

#include "stock_RX24.h"

#ifdef I_AM_ROBOT_BIG

// Les différents includes nécessaires...
#include "../QS/QS_CANmsgList.h"
#include "../QS/QS_ax12.h"
#include "../act_queue_utils.h"
#include "../selftest.h"
#include "../ActManager.h"

#include "stock_RX24_config.h"

#define LOG_PREFIX "stock : "
#define LOG_COMPONENT OUTPUT_LOG_COMPONENT_STOCK
#include "../QS/QS_outputlog.h"

static void STOCK_command_run(queue_id_t queueId);
static void STOCK_initRX24();
static void STOCK_command_init(queue_id_t queueId);
static void STOCK_config(CAN_msg_t* msg);
static void STOCK_get_position(QUEUE_act_e act_id, Uint8 command, Uint16 *right_f1_pos, Uint16 *left_f1_pos, Uint16 *right_f2_pos, Uint16 *left_f2_pos);

typedef struct{
	bool_e is_initialized;
	Uint8 servo_id;
	Uint16 init_pos;
}stock_act_s;

stock_act_s stock_act[8] = {
	{FALSE, STOCKR_R_F1_RX24_ID, STOCKR_RIGHT_F1_INIT_POS},
	{FALSE, STOCKR_L_F1_RX24_ID, STOCKR_LEFT_F1_INIT_POS},
	{FALSE, STOCKL_R_F1_RX24_ID, STOCKL_RIGHT_F1_INIT_POS},
	{FALSE, STOCKL_L_F1_RX24_ID, STOCKL_LEFT_F1_INIT_POS},

	{FALSE, STOCKR_R_F2_RX24_ID, STOCKR_RIGHT_F2_INIT_POS},
	{FALSE, STOCKR_L_F2_RX24_ID, STOCKR_LEFT_F2_INIT_POS},
	{FALSE, STOCKL_R_F2_RX24_ID, STOCKL_RIGHT_F2_INIT_POS},
	{FALSE, STOCKL_L_F2_RX24_ID, STOCKL_LEFT_F2_INIT_POS},
};

// Fonction appellée au lancement de la carte (via ActManager)
void STOCK_init() {
	static bool_e initialized = FALSE;

	if(initialized)
		return;
	initialized = TRUE;

	AX12_init();
	STOCK_initRX24();
}

// Fonction appellée si la carte IHM a détecté une grosse chutte de la tension d'alimentation des servos
// Pour éviter les problèmes d'utilisation de servo non initialisé
void STOCK_reset_config(){
	Uint8 i;
	for(i=0;i<8;i++)
		stock_act[i].is_initialized = FALSE;
	STOCK_initRX24();
}

//Initialise l'AX12 s'il n'était pas alimenté lors d'initialisations précédentes, si déjà initialisé, ne fait rien
static void STOCK_initRX24() {
	Uint8 i;
	for(i=0;i<8;i++){
		if(stock_act[i].is_initialized == FALSE && AX12_is_ready(stock_act[i].servo_id) == TRUE) {
			stock_act[i].is_initialized = TRUE;
			AX12_config_set_lowest_voltage(stock_act[i].servo_id, AX12_MIN_VOLTAGE);
			AX12_config_set_highest_voltage(stock_act[i].servo_id, AX12_MAX_VOLTAGE);
			AX12_set_torque_limit(stock_act[i].servo_id, STOCK_RX24_MAX_TORQUE_PERCENT);
			AX12_config_set_temperature_limit(stock_act[i].servo_id, PINCEMI_RX24_MAX_TEMPERATURE);

			AX12_config_set_maximal_angle(stock_act[i].servo_id, STOCK_RX24_MAX_VALUE);
			AX12_config_set_minimal_angle(stock_act[i].servo_id, STOCK_RX24_MIN_VALUE);

			AX12_config_set_error_before_led(stock_act[i].servo_id, AX12_BEFORE_LED);
			AX12_config_set_error_before_shutdown(stock_act[i].servo_id, AX12_BEFORE_SHUTDOWN);
			debug_printf("Init config rx24(%d) DONE\n", stock_act[i].servo_id);

			if(i > 3)
				AX12_set_torque_response(stock_act[i].servo_id, 32, 10, 10, 32);

		}else if(stock_act[i].is_initialized == FALSE)
			debug_printf("Init config rx24(%d) FAIL\n", stock_act[i].servo_id);
	}
}


// Fonction appellée pour la modification des configurations de l'ax12 telle que la vitesse et le couple (via ActManager)
// Dans le cas de multiple actionneur appartenant à un même actionneur, ajouter des defines dans QS_CANmsgList.h afin de pouvoir les choisirs facilement depuis la stratégie
void STOCK_config(CAN_msg_t* msg){
	if(msg->sid == ACT_STOCK_RIGHT){
		switch(msg->data[1]){
			case 0 :
				ACTMGR_config_AX12(STOCKR_R_F1_RX24_ID, msg);
				break;

			case 1 :
				ACTMGR_config_AX12(STOCKR_L_F1_RX24_ID, msg);
				break;

			case 2 :
				ACTMGR_config_AX12(STOCKR_R_F2_RX24_ID, msg);
				break;

			case 3 :
				ACTMGR_config_AX12(STOCKR_L_F2_RX24_ID, msg);
				break;
			default :
				warn_printf("invalid CAN msg data[1]=%u (sous actionneur inexistant)!\n", msg->data[1]);
		}
	}else if(msg->sid == ACT_STOCK_LEFT){
		switch(msg->data[1]){
			case 0 :
				ACTMGR_config_AX12(STOCKL_R_F1_RX24_ID, msg);
				break;

			case 1 :
				ACTMGR_config_AX12(STOCKL_L_F1_RX24_ID, msg);
				break;

			case 2 :
				ACTMGR_config_AX12(STOCKL_R_F2_RX24_ID, msg);
				break;

			case 3 :
				ACTMGR_config_AX12(STOCKL_L_F2_RX24_ID, msg);
				break;
			default :
				warn_printf("invalid CAN msg data[1]=%u (sous actionneur inexistant)!\n", msg->data[1]);
		}
	}
}

// Fonction appellée pour l'initialisation en position de le RX24 dés l'arrivé de l'alimentation (via ActManager)
void STOCK_init_pos(){
	Uint8 i;

	STOCK_initRX24();

	debug_printf("Init pos : \n");
	for(i=0;i<8;i++){
		if(stock_act[i].is_initialized == TRUE){
			if(!AX12_set_position(stock_act[i].servo_id, stock_act[i].init_pos))
				debug_printf("   Le RX24 n°%d n'est pas là\n", stock_act[i].servo_id);
			else
				debug_printf("   Le RX24 n°%d a été initialisé en position\n", stock_act[i].servo_id);
		}
	}
}

// Fonction appellée à la fin du match (via ActManager)
void STOCK_stop(){
	Uint8 i;
	for(i=0;i<8;i++)
		AX12_set_torque_enabled(stock_act[i].servo_id, FALSE);
}

// fonction appellée à la réception d'un message CAN (via ActManager)
bool_e STOCK_CAN_process_msg(CAN_msg_t* msg) {
	if(msg->sid == ACT_STOCK_RIGHT){
		STOCK_initRX24();
		switch(msg->data[0]) {
			// Listing de toutes les positions de l'actionneur possible
			case ACT_STOCK_RIGHT_CLOSE :
			case ACT_STOCK_RIGHT_LOCK :
			case ACT_STOCK_RIGHT_UNLOCK :
			case ACT_STOCK_RIGHT_OPEN :
			case ACT_STOCK_RIGHT_STOP :
				ACTQ_push_operation_from_msg(msg, QUEUE_ACT_RX24_STOCK_RIGHT, &STOCK_run_command, 0, TRUE);
				break;

			case ACT_CONFIG :
				STOCK_config(msg);
				break;


			default:
				component_printf(LOG_LEVEL_Warning, "invalid CAN msg data[0]=%u !\n", msg->data[0]);
		}
		return TRUE;
	}else if(msg->sid == ACT_STOCK_LEFT){
		STOCK_initRX24();
		switch(msg->data[0]) {
			// Listing de toutes les positions de l'actionneur possible
			case ACT_STOCK_LEFT_CLOSE :
			case ACT_STOCK_LEFT_LOCK :
			case ACT_STOCK_LEFT_UNLOCK :
			case ACT_STOCK_LEFT_OPEN :
			case ACT_STOCK_LEFT_STOP :
				ACTQ_push_operation_from_msg(msg, QUEUE_ACT_RX24_STOCK_LEFT, &STOCK_run_command, 0, TRUE);
				break;

			case ACT_CONFIG :
				STOCK_config(msg);
				break;


			default:
				component_printf(LOG_LEVEL_Warning, "invalid CAN msg data[0]=%u !\n", msg->data[0]);
		}
		return TRUE;
	}else if(msg->sid == ACT_DO_SELFTEST){
		// Lister les différents états que l'actionneur doit réaliser pour réussir le selftest
		SELFTEST_set_actions(&STOCK_run_command, 3, 3, (SELFTEST_action_t[]){
								 {ACT_STOCK_RIGHT_OPEN,		0,  QUEUE_ACT_RX24_STOCK_RIGHT},
								 {ACT_STOCK_RIGHT_CLOSE,	0,  QUEUE_ACT_RX24_STOCK_RIGHT},
								 {ACT_STOCK_RIGHT_OPEN,		0,  QUEUE_ACT_RX24_STOCK_RIGHT}
							 });
		SELFTEST_set_actions(&STOCK_run_command, 3, 3, (SELFTEST_action_t[]){
								 {ACT_STOCK_LEFT_OPEN,		0,  QUEUE_ACT_RX24_STOCK_LEFT},
								 {ACT_STOCK_LEFT_CLOSE,		0,  QUEUE_ACT_RX24_STOCK_LEFT},
								 {ACT_STOCK_LEFT_OPEN,		0,  QUEUE_ACT_RX24_STOCK_LEFT}
							 });
	}
	return FALSE;
}

// Fonction appellée par la queue pendant tout le temps de la commande en cours (le booléen init est à TRUE au premier lancement de la commande)
void STOCK_run_command(queue_id_t queueId, bool_e init) {
	if(QUEUE_has_error(queueId)) {
		QUEUE_behead(queueId);
		return;
	}

	if(QUEUE_get_act(queueId) == QUEUE_ACT_RX24_STOCK_LEFT || QUEUE_get_act(queueId) == QUEUE_ACT_RX24_STOCK_RIGHT){
		if(init)
			STOCK_command_init(queueId);
		else
			STOCK_command_run(queueId);
	}
}

//Initialise une commande
static void STOCK_command_init(queue_id_t queueId) {
	Uint8 command = QUEUE_get_arg(queueId)->canCommand;
	Uint16 rx24_goalPosition_left_floor1 = 0xFFFF, rx24_goalPosition_right_floor1 = 0xFFFF;
	Uint16 rx24_goalPosition_left_floor2 = 0xFFFF, rx24_goalPosition_right_floor2 = 0xFFFF;
	bool_e result = TRUE;

	STOCK_initRX24();

	switch(command) {
		// Listing de toutes les positions de l'actionneur possible avec les valeurs de position associées
		case ACT_STOCK_LEFT_CLOSE :
		case ACT_STOCK_LEFT_LOCK :
		case ACT_STOCK_LEFT_UNLOCK :
		case ACT_STOCK_LEFT_OPEN :
		case ACT_STOCK_RIGHT_CLOSE :
		case ACT_STOCK_RIGHT_LOCK :
		case ACT_STOCK_RIGHT_UNLOCK :
		case ACT_STOCK_RIGHT_OPEN :
			STOCK_get_position(QUEUE_get_act(queueId), command, &rx24_goalPosition_right_floor1, &rx24_goalPosition_left_floor1,
																&rx24_goalPosition_right_floor2, &rx24_goalPosition_left_floor2);
			break;

		case ACT_STOCK_LEFT_STOP :
			AX12_set_torque_enabled(STOCKL_R_F1_RX24_ID, FALSE);
			AX12_set_torque_enabled(STOCKL_L_F1_RX24_ID, FALSE);
			AX12_set_torque_enabled(STOCKL_R_F2_RX24_ID, FALSE);
			AX12_set_torque_enabled(STOCKL_L_F2_RX24_ID, FALSE);
			QUEUE_next(queueId, ACT_STOCK_LEFT, ACT_RESULT_DONE, ACT_RESULT_ERROR_OK, __LINE__);
			return;
		case ACT_STOCK_RIGHT_STOP :
			AX12_set_torque_enabled(STOCKL_R_F1_RX24_ID, FALSE);
			AX12_set_torque_enabled(STOCKL_L_F1_RX24_ID, FALSE);
			AX12_set_torque_enabled(STOCKL_R_F2_RX24_ID, FALSE);
			AX12_set_torque_enabled(STOCKL_L_F2_RX24_ID, FALSE);
			QUEUE_next(queueId, ACT_STOCK_RIGHT, ACT_RESULT_DONE, ACT_RESULT_ERROR_OK, __LINE__);
			return;

		default:
			error_printf("Invalid exemple command: %u, code is broken !\n", command);
			if(QUEUE_get_act(queueId) == QUEUE_ACT_RX24_STOCK_RIGHT)
				QUEUE_next(queueId, ACT_STOCK_RIGHT, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_LOGIC, __LINE__);
			else
				QUEUE_next(queueId, ACT_STOCK_LEFT, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_LOGIC, __LINE__);
			return;
	}

	if(QUEUE_get_act(queueId) == QUEUE_ACT_RX24_STOCK_RIGHT){
		if(stock_act[0].is_initialized == FALSE || stock_act[1].is_initialized == FALSE
				|| stock_act[4].is_initialized == FALSE || stock_act[5].is_initialized == FALSE){
			error_printf("Impossible de mettre l'actionneur en position il n'est pas initialisé\n");
			QUEUE_next(queueId, ACT_STOCK_RIGHT, ACT_RESULT_FAILED, ACT_RESULT_ERROR_NOT_HERE, __LINE__);
			return;
		}

		if(rx24_goalPosition_left_floor1 == 0xFFFF || rx24_goalPosition_right_floor1 == 0xFFFF
				|| rx24_goalPosition_left_floor2 == 0xFFFF || rx24_goalPosition_right_floor2 == 0xFFFF) {
			error_printf("Invalid rx24 position for command: %u, code is broken !\n", command);
			QUEUE_next(queueId, ACT_STOCK_RIGHT, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_LOGIC, __LINE__);
			return;
		}

		//Sécurité anti terroriste. Nous les parano on aime pas voir des erreurs là ou il n'y en a pas.
		AX12_reset_last_error(stock_act[0].servo_id);
		AX12_reset_last_error(stock_act[1].servo_id);
		AX12_reset_last_error(stock_act[4].servo_id);
		AX12_reset_last_error(stock_act[5].servo_id);

		//Si la commande n'a pas été envoyée correctement et/ou que l'AX12 ne répond pas a cet envoi, on l'indique à la carte stratégie
		if(!AX12_set_position(stock_act[0].servo_id, rx24_goalPosition_right_floor1)){
			error_printf("AX12_set_position error: 0x%x (mord droit pince droite etage 1)\n", AX12_get_last_error(stock_act[0].servo_id).error);
			result = FALSE;
		}else
			debug_printf("Move STOCK rx24(%d) to %d\n", stock_act[0].servo_id, rx24_goalPosition_right_floor1);

		if(!AX12_set_position(stock_act[1].servo_id, rx24_goalPosition_left_floor1)){
			error_printf("AX12_set_position error: 0x%x (mord gauche pince droite etage 1)\n", AX12_get_last_error(stock_act[1].servo_id).error);
			result = FALSE;
		}else
			debug_printf("Move STOCK rx24(%d) to %d\n", stock_act[1].servo_id, rx24_goalPosition_left_floor1);

		if(!AX12_set_position(stock_act[4].servo_id, rx24_goalPosition_right_floor2)){
			error_printf("AX12_set_position error: 0x%x (mord droit pince droite etage 2)\n", AX12_get_last_error(stock_act[4].servo_id).error);
			result = FALSE;
		}else
			debug_printf("Move STOCK rx24(%d) to %d\n", stock_act[4].servo_id, rx24_goalPosition_right_floor2);

		if(!AX12_set_position(stock_act[5].servo_id, rx24_goalPosition_left_floor2)){
			error_printf("AX12_set_position error: 0x%x (mord gauche pince droite etage 2)\n", AX12_get_last_error(stock_act[5].servo_id).error);
			result = FALSE;
		}else
			debug_printf("Move STOCK rx24(%d) to %d\n", stock_act[5].servo_id, rx24_goalPosition_left_floor2);


		if(result == FALSE){
			QUEUE_next(queueId, ACT_STOCK_RIGHT, ACT_RESULT_FAILED, ACT_RESULT_ERROR_NOT_HERE, __LINE__);
			return;
		}

	}else{
		if(stock_act[2].is_initialized == FALSE || stock_act[3].is_initialized == FALSE
				|| stock_act[6].is_initialized == FALSE || stock_act[7].is_initialized == FALSE){
			error_printf("Impossible de mettre l'actionneur en position il n'est pas initialisé\n");
			QUEUE_next(queueId, ACT_STOCK_LEFT, ACT_RESULT_FAILED, ACT_RESULT_ERROR_NOT_HERE, __LINE__);
			return;
		}

		if(rx24_goalPosition_left_floor1 == 0xFFFF || rx24_goalPosition_right_floor1 == 0xFFFF
				|| rx24_goalPosition_left_floor2 == 0xFFFF || rx24_goalPosition_right_floor2 == 0xFFFF) {
			error_printf("Invalid rx24 position for command: %u, code is broken !\n", command);
			QUEUE_next(queueId, ACT_STOCK_LEFT, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_LOGIC, __LINE__);
			return;
		}

		//Sécurité anti terroriste. Nous les parano on aime pas voir des erreurs là ou il n'y en a pas.
		AX12_reset_last_error(stock_act[2].servo_id);
		AX12_reset_last_error(stock_act[3].servo_id);
		AX12_reset_last_error(stock_act[6].servo_id);
		AX12_reset_last_error(stock_act[7].servo_id);

		//Si la commande n'a pas été envoyée correctement et/ou que l'AX12 ne répond pas a cet envoi, on l'indique à la carte stratégie
		if(!AX12_set_position(stock_act[2].servo_id, rx24_goalPosition_right_floor1)){
			error_printf("AX12_set_position error: 0x%x (mord droit pince gauche etage 1)\n", AX12_get_last_error(stock_act[2].servo_id).error);
			result = FALSE;
		}else
			debug_printf("Move STOCK rx24(%d) to %d\n", stock_act[2].servo_id, rx24_goalPosition_right_floor1);

		if(!AX12_set_position(stock_act[3].servo_id, rx24_goalPosition_left_floor1)){
			error_printf("AX12_set_position error: 0x%x (mord gauche pince gauche etage 1)\n", AX12_get_last_error(stock_act[3].servo_id).error);
			result = FALSE;
		}else
			debug_printf("Move STOCK rx24(%d) to %d\n", stock_act[3].servo_id, rx24_goalPosition_left_floor1);

		if(!AX12_set_position(stock_act[6].servo_id, rx24_goalPosition_right_floor2)){
			error_printf("AX12_set_position error: 0x%x (mord droit pince gauche etage 2)\n", AX12_get_last_error(stock_act[6].servo_id).error);
			result = FALSE;
		}else
			debug_printf("Move STOCK rx24(%d) to %d\n", stock_act[6].servo_id, rx24_goalPosition_right_floor2);

		if(!AX12_set_position(stock_act[7].servo_id, rx24_goalPosition_left_floor2)){
			error_printf("AX12_set_position error: 0x%x (mord gauche pince gauche etage 2)\n", AX12_get_last_error(stock_act[7].servo_id).error);
			result = FALSE;
		}else
			debug_printf("Move STOCK rx24(%d) to %d\n", stock_act[7].servo_id, rx24_goalPosition_left_floor2);


		if(result == FALSE){
			QUEUE_next(queueId, ACT_STOCK_LEFT, ACT_RESULT_FAILED, ACT_RESULT_ERROR_NOT_HERE, __LINE__);
			return;
		}
	}
}

//Gère les états pendant le mouvement de l'AX12
static void STOCK_command_run(queue_id_t queueId) {
	Uint8 result_left[2], result_right[2], errorCode_left[2], errorCode_right[2];
	Uint16 line_left[2], line_right[2];
	Uint16 rx24_goalPosition_left_floor1 = 0xFFFF, rx24_goalPosition_right_floor1 = 0xFFFF;
	Uint16 rx24_goalPosition_left_floor2 = 0xFFFF, rx24_goalPosition_right_floor2 = 0xFFFF;
	static bool_e done_right[2] = {FALSE, FALSE}, done_left[2] = {FALSE, FALSE};

	if(QUEUE_get_act(queueId) == QUEUE_ACT_RX24_STOCK_RIGHT){

		STOCK_get_position(QUEUE_get_act(queueId), QUEUE_get_arg(queueId)->canCommand, &rx24_goalPosition_right_floor1, &rx24_goalPosition_left_floor1,
																					   &rx24_goalPosition_right_floor2, &rx24_goalPosition_left_floor2);

		if(done_right[0] == FALSE)
			done_right[0] = ACTQ_check_status_ax12(queueId, stock_act[0].servo_id, rx24_goalPosition_right_floor1,
					STOCK_RX24_ASSER_POS_EPSILON, STOCK_RX24_ASSER_TIMEOUT, STOCK_RX24_ASSER_POS_LARGE_EPSILON, &(result_right[0]), &(errorCode_right[0]), &(line_right[0]));

		if(done_left[0] == FALSE)
			done_left[0] = ACTQ_check_status_ax12(queueId, stock_act[1].servo_id, rx24_goalPosition_left_floor1,
					STOCK_RX24_ASSER_POS_EPSILON, STOCK_RX24_ASSER_TIMEOUT, STOCK_RX24_ASSER_POS_LARGE_EPSILON, &(result_left[0]), &(errorCode_left[0]), &(line_left[0]));

		if(done_right[1] == FALSE)
			done_right[1] = ACTQ_check_status_ax12(queueId, stock_act[4].servo_id, rx24_goalPosition_right_floor2,
					STOCK_RX24_ASSER_POS_EPSILON, STOCK_RX24_ASSER_TIMEOUT, STOCK_RX24_ASSER_POS_LARGE_EPSILON, &(result_right[1]), &(errorCode_right[1]), &(line_right[1]));

		if(done_left[1] == FALSE)
			done_left[1] = ACTQ_check_status_ax12(queueId, stock_act[5].servo_id, rx24_goalPosition_left_floor2,
					STOCK_RX24_ASSER_POS_EPSILON, STOCK_RX24_ASSER_TIMEOUT, STOCK_RX24_ASSER_POS_LARGE_EPSILON, &(result_left[1]), &(errorCode_left[1]), &(line_left[1]));

		if(done_right[0] && done_left[0] && done_right[1] && done_left[1]){
			done_right[0] = FALSE;
			done_left[0] = FALSE;
			done_right[1] = FALSE;
			done_left[1] = FALSE;
			if(result_right[0] == ACT_RESULT_DONE && result_left[0] == ACT_RESULT_DONE && result_right[1] == ACT_RESULT_DONE && result_left[1] == ACT_RESULT_DONE)
				QUEUE_next(queueId, ACT_STOCK_RIGHT, ACT_RESULT_DONE, ACT_RESULT_ERROR_OK, 0x0100);
			else if(result_right[0] != ACT_RESULT_DONE)
				QUEUE_next(queueId, ACT_STOCK_RIGHT, result_right[0], errorCode_right[0], line_right[0]);
			else if(result_left[0] != ACT_RESULT_DONE)
				QUEUE_next(queueId, ACT_STOCK_RIGHT, result_left[0], errorCode_left[0], line_left[0]);
			else if(result_right[1] != ACT_RESULT_DONE)
				QUEUE_next(queueId, ACT_STOCK_RIGHT, result_right[1], errorCode_right[1], line_right[1]);
			else
				QUEUE_next(queueId, ACT_STOCK_RIGHT, result_left[1], errorCode_left[1], line_left[1]);
		}

	}else{
		STOCK_get_position(QUEUE_get_act(queueId), QUEUE_get_arg(queueId)->canCommand, &rx24_goalPosition_right_floor1, &rx24_goalPosition_left_floor1,
																					   &rx24_goalPosition_right_floor2, &rx24_goalPosition_left_floor2);

		if(done_right[0] == FALSE)
			done_right[0] = ACTQ_check_status_ax12(queueId, stock_act[2].servo_id, rx24_goalPosition_right_floor1,
					STOCK_RX24_ASSER_POS_EPSILON, STOCK_RX24_ASSER_TIMEOUT, STOCK_RX24_ASSER_POS_LARGE_EPSILON, &(result_right[0]), &(errorCode_right[0]), &(line_right[0]));

		if(done_left[0] == FALSE)
			done_left[0] = ACTQ_check_status_ax12(queueId, stock_act[3].servo_id, rx24_goalPosition_left_floor1,
					STOCK_RX24_ASSER_POS_EPSILON, STOCK_RX24_ASSER_TIMEOUT, STOCK_RX24_ASSER_POS_LARGE_EPSILON, &(result_left[0]), &(errorCode_left[0]), &(line_left[0]));

		if(done_right[1] == FALSE)
			done_right[1] = ACTQ_check_status_ax12(queueId, stock_act[6].servo_id, rx24_goalPosition_right_floor2,
					STOCK_RX24_ASSER_POS_EPSILON, STOCK_RX24_ASSER_TIMEOUT, STOCK_RX24_ASSER_POS_LARGE_EPSILON, &(result_right[1]), &(errorCode_right[1]), &(line_right[1]));

		if(done_left[1] == FALSE)
			done_left[1] = ACTQ_check_status_ax12(queueId, stock_act[7].servo_id, rx24_goalPosition_left_floor2,
					STOCK_RX24_ASSER_POS_EPSILON, STOCK_RX24_ASSER_TIMEOUT, STOCK_RX24_ASSER_POS_LARGE_EPSILON, &(result_left[1]), &(errorCode_left[1]), &(line_left[1]));

		if(done_right[0] && done_left[0] && done_right[1] && done_left[1]){
			done_right[0] = FALSE;
			done_left[0] = FALSE;
			done_right[1] = FALSE;
			done_left[1] = FALSE;
			if(result_right[0] == ACT_RESULT_DONE && result_left[0] == ACT_RESULT_DONE && result_right[1] == ACT_RESULT_DONE && result_left[1] == ACT_RESULT_DONE)
				QUEUE_next(queueId, ACT_STOCK_LEFT, ACT_RESULT_DONE, ACT_RESULT_ERROR_OK, 0x0100);
			else if(result_right[0] != ACT_RESULT_DONE)
				QUEUE_next(queueId, ACT_STOCK_LEFT, result_right[0], errorCode_right[0], line_right[0]);
			else if(result_left[0] != ACT_RESULT_DONE)
				QUEUE_next(queueId, ACT_STOCK_LEFT, result_left[0], errorCode_left[0], line_left[0]);
			else if(result_right[1] != ACT_RESULT_DONE)
				QUEUE_next(queueId, ACT_STOCK_LEFT, result_right[1], errorCode_right[1], line_right[1]);
			else
				QUEUE_next(queueId, ACT_STOCK_LEFT, result_left[1], errorCode_left[1], line_left[1]);
		}
	}
}

static void STOCK_get_position(QUEUE_act_e act_id, Uint8 command, Uint16 *right_f1_pos, Uint16 *left_f1_pos, Uint16 *right_f2_pos, Uint16 *left_f2_pos){
	if(act_id == QUEUE_ACT_RX24_STOCK_RIGHT){
		switch(command){
			case ACT_STOCK_RIGHT_CLOSE :
				*right_f1_pos = STOCKR_RIGHT_F1_CLOSE;
				*left_f1_pos = STOCKR_LEFT_F1_CLOSE;
				*right_f2_pos = STOCKR_RIGHT_F2_CLOSE;
				*left_f2_pos = STOCKR_LEFT_F2_CLOSE;
				break;

			case ACT_STOCK_RIGHT_LOCK :
				*right_f1_pos = STOCKR_RIGHT_F1_LOCK;
				*left_f1_pos = STOCKR_LEFT_F1_LOCK;
				*right_f2_pos = STOCKR_RIGHT_F2_LOCK;
				*left_f2_pos = STOCKR_LEFT_F2_LOCK;
				break;

			case ACT_STOCK_RIGHT_UNLOCK :
				*right_f1_pos = STOCKR_RIGHT_F1_UNLOCK;
				*left_f1_pos = STOCKR_LEFT_F1_UNLOCK;
				*right_f2_pos = STOCKR_RIGHT_F2_UNLOCK;
				*left_f2_pos = STOCKR_LEFT_F2_UNLOCK;
				break;

			case ACT_STOCK_RIGHT_OPEN :
				*right_f1_pos = STOCKR_RIGHT_F1_OPEN;
				*left_f1_pos = STOCKR_LEFT_F1_OPEN;
				*right_f2_pos = STOCKR_RIGHT_F2_OPEN;
				*left_f2_pos = STOCKR_LEFT_F2_OPEN;
				break;

			default:
				*right_f1_pos = 0xFFFF;
				*left_f1_pos = 0xFFFF;
				*right_f2_pos = 0xFFFF;
				*left_f2_pos = 0xFFFF;
				break;
		}
	}else if(act_id == QUEUE_ACT_RX24_STOCK_LEFT){
		switch(command){
			case ACT_STOCK_LEFT_CLOSE :
				*right_f1_pos = STOCKL_RIGHT_F1_CLOSE;
				*left_f1_pos = STOCKL_LEFT_F1_CLOSE;
				*right_f2_pos = STOCKL_RIGHT_F2_CLOSE;
				*left_f2_pos = STOCKL_LEFT_F2_CLOSE;
				break;

			case ACT_STOCK_LEFT_LOCK :
				*right_f1_pos = STOCKL_RIGHT_F1_LOCK;
				*left_f1_pos = STOCKL_LEFT_F1_LOCK;
				*right_f2_pos = STOCKL_RIGHT_F2_LOCK;
				*left_f2_pos = STOCKL_LEFT_F2_LOCK;
				break;

			case ACT_STOCK_LEFT_UNLOCK :
				*right_f1_pos = STOCKL_RIGHT_F1_UNLOCK;
				*left_f1_pos = STOCKL_LEFT_F1_UNLOCK;
				*right_f2_pos = STOCKL_RIGHT_F2_UNLOCK;
				*left_f2_pos = STOCKL_LEFT_F2_UNLOCK;
				break;

			case ACT_STOCK_LEFT_OPEN :
				*right_f1_pos = STOCKL_RIGHT_F1_OPEN;
				*left_f1_pos = STOCKL_LEFT_F1_OPEN;
				*right_f2_pos = STOCKL_RIGHT_F2_OPEN;
				*left_f2_pos = STOCKL_LEFT_F2_OPEN;
				break;

			default:
				*right_f1_pos = 0xFFFF;
				*left_f1_pos = 0xFFFF;
				*right_f2_pos = 0xFFFF;
				*left_f2_pos = 0xFFFF;
				break;
		}
	}else{
		*right_f1_pos = 0xFFFF;
		*left_f1_pos = 0xFFFF;
		*right_f2_pos = 0xFFFF;
		*left_f2_pos = 0xFFFF;
	}
}
#endif
