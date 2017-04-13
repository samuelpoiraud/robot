/*
 *  Club Robot ESEO 2013 - 2015
 *
 *  $Id$
 *
 *  Package : Carte Actionneur
 *  Description : Configuration des modules de QS
 *  Auteur : Jacen, Alexis
 */

#ifndef BIG_CONFIG_USE_H
#define BIG_CONFIG_USE_H

/* Definition des périphériques, connectiques capteurs et actionneurs du gros robot */


/**************** Périphériques ****************/

/* Réglages PWM */
	#define USE_PWM_MODULE
		#define PWM_FREQ	50000
		#define USE_PWM3	// Moteur rouleau
		#define USE_PWM4	// Moteur tourniquet

/* Réglages DC Speed */
	#define USE_DC_MOTOR_SPEED
		#define DC_MOTOR_SPEED_NUMBER 		2
		#define DC_MOTOR_SPEED_TIME_PERIOD	10

/* Servo-Moteurs standart */
	//#define SERVO_TIMER 5
	//#define USE_SERVO

/* Asservissement en position/vitesse de moteurs CC */
	/* déclarer l'utilisation du pilote */
	//#define USE_DCMOTOR2
	/* définir le nombre d'actionneurs asservis */
	//#define DCM_NUMBER			0
	/* Période d'asservisement (en ms) */
	//#define DCM_TIME_PERIOD		10
	/* nombre maximum de positions à gérer par moteur */
	//#define DCMOTOR_NB_POS		1

/* Réglages QEI */
	//#define USE_QUEI1
	//#define USE_QUEI2

/* Réglages ADC */
	#define USE_AN9
	#define ADC_MOSFETS_5V		ADC_9


/* Réglages de la carte Mosfets*/
	#define USE_MOSFETS_MODULE
	#define USE_MOSFET_1
	#define USE_MOSFET_2
	#define USE_MOSFET_3
	#define USE_MOSFET_4
	#define USE_MOSFET_5
	#define USE_MOSFET_6
	#define USE_MOSFET_7
	#define USE_MOSFET_8
	//#define USE_MOSFET_MULTI

/* Réglages mosfet */
	#define USE_MOSTFET_BOARD
		#define MOSFET_BOARD_UART	6
		//#define USE_MOSFET_VIA_UART

/* Réglages uart */
	#define USE_UART6
		#define UART6_BAUDRATE				19200

		#define USE_UART6_TX_BUFFER
			#define UART6_TX_BUFFER_SIZE 	128

		#define USE_UART6_RX_BUFFER
			#define UART6_RX_BUFFER_SIZE 	128

/* Réglages IT externe */
	#define USE_EXTERNAL_IT
	#define USE_RPM_SENSOR
	#define RPM_SENSOR_NB_SENSOR	2

#endif /* BIG_CONFIG_USE_H */
