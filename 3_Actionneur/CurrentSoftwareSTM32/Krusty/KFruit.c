/*  Club Robot ESEO 2013 - 2014
 *	Krusty
 *
 *	Fichier : Fruit.c
 *	Package : Carte actionneur
 *	Description : Gestion des fruit_mouths
 *  Auteur : Amaury
 *  Version 20130219
 *  Robot : Krusty
 */

#include "KFruit.h"
#ifdef I_AM_ROBOT_KRUSTY

//#include "../QS/QS_can.h"
#include "../QS/QS_CANmsgList.h"
#include "../QS/QS_ax12.h"
#include "../act_queue_utils.h"
#include "config_pin.h"
#include "KFruit_config.h"

#include "config_debug.h"
#define LOG_PREFIX "FR: "
#define LOG_COMPONENT OUTPUT_LOG_COMPONENT_FRUIT
#include "../QS/QS_outputlog.h"


static void FRUIT_initAX12();
void FRUIT_stop();
static void FRUIT_command_init(queue_id_t queueId);
static void FRUIT_command_run(queue_id_t queueId);

void FRUIT_init() {
	static bool_e initialized = FALSE;

	if(initialized)
		return;
	initialized = TRUE;

	AX12_init();
	FRUIT_initAX12();
	info_printf("FRUIT_MOUTH initialisé\n");
}

//Initialise l'AX12 de la pince s'il n'était pas allimenté lors d'initialisations précédentes, si déjà initialisé, ne fait rien
static void FRUIT_initAX12() {
	static bool_e ax12_is_initialized = FALSE;
	if(ax12_is_initialized == FALSE && AX12_is_ready(FRUIT_MOUTH_AX12_ID) == TRUE) {
		ax12_is_initialized = TRUE;
		AX12_config_set_highest_voltage(FRUIT_MOUTH_AX12_ID, 136);
		AX12_config_set_lowest_voltage(FRUIT_MOUTH_AX12_ID, 70);
		AX12_config_set_maximum_torque_percentage(FRUIT_MOUTH_AX12_ID, FRUIT_AX12_MAX_TORQUE_PERCENT);

		AX12_config_set_maximal_angle(FRUIT_MOUTH_AX12_ID, 300);
		AX12_config_set_minimal_angle(FRUIT_MOUTH_AX12_ID, 0);

		AX12_config_set_error_before_led(FRUIT_MOUTH_AX12_ID, AX12_ERROR_ANGLE | AX12_ERROR_CHECKSUM | AX12_ERROR_INSTRUCTION | AX12_ERROR_OVERHEATING | AX12_ERROR_OVERLOAD | AX12_ERROR_RANGE);
		AX12_config_set_error_before_shutdown(FRUIT_MOUTH_AX12_ID, AX12_ERROR_OVERHEATING); //On ne met pas l'overload comme par defaut, il faut pouvoir tenir l'assiette et sans que l'AX12 ne s'arrête de forcer pour cause de couple resistant trop fort.

		AX12_set_position(FRUIT_MOUTH_AX12_ID, FRUIT_AX12_INIT_POS);
		info_printf("FRUIT_MOUTH AX12 initialisé\n");
	}
}

void FRUIT_stop() {
}

bool_e FRUIT_CAN_process_msg(CAN_msg_t* msg) {
	queue_id_t queueId1;
	FRUIT_initAX12();
	if(msg->sid == ACT_FRUIT_MOUTH) {
		switch(msg->data[0]) {
			case ACT_FRUIT_MOUTH_CLOSE:
				queueId1 = QUEUE_create();
				if(queueId1 != QUEUE_CREATE_FAILED) {
					QUEUE_add(queueId1, &QUEUE_take_sem, (QUEUE_arg_t){0, 0, NULL}, QUEUE_ACT_AX12_Fruit);
					QUEUE_add(queueId1, &FRUIT_run_command, (QUEUE_arg_t){msg->data[0], FRUIT_CS_CloseAX12,  &ACTQ_finish_SendResult}, QUEUE_ACT_AX12_Fruit);
					QUEUE_add(queueId1, &QUEUE_give_sem, (QUEUE_arg_t){0, 0, NULL}, QUEUE_ACT_AX12_Fruit);

				} else {	//on indique qu'on a pas géré la commande
					QUEUE_flush(queueId1);
					ACTQ_sendResultWithLine(msg->sid, msg->data[0], ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_NO_RESOURCES);
				}
				break;

			case ACT_FRUIT_MOUTH_OPEN:
				queueId1 = QUEUE_create();
				if(queueId1 != QUEUE_CREATE_FAILED) {
					QUEUE_add(queueId1, &QUEUE_take_sem, (QUEUE_arg_t){0, 0, NULL}, QUEUE_ACT_AX12_Fruit);
					QUEUE_add(queueId1, &FRUIT_run_command, (QUEUE_arg_t){msg->data[0], FRUIT_CS_OpenAX12,  &ACTQ_finish_SendResult}, QUEUE_ACT_AX12_Fruit);
					QUEUE_add(queueId1, &QUEUE_give_sem, (QUEUE_arg_t){0, 0, NULL}, QUEUE_ACT_AX12_Fruit);
				} else {	//on indique qu'on a pas géré la commande
					QUEUE_flush(queueId1);
					ACTQ_sendResultWithLine(msg->sid, msg->data[0], ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_NO_RESOURCES);
				}
				break;
			case ACT_FRUIT_MOUTH_MID:
				queueId1 = QUEUE_create();
				if(queueId1 != QUEUE_CREATE_FAILED){
					QUEUE_add(queueId1, &QUEUE_take_sem, (QUEUE_arg_t){0, 0, NULL}, QUEUE_ACT_AX12_Fruit);
					QUEUE_add(queueId1, &FRUIT_run_command, (QUEUE_arg_t){msg->data[0], FRUIT_CS_MidAX12,  &ACTQ_finish_SendResult}, QUEUE_ACT_AX12_Fruit);
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
	}

	return FALSE;
}


void FRUIT_run_command(queue_id_t queueId, bool_e init) {
	if(QUEUE_has_error(queueId)) {
		QUEUE_behead(queueId);
		return;
	}

if(QUEUE_get_act(queueId) == QUEUE_ACT_AX12_Fruit) {    // Gestion des mouvements du fruit
		if(init)
			FRUIT_command_init(queueId);
		else
			FRUIT_command_run(queueId);
	}
}


//Initialise une commande de la pince à assiette
static void FRUIT_command_init(queue_id_t queueId) {
	Uint8 command = QUEUE_get_arg(queueId)->canCommand;
	Uint16* ax12_goalPosition = &QUEUE_get_arg(queueId)->param;

	*ax12_goalPosition = 0xFFFF;

	switch(command) {
		case ACT_FRUIT_MOUTH_OPEN:  *ax12_goalPosition = FRUIT_AX12_OPEN_POS; break;
		case ACT_FRUIT_MOUTH_CLOSE: *ax12_goalPosition = FRUIT_AX12_CLOSED_POS; break;
		case ACT_FRUIT_MOUTH_MID: *ax12_goalPosition = FRUIT_AX12_MID_POS; break;
		case ACT_FRUIT_MOUTH_STOP:
			AX12_set_torque_enabled(FRUIT_MOUTH_AX12_ID, FALSE); //Stopper l'asservissement de l'AX12 qui gère la pince
			QUEUE_next(queueId, ACT_FRUIT_MOUTH, ACT_RESULT_DONE, ACT_RESULT_ERROR_OK, __LINE__);
			return;

		default: {
				error_printf("Invalid plier command: %u, code is broken !\n", command);
				QUEUE_next(queueId, ACT_FRUIT_MOUTH, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_LOGIC, __LINE__);
				return;
			}
	}
	if(*ax12_goalPosition == 0xFFFF) {
		error_printf("Invalid plier position: %u, code is broken !\n", command);
		QUEUE_next(queueId, ACT_FRUIT_MOUTH, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_LOGIC, __LINE__);
		return;
	}

	debug_printf("Move fruit ax12 to %d\n", *ax12_goalPosition);
	AX12_reset_last_error(FRUIT_MOUTH_AX12_ID); //Sécurité anti terroriste. Nous les parano on aime pas voir des erreurs là ou il n'y en a pas.
	if(!AX12_set_position(FRUIT_MOUTH_AX12_ID, *ax12_goalPosition)) {	//Si la commande n'a pas été envoyée correctement et/ou que l'AX12 ne répond pas a cet envoi, on l'indique à la carte stratégie
		error_printf("AX12_set_position error: 0x%x\n", AX12_get_last_error(FRUIT_MOUTH_AX12_ID).error);
		QUEUE_next(queueId, ACT_FRUIT_MOUTH, ACT_RESULT_FAILED, ACT_RESULT_ERROR_NOT_HERE, __LINE__);
		return;
	}
	//La commande a été envoyée et l'AX12 l'a bien reçu
}

//Gère les états pendant le mouvement de la pince.
static void FRUIT_command_run(queue_id_t queueId) {
	Uint8 result, errorCode;
	Uint16 line;

	//360° of large_epsilon, quand on a un timeout, on a forcément un resultat Ok (et pas d'erreur, on considère qu'on serre l'assiette)
	if(ACTQ_check_status_ax12(queueId, FRUIT_MOUTH_AX12_ID, QUEUE_get_arg(queueId)->param, FRUIT_AX12_ASSER_POS_EPSILON, FRUIT_AX12_ASSER_TIMEOUT, 360, &result, &errorCode, &line))
		QUEUE_next(queueId, ACT_FRUIT_MOUTH, result, errorCode, line);
}


#endif  /* I_AM_ROBOT_KRUSTY */
