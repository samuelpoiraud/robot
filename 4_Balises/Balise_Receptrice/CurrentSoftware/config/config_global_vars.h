/*
 *  Club Robot ESEO 2015
 *
 *  Package : Carte Balise r�ceptrice
 *  Description : Variables globales d�finies specifiquement pour
					le code de la carte l'executant.
 *  Auteur : Arnaud
 */


#ifndef CONFIG_GLOBAL_VARS_H
	#define CONFIG_GLOBAL_VARS_H

	#include "../QS/QS_types.h"
	#include "config_global_vars_types.h"

	typedef struct{
		bool_e flag_100ms;
		bool_e start_of_match;
		bool_e end_of_match;
	}flag_list_t;

	typedef struct{
		/* les drapeaux */
		volatile flag_list_t flags;
		volatile color_e color;
		time32_t absolute_time;
	}global_data_storage_t;

	extern global_data_storage_t global;

#endif /* ndef CONFIG_GLOBAL_VARS_H */
