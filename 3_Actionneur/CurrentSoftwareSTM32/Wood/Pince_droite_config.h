/*  Club Robot ESEO 2014 - 2015
 *	SMALL
 *
 *	Fichier : Pince_droite_config.c
 *	Package : Carte actionneur
 *	Description : Gestion de l'actionneur de la pince droite
 *  Auteur : Valentin
 *  Version 20130219
 *  Robot : SMALL
 */

#ifndef PINCE_DROITE_CONFIG_H
#define	PINCE_DROITE_CONFIG_H

//Config
	#define PINCE_DROITE_AX12_ASSER_TIMEOUT				1000	// Si l'ax12 n'atteint pas la position demand�e avant ce temps, on consid�re qu'il y a un probl�me. Temps en ms.
	#define PINCE_DROITE_AX12_ASSER_POS_EPSILON			6		// Lorsque l'ax12 atteint sa position avec +/- POS_EPSILON degr� d'�cart max, on consid�re qu'il a atteint sa position. Angle en degr�.
	#define PINCE_DROITE_AX12_ASSER_POS_LARGE_EPSILON	28		// Si le d�placement de l'ax12 timeout mais que sa position est � +/- POS_LARGE_EPSILON degr� d'�cart max, on consid�re qu'il a atteint sa position. Angle en degr�.
	#define PINCE_DROITE_AX12_MAX_TORQUE_PERCENT		30		// Couple maximum en pourcentage de l'ax12. Utilis� pour limiter le courant dans le moteur. A mettre a une valeur correcte pour pincer assez fort sans risquer d'endommager l'AX12.
	#define PINCE_DROITE_AX12_MAX_TEMPERATURE			60		// Temp�rature maximale en degr�

//Position
	#define PINCE_DROITE_AX12_IDLE_POS					200
	#define PINCE_DROITE_AX12_CLOSE_POS					730		//en degr�
	#define PINCE_DROITE_AX12_MID_POS					695  	//en degr�  35� de diff avec IDLE POS
	#define PINCE_DROITE_AX12_OPEN_POS					512		//en degr�

//Position initiale de l'AX12
	#define PINCE_DROITE_AX12_INIT_POS					PINCE_DROITE_AX12_IDLE_POS

//Extremum valeur
	#define PINCE_DROITE_AX12_MIN_VALUE					0
	#define PINCE_DROITE_AX12_MAX_VALUE					1024

#endif	/* PINCE_DROITE_CONFIG_H */


