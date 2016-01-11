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
		volatile bool_e initial_position_received;

		volatile bool_e ask_prop_calibration;
		volatile bool_e ask_start;
		volatile bool_e ask_suspend_match;
		volatile bool_e ask_resume_match;


		volatile bool_e color_updated;

		volatile bool_e match_started;
		volatile bool_e match_over;
		volatile bool_e match_suspended;

		volatile bool_e alim;

		volatile bool_e foes_updated_for_lcd;

		volatile bool_e go_to_home;

		volatile bool_e ventouses_avant_pearl;
		volatile bool_e ventouses_arriere_pearl;

		volatile bool_e home_rock_one;
		volatile bool_e home_rock_two;
		volatile bool_e home_rock_three;

		volatile bool_e home_one;
		volatile bool_e home_two;
		volatile bool_e home_three;
		volatile bool_e home_four;
		volatile bool_e home_five;

		volatile bool_e neutral_one;
		volatile bool_e neutral_two;
		volatile bool_e neutral_three;
		volatile bool_e neutral_four;
		volatile bool_e neutral_five;
		volatile bool_e neutral_six;

		volatile bool_e away_one;
		volatile bool_e away_two;
		volatile bool_e away_three;
		volatile bool_e away_four;
		volatile bool_e away_five;

		volatile bool_e away_rock_one;
		volatile bool_e away_rock_two;
		volatile bool_e away_rock_three;
	}flag_list_t;

	typedef struct{
		volatile flag_list_t flags;				// Les flags

		volatile color_e color;
		volatile color_e wanted_color;

		volatile Uint16 alim_value;				// en mV

		volatile foe_t foe[MAX_NB_FOES];		//l'ensemble des adversaires vus sur le terrain
		//Attention, un même adversaire peut être mis à jour sur plusieurs cases différents du tableau de foe !

		volatile GEOMETRY_point_t destination;
		volatile communication_t com;

		volatile time32_t match_time;			//temps de match en ms.
		volatile time32_t absolute_time;		//temps depuis le lancement de la STM32

		volatile debug_t debug;
		volatile prop_env_t prop;
		volatile position_t pos;
	}global_data_storage_t;

	extern global_data_storage_t global;

#endif /* ndef CONFIG_GLOBAL_VARS_H */
