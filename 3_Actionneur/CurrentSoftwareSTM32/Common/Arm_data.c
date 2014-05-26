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
		{-168	,240	,49,	55},	//ARM_ST_Pre_Parked_1
		{-168	,217	,32,	55},	//ARM_ST_Pre_Parked_2
		{-124	,217	,32,	55},	//ARM_ST_Parked
		{-124	,180	,29,	55},	//ARM_ST_Mid
		{-124	,180	,84,	55},	//ARM_ST_Open

		{0		,0		,0		,0},	//ARM_ST_On_Torche				// Distance factice
		{-124	,212	,189,	227},	//ARM_ST_On_Torche_Small_Arm

		{-200	,148	,214,	145},	//ARM_ST_To_Storage						NOT USE
		{-351	,169	,280,	233},	//ARM_ST_To_Carry

		{-432	,158	,164,	238},	//ARM_ST_To_Prepare_Return				NOT USE
		{-287	,158	,94,	238},	//ARM_ST_To_Down_Return					NOT USE
		{-306	,133	,65,	238},	//ARM_ST_To_Return						NOT USE

		{-432	,133	,65,	120},	//ARM_ST_Wait_Return					NOT USE

		{-432	,115	,42,	148},	//ARM_ST_To_Prepare_Take_Return			NOT USE
		{-432	,132	,56,	148},	//ARM_ST_To_Take_Return					NOT USE
		{-432	,132	,56,	110},	//ARM_ST_To_Unblock_Return				NOT USE

		{0		,0		,0		,0},	//ARM_ST_On_Triangle			// Distance factice

		{-432	,113	,151,	79},	//ARM_ST_Prepare_Backward				NOT USE
		{-360	,113	,154,	79},	//ARM_ST_Down_Backward					NOT USE
		{-360	,113	,154,	148},	//ARM_ST_Lock_Backward					NOT USE

		{-432	,119	,160,	145},	//ARM_ST_Prepare_Take_On_Edge			NOT USE
		{-261	,119	,160,	145},	//ARM_ST_Take_On_Edge					NOT USE
		{-330	,120	,160,	145},	//ARM_ST_Return_On_Edge					NOT USE
		{-236	,172	,241,	145},	//ARM_ST_Disposed_Simple				NOT USE

		{-220	,200	,248,	249},	//ARM_ST_Prepare_Take_On_Road
		{-457	,232	,248,	249},	//ARM_ST_Take_On_Road
		{-457	,232	,248,	249}	//ARM_ST_Take_On_Road_Mamouth
		};
#else
	const Sint16 ARM_STATES[ARM_ST_NUMBER][sizeof(ARM_MOTORS) / sizeof(ARM_motor_data_t)] = {
		// moteur (dans l'ordre)
		{0		,0		,0		,0},	//ARM_ST_Pre_Parked_1			NOT USE
		{0		,0		,0		,0},	//ARM_ST_Pre_Parked_2			NOT USE
		{-432	,154	,283,	249},	//ARM_ST_Parked
		{-432	,152	,246,	245},	//ARM_ST_Mid
		{-432	,145	,218,	152},	//ARM_ST_Open

		{0		,0		,0		,0},	//ARM_ST_On_Torche				// Distance factice
		{0		,0		,0		,0},	//ARM_ST_On_Torche_Small_Arm	NOT USE

		{0		,0		,0		,0},	//ARM_ST_To_Storage				NOT USE
		{-351	,169	,280,	233},	//ARM_ST_To_Carry

		{-432	,158	,164,	238},	//ARM_ST_To_Prepare_Return
		{-287	,158	,94,	238},	//ARM_ST_To_Down_Return
		{-306	,133	,65,	238},	//ARM_ST_To_Return

		{-432	,133	,65,	120},	//ARM_ST_Wait_Return

		{-432	,115	,42,	148},	//ARM_ST_To_Prepare_Take_Return
		{-432	,132	,56,	148},	//ARM_ST_To_Take_Return
		{-432	,132	,56,	110},	//ARM_ST_To_Unblock_Return

		{0		,0		,0		,0},	//ARM_ST_On_Triangle			// Distance factice

		{-432	,113	,151,	79},	//ARM_ST_Prepare_Backward
		{-360	,113	,154,	79},	//ARM_ST_Down_Backward
		{-360	,113	,154,	148},	//ARM_ST_Lock_Backward

		{-432	,119	,160,	145},	//ARM_ST_Prepare_Take_On_Edge
		{-261	,119	,160,	145},	//ARM_ST_Take_On_Edge
		{-309	,120	,160,	145},	//ARM_ST_Return_On_Edge
		{-236	,172	,241,	145},	//ARM_ST_Disposed_Simple

		{0		,0		,0		,0},	//ARM_ST_Prepare_Take_On_Road	NOT USE
		{-215	,130	,173,	146},	//ARM_ST_Take_On_Road
		{-215	,124	,182,	146}	//ARM_ST_Take_On_Road_Mamouth
		};
#endif

// Liste des positions à effectuer pendant un selftest
const SELFTEST_action_t arm_selftest_action[] = {
	{ACT_ARM_GOTO, ACT_ARM_POS_MID, QUEUE_ACT_Arm},
	{ACT_ARM_GOTO, ACT_ARM_POS_OPEN, QUEUE_ACT_Arm},
	{ACT_ARM_GOTO, ACT_ARM_POS_MID, QUEUE_ACT_Arm},
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


#ifdef I_AM_ROBOT_SMALL
	//						   /Ancien état       /Nombre d'état suivant possible      /Liste des états suivant possible
	add_perm_transitions_table(ACT_ARM_POS_PARKED,					1,	(ARM_state_e[]){ACT_ARM_POS_MID});
	add_perm_transitions_table(ACT_ARM_POS_MID,						2,	(ARM_state_e[]){ACT_ARM_POS_OPEN, ACT_ARM_POS_PARKED});
	add_perm_transitions_table(ACT_ARM_POS_OPEN,					12,	(ARM_state_e[]){ACT_ARM_POS_MID, ACT_ARM_POS_ON_TORCHE, ACT_ARM_POS_TO_STORAGE,
																						ACT_ARM_POS_TO_PREPARE_RETURN, ACT_ARM_POS_ON_TRIANGLE, ACT_ARM_POS_TO_CARRY,
																						ACT_ARM_POS_TO_PREPARE_TAKE_RETURN, ACT_ARM_POS_PREPARE_BACKWARD,
																						ACT_ARM_POS_TAKE_ON_ROAD, ACT_ARM_POS_PREPARE_TAKE_ON_EDGE,
																						ACT_ARM_POS_RETURN_ON_EDGE, ACT_ARM_POS_DISPOSED_SIMPLE});

	add_perm_transitions_table(ACT_ARM_POS_ON_TORCHE,				3,	(ARM_state_e[]){ACT_ARM_POS_OPEN, ACT_ARM_POS_ON_TRIANGLE, ACT_ARM_POS_TO_PREPARE_RETURN});

	add_perm_transitions_table(ACT_ARM_POS_TO_STORAGE,				1,	(ARM_state_e[]){ACT_ARM_POS_OPEN});

	add_perm_transitions_table(ACT_ARM_POS_TO_CARRY,				1,	(ARM_state_e[]){ACT_ARM_POS_OPEN});

	add_perm_transitions_table(ACT_ARM_POS_TO_PREPARE_RETURN,		2,	(ARM_state_e[]){ACT_ARM_POS_OPEN, ACT_ARM_POS_TO_DOWN_RETURN});
	add_perm_transitions_table(ACT_ARM_POS_TO_DOWN_RETURN,			2,	(ARM_state_e[]){ACT_ARM_POS_TO_RETURN, ACT_ARM_POS_TO_PREPARE_RETURN});
	add_perm_transitions_table(ACT_ARM_POS_TO_RETURN,				2,	(ARM_state_e[]){ACT_ARM_POS_TO_DOWN_RETURN, ACT_ARM_POS_WAIT_RETURN});

	add_perm_transitions_table(ACT_ARM_POS_WAIT_RETURN,				3,	(ARM_state_e[]){ACT_ARM_POS_TO_TAKE_RETURN, ACT_ARM_POS_OPEN});

	add_perm_transitions_table(ACT_ARM_POS_TO_PREPARE_TAKE_RETURN,	2,	(ARM_state_e[]){ACT_ARM_POS_OPEN, ACT_ARM_POS_TO_TAKE_RETURN});
	add_perm_transitions_table(ACT_ARM_POS_TO_TAKE_RETURN,			1,	(ARM_state_e[]){ACT_ARM_POS_TO_UNBLOCK_RETURN});
	add_perm_transitions_table(ACT_ARM_POS_TO_UNBLOCK_RETURN,		1,	(ARM_state_e[]){ACT_ARM_POS_OPEN});

	add_perm_transitions_table(ACT_ARM_POS_ON_TRIANGLE,				2,	(ARM_state_e[]){ACT_ARM_POS_OPEN, ACT_ARM_POS_ON_TORCHE});

	add_perm_transitions_table(ACT_ARM_POS_PREPARE_BACKWARD,		2,	(ARM_state_e[]){ACT_ARM_POS_OPEN, ACT_ARM_POS_DOWN_BACKWARD});
	add_perm_transitions_table(ACT_ARM_POS_DOWN_BACKWARD,			2,	(ARM_state_e[]){ACT_ARM_POS_PREPARE_BACKWARD, ACT_ARM_POS_LOCK_BACKWARD});
	add_perm_transitions_table(ACT_ARM_POS_LOCK_BACKWARD,			1,	(ARM_state_e[]){ACT_ARM_POS_DOWN_BACKWARD});

	add_perm_transitions_table(ACT_ARM_POS_PREPARE_TAKE_ON_EDGE,	2,	(ARM_state_e[]){ACT_ARM_POS_OPEN, ACT_ARM_POS_TAKE_ON_EDGE});
	add_perm_transitions_table(ACT_ARM_POS_TAKE_ON_EDGE,			1,	(ARM_state_e[]){ACT_ARM_POS_PREPARE_TAKE_ON_EDGE});
	add_perm_transitions_table(ACT_ARM_POS_RETURN_ON_EDGE,			1,	(ARM_state_e[]){ACT_ARM_POS_OPEN});
	add_perm_transitions_table(ACT_ARM_POS_DISPOSED_SIMPLE,			1,	(ARM_state_e[]){ACT_ARM_POS_OPEN});

	add_perm_transitions_table(ACT_ARM_POS_TAKE_ON_ROAD,			1,	(ARM_state_e[]){ACT_ARM_POS_OPEN});

#else
	add_perm_transitions_table(ACT_ARM_POS_PRE_PARKED_1,			2,	(ARM_state_e[]){ACT_ARM_POS_PRE_PARKED_2});
	add_perm_transitions_table(ACT_ARM_POS_PRE_PARKED_2,			2,	(ARM_state_e[]){ACT_ARM_POS_PARKED, ACT_ARM_POS_PRE_PARKED_1});
	add_perm_transitions_table(ACT_ARM_POS_PARKED,					2,	(ARM_state_e[]){ACT_ARM_POS_MID, ACT_ARM_POS_PRE_PARKED_2});
	add_perm_transitions_table(ACT_ARM_POS_MID,						2,	(ARM_state_e[]){ACT_ARM_POS_OPEN, ACT_ARM_POS_PARKED});

	add_perm_transitions_table(ACT_ARM_POS_OPEN,					4,	(ARM_state_e[]){ACT_ARM_POS_MID, ACT_ARM_POS_ON_TORCHE,
																						ACT_ARM_POS_ON_TORCHE_SMALL_ARM,
																						ACT_ARM_POS_PREPARE_TAKE_ON_ROAD});

	add_perm_transitions_table(ACT_ARM_POS_ON_TORCHE,				1,	(ARM_state_e[]){ACT_ARM_POS_OPEN});
	add_perm_transitions_table(ACT_ARM_POS_ON_TORCHE_SMALL_ARM,		1,	(ARM_state_e[]){ACT_ARM_POS_OPEN});

	add_perm_transitions_table(ACT_ARM_POS_PREPARE_TAKE_ON_ROAD,	3,	(ARM_state_e[]){ACT_ARM_POS_OPEN, ACT_ARM_POS_TAKE_ON_ROAD, ACT_ARM_POS_TAKE_ON_ROAD_MAMOUTH});
	add_perm_transitions_table(ACT_ARM_POS_TAKE_ON_ROAD,			1,	(ARM_state_e[]){ACT_ARM_POS_PREPARE_TAKE_ON_ROAD});
	add_perm_transitions_table(ACT_ARM_POS_TAKE_ON_ROAD_MAMOUTH,	1,	(ARM_state_e[]){ACT_ARM_POS_PREPARE_TAKE_ON_ROAD});
#endif
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
