/*  Club Robot ESEO 2016 - 2017
*
*	Fichier : ORE_GUN_rx24_config.c
*	Package : Carte actionneur
*	Description : Gestion de l'actionneur RX24 ORE_GUN
*	Auteur :
*	Version 2017
*	Robot : BIG
*/

#ifndef ORE_GUN_CONFIG_H
#define	ORE_GUN_CONFIG_H

//Config
	#define ORE_GUN_RX24_ASSER_TIMEOUT				2000	// Si le RX24 n'atteint pas la position demandée avant ce temps, on considère qu'il y a un problème. Temps en ms.
	#define ORE_GUN_RX24_ASSER_POS_EPSILON			6		// Lorsque le RX24 atteint sa position avec +/- POS_EPSILON degré d'écart max, on considère qu'il a atteint sa position. Angle en degré.
	#define ORE_GUN_RX24_ASSER_POS_LARGE_EPSILON	28		// Si le déplacement de le RX24 timeout mais que sa position est à +/- POS_LARGE_EPSILON degré d'écart max, on considère qu'il a atteint sa position. Angle en degré.
	#define ORE_GUN_RX24_MAX_TORQUE_PERCENT		    50		// Couple maximum en pourcentage de le RX24. Utilisé pour limiter le courant dans le moteur. A mettre a une valeur correcte pour pincer assez fort sans risquer d'endommager le RX24.
	#define ORE_GUN_RX24_MAX_TEMPERATURE			60		// Température maximale en degré
	#define ORE_GUN_RX24_SPEED						100		// Vitesse par défaut du RX24

//Position
	#define ORE_GUN_RX24_IDLE_POS					388 //362
    #define ORE_GUN_RX24_DOWN_POS			     	540	//en degré
    #define ORE_GUN_RX24_UP_POS						362	//en degré

//Position initiale de l'RX24
	#define ORE_GUN_RX24_INIT_POS					ORE_GUN_RX24_IDLE_POS

//Extremum valeur
	#define ORE_GUN_RX24_MIN_VALUE					0
	#define ORE_GUN_RX24_MAX_VALUE					1024

#endif	/* ORE_GUN_CONFIG_H */


