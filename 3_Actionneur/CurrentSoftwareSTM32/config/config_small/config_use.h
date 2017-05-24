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

/* Réglages capteur couleur I2C */
	#define USE_I2C_COLOR_SENSOR
		#ifdef USE_I2C_COLOR_SENSOR
			#define USE_APDS9960
				#define APDS9960_I2C		I2C3
		#endif

/* Réglages capteur de distance */
	#define USE_VL53L0X
		#define VL53L0X_I2C I2C3

/* Réglages I2C */
	#if defined(USE_I2C_COLOR_SENSOR) || defined(USE_VL53L0X)
		#define USE_I2C3
			#define I2C3_CLOCK_SPEED	100000
	#endif

#endif /* SMALL_CONFIG_USE_H */
