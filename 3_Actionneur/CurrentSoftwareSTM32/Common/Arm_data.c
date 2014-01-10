
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

#include "Arm_data.h"
#include "../QS/QS_adc.h"

// prefix vaut par exemple ARM_ACT_UPDOWN, prefix##_ID vaut alors ARM_ACT_UPDOWN_ID (## est un concatenation de texte)
#define DECLARE_DCMOTOR(prefix) { ARM_DCMOTOR, prefix##_ID, prefix##_ASSER_TIMEOUT, prefix##_ASSER_POS_EPSILON, prefix##_MAX_PWM_WAY0, prefix##_MAX_PWM_WAY1, prefix##_PWM_NUM, prefix##_PORT_WAY, prefix##_PORT_WAY_BIT, prefix##_ASSER_KP, prefix##_ASSER_KI, prefix##_ASSER_KD }

#define DECLARE_AX12(prefix) { ARM_AX12, prefix##_ID, prefix##_ASSER_TIMEOUT, prefix##_ASSER_POS_EPSILON, prefix##_MAX_TORQUE_PERCENT, 0, 0, 0, 0, 0, 0, 0 }
#define DECLARE_RX24(prefix) { ARM_RX24, prefix##_ID, prefix##_ASSER_TIMEOUT, prefix##_ASSER_POS_EPSILON, prefix##_MAX_TORQUE_PERCENT, 0, 0, 0, 0, 0, 0, 0 }

ARM_motor_data_t arm_motors[ARM_ACT_NUMBER] = {
//   type  id  timeout epsilon maxP0 maxP1 pwm wayport waybit kp ki kd
	DECLARE_DCMOTOR(ARM_ACT_UPDOWN),
	DECLARE_RX24(ARM_ACT_RX24),
	DECLARE_AX12(ARM_ACT_AX12_MID),
	DECLARE_AX12(ARM_ACT_AX12_TRIANGLE)
};

arm_state_t arm_states[ARM_ST_NUMBER] = {
	{0, 0, 0, 0},
	{0, 0, 0, 0}
};

Uint8 arm_states_transitions[ARM_ST_NUMBER][ARM_ST_NUMBER] = {
	{0, 1},
	{1, 0}
};

static Sint16 ARM_readDCMPos() {
	return -ADC_getValue(ARM_ACT_UPDOWN_ID_POTAR_ADC_ID);
}
