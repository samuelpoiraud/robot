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

/* Definition des périphériques, connectiques capteurs et actionneurs du petit robot */

/**************** Périphériques ****************/

/* Réglages PWM */
	#define USE_PWM_MODULE
		#define PWM_FREQ	50000
		#define USE_PWM1     //PWM1 Pompe avant gauche de Pearl
		#define USE_PWM2     //PWM2 Pompe avant droite de Pearl

/* Réglages de la carte Mosfets*/
	#define USE_MOSFETS_MODULE
		 #define USE_MOSFET_1

#endif /* SMALL_CONFIG_USE_H */
