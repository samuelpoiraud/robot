/*
 *  Club Robot ESEO 2008 - 2014
 *
 *  $Id: config_global_vars_types.h 2706 2014-10-04 13:06:44Z aguilmet $
 *
 *  Package : Carte Strategie
 *  Description : D�finition de types pour les variables globales
				d�finies specifiquement pour ce code.
 *  Auteur : Jacen
 */

#ifndef CONFIG_GLOBAL_VARS_TYPES_H
	#define CONFIG_GLOBAL_VARS_TYPES_H

	#include "../QS/QS_types.h"

	typedef struct{
		volatile Sint16 angle;
		volatile Sint16 dist;
		volatile Sint16 x;
		volatile Sint16 y;
		volatile bool_e updated;				//Ce flag est lev� pendant 1 tour de boucle lorsque la donn�e correspondance est mise � jour.
		volatile bool_e enable;					//Ce flag est lev� entre l'instant de mise � jour de l'adversaire JUSQU'A son obsolescence (ger�e par Detection).
		volatile time32_t update_time;
		volatile Uint8 fiability_error;
	}foe_t;

#endif /* ndef CONFIG_GLOBAL_VARS_TYPES_H */
