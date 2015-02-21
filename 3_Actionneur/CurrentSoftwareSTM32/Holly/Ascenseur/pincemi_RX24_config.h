/*  Club Robot ESEO 2014 - 2015
 *	HOLLY - WOOD
 *
 *	Fichier : pincemi_RX24_config.h
 *	Package : Carte actionneur
 *	Description : Gestion des pinces de l'actionneur spot
 *  Auteur : Arnaud
 *  Version 20130219
 *  Robot : HOLLY
 */

#ifndef PINCEMI_RX24_CONFIG_H
#define	PINCEMI_RX24_CONFIG_H

//Config
	#define PINCEMI_RX24_ASSER_TIMEOUT								1000			// Si l'ax12 n'atteint pas la position demand�e avant ce temps, on consid�re qu'il y a un probl�me. Temps en ms.
	#define PINCEMI_RX24_ASSER_POS_EPSILON							2				// Lorsque l'ax12 atteint sa position avec +/- POS_EPSILON degr� d'�cart max, on consid�re qu'il a atteint sa position. Angle en degr�.
	#define PINCEMI_RX24_ASSER_POS_LARGE_EPSILON					10				// Si le d�placement de l'ax12 timeout mais que sa position est � +/- POS_LARGE_EPSILON degr� d'�cart max, on consid�re qu'il a atteint sa position. Angle en degr�.
	#define PINCEMI_RX24_MAX_TORQUE_PERCENT							100				// Couple maximum en pourcentage de l'ax12. Utilis� pour limiter le courant dans le moteur. A mettre a une valeur correcte pour pincer assez fort sans risquer d'endommager l'AX12.

//Position

	// Pince droite, mord droit
	#define PINCEMIR_RIGHT_CLOSE									50
	#define PINCEMIR_RIGHT_CLOSE_INNER								150
	#define PINCEMIR_RIGHT_LOCK										150
	#define PINCEMIR_RIGHT_OPEN										150
	#define PINCEMIR_RIGHT_OPEN_GREAT								150

	// Pince droite, mord gauche
	#define PINCEMIR_LEFT_CLOSE										50
	#define PINCEMIR_LEFT_CLOSE_INNER								150
	#define PINCEMIR_LEFT_LOCK										150
	#define PINCEMIR_LEFT_OPEN										150
	#define PINCEMIR_LEFT_OPEN_GREAT								150

	// Pince gauche, mord droit
	#define PINCEMIL_RIGHT_CLOSE									50
	#define PINCEMIL_RIGHT_CLOSE_INNER								150
	#define PINCEMIL_RIGHT_LOCK										150
	#define PINCEMIL_RIGHT_OPEN										150
	#define PINCEMIL_RIGHT_OPEN_GREAT								150

	// Pince gauche, mord gauche
	#define PINCEMIL_LEFT_CLOSE										50
	#define PINCEMIL_LEFT_CLOSE_INNER								150
	#define PINCEMIL_LEFT_LOCK										150
	#define PINCEMIL_LEFT_OPEN										150
	#define PINCEMIL_LEFT_OPEN_GREAT								150

//Position initiale du RX24
	#define PINCEMIR_RIGHT_INIT_POS									PINCEMIR_RIGHT_CLOSE
	#define PINCEMIR_LEFT_INIT_POS									PINCEMIR_LEFT_CLOSE
	#define PINCEMIL_RIGHT_INIT_POS									PINCEMIL_RIGHT_CLOSE
	#define PINCEMIL_LEFT_INIT_POS									PINCEMIL_LEFT_CLOSE


//Extremum valeur
	#define PINCEMI_RX24_MIN_VALUE									0
	#define PINCEMI_RX24_MAX_VALUE									300


#endif	/* PINCEMI_RX24_CONFIG_H */


