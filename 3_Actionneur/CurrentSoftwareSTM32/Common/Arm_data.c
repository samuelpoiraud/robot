/*
 *  Club Robot ESEO 2013 - 2014
 *
 *  $Id$
 *
 *  Package : Carte Actionneur
 *  Description : Données pour la gestion du bras
 *  Auteur : Alexis
 *  Robot : Pierre & Guy
 */

#include "Arm_data.h"
#include "../QS/QS_adc.h"


// prefix vaut par exemple ARM_ACT_UPDOWN, prefix##_ID vaut alors ARM_ACT_UPDOWN_ID (## est un concatenation de texte)
#define DECLARE_DCMOTOR(prefix, sensorRead) { ARM_DCMOTOR, prefix##_ID, prefix##_ASSER_TIMEOUT, prefix##_ASSER_POS_EPSILON, prefix##_MAX_PWM_WAY0      , prefix##_MAX_PWM_WAY1, prefix##_PWM_NUM, prefix##_PORT_WAY, prefix##_PORT_WAY_BIT, prefix##_ASSER_KP, prefix##_ASSER_KI, prefix##_ASSER_KD, sensorRead }
#define DECLARE_AX12(prefix)                { ARM_AX12   , prefix##_ID, prefix##_ASSER_TIMEOUT, prefix##_ASSER_POS_EPSILON, prefix##_MAX_TORQUE_PERCENT, 0                    , 0               , 0                , 0                    , 0                , 0                , 0                , NULL       }
#define DECLARE_RX24(prefix)                { ARM_RX24   , prefix##_ID, prefix##_ASSER_TIMEOUT, prefix##_ASSER_POS_EPSILON, prefix##_MAX_TORQUE_PERCENT, 0                    , 0               , 0                , 0                    , 0                , 0                , 0                , NULL       }

static Sint16 ARM_readDCMPos();

//liste de moteurs
ARM_motor_data_t arm_motors[ARM_ACT_NUMBER] = {
//   type  id  timeout epsilon maxP0 maxP1 pwm wayport waybit kp ki kd
//	DECLARE_DCMOTOR(ARM_ACT_UPDOWN, &ARM_readDCMPos),
	DECLARE_RX24(ARM_ACT_RX24),
	DECLARE_AX12(ARM_ACT_AX12_MID),
//	DECLARE_AX12(ARM_ACT_AX12_TRIANGLE)
};

//Position d'états (ligne = état, colonne = pos moteur dans l'ordre de arm_motors)
arm_state_t arm_states[ARM_ST_NUMBER] = {
// moteur (dans l'ordre)
	{0  , 150},	//ARM_ST_Parked
	{150, 300},	//ARM_ST_Open
	{54, 176}	//ARM_ST_Mid
};

//changement d'état possible (ligne -> colonne)
//                              lignes         colonnes
//                             ancien état    nouvel état
bool_e arm_states_transitions[ARM_ST_NUMBER][ARM_ST_NUMBER] = {
//   ARM_ST_Parked     ARM_ST_Open       ARM_ST_Mid
	{1,                0,                1}, //ARM_ST_Parked
	{1,                1,                1}, //ARM_ST_Open
	{0,                1,                1}  //ARM_ST_Mid
};

static Sint16 ARM_readDCMPos() {
	return -ADC_getValue(ARM_ACT_UPDOWN_ID_POTAR_ADC_ID);
}
