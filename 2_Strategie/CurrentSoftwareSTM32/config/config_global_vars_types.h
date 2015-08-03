/*
 *  Club Robot ESEO 2008 - 2014
 *
 *  $Id$
 *
 *  Package : Carte Strategie
 *  Description : Définition de types pour les variables globales
				définies specifiquement pour ce code.
 *  Auteur : Jacen
 */

#ifndef CONFIG_GLOBAL_VARS_TYPES_H
	#define CONFIG_GLOBAL_VARS_TYPES_H

	#include "../QS/QS_types.h"
	#include "../QS/QS_maths.h"
	#include "../QS/QS_CANmsgList.h"
	#include "../detection.h"

	typedef struct
	{
		volatile bool_e freine;
		volatile bool_e ended;
		volatile bool_e calibrated;
		volatile bool_e erreur;
		volatile bool_e reach_x;
		volatile bool_e reach_y;
		volatile bool_e reach_teta;
		volatile way_e						current_way;
		volatile SUPERVISOR_error_source_e	current_status;
		volatile bool_e is_in_translation;  //TRUE si le robot est en translation
		volatile bool_e is_in_rotation;		//TRUE si le robot est en rotation
		volatile time32_t	last_time_pos_updated;
	}prop_env_t;

	typedef struct
	{
		Uint8 strategie;
		bool_e evitement;
		bool_e balise;
	}config_t;

	typedef struct
	{
		volatile Sint16 angle;
		volatile Sint16 dist;
		volatile Sint16 x;
		volatile Sint16 y;
		volatile Uint8 nb_points_reached;	//Nombre de point atteint d'un ensemble de point multipoint, pour savoir à quel moment on s'est arreté en cas d'évitement
		volatile Sint16 translation_speed;	// [mm/sec] (modulo 250 mm/s)
		volatile Sint16 rotation_speed;
		volatile Sint16 cosAngle;/*cosinus de l'angle du robot*/
		volatile Sint16 sinAngle;/*sinus de l'angle du robot*/
		volatile time32_t update_time;		//Ce temps sera mis à jour à chaque adversaire nouvellement observé.
		volatile bool_e updated;			//Attention, ce flag sera levé pendant UNE SEULE boucle de tâche de fond, suite à la mise à jour.
	}position_t;

	typedef enum
	{
		DETECTION_FROM_USER = 0,
		DETECTION_FROM_PROPULSION,
		DETECTION_FROM_BEACON_IR
	}detection_from_e;

	typedef struct
	{
		volatile Sint16 angle;
		volatile Sint16 dist;
		volatile Sint16 x;
		volatile Sint16 y;
		volatile bool_e updated;				//Ce flag est levé pendant 1 tour de boucle lorsque la donnée correspondance est mise à jour.
		volatile bool_e enable;					//Ce flag est levé entre l'instant de mise à jour de l'adversaire JUSQU'A son obsolescence (gerée par Detection).
		volatile time32_t update_time;
		volatile Uint8 fiability_error;
		volatile detection_from_e from;	//Source de l'information
	}foe_t;

	//enum utilisé par le tableau d'états du terrain
	typedef enum{
		ELEMENT_TODO = 0,
		ELEMENT_DONE,
		ELEMENT_NONE //Non applicable
	}map_state_e;

	// Structure contenant les informations échangé entre les deux robots (ex : est-ce que le petit robot à fait son get_out ?)
	typedef struct{
		bool_e reach_point_get_out_init;
	}communication_t;

	typedef struct{
		volatile bool_e force_foe;
		volatile Sint32 propulsion_coefs[PROPULSION_NUMBER_COEFS];
		volatile Uint32 propulsion_coefs_updated;	//par soucis de compacité mémoire et temporel, les flags d'updated sont concaténés ici dans un seul entier...
		volatile Uint16 duration_trajectory_for_test_coefs;
	}debug_t;

#endif /* ndef CONFIG_GLOBAL_VARS_TYPES_H */
