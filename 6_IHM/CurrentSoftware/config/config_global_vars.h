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
	volatile bool_e flag_for_compile;
}flag_list_t;

typedef struct{
	volatile flag_list_t flags;				// Les flags

	struct{
		volatile Sint16 x;
		volatile Sint16 y;
		volatile Sint16 teta;
	}pos;

	struct{
		volatile Sint16 Vpermanent;
		volatile Sint16 Vpuissance;
		volatile Sint16 Vhokuyo;
	}voltage;

	volatile foe_t foe[4];

	volatile time32_t absolute_time;		// Temps depuis le lancement de la STM32
}global_data_storage_t;

extern global_data_storage_t global;

#endif /* ndef CONFIG_GLOBAL_VARS_H */
