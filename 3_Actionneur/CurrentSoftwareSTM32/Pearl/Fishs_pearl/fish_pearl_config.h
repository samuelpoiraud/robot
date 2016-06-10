/*  Club Robot ESEO 2015 - 2016
 *	BIG
 *
 *	Fichier : FISH_PEARL_config.h
 *	Package : Carte actionneur
 *	Description : Gestion de l'actionneur FISH_PEARL
 *  Auteur :
 *  Version 2016
 *  Robot : BIG
 */

#ifndef FISH_PEARL_CONFIG_H
#define	FISH_PEARL_CONFIG_H

//Config
	#define FISH_PEARL_AX12_ASSER_TIMEOUT				2000	// Si l'ax12 n'atteint pas la position demand�e avant ce temps, on consid�re qu'il y a un probl�me. Temps en ms.
	#define FISH_PEARL_AX12_ASSER_POS_EPSILON			6		// Lorsque l'ax12 atteint sa position avec +/- POS_EPSILON degr� d'�cart max, on consid�re qu'il a atteint sa position. Angle en degr�.
	#define FISH_PEARL_AX12_ASSER_POS_LARGE_EPSILON		28		// Si le d�placement de l'ax12 timeout mais que sa position est � +/- POS_LARGE_EPSILON degr� d'�cart max, on consid�re qu'il a atteint sa position. Angle en degr�.
	#define FISH_PEARL_AX12_MAX_TORQUE_PERCENT		    50		// Couple maximum en pourcentage de l'ax12. Utilis� pour limiter le courant dans le moteur. A mettre a une valeur correcte pour pincer assez fort sans risquer d'endommager l'AX12.
	#define FISH_PEARL_AX12_MAX_TEMPERATURE				60		// Temp�rature maximale en degr�

//Position
	#define FISH_PEARL_AX12_IDLE_POS					818
	#define FISH_PEARL_AX12_CLOSE_POS				    750		//en degr�
	#define FISH_PEARL_AX12_OPEN_POS					500		//en degr�
	#define FISH_PEARL_AX12_TAPOTAGE_POS                525



//Position initiale de l'AX12
	#define FISH_PEARL_AX12_INIT_POS					FISH_PEARL_AX12_IDLE_POS

//Extremum valeur
	#define FISH_PEARL_AX12_MIN_VALUE					0
	#define FISH_PEARL_AX12_MAX_VALUE					1024

#endif	/* FISH_PEARL_CONFIG_H */


