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
#include "../selftest.h"
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

static Sint8 old_state = -1;

static bool_e gotoState(ARM_state_e state);
static bool_e check_state_transitions();
static void print_state_transitions(bool_e correct);
static bool_e find_state();

void ARM_init() {
	static bool_e initialized = FALSE;

	if(initialized)
		return;
	initialized = TRUE;

	DCM_init();
	AX12_init();
	ADC_init();

	if(check_state_transitions() == FALSE)
		print_state_transitions(TRUE);

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
			//pas initialisé ici mais dans ARM_initAX12 en dessous
		}
	}

	info_printf("Bras initialisé (DCMotor)\n");

	//AX12_config_set_id(21);

	ARM_initAX12();
}

//Initialise l'AX12 de la pince s'il n'était pas allimenté lors d'initialisations précédentes, si déjà initialisé, ne fait rien
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
				AX12_config_set_error_before_shutdown(ARM_MOTORS[i].id, AX12_ERROR_OVERHEATING); //On ne met pas l'overload comme par defaut, il faut pouvoir tenir l'assiette et sans que l'AX12 ne s'arrête de forcer pour cause de couple resistant trop fort.
			} else if(ARM_ax12_is_initialized[i] == FALSE) {
				// Au moins un RX24/AX12 non prêt => pas allOk, on affiche pas le message d'init
				allOk = FALSE;
				debug_printf("AX12 %d not here\n", ARM_MOTORS[i].id);
			}
		}
	}

	if(allOk) {
		info_printf("AX12 et RX24 du bras initialisés\n");
		allInitialized = TRUE;
		find_state();
	}
}

void ARM_initPos() {
	CAN_msg_t msg = {ACT_ARM, {ACT_ARM_INIT}, 1};

	if(find_state() == FALSE)
		ARM_CAN_process_msg(&msg);
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
		//Initialise les RX24/AX12 du bras s'ils n'étaient pas alimentés lors d'initialisations précédentes, si déjà initialisé, ne fait rien
		ARM_initAX12();

		switch(msg->data[0]) {
			case ACT_ARM_GOTO:
				ACTQ_push_operation_from_msg(msg, QUEUE_ACT_Arm, &ARM_run_command, msg->data[1]);
				break;

			case ACT_ARM_STOP:
				ARM_stop();
				break;

			case ACT_ARM_INIT: {
				queue_id_t queueId = QUEUE_create();
				assert(queueId != QUEUE_CREATE_FAILED);
				if(queueId != QUEUE_CREATE_FAILED) {
					Uint8 i;
					QUEUE_add(queueId, &QUEUE_take_sem, (QUEUE_arg_t){0, 0, NULL}, QUEUE_ACT_Arm);

					for(i = 0; i < ARM_INIT_NUMBER; i++) {
						QUEUE_add(queueId, &ARM_run_command, (QUEUE_arg_t){msg->data[0], ARM_INIT[i], &ACTQ_finish_SendNothing}, QUEUE_ACT_Arm);
					}

					QUEUE_add(queueId, &QUEUE_give_sem, (QUEUE_arg_t){0, 0, NULL}, QUEUE_ACT_Arm);
				} else {	//on indique qu'on a pas géré la commande
					ACTQ_sendResult(msg->sid, msg->data[0], ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_NO_RESOURCES);
				}
				break;
			}

			case ACT_ARM_PRINT_POS: {
				Uint8 i;
				bool_e firstValue = TRUE;

				info_printf("Positions:\n");
				OUTPUTLOG_printf(LOG_LEVEL_Info, "    {");
				for(i = 0; i < ARM_MOTORS_NUMBER; i++) {
					if(!firstValue)
						OUTPUTLOG_printf(LOG_LEVEL_Info, ",");
					if(ARM_MOTORS[i].type == ARM_DCMOTOR) {
						OUTPUTLOG_printf(LOG_LEVEL_Info, " %5d", ARM_MOTORS[i].sensorRead());
					} else if(ARM_MOTORS[i].type == ARM_AX12 || ARM_MOTORS[i].type == ARM_RX24) {
						OUTPUTLOG_printf(LOG_LEVEL_Info, " %5d", AX12_get_position(ARM_MOTORS[i].id));
					}
					firstValue = FALSE;
				}

				OUTPUTLOG_printf(LOG_LEVEL_Info, "}, //%s\n", old_state >= 0 ? ARM_STATES_NAME[old_state] : "");
				break;
			}

			case ACT_ARM_PRINT_STATE_TRANSITIONS:
				print_state_transitions(FALSE);
				break;

			default:
				warn_printf("invalid CAN msg data[0]=%u !\n", msg->data[0]);
		}
		return TRUE;
	} else if(msg->sid == ACT_DO_SELFTEST) {
		SELFTEST_action_t tests[ARM_INIT_NUMBER];
		Uint8 i;

		for(i = 0; i < ARM_INIT_NUMBER; i++) {
			tests[i].canCommand = ACT_ARM_INIT;
			tests[i].optionnal_act = QUEUE_ACT_Arm;
			tests[i].param = ARM_INIT[i];
		}

		SELFTEST_set_actions(&ARM_run_command, ARM_INIT_NUMBER, tests);
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
			Uint8 canCommand = QUEUE_get_arg(queueId)->canCommand;

			if(old_state < 0 && canCommand != ACT_ARM_INIT) {
				warn_printf("Etat non initialisé, impossible d\'aller à l\'état %s(%d)\n",
							ARM_STATES_NAME[new_state], new_state);
				QUEUE_next(queueId, ACT_ARM, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_INVALID_ARG, __LINE__);
				return;
			}

			if(old_state >= 0 && ARM_STATES_TRANSITIONS[old_state][new_state] == 0) {
				//déplacement impossible, le bras doit passer par d'autre positions avant d'atteindre la position demandée
				warn_printf("Déplacement impossible de l\'etat %s(%d) à %s(%d)\n",
							ARM_STATES_NAME[old_state], old_state,
							ARM_STATES_NAME[new_state], new_state);
				QUEUE_next(queueId, ACT_ARM, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_INVALID_ARG, __LINE__);
				return;
			}

			debug_printf("Going from state %s(%d) to %s(%d)\n",
						 (old_state >= 0) ? ARM_STATES_NAME[old_state] : "non-initialisé", old_state,
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
					done = ACTQ_check_status_ax12(queueId, ARM_MOTORS[i].id, ARM_get_motor_pos(new_state, i), ARM_MOTORS[i].epsilon, ARM_MOTORS[i].timeout, 0, &result, &error_code, &line);
				}

				//Si au moins un moteur n'a pas terminé son mouvement, alors l'action de déplacer le bras n'est pas terminée
				if(!done) {
					return_result = FALSE;
				}

				//Si au moins un moteur n'a pas pu correctement se déplacer, alors on a fail l'action et on retourne à la position précédente
				if(done && result != ACT_RESULT_DONE) {
					return_result = TRUE;
					if(old_state >= 0) {
						gotoState(old_state);
					}
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

	if(state < 0 || state >= ARM_ST_NUMBER)
		return FALSE;

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

//Vérifie que les transitions d'états sont reversibles
static bool_e check_state_transitions() {
	Uint8 i, j;
	bool_e ok = TRUE;

	for(i = 0; i < ARM_ST_NUMBER; i++) {
		for(j = 0; j < ARM_ST_NUMBER; j++) {
			if(ARM_STATES_TRANSITIONS[i][j] != ARM_STATES_TRANSITIONS[j][i]) {
				warn_printf("Déplacement non reversible: %s(%d) %s %s(%d)\n",
							ARM_STATES_NAME[i], i,
							ARM_STATES_TRANSITIONS[i][j] ? "=>" : "<=",
							ARM_STATES_NAME[j], j);
				ok = FALSE;
			}
		}
	}

	return ok;
}

static void print_state_transitions(bool_e correct) {
	Uint8 i, j;
	Uint8 columnSize = 0;
	bool_e isFirstVal;
	info_printf("Proposition de transitions d\'états corrigées:\n");

	OUTPUTLOG_printf(LOG_LEVEL_Info,
					 "const bool_e ARM_STATES_TRANSITIONS[ARM_ST_NUMBER][ARM_ST_NUMBER] = {  //\n"
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
			bool_e val;

			if(correct)
				val = MAX(ARM_STATES_TRANSITIONS[i][j], ARM_STATES_TRANSITIONS[j][i]);  //avec correction
			else
				val = ARM_STATES_TRANSITIONS[i][j]; //sans correction

			OUTPUTLOG_printf(LOG_LEVEL_Info, "%s%-*d", isFirstVal ? "" : ",", columnSize, val);
			isFirstVal = FALSE;
		}
		OUTPUTLOG_printf(LOG_LEVEL_Info, "},  //%s\n", ARM_STATES_NAME[i]);
	}
	OUTPUTLOG_printf(LOG_LEVEL_Info, "};  //\n");
}

static bool_e find_state() {
	Uint8 state, i;
	bool_e stateOk;

	for(state = 0; state < ARM_ST_NUMBER; state++) {
		stateOk = TRUE;

		for(i = 0; i < ARM_MOTORS_NUMBER; i++) {
			if(ARM_MOTORS[i].type == ARM_DCMOTOR) {
				if(absolute(ARM_MOTORS[i].sensorRead() - ARM_get_motor_pos(state, i)) >= ARM_MOTORS[i].large_epsilon) {
					stateOk = FALSE;
					break;
				}
			} else if(ARM_MOTORS[i].type == ARM_AX12 || ARM_MOTORS[i].type == ARM_RX24) {
				if(absolute(AX12_get_position(ARM_MOTORS[i].id) - ARM_get_motor_pos(state, i)) >= ARM_MOTORS[i].large_epsilon) {
					stateOk = FALSE;
					break;
				}
			}

		}

		if(stateOk == TRUE) {
			old_state = state;
			debug_printf("Etat initial auto détecté: %s(%d)\n",
						ARM_STATES_NAME[old_state], old_state);
			return TRUE;
		}
	}


	debug_printf("Etat initial non détecté\n");
	return FALSE;
}

#endif
