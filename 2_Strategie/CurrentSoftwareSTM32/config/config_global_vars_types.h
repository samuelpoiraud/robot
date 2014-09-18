/*
 *  Club Robot ESEO 2008 - 2014
 *
 *  $Id$
 *
 *  Package : Carte Strategie
 *  Description : D�finition de types pour les variables globales
				d�finies specifiquement pour ce code.
 *  Auteur : Jacen
 */

#ifndef QS_GLOBAL_VARS_H
	#error "config_global_vars_types est inclu par la QS, ne l'incluez pas vous meme"
#endif

	//Position de l'ascenseur et de la pince
	typedef enum
	{
		PREPARE = 0, OPEN, CLOSE, NB_POSITION
	}ACT_position_e;



//	//Acquittement sur action de la pince ou de l'ascenseur
//	typedef enum
//	{
//		ACK_ON_LIFT, ACK_ON_CLAMP
//	}ACT_acknowledgment_e;
//
#include "../Stacks.h"
#include "../environment.h"

