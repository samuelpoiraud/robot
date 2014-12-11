/*  Club Robot ESEO 2014 - 2015
 *	HOLLY - WOOD
 *
 *	Fichier : back_spot_right.c
 *	Package : Carte actionneur
 *	Description : Gestion de l'ax12 de la ventouse de la prise de spot arrière droite
 *  Auteur : Arnaud
 *  Version 20130219
 *  Robot : HOLLY
 */

#ifndef BACK_SPOT_RIGHT_CONFIG_H
#define	BACK_SPOT_RIGHT_CONFIG_H

//Config
	#define BACK_SPOT_RIGHT_AX12_ASSER_TIMEOUT				1000			// Si l'ax12 n'atteint pas la position demandée avant ce temps, on considère qu'il y a un problème. Temps en ms.
	#define BACK_SPOT_RIGHT_AX12_ASSER_POS_EPSILON			2				// Lorsque l'ax12 atteint sa position avec +/- POS_EPSILON degré d'écart max, on considère qu'il a atteint sa position. Angle en degré.
	#define BACK_SPOT_RIGHT_AX12_ASSER_POS_LARGE_EPSILON	10				// Si le déplacement de l'ax12 timeout mais que sa position est à +/- POS_LARGE_EPSILON degré d'écart max, on considère qu'il a atteint sa position. Angle en degré.
	#define BACK_SPOT_RIGHT_AX12_MAX_TORQUE_PERCENT			100				// Couple maximum en pourcentage de l'ax12. Utilisé pour limiter le courant dans le moteur. A mettre a une valeur correcte pour pincer assez fort sans risquer d'endommager l'AX12.

//Position
	#define BACK_SPOT_RIGHT_AX12_OPEN_POS					147				//en degré
	#define BACK_SPOT_RIGHT_AX12_CLOSED_POS					244				//en degré

//Position initiale de l'AX12
	#define BACK_SPOT_RIGHT_AX12_INIT_POS					BACK_SPOT_RIGHT_AX12_CLOSED_POS

//Extremum valeur
	#define BACK_SPOT_RIGHT_AX12_MAX_VALUE					300
	#define BACK_SPOT_RIGHT_AX12_MIN_VALUE					0

#endif	/* BACK_SPOT_RIGHT_CONFIG_H */


