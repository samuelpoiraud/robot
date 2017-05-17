/*  Club Robot ESEO 2016 - 2017
 *
 *	Fichier : BIG_BALL_FRONT_LEFT_ax12.c
 *	Package : Carte actionneur
 *	Description : Gestion de l'actionneur AX12 BIG_BALL_FRONT_LEFT
 *  Auteur :
 *  Version 2017
 *  Robot : BIG
 */

#include "big_ball_front_left.h"


// BIG_BALL_FRONT_LEFT d'un actionneur standart avec AX12

// 1) Ajout l'actionneur dans QS_CANmsgList.h (tout à la fin du fichier)
// 2) Ajout d'une valeur dans l'énumération de la queue dans config_(big/small)/config_global_vars_types.h
//		Formatage : QUEUE_ACT_AX12_BIG_BALL_FRONT_LEFT
// 3) Ajout de la déclaration de l'actionneur dans ActManager dans le tableau actionneurs
// 4) Ajout de la verbosité dans le fichier act_queue_utils.c dans la fonction ACTQ_internal_printResult
// 5) Ajout du pilotage via terminal dans le fichier term_io.c dans le tableau terminal_motor du type : ACT_DECLARE(BIG_BALL_FRONT_LEFT)
// 6) Un #define BIG_BALL_FRONT_LEFT_AX12_ID doit avoir été ajouté au fichier config_big/config_pin.h ou config_small/config_pin.h
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
#include "../QS/QS_can.h"
#include "../act_queue_utils.h"
#include "../selftest.h"
#include "../ActManager.h"

#include "big_ball_front_left_config.h"

// Les différents define pour le verbose sur uart
#define LOG_PREFIX "big_ball_front_left.c : "
#define LOG_COMPONENT OUTPUT_LOG_COMPONENT_BIG_BALL_FRONT_LEFT
#include "../QS/QS_outputlog.h"

// Les fonctions internes au fonctionnement de l'actionneur
static void BIG_BALL_FRONT_LEFT_command_run(queue_id_t queueId);
static void BIG_BALL_FRONT_LEFT_initAX12();
static void BIG_BALL_FRONT_LEFT_command_init(queue_id_t queueId);
static void BIG_BALL_FRONT_LEFT_set_config(CAN_msg_t* msg);
static void BIG_BALL_FRONT_LEFT_get_position_config(ACT_order_e *pOrder, Uint16 *pPos);
static void BIG_BALL_FRONT_LEFT_get_config(CAN_msg_t *incoming_msg);
static void BIG_BALL_FRONT_LEFT_set_warner(CAN_msg_t *msg);
static void BIG_BALL_FRONT_LEFT_check_warner(Uint16 pos);

// Booléen contenant l'état actuel de l'initialisation de l'AX12 (Plusieurs booléens si plusieurs servo dans l'actionneur)
static bool_e ax12_is_initialized = FALSE;

// Warner de l'actionneur : Déclenche l'envoi d'un message CAN lorsqu'une certaine position est franchi
static act_warner_s warner;

// Flag qui permet d'annuler l'ordre en cours dans le cas où l'on vient de recevoir un ordre "run_now"
static bool_e run_now = FALSE;

// Fonction appellée au lancement de la carte (via ActManager)
void BIG_BALL_FRONT_LEFT_init() {
	static bool_e initialized = FALSE;

	if(initialized)
		return;
	initialized = TRUE;

	AX12_init();
    BIG_BALL_FRONT_LEFT_initAX12();
}

// Fonction appellée si la carte IHM a détecté une grosse chutte de la tension d'alimentation des servos
// Pour éviter les problèmes d'utilisation de servo non initialisé
void BIG_BALL_FRONT_LEFT_reset_config(){
	ax12_is_initialized = FALSE;
    BIG_BALL_FRONT_LEFT_initAX12();
}

//Initialise l'AX12 s'il n'était pas alimenté lors d'initialisations précédentes, si déjà initialisé, ne fait rien
static void BIG_BALL_FRONT_LEFT_initAX12() {
    if(ax12_is_initialized == FALSE && AX12_is_ready(BIG_BALL_FRONT_LEFT_AX12_ID) == TRUE) {
		ax12_is_initialized = TRUE;
        AX12_config_set_lowest_voltage(BIG_BALL_FRONT_LEFT_AX12_ID, AX12_MIN_VOLTAGE);
        AX12_config_set_highest_voltage(BIG_BALL_FRONT_LEFT_AX12_ID, AX12_MAX_VOLTAGE);
        AX12_set_torque_limit(BIG_BALL_FRONT_LEFT_AX12_ID, BIG_BALL_FRONT_LEFT_AX12_MAX_TORQUE_PERCENT);
        AX12_config_set_temperature_limit(BIG_BALL_FRONT_LEFT_AX12_ID, BIG_BALL_FRONT_LEFT_AX12_MAX_TEMPERATURE);

        AX12_config_set_maximal_angle(BIG_BALL_FRONT_LEFT_AX12_ID, BIG_BALL_FRONT_LEFT_AX12_MAX_VALUE);
        AX12_config_set_minimal_angle(BIG_BALL_FRONT_LEFT_AX12_ID, BIG_BALL_FRONT_LEFT_AX12_MIN_VALUE);
        AX12_set_move_to_position_speed(BIG_BALL_FRONT_LEFT_AX12_ID, BIG_BALL_FRONT_LEFT_AX12_SPEED);

        AX12_config_set_error_before_led(BIG_BALL_FRONT_LEFT_AX12_ID, AX12_BEFORE_LED);
        AX12_config_set_error_before_shutdown(BIG_BALL_FRONT_LEFT_AX12_ID, AX12_BEFORE_SHUTDOWN);
		debug_printf("Init config DONE\n");
	}else if(ax12_is_initialized == FALSE)
		debug_printf("Init config FAIL\n");
}


// Fonction appellée pour la modification des configurations de l'ax12 telle que la vitesse et le couple (via ActManager)
// Dans le cas de multiple actionneur appartenant à un même actionneur, ajouter des defines dans QS_CANmsgList.h afin de pouvoir les choisirs facilement depuis la stratégie
void BIG_BALL_FRONT_LEFT_set_config(CAN_msg_t* msg){
	switch(msg->data.act_msg.act_data.act_config.sub_act_id){
		case DEFAULT_MONO_ACT : // Premier élement de l'actionneur
            ACTMGR_config_AX12(BIG_BALL_FRONT_LEFT_AX12_ID, msg);
			break;

		default :
			warn_printf("invalid CAN msg data[1]=%u (sous actionneur inexistant)!\n", msg->data.act_msg.act_data.act_config.sub_act_id);
	}
}

static void BIG_BALL_FRONT_LEFT_get_config(CAN_msg_t *incoming_msg){
	bool_e error = FALSE;
	Uint16 pos = 0;;
	ACT_order_e order = 0;
	CAN_msg_t msg;
	msg.sid = ACT_GET_CONFIG_ANSWER;
	msg.size = SIZE_ACT_GET_CONFIG_ANSWER;
	msg.data.act_get_config_answer.sid = 0xFF & ACT_BIG_BALL_FRONT_LEFT;
	msg.data.act_get_config_answer.config = incoming_msg->data.act_msg.act_data.config;

	switch(incoming_msg->data.act_msg.act_data.config){
		case POSITION_CONFIG:
			BIG_BALL_FRONT_LEFT_get_position_config(&order, &pos);
			msg.data.act_get_config_answer.act_get_config_data.act_get_config_pos_answer.order = order;
			msg.data.act_get_config_answer.act_get_config_data.act_get_config_pos_answer.pos = pos;
			break;
		case TORQUE_CONFIG:
			msg.data.act_get_config_answer.act_get_config_data.torque = AX12_get_speed_percentage(BIG_BALL_FRONT_LEFT_AX12_ID);
			break;
		case TEMPERATURE_CONFIG:
			msg.data.act_get_config_answer.act_get_config_data.temperature = AX12_get_temperature(BIG_BALL_FRONT_LEFT_AX12_ID);
			break;
		case LOAD_CONFIG:
			msg.data.act_get_config_answer.act_get_config_data.load = AX12_get_load_percentage(BIG_BALL_FRONT_LEFT_AX12_ID);
			break;
		default:
			error = TRUE;
			warn_printf("This config is not available \n");
	}

	if(!error){
		CAN_send(&msg); // Envoi du message CAN
	}
}

static void BIG_BALL_FRONT_LEFT_get_position_config(ACT_order_e *pOrder, Uint16 *pPos){
	ACT_order_e order = ACT_BIG_BALL_FRONT_LEFT_STOP;
	Uint16 position = AX12_get_position(BIG_BALL_FRONT_LEFT_AX12_ID);
	Uint16 epsilon = BIG_BALL_FRONT_LEFT_AX12_ASSER_POS_EPSILON;

	if(position > BIG_BALL_FRONT_LEFT_AX12_UP_POS - epsilon && position < BIG_BALL_FRONT_LEFT_AX12_UP_POS + epsilon){
		order = ACT_BIG_BALL_FRONT_LEFT_UP;
	}else if(position > BIG_BALL_FRONT_LEFT_AX12_DOWN_POS - epsilon && position < BIG_BALL_FRONT_LEFT_AX12_DOWN_POS + epsilon){
		order = ACT_BIG_BALL_FRONT_LEFT_DOWN;
	}else if(position > BIG_BALL_FRONT_LEFT_AX12_IDLE_POS - epsilon && position < BIG_BALL_FRONT_LEFT_AX12_IDLE_POS + epsilon){
		order = ACT_BIG_BALL_FRONT_LEFT_IDLE;
	}

	if(pOrder != NULL)
		*pOrder = order;

	if(pPos != NULL)
		*pPos = position;
}

// Fonction appellée pour l'initialisation en position de l'AX12 dés l'arrivé de l'alimentation (via ActManager)
void BIG_BALL_FRONT_LEFT_init_pos(){
    BIG_BALL_FRONT_LEFT_initAX12();

	if(ax12_is_initialized == FALSE)
		return;

	debug_printf("Init pos : \n");
    if(!AX12_set_position(BIG_BALL_FRONT_LEFT_AX12_ID, BIG_BALL_FRONT_LEFT_AX12_INIT_POS))
        debug_printf("   L'AX12 n°%d n'est pas là\n", BIG_BALL_FRONT_LEFT_AX12_ID);
	else
        debug_printf("   L'AX12 n°%d a été initialisé en position\n", BIG_BALL_FRONT_LEFT_AX12_ID);
}

// Fonction appellée à la fin du match (via ActManager)
void BIG_BALL_FRONT_LEFT_stop(){
    AX12_set_torque_enabled(BIG_BALL_FRONT_LEFT_AX12_ID, FALSE); //Stopper l'asservissement de l'AX12
}

// fonction appellée à la réception d'un message CAN (via ActManager)
bool_e BIG_BALL_FRONT_LEFT_CAN_process_msg(CAN_msg_t* msg) {
    if(msg->sid == ACT_BIG_BALL_FRONT_LEFT){
        BIG_BALL_FRONT_LEFT_initAX12();
		switch(msg->data.act_msg.order) {
			// Listing de toutes les positions de l'actionneur possible
            case ACT_BIG_BALL_FRONT_LEFT_IDLE :
            case ACT_BIG_BALL_FRONT_LEFT_UP :
            case ACT_BIG_BALL_FRONT_LEFT_DOWN :
            case ACT_BIG_BALL_FRONT_LEFT_STOP :
            	run_now = msg->data.act_msg.act_data.act_order.run_now;
                ACTQ_push_operation_from_msg(msg, QUEUE_ACT_AX12_BIG_BALL_FRONT_LEFT, &BIG_BALL_FRONT_LEFT_run_command, 0,TRUE);
				break;

            case ACT_WARNER:
				BIG_BALL_FRONT_LEFT_set_warner(msg);
				break;

			case ACT_GET_CONFIG:
				BIG_BALL_FRONT_LEFT_get_config(msg);
				break;

			case ACT_SET_CONFIG :
                BIG_BALL_FRONT_LEFT_set_config(msg);
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
void BIG_BALL_FRONT_LEFT_run_command(queue_id_t queueId, bool_e init) {
	if(QUEUE_has_error(queueId)) {
		QUEUE_behead(queueId);
		return;
	}

    if(QUEUE_get_act(queueId) == QUEUE_ACT_AX12_BIG_BALL_FRONT_LEFT) {    // Gestion des mouvements de BIG_BALL_FRONT_LEFT
		if(init)
            BIG_BALL_FRONT_LEFT_command_init(queueId);
		else
            BIG_BALL_FRONT_LEFT_command_run(queueId);
	}
}

//Initialise une commande
static void BIG_BALL_FRONT_LEFT_command_init(queue_id_t queueId) {
	Uint8 command = QUEUE_get_arg(queueId)->canCommand;
	Sint16* ax12_goalPosition = &QUEUE_get_arg(queueId)->param;

	*ax12_goalPosition = 0xFFFF;
    BIG_BALL_FRONT_LEFT_initAX12();

	switch(command) {
		// Listing de toutes les positions de l'actionneur possible avec les valeurs de position associées
        case ACT_BIG_BALL_FRONT_LEFT_IDLE : *ax12_goalPosition = BIG_BALL_FRONT_LEFT_AX12_IDLE_POS; break;
        case ACT_BIG_BALL_FRONT_LEFT_UP : *ax12_goalPosition = BIG_BALL_FRONT_LEFT_AX12_UP_POS; break;
        case ACT_BIG_BALL_FRONT_LEFT_DOWN : *ax12_goalPosition = BIG_BALL_FRONT_LEFT_AX12_DOWN_POS; break;

        case ACT_BIG_BALL_FRONT_LEFT_STOP :
            AX12_set_torque_enabled(BIG_BALL_FRONT_LEFT_AX12_ID, FALSE); //Stopper l'asservissement de l'AX12
            QUEUE_next(queueId, ACT_BIG_BALL_FRONT_LEFT, ACT_RESULT_DONE, ACT_RESULT_ERROR_OK, __LINE__);
			return;

		default: {
            error_printf("Invalid BIG_BALL_FRONT_LEFT command: %u, code is broken !\n", command);
            QUEUE_next(queueId, ACT_BIG_BALL_FRONT_LEFT, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_LOGIC, __LINE__);
			return;
		}
	}

	if(ax12_is_initialized == FALSE){
		error_printf("Impossible de mettre l'actionneur en position il n'est pas initialisé\n");
        QUEUE_next(queueId, ACT_BIG_BALL_FRONT_LEFT, ACT_RESULT_FAILED, ACT_RESULT_ERROR_NOT_HERE, __LINE__);
		return;
	}

	if(*ax12_goalPosition == 0xFFFF) {
		error_printf("Invalid ax12 position for command: %u, code is broken !\n", command);
        QUEUE_next(queueId, ACT_BIG_BALL_FRONT_LEFT, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_LOGIC, __LINE__);
		return;
	}

    AX12_reset_last_error(BIG_BALL_FRONT_LEFT_AX12_ID); //Sécurité anti terroriste. Nous les parano on aime pas voir des erreurs là ou il n'y en a pas.
    if(!AX12_set_position(BIG_BALL_FRONT_LEFT_AX12_ID, *ax12_goalPosition)) {	//Si la commande n'a pas été envoyée correctement et/ou que l'AX12 ne répond pas a cet envoi, on l'indique à la carte stratégie
        error_printf("AX12_set_position error: 0x%x\n", AX12_get_last_error(BIG_BALL_FRONT_LEFT_AX12_ID).error);
        QUEUE_next(queueId, ACT_BIG_BALL_FRONT_LEFT, ACT_RESULT_FAILED, ACT_RESULT_ERROR_NOT_HERE, __LINE__);
		return;
	}
	//La commande a été envoyée et l'AX12 l'a bien reçu
    debug_printf("Move BIG_BALL_FRONT_LEFT ax12 to %d\n", *ax12_goalPosition);
}

//Gère les états pendant le mouvement de l'AX12
static void BIG_BALL_FRONT_LEFT_command_run(queue_id_t queueId) {
	Uint8 result, errorCode;
	Uint16 line;

	Uint16 pos = AX12_get_position(BIG_BALL_FRONT_LEFT_AX12_ID);

	if(!run_now){
		if(ACTQ_check_status_ax12(queueId, BIG_BALL_FRONT_LEFT_AX12_ID, QUEUE_get_arg(queueId)->param, pos, BIG_BALL_FRONT_LEFT_AX12_ASSER_POS_EPSILON, BIG_BALL_FRONT_LEFT_AX12_ASSER_TIMEOUT, BIG_BALL_FRONT_LEFT_AX12_ASSER_POS_LARGE_EPSILON, &result, &errorCode, &line))
			QUEUE_next(queueId, ACT_BIG_BALL_FRONT_LEFT, result, errorCode, line);
	}else{
		// Lorsqu'on vient de recevoir un run_now, on passe directement à l'ordre reçu.
		ACTQ_flush_queue_to_run_now(queueId, ACT_BIG_BALL_FRONT_LEFT);
		run_now = FALSE;
	}

    // On ne surveille le warner que si il est activé
	if(warner.activated)
		BIG_BALL_FRONT_LEFT_check_warner(pos);
}

static void BIG_BALL_FRONT_LEFT_set_warner(CAN_msg_t *msg){
	warner.activated = TRUE;
	warner.last_pos = AX12_get_position(BIG_BALL_FRONT_LEFT_AX12_ID);

	switch(msg->data.act_msg.act_data.warner_pos){
		/*case ACT_BIG_BALL_FRONT_LEFT_WARNER:
			warner.warner_pos = BIG_BALL_FRONT_LEFT_AX12_WARNER_POS;
			break;*/
		default:{
			warner.activated = FALSE;
			warn_printf("This position is not available for setting a warner\n");
		}
	}
}

static void BIG_BALL_FRONT_LEFT_check_warner(Uint16 pos){
	CAN_msg_t msg;

	if( (warner.last_pos < pos && warner.last_pos < warner.warner_pos && pos > warner.warner_pos)
     || (warner.last_pos > pos && warner.last_pos > warner.warner_pos && pos < warner.warner_pos)
	 || (warner.last_pos > AX12_MAX_WARNER && pos < AX12_MIN_WARNER	&& (warner.last_pos < warner.warner_pos || pos > warner.warner_pos))
	 || (warner.last_pos < AX12_MIN_WARNER && pos > AX12_MAX_WARNER	&& (warner.last_pos > warner.warner_pos || pos < warner.warner_pos))
	){
		// Envoi du message CAN pour prévenir la strat
		msg.sid = ACT_WARNER_ANSWER;
		msg.size = SIZE_ACT_WARNER_ANSWER;
		msg.data.act_warner_answer.sid = 0xFF & ACT_BIG_BALL_FRONT_LEFT;
		CAN_send(&msg);

		// Désactivation du warner
		warner.activated = FALSE;
	}

	warner.last_pos = pos;
}
#endif
