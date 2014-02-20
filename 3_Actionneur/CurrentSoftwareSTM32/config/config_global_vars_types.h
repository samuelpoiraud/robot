/*
 *  Club Robot ESEO 2008 - 2014
 *
 *  $Id$
 *
 *  Package : Carte Actionneur
 *  Description : D�finition de types pour les variables globales
				d�finies specifiquement pour ce code.
 *  Auteur : Jacen
 */

#ifndef QS_GLOBAL_VARS_H
	#error "config_global_vars_types est inclu par la QS, ne l'incluez pas vous meme"
#endif

#include "config_global.h"

//D�claration commune au 2 robot ici

/*Pour distinguer les actionneurs avant et arri�re du robot */
typedef enum
{
	FRONT=0,
	BACK,
	NB_WAY
}act_way_e;

//Test et inclusion des configs sp�cifiques au robot
#if (defined(I_AM_ROBOT_BIG) && defined(I_AM_ROBOT_SMALL)) || (!defined(I_AM_ROBOT_BIG) && !defined(I_AM_ROBOT_SMALL))
#	error "Veuillez d�finir I_AM_ROBOT_BIG ou I_AM_ROBOT_SMALL selon le robot cible dans config_global.h"
#elif defined(I_AM_ROBOT_BIG)
#	include "config_big/config_global_vars_types.h"
#elif defined(I_AM_ROBOT_SMALL)
#	include "config_small/config_global_vars_types.h"
#endif
