/*
 *  Club Robot ESEO 2008 - 2014
 *
 *  $Id$
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
		bool_e flag_for_compile;
	}flag_list_t;

	typedef struct{
		/* les drapeaux */
		volatile flag_list_t flags;

		volatile Uint8 chrono_long_push; //compteur des quarts de seconde pout détecté un appui égal à 2 secondes
		volatile environment_t env;
	}global_data_storage_t;

	extern global_data_storage_t global;

#endif /* ndef CONFIG_GLOBAL_VARS_H */
