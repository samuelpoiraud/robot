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

// Augmenter l'offset pour serrer plus
	#define OFFSET_LOCK_SPOT										-1

//Config
    #define PINCEMI_RX24_ASSER_TIMEOUT								750         	// Si l'ax12 n'atteint pas la position demandée avant ce temps, on considère qu'il y a un problème. Temps en ms.
	#define PINCEMI_RX24_ASSER_POS_EPSILON							6				// Lorsque l'ax12 atteint sa position avec +/- POS_EPSILON degré d'écart max, on considère qu'il a atteint sa position. Angle en degré.
	#define PINCEMI_RX24_ASSER_POS_LARGE_EPSILON					28				// Si le déplacement de l'ax12 timeout mais que sa position est à +/- POS_LARGE_EPSILON degré d'écart max, on considère qu'il a atteint sa position. Angle en degré.
    #define PINCEMI_RX24_MAX_TORQUE_PERCENT							100				// Couple maximum en pourcentage de l'ax12. Utilisé pour limiter le courant dans le moteur. A mettre a une valeur correcte pour pincer assez fort sans risquer d'endommager l'AX12.
	#define PINCEMI_RX24_MAX_TEMPERATURE							65				// Température maximale en degré

//Position

	// Pince droite, mord droit			ID : 26
	#define PINCEMIR_RIGHT_CLOSE									600
	#define PINCEMIR_RIGHT_CLOSE_INNER								512
	#define PINCEMIR_RIGHT_LOCK										(529 + OFFSET_LOCK_SPOT)
	#define PINCEMIR_RIGHT_LOCK_BALL								485
	#define PINCEMIR_RIGHT_UNLOCK									514
	#define PINCEMIR_RIGHT_OPEN										445
	#define PINCEMIR_RIGHT_OPEN_GREAT								445

	// Pince droite, mord gauche		ID : 25
	#define PINCEMIR_LEFT_CLOSE										425
	#define PINCEMIR_LEFT_CLOSE_INNER								512
	#define PINCEMIR_LEFT_LOCK										(494 - OFFSET_LOCK_SPOT)
	#define PINCEMIR_LEFT_LOCK_BALL									499
	#define PINCEMIR_LEFT_UNLOCK									510
	#define PINCEMIR_LEFT_OPEN										530
	#define PINCEMIR_LEFT_OPEN_GREAT								580

	// Pince gauche, mord droit			ID : 23
	#define PINCEMIL_RIGHT_CLOSE									599
	#define PINCEMIL_RIGHT_CLOSE_INNER								512
	#define PINCEMIL_RIGHT_LOCK										(529 + OFFSET_LOCK_SPOT)
	#define PINCEMIL_RIGHT_LOCK_BALL								485
	#define PINCEMIL_RIGHT_UNLOCK									514
	#define PINCEMIL_RIGHT_OPEN										494
	#define PINCEMIL_RIGHT_OPEN_GREAT								444

	// Pince gauche, mord gauche		ID : 24
	#define PINCEMIL_LEFT_CLOSE										424
	#define PINCEMIL_LEFT_CLOSE_INNER								512
	#define PINCEMIL_LEFT_LOCK										(495 - OFFSET_LOCK_SPOT)
	#define PINCEMIL_LEFT_LOCK_BALL									499
	#define PINCEMIL_LEFT_UNLOCK									510
	#define PINCEMIL_LEFT_OPEN										580
	#define PINCEMIL_LEFT_OPEN_GREAT								580

//Position initiale du RX24
	#define PINCEMIR_RIGHT_INIT_POS									PINCEMIR_RIGHT_OPEN
	#define PINCEMIR_LEFT_INIT_POS									PINCEMIR_LEFT_OPEN
	#define PINCEMIL_RIGHT_INIT_POS									PINCEMIL_RIGHT_OPEN
	#define PINCEMIL_LEFT_INIT_POS									PINCEMIL_LEFT_OPEN


//Extremum valeur
	#define PINCEMI_RX24_MIN_VALUE									375
	#define PINCEMI_RX24_MAX_VALUE									665

#endif	/* PINCEMI_RX24_CONFIG_H */


