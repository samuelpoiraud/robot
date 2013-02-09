/*
 *	Club Robot ESEO 2012 - 2013
 *	Krusty & Tiny
 *
 *  Fichier : Global_vars_types.h
 *  Package : Actionneur
 *  Description : Définition de types pour les variables globales
				définies specifiquement pour ce code.
 *  Auteur : Gwenn,millman
 *  Version 20100327
 */

	#ifndef QS_GLOBAL_VARS_H
		#error "Global_vars_types est inclu par la QS, ne l'incluez pas vous meme"
	#endif

	//Déclaration commune au 2 robot ici

	/*Pour distinguer les actionneurs avant et arrière du robot */
	typedef enum
	{
		FRONT=0,
		BACK,
		NB_WAY
	}act_way_e;


	//Test et inclusion des configs spécifiques au robot
	#if (defined(I_AM_ROBOT_KRUSTY) && defined(I_AM_ROBOT_TINY)) || (!defined(I_AM_ROBOT_KRUSTY) && !defined(I_AM_ROBOT_TINY))
	#error "Veuillez définir I_AM_ROBOT_KRUSTY ou I_AM_ROBOT_TINY selon le robot cible."
	#endif
	#if defined(I_AM_ROBOT_KRUSTY)
	#include "./Krusty/KGlobal_vars_types.h"
	#endif
	#if defined(I_AM_ROBOT_TINY)
	#include "./Tiny/TGlobal_vars_types.h"
	#endif
