/*  Club Robot ESEO 2014 - 2015
 *	HOLLY - WOOD
 *
 *	Fichier : pincemi_RX24.c
 *	Package : Carte actionneur
 *	Description : Gestion des pinces de l'actionneur spot
 *  Auteur : Arnaud
 *  Version 20130219
 *  Robot : HOLLY
 */


#include "pincemi_RX24.h"

#ifdef I_AM_ROBOT_BIG

// Les différents includes nécessaires...
#include "../QS/QS_CANmsgList.h"
#include "../QS/QS_ax12.h"
#include "../QS/QS_can.h"
#include "../act_queue_utils.h"
#include "../selftest.h"
#include "../ActManager.h"

#include "pincemi_RX24_config.h"

#define LOG_PREFIX "pincemi : "
#define LOG_COMPONENT OUTPUT_LOG_COMPONENT_PINCEMI
#include "../QS/QS_outputlog.h"

static void PINCEMI_command_run(queue_id_t queueId);
static void PINCEMI_initRX24();
static void PINCEMI_command_init(queue_id_t queueId);
static void PINCEMI_config(CAN_msg_t* msg);
static void PINCEMI_get_position(QUEUE_act_e act_id, Uint8 command, Uint16 *right_pos, Uint16 *left_pos);

typedef struct{
	bool_e is_initialized;
	Uint8 servo_id;
	Uint16 init_pos;
}pincemi_act_s;

pincemi_act_s pincemi_act[4] = {
	{FALSE, PINCEMIR_R_RX24_ID, PINCEMIR_RIGHT_INIT_POS},
	{FALSE, PINCEMIR_L_RX24_ID, PINCEMIR_LEFT_INIT_POS},
	{FALSE, PINCEMIL_R_RX24_ID, PINCEMIL_RIGHT_INIT_POS},
	{FALSE, PINCEMIL_L_RX24_ID, PINCEMIL_LEFT_INIT_POS}
};

// Fonction appellée au lancement de la carte (via ActManager)
void PINCEMI_init() {
	static bool_e initialized = FALSE;

	if(initialized)
		return;
	initialized = TRUE;

	AX12_init();
	PINCEMI_initRX24();
}

// Fonction appellée si la carte IHM a détecté une grosse chutte de la tension d'alimentation des servos
// Pour éviter les problèmes d'utilisation de servo non initialisé
void PINCEMI_reset_config(){
	Uint8 i;
	for(i=0;i<4;i++)
		pincemi_act[i].is_initialized = FALSE;
	PINCEMI_initRX24();
}

//Initialise l'AX12 s'il n'était pas alimenté lors d'initialisations précédentes, si déjà initialisé, ne fait rien
static void PINCEMI_initRX24() {
	Uint8 i;
	for(i=0;i<4;i++){
		if(pincemi_act[i].is_initialized == FALSE && AX12_is_ready(pincemi_act[i].servo_id) == TRUE) {
			pincemi_act[i].is_initialized = TRUE;
			AX12_config_set_lowest_voltage(pincemi_act[i].servo_id, AX12_MIN_VOLTAGE);
			AX12_config_set_highest_voltage(pincemi_act[i].servo_id, AX12_MAX_VOLTAGE);
			AX12_set_torque_limit(pincemi_act[i].servo_id, PINCEMI_RX24_MAX_TORQUE_PERCENT);
			//AX12_config_set_return_delay_time(pincemi_act[i].servo_id, Uint16 delay_us);
			AX12_config_set_temperature_limit(pincemi_act[i].servo_id, PINCEMI_RX24_MAX_TEMPERATURE);

			AX12_config_set_maximal_angle(pincemi_act[i].servo_id, PINCEMI_RX24_MAX_VALUE);
			AX12_config_set_minimal_angle(pincemi_act[i].servo_id, PINCEMI_RX24_MIN_VALUE);

			AX12_config_set_error_before_led(pincemi_act[i].servo_id, AX12_BEFORE_LED);
			AX12_config_set_error_before_shutdown(pincemi_act[i].servo_id, AX12_BEFORE_SHUTDOWN);
			debug_printf("Init config rx24(%d) DONE\n", pincemi_act[i].servo_id);
		}else if(pincemi_act[i].is_initialized == FALSE)
			debug_printf("Init config rx24(%d) FAIL\n", pincemi_act[i].servo_id);
	}
}


// Fonction appellée pour la modification des configurations de l'ax12 telle que la vitesse et le couple (via ActManager)
// Dans le cas de multiple actionneur appartenant à un même actionneur, ajouter des defines dans QS_CANmsgList.h afin de pouvoir les choisirs facilement depuis la stratégie
void PINCEMI_config(CAN_msg_t* msg){
	if(msg->sid == ACT_PINCEMI_RIGHT){
		switch(msg->data[1]){
			case 0 :
				ACTMGR_config_AX12(PINCEMIR_R_RX24_ID, msg);
				break;

			case 1 :
				ACTMGR_config_AX12(PINCEMIR_L_RX24_ID, msg);
				break;
			default :
				warn_printf("invalid CAN msg data[1]=%u (sous actionneur inexistant)!\n", msg->data[1]);
		}
	}else if(msg->sid == ACT_PINCEMI_LEFT){
		switch(msg->data[1]){
			case 0 :
				ACTMGR_config_AX12(PINCEMIL_R_RX24_ID, msg);
				break;

			case 1 :
				ACTMGR_config_AX12(PINCEMIL_L_RX24_ID, msg);
				break;
			default :
				warn_printf("invalid CAN msg data[1]=%u (sous actionneur inexistant)!\n", msg->data[1]);
		}
	}
}

// Fonction appellée pour l'initialisation en position de le RX24 dés l'arrivé de l'alimentation (via ActManager)
void PINCEMI_init_pos(){
	Uint8 i;

	PINCEMI_initRX24();

	debug_printf("Init pos : \n");
	for(i=0;i<4;i++){
		if(pincemi_act[i].is_initialized == TRUE){
			if(!AX12_set_position(pincemi_act[i].servo_id, pincemi_act[i].init_pos))
				debug_printf("   Le RX24 n°%d n'est pas là\n", pincemi_act[i].servo_id);
			else
				debug_printf("   Le RX24 n°%d a été initialisé en position\n", pincemi_act[i].servo_id);
		}
	}
}

// Fonction appellée à la fin du match (via ActManager)
void PINCEMI_stop(){
	Uint8 i;
	for(i=0;i<4;i++)
		AX12_set_torque_enabled(pincemi_act[i].servo_id, FALSE);
}

// fonction appellée à la réception d'un message CAN (via ActManager)
bool_e PINCEMI_CAN_process_msg(CAN_msg_t* msg) {
	switch(msg->sid){
		case ACT_PINCEMI_RIGHT :
			PINCEMI_initRX24();
			switch(msg->data[0]) {
				// Listing de toutes les positions de l'actionneur possible
				case ACT_PINCEMI_RIGHT_CLOSE :
				case ACT_PINCEMI_RIGHT_CLOSE_INNER :
				case ACT_PINCEMI_RIGHT_LOCK :
				case ACT_PINCEMI_RIGHT_LOCK_BALL :
				case ACT_PINCEMI_RIGHT_UNLOCK :
				case ACT_PINCEMI_RIGHT_OPEN :
				case ACT_PINCEMI_RIGHT_OPEN_GREAT :
				case ACT_PINCEMI_RIGHT_STOP :
					ACTQ_push_operation_from_msg(msg, QUEUE_ACT_RX24_PINCEMI_RIGHT, &PINCEMI_run_command, 0, TRUE);
					break;

				case ACT_CONFIG :
					PINCEMI_config(msg);
					break;


				default:
					component_printf(LOG_LEVEL_Warning, "invalid CAN msg data[0]=%u !\n", msg->data[0]);
			}
			return TRUE;

		case ACT_PINCEMI_LEFT :
			PINCEMI_initRX24();
			switch(msg->data[0]) {
				// Listing de toutes les positions de l'actionneur possible
				case ACT_PINCEMI_LEFT_CLOSE :
				case ACT_PINCEMI_LEFT_CLOSE_INNER :
				case ACT_PINCEMI_LEFT_LOCK :
				case ACT_PINCEMI_LEFT_LOCK_BALL :
				case ACT_PINCEMI_LEFT_UNLOCK :
				case ACT_PINCEMI_LEFT_OPEN :
				case ACT_PINCEMI_LEFT_OPEN_GREAT :
				case ACT_PINCEMI_LEFT_STOP :
					ACTQ_push_operation_from_msg(msg, QUEUE_ACT_RX24_PINCEMI_LEFT, &PINCEMI_run_command, 0, TRUE);
					break;

				case ACT_CONFIG :
					PINCEMI_config(msg);
					break;


				default:
					component_printf(LOG_LEVEL_Warning, "invalid CAN msg data[0]=%u !\n", msg->data[0]);
			}
			return TRUE;

		case ACT_DO_SELFTEST :
			// Lister les différents états que l'actionneur doit réaliser pour réussir le selftest

			SELFTEST_set_actions(&PINCEMI_run_command, 300, 3, (SELFTEST_action_t[]){
								 {ACT_PINCEMI_RIGHT_OPEN,		0,  QUEUE_ACT_RX24_PINCEMI_RIGHT},
								 {ACT_PINCEMI_RIGHT_CLOSE,		0,  QUEUE_ACT_RX24_PINCEMI_RIGHT},
								 {ACT_PINCEMI_RIGHT_OPEN,		0,  QUEUE_ACT_RX24_PINCEMI_RIGHT}
							 });
			SELFTEST_set_actions(&PINCEMI_run_command, 300, 3, (SELFTEST_action_t[]){
								 {ACT_PINCEMI_LEFT_OPEN,		0,  QUEUE_ACT_RX24_PINCEMI_LEFT},
								 {ACT_PINCEMI_LEFT_CLOSE,		0,  QUEUE_ACT_RX24_PINCEMI_LEFT},
								 {ACT_PINCEMI_LEFT_OPEN,		0,  QUEUE_ACT_RX24_PINCEMI_LEFT}
							 });
			break;
	}

	return FALSE;
}

// Fonction appellée par la queue pendant tout le temps de la commande en cours (le booléen init est à TRUE au premier lancement de la commande)
void PINCEMI_run_command(queue_id_t queueId, bool_e init) {
	if(QUEUE_has_error(queueId)) {
		QUEUE_behead(queueId);
		return;
	}

	if(QUEUE_get_act(queueId) == QUEUE_ACT_RX24_PINCEMI_LEFT || QUEUE_get_act(queueId) == QUEUE_ACT_RX24_PINCEMI_RIGHT){
		if(init)
			PINCEMI_command_init(queueId);
		else
			PINCEMI_command_run(queueId);
	}
}

//Initialise une commande
static void PINCEMI_command_init(queue_id_t queueId) {
	Uint8 command = QUEUE_get_arg(queueId)->canCommand;
	Uint16 rx24_goalPosition_left = 0xFFFF, rx24_goalPosition_right = 0xFFFF;
	bool_e result = TRUE;

	PINCEMI_initRX24();

	switch(command) {
		// Listing de toutes les positions de l'actionneur possible avec les valeurs de position associées
		case ACT_PINCEMI_LEFT_CLOSE :
		case ACT_PINCEMI_LEFT_CLOSE_INNER :
		case ACT_PINCEMI_LEFT_LOCK :
		case ACT_PINCEMI_LEFT_LOCK_BALL :
		case ACT_PINCEMI_LEFT_UNLOCK :
		case ACT_PINCEMI_LEFT_OPEN :
		case ACT_PINCEMI_LEFT_OPEN_GREAT :
		case ACT_PINCEMI_RIGHT_CLOSE :
		case ACT_PINCEMI_RIGHT_CLOSE_INNER :
		case ACT_PINCEMI_RIGHT_LOCK :
		case ACT_PINCEMI_RIGHT_LOCK_BALL :
		case ACT_PINCEMI_RIGHT_UNLOCK :
		case ACT_PINCEMI_RIGHT_OPEN :
		case ACT_PINCEMI_RIGHT_OPEN_GREAT :
			PINCEMI_get_position(QUEUE_get_act(queueId), command, &rx24_goalPosition_right, &rx24_goalPosition_left);
			break;

		case ACT_PINCEMI_LEFT_STOP :
			AX12_set_torque_enabled(PINCEMIL_R_RX24_ID, FALSE);
			AX12_set_torque_enabled(PINCEMIL_L_RX24_ID, FALSE);
			QUEUE_next(queueId, ACT_PINCEMI_LEFT, ACT_RESULT_DONE, ACT_RESULT_ERROR_OK, __LINE__);
			return;
		case ACT_PINCEMI_RIGHT_STOP :
			AX12_set_torque_enabled(PINCEMIL_R_RX24_ID, FALSE);
			AX12_set_torque_enabled(PINCEMIL_L_RX24_ID, FALSE);
			QUEUE_next(queueId, ACT_PINCEMI_RIGHT, ACT_RESULT_DONE, ACT_RESULT_ERROR_OK, __LINE__);
			return;

		default:
			error_printf("Invalid exemple command: %u, code is broken !\n", command);
			if(QUEUE_get_act(queueId) == QUEUE_ACT_RX24_PINCEMI_RIGHT)
				QUEUE_next(queueId, ACT_PINCEMI_RIGHT, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_LOGIC, __LINE__);
			else
				QUEUE_next(queueId, ACT_PINCEMI_LEFT, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_LOGIC, __LINE__);
			return;
	}

	if(QUEUE_get_act(queueId) == QUEUE_ACT_RX24_PINCEMI_RIGHT){
		if(pincemi_act[0].is_initialized == FALSE || pincemi_act[1].is_initialized == FALSE){
			error_printf("Impossible de mettre l'actionneur en position il n'est pas initialisé\n");
			QUEUE_next(queueId, ACT_PINCEMI_RIGHT, ACT_RESULT_FAILED, ACT_RESULT_ERROR_NOT_HERE, __LINE__);
			return;
		}

		if(rx24_goalPosition_left == 0xFFFF || rx24_goalPosition_right == 0xFFFF) {
			error_printf("Invalid rx24 position for command: %u, code is broken !\n", command);
			QUEUE_next(queueId, ACT_PINCEMI_RIGHT, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_LOGIC, __LINE__);
			return;
		}

		//Sécurité anti terroriste. Nous les parano on aime pas voir des erreurs là ou il n'y en a pas.
		AX12_reset_last_error(pincemi_act[0].servo_id);
		AX12_reset_last_error(pincemi_act[1].servo_id);

		//Si la commande n'a pas été envoyée correctement et/ou que l'AX12 ne répond pas a cet envoi, on l'indique à la carte stratégie
		if(!AX12_set_position(pincemi_act[0].servo_id, rx24_goalPosition_right)){
			error_printf("AX12_set_position error: 0x%x (mord droit pince droite)\n", AX12_get_last_error(pincemi_act[0].servo_id).error);
			result = FALSE;
		}else
			debug_printf("Move PINCEMI rx24(%d) to %d\n", pincemi_act[0].servo_id, rx24_goalPosition_right);

		if(!AX12_set_position(pincemi_act[1].servo_id, rx24_goalPosition_left)){
			error_printf("AX12_set_position error: 0x%x (mord gauche pince droite)\n", AX12_get_last_error(pincemi_act[1].servo_id).error);
			result = FALSE;
		}else
			debug_printf("Move PINCEMI rx24(%d) to %d\n", pincemi_act[1].servo_id, rx24_goalPosition_left);


		if(result == FALSE){
			QUEUE_next(queueId, ACT_PINCEMI_RIGHT, ACT_RESULT_FAILED, ACT_RESULT_ERROR_NOT_HERE, __LINE__);
			return;
		}

	}else{
		if(pincemi_act[2].is_initialized == FALSE || pincemi_act[3].is_initialized == FALSE){
			error_printf("Impossible de mettre l'actionneur en position il n'est pas initialisé\n");
			QUEUE_next(queueId, ACT_PINCEMI_LEFT, ACT_RESULT_FAILED, ACT_RESULT_ERROR_NOT_HERE, __LINE__);
			return;
		}

		if(rx24_goalPosition_left == 0xFFFF || rx24_goalPosition_right == 0xFFFF) {
			error_printf("Invalid rx24 position for command: %u, code is broken !\n", command);
			QUEUE_next(queueId, ACT_PINCEMI_LEFT, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_LOGIC, __LINE__);
			return;
		}

		//Sécurité anti terroriste. Nous les parano on aime pas voir des erreurs là ou il n'y en a pas.
		AX12_reset_last_error(pincemi_act[2].servo_id);
		AX12_reset_last_error(pincemi_act[3].servo_id);

		//Si la commande n'a pas été envoyée correctement et/ou que l'AX12 ne répond pas a cet envoi, on l'indique à la carte stratégie
		if(!AX12_set_position(pincemi_act[2].servo_id, rx24_goalPosition_right)){
			error_printf("AX12_set_position error: 0x%x (mord droit pince droite)\n", AX12_get_last_error(pincemi_act[2].servo_id).error);
			result = FALSE;
		}else
			debug_printf("Move PINCEMI ax12(%d) to %d\n", pincemi_act[2].servo_id, rx24_goalPosition_right);

		if(!AX12_set_position(pincemi_act[3].servo_id, rx24_goalPosition_left)){
			error_printf("AX12_set_position error: 0x%x (mord gauche pince droite)\n", AX12_get_last_error(pincemi_act[3].servo_id).error);
			result = FALSE;
		}else
			debug_printf("Move PINCEMI ax12(%d) to %d\n", pincemi_act[3].servo_id, rx24_goalPosition_left);


		if(result == FALSE){
			QUEUE_next(queueId, ACT_PINCEMI_LEFT, ACT_RESULT_FAILED, ACT_RESULT_ERROR_NOT_HERE, __LINE__);
			return;
		}
	}
}

//Gère les états pendant le mouvement de l'AX12
static void PINCEMI_command_run(queue_id_t queueId) {
	static Uint8 result_left, result_right, errorCode_left, errorCode_right;
	static Uint16 line_left, line_right;
	Uint16 rx24_goalPosition_left = 0xFFFF, rx24_goalPosition_right = 0xFFFF;
	static bool_e done_right[2] = {FALSE, FALSE}, done_left[2] = {FALSE, FALSE};

	if(QUEUE_get_act(queueId) == QUEUE_ACT_RX24_PINCEMI_RIGHT){

		PINCEMI_get_position(QUEUE_get_act(queueId), QUEUE_get_arg(queueId)->canCommand, &rx24_goalPosition_right, &rx24_goalPosition_left);

		if(done_right[0] == FALSE)
			done_right[0] = ACTQ_check_status_ax12(queueId, pincemi_act[0].servo_id, rx24_goalPosition_right,
					PINCEMI_RX24_ASSER_POS_EPSILON, PINCEMI_RX24_ASSER_TIMEOUT, PINCEMI_RX24_ASSER_POS_LARGE_EPSILON, &result_right, &errorCode_right, &line_right);

		if(done_left[0] == FALSE)
			done_left[0] = ACTQ_check_status_ax12(queueId, pincemi_act[1].servo_id, rx24_goalPosition_left,
					PINCEMI_RX24_ASSER_POS_EPSILON, PINCEMI_RX24_ASSER_TIMEOUT, PINCEMI_RX24_ASSER_POS_LARGE_EPSILON, &result_left, &errorCode_left, &line_left);

		if(done_right[0] && done_left[0]){
			done_right[0] = FALSE;
			done_left[0] = FALSE;
			if(result_right == ACT_RESULT_DONE && result_left == ACT_RESULT_DONE)
				QUEUE_next(queueId, ACT_PINCEMI_RIGHT, ACT_RESULT_DONE, ACT_RESULT_ERROR_OK, 0x0100);
			else if(result_right != ACT_RESULT_DONE)
				QUEUE_next(queueId, ACT_PINCEMI_RIGHT, result_right, errorCode_right, line_right);
			else
				QUEUE_next(queueId, ACT_PINCEMI_RIGHT, result_left, errorCode_left, line_left);
		}

	}else{
		PINCEMI_get_position(QUEUE_get_act(queueId), QUEUE_get_arg(queueId)->canCommand, &rx24_goalPosition_right, &rx24_goalPosition_left);

		if(done_right[1] == FALSE)
			done_right[1] = ACTQ_check_status_ax12(queueId, pincemi_act[2].servo_id, rx24_goalPosition_right,
					PINCEMI_RX24_ASSER_POS_EPSILON, PINCEMI_RX24_ASSER_TIMEOUT, PINCEMI_RX24_ASSER_POS_LARGE_EPSILON, &result_right, &errorCode_right, &line_right);

		if(done_left[1] == FALSE)
			done_left[1] = ACTQ_check_status_ax12(queueId, pincemi_act[3].servo_id, rx24_goalPosition_left,
					PINCEMI_RX24_ASSER_POS_EPSILON, PINCEMI_RX24_ASSER_TIMEOUT, PINCEMI_RX24_ASSER_POS_LARGE_EPSILON, &result_left, &errorCode_left, &line_left);

		if(done_right[1] && done_left[1]){
			done_right[1] = FALSE;
			done_left[1] = FALSE;
			if(result_right == ACT_RESULT_DONE && result_left == ACT_RESULT_DONE)
				QUEUE_next(queueId, ACT_PINCEMI_LEFT, ACT_RESULT_DONE, ACT_RESULT_ERROR_OK, 0x0100);
			else if(result_right != ACT_RESULT_DONE)
				QUEUE_next(queueId, ACT_PINCEMI_LEFT, result_right, errorCode_right, line_right);
			else
				QUEUE_next(queueId, ACT_PINCEMI_LEFT, result_left, errorCode_left, line_left);
		}
	}
}


static void PINCEMI_get_position(QUEUE_act_e act_id, Uint8 command, Uint16 *right_pos, Uint16 *left_pos){
	if(act_id == QUEUE_ACT_RX24_PINCEMI_RIGHT){
		switch(command){
			case ACT_PINCEMI_RIGHT_CLOSE :
				*right_pos = PINCEMIR_RIGHT_CLOSE;
				*left_pos = PINCEMIR_LEFT_CLOSE;
				break;

			case ACT_PINCEMI_RIGHT_CLOSE_INNER :
				*right_pos = PINCEMIR_RIGHT_CLOSE_INNER;
				*left_pos = PINCEMIR_LEFT_CLOSE_INNER;
				break;

			case ACT_PINCEMI_RIGHT_LOCK :
				*right_pos = PINCEMIR_RIGHT_LOCK;
				*left_pos = PINCEMIR_LEFT_LOCK;
				break;

			case ACT_PINCEMI_RIGHT_LOCK_BALL :
				*right_pos = PINCEMIR_RIGHT_LOCK_BALL;
				*left_pos = PINCEMIR_LEFT_LOCK_BALL;
				break;

			case ACT_PINCEMI_RIGHT_UNLOCK :
				*right_pos = PINCEMIR_RIGHT_UNLOCK;
				*left_pos = PINCEMIR_LEFT_UNLOCK;
				break;

			case ACT_PINCEMI_RIGHT_OPEN :
				*right_pos = PINCEMIR_RIGHT_OPEN;
				*left_pos = PINCEMIR_LEFT_OPEN;
				break;

			case ACT_PINCEMI_RIGHT_OPEN_GREAT :
				*right_pos = PINCEMIR_RIGHT_OPEN_GREAT;
				*left_pos = PINCEMIR_LEFT_OPEN_GREAT;
				break;

			default:
				*right_pos = 0xFFFF;
				*left_pos = 0xFFFF;
				break;
		}
	}else if(act_id == QUEUE_ACT_RX24_PINCEMI_LEFT){
		switch(command){
			case ACT_PINCEMI_LEFT_CLOSE :
				*right_pos = PINCEMIL_RIGHT_CLOSE;
				*left_pos = PINCEMIL_LEFT_CLOSE;
				break;

			case ACT_PINCEMI_LEFT_CLOSE_INNER :
				*right_pos = PINCEMIL_RIGHT_CLOSE_INNER;
				*left_pos = PINCEMIL_LEFT_CLOSE_INNER;
				break;

			case ACT_PINCEMI_LEFT_LOCK :
				*right_pos = PINCEMIL_RIGHT_LOCK;
				*left_pos = PINCEMIL_LEFT_LOCK;
				break;

			case ACT_PINCEMI_LEFT_LOCK_BALL :
				*right_pos = PINCEMIL_RIGHT_LOCK_BALL;
				*left_pos = PINCEMIL_LEFT_LOCK_BALL;
				break;

			case ACT_PINCEMI_LEFT_UNLOCK :
				*right_pos = PINCEMIL_RIGHT_LOCK;
				*left_pos = PINCEMIL_LEFT_LOCK;
				break;

			case ACT_PINCEMI_LEFT_OPEN :
				*right_pos = PINCEMIL_RIGHT_OPEN;
				*left_pos = PINCEMIL_LEFT_OPEN;
				break;

			case ACT_PINCEMI_LEFT_OPEN_GREAT :
				*right_pos = PINCEMIL_RIGHT_OPEN_GREAT;
				*left_pos = PINCEMIL_LEFT_OPEN_GREAT;
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
