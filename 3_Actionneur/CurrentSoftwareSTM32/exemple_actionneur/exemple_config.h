/*  Club Robot ESEO 2015 - 2016
 *	BIG
 *
 *	Fichier : exemple_config.h
 *	Package : Carte actionneur
 *	Description : Gestion de l'actionneur exemple
 *  Auteur :
 *  Version 2016
 *  Robot : BIG
 */

#ifndef EXEMPLE_CONFIG_H
#define	EXEMPLE_CONFIG_H

//Config
	#define EXEMPLE_AX12_ASSER_TIMEOUT				2000	// Si l'ax12 n'atteint pas la position demandée avant ce temps, on considère qu'il y a un problème. Temps en ms.
	#define EXEMPLE_AX12_ASSER_POS_EPSILON			6		// Lorsque l'ax12 atteint sa position avec +/- POS_EPSILON degré d'écart max, on considère qu'il a atteint sa position. Angle en degré.
	#define EXEMPLE_AX12_ASSER_POS_LARGE_EPSILON	28		// Si le déplacement de l'ax12 timeout mais que sa position est à +/- POS_LARGE_EPSILON degré d'écart max, on considère qu'il a atteint sa position. Angle en degré.
	#define EXEMPLE_AX12_MAX_TORQUE_PERCENT		    50		// Couple maximum en pourcentage de l'ax12. Utilisé pour limiter le courant dans le moteur. A mettre a une valeur correcte pour pincer assez fort sans risquer d'endommager l'AX12.
	#define EXEMPLE_AX12_MAX_TEMPERATURE			60		// Température maximale en degré

//Position
	#define EXEMPLE_AX12_IDLE_POS					512
	#define EXEMPLE_AX12_CLOSE_POS				    512		//en degré
	#define EXEMPLE_AX12_OPEN_POS					512		//en degré

//Position initiale de l'AX12
	#define EXEMPLE_AX12_INIT_POS					EXEMPLE_AX12_IDLE_POS

//Extremum valeur
	#define EXEMPLE_AX12_MIN_VALUE					0
	#define EXEMPLE_AX12_MAX_VALUE					1024

#endif	/* EXEMPLE_CONFIG_H */


