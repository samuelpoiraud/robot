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
	#define MOTOR_TURN_TRIHOLE_PWM_NUM					4
	#define MOTOR_TURN_TRIHOLE_PORT_WAY					GPIOC
	#define MOTOR_TURN_TRIHOLE_PORT_WAY_BIT				GPIO_Pin_14
	#define MOTOR_TURN_TRIHOLE_MAX_PWM					100
	#define MOTOR_TURN_TRIHOLE_SIMUATE_WAY				TRUE
	#define MOTOR_TURN_TRIHOLE_RECOVERY_MODE			TRUE
	#define MOTOR_TURN_TRIHOLE_RECOVERY_TIME			1500

// Configuration capteur vitesse
	#define MOTOR_TURN_TRIHOLE_PORT_SENSOR				RPM_SENSOR_GPIO_C
	#define MOTOR_TURN_TRIHOLE_PORT_SENSOR_BIT			RPM_SENSOR_PIN_2
	#define MOTOR_TURN_TRIHOLE_SENSOR_EDGE				RPM_SENSOR_Edge_Rising
	#define MOTOR_TURN_TRIHOLE_SENSOR_TICK_PER_REV		((Uint32)(2*9.7))

// Configuration de l'asservissement PIDs
	#define MOTOR_TURN_TRIHOLE_KP						0
	#define MOTOR_TURN_TRIHOLE_KI						0
	#define MOTOR_TURN_TRIHOLE_KD						0
	#define MOTOR_TURN_TRIHOLE_KV						167			// (Réduction / ConstanteVitesseMoteur * MaxPWM * 1024) / TensionAlimentation
																	// Constante de vitesse moteur : 247	CXR
																	// Constante de vitesse moteur : 274	CR
																	// Réduction : 9.7:1

// Configuration de l'actionneur
	#define MOTOR_TURN_TRIHOLE_TIMEOUT					2000
	#define MOTOR_TURN_TRIHOLE_EPSILON_SPEED			2000

//Configuration des vitesses
	#define MOTOR_TURN_TRIHOLE_RECOVERY_SPEED			-10		// [rpm]

//Vitesse initiale
	#define MOTOR_TURN_TRIHOLE_INIT_SPEED				MOTOR_TURN_TRIHOLE_IDLE_SPEED

//Extremum valeur
	#define MOTOR_TURN_TRIHOLE_MIN_VALUE				0
	#define MOTOR_TURN_TRIHOLE_MAX_VALUE				10

#endif	/* MOTOR_TURN_TRIHOLE_CONFIG_H */


