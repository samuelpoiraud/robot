/*
 *  Club Robot ESEO 2013 - 2014
 *
 *  $Id$
 *
 *  Package : Carte Actionneur
 *  Description : Donn�es pour la gestion du bras
 *  Auteur : Alexis
 *  Robot : Pierre & Guy
 */

#include "Arm_data.h"
#include "../QS/QS_adc.h"
#include "../QS/QS_CANmsgList.h"

// prefix vaut par exemple ARM_ACT_UPDOWN, prefix##_ID vaut alors ARM_ACT_UPDOWN_ID (## est un concatenation de texte)
#define DECLARE_DCMOTOR(prefix, sensorRead) { ARM_DCMOTOR, prefix##_ID, prefix##_ASSER_TIMEOUT, prefix##_ASSER_POS_EPSILON, prefix##_ASSER_POS_LARGE_EPSILON, prefix##_MIN_VALUE, prefix##_MAX_VALUE, prefix##_MAX_PWM_WAY0      , prefix##_MAX_PWM_WAY1, prefix##_PWM_NUM, prefix##_PORT_WAY, prefix##_PORT_WAY_BIT, prefix##_ASSER_KP, prefix##_ASSER_KI, prefix##_ASSER_KD, sensorRead }
#define DECLARE_AX12(prefix)                { ARM_AX12   , prefix##_ID, prefix##_ASSER_TIMEOUT, prefix##_ASSER_POS_EPSILON, prefix##_ASSER_POS_LARGE_EPSILON, prefix##_MIN_VALUE, prefix##_MAX_VALUE, prefix##_MAX_TORQUE_PERCENT, 0                    , 0               , 0                , 0                    , 0                , 0                , 0                , NULL       }
#define DECLARE_RX24(prefix)                { ARM_RX24   , prefix##_ID, prefix##_ASSER_TIMEOUT, prefix##_ASSER_POS_EPSILON, prefix##_ASSER_POS_LARGE_EPSILON, prefix##_MIN_VALUE, prefix##_MAX_VALUE, prefix##_MAX_TORQUE_PERCENT, 0                    , 0               , 0                , 0                    , 0                , 0                , 0                , NULL       }


// Liste de moteurs
const ARM_motor_data_t ARM_MOTORS[] = {
	//DECLARE_DCMOTOR(ARM_ACT_UPDOWN, &ARM_readDCMPos),
	DECLARE_RX24(ARM_ACT_RX24),
	DECLARE_AX12(ARM_ACT_AX12_MID),
	DECLARE_AX12(ARM_ACT_AX12_TRIANGLE)
};


// Position d'�tats (ligne = �tat, colonne = position moteur dans l'ordre de ARM_MOTORS)
#ifdef I_AM_ROBOT_BIG
	const Sint16 ARM_STATES[ARM_ST_NUMBER][sizeof(ARM_MOTORS) / sizeof(ARM_motor_data_t)] = {
	// moteur (dans l'ordre)
		{148	,282,	174},	//ARM_ST_Parked
		{148	,214,	145},	//ARM_ST_Open
		{176	,246,	145},	//ARM_ST_On_Torche
		{150	,289,	145},	//ARM_ST_To_Storage
		{140	,76,	240},	//ARM_ST_To_Return_Triangle
		{150	,150,	150}	//ARM_ST_On_Triangle
	};
#else
	const Sint16 ARM_STATES[ARM_ST_NUMBER][sizeof(ARM_MOTORS) / sizeof(ARM_motor_data_t)] = {
	// moteur (dans l'ordre)
		{148	,282,	174},	//ARM_ST_Parked
		{148	,214,	145},	//ARM_ST_Open
		{176	,246,	145},	//ARM_ST_On_Torche
		{150	,289,	145},	//ARM_ST_To_Storage
		{140	,76,	240},	//ARM_ST_To_Return_Triangle
		{150	,150,	150}	//ARM_ST_On_Triangle
	};
#endif





// Changement d'�tat possible (ligne -> colonne)
//                              lignes         colonnes
//                             ancien �tat    nouvel �tat
const bool_e ARM_STATES_TRANSITIONS[ARM_ST_NUMBER][ARM_ST_NUMBER] = {  //<LF>
//   ACT_ARM_POS_PARKED ACT_ARM_POS_OPEN  ACT_POS_ON_TORCHE		ACT_ARM_POS_TO_STORAGE	ACT_ARM_POS_TO_RETURN_TRIANGLE		ACT_ARM_POS_ON_TRIANGLE    <LF>
	{0                 ,1                 ,0					,0						,0									,0},  //ACT_ARM_POS_PARKED<LF>
	{1                 ,0                 ,1					,1						,1									,1},  //ACT_ARM_POS_OPEN<LF>
	{0                 ,1                 ,0					,1						,0									,0},  //ACT_ARM_POS_ON_TORCHE<LF>
	{0                 ,1                 ,1					,0						,0									,0},  //ACT_ARM_POS_TO_STORAGE<LF>
	{0                 ,1                 ,0					,0						,0									,0},  //ACT_ARM_POS_TO_RETURN_TRIANGLE<LF>
	{0                 ,1                 ,0					,0						,0									,0},  //ACT_ARM_POS_ON_TRIANGLE<LF>

};  //<LF>


// Etats � prendre pour initialiser le bras dans une position connue
const ARM_state_e ARM_INIT[] = {
	ACT_ARM_POS_OPEN,
	ACT_ARM_POS_PARKED
};


const Uint8 ARM_INIT_NUMBER = sizeof(ARM_INIT) / sizeof(ARM_state_e);


//Fonctions de r�cup�ration des positions des moteurs DC

Sint16 ARM_readDCMPos() {
	return -ADC_getValue(ARM_ACT_UPDOWN_ID_POTAR_ADC_ID);
}

//////////////////////////////////////////////////////////////////////////////////////////
/// Interne, � ne pas changer
//////////////////////////////////////////////////////////////////////////////////////////

//Utilisation d'une variable constant pour �viter de maintenir un define en plus de la liste de moteur quand on la change
const Uint8 ARM_MOTORS_NUMBER = sizeof(ARM_MOTORS) / sizeof(ARM_motor_data_t);
bool_e ARM_ax12_is_initialized[sizeof(ARM_MOTORS) / sizeof(ARM_motor_data_t)] = {0};

Sint16 ARM_get_motor_pos(ARM_state_e state, Uint8 motor) {
	return ARM_STATES[state][motor];
}
