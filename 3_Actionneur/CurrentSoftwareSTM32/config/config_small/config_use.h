/*
 *  Club Robot ESEO 2013 - 2014
 *
 *  $Id$
 *
 *  Package : Carte Actionneur
 *  Description : Configuration des modules de QS
 *  Auteur : Jacen, Alexis
 */

#ifndef SMALL_CONFIG_USE_H
#define SMALL_CONFIG_USE_H

/* Definition des p�riph�riques, connectiques capteurs et actionneurs du petit robot */

/**************** P�riph�riques ****************/

/* R�glages PWM */
	#define USE_PWM_MODULE
		#define PWM_FREQ	50000
		#define USE_PWM1
		//#define USE_PWM2

/* R�glages de la carte Mosfets*/
	#define USE_MOSFETS_MODULE
		 #define USE_MOSFET_1

/* R�glages DC Speed */
	#define USE_DC_MOTOR_SPEED
		#define DC_MOTOR_SPEED_NUMBER 		1
		#define DC_MOTOR_SPEED_TIME_PERIOD	10

/* R�glages IT externe */
	#define USE_EXTERNAL_IT



#endif /* SMALL_CONFIG_USE_H */
