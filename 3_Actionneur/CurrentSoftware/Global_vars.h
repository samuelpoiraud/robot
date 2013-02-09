/*
 *	Club Robot ESEO 2012 - 2013
 *	Krusty & Tiny
 *
 *  Fichier : Global_config.h
 *  Package : Actionneur
 *  Description : Variables globales définies specifiquement pour
					le code de la carte l'executant.
 *  Auteur : Gwenn, millman
 *  Version 20100413
 */

#ifndef GLOBAL_VARS_H
	#define GLOBAL_VARS_H

	#ifndef QS_GLOBAL_VARS_H
		#error "Il est interdit d'inclure directement Global_vars.h, lire le CQS."
	#endif
	
	bool_e pawn_front_detected;
	bool_e pawn_back_detected;
	bool_e switch_clamp_front;
	bool_e switch_clamp_back;



	//Test et inclusion des vars spécifiques au robot
	#if (defined(I_AM_ROBOT_KRUSTY) && defined(I_AM_ROBOT_TINY)) || (!defined(I_AM_ROBOT_KRUSTY) && !defined(I_AM_ROBOT_TINY))
	#error "Veuillez définir I_AM_ROBOT_KRUSTY ou I_AM_ROBOT_TINY selon le robot cible."
	#endif
	#if defined(I_AM_ROBOT_KRUSTY)
	#include "./Krusty/KGlobal_vars.h"
	#endif
	#if defined(I_AM_ROBOT_TINY)
	#include "./Tiny/TGlobal_vars.h"
	#endif
#endif /* ndef GLOBAL_VARS_H */
