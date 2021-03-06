/*  Club Robot ESEO 2016 - 2017
 *
 *	Fichier : CYLINDER_SLOPE_RIGHT_ax12_config.h
 *	Package : Carte actionneur
 *	Description : Gestion de l'actionneur AX12 CYLINDER_SLOPE_RIGHT
 *  Auteur :
 *  Version 2017
 *  Robot : BIG
 */


#ifndef CYLINDER_SLOPE_RIGHT_CONFIG_H
#define	CYLINDER_SLOPE_RIGHT_CONFIG_H

//Config
	#define CYLINDER_SLOPE_RIGHT_AX12_ASSER_TIMEOUT				2000	// Si l'ax12 n'atteint pas la position demand�e avant ce temps, on consid�re qu'il y a un probl�me. Temps en ms.
	#define CYLINDER_SLOPE_RIGHT_AX12_ASSER_POS_EPSILON			6		// Lorsque l'ax12 atteint sa position avec +/- POS_EPSILON degr� d'�cart max, on consid�re qu'il a atteint sa position. Angle en degr�.
	#define CYLINDER_SLOPE_RIGHT_AX12_ASSER_POS_LARGE_EPSILON	28		// Si le d�placement de l'ax12 timeout mais que sa position est � +/- POS_LARGE_EPSILON degr� d'�cart max, on consid�re qu'il a atteint sa position. Angle en degr�.
	#define CYLINDER_SLOPE_RIGHT_AX12_MAX_TORQUE_PERCENT		50		// Couple maximum en pourcentage de l'ax12. Utilis� pour limiter le courant dans le moteur. A mettre a une valeur correcte pour pincer assez fort sans risquer d'endommager l'AX12.
	#define CYLINDER_SLOPE_RIGHT_AX12_MAX_TEMPERATURE			60		// Temp�rature maximale en degr�
	#define CYLINDER_SLOPE_RIGHT_AX12_SPEED						60		// Vitesse par d�faut de l'AX12

//Position
	#define CYLINDER_SLOPE_RIGHT_AX12_IDLE_POS					705		//687
	#define CYLINDER_SLOPE_RIGHT_AX12_DOWN_POS				    705		//en degr�
	#define CYLINDER_SLOPE_RIGHT_AX12_UP_POS					500		//en degr�
	#define CYLINDER_SLOPE_RIGHT_AX12_VERY_UP_POS				390

//Position initiale de l'AX12
	#define CYLINDER_SLOPE_RIGHT_AX12_INIT_POS					CYLINDER_SLOPE_RIGHT_AX12_IDLE_POS

//Extremum valeur
	#define CYLINDER_SLOPE_RIGHT_AX12_MIN_VALUE					0
	#define CYLINDER_SLOPE_RIGHT_AX12_MAX_VALUE					1024

#endif	/* CYLINDER_SLOPE_RIGHT_CONFIG_H */


