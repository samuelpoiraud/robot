/*
 *	Club Robot ESEO 2012 - 2013
 *	Krusty & Tiny
 *
 *  Fichier : Global_flags.h
 *  Package : Standard_Project
 *  Description : Flags définis specifiquement pour
					le code de la carte l'executant.
 *  Auteur : Jacen et un peu millman
 *  Version 20100326
 */

#ifndef GLOBAL_FLAGS_H
	#define GLOBAL_FLAGS_H

	#ifndef QS_GLOBAL_VARS_H
		#error "Il est interdit d'inclure directement Global_flags.h, lire le CQS."
	#endif

	/*  Note : Flags communs à tous les codes
	 *	volatile bool_e u1rx;	// message reçu sur uart1
	 *	volatile bool_e u2rx;	// message reçu sur uart2
	 *	volatile bool_e canrx;	// message reçu sur le bus can
	 *	volatile bool_e can2rx;	// message reçu sur le bus can2
	 */




	 //Test et inclusion des flags spécifiques au robot
	#if (defined(I_AM_ROBOT_KRUSTY) && defined(I_AM_ROBOT_TINY)) || (!defined(I_AM_ROBOT_KRUSTY) && !defined(I_AM_ROBOT_TINY))
	#error "Veuillez définir I_AM_ROBOT_KRUSTY ou I_AM_ROBOT_TINY selon le robot cible."
	#endif
	#if defined(I_AM_ROBOT_KRUSTY)
	#include "./Krusty/KGlobal_flags.h"
	#endif
	#if defined(I_AM_ROBOT_TINY)
	#include "./Tiny/TGlobal_flags.h"
	#endif
	
#endif /* ndef GLOBAL_VARS_H */
