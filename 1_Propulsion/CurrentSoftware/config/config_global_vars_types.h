/*
 *  Club Robot ESEO 2008 - 2014
 *
 *  $Id: config_global_vars_types.h 2706 2014-10-04 13:06:44Z aguilmet $
 *
 *  Package : Carte Strategie
 *  Description : Définition de types pour les variables globales
				définies specifiquement pour ce code.
 *  Auteur : Jacen
 */

#ifndef CONFIG_GLOBAL_VARS_TYPES_H
	#define CONFIG_GLOBAL_VARS_TYPES_H

	#include "../QS/QS_types.h"

	typedef struct {
		Sint16 x;		//[mm]
		Sint16 y;		//[mm]
		Sint16 teta;	//[rad.4096]
	}position_t;

	typedef enum {
		NO_ACKNOWLEDGE = 0,	 //Aucun acquittement demandé
		ACKNOWLEDGE_ASKED,   //Acquittement demandé
		BRAKE_ACKNOWLEDGED,  //Acquittement du freinage fourni
		ACKNOWLEDGE_CALIBRATION,
		ACKNOWLEDGE_SELFTEST,
		ACKNOWLEDGE_TRAJECTORY_FOR_TEST_COEFS,
		ACKNOWLEDGED,		 //Acquittement complet fourni
		INTERN_ACKNOWLEDGE	 //Acquittement interne pour fonction de la propulsion
	}acknowledge_e;

	typedef enum {
		NOT_BORDER_MODE = 0,
		BORDER_MODE,
		BORDER_MODE_WITH_UPDATE_POSITION
	}border_mode_e;

	typedef struct{
		volatile bool_e foe_forced;
		volatile bool_e mode_best_effort_enable;

		volatile bool_e recouvrement_IT;
		volatile time32_t recouvrement_IT_time;
		volatile char recouvrement_section;
	}debug_t;

#endif /* ndef CONFIG_GLOBAL_VARS_TYPES_H */
