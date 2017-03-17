/*  Club Robot ESEO 2016 - 2017
 *
 *	Fichier : MOTOR_TURN_TRIHOLE_config.h
 *	Package : Carte actionneur
 *	Description : Gestion de l'actionneur MOTOR_TURN_TRIHOLE
 *  Auteur : Arnaud
 *  Version 2017
 *  Robot : BIG
 */


#ifndef MOTOR_TURN_TRIHOLE_CONFIG_H
	#define	MOTOR_TURN_TRIHOLE_CONFIG_H

	#include "../../../stm32f4xx/stm32f4xx_gpio.h"
	#include "../QS/QS_external_it.h"
	#include "../QS/QS_types.h"

// Configuration moteurs
	#define MOTOR_TURN_TRIHOLE_PWM_NUM					2
	#define MOTOR_TURN_TRIHOLE_PORT_WAY					GPIOC
	#define MOTOR_TURN_TRIHOLE_PORT_WAY_BIT				GPIO_PinSource10
	#define MOTOR_TURN_TRIHOLE_MAX_PWM					100
	#define MOTOR_TURN_TRIHOLE_SIMUATE_WAY				TRUE
	#define MOTOR_TURN_TRIHOLE_RECOVERY_MODE			TRUE
	#define MOTOR_TURN_TRIHOLE_RECOVERY_TIME			1500

// Configuration capteur vitesse
	#define MOTOR_TURN_TRIHOLE_PORT_SENSOR				EXTIT_GpioB
	#define MOTOR_TURN_TRIHOLE_PORT_SENSOR_BIT			GPIO_PinSource10
	#define MOTOR_TURN_TRIHOLE_SENSOR_EDGE				EXTIT_Edge_Rising
	#define MOTOR_TURN_TRIHOLE_SENSOR_TICK_PER_REV		3

// Configuration de l'asservissement PIDs
	#define MOTOR_TURN_TRIHOLE_KP						90
	#define MOTOR_TURN_TRIHOLE_KI						15
	#define MOTOR_TURN_TRIHOLE_KD						1
	#define MOTOR_TURN_TRIHOLE_KV						1

// Configuration de l'actionneur
	#define MOTOR_TURN_TRIHOLE_TIMEOUT					2000
	#define MOTOR_TURN_TRIHOLE_EPSILON_SPEED			2000

//Configuration des vitesses
	#define MOTOR_TURN_TRIHOLE_IDLE_SPEED				0		// [rpm]
	#define MOTOR_TURN_TRIHOLE_RUN_SPEED				10		// [rpm]
	#define MOTOR_TURN_TRIHOLE_RECOVERY_SPEED			-10		// [rpm]

//Vitesse initiale
	#define MOTOR_TURN_TRIHOLE_INIT_SPEED				MOTOR_TURN_TRIHOLE_IDLE_SPEED

//Extremum valeur
	#define MOTOR_TURN_TRIHOLE_MIN_VALUE				0
	#define MOTOR_TURN_TRIHOLE_MAX_VALUE				10

#endif	/* MOTOR_TURN_TRIHOLE_CONFIG_H */


