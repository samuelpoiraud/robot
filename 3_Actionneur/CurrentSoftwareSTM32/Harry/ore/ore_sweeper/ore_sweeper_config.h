/*  Club Robot ESEO 2016 - 2017
 *
 *	Fichier : ORE_SWEEPER_ax12_config.h
 *	Package : Carte actionneur
 *	Description : Gestion de l'actionneur AX12 ORE_SWEEPER
 *  Auteur :
 *  Version 2017
 *  Robot : BIG
 */


#ifndef ORE_SWEEPER_CONFIG_H
#define	ORE_SWEEPER_CONFIG_H

//Config
	#define ORE_SWEEPER_AX12_ASSER_TIMEOUT				2000	// Si l'ax12 n'atteint pas la position demandée avant ce temps, on considère qu'il y a un problème. Temps en ms.
	#define ORE_SWEEPER_AX12_ASSER_POS_EPSILON			6		// Lorsque l'ax12 atteint sa position avec +/- POS_EPSILON degré d'écart max, on considère qu'il a atteint sa position. Angle en degré.
	#define ORE_SWEEPER_AX12_ASSER_POS_LARGE_EPSILON	28		// Si le déplacement de l'ax12 timeout mais que sa position est à +/- POS_LARGE_EPSILON degré d'écart max, on considère qu'il a atteint sa position. Angle en degré.
	#define ORE_SWEEPER_AX12_MAX_TORQUE_PERCENT		    50		// Couple maximum en pourcentage de l'ax12. Utilisé pour limiter le courant dans le moteur. A mettre a une valeur correcte pour pincer assez fort sans risquer d'endommager l'AX12.
	#define ORE_SWEEPER_AX12_MAX_TEMPERATURE			60		// Température maximale en degré

//Position
	#define ORE_SWEEPER_AX12_IDLE_POS					512
	#define ORE_SWEEPER_AX12_LOCK_POS				    512		//en degré
	#define ORE_SWEEPER_AX12_UNLOCK_POS					512		//en degré

//Position initiale de l'AX12
	#define ORE_SWEEPER_AX12_INIT_POS					ORE_SWEEPER_AX12_IDLE_POS

//Extremum valeur
	#define ORE_SWEEPER_AX12_MIN_VALUE					0
	#define ORE_SWEEPER_AX12_MAX_VALUE					1024

#endif	/* ORE_SWEEPER_CONFIG_H */


