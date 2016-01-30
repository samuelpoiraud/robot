/*  Club Robot ESEO 2015 - 2016
 *	Fichier : black_sand_circle_config.h
 *	Package : Carte actionneur
 *	Description : Gestion de l'actionneur AX12 BLACK_SAND_CIRCLE
 *  Auteur :
 *  Version 2016
 *  Robot : BIG
 */

#ifndef BLACK_SAND_CIRCLE_CONFIG_H
#define	BLACK_SAND_CIRCLE_CONFIG_H

//Config
	#define BLACK_SAND_CIRCLE_AX12_ASSER_TIMEOUT				1000	// Si l'ax12 n'atteint pas la position demandée avant ce temps, on considère qu'il y a un problème. Temps en ms.
	#define BLACK_SAND_CIRCLE_AX12_ASSER_POS_EPSILON			6		// Lorsque l'ax12 atteint sa position avec +/- POS_EPSILON degré d'écart max, on considère qu'il a atteint sa position. Angle en degré.
	#define BLACK_SAND_CIRCLE_AX12_ASSER_POS_LARGE_EPSILON	    28		// Si le déplacement de l'ax12 timeout mais que sa position est à +/- POS_LARGE_EPSILON degré d'écart max, on considère qu'il a atteint sa position. Angle en degré.
	#define BLACK_SAND_CIRCLE_AX12_MAX_TORQUE_PERCENT		    70		// Couple maximum en pourcentage de l'ax12. Utilisé pour limiter le courant dans le moteur. A mettre a une valeur correcte pour pincer assez fort sans risquer d'endommager l'AX12.
	#define BLACK_SAND_CIRCLE_AX12_MAX_TEMPERATURE			    60		// Température maximale en degré

//Position
	#define BLACK_SAND_CIRCLE_AX12_IDLE_POS					    400
	#define BLACK_SAND_CIRCLE_AX12_CLOSE_POS				    512	    //en degré
	#define BLACK_SAND_CIRCLE_AX12_OPEN_POS					    600		//en degré

//Position initiale de l'AX12
	#define BLACK_SAND_CIRCLE_AX12_INIT_POS					     BLACK_SAND_CIRCLE_AX12_IDLE_POS

//Extremum valeur
	#define BLACK_SAND_CIRCLE_AX12_MIN_VALUE					 0
	#define BLACK_SAND_CIRCLE_AX12_MAX_VALUE					 1024

#endif	/* BLACK_SAND_CIRCLE_CONFIG_H */


