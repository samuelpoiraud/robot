/*  Club Robot ESEO 2012 - 2013
 *	Krusty
 *
 *  Fichier : Global_vars_types.h
 *  Package : Actionneur
 *  Description : Définition de types pour les variables globales
				définies specifiquement pour ce code.
 *  Auteur : Gwenn,millman
 *  Version 20100327
 *  Robot : Krusty
 */

#if !defined(KRUSTY_GLOBAL_VARS_TYPES_H) && defined(I_AM_ROBOT_KRUSTY)
	#define KRUSTY_GLOBAL_VARS_TYPES_H

	#ifndef QS_GLOBAL_VARS_H
		#error "Global_vars_types est inclu par la QS, ne l'incluez pas vous meme"
	#endif

	typedef enum {
		BALL_GRABBER=0,
		HAMMER,
		QUEUE_ACT_BallLauncher,	//avec QUEUE devant, n'importe ou dans le code on sait que ça fait référence à un actionneur lié à queue.h/c
		QUEUE_ACT_Plate_Rotation,
		QUEUE_ACT_Plate_AX12_Plier,
		NB_ACT

	} QUEUE_act_e;


#endif /* ndef KRUSTY_GLOBAL_VARS_TYPES_H && def I_AM_ROBOT_KRUSTY */
