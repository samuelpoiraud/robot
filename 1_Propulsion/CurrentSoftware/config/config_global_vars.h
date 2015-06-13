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
		volatile bool_e disable_virtual_perfect_robot;

		volatile bool_e match_started;
		volatile bool_e match_over;

		volatile bool_e alim;
	}flag_list_t;

	typedef struct{
		/* les drapeaux */
		volatile flag_list_t flags;

		///REFERENTIEL GENERAL/////////(x, y, teta...)///////////////////

		//Position actuelle du robot (x, y, teta)
		//Dans le référentiel Général
		volatile position_t position;
		// x 		[mm]
		// y 		[mm]
		// teta 	[rad/4096]		<<12


		///REFERENTIEL IT///////////////////////////////////////////

		//Vitesse réelle du robot mesurée pour 5ms
		volatile Sint32 real_speed_translation;		//[mm/4096/5ms]		<<12 /5ms
		volatile Sint32 real_speed_rotation;		//[rad/4096/1024/5ms]	<<22

		//Distance et Angle réellement parcourue par le robot mesurée pour 5ms
		//Dans le référentiel IT
		volatile Sint32 real_position_translation;	//[mm/4096]  <<12
		volatile Sint32 real_position_rotation;		//[rad/4096/1024] <<22

		//Ces écarts correspondent à la différence entre les coordonnées du point fictif et les coordonnées réelles du robot.
		volatile Sint32 ecart_translation;			//[mm/4096]  <<12
		volatile Sint32 ecart_rotation;				//[rad/4096] <<12
		volatile Sint32 ecart_translation_prec;		//[mm/4096]  <<12
		volatile Sint32 ecart_rotation_prec;		//[rad/4096] <<12

		//////////////////////////////////////////////////////////

		// Caractéristiques de la trajectoire en cours... Acc, Vit, Pos

		//Accélérations, vitesses, positions DU POINT FICTIF calculées par la gestion des trajectoires

		volatile Sint32 acceleration_translation;	//[mm/4096/5ms/5ms]
		volatile Sint32 vitesse_translation;		//[mm/4096/5ms]
		volatile Sint32 position_translation; 		//[mm/4096]


		volatile Sint32 acceleration_rotation;		//[rad/4096/1024/5ms/5ms]
		volatile Sint32 vitesse_rotation;			//[rad/4096/1024/5ms]
		volatile Sint32 position_rotation;			//[rad/4096]


		//Dernières vitesse avant erreur
		volatile Sint32 vitesse_translation_erreur;	//[mm/5ms]   /5ms
		volatile Sint32 vitesse_rotation_erreur;	//[rad/1024]   <<10

		//////////////////////////////////////////////////////////

		// Etat alimentation
		volatile Uint16 alim_value;					// en mV

		volatile time32_t absolute_time;

		volatile debug_t debug;
	}global_data_storage_t;

	extern global_data_storage_t global;

#endif /* ndef CONFIG_GLOBAL_VARS_H */
