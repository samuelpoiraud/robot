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
		#define SMALL_POMPE_DISPOSE_PWM_NUM		(2)
		#define USE_PWM2
		#define SMALL_POMPE_PRISE_PWM_NUM		(3)
		#define USE_PWM3


/* Réglages de la carte Mosfets*/
	//#define USE_MOSFETS_MODULE

#endif /* SMALL_CONFIG_USE_H */
