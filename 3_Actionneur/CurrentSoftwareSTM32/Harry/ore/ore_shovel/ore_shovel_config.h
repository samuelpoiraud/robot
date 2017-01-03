/*  Club Robot ESEO 2016 - 2017
*
*	Fichier : ORE_SHOVEL_rx24_config.c
*	Package : Carte actionneur
*	Description : Gestion de l'actionneur RX24 ORE_SHOVEL
*	Auteur :
*	Version 2017
*	Robot : BIG
*/

#ifndef ORE_SHOVEL_CONFIG_H
#define	ORE_SHOVEL_CONFIG_H

//Config
	#define ORE_SHOVEL_RX24_ASSER_TIMEOUT				2000	// Si le RX24 n'atteint pas la position demandée avant ce temps, on considère qu'il y a un problème. Temps en ms.
	#define ORE_SHOVEL_RX24_ASSER_POS_EPSILON			6		// Lorsque le RX24 atteint sa position avec +/- POS_EPSILON degré d'écart max, on considère qu'il a atteint sa position. Angle en degré.
	#define ORE_SHOVEL_RX24_ASSER_POS_LARGE_EPSILON	    28		// Si le déplacement de le RX24 timeout mais que sa position est à +/- POS_LARGE_EPSILON degré d'écart max, on considère qu'il a atteint sa position. Angle en degré.
	#define ORE_SHOVEL_RX24_MAX_TORQUE_PERCENT		    50		// Couple maximum en pourcentage de le RX24. Utilisé pour limiter le courant dans le moteur. A mettre a une valeur correcte pour pincer assez fort sans risquer d'endommager le RX24.
	#define ORE_SHOVEL_RX24_MAX_TEMPERATURE			    60		// Température maximale en degré

//Position
	#define ORE_SHOVEL_RX24_IDLE_POS					512
	#define ORE_SHOVEL_RX24_LOCK_POS			     	512	//en degré
	#define ORE_SHOVEL_RX24_UNLOCK_POS					512	//en degré

//Position initiale de l'RX24
	#define ORE_SHOVEL_RX24_INIT_POS					ORE_SHOVEL_RX24_IDLE_POS

//Extremum valeur
	#define ORE_SHOVEL_RX24_MIN_VALUE					0
	#define ORE_SHOVEL_RX24_MAX_VALUE					1024

#endif	/* ORE_SHOVEL_CONFIG_H */


