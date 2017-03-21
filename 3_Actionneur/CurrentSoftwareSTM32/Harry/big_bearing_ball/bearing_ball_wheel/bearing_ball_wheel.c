/*  Club Robot ESEO 2016 - 2017
 *
 *	Fichier : BEARING_BALL_WHEEL_ax12.c
 *	Package : Carte actionneur
 *	Description : Gestion de l'actionneur AX12 BEARING_BALL_WHEEL
 *  Auteur :
 *  Version 2017
 *  Robot : BIG
 */

#include "bearing_ball_wheel.h"


// BEARING_BALL_WHEEL d'un actionneur standart avec AX12

// 1) Ajout l'actionneur dans QS_CANmsgList.h (tout � la fin du fichier)
// 2) Ajout d'une valeur dans l'�num�ration de la queue dans config_(big/small)/config_global_vars_types.h
//		Formatage : QUEUE_ACT_AX12_BEARING_BALL_WHEEL
// 3) Ajout de la d�claration de l'actionneur dans ActManager dans le tableau actionneurs
// 4) Ajout de la verbosit� dans le fichier act_queue_utils.c dans la fonction ACTQ_internal_printResult
// 5) Ajout du pilotage via terminal dans le fichier term_io.c dans le tableau terminal_motor du type : ACT_DECLARE(BEARING_BALL_WHEEL)
// 6) Un #define BEARING_BALL_WHEEL_AX12_ID doit avoir �t� ajout� au fichier config_big/config_pin.h ou config_small/config_pin.h
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
#ifdef I_AM_ROBOT_BIG

// Les diff�rents includes n�cessaires...
#include "../QS/QS_CANmsgList.h"
#include "../QS/QS_ax12.h"
#include "../QS/QS_can.h"
#include "../act_queue_utils.h"
#include "../selftest.h"
#include "../ActManager.h"

#include "bearing_ball_wheel_config.h"

// Les diff�rents define pour le verbose sur uart
#define LOG_PREFIX "bearing_ball_wheel.c : "
#define LOG_COMPONENT OUTPUT_LOG_COMPONENT_BEARING_BALL_WHEEL
#include "../QS/QS_outputlog.h"

// Les fonctions internes au fonctionnement de l'actionneur
static void BEARING_BALL_WHEEL_command_run(queue_id_t queueId);
static void BEARING_BALL_WHEEL_initAX12();
static void BEARING_BALL_WHEEL_command_init(queue_id_t queueId);
static void BEARING_BALL_WHEEL_set_config(CAN_msg_t* msg);
static void BEARING_BALL_WHEEL_get_position_config(ACT_order_e *pOrder, Uint16 *pPos);
static void BEARING_BALL_WHEEL_get_config(CAN_msg_t *incoming_msg);
static void BEARING_BALL_WHEEL_set_warner(CAN_msg_t *msg);
static void BEARING_BALL_WHEEL_check_warner(Uint16 pos);

// Bool�en contenant l'�tat actuel de l'initialisation de l'AX12 (Plusieurs bool�ens si plusieurs servo dans l'actionneur)
static bool_e ax12_is_initialized = FALSE;

// Warner de l'actionneur : D�clenche l'envoi d'un message CAN lorsqu'une certaine position est franchi
static act_warner_s warner;

// Fonction appell�e au lancement de la carte (via ActManager)
void BEARING_BALL_WHEEL_init() {
	static bool_e initialized = FALSE;

	if(initialized)
		return;
	initialized = TRUE;

	AX12_init();
    BEARING_BALL_WHEEL_initAX12();
}

// Fonction appell�e si la carte IHM a d�tect� une grosse chutte de la tension d'alimentation des servos
// Pour �viter les probl�mes d'utilisation de servo non initialis�
void BEARING_BALL_WHEEL_reset_config(){
	ax12_is_initialized = FALSE;
    BEARING_BALL_WHEEL_initAX12();
}

//Initialise l'AX12 s'il n'�tait pas aliment� lors d'initialisations pr�c�dentes, si d�j� initialis�, ne fait rien
static void BEARING_BALL_WHEEL_initAX12() {
    if(ax12_is_initialized == FALSE && AX12_is_ready(BEARING_BALL_WHEEL_AX12_ID) == TRUE) {
		ax12_is_initialized = TRUE;
        AX12_config_set_lowest_voltage(BEARING_BALL_WHEEL_AX12_ID, AX12_MIN_VOLTAGE);
        AX12_config_set_highest_voltage(BEARING_BALL_WHEEL_AX12_ID, AX12_MAX_VOLTAGE);
        AX12_set_torque_limit(BEARING_BALL_WHEEL_AX12_ID, BEARING_BALL_WHEEL_AX12_MAX_TORQUE_PERCENT);
        AX12_config_set_temperature_limit(BEARING_BALL_WHEEL_AX12_ID, BEARING_BALL_WHEEL_AX12_MAX_TEMPERATURE);

        AX12_config_set_maximal_angle(BEARING_BALL_WHEEL_AX12_ID, BEARING_BALL_WHEEL_AX12_MAX_VALUE);
        AX12_config_set_minimal_angle(BEARING_BALL_WHEEL_AX12_ID, BEARING_BALL_WHEEL_AX12_MIN_VALUE);
        AX12_set_move_to_position_speed(BEARING_BALL_WHEEL_AX12_ID, BEARING_BALL_WHEEL_AX12_SPEED);

        AX12_config_set_error_before_led(BEARING_BALL_WHEEL_AX12_ID, AX12_BEFORE_LED);
        AX12_config_set_error_before_shutdown(BEARING_BALL_WHEEL_AX12_ID, AX12_BEFORE_SHUTDOWN);
		debug_printf("Init config DONE\n");
	}else if(ax12_is_initialized == FALSE)
		debug_printf("Init config FAIL\n");
}


// Fonction appell�e pour la modification des configurations de l'ax12 telle que la vitesse et le couple (via ActManager)
// Dans le cas de multiple actionneur appartenant � un m�me actionneur, ajouter des defines dans QS_CANmsgList.h afin de pouvoir les choisirs facilement depuis la strat�gie
void BEARING_BALL_WHEEL_set_config(CAN_msg_t* msg){
	switch(msg->data.act_msg.act_data.act_config.sub_act_id){
		case DEFAULT_MONO_ACT : // Premier �lement de l'actionneur
            ACTMGR_config_AX12(BEARING_BALL_WHEEL_AX12_ID, msg);
			break;

		default :
			warn_printf("invalid CAN msg data[1]=%u (sous actionneur inexistant)!\n", msg->data.act_msg.act_data.act_config.sub_act_id);
	}
}

static void BEARING_BALL_WHEEL_get_config(CAN_msg_t *incoming_msg){
	bool_e error = FALSE;
	Uint16 pos = 0;;
	ACT_order_e order = 0;
	CAN_msg_t msg;
	msg.sid = ACT_GET_CONFIG_ANSWER;
	msg.size = SIZE_ACT_GET_CONFIG_ANSWER;
	msg.data.act_get_config_answer.sid = 0xFF & ACT_BEARING_BALL_WHEEL;
	msg.data.act_get_config_answer.config = incoming_msg->data.act_msg.act_data.config;

	switch(incoming_msg->data.act_msg.act_data.config){
		case POSITION_CONFIG:
			BEARING_BALL_WHEEL_get_position_config(&order, &pos);
			msg.data.act_get_config_answer.act_get_config_data.act_get_config_pos_answer.order = order;
			msg.data.act_get_config_answer.act_get_config_data.act_get_config_pos_answer.pos = pos;
			break;
		case TORQUE_CONFIG:
			msg.data.act_get_config_answer.act_get_config_data.torque = AX12_get_speed_percentage(BEARING_BALL_WHEEL_AX12_ID);
			break;
		case TEMPERATURE_CONFIG:
			msg.data.act_get_config_answer.act_get_config_data.temperature = AX12_get_temperature(BEARING_BALL_WHEEL_AX12_ID);
			break;
		case LOAD_CONFIG:
			msg.data.act_get_config_answer.act_get_config_data.load = AX12_get_load_percentage(BEARING_BALL_WHEEL_AX12_ID);
			break;
		default:
			error = TRUE;
			warn_printf("This config is not available \n");
	}

	if(!error){
		CAN_send(&msg); // Envoi du message CAN
	}
}

static void BEARING_BALL_WHEEL_get_position_config(ACT_order_e *pOrder, Uint16 *pPos){
	ACT_order_e order = ACT_BEARING_BALL_WHEEL_STOP;
	Uint16 position = AX12_get_position(BEARING_BALL_WHEEL_AX12_ID);
	Uint16 epsilon = BEARING_BALL_WHEEL_AX12_ASSER_POS_EPSILON;

	if(position > BEARING_BALL_WHEEL_AX12_UP_POS - epsilon && position < BEARING_BALL_WHEEL_AX12_UP_POS + epsilon){
		order = ACT_BEARING_BALL_WHEEL_UP;
	}else if(position > BEARING_BALL_WHEEL_AX12_DOWN_POS - epsilon && position < BEARING_BALL_WHEEL_AX12_DOWN_POS + epsilon){
		order = ACT_BEARING_BALL_WHEEL_DOWN;
	}else if(position > BEARING_BALL_WHEEL_AX12_IDLE_POS - epsilon && position < BEARING_BALL_WHEEL_AX12_IDLE_POS + epsilon){
		order = ACT_BEARING_BALL_WHEEL_IDLE;
	}

	if(pOrder != NULL)
		*pOrder = order;

	if(pPos != NULL)
		*pPos = position;
}

// Fonction appell�e pour l'initialisation en position de l'AX12 d�s l'arriv� de l'alimentation (via ActManager)
void BEARING_BALL_WHEEL_init_pos(){
    BEARING_BALL_WHEEL_initAX12();

	if(ax12_is_initialized == FALSE)
		return;

	debug_printf("Init pos : \n");
    if(!AX12_set_position(BEARING_BALL_WHEEL_AX12_ID, BEARING_BALL_WHEEL_AX12_INIT_POS))
        debug_printf("   L'AX12 n�%d n'est pas l�\n", BEARING_BALL_WHEEL_AX12_ID);
	else
        debug_printf("   L'AX12 n�%d a �t� initialis� en position\n", BEARING_BALL_WHEEL_AX12_ID);
}

// Fonction appell�e � la fin du match (via ActManager)
void BEARING_BALL_WHEEL_stop(){
    AX12_set_torque_enabled(BEARING_BALL_WHEEL_AX12_ID, FALSE); //Stopper l'asservissement de l'AX12
}

// fonction appell�e � la r�ception d'un message CAN (via ActManager)
bool_e BEARING_BALL_WHEEL_CAN_process_msg(CAN_msg_t* msg) {
    if(msg->sid == ACT_BEARING_BALL_WHEEL){
        BEARING_BALL_WHEEL_initAX12();
		switch(msg->data.act_msg.order) {
			// Listing de toutes les positions de l'actionneur possible
            case ACT_BEARING_BALL_WHEEL_IDLE :
            case ACT_BEARING_BALL_WHEEL_UP :
            case ACT_BEARING_BALL_WHEEL_DOWN :
            case ACT_BEARING_BALL_WHEEL_STOP :
                ACTQ_push_operation_from_msg(msg, QUEUE_ACT_AX12_BEARING_BALL_WHEEL, &BEARING_BALL_WHEEL_run_command, 0,TRUE);
				break;

            case ACT_WARNER:
				BEARING_BALL_WHEEL_set_warner(msg);
				break;

			case ACT_GET_CONFIG:
				BEARING_BALL_WHEEL_get_config(msg);
				break;

			case ACT_SET_CONFIG :
                BEARING_BALL_WHEEL_set_config(msg);
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
void BEARING_BALL_WHEEL_run_command(queue_id_t queueId, bool_e init) {
	if(QUEUE_has_error(queueId)) {
		QUEUE_behead(queueId);
		return;
	}

    if(QUEUE_get_act(queueId) == QUEUE_ACT_AX12_BEARING_BALL_WHEEL) {    // Gestion des mouvements de BEARING_BALL_WHEEL
		if(init)
            BEARING_BALL_WHEEL_command_init(queueId);
		else
            BEARING_BALL_WHEEL_command_run(queueId);
	}
}

//Initialise une commande
static void BEARING_BALL_WHEEL_command_init(queue_id_t queueId) {
	Uint8 command = QUEUE_get_arg(queueId)->canCommand;
	Sint16* ax12_goalPosition = &QUEUE_get_arg(queueId)->param;

	*ax12_goalPosition = 0xFFFF;
    BEARING_BALL_WHEEL_initAX12();

	switch(command) {
		// Listing de toutes les positions de l'actionneur possible avec les valeurs de position associ�es
        case ACT_BEARING_BALL_WHEEL_IDLE : *ax12_goalPosition = BEARING_BALL_WHEEL_AX12_IDLE_POS; break;
        case ACT_BEARING_BALL_WHEEL_UP : *ax12_goalPosition = BEARING_BALL_WHEEL_AX12_UP_POS; break;
        case ACT_BEARING_BALL_WHEEL_DOWN : *ax12_goalPosition = BEARING_BALL_WHEEL_AX12_DOWN_POS; break;

        case ACT_BEARING_BALL_WHEEL_STOP :
            AX12_set_torque_enabled(BEARING_BALL_WHEEL_AX12_ID, FALSE); //Stopper l'asservissement de l'AX12
            QUEUE_next(queueId, ACT_BEARING_BALL_WHEEL, ACT_RESULT_DONE, ACT_RESULT_ERROR_OK, __LINE__);
			return;

		default: {
            error_printf("Invalid BEARING_BALL_WHEEL command: %u, code is broken !\n", command);
            QUEUE_next(queueId, ACT_BEARING_BALL_WHEEL, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_LOGIC, __LINE__);
			return;
		}
	}

	if(ax12_is_initialized == FALSE){
		error_printf("Impossible de mettre l'actionneur en position il n'est pas initialis�\n");
        QUEUE_next(queueId, ACT_BEARING_BALL_WHEEL, ACT_RESULT_FAILED, ACT_RESULT_ERROR_NOT_HERE, __LINE__);
		return;
	}

	if(*ax12_goalPosition == 0xFFFF) {
		error_printf("Invalid ax12 position for command: %u, code is broken !\n", command);
        QUEUE_next(queueId, ACT_BEARING_BALL_WHEEL, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_LOGIC, __LINE__);
		return;
	}

    AX12_reset_last_error(BEARING_BALL_WHEEL_AX12_ID); //S�curit� anti terroriste. Nous les parano on aime pas voir des erreurs l� ou il n'y en a pas.
    if(!AX12_set_position(BEARING_BALL_WHEEL_AX12_ID, *ax12_goalPosition)) {	//Si la commande n'a pas �t� envoy�e correctement et/ou que l'AX12 ne r�pond pas a cet envoi, on l'indique � la carte strat�gie
        error_printf("AX12_set_position error: 0x%x\n", AX12_get_last_error(BEARING_BALL_WHEEL_AX12_ID).error);
        QUEUE_next(queueId, ACT_BEARING_BALL_WHEEL, ACT_RESULT_FAILED, ACT_RESULT_ERROR_NOT_HERE, __LINE__);
		return;
	}
	//La commande a �t� envoy�e et l'AX12 l'a bien re�u
    debug_printf("Move BEARING_BALL_WHEEL ax12 to %d\n", *ax12_goalPosition);
}

//G�re les �tats pendant le mouvement de l'AX12
static void BEARING_BALL_WHEEL_command_run(queue_id_t queueId) {
	Uint8 result, errorCode;
	Uint16 line;

	Uint16 pos = AX12_get_position(BEARING_BALL_WHEEL_AX12_ID);

    if(ACTQ_check_status_ax12(queueId, BEARING_BALL_WHEEL_AX12_ID, QUEUE_get_arg(queueId)->param, pos, BEARING_BALL_WHEEL_AX12_ASSER_POS_EPSILON, BEARING_BALL_WHEEL_AX12_ASSER_TIMEOUT, BEARING_BALL_WHEEL_AX12_ASSER_POS_LARGE_EPSILON, &result, &errorCode, &line))
        QUEUE_next(queueId, ACT_BEARING_BALL_WHEEL, result, errorCode, line);

    // On ne surveille le warner que si il est activ�
	if(warner.activated)
		BEARING_BALL_WHEEL_check_warner(pos);
}

static void BEARING_BALL_WHEEL_set_warner(CAN_msg_t *msg){
	warner.activated = TRUE;
	warner.last_pos = AX12_get_position(BEARING_BALL_WHEEL_AX12_ID);

	switch(msg->data.act_msg.act_data.warner_pos){
		/*case ACT_BEARING_BALL_WHEEL_WARNER:
			warner.warner_pos = BEARING_BALL_WHEEL_AX12_WARNER_POS;
			break;*/
		default:{
			warner.activated = FALSE;
			warn_printf("This position is not available for setting a warner\n");
		}
	}
}

static void BEARING_BALL_WHEEL_check_warner(Uint16 pos){
	CAN_msg_t msg;

	if( (warner.last_pos < pos && warner.last_pos < warner.warner_pos && pos > warner.warner_pos)
     || (warner.last_pos > pos && warner.last_pos > warner.warner_pos && pos < warner.warner_pos)
	 || (warner.last_pos > AX12_MAX_WARNER && pos < AX12_MIN_WARNER	&& (warner.last_pos < warner.warner_pos || pos > warner.warner_pos))
	 || (warner.last_pos < AX12_MIN_WARNER && pos > AX12_MAX_WARNER	&& (warner.last_pos > warner.warner_pos || pos < warner.warner_pos))
	){
		// Envoi du message CAN pour pr�venir la strat
		msg.sid = ACT_WARNER_ANSWER;
		msg.size = SIZE_ACT_WARNER_ANSWER;
		msg.data.act_warner_answer.sid = 0xFF & ACT_BEARING_BALL_WHEEL;
		CAN_send(&msg);

		// D�sactivation du warner
		warner.activated = FALSE;
	}

	warner.last_pos = pos;
}
#endif
