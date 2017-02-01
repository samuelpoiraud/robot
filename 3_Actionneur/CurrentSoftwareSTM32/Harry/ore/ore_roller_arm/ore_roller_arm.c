/*  Club Robot ESEO 2015 - 2016
 *	BIG
 *
 *	Fichier : ORE_ROLLER_ARM.c
 *	Package : Carte actionneur
 *	Description : Gestion de l'actionneur RX24 ORE_ROLLER_ARM
 *  Auteur : Corentin
 *  Version 2017
 *  Robot : BIG
 */

#include "ore_roller_arm.h"


// If def à mettre si l'actionneur est seulement présent sur le petit robot (I_AM_ROBOT_SMALL) ou le gros (I_AM_ROBOT_BIG)
#ifdef I_AM_ROBOT_BIG

// Les différents includes nécessaires...
#include "../QS/QS_CANmsgList.h"
#include "../QS/QS_rx24.h"
#include "../act_queue_utils.h"
#include "../selftest.h"
#include "../ActManager.h"

#include "ore_roller_arm_config.h"

// Les différents define pour le verbose sur uart
#define LOG_PREFIX "ORE_ROLLER_ARM_rx24.c : "
#define LOG_COMPONENT OUTPUT_LOG_COMPONENT_ORE_ROLLER_ARM
#include "../QS/QS_outputlog.h"

// Les fonctions internes au fonctionnement de l'actionneur
static void ORE_ROLLER_ARM_command_run(queue_id_t queueId);
static void ORE_ROLLER_ARM_initRX24();
static void ORE_ROLLER_ARM_command_init(queue_id_t queueId);
static void ORE_ROLLER_ARM_config(CAN_msg_t* msg);
static void ORE_ROLLER_ARM_get_position(QUEUE_act_e act_id, Uint8 command, Uint16 *right_pos, Uint16 *left_pos);

// Booléen contenant l'état actuel de l'initialisation du RX24 (Plusieurs booléens si plusieurs servo dans l'actionneur)
static bool_e rx24_is_initialized_left = FALSE;
static bool_e rx24_is_initialized_right = FALSE;

// Fonction appellée au lancement de la carte (via ActManager)
void ORE_ROLLER_ARM_init() {
	static bool_e initialized = FALSE;

	if(initialized)
		return;
	initialized = TRUE;

	RX24_init();
    ORE_ROLLER_ARM_initRX24();
}

// Fonction appellée si la carte IHM a détecté une grosse chutte de la tension d'alimentation des servos
// Pour éviter les problèmes d'utilisation de servo non initialisé
void ORE_ROLLER_ARM_reset_config(){
	rx24_is_initialized_left = FALSE;
	rx24_is_initialized_right = FALSE;
    ORE_ROLLER_ARM_initRX24();
}

//Initialise le RX24 s'il n'était pas alimenté lors d'initialisations précédentes, si déjà initialisé, ne fait rien
static void ORE_ROLLER_ARM_initRX24() {

    //ORE_ROLLER_ARM_LEFT
    if(rx24_is_initialized_left == FALSE && RX24_is_ready(ORE_ROLLER_ARM_LEFT_RX24_ID) == TRUE) {
		time32_t local_time = global.absolute_time;
		while(global.absolute_time - local_time < 500);

		rx24_is_initialized_left = TRUE;

        RX24_config_set_lowest_voltage(ORE_ROLLER_ARM_LEFT_RX24_ID, RX24_MIN_VOLTAGE);
        RX24_config_set_highest_voltage(ORE_ROLLER_ARM_LEFT_RX24_ID, RX24_MAX_VOLTAGE);
        RX24_set_torque_limit(ORE_ROLLER_ARM_LEFT_RX24_ID, ORE_ROLLER_ARM_RX24_MAX_TORQUE_PERCENT);
        RX24_config_set_temperature_limit(ORE_ROLLER_ARM_LEFT_RX24_ID, ORE_ROLLER_ARM_RX24_MAX_TEMPERATURE);
        RX24_set_move_to_position_speed(ORE_ROLLER_ARM_LEFT_RX24_ID,ORE_ROLLER_ARM_RX24_MAX_SPEED);


        RX24_config_set_maximal_angle(ORE_ROLLER_ARM_LEFT_RX24_ID, ORE_ROLLER_ARM_RX24_MAX_VALUE);
        RX24_config_set_minimal_angle(ORE_ROLLER_ARM_LEFT_RX24_ID, ORE_ROLLER_ARM_RX24_MIN_VALUE);

        RX24_config_set_error_before_led(ORE_ROLLER_ARM_LEFT_RX24_ID, RX24_BEFORE_LED);
        RX24_config_set_error_before_shutdown(ORE_ROLLER_ARM_LEFT_RX24_ID, RX24_BEFORE_SHUTDOWN);

		debug_printf("Init config DONE\n");
	}else if(rx24_is_initialized_left == FALSE)
		debug_printf("Init config FAIL\n");

    //ORE_ROLLER_ARM_RIGHT
    if(rx24_is_initialized_right == FALSE && RX24_is_ready(ORE_ROLLER_ARM_RIGHT_RX24_ID) == TRUE) {
		rx24_is_initialized_right = TRUE;

        RX24_config_set_lowest_voltage(ORE_ROLLER_ARM_RIGHT_RX24_ID, RX24_MIN_VOLTAGE);
        RX24_config_set_highest_voltage(ORE_ROLLER_ARM_RIGHT_RX24_ID, RX24_MAX_VOLTAGE);
        RX24_set_torque_limit(ORE_ROLLER_ARM_RIGHT_RX24_ID, ORE_ROLLER_ARM_RX24_MAX_TORQUE_PERCENT);
        RX24_config_set_temperature_limit(ORE_ROLLER_ARM_RIGHT_RX24_ID, ORE_ROLLER_ARM_RX24_MAX_TEMPERATURE);

        RX24_config_set_maximal_angle(ORE_ROLLER_ARM_RIGHT_RX24_ID, ORE_ROLLER_ARM_RX24_MAX_VALUE);
        RX24_config_set_minimal_angle(ORE_ROLLER_ARM_RIGHT_RX24_ID, ORE_ROLLER_ARM_RX24_MIN_VALUE);

        RX24_config_set_error_before_led(ORE_ROLLER_ARM_RIGHT_RX24_ID, RX24_BEFORE_LED);
        RX24_config_set_error_before_shutdown(ORE_ROLLER_ARM_RIGHT_RX24_ID, RX24_BEFORE_SHUTDOWN);
		debug_printf("Init config DONE\n");
	}else if(rx24_is_initialized_right == FALSE)
		debug_printf("Init config FAIL\n");
}


// Fonction appellée pour la modification des configurations du rx24 telle que la vitesse et le couple (via ActManager)
// Dans le cas de multiple actionneur appartenant à un même actionneur, ajouter des defines dans QS_CANmsgList.h afin de pouvoir les choisirs facilement depuis la stratégie
void ORE_ROLLER_ARM_config(CAN_msg_t* msg){
	switch(msg->data.act_msg.act_data.act_config.sub_act_id){
		case DEFAULT_MONO_ACT : // Premier élement de l'actionneur
            ACTMGR_config_RX24(ORE_ROLLER_ARM_LEFT_RX24_ID, msg);
            ACTMGR_config_RX24(ORE_ROLLER_ARM_RIGHT_RX24_ID, msg);
			break;

		default :
			warn_printf("invalid CAN msg data[1]=%u (sous actionneur inexistant)!\n", msg->data.act_msg.act_data.act_config.sub_act_id);
	}
}

// Fonction appellée pour l'initialisation en position du rx24 dés l'arrivé de l'alimentation (via ActManager)
void ORE_ROLLER_ARM_init_pos(){
    ORE_ROLLER_ARM_initRX24();

	if(rx24_is_initialized_left == TRUE){
		debug_printf("Init pos : \n");
        if(!RX24_set_position(ORE_ROLLER_ARM_LEFT_RX24_ID, ORE_ROLLER_ARM_L_RX24_INIT_POS))
            debug_printf("   Le RX24 n°%d n'est pas là\n", ORE_ROLLER_ARM_LEFT_RX24_ID);
		else
            debug_printf("   Le RX24 n°%d a été initialisé en position\n", ORE_ROLLER_ARM_LEFT_RX24_ID);
	}

	if(rx24_is_initialized_right == TRUE){
		debug_printf("Init pos : \n");
        if(!RX24_set_position(ORE_ROLLER_ARM_RIGHT_RX24_ID, ORE_ROLLER_ARM_R_RX24_INIT_POS))
            debug_printf("   Le RX24 n°%d n'est pas là\n", ORE_ROLLER_ARM_RIGHT_RX24_ID);
		else
            debug_printf("   Le RX24 n°%d a été initialisé en position\n", ORE_ROLLER_ARM_RIGHT_RX24_ID);
	}
}

// Fonction appellée à la fin du match (via ActManager)
void ORE_ROLLER_ARM_stop(){
    RX24_set_torque_enabled(ORE_ROLLER_ARM_LEFT_RX24_ID, FALSE); //Stopper l'asservissement du RX24
    RX24_set_torque_enabled(ORE_ROLLER_ARM_RIGHT_RX24_ID, FALSE); //Stopper l'asservissement du RX24
}

// fonction appellée à la réception d'un message CAN (via ActManager)
bool_e ORE_ROLLER_ARM_CAN_process_msg(CAN_msg_t* msg) {
    if(msg->sid == ACT_ORE_ROLLER_ARM){
        ORE_ROLLER_ARM_initRX24();
		switch(msg->data.act_msg.order) {
			// Listing de toutes les positions de l'actionneur possible
            case ACT_ORE_ROLLER_ARM_IDLE :
            case ACT_ORE_ROLLER_ARM_OUT :
            case ACT_ORE_ROLLER_ARM_UNLOCK :
            case ACT_ORE_ROLLER_ARM_MID :
            case ACT_ORE_ROLLER_ARM_RESCUE :
            case ACT_ORE_ROLLER_ARM_STOP :
            case ACT_ORE_ROLLER_ARM_CHECK :
                ACTQ_push_operation_from_msg(msg, QUEUE_ACT_RX24_ORE_ROLLER_ARM, &ORE_ROLLER_ARM_run_command, 0,TRUE);
				break;

			case ACT_CONFIG :
                ORE_ROLLER_ARM_config(msg);
				break;


			default:
				component_printf(LOG_LEVEL_Warning, "invalid CAN msg data[0]=%u !\n", msg->data.act_msg.order);
		}
		return TRUE;
	}else if(msg->sid == ACT_DO_SELFTEST){
		// Lister les différents états que l'actionneur doit réaliser pour réussir le selftest
        /*SELFTEST_set_actions(&ORE_ROLLER_ARM_run_command, 3, (SELFTEST_action_t[]){
                                 {ACT_ORE_ROLLER_ARM_IDLE,		0,  QUEUE_ACT_RX24_ORE_ROLLER_ARM},
                                 {ACT_ORE_ROLLER_ARM_OPEN,       0,  QUEUE_ACT_RX24_ORE_ROLLER_ARM},
                                 {ACT_ORE_ROLLER_ARM_IDLE,		0,  QUEUE_ACT_RX24_ORE_ROLLER_ARM}
							 });*/
	}
	return FALSE;
}

// Fonction appellée par la queue pendant tout le temps de la commande en cours (le booléen init est à TRUE au premier lancement de la commande)
void ORE_ROLLER_ARM_run_command(queue_id_t queueId, bool_e init) {
	if(QUEUE_has_error(queueId)) {
		QUEUE_behead(queueId);
		return;
	}

    if(QUEUE_get_act(queueId) == QUEUE_ACT_RX24_ORE_ROLLER_ARM) {    // Gestion des mouvements de ORE_ROLLER_ARM
		if(init)
            ORE_ROLLER_ARM_command_init(queueId);
		else
            ORE_ROLLER_ARM_command_run(queueId);
	}
}

//Initialise une commande
static void ORE_ROLLER_ARM_command_init(queue_id_t queueId) {
	Uint8 command = QUEUE_get_arg(queueId)->canCommand;
	Uint16 rx24_goalPosition_left = 0xFFFF, rx24_goalPosition_right = 0xFFFF;
	bool_e result = TRUE;

    ORE_ROLLER_ARM_initRX24();

	switch(command) {
		// Listing de toutes les positions de l'actionneur possible avec les valeurs de position associées
        case ACT_ORE_ROLLER_ARM_IDLE :
        case ACT_ORE_ROLLER_ARM_OUT :
        case ACT_ORE_ROLLER_ARM_UNLOCK :
        case ACT_ORE_ROLLER_ARM_RESCUE:
        case ACT_ORE_ROLLER_ARM_MID :
        case ACT_ORE_ROLLER_ARM_CHECK :
            ORE_ROLLER_ARM_get_position(QUEUE_get_act(queueId), QUEUE_get_arg(queueId)->canCommand, &rx24_goalPosition_right, &rx24_goalPosition_left);
			break;

        case ACT_ORE_ROLLER_ARM_STOP :
            RX24_set_torque_enabled(ORE_ROLLER_ARM_LEFT_RX24_ID, FALSE); //Stopper l'asservissement du RX24
            RX24_set_torque_enabled(ORE_ROLLER_ARM_RIGHT_RX24_ID, FALSE); //Stopper l'asservissement du RX24
            QUEUE_next(queueId, ACT_ORE_ROLLER_ARM, ACT_RESULT_DONE, ACT_RESULT_ERROR_OK, __LINE__);
			return;

		default: {
            error_printf("Invalid ORE_ROLLER_ARM command: %u, code is broken !\n", command);
            QUEUE_next(queueId, ACT_ORE_ROLLER_ARM, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_LOGIC, __LINE__);
			return;
		}
	}

	if(rx24_is_initialized_left == FALSE || rx24_is_initialized_right == FALSE ){
		error_printf("Impossible de mettre l'actionneur en position il n'est pas initialisé\n");
        QUEUE_next(queueId, ACT_ORE_ROLLER_ARM, ACT_RESULT_FAILED, ACT_RESULT_ERROR_NOT_HERE, __LINE__);
		return;
	}

	if(rx24_goalPosition_left == 0xFFFF || rx24_goalPosition_left == 0xFFFF) {
		error_printf("Invalid rx24 position for command: %u, code is broken !\n", command);
        QUEUE_next(queueId, ACT_ORE_ROLLER_ARM, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_LOGIC, __LINE__);
		return;
	}

    RX24_reset_last_error(ORE_ROLLER_ARM_LEFT_RX24_ID); //Sécurité anti terroriste. Nous les parano on aime pas voir des erreurs là ou il n'y en a pas.
    RX24_reset_last_error(ORE_ROLLER_ARM_RIGHT_RX24_ID); //Sécurité anti terroriste. Nous les parano on aime pas voir des erreurs là ou il n'y en a pas.

	debug_printf("result = %d\n", result);

    if(!RX24_set_position(ORE_ROLLER_ARM_LEFT_RX24_ID, rx24_goalPosition_left)) {	//Si la commande n'a pas été envoyée correctement et/ou que le RX24 ne répond pas a cet envoi, on l'indique à la carte stratégie
        error_printf("RX24_set_position error: 0x%x\n", RX24_get_last_error(ORE_ROLLER_ARM_LEFT_RX24_ID).error);
		result = FALSE;
	}else{
        debug_printf("Move ORE_ROLLER_ARM rx24(%d) to %d\n", ORE_ROLLER_ARM_LEFT_RX24_ID, rx24_goalPosition_left);
	}

    if(!RX24_set_position(ORE_ROLLER_ARM_RIGHT_RX24_ID, rx24_goalPosition_right)) {	//Si la commande n'a pas été envoyée correctement et/ou que le RX24 ne répond pas a cet envoi, on l'indique à la carte stratégie
        error_printf("RX24_set_position error: 0x%x\n", RX24_get_last_error(ORE_ROLLER_ARM_LEFT_RX24_ID).error);
		result = FALSE;
	}else{
        debug_printf("Move ORE_ROLLER_ARM rx24(%d) to %d\n", ORE_ROLLER_ARM_LEFT_RX24_ID, rx24_goalPosition_right);
	}

	if(result == FALSE){
        debug_printf("ORE_ROLLER_ARM rx24 failed : NotHere\n");
        QUEUE_next(queueId, ACT_ORE_ROLLER_ARM, ACT_RESULT_FAILED, ACT_RESULT_ERROR_NOT_HERE, __LINE__);
		return;
	}

}

//Gère les états pendant le mouvement du RX24
static void ORE_ROLLER_ARM_command_run(queue_id_t queueId) {
	Uint8 result_left, result_right, errorCode_left, errorCode_right;
	Uint16 line_left, line_right;
	Uint16 rx24_goalPosition_left = 0xFFFF, rx24_goalPosition_right = 0xFFFF;
	static bool_e done_left=FALSE, done_right=FALSE;

    ORE_ROLLER_ARM_get_position(QUEUE_get_act(queueId), QUEUE_get_arg(queueId)->canCommand, &rx24_goalPosition_right, &rx24_goalPosition_left);

	if(done_right == FALSE){
        done_right = ACTQ_check_status_rx24(queueId, ORE_ROLLER_ARM_RIGHT_RX24_ID, rx24_goalPosition_right, ORE_ROLLER_ARM_RX24_ASSER_POS_EPSILON, ORE_ROLLER_ARM_RX24_ASSER_TIMEOUT, ORE_ROLLER_ARM_RX24_ASSER_POS_LARGE_EPSILON, &result_right, &errorCode_right, &line_right);
	}

	if(done_left == FALSE){
        done_left = ACTQ_check_status_rx24(queueId, ORE_ROLLER_ARM_LEFT_RX24_ID, rx24_goalPosition_left, ORE_ROLLER_ARM_RX24_ASSER_POS_EPSILON, ORE_ROLLER_ARM_RX24_ASSER_TIMEOUT, ORE_ROLLER_ARM_RX24_ASSER_POS_LARGE_EPSILON, &result_left, &errorCode_left, &line_left);
	}

	if(done_right && done_left){
		done_right = FALSE;
		done_left = FALSE;
		if(result_right == ACT_RESULT_DONE && result_left == ACT_RESULT_DONE){
            QUEUE_next(queueId, ACT_ORE_ROLLER_ARM, ACT_RESULT_DONE, ACT_RESULT_ERROR_OK, 0x0100);
		}else if(result_right != ACT_RESULT_DONE){
            QUEUE_next(queueId, ACT_ORE_ROLLER_ARM, result_right, errorCode_right, line_right);
		}else {
            QUEUE_next(queueId, ACT_ORE_ROLLER_ARM, result_left, errorCode_left, line_left);
		}
	}
}

static void ORE_ROLLER_ARM_get_position(QUEUE_act_e act_id, Uint8 command, Uint16 *right_pos, Uint16 *left_pos){
    if(act_id == QUEUE_ACT_RX24_ORE_ROLLER_ARM){
		switch(command){
            case ACT_ORE_ROLLER_ARM_IDLE :
                *right_pos = ORE_ROLLER_ARM_R_RX24_IDLE_POS;
                *left_pos = ORE_ROLLER_ARM_L_RX24_IDLE_POS;
				break;

            case ACT_ORE_ROLLER_ARM_OUT :
                *right_pos = ORE_ROLLER_ARM_R_RX24_OUT_POS;
                *left_pos = ORE_ROLLER_ARM_L_RX24_OUT_POS;
				break;

            case ACT_ORE_ROLLER_ARM_UNLOCK :
                *right_pos = ORE_ROLLER_ARM_R_RX24_UNLOCK_POS;
                *left_pos = ORE_ROLLER_ARM_L_RX24_UNLOCK_POS;
				break;

            case ACT_ORE_ROLLER_ARM_MID :
                *right_pos = ORE_ROLLER_ARM_R_RX24_MID_POS;
                *left_pos = ORE_ROLLER_ARM_L_RX24_MID_POS;
				break;

            case ACT_ORE_ROLLER_ARM_RESCUE :
                *right_pos = ORE_ROLLER_ARM_R_RX24_RESCUE_POS;
                *left_pos = ORE_ROLLER_ARM_L_RX24_RESCUE_POS;
				break;

            case ACT_ORE_ROLLER_ARM_CHECK :
                *right_pos = ORE_ROLLER_ARM_R_RX24_CHECK_POS;
                *left_pos = ORE_ROLLER_ARM_L_RX24_CHECK_POS;
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
