/*  Club Robot ESEO 2016 - 2017
 *
 *	Fichier : cylinder_dispose_right_config.h
 *	Package : Carte actionneur
 *	Description : Gestion de l'actionneur AX12 CYLINDER_DISPOSE_RIGHT
 *  Auteur :
 *  Version 2017
 *  Robot : BIG
 */


#ifndef CYLINDER_DISPOSE_RIGHT_CONFIG_H
#define	CYLINDER_DISPOSE_RIGHT_CONFIG_H

//Config
	#define CYLINDER_DISPOSE_RIGHT_AX12_ASSER_TIMEOUT				2000	// Si l'ax12 n'atteint pas la position demand�e avant ce temps, on consid�re qu'il y a un probl�me. Temps en ms.
	#define CYLINDER_DISPOSE_RIGHT_AX12_ASSER_POS_EPSILON			6		// Lorsque l'ax12 atteint sa position avec +/- POS_EPSILON degr� d'�cart max, on consid�re qu'il a atteint sa position. Angle en degr�.
	#define CYLINDER_DISPOSE_RIGHT_AX12_ASSER_POS_LARGE_EPSILON		28		// Si le d�placement de l'ax12 timeout mais que sa position est � +/- POS_LARGE_EPSILON degr� d'�cart max, on consid�re qu'il a atteint sa position. Angle en degr�.
	#define CYLINDER_DISPOSE_RIGHT_AX12_MAX_TORQUE_PERCENT		    50		// Couple maximum en pourcentage de l'ax12. Utilis� pour limiter le courant dans le moteur. A mettre a une valeur correcte pour pincer assez fort sans risquer d'endommager l'AX12.
	#define CYLINDER_DISPOSE_RIGHT_AX12_MAX_TEMPERATURE				60		// Temp�rature maximale en degr�
	#define CYLINDER_DISPOSE_RIGHT_AX12_SPEED						30		// Vitesse par d�faut de l'AX12

//Position
	#define CYLINDER_DISPOSE_RIGHT_AX12_IDLE_POS					42
	#define CYLINDER_DISPOSE_RIGHT_AX12_TAKE_POS				    130		//en degr�
	#define CYLINDER_DISPOSE_RIGHT_AX12_RAISE_POS					110		//en degr�
	#define CYLINDER_DISPOSE_RIGHT_AX12_DISPOSE_POS					426		//en degr�

	#define CYLINDER_DISPOSE_RIGHT_AX12_WARNER_POS					512

//Position initiale de l'AX12
	#define CYLINDER_DISPOSE_RIGHT_AX12_INIT_POS					CYLINDER_DISPOSE_RIGHT_AX12_IDLE_POS

//Extremum valeur
	#define CYLINDER_DISPOSE_RIGHT_AX12_MIN_VALUE					0
	#define CYLINDER_DISPOSE_RIGHT_AX12_MAX_VALUE					1024

#endif	/* CYLINDER_DISPOSE_RIGHT_CONFIG_H */


