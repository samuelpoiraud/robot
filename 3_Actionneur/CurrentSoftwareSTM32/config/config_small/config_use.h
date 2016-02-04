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
	#define USE_PWM1     //PWM1 Pompe avant gauche de Pearl
	#define USE_PWM2     //PWM2 Pompe avant droite de Pearl
	//#define USE_PWM3
	//#define USE_PWM4
	#define FREQ_PWM_50KHZ

/* Utilisation de la carte Mosfets*/
	/* déclarer l'utilisation du pilote */
	#define USE_MOSFETS
	/*Nombre de mosfets à piloter (max = 8)*/
	#define NB_MOSFETS          2

#endif /* SMALL_CONFIG_USE_H */
