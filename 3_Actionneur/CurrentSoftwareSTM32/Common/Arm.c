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

#include "config_debug.h"
#define LOG_PREFIX "Arm: "
#define LOG_COMPONENT OUTPUT_LOG_COMPONENT_ARM
#include "../QS/QS_outputlog.h"

#define ARM_NUM_POS           3
	#define ARM_UP_POS_ID     0
	#define ARM_MID_POS_ID    1
	#define ARM_DOWN_POS_ID   2

#if DCMOTOR_NB_POS < ARM_NUM_POS
#error "Le nombre de position disponible dans l'asservissement DCMotor n'est pas suffisant"
#endif
#if DCM_NUMBER <= ARM_LEFT_DCMOTOR_ID || DCM_NUMBER <= ARM_RIGHT_DCMOTOR_ID
#error "Le nombre de DCMotor disponible n'est pas suffisant, veuillez augmenter DCM_NUMBER"
#endif

static void ARM_initAX12();

#warning "à changer pour une valeur indiquant qu'on ne connait pas l'etat actuel du bras"
static Sint8 old_state = 0;

void ARM_init() {
	static bool_e initialized = FALSE;

	if(initialized)
		return;
	initialized = TRUE;

	DCM_init();
	AX12_init();
	ADC_init();

	Uint8 i;
	for(i = 0; i < ARM_ACT_NUMBER; i++) {
		if(arm_motors[i].type == ARM_DCMOTOR) {
			DCMotor_config_t dcconfig;

			dcconfig.sensor_read = arm_motors[i].sensorRead;
			dcconfig.Kp = arm_motors[i].kp;
			dcconfig.Ki = arm_motors[i].ki;
			dcconfig.Kd = arm_motors[i].kd;
			dcconfig.pos[0] = 0;
			dcconfig.pwm_number = arm_motors[i].pwmNum;
			dcconfig.way_latch = &arm_motors[i].pwmWayPort->ODR;
			dcconfig.way_bit_number = arm_motors[i].pwmWayBit;
			dcconfig.way0_max_duty = arm_motors[i].maxPowerWay0;
			dcconfig.way1_max_duty = arm_motors[i].maxPowerWay1;
			dcconfig.timeout = arm_motors[i].timeout;
			dcconfig.epsilon = arm_motors[i].epsilon;
			DCM_config(arm_motors[i].id, &dcconfig);
			DCM_stop(arm_motors[i].id);
		} else if(arm_motors[i].type == ARM_AX12 || arm_motors[i].type == ARM_RX24) {
			//pas initialisé ici mais dans ARM_initAX12 en dessous
		}
	}

	info_printf("Bras initialisé (DCMotor)\n");

	//AX12_config_set_id(21);

	ARM_initAX12();
}

//Initialise l'AX12 de la pince s'il n'était pas allimenté lors d'initialisations précédentes, si déjà initialisé, ne fait rien
static void ARM_initAX12() {
	static bool_e ax12_is_initialized[ARM_ACT_NUMBER] = {0};
	static bool_e allInitialized = FALSE;
	Uint8 i;
	bool_e allOk = TRUE;

	if(allInitialized)
		return;

	for(i = 0; i < ARM_ACT_NUMBER; i++) {
		if(arm_motors[i].type == ARM_AX12 || arm_motors[i].type == ARM_RX24) {
			if(ax12_is_initialized[i] == FALSE && AX12_is_ready(arm_motors[i].id) == TRUE) {
				ax12_is_initialized[i] = TRUE;

				AX12_config_set_highest_voltage(arm_motors[i].id, 150);
				AX12_config_set_lowest_voltage(arm_motors[i].id, 60);
				AX12_config_set_maximum_torque_percentage(arm_motors[i].id, arm_motors[i].maxPowerWay0);

				AX12_config_set_maximal_angle(arm_motors[i].id, 300);
				AX12_config_set_minimal_angle(arm_motors[i].id, 0);

				AX12_config_set_error_before_led(arm_motors[i].id, AX12_ERROR_ANGLE | AX12_ERROR_CHECKSUM | AX12_ERROR_INSTRUCTION | AX12_ERROR_OVERHEATING | AX12_ERROR_OVERLOAD | AX12_ERROR_RANGE);
				AX12_config_set_error_before_shutdown(arm_motors[i].id, AX12_ERROR_OVERHEATING); //On ne met pas l'overload comme par defaut, il faut pouvoir tenir l'assiette et sans que l'AX12 ne s'arrête de forcer pour cause de couple resistant trop fort.
			} else {
				// Au moins un RX24/AX12 non prêt => pas allOk, on affiche pas le message d'init
				allOk = FALSE;
				debug_printf("AX12 %d not here\n", arm_motors[i].id);
			}
		}
	}

	if(allOk) {
		info_printf("AX12 et RX24 du bras initialisés\n");
		allInitialized = TRUE;
	}
}

void ARM_stop() {
	Uint8 i;
	for(i = 0; i < ARM_ACT_NUMBER; i++) {
		if(arm_motors[i].type == ARM_DCMOTOR) {
			DCM_stop(arm_motors[i].id);
		} else if(arm_motors[i].type == ARM_AX12 || arm_motors[i].type == ARM_RX24) {
			AX12_set_torque_enabled(arm_motors[i].id, FALSE);
		}
	}
}

bool_e ARM_CAN_process_msg(CAN_msg_t* msg) {
	if(msg->sid == ACT_ARM) {
		//Initialise les RX24/AX12 du bras s'ils n'étaient pas alimentés lors d'initialisations précédentes, si déjà initialisé, ne fait rien
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
		Sint8 new_state = QUEUE_get_arg(queueId)->param;
		Uint8 i;

		if(init) {
			if(old_state < 0 || arm_states_transitions[old_state][new_state] == 0) {
				//déplacement impossible, le bras doit passer par d'autre positions avant d'atteindre la position demandée
				warn_printf("Deplacement impossible de l'etat %d à %d\n", old_state, new_state);
				QUEUE_next(queueId, ACT_ARM, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_INVALID_ARG, __LINE__);
				return;
			}

			debug_printf("Going from state %d to %d\n", old_state, new_state);

			for(i = 0; i < ARM_ACT_NUMBER; i++) {
				if(arm_motors[i].type == ARM_DCMOTOR) {
					DCM_setPosValue(arm_motors[i].id, 0, arm_states[new_state][i]);
					DCM_goToPos(arm_motors[i].id, 0);
					DCM_restart(arm_motors[i].id);
				} else if(arm_motors[i].type == ARM_AX12 || arm_motors[i].type == ARM_RX24) {
					AX12_set_position(arm_motors[i].id, arm_states[new_state][i]);
				}
			}
		} else {
			bool_e done = TRUE, return_result = TRUE;
			Uint8 result = ACT_RESULT_DONE, error_code = ACT_RESULT_ERROR_OK;
			Uint16 line = 0;

			for(i = 0; i < ARM_ACT_NUMBER; i++) {
				if(arm_motors[i].type == ARM_DCMOTOR) {
					done = ACTQ_check_status_dcmotor(arm_motors[i].id, FALSE, &result, &error_code, &line);
				} else if(arm_motors[i].type == ARM_AX12 || arm_motors[i].type == ARM_RX24) {
					done = ACTQ_check_status_ax12(queueId, arm_motors[i].id, arm_states[new_state][i], arm_motors[i].epsilon, arm_motors[i].timeout, 360, &result, &error_code, &line);
				}

				if(!done)
					return_result = FALSE;
				if(done && result != ACT_RESULT_DONE) {
					return_result = TRUE;
					break;
				}
			}

			if(return_result) {
				QUEUE_next(queueId, ACT_ARM, result, error_code, line);
				old_state = new_state;
			}
		}
	} else {
		error_printf("Invalid act: %d\n", QUEUE_get_act(queueId));
		QUEUE_next(queueId, ACT_ARM, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_LOGIC, __LINE__);
	}
}

#endif
