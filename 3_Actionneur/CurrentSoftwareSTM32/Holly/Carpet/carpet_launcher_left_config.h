/*  Club Robot ESEO 2014 - 2015
 *	HOLLY - WOOD
 *
 *	Fichier : carpet_launcher_left.c
 *	Package : Carte actionneur
 *	Description : Gestion de l'ax12 du lanceur de tapis gauche
 *  Auteur : Arnaud
 *  Version 20130219
 *  Robot : HOLLY
 */

#ifndef CARPET_LAUNCHER_LEFT_CONFIG_H
#define	CARPET_LAUNCHER_LEFT_CONFIG_H

//Config
	#define CARPET_LAUNCHER_LEFT_AX12_ASSER_TIMEOUT					1000			// Si l'ax12 n'atteint pas la position demand�e avant ce temps, on consid�re qu'il y a un probl�me. Temps en ms.
	#define CARPET_LAUNCHER_LEFT_AX12_ASSER_POS_EPSILON				6				// Lorsque l'ax12 atteint sa position avec +/- POS_EPSILON degr� d'�cart max, on consid�re qu'il a atteint sa position. Angle en degr�.
	#define CARPET_LAUNCHER_LEFT_AX12_ASSER_POS_LARGE_EPSILON		28				// Si le d�placement de l'ax12 timeout mais que sa position est � +/- POS_LARGE_EPSILON degr� d'�cart max, on consid�re qu'il a atteint sa position. Angle en degr�.
	#define CARPET_LAUNCHER_LEFT_AX12_MAX_TORQUE_PERCENT			70				// Couple maximum en pourcentage de l'ax12. Utilis� pour limiter le courant dans le moteur. A mettre a une valeur correcte pour pincer assez fort sans risquer d'endommager l'AX12.
	#define CARPET_LAUNCHER_LEFT_AX12_MAX_TEMPERATURE               60				// Temp�rature maximale en degr�

//Position
	#define CARPET_LAUNCHER_LEFT_AX12_IDLE_POS						799				//en degr�
	#define CARPET_LAUNCHER_LEFT_AX12_LOADING_POS					730				//en degr�
	#define CARPET_LAUNCHER_LEFT_AX12_LAUNCH_POS					275				//en degr�

//Position initiale de l'AX12
	#define CARPET_LAUNCHER_LEFT_AX12_INIT_POS						CARPET_LAUNCHER_LEFT_AX12_IDLE_POS

//Extremum valeur
	#define CARPET_LAUNCHER_LEFT_AX12_MIN_VALUE						0
	#define CARPET_LAUNCHER_LEFT_AX12_MAX_VALUE						1024

#endif	/* CARPET_LAUNCHER_LEFT_CONFIG_H */


