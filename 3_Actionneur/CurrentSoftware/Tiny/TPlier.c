/*  Club Robot ESEO 2012 - 2013
 *	Tiny
 *
 *	Fichier : Plier.c
 *	Package : Carte actionneur
 *	Description : Gestion des bras horizontaux de tiny
 *  Auteur : Alexis
 *  Version 20130312
 *  Robot : Tiny
 */

#include "TPlier.h"
#ifdef I_AM_ROBOT_TINY

#include "../QS/QS_ax12.h"
#include "../QS/QS_CANmsgList.h"
#include "../output_log.h"
#include "../act_queue_utils.h"
#include "TPlier_config.h"

#define LOG_PREFIX "Plier: "
#define COMPONENT_log(log_level, format, ...) OUTPUTLOG_printf(OUTPUT_LOG_COMPONENT_PLIER, log_level, LOG_PREFIX format, ## __VA_ARGS__)

static void PLIER_initAX12();

void PLIER_init() {
	static bool_e initialized = FALSE;

	if(initialized)
		return;
	initialized = TRUE;

	AX12_init();

	PLIER_initAX12();

	COMPONENT_log(LOG_LEVEL_Info, "PLIER initialisé\n");
}

static void PLIER_initAX12() {
	static bool_e ax12_is_initialized[2] = {FALSE, FALSE};
	Uint8 ax12_id[2] =             {PLIER_LEFT_AX12_ID,                 PLIER_RIGHT_AX12_ID};
	Uint8 ax12_max_torque[2] =     {PLIER_LEFT_AX12_MAX_TORQUE_PERCENT, PLIER_RIGHT_AX12_MAX_TORQUE_PERCENT};
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
			AX12_config_set_error_before_shutdown(ax12_id[i], AX12_ERROR_OVERHEATING); //On ne met pas l'overload comme par defaut, il faut pouvoir serrer des verres

			COMPONENT_log(LOG_LEVEL_Info, "AX12 %s initialisé\n", (i == 0)? "Gauche" : "Droite");
		}
	}
}


void PLIER_stop() {
}

bool_e PLIER_CAN_process_msg(CAN_msg_t* msg) {
	queue_id_t queueId;

	if(msg->sid == ACT_PLIER) {
		switch(msg->data[0]) {
			case ACT_PLIER_CLOSE:
				queueId = QUEUE_create();
				assert(queueId != QUEUE_CREATE_FAILED);
				if(queueId != QUEUE_CREATE_FAILED) {
					QUEUE_add(queueId, &QUEUE_take_sem, (QUEUE_arg_t){0, 0, NULL}, QUEUE_ACT_Plier);
					QUEUE_add(queueId, &PLIER_run_command, (QUEUE_arg_t){msg->data[0], PLIER_CS_CloseRightAX12, &ACTQ_finish_SendResultIfFail}, QUEUE_ACT_Plier);
					QUEUE_add(queueId, &PLIER_run_command, (QUEUE_arg_t){msg->data[0], PLIER_CS_CloseLeftAX12,  &ACTQ_finish_SendResult}      , QUEUE_ACT_Plier);
					QUEUE_add(queueId, &QUEUE_give_sem, (QUEUE_arg_t){0, 0, NULL}, QUEUE_ACT_Plier);
				} else {	//on indique qu'on a pas géré la commande
					ACTQ_sendResultWithLine(msg->sid, msg->data[0], ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_NO_RESOURCES);
				}
				break;

			case ACT_PLIER_OPEN:
				queueId = QUEUE_create();
				assert(queueId != QUEUE_CREATE_FAILED);
				if(queueId != QUEUE_CREATE_FAILED) {
					QUEUE_add(queueId, &QUEUE_take_sem, (QUEUE_arg_t){0, 0, NULL}, QUEUE_ACT_Plier);
					QUEUE_add(queueId, &PLIER_run_command, (QUEUE_arg_t){msg->data[0], PLIER_CS_OpenLeftAX12,  &ACTQ_finish_SendResultIfFail}, QUEUE_ACT_Plier);
					QUEUE_add(queueId, &PLIER_run_command, (QUEUE_arg_t){msg->data[0], PLIER_CS_OpenRightAX12, &ACTQ_finish_SendResult}      , QUEUE_ACT_Plier);
					QUEUE_add(queueId, &QUEUE_give_sem, (QUEUE_arg_t){0, 0, NULL}, QUEUE_ACT_Plier);
				} else {	//on indique qu'on a pas géré la commande
					ACTQ_sendResultWithLine(msg->sid, msg->data[0], ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_NO_RESOURCES);
				}
				break;

			default:
				COMPONENT_log(LOG_LEVEL_Warning, "invalid CAN msg data[0]=%u !\n", msg->data[0]);
		}
		return TRUE;
	}

	return FALSE;
}

void PLIER_run_command(queue_id_t queueId, bool_e init) {
	static Uint16 wantedPosition;
	static Uint8 currentAX12;
	PLIER_command_state_e state = QUEUE_get_arg(queueId)->param;

	if(QUEUE_get_act(queueId) == QUEUE_ACT_Plier) {
		if(QUEUE_has_error(queueId)) {
			QUEUE_behead(queueId);
			return;
		}

		if(init == TRUE) {
			//Send command
			Uint8 command = QUEUE_get_arg(queueId)->canCommand;

			switch(state) {
				case PLIER_CS_CloseLeftAX12:
					currentAX12 = PLIER_LEFT_AX12_ID;
					wantedPosition = PLIER_LEFT_AX12_CLOSED_POS;
					break;

				case PLIER_CS_CloseRightAX12:
					currentAX12 = PLIER_RIGHT_AX12_ID;
					wantedPosition = PLIER_RIGHT_AX12_CLOSED_POS;
					break;

				case PLIER_CS_OpenLeftAX12:
					currentAX12 = PLIER_LEFT_AX12_ID;
					wantedPosition = PLIER_LEFT_AX12_OPEN_POS;
					break;

				case PLIER_CS_OpenRightAX12:
					currentAX12 = PLIER_RIGHT_AX12_ID;
					wantedPosition = PLIER_RIGHT_AX12_OPEN_POS;
					break;

				default: {
						COMPONENT_log(LOG_LEVEL_Error, "invalid rotation command: %u, code is broken !\n", command);
						QUEUE_next(queueId, ACT_PLIER, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_LOGIC, __LINE__);
						return;
					}
			}

			AX12_reset_last_error(currentAX12);
			if(!AX12_set_position(currentAX12, wantedPosition)) {	//Si la commande n'a pas été envoyée correctement et/ou que l'AX12 ne répond pas a cet envoi, on l'indique à la carte stratégie
				COMPONENT_log(LOG_LEVEL_Error, "AX12_set_position(%d) error: 0x%x\n", currentAX12, AX12_get_last_error(currentAX12).error);
				QUEUE_next(queueId, ACT_PLIER, ACT_RESULT_FAILED, ACT_RESULT_ERROR_NOT_HERE, __LINE__);
			}
		} else {
			Uint16 line;
			Uint8 result, errorCode;

			if(ACTQ_check_status_ax12(queueId, currentAX12, wantedPosition, PLIER_AX12_ASSER_POS_EPSILON, PLIER_AX12_ASSER_TIMEOUT, PLIER_AX12_POS_LARGE_EPSILON, &result, &errorCode, &line))
				QUEUE_next(queueId, ACT_PLIER, result, errorCode, line);
		}
	}
}

#endif	//I_AM_ROBOT_TINY
