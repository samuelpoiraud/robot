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


#ifndef ARM_DATA_H
#define	ARM_DATA_H

#include "../QS/QS_all.h"
#include "Arm_config.h"
#include "../QS/QS_DCMotor2.h"

typedef int arm_state_t[ARM_ACT_NUMBER];

typedef enum {
	ARM_ST_Parked,
	ARM_ST_Open,
	ARM_ST_NUMBER
} ARM_state_e;

typedef enum {
	ARM_DCMOTOR,
	ARM_AX12,
	ARM_RX24 = ARM_AX12
} ARM_motor_type_e;

typedef struct {
	ARM_motor_type_e type;
	Uint8 id;
	Uint16 timeout;
	Uint16 epsilon;
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

extern ARM_motor_data_t arm_motors[ARM_ACT_NUMBER];

extern arm_state_t arm_states[ARM_ST_NUMBER];

//                                    lignes         colonnes
//                                   ancien état    nouvel état
extern Uint8 arm_states_transitions[ARM_ST_NUMBER][ARM_ST_NUMBER];

#endif	/* ARM_DATA_H */

