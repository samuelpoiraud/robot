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
		{-168+OFFSET_BIG_UP_DOWN	,240	,49+OFFSET_BIG_AX12_MID,	43},	//ARM_ST_Pre_Parked_1
		{-168+OFFSET_BIG_UP_DOWN	,217	,32+OFFSET_BIG_AX12_MID,	43},	//ARM_ST_Pre_Parked_2
		{-131+OFFSET_BIG_UP_DOWN	,217	,32+OFFSET_BIG_AX12_MID,	43},	//ARM_ST_Parked
		{-131+OFFSET_BIG_UP_DOWN	,180	,29+OFFSET_BIG_AX12_MID,	43},	//ARM_ST_Mid
		{-131+OFFSET_BIG_UP_DOWN	,180	,84+OFFSET_BIG_AX12_MID,	43},	//ARM_ST_Open

		{-131+OFFSET_BIG_UP_DOWN	,200	,161,		54},	//ARM_ST_Open_2
		{0		,0		,0		,0},	//ARM_ST_On_Torche						NOT USE			// Distance factice
		{-131+OFFSET_BIG_UP_DOWN	,212	,200+OFFSET_BIG_AX12_MID,	43},	//ARM_ST_On_Torche_Small_Arm
		{-131+OFFSET_BIG_UP_DOWN	,224	,164+OFFSET_BIG_AX12_MID,	43},	//ARM_ST_On_Torche_Small_Arm_Rescue
		{-131+OFFSET_BIG_UP_DOWN	,159	,23+OFFSET_BIG_AX12_MID,	43},	//ARM_ST_Prepare_1_Torche_Auto

		{-99+OFFSET_BIG_UP_DOWN	,159	,23+OFFSET_BIG_AX12_MID,	43},	//ARM_ST_Prepare_2_Torche_Auto
		{-99+OFFSET_BIG_UP_DOWN	,163	,22+OFFSET_BIG_AX12_MID,	158},	//ARM_ST_On_Torche_Auto
		{-71+OFFSET_BIG_UP_DOWN	,159	,23+OFFSET_BIG_AX12_MID,	158},	//ARM_ST_On_Torche_Auto_Escape_1
		{-71+OFFSET_BIG_UP_DOWN	,159	,120+OFFSET_BIG_AX12_MID,	158},	//ARM_ST_On_Torche_Auto_Escape_2


			// Position de dépose des triangles de pierre
		{-71	,151	,200+OFFSET_BIG_AX12_MID,	43},	//ARM_ST_On_Prepare_Drop_1_Auto
		{-71	,151	,270+OFFSET_BIG_AX12_MID,	43},	//ARM_ST_On_Prepare_Drop_2_Auto
		{-71.	,151	,270+OFFSET_BIG_AX12_MID,	43},	//ARM_ST_On_Drop_1_Auto
		{-71	,157	,127	,152},	//ARM_ST_On_Drop_2_Auto
		{-131	,194	,133	,154},	//ARM_ST_On_Prepare_1_Drop_3_Auto
		{-200	,194	,133	,154},	//ARM_ST_On_Prepare_2_Drop_3_Auto
		{-200   ,176	,134	,154},	//ARM_ST_On_Drop_3_Auto

		{0		,0		,0		,0},	//ARM_ST_To_Storage						NOT USE
		{-221+OFFSET_BIG_UP_DOWN	,238	,265+OFFSET_BIG_AX12_MID	,154},	//ARM_ST_To_Carry
		{-131+OFFSET_BIG_UP_DOWN	,146	,134+OFFSET_BIG_AX12_MID	,56},	//ARM_ST_To_Prepare_Return
		{-71+OFFSET_BIG_UP_DOWN	,146	,134+OFFSET_BIG_AX12_MID	,56},	//ARM_ST_To_Down_Return

		{-71+OFFSET_BIG_UP_DOWN	,146	,134+OFFSET_BIG_AX12_MID	,56},	//ARM_ST_To_Return
		{0		,0		,0		,0},	//ARM_ST_Wait_Return					NOT USE
		{0		,0		,0		,0},	//ARM_ST_To_Prepare_Take_Return			NOT USE
		{0		,0		,0		,0},	//ARM_ST_To_Take_Return					NOT USE
		{-71+OFFSET_BIG_UP_DOWN	,146	,134+OFFSET_BIG_AX12_MID	,152},	//ARM_ST_To_Unblock_Return

		{0		,0		,0		,0},	//ARM_ST_To_Unblock_Return_Up
		{0		,0		,0		,0},	//ARM_ST_On_Triangle					NOT USE			// Distance factice
		{0		,0		,0		,0},	//ARM_ST_Prepare_Backward				NOT USE
		{0		,0		,0		,0},	//ARM_ST_Down_Backward					NOT USE
		{0		,0		,0		,0},	//ARM_ST_Lock_Backward					NOT USE

		{-71+OFFSET_BIG_UP_DOWN	,214	,214+OFFSET_BIG_AX12_MID,	244},	//ARM_ST_Prepare_Take_On_Edge
		{-181+OFFSET_BIG_UP_DOWN	,214	,214+OFFSET_BIG_AX12_MID	,244},	//ARM_ST_Prepare_Take_On_Edge_2
		{-181+OFFSET_BIG_UP_DOWN	,214	,214+OFFSET_BIG_AX12_MID	,161},	//ARM_ST_Take_On_Edge
		{0		,0		,0		,0},	//ARM_ST_Return_On_Edge					NOT USE
		{-221+OFFSET_BIG_UP_DOWN	,205	,204+OFFSET_BIG_AX12_MID	,169},	//ARM_ST_Disposed_Simple

		{-145	,150	,86,	42},	//ARM_ST_Prepare_Take_On_Road
		{-353	,178	,112,	42},	//ARM_ST_Take_On_Road
		{-395	,152	,88,	47},	//ARM_ST_Take_On_Road_Mamouth
		{-71+OFFSET_BIG_UP_DOWN	,171	,140+OFFSET_BIG_AX12_MID	,152},	//ARM_ST_Deposed_Torch
		{-71+OFFSET_BIG_UP_DOWN	,171	,140+OFFSET_BIG_AX12_MID	,243},	//ARM_ST_Escape_Torch_1

		{-71+OFFSET_BIG_UP_DOWN	,182	,200+OFFSET_BIG_AX12_MID	,243},	//ARM_ST_Escape_Torch_2
		{0		,0		,0		,0},  // ACT_ARM_POS_TORCHE_CENTRAL				NOT USE
		{0		,0		,0		,0}  // ACT_ARM_POS_TORCHE_ADV					NOT USE
		};
#else
	const Sint16 ARM_STATES[ARM_ST_NUMBER][sizeof(ARM_MOTORS) / sizeof(ARM_motor_data_t)] = {
		// moteur (dans l'ordre)
		{0		,0		,0		,0},	//ARM_ST_Pre_Parked_1				NOT USE
		{0		,0		,0		,0},	//ARM_ST_Pre_Parked_2				NOT USE
		{-432+OFFSET_SMALL_UPDOWN	,154	,283,	249},	//ARM_ST_Parked
		{-432+OFFSET_SMALL_UPDOWN	,152	,246,	245},	//ARM_ST_Mid
		{-432+OFFSET_SMALL_UPDOWN	,145	,218,	SMALL_VENTOUSE_HORIZONTALE},	//ARM_ST_Open

		{0		,0		,0		,0},	//ARM_ST_Open_2		NOT USE
		{0		,0		,0		,0},	//ARM_ST_On_Torche				// Distance factice
		{0		,0		,0		,0},	//ARM_ST_On_Torche_Small_Arm		NOT USE
		{0		,0		,0		,0},	//ARM_ST_On_Torche_Small_Arm_Rescue	NOT USE
		{0		,0		,0		,0},	//ARM_ST_Prepare_1_Torche_Auto		NOT USE

		{0		,0		,0		,0},	//ARM_ST_Prepare_2_Torche_Auto		NOT USE
		{0		,0		,0		,0},	//ARM_ST_On_Torche_Auto				NOT USE
		{-131+OFFSET_SMALL_UPDOWN	,159	,23,	158},	//ARM_ST_On_Torche_Auto_Escape_1	NOT USE
		{-131+OFFSET_SMALL_UPDOWN	,159	,120,	158},	//ARM_ST_On_Torche_Auto_Escape_2	NOT USE
		{0		,0		,0		,0},	//ARM_ST_Prepare_Drop_1_Auto			NOT USE

		{0		,0		,0		,0},	//ARM_ST_Prepare_Drop_2_Auto			NOT USE
		{0		,0		,0		,0},	//ARM_ST_On_Drop_1_Auto				NOT USE
		{0		,0		,0		,0},	//ARM_ST_On_Drop_2_Auto				NOT USE
		{0		,0		,0		,0},	//ARM_ST_On_Prepare1_Drop_3_Auto		NOT USE
		{0		,0		,0		,0},	//ARM_ST_On_Prepare2_Drop_3_Auto		NOT USE

		{0		,0		,0		,0},	//ARM_ST_On_Drop_3_Auto				NOT USE
		{0		,0		,0		,0},	//ARM_ST_To_Storage					NOT USE
		{-351+OFFSET_SMALL_UPDOWN	,169	,280,	233},	//ARM_ST_To_Carry
		{-432+OFFSET_SMALL_UPDOWN	,158	,102,	238},	//ARM_ST_To_Prepare_Return
		{-432+OFFSET_SMALL_UPDOWN	,158	,107,	238},	//ARM_ST_To_Down_Return

		{-432+OFFSET_SMALL_UPDOWN	,186	,171,	200},	//ARM_ST_To_Return
		{-432+OFFSET_SMALL_UPDOWN	,133	,65,	120},	//ARM_ST_Wait_Return
		{-432+OFFSET_SMALL_UPDOWN	,115	,42,	148},	//ARM_ST_To_Prepare_Take_Return
		{-432+OFFSET_SMALL_UPDOWN	,132	,80,	SMALL_VENTOUSE_HORIZONTALE},	//ARM_ST_To_Take_Return
		{-432+OFFSET_SMALL_UPDOWN	,132	,56,	110},	//ARM_ST_To_Unblock_Return

		{-432+OFFSET_SMALL_UPDOWN	,132	,56,	SMALL_VENTOUSE_HORIZONTALE},	//ARM_ST_To_Unblock_Return_Up
		{0		,0		,0		,0},	//ARM_ST_On_Triangle			// Distance factice
		{-432+OFFSET_SMALL_UPDOWN	,113	,151,	79},	//ARM_ST_Prepare_Backward
		{-360+OFFSET_SMALL_UPDOWN	,113	,154,	79},	//ARM_ST_Down_Backward
		{-360+OFFSET_SMALL_UPDOWN	,113	,154,	148},	//ARM_ST_Lock_Backward

		{-432+OFFSET_SMALL_UPDOWN	,119	,160,	145},	//ARM_ST_Prepare_Take_On_Edge
		{0		,0		,0		,0 },	//ARM_ST_Prepare_Take_On_Edge_2					NOT USE
		{-261+OFFSET_SMALL_UPDOWN	,119	,160,	145},	//ARM_ST_Take_On_Edge
		{-309+OFFSET_SMALL_UPDOWN	,114	,160,	SMALL_VENTOUSE_HORIZONTALE},	//ARM_ST_Return_On_Edge
		{-236+OFFSET_SMALL_UPDOWN	,172	,241,	145},	//ARM_ST_Disposed_Simple

		{0		,0		,0		,0},	//ARM_ST_Prepare_Take_On_Road		NOT USE
		{-171+OFFSET_SMALL_UPDOWN	,174	,260	,SMALL_VENTOUSE_HORIZONTALE},	//ARM_ST_Take_On_Road
		{0		,0		,0		,0},	//ARM_ST_Take_On_Road_Mamouth		NOT USE
		{0		,0		,0		,0},	//ARM_ST_Deposed_Torch				NOT USE
		{0		,0		,0		,0},	//ARM_ST_Escape_Torch_1				NOT USE

		{0		,0		,0		,0},	//ARM_ST_Escape_Torch_2				NOT USE
		{-430+OFFSET_SMALL_UPDOWN	,149	,89 	,SMALL_VENTOUSE_HORIZONTALE},	 // ACT_ARM_POS_TORCHE_CENTRAL
		{-430+OFFSET_SMALL_UPDOWN	,129	,72 	,SMALL_VENTOUSE_HORIZONTALE}	 // ACT_ARM_POS_TORCHE_ADV
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
	add_perm_transitions_table(ACT_ARM_POS_OPEN,					13,	(ARM_state_e[]){ACT_ARM_POS_MID, ACT_ARM_POS_ON_TORCHE,
																						ACT_ARM_POS_TO_PREPARE_RETURN, ACT_ARM_POS_ON_TRIANGLE, ACT_ARM_POS_TO_CARRY,
																						ACT_ARM_POS_TO_PREPARE_TAKE_RETURN, ACT_ARM_POS_PREPARE_BACKWARD,
																						ACT_ARM_POS_TAKE_ON_ROAD, ACT_ARM_POS_PREPARE_TAKE_ON_EDGE,
																						ACT_ARM_POS_RETURN_ON_EDGE, ACT_ARM_POS_DISPOSED_SIMPLE,
																						ACT_ARM_POS_TORCHE_CENTRAL,ACT_ARM_POS_TORCHE_ADV});

	add_perm_transitions_table(ACT_ARM_POS_ON_TORCHE,				3,	(ARM_state_e[]){ACT_ARM_POS_OPEN, ACT_ARM_POS_ON_TRIANGLE, ACT_ARM_POS_TO_PREPARE_RETURN});

	add_perm_transitions_table(ACT_ARM_POS_TO_CARRY,				1,	(ARM_state_e[]){ACT_ARM_POS_OPEN});

	add_perm_transitions_table(ACT_ARM_POS_TO_PREPARE_RETURN,		2,	(ARM_state_e[]){ACT_ARM_POS_OPEN, ACT_ARM_POS_TO_DOWN_RETURN});
	add_perm_transitions_table(ACT_ARM_POS_TO_DOWN_RETURN,			1,	(ARM_state_e[]){ACT_ARM_POS_TO_RETURN});
	add_perm_transitions_table(ACT_ARM_POS_TO_RETURN,				1,	(ARM_state_e[]){ACT_ARM_POS_WAIT_RETURN});

	add_perm_transitions_table(ACT_ARM_POS_WAIT_RETURN,				3,	(ARM_state_e[]){ACT_ARM_POS_TO_TAKE_RETURN, ACT_ARM_POS_OPEN,ACT_ARM_POS_TORCHE_ADV});

	add_perm_transitions_table(ACT_ARM_POS_TO_PREPARE_TAKE_RETURN,	2,	(ARM_state_e[]){ACT_ARM_POS_OPEN, ACT_ARM_POS_TO_TAKE_RETURN});
	add_perm_transitions_table(ACT_ARM_POS_TO_TAKE_RETURN,			1,	(ARM_state_e[]){ACT_ARM_POS_TO_UNBLOCK_RETURN});
	add_perm_transitions_table(ACT_ARM_POS_TO_UNBLOCK_RETURN,		1,	(ARM_state_e[]){ACT_ARM_POS_TO_UNBLOCK_RETURN_UP});
	add_perm_transitions_table(ACT_ARM_POS_TO_UNBLOCK_RETURN_UP,	3,	(ARM_state_e[]){ACT_ARM_POS_OPEN,
																						ACT_ARM_POS_ON_TORCHE,
																						ACT_ARM_POS_ON_TRIANGLE});

	add_perm_transitions_table(ACT_ARM_POS_ON_TRIANGLE,				2,	(ARM_state_e[]){ACT_ARM_POS_OPEN, ACT_ARM_POS_ON_TORCHE});

	add_perm_transitions_table(ACT_ARM_POS_PREPARE_BACKWARD,		2,	(ARM_state_e[]){ACT_ARM_POS_OPEN, ACT_ARM_POS_DOWN_BACKWARD});
	add_perm_transitions_table(ACT_ARM_POS_DOWN_BACKWARD,			2,	(ARM_state_e[]){ACT_ARM_POS_PREPARE_BACKWARD, ACT_ARM_POS_LOCK_BACKWARD});
	add_perm_transitions_table(ACT_ARM_POS_LOCK_BACKWARD,			1,	(ARM_state_e[]){ACT_ARM_POS_DOWN_BACKWARD});

	add_perm_transitions_table(ACT_ARM_POS_PREPARE_TAKE_ON_EDGE,	2,	(ARM_state_e[]){ACT_ARM_POS_OPEN, ACT_ARM_POS_TAKE_ON_EDGE});
	add_perm_transitions_table(ACT_ARM_POS_TAKE_ON_EDGE,			1,	(ARM_state_e[]){ACT_ARM_POS_PREPARE_TAKE_ON_EDGE});
	add_perm_transitions_table(ACT_ARM_POS_RETURN_ON_EDGE,			1,	(ARM_state_e[]){ACT_ARM_POS_OPEN});
	add_perm_transitions_table(ACT_ARM_POS_DISPOSED_SIMPLE,			1,	(ARM_state_e[]){ACT_ARM_POS_OPEN});

	add_perm_transitions_table(ACT_ARM_POS_TAKE_ON_ROAD,			1,	(ARM_state_e[]){ACT_ARM_POS_OPEN});
	add_perm_transitions_table(ACT_ARM_POS_TORCHE_CENTRAL,			1,	(ARM_state_e[]){ACT_ARM_POS_OPEN});
	add_perm_transitions_table(ACT_ARM_POS_TORCHE_ADV,				2,	(ARM_state_e[]){ACT_ARM_POS_OPEN,ACT_ARM_POS_WAIT_RETURN});

#else
	add_perm_transitions_table(ACT_ARM_POS_PRE_PARKED_1,			1,	(ARM_state_e[]){ACT_ARM_POS_PRE_PARKED_2});
	add_perm_transitions_table(ACT_ARM_POS_PRE_PARKED_2,			2,	(ARM_state_e[]){ACT_ARM_POS_PARKED, ACT_ARM_POS_PRE_PARKED_1});
	add_perm_transitions_table(ACT_ARM_POS_PARKED,					2,	(ARM_state_e[]){ACT_ARM_POS_MID, ACT_ARM_POS_PRE_PARKED_2});
	add_perm_transitions_table(ACT_ARM_POS_MID,						2,	(ARM_state_e[]){ACT_ARM_POS_OPEN, ACT_ARM_POS_PARKED});

	add_perm_transitions_table(ACT_ARM_POS_OPEN,					4,	(ARM_state_e[]){ACT_ARM_POS_MID,
																						ACT_ARM_POS_OPEN_2,
																						ACT_ARM_POS_ON_PREPARE_1_DROP_3_AUTO,
																						ACT_ARM_POS_ON_PREPARE_DROP_1_AUTO});

	add_perm_transitions_table(ACT_ARM_POS_OPEN_2,					9,	(ARM_state_e[]){ACT_ARM_POS_OPEN,
																						ACT_ARM_POS_ON_TORCHE_SMALL_ARM,
																						ACT_ARM_POS_PREPARE_TAKE_ON_ROAD,
																						ACT_ARM_POS_PREPARE_1_TORCHE_AUTO,
																						ACT_ARM_POS_ON_PREPARE_DROP_1_AUTO,
																						ACT_ARM_POS_TO_PREPARE_RETURN,
																						ACT_ARM_POS_DISPOSED_TORCH,
																						ACT_ARM_POS_ON_TORCHE_SMALL_ARM_RESCUE,
																						ACT_ARM_POS_PREPARE_TAKE_ON_EDGE});

	add_perm_transitions_table(ACT_ARM_POS_ON_TORCHE_SMALL_ARM,			1,	(ARM_state_e[]){ACT_ARM_POS_OPEN_2});
	add_perm_transitions_table(ACT_ARM_POS_ON_TORCHE_SMALL_ARM_RESCUE,	1,	(ARM_state_e[]){ACT_ARM_POS_OPEN_2});
	add_perm_transitions_table(ACT_ARM_POS_PREPARE_1_TORCHE_AUTO,		2,	(ARM_state_e[]){ACT_ARM_POS_OPEN_2,
																						ACT_ARM_POS_PREPARE_2_TORCHE_AUTO});
	add_perm_transitions_table(ACT_ARM_POS_PREPARE_2_TORCHE_AUTO,		2,	(ARM_state_e[]){ACT_ARM_POS_PREPARE_1_TORCHE_AUTO,
																						ACT_ARM_POS_ON_TORCHE_AUTO});
	add_perm_transitions_table(ACT_ARM_POS_ON_TORCHE_AUTO,				1,	(ARM_state_e[]){ACT_ARM_POS_ON_TORCHE_AUTO_ESCAPE_1});
	add_perm_transitions_table(ACT_ARM_POS_ON_TORCHE_AUTO_ESCAPE_1,		1,	(ARM_state_e[]){ACT_ARM_POS_ON_TORCHE_AUTO_ESCAPE_2});
	add_perm_transitions_table(ACT_ARM_POS_ON_TORCHE_AUTO_ESCAPE_2,		2,	(ARM_state_e[]){ACT_ARM_POS_OPEN_2, ACT_ARM_POS_ON_PREPARE_1_DROP_3_AUTO});

	add_perm_transitions_table(ACT_ARM_POS_ON_PREPARE_DROP_1_AUTO,	2,	(ARM_state_e[]){ACT_ARM_POS_ON_PREPARE_DROP_2_AUTO, ACT_ARM_POS_OPEN_2});
	add_perm_transitions_table(ACT_ARM_POS_ON_PREPARE_DROP_2_AUTO,	2,	(ARM_state_e[]){ACT_ARM_POS_ON_DROP_1_AUTO, ACT_ARM_POS_ON_PREPARE_DROP_1_AUTO});
	add_perm_transitions_table(ACT_ARM_POS_ON_DROP_1_AUTO,			2,	(ARM_state_e[]){ACT_ARM_POS_ON_PREPARE_DROP_2_AUTO, ACT_ARM_POS_OPEN_2});

	add_perm_transitions_table(ACT_ARM_POS_ON_DROP_2_AUTO,			1,	(ARM_state_e[]){ACT_ARM_POS_OPEN_2});

	add_perm_transitions_table(ACT_ARM_POS_ON_PREPARE_1_DROP_3_AUTO,	2,	(ARM_state_e[]){ACT_ARM_POS_OPEN_2, ACT_ARM_POS_ON_PREPARE_2_DROP_3_AUTO});
	add_perm_transitions_table(ACT_ARM_POS_ON_PREPARE_2_DROP_3_AUTO,	2,	(ARM_state_e[]){ACT_ARM_POS_ON_DROP_3_AUTO, ACT_ARM_POS_ON_PREPARE_1_DROP_3_AUTO});
	add_perm_transitions_table(ACT_ARM_POS_ON_DROP_3_AUTO,			1,	(ARM_state_e[]){ACT_ARM_POS_ON_PREPARE_2_DROP_3_AUTO});


	add_perm_transitions_table(ACT_ARM_POS_TO_PREPARE_RETURN,		1,	(ARM_state_e[]){ACT_ARM_POS_TO_DOWN_RETURN});
	add_perm_transitions_table(ACT_ARM_POS_TO_DOWN_RETURN,			1,	(ARM_state_e[]){ACT_ARM_POS_TO_RETURN});
	add_perm_transitions_table(ACT_ARM_POS_TO_RETURN,				1,	(ARM_state_e[]){ACT_ARM_POS_TO_UNBLOCK_RETURN});
	add_perm_transitions_table(ACT_ARM_POS_TO_UNBLOCK_RETURN,		2,	(ARM_state_e[]){ACT_ARM_POS_OPEN_2, ACT_ARM_POS_ON_DROP_2_AUTO});

	add_perm_transitions_table(ACT_ARM_POS_PREPARE_TAKE_ON_ROAD,	3,	(ARM_state_e[]){ACT_ARM_POS_OPEN, ACT_ARM_POS_TAKE_ON_ROAD, ACT_ARM_POS_TAKE_ON_ROAD_MAMOUTH});
	add_perm_transitions_table(ACT_ARM_POS_TAKE_ON_ROAD,			1,	(ARM_state_e[]){ACT_ARM_POS_PREPARE_TAKE_ON_ROAD});
	add_perm_transitions_table(ACT_ARM_POS_TAKE_ON_ROAD_MAMOUTH,	1,	(ARM_state_e[]){ACT_ARM_POS_PREPARE_TAKE_ON_ROAD});

	add_perm_transitions_table(ACT_ARM_POS_DISPOSED_TORCH,			1,	(ARM_state_e[]){ACT_ARM_POS_ESCAPE_TORCH_1});
	add_perm_transitions_table(ACT_ARM_POS_ESCAPE_TORCH_1,			1,	(ARM_state_e[]){ACT_ARM_POS_ESCAPE_TORCH_2});
	add_perm_transitions_table(ACT_ARM_POS_ESCAPE_TORCH_2,			1,	(ARM_state_e[]){ACT_ARM_POS_OPEN_2});

	add_perm_transitions_table(ACT_ARM_POS_TAKE_ON_EDGE,			3,	(ARM_state_e[]){ACT_ARM_POS_OPEN_2,ACT_ARM_POS_DISPOSED_SIMPLE,ACT_ARM_POS_TO_CARRY});
	add_perm_transitions_table(ACT_ARM_POS_DISPOSED_SIMPLE,			1,	(ARM_state_e[]){ACT_ARM_POS_OPEN_2});
	add_perm_transitions_table(ACT_ARM_POS_PREPARE_TAKE_ON_EDGE,	1,	(ARM_state_e[]){ACT_ARM_POS_PREPARE_TAKE_ON_EDGE_2});
	add_perm_transitions_table(ACT_ARM_POS_PREPARE_TAKE_ON_EDGE_2,	1,	(ARM_state_e[]){ACT_ARM_POS_TAKE_ON_EDGE});

	add_perm_transitions_table(ACT_ARM_POS_TO_CARRY,				1,	(ARM_state_e[]){ACT_ARM_POS_TAKE_ON_EDGE});

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
