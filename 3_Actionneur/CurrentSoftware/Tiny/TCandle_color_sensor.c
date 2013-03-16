/*  Club Robot ESEO 2012 - 2013
 *	Tiny
 *
 *	Fichier : Candle_color_sensor.c
 *	Package : Carte actionneur
 *	Description : Gestion du capteur de couleur Tritronics
 *  Auteur : Alexis
 *  Version 20130314
 *  Robot : Tiny
 */

#include "TCandle_color_sensor.h"
#ifdef I_AM_ROBOT_TINY

#include "../QS/QS_CapteurCouleurCW.h"
#include "../QS/QS_CANmsgList.h"
#include "../QS/QS_can.h"
#include "../QS/QS_ax12.h"
#include "../output_log.h"
#include "../Can_msg_processing.h"

#define LOG_PREFIX "CC: "

#ifndef CANDLECOLOR_CW_PIN_CHANNEL1
#  define CANDLECOLOR_CW_PIN_CHANNEL1 CW_UNUSED_PORT
#endif
#ifndef CANDLECOLOR_CW_PIN_CHANNEL2
#  define CANDLECOLOR_CW_PIN_CHANNEL2 CW_UNUSED_PORT
#endif
#ifndef CANDLECOLOR_CW_PIN_CHANNEL3
#  define CANDLECOLOR_CW_PIN_CHANNEL3 CW_UNUSED_PORT
#endif
#ifndef CANDLECOLOR_CW_PIN_CHANNEL4
#  define CANDLECOLOR_CW_PIN_CHANNEL4 CW_UNUSED_PORT
#endif
#ifndef CANDLECOLOR_CW_PIN_GATE
#  define CANDLECOLOR_CW_PIN_GATE CW_UNUSED_PORT
#endif

static void CANDLECOLOR_initAX12();
static void CANDLECOLOR_run_command(queue_id_t queueId, bool_e init);

void CANDLECOLOR_init() {
	static bool_e initialized = FALSE;
	CW_config_t sensorConfig;

	if(initialized)
		return;
	initialized = TRUE;

	AX12_init();
	CW_init();

	sensorConfig.analog_X = CANDLECOLOR_CW_PIN_ADC_X;
	sensorConfig.analog_Y = CANDLECOLOR_CW_PIN_ADC_Y;
	sensorConfig.analog_Z = CANDLECOLOR_CW_PIN_ADC_Z;

	//Fixme: ajouter bit_number
	sensorConfig.digital_ports[CW_PP_Channel1].port = CANDLECOLOR_CW_PIN_CHANNEL1;
	sensorConfig.digital_ports[CW_PP_Channel2].port = CANDLECOLOR_CW_PIN_CHANNEL2;
	sensorConfig.digital_ports[CW_PP_Channel3].port = CANDLECOLOR_CW_PIN_CHANNEL3;
	sensorConfig.digital_ports[CW_PP_Channel4].port = CANDLECOLOR_CW_PIN_CHANNEL4;
	sensorConfig.digital_ports[CW_PP_Gate].port = CANDLECOLOR_CW_PIN_GATE;
	CW_config_sensor(CANDLECOLOR_CW_ID, &sensorConfig);
}

static void CANDLECOLOR_initAX12() {
	static bool_e ax12_is_initialized = FALSE;
	if(ax12_is_initialized == FALSE && AX12_is_ready(CANDLECOLOR_AX12_ID) == TRUE) {
		ax12_is_initialized = TRUE;
		AX12_config_set_highest_voltage(CANDLECOLOR_AX12_ID, 136);
		AX12_config_set_lowest_voltage(CANDLECOLOR_AX12_ID, 70);
		AX12_config_set_maximum_torque_percentage(CANDLECOLOR_AX12_ID, CANDLECOLOR_AX12_MAX_TORQUE_PERCENT);

		//Fixme: A voir, l'angle effectif n'est pas super précis pour pouvoir utiliser directement les positions sans prendre de marge.
	//	AX12_config_set_maximal_angle(CANDLECOLOR_AX12_ID, (CANDLECOLOR_AX12_CLOSED_POS > PLATE_PLIER_AX12_OPEN_POS)? PLATE_PLIER_AX12_CLOSED_POS : PLATE_PLIER_AX12_OPEN_POS);
	//	AX12_config_set_minimal_angle(CANDLECOLOR_AX12_ID, (CANDLECOLOR_AX12_CLOSED_POS < PLATE_PLIER_AX12_OPEN_POS)? PLATE_PLIER_AX12_CLOSED_POS : PLATE_PLIER_AX12_OPEN_POS);

		AX12_config_set_error_before_led(CANDLECOLOR_AX12_ID, AX12_ERROR_ANGLE | AX12_ERROR_CHECKSUM | AX12_ERROR_INSTRUCTION | AX12_ERROR_OVERHEATING | AX12_ERROR_OVERLOAD | AX12_ERROR_RANGE);
		AX12_config_set_error_before_shutdown(CANDLECOLOR_AX12_ID, AX12_ERROR_OVERHEATING | AX12_ERROR_OVERHEATING);
	}
}

bool_e CANDLECOLOR_CAN_process_msg(CAN_msg_t* msg) {
	if(msg->sid == ACT_CANDLECOLOR) {
		//Initialise l'AX12 s'il n'était pas allimenté lors d'initialisations précédentes, si déjà initialisé, ne fait rien
		CANDLECOLOR_initAX12();

		switch(msg->data[0]) {
			case ACT_CANDLECOLOR_GET_LOW:
			case ACT_CANDLECOLOR_GET_HIGH:
				CAN_push_operation_from_msg(msg, QUEUE_ACT_CandleColor, &CANDLECOLOR_run_command, 0);
				break;

			default:
				OUTPUTLOG_printf(LOG_LEVEL_Warning, LOG_PREFIX"invalid CAN msg data[0]=%u !\n", msg->data[0]);
		}
		return TRUE;
	}

	return FALSE;
}

static void CANDLECOLOR_run_command(queue_id_t queueId, bool_e init) {
	if(QUEUE_get_act(queueId) == QUEUE_ACT_Hammer) {
		if(init == TRUE && !QUEUE_has_error(queueId)) {
			Uint8 command = QUEUE_get_arg(queueId)->canCommand;
			Sint16* wantedPosition = (Sint16*) &QUEUE_get_arg(queueId)->param;
			bool_e cmdOk;

			switch(command) {
				case ACT_CANDLECOLOR_GET_LOW:  *wantedPosition = ACT_CANDLECOLOR_GET_LOW;  break;
				case ACT_CANDLECOLOR_GET_HIGH: *wantedPosition = ACT_CANDLECOLOR_GET_HIGH; break;

				default: {
						CAN_msg_t resultMsg = {ACT_RESULT, {ACT_CANDLECOLOR & 0xFF, command, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_LOGIC}, 4};
						CAN_send(&resultMsg);
						OUTPUTLOG_printf(LOG_LEVEL_Error, LOG_PREFIX"invalid rotation command: %u, code is broken !\n", command);
						QUEUE_set_error(queueId);
						QUEUE_behead(queueId);
						return;
					}
			}
			if(*wantedPosition == 0xFFFF) {
				OUTPUTLOG_printf(LOG_LEVEL_Error, LOG_PREFIX"invalid plier position: %u, code is broken !\n", command);
				return;
			}
			//ax12_timeout_time = CLOCK_get_time() + PLATE_PLIER_AX12_ASSER_TIMEOUT;  //Calcul du timeout
			AX12_reset_last_error(CANDLECOLOR_AX12_ID);
			cmdOk = AX12_set_position(CANDLECOLOR_AX12_ID, *wantedPosition);
			if(!cmdOk) {	//Si la commande n'a pas été envoyée correctement et/ou que l'AX12 ne répond pas a cet envoi, on l'indique à la carte stratégie
				CAN_msg_t resultMsg = {ACT_RESULT, {ACT_CANDLECOLOR & 0xFF, command, ACT_RESULT_FAILED, ACT_RESULT_ERROR_NOT_HERE}, 4};
				CAN_send(&resultMsg);
				QUEUE_set_error(queueId);
				QUEUE_behead(queueId);
				return;
			}
		} else {
			Sint16 wantedPosition = QUEUE_get_arg(queueId)->param;
			Uint16 ax12Pos;
			Uint8 error;
			CAN_msg_t resultMsg;

			ax12Pos = AX12_get_position(CANDLECOLOR_AX12_ID);
			error = AX12_get_last_error(CANDLECOLOR_AX12_ID).error;

			resultMsg.size = 4;
			if(QUEUE_has_error(queueId)) {
				resultMsg.data[2] = ACT_RESULT_NOT_HANDLED;
				resultMsg.data[3] = ACT_RESULT_ERROR_OTHER;
			} else if(abs((Sint16)ax12Pos - (Sint16)(wantedPosition)) <= CANDLECOLOR_AX12_POS_EPSILON) {
				resultMsg.data[2] = ACT_RESULT_DONE;
				resultMsg.data[3] = ACT_RESULT_ERROR_OK;
				resultMsg.data[4] = 0; //TODO: lire la couleur
				resultMsg.size = 5;
			} else if((error & AX12_ERROR_TIMEOUT) && (error & AX12_ERROR_RANGE)) {
				resultMsg.data[2] = ACT_RESULT_NOT_HANDLED;
				resultMsg.data[3] = ACT_RESULT_ERROR_LOGIC;	//Si le driver a attendu trop longtemps, c'est a cause d'un deadlock plutot qu'un manque de ressources (il attend suffisament longtemps pour que les commandes soit bien envoyées)
				AX12_set_torque_enabled(CANDLECOLOR_AX12_ID, FALSE);
				QUEUE_set_error(queueId);
			} else if(error & AX12_ERROR_TIMEOUT) {	//L'ax12 n'a pas répondu à la commande
				resultMsg.data[2] = ACT_RESULT_FAILED;
				resultMsg.data[3] = ACT_RESULT_ERROR_NOT_HERE;
				AX12_set_torque_enabled(CANDLECOLOR_AX12_ID, FALSE);
				QUEUE_set_error(queueId);
			} else if(CLOCK_get_time() >= QUEUE_get_initial_time(queueId) + CANDLECOLOR_AX12_TIMEOUT) {    //Timeout, l'ax12 n'a pas bouger à la bonne position a temps
				resultMsg.data[2] = ACT_RESULT_FAILED;
				resultMsg.data[3] = ACT_RESULT_ERROR_UNKNOWN;
				AX12_set_torque_enabled(CANDLECOLOR_AX12_ID, FALSE);
				QUEUE_set_error(queueId);
			} else if(error) {							//autres erreurs
				resultMsg.data[2] = ACT_RESULT_FAILED;
				resultMsg.data[3] = ACT_RESULT_ERROR_UNKNOWN;
				AX12_set_torque_enabled(CANDLECOLOR_AX12_ID, FALSE);
				QUEUE_set_error(queueId);
			} else return;	//Operation is not finished, do nothing

			resultMsg.sid = ACT_RESULT;
			resultMsg.data[0] = ACT_CANDLECOLOR & 0xFF;
			resultMsg.data[1] = QUEUE_get_arg(queueId)->canCommand;

			CAN_send(&resultMsg);
			QUEUE_behead(queueId);	//gestion terminée
		}
	}
}

#endif	//I_AM_ROBOT_TINY

