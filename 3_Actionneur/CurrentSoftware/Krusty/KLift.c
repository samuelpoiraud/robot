
/*  Club Robot ESEO 2012 - 2013
 *	Krusty
 *
 *	Fichier : Lift.c
 *	Package : Carte actionneur
 *	Description : Gestion des ascenseurs
 *  Auteur : Alexis
 *  Version 20130314
 *  Robot : Krusty
 */

#include "KLift.h"
#ifdef I_AM_ROBOT_KRUSTY

#include "../QS/QS_can.h"
#include "../QS/QS_CANmsgList.h"
#include "../QS/QS_DCMotor2.h"
#include "../QS/QS_ax12.h"
#include "../QS/QS_adc.h"
#include "../output_log.h"
#include "../Can_msg_processing.h"

#define LOG_PREFIX "LI: "

#define LIFT_NUM_POS           3
	#define LIFT_UP_POS_ID     0
	#define LIFT_MID_POS_ID    1
	#define LIFT_DOWN_POS_ID   2

#if DCMOTOR_NB_POS < LIFT_NUM_POS
#error "Le nombre de position disponible dans l'asservissement DCMotor n'est pas suffisant"
#endif

#define LIFT_IS_LEFT(param_queueId) (QUEUE_get_act(param_queueId) == QUEUE_ACT_Lift_Left_Translation || QUEUE_get_act(param_queueId) == QUEUE_ACT_Lift_Left_AX12_Plier)
#define LIFT_IS_RIGHT(param_queueId) (QUEUE_get_act(param_queueId) == QUEUE_ACT_Lift_Right_Translation || QUEUE_get_act(param_queueId) == QUEUE_ACT_Lift_Right_AX12_Plier)
#define LIFT_IS_DCMOTOR(param_queueId) (QUEUE_get_act(param_queueId) == QUEUE_ACT_Lift_Left_Translation || QUEUE_get_act(param_queueId) == QUEUE_ACT_Lift_Right_Translation)
#define LIFT_IS_AX12(param_queueId) (QUEUE_get_act(param_queueId) == QUEUE_ACT_Lift_Left_AX12_Plier || QUEUE_get_act(param_queueId) == QUEUE_ACT_Lift_Right_AX12_Plier)

static void LIFT_initAX12();
static Sint16 LIFT_LEFT_getTranslationPos();
static Sint16 LIFT_RIGHT_getTranslationPos();
static void LIFT_run_command(queue_id_t queueId, bool_e init);
static void LIFT_translation_command_init(queue_id_t queueId);
static void LIFT_translation_command_run(queue_id_t queueId);
static void LIFT_plier_command_init(queue_id_t queueId);
static void LIFT_plier_command_run(queue_id_t queueId);

void LIFT_init() {
	DCMotor_config_t left_lift_translation_config;
	DCMotor_config_t right_lift_translation_config;
	static bool_e initialized = FALSE;

	if(initialized)
		return;
	initialized = TRUE;

	DCM_init();
	AX12_init();
	ADC_init();

	left_lift_translation_config.sensor_read = &LIFT_LEFT_getTranslationPos;
	left_lift_translation_config.Kp = LIFT_LEFT_ASSER_KP;
	left_lift_translation_config.Ki = LIFT_LEFT_ASSER_KI;
	left_lift_translation_config.Kd = LIFT_LEFT_ASSER_KD;
	left_lift_translation_config.pos[LIFT_UP_POS_ID] = LIFT_LEFT_UP_POS;
	left_lift_translation_config.pos[LIFT_MID_POS_ID] = LIFT_LEFT_MID_POS;
	left_lift_translation_config.pos[LIFT_DOWN_POS_ID] = LIFT_LEFT_DOWN_POS;
	left_lift_translation_config.pwm_number = LIFT_LEFT_DCMOTOR_PWM_NUM;
	left_lift_translation_config.way_latch = &LIFT_LEFT_DCMOTOR_PORT_WAY;
	left_lift_translation_config.way_bit_number = LIFT_LEFT_DCMOTOR_PORT_WAY_BIT;
	left_lift_translation_config.way0_max_duty = LIFT_LEFT_DCMOTOR_MAX_PWM_WAY0;
	left_lift_translation_config.way1_max_duty = LIFT_LEFT_DCMOTOR_MAX_PWM_WAY1;
	left_lift_translation_config.timeout = LIFT_LEFT_ASSER_TIMEOUT;
	left_lift_translation_config.epsilon = LIFT_LEFT_ASSER_POS_EPSILON;
	DCM_config(LIFT_LEFT_DCMOTOR_ID, &left_lift_translation_config);
	DCM_stop(LIFT_LEFT_DCMOTOR_ID);

	right_lift_translation_config.sensor_read = &LIFT_RIGHT_getTranslationPos;
	right_lift_translation_config.Kp = LIFT_LEFT_ASSER_KP;
	right_lift_translation_config.Ki = LIFT_LEFT_ASSER_KI;
	right_lift_translation_config.Kd = LIFT_LEFT_ASSER_KD;
	right_lift_translation_config.pos[LIFT_UP_POS_ID] = LIFT_LEFT_UP_POS;
	right_lift_translation_config.pos[LIFT_MID_POS_ID] = LIFT_LEFT_MID_POS;
	right_lift_translation_config.pos[LIFT_DOWN_POS_ID] = LIFT_LEFT_DOWN_POS;
	right_lift_translation_config.pwm_number = LIFT_LEFT_DCMOTOR_PWM_NUM;
	right_lift_translation_config.way_latch = &LIFT_LEFT_DCMOTOR_PORT_WAY;
	right_lift_translation_config.way_bit_number = LIFT_LEFT_DCMOTOR_PORT_WAY_BIT;
	right_lift_translation_config.way0_max_duty = LIFT_LEFT_DCMOTOR_MAX_PWM_WAY0;
	right_lift_translation_config.way1_max_duty = LIFT_LEFT_DCMOTOR_MAX_PWM_WAY1;
	right_lift_translation_config.timeout = LIFT_LEFT_ASSER_TIMEOUT;
	right_lift_translation_config.epsilon = LIFT_LEFT_ASSER_POS_EPSILON;
	DCM_config(LIFT_RIGHT_DCMOTOR_ID, &right_lift_translation_config);
	DCM_stop(LIFT_RIGHT_DCMOTOR_ID);

	LIFT_initAX12();

	OUTPUTLOG_printf(LOG_LEVEL_Info, LOG_PREFIX"actionneur ascenseur initialisé\n");
}

//Initialise l'AX12 de la pince s'il n'était pas allimenté lors d'initialisations précédentes, si déjà initialisé, ne fait rien
static void LIFT_initAX12() {
	static bool_e left_ax12_is_initialized = FALSE;
	static bool_e right_ax12_is_initialized = FALSE;
	if(left_ax12_is_initialized == FALSE && AX12_is_ready(LIFT_LEFT_PLIER_AX12_ID) == TRUE) {
		left_ax12_is_initialized = TRUE;
		AX12_config_set_highest_voltage(LIFT_LEFT_PLIER_AX12_ID, 136);
		AX12_config_set_lowest_voltage(LIFT_LEFT_PLIER_AX12_ID, 70);
		AX12_config_set_maximum_torque_percentage(LIFT_LEFT_PLIER_AX12_ID, LIFT_LEFT_PLIER_AX12_MAX_TORQUE_PERCENT);

		//FIXME: A voir, l'angle effectif n'est pas super précis pour pouvoir utiliser directement les positions sans prendre de marge.
	//	AX12_config_set_maximal_angle(LIFT_LEFT_PLIER_AX12_ID, (LIFT_LEFT_PLIER_AX12_CLOSED_POS > LIFT_LEFT_PLIER_AX12_OPEN_POS)? LIFT_PLIER_AX12_CLOSED_POS : LIFT_PLIER_AX12_OPEN_POS);
	//	AX12_config_set_minimal_angle(LIFT_LEFT_PLIER_AX12_ID, (LIFT_LEFT_PLIER_AX12_CLOSED_POS < LIFT_LEFT_PLIER_AX12_OPEN_POS)? LIFT_PLIER_AX12_CLOSED_POS : LIFT_PLIER_AX12_OPEN_POS);

		AX12_config_set_error_before_led(LIFT_LEFT_PLIER_AX12_ID, AX12_ERROR_ANGLE | AX12_ERROR_CHECKSUM | AX12_ERROR_INSTRUCTION | AX12_ERROR_OVERHEATING | AX12_ERROR_OVERLOAD | AX12_ERROR_RANGE);
		AX12_config_set_error_before_shutdown(LIFT_LEFT_PLIER_AX12_ID, AX12_ERROR_OVERHEATING); //On ne met pas l'overload comme par defaut, il faut pouvoir tenir l'assiette et sans que l'AX12 ne s'arrête de forcer pour cause de couple resistant trop fort.
	}
	if(right_ax12_is_initialized == FALSE && AX12_is_ready(LIFT_RIGHT_PLIER_AX12_ID) == TRUE) {
		right_ax12_is_initialized = TRUE;
		AX12_config_set_highest_voltage(LIFT_RIGHT_PLIER_AX12_ID, 136);
		AX12_config_set_lowest_voltage(LIFT_RIGHT_PLIER_AX12_ID, 70);
		AX12_config_set_maximum_torque_percentage(LIFT_RIGHT_PLIER_AX12_ID, LIFT_RIGHT_PLIER_AX12_MAX_TORQUE_PERCENT);

		AX12_config_set_error_before_led(LIFT_RIGHT_PLIER_AX12_ID, AX12_ERROR_ANGLE | AX12_ERROR_CHECKSUM | AX12_ERROR_INSTRUCTION | AX12_ERROR_OVERHEATING | AX12_ERROR_OVERLOAD | AX12_ERROR_RANGE);
		AX12_config_set_error_before_shutdown(LIFT_RIGHT_PLIER_AX12_ID, AX12_ERROR_OVERHEATING); //On ne met pas l'overload comme par defaut, il faut pouvoir tenir l'assiette et sans que l'AX12 ne s'arrête de forcer pour cause de couple resistant trop fort.
	}
}

bool_e LIFT_CAN_process_msg(CAN_msg_t* msg) {

	if(msg->sid == ACT_LIFT_LEFT || msg->sid == ACT_LIFT_RIGHT) {
		QUEUE_act_e liftAct;

		//Initialise l'AX12 de la pince s'il n'était pas alimenté lors d'initialisations précédentes, si déjà initialisé, ne fait rien
		LIFT_initAX12();

		switch(msg->data[0]) {
			case ACT_LIFT_GO_UP:
			case ACT_LIFT_GO_MID:
			case ACT_LIFT_GO_DOWN:
			case ACT_LIFT_STOP:
				if(msg->sid == ACT_LIFT_LEFT)
					liftAct = QUEUE_ACT_Lift_Left_Translation;
				else liftAct = QUEUE_ACT_Lift_Right_Translation;
				CAN_push_operation_from_msg(msg, liftAct, &LIFT_run_command, 0);
				break;

			case ACT_LIFT_PLIER_CLOSE:
			case ACT_LIFT_PLIER_OPEN:
			case ACT_LIFT_PLIER_STOP:
				if(msg->sid == ACT_LIFT_LEFT)
					liftAct = QUEUE_ACT_Lift_Left_AX12_Plier;
				else liftAct = QUEUE_ACT_Lift_Right_AX12_Plier;
				CAN_push_operation_from_msg(msg, liftAct, &LIFT_run_command, 0);
				break;

			default:
				OUTPUTLOG_printf(LOG_LEVEL_Warning, LOG_PREFIX"invalid CAN msg data[0]=%u !\n", msg->data[0]);
		}
		return TRUE;
	}

	return FALSE;
}

static Sint16 LIFT_LEFT_getTranslationPos() {
	return ADC_getValue(LIFT_LEFT_TRANSLATION_POTAR_ADC_ID);
}

static Sint16 LIFT_RIGHT_getTranslationPos() {
	return ADC_getValue(LIFT_RIGHT_TRANSLATION_POTAR_ADC_ID);
}

static void LIFT_run_command(queue_id_t queueId, bool_e init) {
	if(LIFT_IS_DCMOTOR(queueId)) {    // Gestion des mouvements de translation d'un ascenseur
		if(init && !QUEUE_has_error(queueId))
			LIFT_translation_command_init(queueId);
		else
			LIFT_translation_command_run(queueId);
	} else if(LIFT_IS_AX12(queueId)) {    // Gestion des mouvements de la pince a verre
		if(init && !QUEUE_has_error(queueId))
			LIFT_plier_command_init(queueId);
		else
			LIFT_plier_command_run(queueId);
	}
}

//Initialisation d'une commande de translation du bras
static void LIFT_translation_command_init(queue_id_t queueId) {
	Uint8 dcMotorId = (LIFT_IS_LEFT(queueId))? LIFT_LEFT_DCMOTOR_ID : LIFT_RIGHT_DCMOTOR_ID;
	Uint8 command = QUEUE_get_arg(queueId)->canCommand;
	Uint8 wantedPosition;
	switch(command) {
		case ACT_LIFT_GO_UP:   wantedPosition = LIFT_UP_POS_ID;   break;
		case ACT_LIFT_GO_MID:  wantedPosition = LIFT_MID_POS_ID;  break;
		case ACT_LIFT_GO_DOWN: wantedPosition = LIFT_DOWN_POS_ID; break;
		case ACT_LIFT_STOP:
			DCM_stop(dcMotorId);
			//Le moteur atteindra l'état IDLE automatiquement et le message de retour sera envoyé, voir LIFT_translation_command_run
			return;

		default: {
				Uint8 canSid = (LIFT_IS_LEFT(queueId))? ACT_LIFT_LEFT : ACT_LIFT_RIGHT;
				CAN_msg_t resultMsg = {ACT_RESULT, {canSid, command, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_LOGIC}, 4};
				CAN_send(&resultMsg);
				OUTPUTLOG_printf(LOG_LEVEL_Error, LOG_PREFIX"invalid translation command: %u, code is broken !\n", command);
				QUEUE_set_error(queueId);
				QUEUE_behead(queueId);
				return;
			}
	}
	DCM_goToPos(dcMotorId, wantedPosition);
	DCM_restart(dcMotorId);
}

//Gestion des états en cours d'exécution de la commande de translation
static void LIFT_translation_command_run(queue_id_t queueId) {
	Uint8 canSid = (LIFT_IS_LEFT(queueId))? ACT_LIFT_LEFT : ACT_LIFT_RIGHT;
	Uint8 dcMotorId = (LIFT_IS_LEFT(queueId))? LIFT_LEFT_DCMOTOR_ID : LIFT_RIGHT_DCMOTOR_ID;
	DCM_working_state_e asserState = DCM_get_state(dcMotorId);
	CAN_msg_t resultMsg;

	if(asserState == DCM_IDLE) {
		resultMsg.data[2] = ACT_RESULT_DONE;
		resultMsg.data[3] = ACT_RESULT_ERROR_OK;
	} else if(QUEUE_has_error(queueId)) {
		resultMsg.data[2] = ACT_RESULT_NOT_HANDLED;
		resultMsg.data[3] = ACT_RESULT_ERROR_OTHER;
	} else if(asserState == DCM_TIMEOUT) {
		resultMsg.data[2] = ACT_RESULT_FAILED;
		resultMsg.data[3] = ACT_RESULT_ERROR_TIMEOUT;
	QUEUE_set_error(queueId);
	} else return;	//Operation is not finished, do nothing

	resultMsg.sid = ACT_RESULT;
	resultMsg.data[0] = canSid;
	resultMsg.data[1] = QUEUE_get_arg(queueId)->canCommand;
	resultMsg.size = 4;

	CAN_send(&resultMsg);
	QUEUE_behead(queueId);	//gestion terminée
}

//Initialise une commande de la pince à assiette
static void LIFT_plier_command_init(queue_id_t queueId) {
	Uint8 canSid = (LIFT_IS_LEFT(queueId))? ACT_LIFT_LEFT : ACT_LIFT_RIGHT;
	Uint8 ax12Id = (LIFT_IS_LEFT(queueId))? LIFT_LEFT_PLIER_AX12_ID : LIFT_RIGHT_PLIER_AX12_ID;
	Uint16 ax12OpenPos = (LIFT_IS_LEFT(queueId))? LIFT_LEFT_PLIER_AX12_OPEN_POS : LIFT_RIGHT_PLIER_AX12_OPEN_POS;
	Uint16 ax12ClosedPos = (LIFT_IS_LEFT(queueId))? LIFT_LEFT_PLIER_AX12_CLOSED_POS : LIFT_RIGHT_PLIER_AX12_CLOSED_POS;
	Uint8 command = QUEUE_get_arg(queueId)->canCommand;
	bool_e cmdOk;
	Uint16* ax12_goalPosition = &QUEUE_get_arg(queueId)->param;

	*ax12_goalPosition = 0xFFFF;

	switch(command) {
		case ACT_LIFT_PLIER_OPEN:  *ax12_goalPosition = ax12OpenPos; break;
		case ACT_LIFT_PLIER_CLOSE: *ax12_goalPosition = ax12ClosedPos; break;
		case ACT_LIFT_PLIER_STOP:
			AX12_set_torque_enabled(ax12Id, FALSE); //Stopper l'asservissement de l'AX12 qui gère la pince
			CAN_msg_t resultMsg = {ACT_RESULT, {canSid, command, ACT_RESULT_DONE, ACT_RESULT_ERROR_OK}, 4};
			CAN_send(&resultMsg);
			QUEUE_behead(queueId);
			return;

		default: {
				CAN_msg_t resultMsg = {ACT_RESULT, {canSid, command, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_LOGIC}, 4};
				CAN_send(&resultMsg);
				OUTPUTLOG_printf(LOG_LEVEL_Error, LOG_PREFIX"invalid plier command: %u, code is broken !\n", command);
				QUEUE_set_error(queueId);
				QUEUE_behead(queueId);
				return;
			}
	}
	if(*ax12_goalPosition == 0xFFFF) {
		OUTPUTLOG_printf(LOG_LEVEL_Error, LOG_PREFIX"invalid plier position: %u, code is broken !\n", command);
		return;
	}

	AX12_reset_last_error(ax12Id); //Sécurité anti terroriste. Nous les parano on aime pas voir des erreurs là ou il n'y en a pas.
	cmdOk = AX12_set_position(ax12Id, *ax12_goalPosition);
	if(!cmdOk) {	//Si la commande n'a pas été envoyée correctement et/ou que l'AX12 ne répond pas a cet envoi, on l'indique à la carte stratégie
		CAN_msg_t resultMsg = {ACT_RESULT, {canSid, command, ACT_RESULT_FAILED, ACT_RESULT_ERROR_NOT_HERE}, 4};
		CAN_send(&resultMsg);
		QUEUE_set_error(queueId);
		QUEUE_behead(queueId);
		return;
	}
	//La commande a été envoyée et l'AX12 l'a bien reçu
}

//Gère les états pendant le mouvement de la pince.
static void LIFT_plier_command_run(queue_id_t queueId) {
	Uint8 canSid = (LIFT_IS_LEFT(queueId))? ACT_LIFT_LEFT : ACT_LIFT_RIGHT;
	Uint8 ax12Id = (LIFT_IS_LEFT(queueId))? LIFT_LEFT_PLIER_AX12_ID : LIFT_RIGHT_PLIER_AX12_ID;
	Uint16 posEpsilon = (LIFT_IS_LEFT(queueId))? LIFT_LEFT_PLIER_AX12_ASSER_POS_EPSILON : LIFT_RIGHT_PLIER_AX12_ASSER_POS_EPSILON;
	Uint16 asserTimeout = (LIFT_IS_LEFT(queueId))? LIFT_LEFT_PLIER_AX12_ASSER_TIMEOUT : LIFT_RIGHT_PLIER_AX12_ASSER_TIMEOUT;

	Uint8 error;
	Uint16 ax12Pos;
	CAN_msg_t resultMsg;
	Uint16* ax12_goalPosition = &QUEUE_get_arg(queueId)->param;

	AX12_reset_last_error(ax12Id);
	ax12Pos = AX12_get_position(ax12Id); //même si non utilisé, permet de faire un ping en même temps. S'il n'est plus là (parce que kingkong l'a kidnappé par exemple) il ne répondra plus.
	error = AX12_get_last_error(ax12Id).error;

	if(QUEUE_has_error(queueId)) {
		resultMsg.data[2] = ACT_RESULT_NOT_HANDLED;
		resultMsg.data[3] = ACT_RESULT_ERROR_OTHER;
	} else if(abs((Sint16)ax12Pos - (Sint16)(*ax12_goalPosition)) <= posEpsilon) {	//Fin du mouvement
	//if(AX12_is_moving(LIFT_PLIER_AX12_ID) == FALSE) {  //Fin du mouvement
		resultMsg.data[2] = ACT_RESULT_DONE;
		resultMsg.data[3] = ACT_RESULT_ERROR_OK;
	} else if((error & AX12_ERROR_TIMEOUT) && (error & AX12_ERROR_RANGE)) {
		resultMsg.data[2] = ACT_RESULT_NOT_HANDLED;
		resultMsg.data[3] = ACT_RESULT_ERROR_LOGIC;	//Si le driver a attendu trop longtemps, c'est a cause d'un deadlock plutot qu'un manque de ressources (il attend suffisament longtemps pour que les commandes soit bien envoyées)
		AX12_set_torque_enabled(ax12Id, FALSE);
		QUEUE_set_error(queueId);
	} else if(error & AX12_ERROR_TIMEOUT) {	//L'ax12 n'a pas répondu à la commande
		resultMsg.data[2] = ACT_RESULT_FAILED;
		resultMsg.data[3] = ACT_RESULT_ERROR_NOT_HERE;
		AX12_set_torque_enabled(ax12Id, FALSE);
		QUEUE_set_error(queueId);
	} else if(CLOCK_get_time() >= QUEUE_get_initial_time(queueId) + asserTimeout) {    //Timeout, l'ax12 n'a pas bouger à la bonne position a temps
		resultMsg.data[2] = ACT_RESULT_FAILED;
		resultMsg.data[3] = ACT_RESULT_ERROR_UNKNOWN;
		AX12_set_torque_enabled(ax12Id, FALSE);
		QUEUE_set_error(queueId);
	} else if(error & ~AX12_ERROR_OVERLOAD) {							//autres erreurs (sans compter l'overload si on force sur la pince pour serrer l'assiette)
		resultMsg.data[2] = ACT_RESULT_FAILED;
		resultMsg.data[3] = ACT_RESULT_ERROR_UNKNOWN;
		AX12_set_torque_enabled(ax12Id, FALSE);
		QUEUE_set_error(queueId);
	} else return;	//Operation is not finished, do nothing

	resultMsg.sid = ACT_RESULT;
	resultMsg.data[0] = canSid;
	resultMsg.data[1] = QUEUE_get_arg(queueId)->canCommand;
	resultMsg.size = 4;

	CAN_send(&resultMsg);
	QUEUE_behead(queueId);	//gestion terminée
}


#endif  /* I_AM_ROBOT_KRUSTY */
