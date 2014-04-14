/*
 *	Club Robot ESEO 2008 - 2010
 *	Archi'Tech, PACMAN
 *
 *	Fichier : environment.h
 *	Package : Carte Principale
 *	Description : 	Fonctions de supervision de l'environnement
 *	Auteur : Jacen
 *	Version 20100119
 */

#include "QS/QS_all.h"

#ifndef ENVIRONMENT_H
	#define ENVIRONMENT_H

	#include "detection.h"
	#include "sick.h"
	#include "elements.h"
	#include "Geometry.h"

	/* #define communs à plusieurs modules */
	#define MATCH_DURATION 				90000UL //90 secondes)
	#define GAME_ZONE_SIZE_X			2000 //2000 mm
	#define GAME_ZONE_SIZE_Y			3000 //3000 mm

	// Dimensions des éléments de jeu
	#define CD_RADIUS				60
	#define CD_DIAMETER				120
	#define CD_SQUARED_RADIUS		3600
	#define CD_SQUARED_DIAMETER		14400
	#define GOLD_BAR_LENGTH_BOTTOM	150
	#define GOLD_BAR_WIDTH_BOTTOM	70
	#define GOLD_BAR_LENGTH_TOP		124
	#define GOLD_BAR_WIDTH_TOP		44
	#define GOLD_BAR_HEIGHT			48,5
	#define DISTANCE_CLAMPCENTER_ROBOT_CENTER 207


	typedef struct
	{
		volatile bool_e freine;
		volatile bool_e fini;
		volatile bool_e calibrated;
		volatile bool_e erreur;
		volatile bool_e reach_x;
		volatile bool_e reach_y;
		volatile bool_e reach_teta;
		volatile Sint32 vitesse_translation_erreur;
		volatile Sint32 vitesse_rotation_erreur;
		volatile way_e						current_way;
		volatile SUPERVISOR_error_source_e	current_status;
		volatile bool_e is_in_translation;  //TRUE si le robot est en translation
		volatile bool_e is_in_rotation;		//TRUE si le robot est en rotation
		volatile time32_t	last_time_pos_updated;
	}asser_env_t;

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

	typedef enum
	{
		DETECTION_IS_RECENT = 0,
		DETECTION_IS_GETTING_OBSOLETE,		//Etape qui ne dure qu'un tour de boucle lorsque l'adversaire devient "obsolete"
		DETECTION_IS_OBSOLETE
	}detection_obsolete_e;


	typedef struct
	{
		volatile Sint16 angle;
		volatile Sint16 dist;
		volatile Sint16 x;
		volatile Sint16 y;
		volatile bool_e updated;
		volatile detection_obsolete_e obsolete;		//Rendu obsolete
		volatile time32_t update_time;
		volatile detection_from_e from;	//Source de l'information
	}foe_t;
	///////////////////////////////

	//enum utilisé par le tableau d'états du terrain
	typedef enum{
		ELEMENT_TODO = 0,
		ELEMENT_DONE,
		ELEMENT_NONE //Non applicable
	}map_state_e;

	#define MAX_NB_FOES	16	//Nombre max d'aversaires

	typedef struct
	{
		element_t elements_list[NB_ELEMENTS];
		Uint8 nb_elements;
		color_e	color;
		color_e	wanted_color;
		bool_e color_updated;
		bool_e ask_asser_calibration;
		bool_e ask_start;
		bool_e alim;
		Uint16 alim_value;		// en mV
		asser_env_t asser;
		position_t pos;					//comme son nom l'indique, c'est la position de notre robot
		foe_t foe[MAX_NB_FOES];		//l'ensemble des adversaires vus sur le terrain - ces données concernent plus l'évitement que le zoning !
			//Attention, un même adversaire peut être mis à jour sur plusieurs cases différents du tableau de foe !
		bool_e match_started, match_over;
		time32_t match_time; //temps de match en ms.
		time32_t absolute_time; //temps depuis le lancement du module clock en ms.
		bool_e debug_force_foe;
		Uint16 duration_trajectory_for_test_coefs;
		bool_e reach_point_C1;
		bool_e reach_point_get_out_init;
		GEOMETRY_point_t destination;
	}environment_t;



	//NOTE MICHAEL: Mettre à jour les fonctions suivantes pour le tableau des elements

	/* Appelée dans l'INIT : initialise toutes les variables d'environnement */
	void ENV_init(void);

	/* Appelée en début de tache de fond : met à jour l'environnement */
	void ENV_update(void);

	/* Change la couleur et prévient tout le monde */
	void ENV_set_color(color_e color);

	/* Traitement d'un message CAN et renvoie vers d'autres bus */
	void ENV_process_can_msg(CAN_msg_t * incoming_msg, bool_e bCAN, bool_e bU1, bool_e bU2, bool_e bXBee);



#endif /* ndef ENVIRONMENT_H */
