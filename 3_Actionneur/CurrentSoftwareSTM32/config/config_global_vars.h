/*
 *  Club Robot ESEO 2008 - 2014
 *
 *  $Id$
 *
 *  Package : Carte Actionneur
 *  Description : Variables globales définies specifiquement pour
					le code de la carte l'executant.
 *  Auteur : Jacen
 */

#ifndef CONFIG_GLOBAL_VARS_H
	#define CONFIG_GLOBAL_VARS_H

	#include "../QS/QS_types.h"
	#include "config_global_vars_types.h"


	typedef struct{
		volatile bool_e match_over;
		volatile bool_e match_started;

		volatile bool_e alim;

	}flag_list_t;

	typedef struct{
		volatile flag_list_t flags;

		volatile Uint16 alim_value;				// en mV

		volatile time32_t absolute_time;

		volatile position_t pos;				//Position du robot
	}global_data_storage_t;

	extern global_data_storage_t global;

#endif /* ndef CONFIG_GLOBAL_VARS_H */
