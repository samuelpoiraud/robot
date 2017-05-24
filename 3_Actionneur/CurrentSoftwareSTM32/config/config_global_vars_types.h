/*
 *  Club Robot ESEO 2008 - 2014
 *
 *  $Id$
 *
 *  Package : Carte Actionneur
 *  Description : Définition de types pour les variables globales
				définies specifiquement pour ce code.
 *  Auteur : Jacen
 */


#ifndef CONFIG_GLOBAL_VARS_TYPES_H
	#define CONFIG_GLOBAL_VARS_TYPES_H


	#include "config_global.h"

	//Déclaration commune au 2 robot ici

	//Test et inclusion des configs spécifiques au robot
	#if (defined(I_AM_ROBOT_BIG) && defined(I_AM_ROBOT_SMALL)) || (!defined(I_AM_ROBOT_BIG) && !defined(I_AM_ROBOT_SMALL))
	#	error "Veuillez définir I_AM_ROBOT_BIG ou I_AM_ROBOT_SMALL selon le robot cible dans config_global.h"
	#elif defined(I_AM_ROBOT_BIG)
	#	include "config_big/config_global_vars_types.h"
	#elif defined(I_AM_ROBOT_SMALL)
	#	include "config_small/config_global_vars_types.h"
	#endif

	typedef struct{
		volatile Sint16 angle;
		volatile Sint16 x;
		volatile Sint16 y;
		volatile bool_e updated;
	}position_t;

#endif /* ndef CONFIG_GLOBAL_VARS_TYPES_H */
