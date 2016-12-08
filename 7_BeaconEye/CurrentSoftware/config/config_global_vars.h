/*
 *  Club Robot ESEO 2008 - 2014
 *
 *  $Id: config_global_vars.h 897 2013-10-10 19:13:19Z amurzeau $
 *
 *  Package : Carte Strategie
 *  Description : Variables globales définies specifiquement pour
					le code de la carte l'executant.
 *  Auteur : Jacen
 */

#ifndef CONFIG_GLOBAL_VARS_H
	#define CONFIG_GLOBAL_VARS_H

	#include "../QS/QS_types.h"
	#include "config_global_vars_types.h"

	typedef struct{
		volatile bool_e match_started;
		volatile bool_e match_over;
		volatile bool_e match_suspended;
		volatile bool_e LASER;
	}flag_list_t;

	typedef struct{
		volatile flag_list_t flags;

		volatile time32_t match_time;			//temps de match en ms.
		volatile time32_t absolute_time;		//temps depuis le lancement de la STM32
	}global_data_storage_t;

	extern global_data_storage_t global;

#endif /* ndef CONFIG_GLOBAL_VARS_H */
