/*
 *  Club Robot ESEO 2015
 *
 *  Package : Balise émettrice
 *  Description : Variables globales définies specifiquement pour
					le code de la carte l'executant.
 *  Auteur : Arnaud
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

		time32_t absolute_time;
	}global_data_storage_t;

	extern global_data_storage_t global;

#endif /* ndef CONFIG_GLOBAL_VARS_H */
