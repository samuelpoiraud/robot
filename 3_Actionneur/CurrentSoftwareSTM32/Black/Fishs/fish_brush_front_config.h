/*  Club Robot ESEO 2015 - 2016
 *	BIG
 *
 *	Fichier : fich_brush_config.h
 *	Package : Carte actionneur
 *	Description : Gestion de l'actionneur fich_brush
 *  Auteur : Cailyn
 *  Version 2016
 *  Robot : BIG
 */

#ifndef FISH_BRUSH_FRONT_CONFIG_H
#define	FISH_BRUSH_FRONT_CONFIG_H

//Config
	#define FISH_BRUSH_FRONT_AX12_RX24_ASSER_TIMEOUT			2000	// Si l'RX24 n'atteint pas la position demand�e avant ce temps, on consid�re qu'il y a un probl�me. Temps en ms.
	#define FISH_BRUSH_FRONT_AX12_RX24_ASSER_POS_EPSILON		6		// Lorsque le RX24 atteint sa position avec +/- POS_EPSILON degr� d'�cart max, on consid�re qu'il a atteint sa position. Angle en degr�.
	#define FISH_BRUSH_FRONT_AX12_RX24_ASSER_POS_LARGE_EPSILON	28		// Si le d�placement de le RX24 timeout mais que sa position est � +/- POS_LARGE_EPSILON degr� d'�cart max, on consid�re qu'il a atteint sa position. Angle en degr�.
	#define FISH_BRUSH_FRONT_AX12_RX24_MAX_TORQUE_PERCENT		50		// Couple maximum en pourcentage de le RX24. Utilis� pour limiter le courant dans le moteur. A mettre a une valeur correcte pour pincer assez fort sans risquer d'endommager le RX24.
	#define FISH_BRUSH_FRONT_AX12_RX24_MAX_TEMPERATURE			60		// Temp�rature maximale en degr�

//Position
	#define FISH_BRUSH_FRONT_AX12_RX24_IDLE_POS					990
	#define FISH_BRUSH_FRONT_AX12_RX24_CLOSE_POS				990		//en degr�
	#define FISH_BRUSH_FRONT_AX12_RX24_OPEN_POS					760  	//en degr�

//Position initiale du RX24
	#define FISH_BRUSH_FRONT_AX12_RX24_INIT_POS					FISH_BRUSH_FRONT_AX12_RX24_IDLE_POS

//Extremum valeur
	#define FISH_BRUSH_FRONT_AX12_RX24_MIN_VALUE			0
	#define FISH_BRUSH_FRONT_AX12_RX24_MAX_VALUE				1024

#endif	/* FISH_BRUSH_FRONT_CONFIG_H */

