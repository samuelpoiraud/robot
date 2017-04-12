/*  Club Robot ESEO 2016 - 2017
 *
 *	Fichier : small_cylinder_balancer_config.h
 *	Package : Carte actionneur
 *	Description : Gestion de l'actionneur AX12 SMALL_CYLINDER_BALANCER
 *  Auteur :
 *  Version 2017
 *  Robot : BIG
 */


#ifndef SMALL_CYLINDER_BALANCER_CONFIG_H
#define	SMALL_CYLINDER_BALANCER_CONFIG_H

//Config
	#define SMALL_CYLINDER_BALANCER_AX12_ASSER_TIMEOUT				2000	// Si l'ax12 n'atteint pas la position demand�e avant ce temps, on consid�re qu'il y a un probl�me. Temps en ms.
	#define SMALL_CYLINDER_BALANCER_AX12_ASSER_POS_EPSILON			6		// Lorsque l'ax12 atteint sa position avec +/- POS_EPSILON degr� d'�cart max, on consid�re qu'il a atteint sa position. Angle en degr�.
	#define SMALL_CYLINDER_BALANCER_AX12_ASSER_POS_LARGE_EPSILON		28		// Si le d�placement de l'ax12 timeout mais que sa position est � +/- POS_LARGE_EPSILON degr� d'�cart max, on consid�re qu'il a atteint sa position. Angle en degr�.
	#define SMALL_CYLINDER_BALANCER_AX12_MAX_TORQUE_PERCENT		    50		// Couple maximum en pourcentage de l'ax12. Utilis� pour limiter le courant dans le moteur. A mettre a une valeur correcte pour pincer assez fort sans risquer d'endommager l'AX12.
	#define SMALL_CYLINDER_BALANCER_AX12_MAX_TEMPERATURE				60		// Temp�rature maximale en degr�
	#define SMALL_CYLINDER_BALANCER_AX12_SPEED						100		// Vitesse par d�faut de l'AX12

//Position
	#define SMALL_CYLINDER_BALANCER_AX12_IDLE_POS					886
	#define SMALL_CYLINDER_BALANCER_AX12_IN_POS				    	886		//en degr�
	#define SMALL_CYLINDER_BALANCER_AX12_OUT_POS						297		//en degr�

//Position initiale de l'AX12
	#define SMALL_CYLINDER_BALANCER_AX12_INIT_POS					SMALL_CYLINDER_BALANCER_AX12_IDLE_POS

//Extremum valeur
	#define SMALL_CYLINDER_BALANCER_AX12_MIN_VALUE					0
	#define SMALL_CYLINDER_BALANCER_AX12_MAX_VALUE					1024

#endif	/* SMALL_CYLINDER_BALANCER_CONFIG_H */

