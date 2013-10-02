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
#include "../act_queue_utils.h"
#include "KPlate_config.h"

#define COMPONENT_log(...) OUTPUTLOG_PRETTY("PL: ", OUTPUT_LOG_COMPONENT_PLATE, ## __VA_ARGS__)

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

		AX12_set_position(PLATE_PLIER_AX12_ID, PLATE_ACT_PLIER_AX12_INIT_POS);
		COMPONENT_log(LOG_LEVEL_Info, "Pince AX12 initialisé\n");
	}
}

void PLATE_stop() {
	DCM_stop(PLATE_DCMOTOR_ID);
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
				ACTQ_push_operation_from_msg(msg, QUEUE_ACT_Plate_Rotation, &PLATE_run_command, 0);
				break;

			case ACT_PLATE_ROTATE_VERTICALLY:    //Cas spécial: fermer la pince avant de tourner en vertical (sinon ça ne rentrera pas dans le robot)
				queueId = QUEUE_create();
				assert(queueId != QUEUE_CREATE_FAILED);
				if(queueId != QUEUE_CREATE_FAILED) {
					QUEUE_add(queueId, &QUEUE_take_sem, (QUEUE_arg_t){0, 0, NULL}, QUEUE_ACT_Plate_AX12_Plier);
					QUEUE_add(queueId, &QUEUE_take_sem, (QUEUE_arg_t){0, 0, NULL}, QUEUE_ACT_Plate_Rotation);
					QUEUE_add(queueId, &PLATE_run_command, (QUEUE_arg_t){ACT_PLATE_PLIER_CLOSE, 0, &ACTQ_finish_SendResultIfFail}, QUEUE_ACT_Plate_AX12_Plier);
					QUEUE_add(queueId, &PLATE_run_command, (QUEUE_arg_t){msg->data[0]         , 0, &ACTQ_finish_SendResult}      , QUEUE_ACT_Plate_Rotation);
					QUEUE_add(queueId, &QUEUE_give_sem, (QUEUE_arg_t){0, 0, NULL}, QUEUE_ACT_Plate_Rotation);
					QUEUE_add(queueId, &QUEUE_give_sem, (QUEUE_arg_t){0, 0, NULL}, QUEUE_ACT_Plate_AX12_Plier);
				} else {	//on indique qu'on a pas géré la commande
					ACTQ_sendResultWithLine(msg->sid, msg->data[0], ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_NO_RESOURCES);
				}
				break;

			case ACT_PLATE_PLIER_CLOSE:
			case ACT_PLATE_PLIER_OPEN:
			case ACT_PLATE_PLIER_STOP:
				ACTQ_push_operation_from_msg(msg, QUEUE_ACT_Plate_AX12_Plier, &PLATE_run_command, 0);
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

void PLATE_run_command(queue_id_t queueId, bool_e init) {
	if(QUEUE_has_error(queueId)) {
		QUEUE_behead(queueId);
		return;
	}

	if(QUEUE_get_act(queueId) == QUEUE_ACT_Plate_Rotation) {    // Gestion des mouvements de rotation de l'assiette
		if(init)
			PLATE_rotation_command_init(queueId);
		else
			PLATE_rotation_command_run(queueId);
	} else if(QUEUE_get_act(queueId) == QUEUE_ACT_Plate_AX12_Plier) {    // Gestion des mouvements de la pince à assiette
		if(init)
			PLATE_plier_command_init(queueId);
		else
			PLATE_plier_command_run(queueId);
	}
}

//Initialisation d'une commande de rotation du bras
static void PLATE_rotation_command_init(queue_id_t queueId) {
	Uint8 wantedPosition;
	Uint8 command = QUEUE_get_arg(queueId)->canCommand;
	switch(command) {
		case ACT_PLATE_ROTATE_HORIZONTALLY:
			wantedPosition = PLATE_HORIZONTAL_POS_ID;
			//Coefs différents pour la descente car on sinon on tape trop fort le sol a cause du poids de l'assiette entre autres
			DCM_setCoefs(PLATE_DCMOTOR_ID, PLATE_ASSER_AMORTIZED_KP, PLATE_ASSER_AMORTIZED_KI, PLATE_ASSER_AMORTIZED_KD);
			break;

		case ACT_PLATE_ROTATE_PREPARE:
			wantedPosition = PLATE_PREPARE_POS_ID;
			DCM_setCoefs(PLATE_DCMOTOR_ID, PLATE_ASSER_KP, PLATE_ASSER_KI, PLATE_ASSER_KD);
			break;

		case ACT_PLATE_ROTATE_VERTICALLY:
			wantedPosition = PLATE_VERTICAL_POS_ID;
			DCM_setCoefs(PLATE_DCMOTOR_ID, PLATE_ASSER_KP, PLATE_ASSER_KI, PLATE_ASSER_KD);
			break;

		case ACT_PLATE_ROTATE_STOP:
			DCM_stop(PLATE_DCMOTOR_ID);
			//Le moteur atteindra l'état IDLE automatiquement et le message de retour sera envoyé, voir PLATE_rotation_command_run
			return;

		default: {
				COMPONENT_log(LOG_LEVEL_Error, "Invalid rotation command: %u, code is broken !\n", command);
				QUEUE_next(queueId, ACT_PLATE, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_LOGIC, __LINE__);
				return;
			}
	}
	COMPONENT_log(LOG_LEVEL_Debug, "Rotation at position %d\n", DCM_getPosValue(PLATE_DCMOTOR_ID, wantedPosition));
	DCM_goToPos(PLATE_DCMOTOR_ID, wantedPosition);
	DCM_restart(PLATE_DCMOTOR_ID);
}

//Gestion des états en cours d'exécution de la commande de rotation
static void PLATE_rotation_command_run(queue_id_t queueId) {
	Uint8 result, errorCode;
	Uint16 line;

	COMPONENT_log(LOG_LEVEL_Trace, "At position %d\n", PLATE_getRotationAngle());

	if(ACTQ_check_status_dcmotor(PLATE_DCMOTOR_ID, FALSE, &result, &errorCode, &line))
		QUEUE_next(queueId, ACT_PLATE, result, errorCode, line);
}

//Initialise une commande de la pince à assiette
static void PLATE_plier_command_init(queue_id_t queueId) {
	Uint8 command = QUEUE_get_arg(queueId)->canCommand;
	Uint16* ax12_goalPosition = &QUEUE_get_arg(queueId)->param;

	*ax12_goalPosition = 0xFFFF;

	switch(command) {
		case ACT_PLATE_PLIER_OPEN:  *ax12_goalPosition = PLATE_PLIER_AX12_OPEN_POS; break;
		case ACT_PLATE_PLIER_CLOSE: *ax12_goalPosition = PLATE_PLIER_AX12_CLOSED_POS; break;
		case ACT_PLATE_PLIER_STOP:
			AX12_set_torque_enabled(PLATE_PLIER_AX12_ID, FALSE); //Stopper l'asservissement de l'AX12 qui gère la pince
			QUEUE_next(queueId, ACT_PLATE, ACT_RESULT_DONE, ACT_RESULT_ERROR_OK, __LINE__);
			return;

		default: {
				COMPONENT_log(LOG_LEVEL_Error, "Invalid plier command: %u, code is broken !\n", command);
				QUEUE_next(queueId, ACT_PLATE, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_LOGIC, __LINE__);
				return;
			}
	}
	if(*ax12_goalPosition == 0xFFFF) {
		COMPONENT_log(LOG_LEVEL_Error, "Invalid plier position: %u, code is broken !\n", command);
		QUEUE_next(queueId, ACT_PLATE, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_LOGIC, __LINE__);
		return;
	}

	COMPONENT_log(LOG_LEVEL_Debug, "Move plier ax12 to %d\n", *ax12_goalPosition);
	AX12_reset_last_error(PLATE_PLIER_AX12_ID); //Sécurité anti terroriste. Nous les parano on aime pas voir des erreurs là ou il n'y en a pas.
	if(!AX12_set_position(PLATE_PLIER_AX12_ID, *ax12_goalPosition)) {	//Si la commande n'a pas été envoyée correctement et/ou que l'AX12 ne répond pas a cet envoi, on l'indique à la carte stratégie
		COMPONENT_log(LOG_LEVEL_Error, "AX12_set_position error: 0x%x\n", AX12_get_last_error(PLATE_PLIER_AX12_ID).error);
		QUEUE_next(queueId, ACT_PLATE, ACT_RESULT_FAILED, ACT_RESULT_ERROR_NOT_HERE, __LINE__);
		return;
	}
	//La commande a été envoyée et l'AX12 l'a bien reçu
}

//Gère les états pendant le mouvement de la pince.
static void PLATE_plier_command_run(queue_id_t queueId) {
	Uint8 result, errorCode;
	Uint16 line;

	//360° of large_epsilon, quand on a un timeout, on a forcément un resultat Ok (et pas d'erreur, on considère qu'on serre l'assiette)
	if(ACTQ_check_status_ax12(queueId, PLATE_PLIER_AX12_ID, QUEUE_get_arg(queueId)->param, PLATE_PLIER_AX12_ASSER_POS_EPSILON, PLATE_PLIER_AX12_ASSER_TIMEOUT, 360, &result, &errorCode, &line))
		QUEUE_next(queueId, ACT_PLATE, result, errorCode, line);
}


#endif  /* I_AM_ROBOT_KRUSTY */
