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

/* Definition des p�riph�riques, connectiques capteurs et actionneurs du gros robot */

/**************** P�riph�riques ****************/

/* R�glages PWM */
	#define USE_PWM1
	#define USE_PWM2
	#define USE_PWM3
	#define USE_PWM4
	#define USE_CUSTOM_PWM_5		// Gestion actionneur pompe gauche
	#define FREQ_PWM_50KHZ

/* Servo-Moteurs standart */
	#define SERVO_TIMER 5
	#define USE_SERVO

/* Asservissement en position/vitesse de moteurs CC */
	/* d�clarer l'utilisation du pilote */
	#define USE_DCMOTOR2
	/* d�finir le nombre d'actionneurs asservis */
	#define DCM_NUMBER			0
	/* P�riode d'asservisement (en ms) */
	#define DCM_TIME_PERIOD		10
	/* nombre maximum de positions � g�rer par moteur */
	#define DCMOTOR_NB_POS		1

/* R�glages QEI */
	#define USE_QUEI1
	#define USE_QUEI2

/* R�glages ADC */
	#define USE_AN13	// Clap

#endif /* BIG_CONFIG_USE_H */
