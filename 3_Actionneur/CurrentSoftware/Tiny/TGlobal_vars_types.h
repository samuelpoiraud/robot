/*  Club Robot ESEO 2012 - 2013
 *	Tiny
 *
 *  Fichier : Global_vars_types.h
 *  Package : Actionneur
 *  Description : Définition de types pour les variables globales
				définies specifiquement pour ce code.
 *  Auteur : Gwenn,millman
 *  Version 20100327
 *  Robot : Tiny
 */

#if !defined(TINY_GLOBAL_VARS_TYPES_H) && defined(I_AM_ROBOT_TINY)
	#define TINY_GLOBAL_VARS_TYPES_H

	#ifndef QS_GLOBAL_VARS_H
		#error "Global_vars_types est inclu par la QS, ne l'incluez pas vous meme"
	#endif

	typedef enum {
		QUEUE_ACT_LongHammer,
		QUEUE_ACT_BallInflater,
		NB_ACT

	} QUEUE_act_e;

#endif /* ndef TINY_GLOBAL_VARS_TYPES_H && def I_AM_ROBOT_TINY */
