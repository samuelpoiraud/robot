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


	#define USE_SHOVEL_DUNE_HELPER

/**************** Périphériques ****************/

/* Réglages PWM */
	#define USE_PWM_MODULE
		#define PWM_FREQ	50000
		#define USE_PWM1
		#define USE_PWM2

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

/* Utilisation de la carte Mosfets*/
	/* déclarer l'utilisation du pilote */
	#define USE_MOSFETS
	/*Nombre de mosfets à piloter (max = 8)*/
	#define NB_MOSFETS          8


#endif /* BIG_CONFIG_USE_H */
