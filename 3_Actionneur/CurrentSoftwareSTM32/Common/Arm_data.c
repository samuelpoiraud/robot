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
#include "../QS/QS_CANmsgList.h"

// prefix vaut par exemple ARM_ACT_UPDOWN, prefix##_ID vaut alors ARM_ACT_UPDOWN_ID (## est un concatenation de texte)
#define DECLARE_DCMOTOR(prefix, sensorRead) { ARM_DCMOTOR, prefix##_ID, prefix##_ASSER_TIMEOUT, prefix##_ASSER_POS_EPSILON, prefix##_ASSER_POS_LARGE_EPSILON, prefix##_MIN_VALUE, prefix##_MAX_VALUE, prefix##_MAX_PWM_WAY0      , prefix##_MAX_PWM_WAY1, prefix##_PWM_NUM, prefix##_PORT_WAY, prefix##_PORT_WAY_BIT, prefix##_DOUBLE_PID, prefix##_ASSER_KP, prefix##_ASSER_KI, prefix##_ASSER_KD, prefix##_ASSER_KP2, prefix##_ASSER_KI2, prefix##_ASSER_KD2, sensorRead }
#define DECLARE_AX12(prefix)                { ARM_AX12   , prefix##_ID, prefix##_ASSER_TIMEOUT, prefix##_ASSER_POS_EPSILON, prefix##_ASSER_POS_LARGE_EPSILON, prefix##_MIN_VALUE, prefix##_MAX_VALUE, prefix##_MAX_TORQUE_PERCENT, 0                    , 0               , 0                , 0                    , 0                  , 0                , 0                , 0                , 0                 , 0                 , 0                 , NULL       }
#define DECLARE_RX24(prefix)                { ARM_RX24   , prefix##_ID, prefix##_ASSER_TIMEOUT, prefix##_ASSER_POS_EPSILON, prefix##_ASSER_POS_LARGE_EPSILON, prefix##_MIN_VALUE, prefix##_MAX_VALUE, prefix##_MAX_TORQUE_PERCENT, 0                    , 0               , 0                , 0                    , 0                  , 0                , 0                , 0                , 0                 , 0                 , 0                 , NULL       }


// Liste de moteurs
const ARM_motor_data_t ARM_MOTORS[] = {
	DECLARE_DCMOTOR(ARM_ACT_UPDOWN, &ARM_readDCMPos),
	DECLARE_RX24(ARM_ACT_RX24),
	DECLARE_AX12(ARM_ACT_AX12_MID),
	DECLARE_AX12(ARM_ACT_AX12_TRIANGLE)
};


// Position d'états (ligne = état, colonne = position moteur dans l'ordre de ARM_MOTORS)
#ifdef I_AM_ROBOT_BIG
	const Sint16 ARM_STATES[ARM_ST_NUMBER][sizeof(ARM_MOTORS) / sizeof(ARM_motor_data_t)] = {
	// moteur (dans l'ordre)
	{-345	,153	,274,	249},	//ARM_ST_Parked
	{-345	,142	,239,	245},	//ARM_ST_Mid
	{-345	,145	,218,	249},	//ARM_ST_Open
	{-345	,176	,246,	145},	//ARM_ST_On_Torche				// Distance factice
	{-200	,148	,214,	145},	//ARM_ST_To_Storage
	{-351	,169	,280,	233},	//ARM_ST_To_Carry
	{-355	,123	,115,	237},	//ARM_ST_To_Prepare_Return
	{-230	,123	,115,	237},	//ARM_ST_To_Down_Return
	{-230	,127	,53,	236},	//ARM_ST_To_Return
	{-36	,111	,43,	148},	//ARM_ST_To_Take_Return
	{-345	,150	,150,	150}	//ARM_ST_On_Triangle			// Distance factice
	};
#else
	const Sint16 ARM_STATES[ARM_ST_NUMBER][sizeof(ARM_MOTORS) / sizeof(ARM_motor_data_t)] = {
	// moteur (dans l'ordre)
	{-345	,153	,274,	249},	//ARM_ST_Parked
	{-345	,142	,239,	245},	//ARM_ST_Mid
	{-345	,145	,218,	152},	//ARM_ST_Open
	{-345	,176	,246,	145},	//ARM_ST_On_Torche				// Distance factice
	{-200	,148	,214,	145},	//ARM_ST_To_Storage
	{-351	,169	,280,	233},	//ARM_ST_To_Carry
	{-355	,123	,115,	237},	//ARM_ST_To_Prepare_Return
	{-230	,123	,115,	237},	//ARM_ST_To_Down_Return
	{-230	,127	,53,	236},	//ARM_ST_To_Return
	{-36	,111	,43,	148},	//ARM_ST_To_Take_Return
	{-345	,150	,150,	150}	//ARM_ST_On_Triangle			// Distance factice
	};
#endif

// Liste des positions à effectuer pendant un selftest
const SELFTEST_action_t arm_selftest_action[] = {
	{ACT_ARM_GOTO, ACT_ARM_POS_OPEN, QUEUE_ACT_Arm},
	{ACT_ARM_GOTO, ACT_ARM_POS_PARKED, QUEUE_ACT_Arm}
};

const Uint8 ARM_SELFTEST_NUMBER = sizeof(arm_selftest_action) / sizeof(SELFTEST_action_t);

// Changement d'état possible (ligne -> colonne)
//                              lignes         colonnes
//                             ancien état    nouvel état
bool_e arm_states_transitions[ARM_ST_NUMBER][ARM_ST_NUMBER];

static void add_perm_transitions_table(ARM_state_e old_state, Uint8 number_of_new_state, ARM_state_e new_state[]);


//Fonctions de récupération des positions des moteurs DC

Sint16 ARM_readDCMPos() {
	return -ADC_getValue(ARM_ACT_UPDOWN_ID_POTAR_ADC_ID);
}

//////////////////////////////////////////////////////////////////////////////////////////
/// Interne, à ne pas changer
//////////////////////////////////////////////////////////////////////////////////////////

//Utilisation d'une variable constant pour éviter de maintenir un define en plus de la liste de moteur quand on la change
const Uint8 ARM_MOTORS_NUMBER = sizeof(ARM_MOTORS) / sizeof(ARM_motor_data_t);
bool_e ARM_ax12_is_initialized[sizeof(ARM_MOTORS) / sizeof(ARM_motor_data_t)] = {0};

Sint16 ARM_get_motor_pos(ARM_state_e state, Uint8 motor) {
	return ARM_STATES[state][motor];
}

static bool_e initialized_transitions[ARM_ST_NUMBER] = {FALSE};

void init_perm_transitions_table(){
	Uint8 i,j;
	for(i=0;i<ARM_ST_NUMBER;i++)
		for(j=0;j<ARM_ST_NUMBER;j++)
			arm_states_transitions[i][j] = 0;
	//						   /Ancien état       /Nombre d'état suivant possible      /Liste des états suivant possible
	add_perm_transitions_table(ACT_ARM_POS_PARKED,				1,	(ARM_state_e[]){ACT_ARM_POS_MID});
	add_perm_transitions_table(ACT_ARM_POS_MID,					2,	(ARM_state_e[]){ACT_ARM_POS_OPEN, ACT_ARM_POS_PARKED});
	add_perm_transitions_table(ACT_ARM_POS_OPEN,				6,	(ARM_state_e[]){ACT_ARM_POS_MID, ACT_ARM_POS_ON_TORCHE, ACT_ARM_POS_TO_STORAGE, ACT_ARM_POS_TO_PREPARE_RETURN, ACT_ARM_POS_ON_TRIANGLE, ACT_ARM_POS_TO_CARRY});
	add_perm_transitions_table(ACT_ARM_POS_ON_TORCHE,			1,	(ARM_state_e[]){ACT_ARM_POS_OPEN});
	add_perm_transitions_table(ACT_ARM_POS_TO_STORAGE,			1,	(ARM_state_e[]){ACT_ARM_POS_OPEN});
	add_perm_transitions_table(ACT_ARM_POS_TO_CARRY,			1,	(ARM_state_e[]){ACT_ARM_POS_OPEN});
	add_perm_transitions_table(ACT_ARM_POS_TO_PREPARE_RETURN,	3,	(ARM_state_e[]){ACT_ARM_POS_OPEN, ACT_ARM_POS_TO_DOWN_RETURN, ACT_ARM_POS_TO_TAKE_RETURN});
	add_perm_transitions_table(ACT_ARM_POS_TO_DOWN_RETURN,		2,	(ARM_state_e[]){ACT_ARM_POS_TO_RETURN, ACT_ARM_POS_TO_PREPARE_RETURN});
	add_perm_transitions_table(ACT_ARM_POS_TO_RETURN,			1,	(ARM_state_e[]){ACT_ARM_POS_TO_DOWN_RETURN});
	add_perm_transitions_table(ACT_ARM_POS_TO_TAKE_RETURN,		2,	(ARM_state_e[]){ACT_ARM_POS_TO_PREPARE_RETURN});
	add_perm_transitions_table(ACT_ARM_POS_ON_TRIANGLE,			1,	(ARM_state_e[]){ACT_ARM_POS_OPEN});
}

void add_perm_transitions_table(ARM_state_e old_state, Uint8 number_of_new_state, ARM_state_e new_state[]){
	Uint8 i, j;
	for(i=0;i<number_of_new_state;i++){
		for(j=0;j<ARM_ST_NUMBER;j++){
			if(j == new_state[i])
				arm_states_transitions[old_state][j] = 1;
		}
	}
	initialized_transitions[old_state] = TRUE;
}

bool_e check_all_state_perm_transitions_initialized(){
	Uint8 i;
	for(i=0;i<ARM_ST_NUMBER;i++)
		if(initialized_transitions[i] == FALSE)
			return FALSE;
	return TRUE;
}
