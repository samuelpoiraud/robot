/*
 *  Club Robot ESEO 2008 - 2010
 *  Archi'Tech, PACMAN
 *
 *  Fichier : Global_vars_types.h
 *  Package : Carte P
 *  Description : Définition de types pour les variables globales
				définies specifiquement pour ce code.
 *  Auteur : Jacen
 *	Version 20100420
 */

#ifndef QS_GLOBAL_VARS_H
	#error "Global_vars_types est inclu par la QS, ne l'incluez pas vous meme"
#endif

	// time_t à l'origine, mais modifié pour être compatible avec le simulateur EVE
	typedef Uint32 time32_t;
	
	//Position de l'ascenseur et de la pince
	typedef enum
	{
		PREPARE = 0, OPEN, CLOSE, NB_POSITION
	}ACT_position_e;
	
	typedef enum
	{
		FOE_1 = 0, FOE_2, NB_FOES
	}foe_e;
	
//	//Acquittement sur action de la pince ou de l'ascenseur
//	typedef enum
//	{
//		ACK_ON_LIFT, ACK_ON_CLAMP
//	}ACT_acknowledgment_e;
//
#include "Stacks.h"
#include "environment.h"

