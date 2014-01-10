
/*  Club Robot ESEO 2012 - 2013
 *	Krusty
 *
 *	Fichier : Lift.c
 *	Package : Carte actionneur
 *	Description : Gestion des ascenseurs
 *  Auteur : Alexis
 *  Version 20130314
 *  Robot : Krusty
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
#define LOG_PREFIX "LI: "
#define LOG_COMPONENT OUTPUT_LOG_COMPONENT_LIFT
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
static Sint16 ARM_readDCMPos();

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

			dcconfig.sensor_read = &ARM_readDCMPos;
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
		}
	}
}

bool_e ARM_CAN_process_msg(CAN_msg_t* msg) {
	if(msg->sid == ACT_ARM) {
		//Initialise les RX24/AX12 du bras s'ils n'étaient pas alimentés lors d'initialisations précédentes, si déjà initialisé, ne fait rien
		ARM_initAX12();

		switch(msg->data[0]) {
			case ACT_ARM_GOTO:
				ACTQ_push_operation_from_msg(msg, QUEUE_ACT_Arm, &ARM_run_command, msg->data[0]);
				break;

			default:
				warn_printf("invalid CAN msg data[0]=%u !\n", msg->data[0]);
		}
		return TRUE;
	}

	return FALSE;
}

static Sint16 ARM_readDCMPos() {
	return 0; //-ADC_getValue(ARM_LEFT_TRANSLATION_POTAR_ADC_ID);
	// fonction à mettre dans la struct de config, pour une fonction par dcmotor (avec le changement de signe dépendant du dcmotor en passant)
}

void ARM_run_command(queue_id_t queueId, bool_e init) {
	if(QUEUE_has_error(queueId)) {
		QUEUE_behead(queueId);
		return;
	}

	Uint8 canSid = 0;
	error_printf("Invalid act: %d\n", QUEUE_get_act(queueId));
	QUEUE_next(queueId, canSid, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_LOGIC, __LINE__);
}

#endif
