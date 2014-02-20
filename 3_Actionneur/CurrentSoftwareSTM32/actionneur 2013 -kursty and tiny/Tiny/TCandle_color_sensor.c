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
#include "../act_queue_utils.h"
#include "config_tiny/config_pin.h"
#include "TCandle_color_sensor_config.h"

#include "config_debug.h"
#define LOG_PREFIX "CC: "
#define LOG_COMPONENT OUTPUT_LOG_COMPONENT_CANDLECOLOR
#include "../QS/QS_outputlog.h"

#ifndef CANDLECOLOR_CW_PIN_CHANNEL0
#  define CANDLECOLOR_CW_PIN_CHANNEL0 CW_UNUSED_PORT
#endif
#ifndef CANDLECOLOR_CW_PIN_CHANNEL1
#  define CANDLECOLOR_CW_PIN_CHANNEL1 CW_UNUSED_PORT
#endif
#ifndef CANDLECOLOR_CW_PIN_CHANNEL2
#  define CANDLECOLOR_CW_PIN_CHANNEL2 CW_UNUSED_PORT
#endif
#ifndef CANDLECOLOR_CW_PIN_CHANNEL3
#  define CANDLECOLOR_CW_PIN_CHANNEL3 CW_UNUSED_PORT
#endif
#ifndef CANDLECOLOR_CW_PIN_GATE
#  define CANDLECOLOR_CW_PIN_GATE     CW_UNUSED_PORT
#endif

#ifndef CANDLECOLOR_CW_PIN_CHANNEL0_BIT
#  define CANDLECOLOR_CW_PIN_CHANNEL0_BIT 0
#endif
#ifndef CANDLECOLOR_CW_PIN_CHANNEL1_BIT
#  define CANDLECOLOR_CW_PIN_CHANNEL1_BIT 0
#endif
#ifndef CANDLECOLOR_CW_PIN_CHANNEL2_BIT
#  define CANDLECOLOR_CW_PIN_CHANNEL2_BIT 0
#endif
#ifndef CANDLECOLOR_CW_PIN_CHANNEL3_BIT
#  define CANDLECOLOR_CW_PIN_CHANNEL3_BIT 0
#endif
#ifndef CANDLECOLOR_CW_PIN_GATE_BIT
#  define CANDLECOLOR_CW_PIN_GATE_BIT     0
#endif

#ifndef CANDLECOLOR_CW_PIN_CHANNEL0_INVERTED_LOGIC
#  define CANDLECOLOR_CW_PIN_CHANNEL0_INVERTED_LOGIC FALSE
#endif
#ifndef CANDLECOLOR_CW_PIN_CHANNEL1_INVERTED_LOGIC
#  define CANDLECOLOR_CW_PIN_CHANNEL1_INVERTED_LOGIC FALSE
#endif
#ifndef CANDLECOLOR_CW_PIN_CHANNEL2_INVERTED_LOGIC
#  define CANDLECOLOR_CW_PIN_CHANNEL2_INVERTED_LOGIC FALSE
#endif
#ifndef CANDLECOLOR_CW_PIN_CHANNEL3_INVERTED_LOGIC
#  define CANDLECOLOR_CW_PIN_CHANNEL3_INVERTED_LOGIC FALSE
#endif
#ifndef CANDLECOLOR_CW_PIN_GATE_BIT
#  define CANDLECOLOR_CW_PIN_GATE_BIT     0
#endif

#ifndef CANDLECOLOR_CW_PIN_ADC_X
#  define CANDLECOLOR_CW_PIN_ADC_X    CW_UNUSED_ANALOG_PORT
#endif
#ifndef CANDLECOLOR_CW_PIN_ADC_Y
#  define CANDLECOLOR_CW_PIN_ADC_Y    CW_UNUSED_ANALOG_PORT
#endif
#ifndef CANDLECOLOR_CW_PIN_ADC_Z
#  define CANDLECOLOR_CW_PIN_ADC_Z    CW_UNUSED_ANALOG_PORT
#endif

static void CANDLECOLOR_initAX12();
static Uint8 CANDLECOLOR_process_color();

#ifdef CANDLECOLOR_CW_DEBUG_COLOR
	static void CANDLECOLOR_debug_color_run(queue_id_t queueId, bool_e init);
#endif

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

	sensorConfig.digital_ports[CW_PP_Channel0].port = CANDLECOLOR_CW_PIN_CHANNEL0;
	sensorConfig.digital_ports[CW_PP_Channel0].bit_number = CANDLECOLOR_CW_PIN_CHANNEL0_BIT;
	sensorConfig.digital_ports[CW_PP_Channel0].is_inverted_logic = CANDLECOLOR_CW_PIN_CHANNEL0_INVERTED_LOGIC;
	sensorConfig.digital_ports[CW_PP_Channel1].port = CANDLECOLOR_CW_PIN_CHANNEL1;
	sensorConfig.digital_ports[CW_PP_Channel1].bit_number = CANDLECOLOR_CW_PIN_CHANNEL1_BIT;
	sensorConfig.digital_ports[CW_PP_Channel1].is_inverted_logic = CANDLECOLOR_CW_PIN_CHANNEL1_INVERTED_LOGIC;
	sensorConfig.digital_ports[CW_PP_Channel2].port = CANDLECOLOR_CW_PIN_CHANNEL2;
	sensorConfig.digital_ports[CW_PP_Channel2].bit_number = CANDLECOLOR_CW_PIN_CHANNEL2_BIT;
	sensorConfig.digital_ports[CW_PP_Channel2].is_inverted_logic = CANDLECOLOR_CW_PIN_CHANNEL2_INVERTED_LOGIC;
	sensorConfig.digital_ports[CW_PP_Channel3].port = CANDLECOLOR_CW_PIN_CHANNEL3;
	sensorConfig.digital_ports[CW_PP_Channel3].bit_number = CANDLECOLOR_CW_PIN_CHANNEL3_BIT;
	sensorConfig.digital_ports[CW_PP_Channel3].is_inverted_logic = CANDLECOLOR_CW_PIN_CHANNEL3_INVERTED_LOGIC;
	sensorConfig.digital_ports[CW_PP_Gate].port = CANDLECOLOR_CW_PIN_GATE;
	sensorConfig.digital_ports[CW_PP_Gate].bit_number = CANDLECOLOR_CW_PIN_GATE_BIT;

	CW_config_sensor(CANDLECOLOR_CW_ID, &sensorConfig);

	component_printf(LOG_LEVEL_Info, "Capteur couleur de bougie initialisé (sensor)\n");

	CANDLECOLOR_initAX12();

#ifdef CANDLECOLOR_CW_DEBUG_COLOR
	QUEUE_add(QUEUE_create(), &CANDLECOLOR_debug_color_run, (QUEUE_arg_t){0, 0, &ACTQ_finish_SendNothing}, QUEUE_ACT_CandleColor);
#endif
}

static void CANDLECOLOR_initAX12() {
	static bool_e ax12_is_initialized = FALSE;
	if(ax12_is_initialized == FALSE && AX12_is_ready(CANDLECOLOR_AX12_ID) == TRUE) {
		ax12_is_initialized = TRUE;
		AX12_config_set_highest_voltage(CANDLECOLOR_AX12_ID, 136);
		AX12_config_set_lowest_voltage(CANDLECOLOR_AX12_ID, 70);
		AX12_config_set_maximum_torque_percentage(CANDLECOLOR_AX12_ID, CANDLECOLOR_AX12_MAX_TORQUE_PERCENT);

		AX12_config_set_maximal_angle(CANDLECOLOR_AX12_ID, CANDLECOLOR_AX12_ANGLE_MAX);
		AX12_config_set_minimal_angle(CANDLECOLOR_AX12_ID, CANDLECOLOR_AX12_ANGLE_MIN);

		AX12_config_set_error_before_led(CANDLECOLOR_AX12_ID, AX12_ERROR_ANGLE | AX12_ERROR_CHECKSUM | AX12_ERROR_INSTRUCTION | AX12_ERROR_OVERHEATING | AX12_ERROR_OVERLOAD | AX12_ERROR_RANGE);
		AX12_config_set_error_before_shutdown(CANDLECOLOR_AX12_ID, AX12_ERROR_OVERHEATING | AX12_ERROR_OVERLOAD);

		component_printf(LOG_LEVEL_Info, "AX12 initialisé\n");
	}
}

void CANDLECOLOR_stop() {

}

bool_e CANDLECOLOR_CAN_process_msg(CAN_msg_t* msg) {
	if(msg->sid == ACT_CANDLECOLOR) {
		//Initialise l'AX12 s'il n'était pas allimenté lors d'initialisations précédentes, si déjà initialisé, ne fait rien
		CANDLECOLOR_initAX12();

		switch(msg->data[0]) {
			case ACT_CANDLECOLOR_GET_LOW:
			case ACT_CANDLECOLOR_GET_HIGH:
				ACTQ_push_operation_from_msg(msg, QUEUE_ACT_CandleColor, &CANDLECOLOR_run_command, 0);
				break;

			default:
				component_printf(LOG_LEVEL_Warning, "invalid CAN msg data[0]=%u !\n", msg->data[0]);
		}
		return TRUE;
	}

	return FALSE;
}

void CANDLECOLOR_run_command(queue_id_t queueId, bool_e init) {
	if(QUEUE_get_act(queueId) == QUEUE_ACT_CandleColor) {
		if(QUEUE_has_error(queueId)) {
			QUEUE_behead(queueId);
			return;
		}

		if(init == TRUE && !QUEUE_has_error(queueId)) {
			Uint8 command = QUEUE_get_arg(queueId)->canCommand;
			Sint16* wantedPosition = (Sint16*) &QUEUE_get_arg(queueId)->param;
			//bool_e cmdOk;

			switch(command) {
				case ACT_CANDLECOLOR_GET_LOW:  *wantedPosition = ACT_CANDLECOLOR_GET_LOW;  break;
				case ACT_CANDLECOLOR_GET_HIGH: *wantedPosition = ACT_CANDLECOLOR_GET_HIGH; break;

				default: {
						component_printf(LOG_LEVEL_Error, "invalid rotation command: %u, code is broken !\n", command);
						QUEUE_next(queueId, ACT_CANDLECOLOR, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_LOGIC, __LINE__);
						return;
					}
			}
			if(*wantedPosition == 0xFFFF) {
				component_printf(LOG_LEVEL_Error, "Invalid AX12 position: %u, code is broken !\n", command);
				QUEUE_next(queueId, ACT_CANDLECOLOR, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_LOGIC, __LINE__);
				return;
			}

//			AX12_reset_last_error(CANDLECOLOR_AX12_ID);
//			cmdOk = AX12_set_position(CANDLECOLOR_AX12_ID, *wantedPosition);
//			if(!cmdOk) {	//Si la commande n'a pas été envoyée correctement et/ou que l'AX12 ne répond pas a cet envoi, on l'indique à la carte stratégie
//				component_printf(LOG_LEVEL_Error, "AX12_set_position error: 0x%x\n", AX12_get_last_error(CANDLECOLOR_AX12_ID).error);
//				QUEUE_next(queueId, ACT_CANDLECOLOR, ACT_RESULT_FAILED, ACT_RESULT_ERROR_NOT_HERE, __LINE__);
//				return;
//			}
		} else {
			Uint8 result, error_code;
			Uint16 line = __LINE__;

			result = ACT_RESULT_DONE;
			error_code = ACT_RESULT_ERROR_OK;

			//if(ACTQ_check_status_ax12(queueId, CANDLECOLOR_AX12_ID, QUEUE_get_arg(queueId)->param, CANDLECOLOR_AX12_POS_EPSILON, CANDLECOLOR_AX12_TIMEOUT, 0, &result, &error_code, &line)) {
				if(result == ACT_RESULT_DONE) {
					Uint8 color = CANDLECOLOR_process_color();
					CAN_msg_t colorDetectedMsg = {ACT_CANDLECOLOR_RESULT, {color}, 1};   //Envoyer la couleur detectée
					CAN_send(&colorDetectedMsg);
					QUEUE_next(queueId, ACT_CANDLECOLOR, result, error_code, color); //param vaut la couleur si il n'y a pas eu d'erreur
				} else QUEUE_next(queueId, ACT_CANDLECOLOR, result, error_code, line);
			//}
		}
	}
}

#ifdef CANDLECOLOR_CW_DEBUG_COLOR
static void CANDLECOLOR_debug_color_run(queue_id_t queueId, bool_e init) {
	switch(CANDLECOLOR_process_color()) {
		case ACT_CANDLECOLOR_COLOR_YELLOW: component_printf(LOG_LEVEL_Error, "  Color detected: yellow\n");  break;
		case ACT_CANDLECOLOR_COLOR_BLUE:   component_printf(LOG_LEVEL_Error, "  Color detected: blue\n");    break;
		case ACT_CANDLECOLOR_COLOR_OTHER:  component_printf(LOG_LEVEL_Error, "  No color detected\n");       break;
		case ACT_CANDLECOLOR_COLOR_RED:    component_printf(LOG_LEVEL_Error, "  Color detected: red\n");     break;
		case ACT_CANDLECOLOR_COLOR_WHITE:  component_printf(LOG_LEVEL_Error, "  Color detected: white\n");   break;
		default:                           component_printf(LOG_LEVEL_Error, "  Color detected: unknown\n"); break;
	}
}
#endif

static Uint8 CANDLECOLOR_process_color() {
	Uint16 x, y, z, Y, L; //L = light power = Y/y = X+Y+Z

#ifdef CANDLECOLOR_CW_USE_DIGITAL
	#ifdef CANDLECOLOR_CW_CHANNEL_RED
	if(CW_is_color_detected(CANDLECOLOR_CW_ID, CANDLECOLOR_CW_CHANNEL_RED))
		return ACT_CANDLECOLOR_COLOR_RED;
	#endif

	#ifdef CANDLECOLOR_CW_CHANNEL_BLUE
	if(CW_is_color_detected(CANDLECOLOR_CW_ID, CANDLECOLOR_CW_CHANNEL_BLUE))
		return ACT_CANDLECOLOR_COLOR_BLUE;
	#endif

	#ifdef CANDLECOLOR_CW_CHANNEL_YELLOW
	if(CW_is_color_detected(CANDLECOLOR_CW_ID, CANDLECOLOR_CW_CHANNEL_YELLOW))
		return ACT_CANDLECOLOR_COLOR_YELLOW;
	#endif

	#ifdef CANDLECOLOR_CW_CHANNEL_WHITE
	if(CW_is_color_detected(CANDLECOLOR_CW_ID, CANDLECOLOR_CW_CHANNEL_WHITE))
		return ACT_CANDLECOLOR_COLOR_WHITE;
	#endif
#endif //CANDLECOLOR_CW_USE_DIGITAL

#ifdef CANDLECOLOR_CW_USE_ANALOG
	x = CW_get_color_intensity(CANDLECOLOR_CW_ID, CW_AC_xyY_x);
	y = CW_get_color_intensity(CANDLECOLOR_CW_ID, CW_AC_xyY_y);
	Y = CW_get_color_intensity(CANDLECOLOR_CW_ID, CW_AC_xyY_Y);
	z = 1024 - x - y;
	L = (((Uint32)Y)*1024)/y;
	component_printf(LOG_LEVEL_Debug, "Detected color [%d,%d,%d] / 1024, Y: %d, L: %d\n", x, y, z, Y, L);

	#if defined(CANDLECOLOR_CW_YELLOW_MIN_xy) && defined(CANDLECOLOR_CW_YELLOW_MAX_DIFF_xy)
	if( x > CANDLECOLOR_CW_YELLOW_MIN_xy && y > CANDLECOLOR_CW_YELLOW_MIN_xy && abs(y-x) < CANDLECOLOR_CW_YELLOW_MAX_DIFF_xy)
		return ACT_CANDLECOLOR_COLOR_YELLOW;
	#endif

	if(Y < CANDLECOLOR_CW_MIN_Y)
		return ACT_CANDLECOLOR_COLOR_OTHER;

	#ifdef CANDLECOLOR_CW_BLUE_MIN_z
	if(z > CANDLECOLOR_CW_BLUE_MIN_z)
		return ACT_CANDLECOLOR_COLOR_BLUE;
	#endif

	#ifdef CANDLECOLOR_CW_RED_MIN_x
	if(x > CANDLECOLOR_CW_RED_MIN_x)
		return ACT_CANDLECOLOR_COLOR_RED;
	#endif

	#ifdef CANDLECOLOR_CW_GREEN_MIN_y
	if(y > CANDLECOLOR_CW_GREEN_MIN_y)
		return ACT_CANDLECOLOR_COLOR_OTHER;
	#endif

	#ifdef CANDLECOLOR_CW_WHITE_MIN_Y
	if(Y > CANDLECOLOR_CW_WHITE_MIN_Y)
		return ACT_CANDLECOLOR_COLOR_WHITE;
	#endif
	#ifdef CANDLECOLOR_CW_WHITE_MIN_L
	if(L > CANDLECOLOR_CW_WHITE_MIN_L)
		return ACT_CANDLECOLOR_COLOR_WHITE;
	#endif
#endif // CANDLECOLOR_CW_USE_ANALOG

	return ACT_CANDLECOLOR_COLOR_OTHER;
}

#endif	//I_AM_ROBOT_TINY

