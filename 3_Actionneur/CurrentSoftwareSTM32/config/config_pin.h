/*
 *  Club Robot ESEO 2013 - 2014
 *
 *  $Id$
 *
 *  Package : Carte Actionneur
 *  Description : Configuration des pins
 *  Auteur : Jacen, Alexis
 */

#ifndef CONFIG_PIN_H
	#define CONFIG_PIN_H

	#include "config_global.h"
	#include "../QS/QS_ports.h"
	#include "../stm32f4xx/stm32f4xx_gpio.h"

	// Définir les configurations pin propre à chaque robot dans config_big/config_pin.h ou config_small/config_pin.h !

	//Test et inclusion des configs spécifiques au robot
	#if (defined(I_AM_ROBOT_BIG) && defined(I_AM_ROBOT_SMALL)) || (!defined(I_AM_ROBOT_BIG) && !defined(I_AM_ROBOT_SMALL))
	#	error "Veuillez définir I_AM_ROBOT_BIG ou I_AM_ROBOT_SMALL selon le robot cible dans config_global.h"
	#elif defined(I_AM_ROBOT_BIG)
	#	include "config_big/config_pin.h"
	#elif defined(I_AM_ROBOT_SMALL)
	#	include "config_small/config_pin.h"
	#endif

#endif /* CONFIG_PIN_H */
