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

	#include "QS/QS_CANmsgList.h"
	#include "detection.h"
	//#include "elements.h"
	#include "QS/QS_maths.h"

	/* #define communs à plusieurs modules */
	#define MATCH_DURATION 				90000UL //90 secondes)
	#define GAME_ZONE_SIZE_X			2000 //2000 mm
	#define GAME_ZONE_SIZE_Y			3000 //3000 mm


	//une couleur erronnée pour forcer la maj de la couleur
	#define COLOR_INIT_VALUE 0xFF

	// Macros permettant de symétriser le terrain
	#define COLOR_Y(y)		((global.color == BOT_COLOR) ? (y) : (GAME_ZONE_SIZE_Y - (y)))
	#define COLOR_ANGLE(a)	((global.color == BOT_COLOR) ? (a) : (-(a)))
	#define COLOR_EXP(a,b)	((global.color == BOT_COLOR) ? (a) : (b))

	/* Appelée dans l'INIT : initialise toutes les variables d'environnement */
	void ENV_init(void);

	/* Appelée en début de tache de fond : met à jour l'environnement */
	void ENV_update(void);

	/* Change la couleur et prévient tout le monde */
	void ENV_set_color(color_e color);

	/* Traitement d'un message CAN et renvoie vers d'autres bus */
	void ENV_process_can_msg(CAN_msg_t * incoming_msg, bool_e bCAN, bool_e bU1, bool_e bU2, bool_e bXBee);



#endif /* ndef ENVIRONMENT_H */
