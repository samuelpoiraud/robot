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
	//	#define USE_PWM1
	//	#define USE_PWM2

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

/* Réglages de la carte Mosfets*/
	#define USE_MOSFETS_MODULE
	//	 #define USE_MOSFET_1
	//	 #define USE_MOSFET_2
	//	 #define USE_MOSFET_3
	//	 #define USE_MOSFET_4
	//	 #define USE_MOSFET_5
	//	 #define USE_MOSFET_6
	//	 #define USE_MOSFET_7
	//	 #define USE_MOSFET_8
	//	 #define USE_MOSFET_MULTI


#endif /* BIG_CONFIG_USE_H */
