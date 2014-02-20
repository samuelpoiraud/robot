
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

//#include "../QS/QS_can.h"
#include "../QS/QS_CANmsgList.h"
#include "../QS/QS_DCMotor2.h"
#include "../QS/QS_ax12.h"
#include "../QS/QS_adc.h"
#include "../act_queue_utils.h"
#include "config_krusty/config_pin.h"
#include "KLift_config.h"

#include "config_debug.h"
#define LOG_PREFIX "LI: "
#define LOG_COMPONENT OUTPUT_LOG_COMPONENT_LIFT
#include "../QS/QS_outputlog.h"

#define LIFT_NUM_POS           3
	#define LIFT_UP_POS_ID     0
	#define LIFT_MID_POS_ID    1
	#define LIFT_DOWN_POS_ID   2

#if DCMOTOR_NB_POS < LIFT_NUM_POS
#error "Le nombre de position disponible dans l'asservissement DCMotor n'est pas suffisant"
#endif
#if DCM_NUMBER <= LIFT_LEFT_DCMOTOR_ID || DCM_NUMBER <= LIFT_RIGHT_DCMOTOR_ID
#error "Le nombre de DCMotor disponible n'est pas suffisant, veuillez augmenter DCM_NUMBER"
#endif

#define LIFT_IS_LEFT(param_queueId) (QUEUE_get_act(param_queueId) == QUEUE_ACT_Lift_Left_Translation || QUEUE_get_act(param_queueId) == QUEUE_ACT_Lift_Left_AX12_Plier)
#define LIFT_IS_RIGHT(param_queueId) (QUEUE_get_act(param_queueId) == QUEUE_ACT_Lift_Right_Translation || QUEUE_get_act(param_queueId) == QUEUE_ACT_Lift_Right_AX12_Plier)
#define LIFT_IS_DCMOTOR(param_queueId) (QUEUE_get_act(param_queueId) == QUEUE_ACT_Lift_Left_Translation || QUEUE_get_act(param_queueId) == QUEUE_ACT_Lift_Right_Translation)
#define LIFT_IS_AX12(param_queueId) (QUEUE_get_act(param_queueId) == QUEUE_ACT_Lift_Left_AX12_Plier || QUEUE_get_act(param_queueId) == QUEUE_ACT_Lift_Right_AX12_Plier)

static void LIFT_initAX12();
static Sint16 LIFT_LEFT_getTranslationPos();
static Sint16 LIFT_RIGHT_getTranslationPos();
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
	right_lift_translation_config.Kp = LIFT_RIGHT_ASSER_KP;
	right_lift_translation_config.Ki = LIFT_RIGHT_ASSER_KI;
	right_lift_translation_config.Kd = LIFT_RIGHT_ASSER_KD;
	right_lift_translation_config.pos[LIFT_UP_POS_ID] = LIFT_RIGHT_UP_POS;
	right_lift_translation_config.pos[LIFT_MID_POS_ID] = LIFT_RIGHT_MID_POS;
	right_lift_translation_config.pos[LIFT_DOWN_POS_ID] = LIFT_RIGHT_DOWN_POS;
	right_lift_translation_config.pwm_number = LIFT_RIGHT_DCMOTOR_PWM_NUM;
	right_lift_translation_config.way_latch = &LIFT_RIGHT_DCMOTOR_PORT_WAY;
	right_lift_translation_config.way_bit_number = LIFT_RIGHT_DCMOTOR_PORT_WAY_BIT;
	right_lift_translation_config.way0_max_duty = LIFT_RIGHT_DCMOTOR_MAX_PWM_WAY0;
	right_lift_translation_config.way1_max_duty = LIFT_RIGHT_DCMOTOR_MAX_PWM_WAY1;
	right_lift_translation_config.timeout = LIFT_RIGHT_ASSER_TIMEOUT;
	right_lift_translation_config.epsilon = LIFT_RIGHT_ASSER_POS_EPSILON;
	DCM_config(LIFT_RIGHT_DCMOTOR_ID, &right_lift_translation_config);
	DCM_stop(LIFT_RIGHT_DCMOTOR_ID);

	info_printf("Ascenseur à verres initialisés (DCMotor)\n");

	LIFT_initAX12();
}

//Initialise l'AX12 de la pince s'il n'était pas allimenté lors d'initialisations précédentes, si déjà initialisé, ne fait rien
static void LIFT_initAX12() {
	static bool_e ax12_is_initialized[2] = {FALSE, FALSE};
	Uint8 ax12_id[2] =             {LIFT_LEFT_PLIER_AX12_ID,                 LIFT_RIGHT_PLIER_AX12_ID};
	Uint8 ax12_max_torque[2] =     {LIFT_LEFT_PLIER_AX12_MAX_TORQUE_PERCENT, LIFT_RIGHT_PLIER_AX12_MAX_TORQUE_PERCENT};
//	Uint16 ax12_init_position[2] = {LIFT_LEFT_ACT_PLIER_AX12_INIT_POS,       LIFT_RIGHT_ACT_PLIER_AX12_INIT_POS};
	Uint8 i;
	for(i = 0; i < 2; i++) { //Init des 2 ax12, gauche et droite
		if(ax12_is_initialized[i] == FALSE && AX12_is_ready(ax12_id[i]) == TRUE) {
			ax12_is_initialized[i] = TRUE;

			AX12_config_set_highest_voltage(ax12_id[i], 136);
			AX12_config_set_lowest_voltage(ax12_id[i], 70);
			AX12_config_set_maximum_torque_percentage(ax12_id[i], ax12_max_torque[i]);

			AX12_config_set_maximal_angle(ax12_id[i], 300);
			AX12_config_set_minimal_angle(ax12_id[i], 0);

			AX12_config_set_error_before_led(ax12_id[i], AX12_ERROR_ANGLE | AX12_ERROR_CHECKSUM | AX12_ERROR_INSTRUCTION | AX12_ERROR_OVERHEATING | AX12_ERROR_OVERLOAD | AX12_ERROR_RANGE);
			AX12_config_set_error_before_shutdown(ax12_id[i], AX12_ERROR_OVERHEATING); //On ne met pas l'overload comme par defaut, il faut pouvoir tenir l'assiette et sans que l'AX12 ne s'arrête de forcer pour cause de couple resistant trop fort.

			info_printf("AX12 %s initialisé\n", (i == 0)? "Gauche" : "Droite");
		}
	}
}

void LIFT_stop() {
	DCM_stop(LIFT_LEFT_DCMOTOR_ID);
	DCM_stop(LIFT_RIGHT_DCMOTOR_ID);
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
				ACTQ_push_operation_from_msg(msg, liftAct, &LIFT_run_command, 0);
				break;

			case ACT_LIFT_PLIER_CLOSE:
			case ACT_LIFT_PLIER_OPEN:
			case ACT_LIFT_PLIER_STOP:
				if(msg->sid == ACT_LIFT_LEFT)
					liftAct = QUEUE_ACT_Lift_Left_AX12_Plier;
				else liftAct = QUEUE_ACT_Lift_Right_AX12_Plier;
				ACTQ_push_operation_from_msg(msg, liftAct, &LIFT_run_command, 0);
				break;

			default:
				warn_printf("invalid CAN msg data[0]=%u !\n", msg->data[0]);
		}
		return TRUE;
	}

	return FALSE;
}

static Sint16 LIFT_LEFT_getTranslationPos() {
	return -ADC_getValue(LIFT_LEFT_TRANSLATION_POTAR_ADC_ID);
}

static Sint16 LIFT_RIGHT_getTranslationPos() {
	return -ADC_getValue(LIFT_RIGHT_TRANSLATION_POTAR_ADC_ID);
}

void LIFT_run_command(queue_id_t queueId, bool_e init) {
	if(QUEUE_has_error(queueId)) {
		QUEUE_behead(queueId);
		return;
	}

	if(LIFT_IS_DCMOTOR(queueId)) {    // Gestion des mouvements de translation d'un ascenseur
		if(init)
			LIFT_translation_command_init(queueId);
		else
			LIFT_translation_command_run(queueId);
	} else if(LIFT_IS_AX12(queueId)) {    // Gestion des mouvements de la pince a verre
		if(init)
			LIFT_plier_command_init(queueId);
		else
			LIFT_plier_command_run(queueId);
	} else {
		Uint8 canSid = (LIFT_IS_LEFT(queueId))? ACT_LIFT_LEFT : ACT_LIFT_RIGHT;
		error_printf("Invalid act: %d\n", QUEUE_get_act(queueId));
		QUEUE_next(queueId, canSid, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_LOGIC, __LINE__);
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
				error_printf("invalid translation command: %u, code is broken !\n", command);
				QUEUE_next(queueId, canSid, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_LOGIC, __LINE__);
				return;
			}
	}
	debug_printf("Lift translate motor id %d, pos %d\n", dcMotorId, wantedPosition);
	DCM_goToPos(dcMotorId, wantedPosition);
	DCM_restart(dcMotorId);
}

//Gestion des états en cours d'exécution de la commande de translation
static void LIFT_translation_command_run(queue_id_t queueId) {
	Uint8 canSid = (LIFT_IS_LEFT(queueId))? ACT_LIFT_LEFT : ACT_LIFT_RIGHT;
	Uint8 dcMotorId = (LIFT_IS_LEFT(queueId))? LIFT_LEFT_DCMOTOR_ID : LIFT_RIGHT_DCMOTOR_ID;
	Uint8 result, errorCode;
	Uint16 line;

	debug_printf("Lift translate motor id %d, pos %d\n", dcMotorId, (LIFT_IS_LEFT(queueId))? LIFT_LEFT_getTranslationPos() : LIFT_RIGHT_getTranslationPos());

	if(ACTQ_check_status_dcmotor(dcMotorId, FALSE, &result, &errorCode, &line))
		QUEUE_next(queueId, canSid, result, errorCode, line);
}

//Initialise une commande de la pince à assiette
static void LIFT_plier_command_init(queue_id_t queueId) {
	Uint8 canSid = (LIFT_IS_LEFT(queueId))? ACT_LIFT_LEFT : ACT_LIFT_RIGHT;
	Uint8 ax12Id = (LIFT_IS_LEFT(queueId))? LIFT_LEFT_PLIER_AX12_ID : LIFT_RIGHT_PLIER_AX12_ID;
	Uint16 ax12OpenPos = (LIFT_IS_LEFT(queueId))? LIFT_LEFT_PLIER_AX12_OPEN_POS : LIFT_RIGHT_PLIER_AX12_OPEN_POS;
	Uint16 ax12ClosedPos = (LIFT_IS_LEFT(queueId))? LIFT_LEFT_PLIER_AX12_CLOSED_POS : LIFT_RIGHT_PLIER_AX12_CLOSED_POS;
	Uint8 command = QUEUE_get_arg(queueId)->canCommand;
	Uint16* ax12_goalPosition = &QUEUE_get_arg(queueId)->param;

	*ax12_goalPosition = 0xFFFF;

	switch(command) {
		case ACT_LIFT_PLIER_OPEN:  *ax12_goalPosition = ax12OpenPos; break;
		case ACT_LIFT_PLIER_CLOSE: *ax12_goalPosition = ax12ClosedPos; break;
		case ACT_LIFT_PLIER_STOP:
			AX12_set_torque_enabled(ax12Id, FALSE); //Stopper l'asservissement de l'AX12 qui gère la pince
			QUEUE_next(queueId, canSid, ACT_RESULT_DONE, ACT_RESULT_ERROR_OK, __LINE__);
			return;

		default: {
				error_printf("Invalid plier command: %u, code is broken !\n", command);
				QUEUE_next(queueId, canSid, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_LOGIC, __LINE__);
				return;
			}
	}
	if(*ax12_goalPosition == 0xFFFF) {
		error_printf("Invalid plier position: %u, code is broken !\n", command);
		QUEUE_next(queueId, canSid, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_LOGIC, __LINE__);
		return;
	}

	AX12_reset_last_error(ax12Id); //Sécurité anti terroriste. Nous les parano on aime pas voir des erreurs là ou il n'y en a pas.
	if(!AX12_set_position(ax12Id, *ax12_goalPosition)) {	//Si la commande n'a pas été envoyée correctement et/ou que l'AX12 ne répond pas a cet envoi, on l'indique à la carte stratégie
		error_printf("AX12_set_position error: 0x%x\n", AX12_get_last_error(ax12Id).error);
		QUEUE_next(queueId, canSid, ACT_RESULT_FAILED, ACT_RESULT_ERROR_NOT_HERE, __LINE__);
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
	Uint8 result, errorCode;
	Uint16 line;

	//360° of large_epsilon, quand on a un timeout, on a forcément un resultat Ok (et pas d'erreur, on considère qu'on serre le verre)
	if(ACTQ_check_status_ax12(queueId, ax12Id, QUEUE_get_arg(queueId)->param, posEpsilon, asserTimeout, 360, &result, &errorCode, &line))
		QUEUE_next(queueId, canSid, result, errorCode, line);
}


#endif  /* I_AM_ROBOT_KRUSTY */
