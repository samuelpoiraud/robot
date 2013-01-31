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
		volatile bool_e freine_multi_point;
		volatile bool_e change_point;
		volatile bool_e fini;
		volatile bool_e calibrated;
		volatile bool_e erreur;
		volatile Sint32 vitesse_translation_erreur;
		volatile Sint32 vitesse_rotation_erreur;
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
		volatile double cosAngle;/*cosinus de l'angle du robot*/
		volatile double sinAngle;/*sinus de l'angle du robot*/
		volatile time32_t update_time;
		volatile bool_e updated;
	}position_t;

	typedef struct
	{
		volatile bool_e opened;
		volatile bool_e closed;
		volatile bool_e failure;
		volatile bool_e ready;
	}act_t;

	typedef struct
	{
		detection_information_t sensor [SENSOR_NB];
		//#warning voir si changement element_t
		// Warning très très gênant pour la compilation, et c'est sûr qu'il va changer element_t ^^
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
		act_t act[NB_ACT];	// actionneurs
	}environment_t;

	/* baisse les drapeaux d'environnement pour préparer la prochaine MaJ */
	void ENV_clean();

	/* initialise toutes les variables d'environnement */
	void ENV_init();

	/* met à jour l'environnement */
	void ENV_update();

	/* Change la couleur et prévient tout le monde */
	void ENV_set_color(color_e color);

	/* envoie la config actuelle sur le CAN (pour la super) */
	void ENV_dispatch_config();

	/* Permet d'éxécuter un traitement rapide dans l'interruption suivantle message CAN reçu 
   (ATTENTION : le message n'est pas placé dans le buffer) */
	bool_e CAN_fast_update(CAN_msg_t* msg);
	
    /* Fonction de filtrage des points appartenants au terrain.
	Elle renvoie si le point est valide ou non càd hors des zones de départ, des zones sécurisées 
	et écarté de delta des bordures du terrain */
	bool_e ENV_game_zone_filter(Sint16 x, Sint16 y, Uint16 delta);

	#ifdef ENVIRONMENT_C

		#include "QS/QS_can.h"
		#include "QS/QS_adc.h"
		#include "QS/QS_CANmsgList.h"
		#include "maths_home.h"
		#include "sick.h"
		#include "telemeter.h"
		#include "button.h"
		#include "elements.h"

		/* met à jour l'environnement en fonction du message CAN reçu */
		void CAN_update (CAN_msg_t* incoming_msg);

		/* met a jour la position a partir d'un message asser la délivrant */
		void ENV_pos_update (CAN_msg_t* msg);
			/* Liste des raisons de l'envoi d'un message de BROADCAST_POSITION */
			#define RAISON_TEMPS_ECOULE						(0b00000001)
			#define RAISON_TRANSLATION							(0b00000010)
			#define RAISON_ROTATION							(0b00000100)
			#define RAISON_ATTEINTE_X							(0b00001000)
			#define RAISON_ATTEINTE_Y							(0b00010000)
			#define RAISON_ATTEINTE_TETA						(0b00100000)
			#define RAISON_CHANGE_POINT_MULTI					(0b01000000)
			#define RAISON_FREINE_MULTI 						(0b10000000)
		/* met a jour la position de l'adversaire à partir des balises */
		void ENV_pos_foe_update (CAN_msg_t* msg);
			/* Liste des messages de definition d'erreur --- Pour plus de doc, consulter QS_CANmsgDoc.h */
			#define AUCUNE_ERREUR						(0b00000000)
			#define AUCUN_SIGNAL						(0b00000001)
			#define SIGNAL_INSUFFISANT					(0b00000010)
			#define TACHE_TROP_GRANDE					(0b00000100)
			#define TROP_DE_SIGNAL						(0b00001000)
			#define ERREUR_POSITION_INCOHERENTE 		(0b00010000)
			#define OBSOLESCENCE						(0b10000000)
		
		#define ADC_THRESHOLD 10 //Valeur de l'ADC sans dispositif de connecté
		void ENV_release_selftest();
		
		//une couleur erronnée pour forcer la maj de la couleur
		#define COLOR_INIT_VALUE 0xFF

	#endif /* def ENVIRONMENT_C */

#endif /* ndef ENVIRONMENT_H */
