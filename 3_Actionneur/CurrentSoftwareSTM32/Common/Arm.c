/*
 *  Club Robot ESEO 2013 - 2014
 *
 *  $Id$
 *
 *  Package : Carte Actionneur
 *  Description : Gestion du bras
 *  Auteur : Alexis
 *  Robot : Pierre & Guy
 */

#include "Arm.h"

#if 1

#include "../QS/QS_CANmsgList.h"
#include "../QS/QS_DCMotor2.h"
#include "../QS/QS_ax12.h"
#include "../QS/QS_adc.h"
#include "../act_queue_utils.h"
#include "config_pin.h"
#include "Arm_config.h"
#include "Arm_data.h"
#include <string.h>

#include "config_debug.h"
#define LOG_PREFIX "Arm: "
#define LOG_COMPONENT OUTPUT_LOG_COMPONENT_ARM
#include "../QS/QS_outputlog.h"

#define XX(val) #val,
const char* ARM_STATES_NAME[] = {
	ARM_STATE_ENUMVALS(XX)
};
#undef XX

static void ARM_initAX12();

#warning "� changer pour une valeur indiquant qu'on ne connait pas l'etat actuel du bras"
static Sint8 old_state = 0;

static bool_e gotoState(ARM_state_e state);
static bool_e check_state_transitions();
static void print_corrected_state_transitions();

void ARM_init() {
	static bool_e initialized = FALSE;

	if(initialized)
		return;
	initialized = TRUE;

	DCM_init();
	AX12_init();
	ADC_init();

	if(check_state_transitions() == FALSE)
		print_corrected_state_transitions();

	Uint8 i;
	for(i = 0; i < ARM_MOTORS_NUMBER; i++) {
		if(ARM_MOTORS[i].type == ARM_DCMOTOR) {
			DCMotor_config_t dcconfig;

			dcconfig.sensor_read = ARM_MOTORS[i].sensorRead;
			dcconfig.Kp = ARM_MOTORS[i].kp;
			dcconfig.Ki = ARM_MOTORS[i].ki;
			dcconfig.Kd = ARM_MOTORS[i].kd;
			dcconfig.pos[0] = 0;
			dcconfig.pwm_number = ARM_MOTORS[i].pwmNum;
			dcconfig.way_latch = &ARM_MOTORS[i].pwmWayPort->ODR;
			dcconfig.way_bit_number = ARM_MOTORS[i].pwmWayBit;
			dcconfig.way0_max_duty = ARM_MOTORS[i].maxPowerWay0;
			dcconfig.way1_max_duty = ARM_MOTORS[i].maxPowerWay1;
			dcconfig.timeout = ARM_MOTORS[i].timeout;
			dcconfig.epsilon = ARM_MOTORS[i].epsilon;
			DCM_config(ARM_MOTORS[i].id, &dcconfig);
			DCM_stop(ARM_MOTORS[i].id);
		} else if(ARM_MOTORS[i].type == ARM_AX12 || ARM_MOTORS[i].type == ARM_RX24) {
			//pas initialis� ici mais dans ARM_initAX12 en dessous
		}
	}

	info_printf("Bras initialis� (DCMotor)\n");

	//AX12_config_set_id(21);

	ARM_initAX12();
}

//Initialise l'AX12 de la pince s'il n'�tait pas alliment� lors d'initialisations pr�c�dentes, si d�j� initialis�, ne fait rien
static void ARM_initAX12() {
	static bool_e allInitialized = FALSE;
	Uint8 i;
	bool_e allOk = TRUE;

	if(allInitialized)
		return;

	for(i = 0; i < ARM_MOTORS_NUMBER; i++) {
		if(ARM_MOTORS[i].type == ARM_AX12 || ARM_MOTORS[i].type == ARM_RX24) {
			if(ARM_ax12_is_initialized[i] == FALSE && AX12_is_ready(ARM_MOTORS[i].id) == TRUE) {
				ARM_ax12_is_initialized[i] = TRUE;

				AX12_config_set_highest_voltage(ARM_MOTORS[i].id, 150);
				AX12_config_set_lowest_voltage(ARM_MOTORS[i].id, 60);
				AX12_config_set_maximum_torque_percentage(ARM_MOTORS[i].id, ARM_MOTORS[i].maxPowerWay0);

				AX12_config_set_maximal_angle(ARM_MOTORS[i].id, 300);
				AX12_config_set_minimal_angle(ARM_MOTORS[i].id, 0);

				AX12_config_set_error_before_led(ARM_MOTORS[i].id, AX12_ERROR_ANGLE | AX12_ERROR_CHECKSUM | AX12_ERROR_INSTRUCTION | AX12_ERROR_OVERHEATING | AX12_ERROR_OVERLOAD | AX12_ERROR_RANGE);
				AX12_config_set_error_before_shutdown(ARM_MOTORS[i].id, AX12_ERROR_OVERHEATING); //On ne met pas l'overload comme par defaut, il faut pouvoir tenir l'assiette et sans que l'AX12 ne s'arr�te de forcer pour cause de couple resistant trop fort.
			} else if(ARM_ax12_is_initialized[i] == FALSE) {
				// Au moins un RX24/AX12 non pr�t => pas allOk, on affiche pas le message d'init
				allOk = FALSE;
				debug_printf("AX12 %d not here\n", ARM_MOTORS[i].id);
			}
		}
	}

	if(allOk) {
		info_printf("AX12 et RX24 du bras initialis�s\n");
		allInitialized = TRUE;
	}
}

void ARM_stop() {
	Uint8 i;
	for(i = 0; i < ARM_MOTORS_NUMBER; i++) {
		if(ARM_MOTORS[i].type == ARM_DCMOTOR) {
			DCM_stop(ARM_MOTORS[i].id);
		} else if(ARM_MOTORS[i].type == ARM_AX12 || ARM_MOTORS[i].type == ARM_RX24) {
			AX12_set_torque_enabled(ARM_MOTORS[i].id, FALSE);
		}
	}
}

bool_e ARM_CAN_process_msg(CAN_msg_t* msg) {
	if(msg->sid == ACT_ARM) {
		//Initialise les RX24/AX12 du bras s'ils n'�taient pas aliment�s lors d'initialisations pr�c�dentes, si d�j� initialis�, ne fait rien
		ARM_initAX12();

		switch(msg->data[0]) {
			case ACT_ARM_GOTO:
				ACTQ_push_operation_from_msg(msg, QUEUE_ACT_Arm, &ARM_run_command, msg->data[1]);
				break;

			case ACT_ARM_STOP:
				ARM_stop();
				break;

			default:
				warn_printf("invalid CAN msg data[0]=%u !\n", msg->data[0]);
		}
		return TRUE;
	}

	return FALSE;
}

void ARM_run_command(queue_id_t queueId, bool_e init) {
	if(QUEUE_has_error(queueId)) {
		QUEUE_behead(queueId);
		return;
	}

	if(QUEUE_get_act(queueId) == QUEUE_ACT_Arm) {    // Gestion des mouvements de rotation de l'assiette
		Sint16 new_state = QUEUE_get_arg(queueId)->param;

		if(init) {
			if(old_state < 0 || ARM_STATES_TRANSITIONS[old_state][new_state] == 0) {
				//d�placement impossible, le bras doit passer par d'autre positions avant d'atteindre la position demand�e
				warn_printf("D�placement impossible de l'etat %s(%d) to %s(%d)\n",
							ARM_STATES_NAME[old_state], old_state,
							ARM_STATES_NAME[new_state], new_state);
				QUEUE_next(queueId, ACT_ARM, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_INVALID_ARG, __LINE__);
				return;
			}

			debug_printf("Going from state %s(%d) to %s(%d)\n",
						 ARM_STATES_NAME[old_state], old_state,
						 ARM_STATES_NAME[new_state], new_state);
			gotoState(new_state);

		} else {
			bool_e done = TRUE, return_result = TRUE;
			Uint8 result = ACT_RESULT_DONE, error_code = ACT_RESULT_ERROR_OK;
			Uint16 line = 0;
			Uint8 i;

			for(i = 0; i < ARM_MOTORS_NUMBER; i++) {
				if(ARM_MOTORS[i].type == ARM_DCMOTOR) {
					done = ACTQ_check_status_dcmotor(ARM_MOTORS[i].id, FALSE, &result, &error_code, &line);
				} else if(ARM_MOTORS[i].type == ARM_AX12 || ARM_MOTORS[i].type == ARM_RX24) {
					done = ACTQ_check_status_ax12(queueId, ARM_MOTORS[i].id, ARM_get_motor_pos(new_state, i), ARM_MOTORS[i].epsilon, ARM_MOTORS[i].timeout, 360, &result, &error_code, &line);
				}

				//Si au moins un moteur n'a pas termin� son mouvement, alors l'action de d�placer le bras n'est pas termin�e
				if(!done) {
					return_result = FALSE;
				}

				//Si au moins un moteur n'a pas pu correctement se d�placer, alors on a fail l'action et on retourne � la position pr�c�dente
				if(done && result != ACT_RESULT_DONE) {
					return_result = TRUE;
					QUEUE_get_arg(queueId)->param = old_state;
					gotoState(old_state);
					break;
				}
			}

			if(return_result) {
				if(result == ACT_RESULT_DONE)
					old_state = new_state;
				QUEUE_next(queueId, ACT_ARM, result, error_code, line);
			}
		}
	} else {
		error_printf("Invalid act: %d\n", QUEUE_get_act(queueId));
		QUEUE_next(queueId, ACT_ARM, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_LOGIC, __LINE__);
	}
}

static bool_e gotoState(ARM_state_e state) {
	bool_e ok = TRUE;
	Uint8 i;

	for(i = 0; ok && i < ARM_MOTORS_NUMBER; i++) {
		if(ARM_MOTORS[i].type == ARM_DCMOTOR) {
			DCM_setPosValue(ARM_MOTORS[i].id, 0, ARM_get_motor_pos(state, i));
			DCM_goToPos(ARM_MOTORS[i].id, 0);
			DCM_restart(ARM_MOTORS[i].id);
		} else if(ARM_MOTORS[i].type == ARM_AX12 || ARM_MOTORS[i].type == ARM_RX24) {
			if(!AX12_set_position(ARM_MOTORS[i].id, ARM_get_motor_pos(state, i)))
				ok = FALSE;
		}
	}

	return ok;
}

//V�rifie que les transitions d'�tats sont reversibles
static bool_e check_state_transitions() {
	Uint8 i, j;
	bool_e ok = TRUE;

	for(i = 0; i < ARM_ST_NUMBER; i++) {
		for(j = 0; j < ARM_ST_NUMBER; j++) {
			if(ARM_STATES_TRANSITIONS[i][j] != ARM_STATES_TRANSITIONS[j][i]) {
				warn_printf("D�placement non reversible: %s(%d) %s %s(%d)\n",
							ARM_STATES_NAME[i], i,
							ARM_STATES_TRANSITIONS[i][j] ? "=>" : "<=",
							ARM_STATES_NAME[j], j);
				ok = FALSE;
			}
		}
	}

	return ok;
}

static void print_corrected_state_transitions() {
	Uint8 i, j;
	Uint8 columnSize = 0;
	bool_e isFirstVal;
	info_printf("Transitions d\'�tats:\n");

	OUTPUTLOG_printf(LOG_LEVEL_Info,
					 "bool_e ARM_STATES_TRANSITIONS[ARM_ST_NUMBER][ARM_ST_NUMBER] = {  //\n"
					 "//   ");
	for(j = 0; j < ARM_ST_NUMBER; j++) {
		Uint8 current_column_size = strlen(ARM_STATES_NAME[j]);
		if(columnSize < current_column_size)
			columnSize = current_column_size;

	}
	for(j = 0; j < ARM_ST_NUMBER; j++) {
		OUTPUTLOG_printf(LOG_LEVEL_Info, "%-*s ", columnSize, ARM_STATES_NAME[j]);
	}
	OUTPUTLOG_printf(LOG_LEVEL_Info, "\n");
	for(i = 0; i < ARM_ST_NUMBER; i++) {
		OUTPUTLOG_printf(LOG_LEVEL_Info, "    {");
		for(isFirstVal = TRUE, j = 0; j < ARM_ST_NUMBER; j++) {
			bool_e val = MAX(ARM_STATES_TRANSITIONS[i][j], ARM_STATES_TRANSITIONS[j][i]);
			OUTPUTLOG_printf(LOG_LEVEL_Info, "%s%-*d", isFirstVal ? "" : ",", columnSize, val);
			isFirstVal = FALSE;
		}
		OUTPUTLOG_printf(LOG_LEVEL_Info, "},  //%s\n", ARM_STATES_NAME[i]);
	}
	OUTPUTLOG_printf(LOG_LEVEL_Info, "};  //\n");
}

#endif
