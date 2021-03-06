/*  Club Robot ESEO 2016 - 2017
*
*	Fichier : small_cylinder_elevator.c
*	Package : Carte actionneur
*	Description : Gestion de l'actionneur RX24 SMALL_CYLINDER_ELEVATOR
*	Auteur :
*	Version 2017
*	Robot : BIG
*/

#include "small_cylinder_elevator.h"


// SMALL_CYLINDER_ELEVATOR d'un actionneur standart avec RX24

// 1) Ajout l'actionneur dans QS_CANmsgList.h (tout � la fin du fichier)
// 2) Ajout d'une valeur dans l'�num�ration de la queue dans config_(big/small)/config_global_vars_types.h
//		Formatage : QUEUE_ACT_RX24_SMALL_CYLINDER_ELEVATOR
// 3) Ajout de la d�claration de l'actionneur dans ActManager dans le tableau actionneurs
// 4) Ajout de la verbosit� dans le fichier act_queue_utils.c dans la fonction ACTQ_internal_printResult
// 5) Ajout du pilotage via terminal dans le fichier term_io.c dans le tableau terminal_motor du type : ACT_DECLARE(SMALL_CYLINDER_ELEVATOR)
// 6) Un #define SMALL_CYLINDER_ELEVATOR_RX24_ID doit avoir �t� ajout� au fichier config_big/config_pin.h ou config_small/config_pin.h
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
#include "../QS/QS_rx24.h"
#include "../QS/QS_can.h"
#include "../act_queue_utils.h"
#include "../selftest.h"
#include "../ActManager.h"

#include "small_cylinder_elevator_config.h"

// Les diff�rents define pour le verbose sur uart
#define LOG_PREFIX "small_cylinder_elevator.c : "
#define LOG_COMPONENT OUTPUT_LOG_COMPONENT_SMALL_CYLINDER_ELEVATOR
#include "../QS/QS_outputlog.h"

// Les fonctions internes au fonctionnement de l'actionneur
static void SMALL_CYLINDER_ELEVATOR_command_run(queue_id_t queueId);
static void SMALL_CYLINDER_ELEVATOR_initRX24();
static void SMALL_CYLINDER_ELEVATOR_command_init(queue_id_t queueId);
static void SMALL_CYLINDER_ELEVATOR_set_config(CAN_msg_t* msg);
static void SMALL_CYLINDER_ELEVATOR_get_position_config(ACT_order_e *pOrder, Uint16 *pPos);
static void SMALL_CYLINDER_ELEVATOR_get_config(CAN_msg_t *incoming_msg);
static void SMALL_CYLINDER_ELEVATOR_set_warner(CAN_msg_t *msg);
static void SMALL_CYLINDER_ELEVATOR_check_warner(Uint16 pos);

// Bool�en contenant l'�tat actuel de l'initialisation du RX24 (Plusieurs bool�ens si plusieurs servo dans l'actionneur)
static bool_e rx24_is_initialized = FALSE;

// Warner de l'actionneur : D�clenche l'envoi d'un message CAN lorsqu'une certaine position est franchi
static act_warner_s warner;

// Flag qui permet d'annuler l'ordre en cours dans le cas o� l'on vient de recevoir un ordre "run_now"
static bool_e run_now = FALSE;

// Fonction appell�e au lancement de la carte (via ActManager)
void SMALL_CYLINDER_ELEVATOR_init() {
	static bool_e initialized = FALSE;

	if(initialized)
		return;
	initialized = TRUE;

	RX24_init();
    SMALL_CYLINDER_ELEVATOR_initRX24();
}

// Fonction appell�e si la carte IHM a d�tect� une grosse chutte de la tension d'alimentation des servos
// Pour �viter les probl�mes d'utilisation de servo non initialis�
void SMALL_CYLINDER_ELEVATOR_reset_config(){
	rx24_is_initialized = FALSE;
    SMALL_CYLINDER_ELEVATOR_initRX24();
}

//Initialise le RX24 s'il n'�tait pas aliment� lors d'initialisations pr�c�dentes, si d�j� initialis�, ne fait rien
static void SMALL_CYLINDER_ELEVATOR_initRX24() {
    if(rx24_is_initialized == FALSE && RX24_is_ready(SMALL_CYLINDER_ELEVATOR_RX24_ID) == TRUE) {
		rx24_is_initialized = TRUE;
        RX24_config_set_lowest_voltage(SMALL_CYLINDER_ELEVATOR_RX24_ID, RX24_MIN_VOLTAGE);
        RX24_config_set_highest_voltage(SMALL_CYLINDER_ELEVATOR_RX24_ID, RX24_MAX_VOLTAGE);
        RX24_set_torque_limit(SMALL_CYLINDER_ELEVATOR_RX24_ID, SMALL_CYLINDER_ELEVATOR_RX24_MAX_TORQUE_PERCENT);
        RX24_config_set_temperature_limit(SMALL_CYLINDER_ELEVATOR_RX24_ID, SMALL_CYLINDER_ELEVATOR_RX24_MAX_TEMPERATURE);

        RX24_config_set_maximal_angle(SMALL_CYLINDER_ELEVATOR_RX24_ID, SMALL_CYLINDER_ELEVATOR_RX24_MAX_VALUE);
        RX24_config_set_minimal_angle(SMALL_CYLINDER_ELEVATOR_RX24_ID, SMALL_CYLINDER_ELEVATOR_RX24_MIN_VALUE);
        RX24_set_move_to_position_speed(SMALL_CYLINDER_ELEVATOR_RX24_ID, SMALL_CYLINDER_ELEVATOR_RX24_SPEED);

        RX24_config_set_error_before_led(SMALL_CYLINDER_ELEVATOR_RX24_ID, RX24_BEFORE_LED);
        RX24_config_set_error_before_shutdown(SMALL_CYLINDER_ELEVATOR_RX24_ID, RX24_BEFORE_SHUTDOWN);
		debug_printf("Init config DONE\n");
	}else if(rx24_is_initialized == FALSE)
		debug_printf("Init config FAIL\n");
}


// Fonction appell�e pour la modification des configurations du rx24 telle que la vitesse et le couple (via ActManager)
// Dans le cas de multiple actionneur appartenant � un m�me actionneur, ajouter des defines dans QS_CANmsgList.h afin de pouvoir les choisirs facilement depuis la strat�gie
void SMALL_CYLINDER_ELEVATOR_set_config(CAN_msg_t* msg){
	switch(msg->data.act_msg.act_data.act_config.sub_act_id){
		case DEFAULT_MONO_ACT : // Premier �lement de l'actionneur
            ACTMGR_config_RX24(SMALL_CYLINDER_ELEVATOR_RX24_ID, msg);
			break;

		default :
			warn_printf("invalid CAN msg data[1]=%u (sous actionneur inexistant)!\n", msg->data.act_msg.act_data.act_config.sub_act_id);
	}
}

static void SMALL_CYLINDER_ELEVATOR_get_config(CAN_msg_t *incoming_msg){
	bool_e error = FALSE;
	Uint16 pos = 0;;
	ACT_order_e order = 0;
	CAN_msg_t msg;
	msg.sid = ACT_GET_CONFIG_ANSWER;
	msg.size = SIZE_ACT_GET_CONFIG_ANSWER;
	msg.data.act_get_config_answer.sid = 0xFF & ACT_SMALL_CYLINDER_ELEVATOR;
	msg.data.act_get_config_answer.config = incoming_msg->data.act_msg.act_data.config;

	switch(incoming_msg->data.act_msg.act_data.config){
		case POSITION_CONFIG:
			SMALL_CYLINDER_ELEVATOR_get_position_config(&order, &pos);
			msg.data.act_get_config_answer.act_get_config_data.act_get_config_pos_answer.order = order;
			msg.data.act_get_config_answer.act_get_config_data.act_get_config_pos_answer.pos = pos;
			break;
		case TORQUE_CONFIG:
			msg.data.act_get_config_answer.act_get_config_data.torque = RX24_get_speed_percentage(SMALL_CYLINDER_ELEVATOR_RX24_ID);
			break;
		case TEMPERATURE_CONFIG:
			msg.data.act_get_config_answer.act_get_config_data.temperature = RX24_get_temperature(SMALL_CYLINDER_ELEVATOR_RX24_ID);
			break;
		case LOAD_CONFIG:
			msg.data.act_get_config_answer.act_get_config_data.load = RX24_get_load_percentage(SMALL_CYLINDER_ELEVATOR_RX24_ID);
			break;
		default:
			error = TRUE;
			warn_printf("This config is not available \n");
	}

	if(!error){
		CAN_send(&msg); // Envoi du message CAN
	}
}

static void SMALL_CYLINDER_ELEVATOR_get_position_config(ACT_order_e *pOrder, Uint16 *pPos){
	ACT_order_e order = ACT_SMALL_CYLINDER_ELEVATOR_STOP;
	Uint16 position = RX24_get_position(SMALL_CYLINDER_ELEVATOR_RX24_ID);
	Uint16 epsilon = SMALL_CYLINDER_ELEVATOR_RX24_ASSER_POS_EPSILON;

	if(position > SMALL_CYLINDER_ELEVATOR_RX24_TOP_POS - epsilon && position < SMALL_CYLINDER_ELEVATOR_RX24_TOP_POS + epsilon){
		order = ACT_SMALL_CYLINDER_ELEVATOR_TOP;
	}else if(position > SMALL_CYLINDER_ELEVATOR_RX24_WAIT_FOR_SLOPE - epsilon && position < SMALL_CYLINDER_ELEVATOR_RX24_WAIT_FOR_SLOPE + epsilon){
		order = ACT_SMALL_CYLINDER_ELEVATOR_WAIT_FOR_SLOPE;
	}else if(position > SMALL_CYLINDER_ELEVATOR_RX24_BOTTOM_POS - epsilon && position < SMALL_CYLINDER_ELEVATOR_RX24_BOTTOM_POS + epsilon){
		order = ACT_SMALL_CYLINDER_ELEVATOR_BOTTOM;
	}else if(position > SMALL_CYLINDER_ELEVATOR_RX24_LOCK_WITH_CYLINDER_POS - epsilon && position < SMALL_CYLINDER_ELEVATOR_RX24_LOCK_WITH_CYLINDER_POS + epsilon){
		order = ACT_SMALL_CYLINDER_ELEVATOR_LOCK_WITH_CYLINDER;
	}else if(position > SMALL_CYLINDER_ELEVATOR_RX24_IDLE_POS - epsilon && position < SMALL_CYLINDER_ELEVATOR_RX24_IDLE_POS + epsilon){
		order = ACT_SMALL_CYLINDER_ELEVATOR_IDLE;
	}

	if(pOrder != NULL)
		*pOrder = order;

	if(pPos != NULL)
		*pPos = position;
}

// Fonction appell�e pour l'initialisation en position du rx24 d�s l'arriv� de l'alimentation (via ActManager)
void SMALL_CYLINDER_ELEVATOR_init_pos(){
    SMALL_CYLINDER_ELEVATOR_initRX24();

	if(rx24_is_initialized == FALSE)
		return;

	debug_printf("Init pos : \n");
    if(!RX24_set_position(SMALL_CYLINDER_ELEVATOR_RX24_ID, SMALL_CYLINDER_ELEVATOR_RX24_INIT_POS))
        debug_printf("   Le RX24 n�%d n'est pas l�\n", SMALL_CYLINDER_ELEVATOR_RX24_ID);
	else
        debug_printf("   Le RX24 n�%d a �t� initialis� en position\n", SMALL_CYLINDER_ELEVATOR_RX24_ID);
}

// Fonction appell�e � la fin du match (via ActManager)
void SMALL_CYLINDER_ELEVATOR_stop(){
    RX24_set_torque_enabled(SMALL_CYLINDER_ELEVATOR_RX24_ID, FALSE); //Stopper l'asservissement du RX24
}

// fonction appell�e � la r�ception d'un message CAN (via ActManager)
bool_e SMALL_CYLINDER_ELEVATOR_CAN_process_msg(CAN_msg_t* msg) {
    if(msg->sid == ACT_SMALL_CYLINDER_ELEVATOR){
        SMALL_CYLINDER_ELEVATOR_initRX24();
		switch(msg->data.act_msg.order) {
			// Listing de toutes les positions de l'actionneur possible
            case ACT_SMALL_CYLINDER_ELEVATOR_IDLE :
            case ACT_SMALL_CYLINDER_ELEVATOR_TOP :
            case ACT_SMALL_CYLINDER_ELEVATOR_WAIT_FOR_SLOPE:
            case ACT_SMALL_CYLINDER_ELEVATOR_BOTTOM :
            case ACT_SMALL_CYLINDER_ELEVATOR_LOCK_WITH_CYLINDER :
            case ACT_SMALL_CYLINDER_ELEVATOR_STOP :
            	run_now = msg->data.act_msg.act_data.act_order.run_now;
                ACTQ_push_operation_from_msg(msg, QUEUE_ACT_RX24_SMALL_CYLINDER_ELEVATOR, &SMALL_CYLINDER_ELEVATOR_run_command, 0,TRUE);
				break;

            case ACT_WARNER:
            	SMALL_CYLINDER_ELEVATOR_set_warner(msg);
            	break;

			case ACT_GET_CONFIG:
				SMALL_CYLINDER_ELEVATOR_get_config(msg);
				break;

			case ACT_SET_CONFIG :
				SMALL_CYLINDER_ELEVATOR_set_config(msg);
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
void SMALL_CYLINDER_ELEVATOR_run_command(queue_id_t queueId, bool_e init) {
	if(QUEUE_has_error(queueId)) {
		QUEUE_behead(queueId);
		return;
	}

    if(QUEUE_get_act(queueId) == QUEUE_ACT_RX24_SMALL_CYLINDER_ELEVATOR) {    // Gestion des mouvements de SMALL_CYLINDER_ELEVATOR
		if(init)
            SMALL_CYLINDER_ELEVATOR_command_init(queueId);
		else
            SMALL_CYLINDER_ELEVATOR_command_run(queueId);
	}
}

//Initialise une commande
static void SMALL_CYLINDER_ELEVATOR_command_init(queue_id_t queueId) {
	Uint8 command = QUEUE_get_arg(queueId)->canCommand;
	Sint16* rx24_goalPosition = &QUEUE_get_arg(queueId)->param;

	*rx24_goalPosition = 0xFFFF;
    SMALL_CYLINDER_ELEVATOR_initRX24();

	switch(command) {
		// Listing de toutes les positions de l'actionneur possible avec les valeurs de position associ�es
        case ACT_SMALL_CYLINDER_ELEVATOR_IDLE : *rx24_goalPosition = SMALL_CYLINDER_ELEVATOR_RX24_IDLE_POS; break;
        case ACT_SMALL_CYLINDER_ELEVATOR_TOP : *rx24_goalPosition = SMALL_CYLINDER_ELEVATOR_RX24_TOP_POS; break;
		case ACT_SMALL_CYLINDER_ELEVATOR_WAIT_FOR_SLOPE : *rx24_goalPosition = SMALL_CYLINDER_ELEVATOR_RX24_WAIT_FOR_SLOPE; break;
		case ACT_SMALL_CYLINDER_ELEVATOR_BOTTOM : *rx24_goalPosition = SMALL_CYLINDER_ELEVATOR_RX24_BOTTOM_POS; break;
        case ACT_SMALL_CYLINDER_ELEVATOR_LOCK_WITH_CYLINDER : *rx24_goalPosition = SMALL_CYLINDER_ELEVATOR_RX24_LOCK_WITH_CYLINDER_POS; break;

        case ACT_SMALL_CYLINDER_ELEVATOR_STOP :
            RX24_set_torque_enabled(SMALL_CYLINDER_ELEVATOR_RX24_ID, FALSE); //Stopper l'asservissement du RX24
            QUEUE_next(queueId, ACT_SMALL_CYLINDER_ELEVATOR, ACT_RESULT_DONE, ACT_RESULT_ERROR_OK, __LINE__);
			return;

		default: {
            error_printf("Invalid SMALL_CYLINDER_ELEVATOR command: %u, code is broken !\n", command);
            QUEUE_next(queueId, ACT_SMALL_CYLINDER_ELEVATOR, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_LOGIC, __LINE__);
			return;
		}
	}

	if(rx24_is_initialized == FALSE){
		error_printf("Impossible de mettre l'actionneur en position il n'est pas initialis�\n");
        QUEUE_next(queueId, ACT_SMALL_CYLINDER_ELEVATOR, ACT_RESULT_FAILED, ACT_RESULT_ERROR_NOT_HERE, __LINE__);
		return;
	}

	if(*rx24_goalPosition == 0xFFFF) {
		error_printf("Invalid rx24 position for command: %u, code is broken !\n", command);
        QUEUE_next(queueId, ACT_SMALL_CYLINDER_ELEVATOR, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_LOGIC, __LINE__);
		return;
	}

    RX24_reset_last_error(SMALL_CYLINDER_ELEVATOR_RX24_ID); //S�curit� anti terroriste. Nous les parano on aime pas voir des erreurs l� ou il n'y en a pas.
    if(!RX24_set_position(SMALL_CYLINDER_ELEVATOR_RX24_ID, *rx24_goalPosition)) {	//Si la commande n'a pas �t� envoy�e correctement et/ou que le RX24 ne r�pond pas a cet envoi, on l'indique � la carte strat�gie
        error_printf("RX24_set_position error: 0x%x\n", RX24_get_last_error(SMALL_CYLINDER_ELEVATOR_RX24_ID).error);
        QUEUE_next(queueId, ACT_SMALL_CYLINDER_ELEVATOR, ACT_RESULT_FAILED, ACT_RESULT_ERROR_NOT_HERE, __LINE__);
		return;
	}
	//La commande a �t� envoy�e et le RX24 l'a bien re�u
    debug_printf("Move SMALL_CYLINDER_ELEVATOR rx24 to %d\n", *rx24_goalPosition);
}

//G�re les �tats pendant le mouvement du RX24
static void SMALL_CYLINDER_ELEVATOR_command_run(queue_id_t queueId) {
	Uint8 result, errorCode;
	Uint16 line;

	Uint16 pos = RX24_get_position(SMALL_CYLINDER_ELEVATOR_RX24_ID);

	if(!run_now){
		if(ACTQ_check_status_rx24(queueId, SMALL_CYLINDER_ELEVATOR_RX24_ID, QUEUE_get_arg(queueId)->param, pos, SMALL_CYLINDER_ELEVATOR_RX24_ASSER_POS_EPSILON, SMALL_CYLINDER_ELEVATOR_RX24_ASSER_TIMEOUT, SMALL_CYLINDER_ELEVATOR_RX24_ASSER_POS_LARGE_EPSILON, &result, &errorCode, &line))
			QUEUE_next(queueId, ACT_SMALL_CYLINDER_ELEVATOR, result, errorCode, line);
	}else{
		// Lorsqu'on vient de recevoir un run_now, on passe directement � l'ordre re�u.
		ACTQ_flush_queue_to_run_now(queueId, ACT_SMALL_CYLINDER_ELEVATOR);
		run_now = FALSE;
	}

    // On ne surveille le warner que si il est activ�
	if(warner.activated)
		SMALL_CYLINDER_ELEVATOR_check_warner(pos);
}

static void SMALL_CYLINDER_ELEVATOR_set_warner(CAN_msg_t *msg){
	warner.activated = TRUE;
	warner.last_pos = RX24_get_position(SMALL_CYLINDER_ELEVATOR_RX24_ID);

	switch(msg->data.act_msg.act_data.warner_pos){
		case ACT_SMALL_CYLINDER_ELEVATOR_WARNER:
			warner.warner_pos = SMALL_CYLINDER_ELEVATOR_RX24_WARNER_POS;
			debug_printf("WARNER is set \n\n");
			break;
		default:{
			warner.activated = FALSE;
			warn_printf("This position is not available for setting a warner\n");
		}
	}
}

static void SMALL_CYLINDER_ELEVATOR_check_warner(Uint16 pos){
	CAN_msg_t msg;

	if( (warner.last_pos < pos && warner.last_pos < warner.warner_pos && pos > warner.warner_pos)
     || (warner.last_pos > pos && warner.last_pos > warner.warner_pos && pos < warner.warner_pos)
	 || (warner.last_pos > RX24_MAX_WARNER && pos < RX24_MIN_WARNER	&& (warner.last_pos < warner.warner_pos || pos > warner.warner_pos))
	 || (warner.last_pos < RX24_MIN_WARNER && pos > RX24_MAX_WARNER	&& (warner.last_pos > warner.warner_pos || pos < warner.warner_pos))
	){
		// Envoi du message CAN pour pr�venir la strat
		msg.sid = ACT_WARNER_ANSWER;
		msg.size = SIZE_ACT_WARNER_ANSWER;
		msg.data.act_warner_answer.sid = 0xFF & ACT_SMALL_CYLINDER_ELEVATOR;
		CAN_send(&msg);

		// D�sactivation du warner
		warner.activated = FALSE;

		debug_printf("WARNER ANSWER SEND\n\n");
	}

	warner.last_pos = pos;
}
#endif
