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


#ifndef ARM_DATA_H
#define	ARM_DATA_H

#include "../QS/QS_all.h"
#ifdef I_AM_ROBOT_BIG
	#include "../Pierre/Arm_config.h"
#else
	#include "../Guy/Arm_config.h"
#endif
#include "../QS/QS_DCMotor2.h"
#include "../QS/QS_CANmsgList.h"

typedef enum {
	ARM_DCMOTOR,
	ARM_AX12,
	ARM_RX24 = ARM_AX12  //m�me gestion pour les 2
} ARM_motor_type_e;

typedef struct {
	ARM_motor_type_e type;
	Uint8 id;
	Uint16 timeout;
	Uint16 epsilon;
	Uint16 large_epsilon;
	Sint16 min_value;
	Sint16 max_value;
	Uint8 maxPowerWay0;	//aussi le max torque pour les ax12 & rx24

	//valide que pour DCMotor:
	Uint8 maxPowerWay1;
	Uint8 pwmNum;
	GPIO_TypeDef* pwmWayPort;
	Uint8 pwmWayBit;
	Uint16 kp;
	Uint16 ki;
	Uint16 kd;
	sensor_read_fun_t sensorRead;
} ARM_motor_data_t;

extern const ARM_motor_data_t ARM_MOTORS[];
extern const Uint8 ARM_MOTORS_NUMBER;

//                                    lignes         colonnes
//                                   ancien �tat    nouvel �tat
extern bool_e arm_states_transitions[ARM_ST_NUMBER][ARM_ST_NUMBER];

// Etats � prendre pour initialiser le bras dans une position connue
extern const ARM_state_e ARM_INIT[];
extern const Uint8 ARM_INIT_NUMBER;

extern bool_e ARM_ax12_is_initialized[];
Sint16 ARM_get_motor_pos(ARM_state_e state, Uint8 motor);
Sint16 ARM_readDCMPos();
void init_perm_transitions_table();
bool_e check_all_state_perm_transitions_initialized();

#endif	/* ARM_DATA_H */

