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
	//#define USE_PWM1
	#define USE_PWM2
	#define USE_PWM3
	//#define USE_PWM4
	#define FREQ_PWM_50KHZ

/* Servo-Moteurs standart */
	#define SERVO_TIMER 5
	#define USE_SERVO

#endif /* BIG_CONFIG_USE_H */
