/*  Club Robot ESEO 2016 - 2017
 *
 *	Fichier : MOTOR_ROLLER_FOAM_config.h
 *	Package : Carte actionneur
 *	Description : Gestion de l'actionneur MOTOR_ROLLER_FOAM
 *  Auteur : Arnaud
 *  Version 2017
 *  Robot : BIG
 */


#ifndef MOTOR_ROLLER_FOAM_CONFIG_H
	#define	MOTOR_ROLLER_FOAM_CONFIG_H

	#include "../../../stm32f4xx/stm32f4xx_gpio.h"
	#include "../QS/QS_external_it.h"
	#include "../QS/QS_types.h"

// Configuration moteurs
	#define MOTOR_ROLLER_FOAM_PWM_NUM					3
	#define MOTOR_ROLLER_FOAM_PORT_WAY					GPIOC
	#define MOTOR_ROLLER_FOAM_PORT_WAY_BIT				GPIO_Pin_11
	#define MOTOR_ROLLER_FOAM_MAX_PWM					100
	#define MOTOR_ROLLER_FOAM_SIMUATE_WAY				TRUE
	#define MOTOR_ROLLER_FOAM_RECOVERY_MODE				FALSE
	#define MOTOR_ROLLER_FOAM_RECOVERY_TIME				1500

// Configuration capteur vitesse
	#define MOTOR_ROLLER_FOAM_PORT_SENSOR				RPM_SENSOR_GPIO_A
	#define MOTOR_ROLLER_FOAM_PORT_SENSOR_BIT			RPM_SENSOR_PIN_7
	#define MOTOR_ROLLER_FOAM_SENSOR_EDGE				RPM_SENSOR_Edge_Rising
	#define MOTOR_ROLLER_FOAM_SENSOR_TICK_PER_REV		8

// Configuration de l'asservissement PIDs
	#define MOTOR_ROLLER_FOAM_KP						0
	#define MOTOR_ROLLER_FOAM_KI						0
	#define MOTOR_ROLLER_FOAM_KD						0
	#define MOTOR_ROLLER_FOAM_KV						177			// (Réduction / ConstanteVitesseMoteur * MaxPWM * 1024) / TensionAlimentation
																	// Constante de vitesse moteur : 460
																	// Réduction : 19:1

// Configuration de l'actionneur
	#define MOTOR_ROLLER_FOAM_TIMEOUT					2000
	#define MOTOR_ROLLER_FOAM_EPSILON_SPEED				2000

//Configuration des vitesses
	#define MOTOR_ROLLER_FOAM_RECOVERY_SPEED			-10		// [rpm]

//Vitesse initiale
	#define MOTOR_ROLLER_FOAM_INIT_SPEED				MOTOR_ROLLER_FOAM_IDLE_SPEED

//Extremum valeur
	#define MOTOR_ROLLER_FOAM_MIN_VALUE					0
	#define MOTOR_ROLLER_FOAM_MAX_VALUE					10

#endif	/* MOTOR_ROLLER_FOAM_CONFIG_H */


