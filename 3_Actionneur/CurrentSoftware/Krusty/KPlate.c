/*  Club Robot ESEO 2012 - 2013
 *	Krusty
 *
 *	Fichier : Plate.c
 *	Package : Carte actionneur
 *	Description : Gestion de l'assiette contenant les cerises
 *  Auteur : Alexis
 *  Version 20130219
 *  Robot : Krusty
 */

#include "KPlate.h"
#ifdef I_AM_ROBOT_KRUSTY

//#include "../QS/QS_can.h"
#include "../QS/QS_CANmsgList.h"
#include "../QS/QS_DCMotor2.h"
#include "../QS/QS_ax12.h"
#include "../QS/QS_adc.h"
#include "../output_log.h"
#include "../Can_msg_processing.h"
#include "KPlate_config.h"

#define LOG_PREFIX "PL: "
#define COMPONENT_log(log_level, format, ...) OUTPUTLOG_printf(OUTPUT_LOG_COMPONENT_PLATE, log_level, LOG_PREFIX format, ## __VA_ARGS__)

#define PLATE_NUM_POS           3
	#define PLATE_HORIZONTAL_POS_ID 0
	#define PLATE_PREPARE_POS_ID    1
	#define PLATE_VERTICAL_POS_ID   2

#define PLATE_PLIER_DONT_SEND_RESULT 1

#if DCMOTOR_NB_POS < PLATE_NUM_POS
#error "Le nombre de position disponible dans l'asservissement DCMotor n'est pas suffisant"
#endif
#if DCM_NUMBER <= PLATE_DCMOTOR_ID
#error "Le nombre de DCMotor disponible n'est pas suffisant, veuillez augmenter DCM_NUMBER"
#endif

static void PLATE_initAX12();
static Sint16 PLATE_getRotationAngle();
static void PLATE_run_command(queue_id_t queueId, bool_e init);
static void PLATE_rotation_command_init(queue_id_t queueId);
static void PLATE_rotation_command_run(queue_id_t queueId);
static void PLATE_plier_command_init(queue_id_t queueId);
static void PLATE_plier_command_run(queue_id_t queueId);

void PLATE_init() {
	DCMotor_config_t plate_rotation_config;
	static bool_e initialized = FALSE;

	if(initialized)
		return;
	initialized = TRUE;

	DCM_init();
	AX12_init();
	ADC_init();

	plate_rotation_config.sensor_read = &PLATE_getRotationAngle;
	plate_rotation_config.Kp = PLATE_ASSER_KP;
	plate_rotation_config.Ki = PLATE_ASSER_KI;
	plate_rotation_config.Kd = PLATE_ASSER_KD;
	plate_rotation_config.pos[PLATE_HORIZONTAL_POS_ID] = PLATE_HORIZONTAL_POS;
	plate_rotation_config.pos[PLATE_PREPARE_POS_ID] = PLATE_PREPARE_POS;
	plate_rotation_config.pos[PLATE_VERTICAL_POS_ID] = PLATE_VERTICAL_POS;
	plate_rotation_config.pwm_number = PLATE_DCMOTOR_PWM_NUM;
	plate_rotation_config.way_latch = &PLATE_DCMOTOR_PORT_WAY;
	plate_rotation_config.way_bit_number = PLATE_DCMOTOR_PORT_WAY_BIT;
	plate_rotation_config.way0_max_duty = PLATE_DCMOTOR_MAX_PWM_WAY0;
	plate_rotation_config.way1_max_duty = PLATE_DCMOTOR_MAX_PWM_WAY1;
	plate_rotation_config.timeout = PLATE_ASSER_TIMEOUT;
	plate_rotation_config.epsilon = PLATE_ASSER_POS_EPSILON;
	DCM_config(PLATE_DCMOTOR_ID, &plate_rotation_config);
	DCM_stop(PLATE_DCMOTOR_ID);

	COMPONENT_log(LOG_LEVEL_Info, "Pince à assiette initialisé (DCMotor)\n");

	PLATE_initAX12();
}

//Initialise l'AX12 de la pince s'il n'était pas allimenté lors d'initialisations précédentes, si déjà initialisé, ne fait rien
static void PLATE_initAX12() {
	static bool_e ax12_is_initialized = FALSE;
	if(ax12_is_initialized == FALSE && AX12_is_ready(PLATE_PLIER_AX12_ID) == TRUE) {
		ax12_is_initialized = TRUE;
		AX12_config_set_highest_voltage(PLATE_PLIER_AX12_ID, 136);
		AX12_config_set_lowest_voltage(PLATE_PLIER_AX12_ID, 70);
		AX12_config_set_maximum_torque_percentage(PLATE_PLIER_AX12_ID, PLATE_PLIER_AX12_MAX_TORQUE_PERCENT);

		AX12_config_set_maximal_angle(PLATE_PLIER_AX12_ID, 300);
		AX12_config_set_minimal_angle(PLATE_PLIER_AX12_ID, 0);

		AX12_config_set_error_before_led(PLATE_PLIER_AX12_ID, AX12_ERROR_ANGLE | AX12_ERROR_CHECKSUM | AX12_ERROR_INSTRUCTION | AX12_ERROR_OVERHEATING | AX12_ERROR_OVERLOAD | AX12_ERROR_RANGE);
		AX12_config_set_error_before_shutdown(PLATE_PLIER_AX12_ID, AX12_ERROR_OVERHEATING); //On ne met pas l'overload comme par defaut, il faut pouvoir tenir l'assiette et sans que l'AX12 ne s'arrête de forcer pour cause de couple resistant trop fort.
		COMPONENT_log(LOG_LEVEL_Info, "Pince AX12 initialisé\n");
	}
}

bool_e PLATE_CAN_process_msg(CAN_msg_t* msg) {
	queue_id_t queueId;

	if(msg->sid == ACT_PLATE) {
		//Initialise l'AX12 de la pince s'il n'était pas allimenté lors d'initialisations précédentes, si déjà initialisé, ne fait rien
		PLATE_initAX12(); 

		switch(msg->data[0]) {
			case ACT_PLATE_ROTATE_HORIZONTALLY:
			case ACT_PLATE_ROTATE_PREPARE:
			case ACT_PLATE_ROTATE_STOP:
			//case ACT_PLATE_ROTATE_VERTICALLY:
				CAN_push_operation_from_msg(msg, QUEUE_ACT_Plate_Rotation, &PLATE_run_command, 0);
				break;

				//FIXME: decommenter le code gérant l'AX12 avec ACT_PLATE_ROTATE_VERTICALLY
			case ACT_PLATE_ROTATE_VERTICALLY:    //Cas spécial: fermer la pince avant de tourner en vertical (sinon ça ne rentrera pas dans le robot)
				queueId = QUEUE_create();
				assert(queueId != QUEUE_CREATE_FAILED);
				if(queueId != QUEUE_CREATE_FAILED) {
					QUEUE_add(queueId, &QUEUE_take_sem, (QUEUE_arg_t){0, 0}, QUEUE_ACT_Plate_AX12_Plier);
					QUEUE_add(queueId, &QUEUE_take_sem, (QUEUE_arg_t){0, 0}, QUEUE_ACT_Plate_Rotation);
					QUEUE_add(queueId, &PLATE_run_command, (QUEUE_arg_t){ACT_PLATE_PLIER_CLOSE, PLATE_PLIER_DONT_SEND_RESULT}, QUEUE_ACT_Plate_AX12_Plier);
					QUEUE_add(queueId, &PLATE_run_command, (QUEUE_arg_t){msg->data[0], 0}, QUEUE_ACT_Plate_Rotation);
					QUEUE_add(queueId, &QUEUE_give_sem, (QUEUE_arg_t){0, 0}, QUEUE_ACT_Plate_Rotation);
					QUEUE_add(queueId, &QUEUE_give_sem, (QUEUE_arg_t){0, 0}, QUEUE_ACT_Plate_AX12_Plier);
				} else {	//on indique qu'on a pas géré la commande
					CAN_sendResultWithLine(msg->sid, msg->data[0], ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_NO_RESOURCES);
				}
				break;

			case ACT_PLATE_PLIER_CLOSE:
			case ACT_PLATE_PLIER_OPEN:
			case ACT_PLATE_PLIER_STOP:
				CAN_push_operation_from_msg(msg, QUEUE_ACT_Plate_AX12_Plier, &PLATE_run_command, 0);
				break;

			default:
				COMPONENT_log(LOG_LEVEL_Warning, "invalid CAN msg data[0]=%u !\n", msg->data[0]);
		}
		return TRUE;
	}

	return FALSE;
}

static Sint16 PLATE_getRotationAngle() {
	return ADC_getValue(PLATE_ROTATION_POTAR_ADC_ID);
}

static void PLATE_run_command(queue_id_t queueId, bool_e init) {
	if(QUEUE_get_act(queueId) == QUEUE_ACT_Plate_Rotation) {    // Gestion des mouvements de rotation de l'assiette
		if(init && !QUEUE_has_error(queueId))
			PLATE_rotation_command_init(queueId);
		else
			PLATE_rotation_command_run(queueId);
	} else if(QUEUE_get_act(queueId) == QUEUE_ACT_Plate_AX12_Plier) {    // Gestion des mouvements de la pince à assiette
		if(init && !QUEUE_has_error(queueId))
			PLATE_plier_command_init(queueId);
		else
			PLATE_plier_command_run(queueId);
	}
}

//Initialisation d'une commande de rotation du bras
static void PLATE_rotation_command_init(queue_id_t queueId) {
	Uint8 command = QUEUE_get_arg(queueId)->canCommand;
	Uint8 wantedPosition;
	switch(command) {
		case ACT_PLATE_ROTATE_HORIZONTALLY: wantedPosition = PLATE_HORIZONTAL_POS_ID; break;
		case ACT_PLATE_ROTATE_PREPARE:      wantedPosition = PLATE_PREPARE_POS_ID;    break;
		case ACT_PLATE_ROTATE_VERTICALLY:   wantedPosition = PLATE_VERTICAL_POS_ID;   break;
		case ACT_PLATE_ROTATE_STOP:
			DCM_stop(PLATE_DCMOTOR_ID);
			//Le moteur atteindra l'état IDLE automatiquement et le message de retour sera envoyé, voir PLATE_rotation_command_run
			return;

		default: {
//				CAN_msg_t resultMsg = {ACT_RESULT, {ACT_PLATE & 0xFF, command, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_LOGIC}, 4};
//				CAN_send(&resultMsg);
				CAN_sendResultWithLine(ACT_PLATE, command, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_LOGIC);
				COMPONENT_log(LOG_LEVEL_Error, "Invalid rotation command: %u, code is broken !\n", command);
				QUEUE_set_error(queueId);
				QUEUE_behead(queueId);
				return;
			}
	}
	COMPONENT_log(LOG_LEVEL_Debug, "Rotation at position %d\n", DCM_getPosValue(PLATE_DCMOTOR_ID, wantedPosition));
	DCM_goToPos(PLATE_DCMOTOR_ID, wantedPosition);
	DCM_restart(PLATE_DCMOTOR_ID);
}

//Gestion des états en cours d'exécution de la commande de rotation
static void PLATE_rotation_command_run(queue_id_t queueId) {
	DCM_working_state_e asserState = DCM_get_state(PLATE_DCMOTOR_ID);
//	CAN_msg_t resultMsg;
	Uint8 result, errorCode;

	COMPONENT_log(LOG_LEVEL_Trace, "At position %d\n", PLATE_getRotationAngle());

	if(QUEUE_has_error(queueId)) {
		result =    ACT_RESULT_NOT_HANDLED;
		errorCode = ACT_RESULT_ERROR_OTHER;
	} else if(asserState == DCM_IDLE) {
		result =    ACT_RESULT_DONE;
		errorCode = ACT_RESULT_ERROR_OK;
	} else if(asserState == DCM_TIMEOUT) {
		result =    ACT_RESULT_FAILED;
		errorCode = ACT_RESULT_ERROR_TIMEOUT;
		QUEUE_set_error(queueId);
	} else return;	//Operation is not finished, do nothing

//	resultMsg.sid = ACT_RESULT;
//	resultMsg.data[0] = ACT_PLATE & 0xFF;
//	resultMsg.data[1] = QUEUE_get_arg(queueId)->canCommand;
//	resultMsg.size = 4;
//
//	CAN_send(&resultMsg);

	CAN_sendResultWithLine(ACT_PLATE, QUEUE_get_arg(queueId)->canCommand, result, errorCode);
	QUEUE_behead(queueId);	//gestion terminée
}

//Initialise une commande de la pince à assiette
static void PLATE_plier_command_init(queue_id_t queueId) {
	Uint8 command = QUEUE_get_arg(queueId)->canCommand;
	bool_e cmdOk;
	Uint16* ax12_goalPosition = &QUEUE_get_arg(queueId)->param;

	*ax12_goalPosition = 0xFFFF;

	switch(command) {
		case ACT_PLATE_PLIER_OPEN:  *ax12_goalPosition = PLATE_PLIER_AX12_OPEN_POS; break;
		case ACT_PLATE_PLIER_CLOSE: *ax12_goalPosition = PLATE_PLIER_AX12_CLOSED_POS; break;
		case ACT_PLATE_PLIER_STOP:
			AX12_set_torque_enabled(PLATE_PLIER_AX12_ID, FALSE); //Stopper l'asservissement de l'AX12 qui gère la pince
//			CAN_msg_t resultMsg = {ACT_RESULT, {ACT_PLATE & 0xFF, command, ACT_RESULT_DONE, ACT_RESULT_ERROR_OK}, 4};
//			CAN_send(&resultMsg);
			CAN_sendResultWithLine(ACT_PLATE, command, ACT_RESULT_DONE, ACT_RESULT_ERROR_OK);
			QUEUE_behead(queueId);
			return;

		default: {
//				CAN_msg_t resultMsg = {ACT_RESULT, {ACT_PLATE & 0xFF, command, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_LOGIC}, 4};
//				CAN_send(&resultMsg);
				CAN_sendResultWithLine(ACT_PLATE, command, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_LOGIC);
				COMPONENT_log(LOG_LEVEL_Error, "Invalid plier command: %u, code is broken !\n", command);
				QUEUE_set_error(queueId);
				QUEUE_behead(queueId);
				return;
			}
	}
	if(*ax12_goalPosition == 0xFFFF) {
		COMPONENT_log(LOG_LEVEL_Error, "Invalid plier position: %u, code is broken !\n", command);
		CAN_sendResultWithLine(ACT_PLATE, command, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_LOGIC);
		QUEUE_set_error(queueId);
		QUEUE_behead(queueId);
		return;
	}

	COMPONENT_log(LOG_LEVEL_Debug, "Move plier ax12 to %d\n", *ax12_goalPosition);
	AX12_reset_last_error(PLATE_PLIER_AX12_ID); //Sécurité anti terroriste. Nous les parano on aime pas voir des erreurs là ou il n'y en a pas.
	cmdOk = AX12_set_position(PLATE_PLIER_AX12_ID, *ax12_goalPosition);
	if(!cmdOk) {	//Si la commande n'a pas été envoyée correctement et/ou que l'AX12 ne répond pas a cet envoi, on l'indique à la carte stratégie
//		CAN_msg_t resultMsg = {ACT_RESULT, {ACT_PLATE & 0xFF, command, ACT_RESULT_FAILED, ACT_RESULT_ERROR_NOT_HERE}, 4};
//		CAN_send(&resultMsg);
		CAN_sendResultWithLine(ACT_PLATE, command, ACT_RESULT_FAILED, ACT_RESULT_ERROR_NOT_HERE);
		COMPONENT_log(LOG_LEVEL_Error, "AX12_set_position error: 0x%x\n", AX12_get_last_error(PLATE_PLIER_AX12_ID).error);
		QUEUE_set_error(queueId);
		QUEUE_behead(queueId);
		return;
	}
	//La commande a été envoyée et l'AX12 l'a bien reçu
}

//Gère les états pendant le mouvement de la pince.
static void PLATE_plier_command_run(queue_id_t queueId) {
	Uint8 error;
	Uint16 ax12Pos;
//	CAN_msg_t resultMsg;
	Uint8 result, errorCode;
	Uint16* ax12_goalPosition = &QUEUE_get_arg(queueId)->param;
	Uint16 line;

	//AX12_reset_last_error(PLATE_PLIER_AX12_ID);

	//FIXME: commentaire debuggage, a enlever
	ax12Pos = AX12_get_position(PLATE_PLIER_AX12_ID); //même si non utilisé, permet de faire un ping en même temps. S'il n'est plus là (parce que kingkong l'a kidnappé par exemple) il ne répondra plus.
	error = AX12_get_last_error(PLATE_PLIER_AX12_ID).error;
	if(error)
		COMPONENT_log(LOG_LEVEL_Error, "AX12_get_position error: 0x%x\n", error);

	if(QUEUE_has_error(queueId)) {
		result =    ACT_RESULT_NOT_HANDLED;
		errorCode = ACT_RESULT_ERROR_OTHER;
		AX12_set_torque_enabled(PLATE_PLIER_AX12_ID, FALSE);
		line = __LINE__;
	} else if((error & AX12_ERROR_OVERLOAD) || (abs((Sint16)ax12Pos - (Sint16)(*ax12_goalPosition)) <= PLATE_PLIER_AX12_ASSER_POS_EPSILON)) {	//Fin du mouvement
	//if(AX12_is_moving(PLATE_PLIER_AX12_ID) == FALSE) {  //Fin du mouvement
		result =    ACT_RESULT_DONE;
		errorCode = ACT_RESULT_ERROR_OK;
		line = __LINE__;
	} else if((error & AX12_ERROR_TIMEOUT) && (error & AX12_ERROR_RANGE)) {
		result =    ACT_RESULT_NOT_HANDLED;
		errorCode = ACT_RESULT_ERROR_LOGIC;	//Si le driver a attendu trop longtemps, c'est a cause d'un deadlock plutot qu'un manque de ressources (il attend suffisament longtemps pour que les commandes soit bien envoyées)
		AX12_set_torque_enabled(PLATE_PLIER_AX12_ID, FALSE);
		QUEUE_set_error(queueId);
		line = __LINE__;
	} else if(error & AX12_ERROR_TIMEOUT) {	//L'ax12 n'a pas répondu à la commande
		result =    ACT_RESULT_FAILED;
		errorCode = ACT_RESULT_ERROR_NOT_HERE;
		AX12_set_torque_enabled(PLATE_PLIER_AX12_ID, FALSE);
		QUEUE_set_error(queueId);
		line = __LINE__;
	} else if(CLOCK_get_time() >= QUEUE_get_initial_time(queueId) + PLATE_PLIER_AX12_ASSER_TIMEOUT) {    //Timeout, l'ax12 n'a pas bouger à la bonne position a temps
//		result =    ACT_RESULT_FAILED;
//		errorCode = ACT_RESULT_ERROR_UNKNOWN;
		//Si on ne peut pas atteindre la position alors que l'ax12 à bien compris la commande, on considère qu'une assiette bloque l'ax12 -> tout est Ok
		result =    ACT_RESULT_DONE;
		errorCode = ACT_RESULT_ERROR_OK;
		//AX12_set_torque_enabled(PLATE_PLIER_AX12_ID, FALSE);
		//QUEUE_set_error(queueId);
		line = __LINE__;
	} else if(error) {							//autres erreurs (sans compter l'overload si on force sur la pince pour serrer l'assiette)
		result =   ACT_RESULT_FAILED;
		errorCode = ACT_RESULT_ERROR_UNKNOWN;
		AX12_set_torque_enabled(PLATE_PLIER_AX12_ID, FALSE);
		QUEUE_set_error(queueId);
		line = error;
	} else return;	//Operation is not finished, do nothing

//	resultMsg.sid = ACT_RESULT;
//	resultMsg.data[0] = ACT_PLATE & 0xFF;
//	resultMsg.data[1] = QUEUE_get_arg(queueId)->canCommand;
//	resultMsg.size = 4;
//
//	CAN_send(&resultMsg);
	if(QUEUE_get_arg(queueId)->param != PLATE_PLIER_DONT_SEND_RESULT)
		CAN_sendResultWithParam(ACT_PLATE, QUEUE_get_arg(queueId)->canCommand, result, errorCode, line);
	QUEUE_behead(queueId);	//gestion terminée
}


#endif  /* I_AM_ROBOT_KRUSTY */
