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
	#include "grille.h"
	
	/* #define communs � plusieurs modules */
	#define MATCH_DURATION 				90000UL //90 secondes)
	#define GAME_ZONE_SIZE_X			2000 //2000 mm
	#define GAME_ZONE_SIZE_Y			3000 //3000 mm
	
	// Dimensions des �l�ments de jeu
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
		volatile Uint8 nb_points_reached;	//Nombre de point atteint d'un ensemble de point multipoint, pour savoir � quel moment on s'est arret� en cas d'�vitement
		volatile Sint16 translation_speed;
		volatile Sint16 rotation_speed;
		volatile double cosAngle;/*cosinus de l'angle du robot*/
		volatile double sinAngle;/*sinus de l'angle du robot*/
		volatile time32_t update_time;
		volatile bool_e updated;
	}position_t;
	
	///////////////////////////////

	//enum utilis� par le tableau d'�tats du terrain
	typedef enum{
		ELEMENT_TODO = 0,
		ELEMENT_DONE,
		ELEMENT_NONE //Non applicable
	}map_state_e;

	typedef struct
	{
		detection_information_t sensor [SENSOR_NB];
		//#warning voir si changement element_t
		// Warning tr�s tr�s g�nant pour la compilation, et c'est s�r qu'il va changer element_t ^^
		element_t elements_list[NB_ELEMENTS];
		Uint8 nb_elements;
		color_e	color;
		color_e	wanted_color;
		bool_e color_updated;
		bool_e ask_asser_calibration;
		bool_e ask_start;
		config_t config;
		config_t wanted_config;
		bool_e config_updated;
		asser_env_t asser;
		position_t pos;		//comme son nom l'indique, c'est la position de notre robot
		position_t foe[NB_FOES];		//comme son nom l'indique, c'est la position de l'adversaire
		bool_e match_started, match_over;
		time32_t match_time; //temps de match en ms.
		time32_t absolute_time; //temps depuis le lancement du module clock en ms.
		/*Tableau d'�tat des elements du terrain */
		map_state_e map_elements[(map_goal_e)40]; //Voir doc pour connaitre les �l�ments associ�s
		Uint8 color_ball;
		bool_e debug_force_foe;

		//Strat verres: on a gard� les verres car on a pas pu les ranger, il faudra les poser a la fin
		bool_e must_drop_glasses_at_end;
		Sint16 glasses_x_pos;			//Position en X des verres. Pour savoir ou ils sont pour les proteger � la fin du match
	}environment_t;



	//NOTE MICHAEL: Mettre � jour les fonctions suivantes pour le tableau des elements

	/* baisse les drapeaux d'environnement pour pr�parer la prochaine MaJ */
	void ENV_clean();

	/* initialise toutes les variables d'environnement */
	void ENV_init();

	/* met � jour l'environnement */
	void ENV_update();

	/* Change la couleur et pr�vient tout le monde */
	void ENV_set_color(color_e color);


	/* Cause � l'autre robot pour savoir s'il est pr�sent...*/
	void ENV_XBEE_ping_process(void);

	/* Permet d'�x�cuter un traitement rapide dans l'interruption suivantle message CAN re�u 
   (ATTENTION : le message n'est pas plac� dans le buffer) */
	bool_e CAN_fast_update(CAN_msg_t* msg);
	
	/* Fonction de filtrage des points appartenants au terrain.
	Elle renvoie si le point est valide ou non c�d hors des zones de d�part, des zones s�curis�es 
	et �cart� de delta des bordures du terrain */
	bool_e ENV_game_zone_filter(Sint16 x, Sint16 y, Uint16 delta);


#endif /* ndef ENVIRONMENT_H */
