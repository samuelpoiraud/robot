/*  Club Robot ESEO 2016 - 2017
 *
 *	Fichier : small_cylinder_dispose_config.h
 *	Package : Carte actionneur
 *	Description : Gestion de l'actionneur AX12 SMALL_CYLINDER_DISPOSE
 *  Auteur :
 *  Version 2017
 *  Robot : BIG
 */


#ifndef SMALL_CYLINDER_DISPOSE_CONFIG_H
#define	SMALL_CYLINDER_DISPOSE_CONFIG_H

//Config
	#define SMALL_CYLINDER_DISPOSE_AX12_ASSER_TIMEOUT				2000	// Si l'ax12 n'atteint pas la position demandée avant ce temps, on considère qu'il y a un problème. Temps en ms.
	#define SMALL_CYLINDER_DISPOSE_AX12_ASSER_POS_EPSILON			6		// Lorsque l'ax12 atteint sa position avec +/- POS_EPSILON degré d'écart max, on considère qu'il a atteint sa position. Angle en degré.
	#define SMALL_CYLINDER_DISPOSE_AX12_ASSER_POS_LARGE_EPSILON		28		// Si le déplacement de l'ax12 timeout mais que sa position est à +/- POS_LARGE_EPSILON degré d'écart max, on considère qu'il a atteint sa position. Angle en degré.
	#define SMALL_CYLINDER_DISPOSE_AX12_MAX_TORQUE_PERCENT		    90		// Couple maximum en pourcentage de l'ax12. Utilisé pour limiter le courant dans le moteur. A mettre a une valeur correcte pour pincer assez fort sans risquer d'endommager l'AX12.
	#define SMALL_CYLINDER_DISPOSE_AX12_MAX_TEMPERATURE				60		// Température maximale en degré
	#define SMALL_CYLINDER_DISPOSE_AX12_SPEED						30		// Vitesse par défaut de l'AX12

//Position
	#define SMALL_CYLINDER_DISPOSE_AX12_IDLE_POS					519
	#define SMALL_CYLINDER_DISPOSE_AX12_TAKE_POS				    430		//en degré
	#define SMALL_CYLINDER_DISPOSE_AX12_RAISE_POS				445		//en degré
	#define SMALL_CYLINDER_DISPOSE_AX12_DISPOSE_POS				128		//en degré

	#define SMALL_CYLINDER_DISPOSE_AX12_WARNER_POS				512

//Position initiale de l'AX12
	#define SMALL_CYLINDER_DISPOSE_AX12_INIT_POS					SMALL_CYLINDER_DISPOSE_AX12_IDLE_POS

//Extremum valeur
	#define SMALL_CYLINDER_DISPOSE_AX12_MIN_VALUE					0
	#define SMALL_CYLINDER_DISPOSE_AX12_MAX_VALUE					1024

#endif	/* SMALL_CYLINDER_DISPOSE_CONFIG_H */


