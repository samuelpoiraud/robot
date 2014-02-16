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
#if (defined(I_AM_ROBOT_KRUSTY) && defined(I_AM_ROBOT_TINY)) || (!defined(I_AM_ROBOT_KRUSTY) && !defined(I_AM_ROBOT_TINY))
#	error "Veuillez d�finir I_AM_ROBOT_KRUSTY ou I_AM_ROBOT_TINY selon le robot cible dans config_global.h"
#elif defined(I_AM_ROBOT_KRUSTY)
#	include "config_krusty/config_global_vars_types.h"
#elif defined(I_AM_ROBOT_TINY)
#	include "config_tiny/config_global_vars_types.h"
#endif
