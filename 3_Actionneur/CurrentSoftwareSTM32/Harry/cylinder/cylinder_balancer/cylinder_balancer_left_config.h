/*  Club Robot ESEO 2016 - 2017
 *
 *	Fichier : cylinder_balancer_left_config.h
 *	Package : Carte actionneur
 *	Description : Gestion de l'actionneur AX12 CYLINDER_BALANCER_LEFT
 *  Auteur :
 *  Version 2017
 *  Robot : BIG
 */


#ifndef CYLINDER_BALANCER_LEFT_CONFIG_H
#define	CYLINDER_BALANCER_LEFT_CONFIG_H

//Config
	#define CYLINDER_BALANCER_LEFT_AX12_ASSER_TIMEOUT				2000	// Si l'ax12 n'atteint pas la position demandée avant ce temps, on considère qu'il y a un problème. Temps en ms.
	#define CYLINDER_BALANCER_LEFT_AX12_ASSER_POS_EPSILON			6		// Lorsque l'ax12 atteint sa position avec +/- POS_EPSILON degré d'écart max, on considère qu'il a atteint sa position. Angle en degré.
	#define CYLINDER_BALANCER_LEFT_AX12_ASSER_POS_LARGE_EPSILON		28		// Si le déplacement de l'ax12 timeout mais que sa position est à +/- POS_LARGE_EPSILON degré d'écart max, on considère qu'il a atteint sa position. Angle en degré.
	#define CYLINDER_BALANCER_LEFT_AX12_MAX_TORQUE_PERCENT		    50		// Couple maximum en pourcentage de l'ax12. Utilisé pour limiter le courant dans le moteur. A mettre a une valeur correcte pour pincer assez fort sans risquer d'endommager l'AX12.
	#define CYLINDER_BALANCER_LEFT_AX12_MAX_TEMPERATURE				60		// Température maximale en degré

//Position
	#define CYLINDER_BALANCER_LEFT_AX12_IDLE_POS					512
	#define CYLINDER_BALANCER_LEFT_AX12_IN_POS				   		512		//en degré
	#define CYLINDER_BALANCER_LEFT_AX12_OUT_POS						512		//en degré

//Position initiale de l'AX12
	#define CYLINDER_BALANCER_LEFT_AX12_INIT_POS					CYLINDER_BALANCER_LEFT_AX12_IDLE_POS

//Extremum valeur
	#define CYLINDER_BALANCER_LEFT_AX12_MIN_VALUE					0
	#define CYLINDER_BALANCER_LEFT_AX12_MAX_VALUE					1024

#endif	/* CYLINDER_BALANCER_LEFT_CONFIG_H */


