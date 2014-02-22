/*  Club Robot ESEO 2013 - 2014
 *	BIG
 *
 *	Fichier : Fruit.c
 *	Package : Carte actionneur
 *	Description : Gestion des fruit_mouths
 *  Auteur : Amaury
 *  Version 20130219
 *  Robot : BIG
 */

#include "PFruit.h"
#ifdef I_AM_ROBOT_BIG

//#include "../QS/QS_can.h"
#include "../QS/QS_CANmsgList.h"
#include "../QS/QS_ax12.h"
#include "../QS/QS_pwm.h"
#include "../QS/QS_ports.h"
#include "../act_queue_utils.h"
#include "../selftest.h"
#include "config_pin.h"
#include "PFruit_config.h"

#include "config_debug.h"
#define LOG_PREFIX "FR: "
#define LOG_COMPONENT OUTPUT_LOG_COMPONENT_FRUIT
#include "../QS/QS_outputlog.h"


static void FRUIT_initAX12();
static void FRUIT_initDCM();
static void FRUIT_command_init(queue_id_t queueId);
static void FRUIT_command_run(queue_id_t queueId);
static void FRUIT_command_pompe_run(queue_id_t queueId);
static void POMPE_goToPos(Uint8 command);

static bool_e stopActVerin = FALSE;

void FRUIT_init() {
	static bool_e initialized = FALSE;

	if(initialized)
		return;
	initialized = TRUE;

	AX12_init();
	FRUIT_initDCM();
	FRUIT_initAX12();
	info_printf("FRUIT_MOUTH initialisé\n");
}

// Initilastion du moteur, si init ne fait rien
static void FRUIT_initDCM() {
	static bool_e dcm_is_initialized = FALSE;

	if(dcm_is_initialized == FALSE ) {
		PORTS_pwm_init();
		PWM_stop(FRUIT_POMPE_PWM_NUM);

		info_printf("VERIN FRUIT initialisé (pompe) \n");
	}
}

//Initialise l'AX12 de la pince s'il n'était pas allimenté lors d'initialisations précédentes, si déjà initialisé, ne fait rien
static void FRUIT_initAX12() {
	static bool_e ax12_is_initialized = FALSE;

	if(ax12_is_initialized == FALSE && AX12_is_ready(FRUIT_LABIUM_AX12_ID) == TRUE) {
		ax12_is_initialized = TRUE;
		AX12_config_set_highest_voltage(FRUIT_LABIUM_AX12_ID, 136);
		AX12_config_set_lowest_voltage(FRUIT_LABIUM_AX12_ID, 70);
		AX12_config_set_maximum_torque_percentage(FRUIT_LABIUM_AX12_ID, FRUIT_AX12_MAX_TORQUE_PERCENT);

		AX12_config_set_maximal_angle(FRUIT_LABIUM_AX12_ID, 300);
		AX12_config_set_minimal_angle(FRUIT_LABIUM_AX12_ID, 0);

		AX12_config_set_error_before_led(FRUIT_LABIUM_AX12_ID, AX12_ERROR_ANGLE | AX12_ERROR_CHECKSUM | AX12_ERROR_INSTRUCTION | AX12_ERROR_OVERHEATING | AX12_ERROR_OVERLOAD | AX12_ERROR_RANGE);
		AX12_config_set_error_before_shutdown(FRUIT_LABIUM_AX12_ID, AX12_ERROR_OVERHEATING); //On ne met pas l'overload comme par defaut, il faut pouvoir tenir l'assiette et sans que l'AX12 ne s'arrête de forcer pour cause de couple resistant trop fort.

		debug_printf("FRUIT_LABIUM AX12 initialisé\n");
	}
}

void FRUIT_stop() {
	PWM_stop(FRUIT_POMPE_PWM_NUM);
}

void FRUIT_init_pos(){
	FRUIT_initAX12();

	if(!AX12_set_position(FRUIT_LABIUM_AX12_ID, FRUIT_AX12_LABIUM_INIT_POS))
		debug_printf("L'AX12 n°%d n'est pas la", FRUIT_LABIUM_AX12_ID);
}

bool_e FRUIT_CAN_process_msg(CAN_msg_t* msg) {
	queue_id_t queueId1;
	FRUIT_initAX12();
	if(msg->sid == ACT_FRUIT_MOUTH) {
		switch(msg->data[0]) {


			/******** POMPE ***********/


			case ACT_FRUIT_MOUTH_CANCELED:
				stopActVerin = TRUE;
				break;

				//Pour les 2 cas (open ou close), la différence est faite dans FRUIT_command_init
			case ACT_FRUIT_MOUTH_CLOSE:
			case ACT_FRUIT_MOUTH_OPEN:
				ACTQ_push_operation_from_msg(msg, QUEUE_ACT_POMPE_Fruit, &FRUIT_run_command, 0);
				break;


			/******** AX12 LABIUM ***********/


			case ACT_FRUIT_LABIUM_CLOSE:
				queueId1 = QUEUE_create();
				if(queueId1 != QUEUE_CREATE_FAILED){
					QUEUE_add(queueId1, &QUEUE_take_sem, (QUEUE_arg_t){0, 0, NULL}, QUEUE_ACT_AX12_Fruit);
					QUEUE_add(queueId1, &FRUIT_run_command, (QUEUE_arg_t){msg->data[0], FRUIT_LABIUM_CS_CloseAX12,  &ACTQ_finish_SendResult}, QUEUE_ACT_AX12_Fruit);
					QUEUE_add(queueId1, &QUEUE_give_sem, (QUEUE_arg_t){0, 0, NULL}, QUEUE_ACT_AX12_Fruit);
				} else {
					QUEUE_flush(queueId1);
					ACTQ_sendResultWithLine(msg->sid, msg->data[0], ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_NO_RESOURCES);
				}
				break;
			case ACT_FRUIT_LABIUM_OPEN:
				queueId1 = QUEUE_create();
				if(queueId1 != QUEUE_CREATE_FAILED){
					QUEUE_add(queueId1, &QUEUE_take_sem, (QUEUE_arg_t){0, 0, NULL}, QUEUE_ACT_AX12_Fruit);
					QUEUE_add(queueId1, &FRUIT_run_command, (QUEUE_arg_t){msg->data[0], FRUIT_LABIUM_CS_OpenAX12,  &ACTQ_finish_SendResult}, QUEUE_ACT_AX12_Fruit);
					QUEUE_add(queueId1, &QUEUE_give_sem, (QUEUE_arg_t){0, 0, NULL}, QUEUE_ACT_AX12_Fruit);
				} else {
					QUEUE_flush(queueId1);
					ACTQ_sendResultWithLine(msg->sid, msg->data[0], ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_NO_RESOURCES);
				}
				break;
			default:
				component_printf(LOG_LEVEL_Warning, "invalid CAN msg data[0]=%u !\n", msg->data[0]);
		}
		return TRUE;
	}else if(msg->sid == ACT_DO_SELFTEST) {
		SELFTEST_set_actions(&FRUIT_run_command, 2, (SELFTEST_action_t[]){
								 {ACT_FRUIT_LABIUM_OPEN,  FRUIT_LABIUM_CS_OpenAX12,  QUEUE_ACT_AX12_Fruit},
								 {ACT_FRUIT_LABIUM_CLOSE, FRUIT_LABIUM_CS_CloseAX12, QUEUE_ACT_AX12_Fruit}
							 });
	}

	return FALSE;
}

void FRUIT_run_command(queue_id_t queueId, bool_e init) {
	if(QUEUE_has_error(queueId)) {
		QUEUE_behead(queueId);
		return;
	}

	if(QUEUE_get_act(queueId) == QUEUE_ACT_AX12_Fruit) {    // Gestion des mouvements du labium
			if(init)
				FRUIT_command_init(queueId);
			else
				FRUIT_command_run(queueId);
	}

	if(QUEUE_get_act(queueId) == QUEUE_ACT_POMPE_Fruit) {

		if(init)
			FRUIT_command_init(queueId);
		else
			FRUIT_command_pompe_run(queueId);
	}
}

//Initialise une commande
static void FRUIT_command_init(queue_id_t queueId) {
	Uint8 command = QUEUE_get_arg(queueId)->canCommand;
	Uint16* ax12_goalPosition = &QUEUE_get_arg(queueId)->param;
	Uint16 wantedPositionPompe;
	*ax12_goalPosition = 0xFFFF;

	bool_e forFruitMouth = FALSE;

	switch(command) {
		case ACT_FRUIT_MOUTH_OPEN: wantedPositionPompe = ACT_FRUIT_MOUTH_OPEN; forFruitMouth = TRUE; break;
		case ACT_FRUIT_MOUTH_CLOSE: wantedPositionPompe = ACT_FRUIT_MOUTH_CLOSE; forFruitMouth = TRUE; break;

		case ACT_FRUIT_MOUTH_STOP:
			PWM_stop(FRUIT_POMPE_PWM_NUM);
			return;

		case ACT_FRUIT_LABIUM_OPEN: *ax12_goalPosition = FRUIT_AX12_LABIUM_OPEN_POS; forFruitMouth = FALSE; break;
		case ACT_FRUIT_LABIUM_CLOSE: *ax12_goalPosition = FRUIT_AX12_LABIUM_CLOSE_POS; forFruitMouth = FALSE; break;
		case ACT_FRUIT_LABIUM_STOP:
			AX12_set_torque_enabled(FRUIT_LABIUM_AX12_ID, FALSE);
			QUEUE_next(queueId, ACT_FRUIT_MOUTH, ACT_RESULT_DONE, ACT_RESULT_ERROR_OK, __LINE__);
			return;

		default: {
				error_printf("Invalid plier command: %u, code is broken !\n", command);
				QUEUE_next(queueId, ACT_FRUIT_MOUTH, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_LOGIC, __LINE__);
				return;
			}
	}

	if(QUEUE_get_act(queueId) == QUEUE_ACT_POMPE_Fruit)
		POMPE_goToPos(wantedPositionPompe);
	else if(*ax12_goalPosition == 0xFFFF && (command == ACT_FRUIT_LABIUM_OPEN || command == ACT_FRUIT_LABIUM_CLOSE || command == ACT_FRUIT_LABIUM_STOP)){// Pour l'ax12 Labium
		error_printf("Invalid plier position: %u, code is broken !\n", command);
		QUEUE_next(queueId, ACT_FRUIT_MOUTH, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_LOGIC, __LINE__);
		return;
	}


	if(forFruitMouth == FALSE){
		debug_printf("Move fruit ax12 to %d\n", *ax12_goalPosition);

		AX12_reset_last_error(FRUIT_LABIUM_AX12_ID);
		if(!AX12_set_position(FRUIT_LABIUM_AX12_ID, *ax12_goalPosition)){
			error_printf("AX12_set_position error: 0x%x\n", AX12_get_last_error(FRUIT_LABIUM_AX12_ID).error);
			QUEUE_next(queueId, ACT_FRUIT_MOUTH, ACT_RESULT_FAILED, ACT_RESULT_ERROR_NOT_HERE, __LINE__);
		}
	}
	//La commande a été envoyée et l'AX12 l'a bien reçu
}

// La pompe en déplacement vérification de son état
static void FRUIT_command_pompe_run(queue_id_t queueId){
	Uint8 command = QUEUE_get_arg(queueId)->canCommand;

	if((FRUIT_POMPE_TOR_OPEN == 0 && command == ACT_FRUIT_MOUTH_OPEN) || (FRUIT_POMPE_TOR_CLOSE == 0 && command == ACT_FRUIT_MOUTH_CLOSE) || stopActVerin == TRUE){ // le verin atteint la postion du capteur de sortie, on arrete
		stopActVerin = FALSE;

		PWM_stop(FRUIT_POMPE_PWM_NUM);

		if(stopActVerin == TRUE)
			QUEUE_next(queueId, ACT_FRUIT_MOUTH, ACT_RESULT_FAILED, ACT_RESULT_ERROR_CANCELED, __LINE__);
		else
			QUEUE_next(queueId, ACT_FRUIT_MOUTH, ACT_RESULT_DONE, ACT_RESULT_ERROR_OK, __LINE__);
	}
}

static void POMPE_goToPos(Uint8 command){
	if(command == ACT_FRUIT_MOUTH_OPEN)
		FRUIT_POMPE_SENS = 1;
	else if(command == ACT_FRUIT_MOUTH_CLOSE)
		FRUIT_POMPE_SENS = 0;

	PWM_run(FRUIT_POMPE_MAX_PWM_WAY, FRUIT_POMPE_PWM_NUM);
}

//Gère les états pendant le mouvement de la pince.
static void FRUIT_command_run(queue_id_t queueId) {
	Uint8 result, errorCode;
	Uint16 line;

	if(ACTQ_check_status_ax12(queueId, FRUIT_LABIUM_AX12_ID, QUEUE_get_arg(queueId)->param, FRUIT_AX12_ASSER_POS_EPSILON, FRUIT_AX12_ASSER_TIMEOUT, 360, &result, &errorCode, &line))
		QUEUE_next(queueId, ACT_FRUIT_MOUTH, result, errorCode, line);
}

#endif  /* I_AM_ROBOT_BIG */
