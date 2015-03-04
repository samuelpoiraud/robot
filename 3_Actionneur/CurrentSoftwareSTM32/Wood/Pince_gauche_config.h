/*  Club Robot ESEO 2014 - 2015
 *	SMALL
 *
 *	Fichier : Pince_Gauche_config.h
 *	Package : Carte actionneur
 *	Description : Gestion de l'ax12 de la pince gauche
 *  Auteur : Valentin
 *  Version 20130219
 *  Robot : SMALL
 */

#ifndef PINCE_GAUCHE_CONFIG_H
#define	PINCE_GAUCHE_CONFIG_H

//Config
	#define PINCE_GAUCHE_AX12_ASSER_TIMEOUT				1000	// Si l'ax12 n'atteint pas la position demand�e avant ce temps, on consid�re qu'il y a un probl�me. Temps en ms.
	#define PINCE_GAUCHE_AX12_ASSER_POS_EPSILON			2		// Lorsque l'ax12 atteint sa position avec +/- POS_EPSILON degr� d'�cart max, on consid�re qu'il a atteint sa position. Angle en degr�.
	#define PINCE_GAUCHE_AX12_ASSER_POS_LARGE_EPSILON	10		// Si le d�placement de l'ax12 timeout mais que sa position est � +/- POS_LARGE_EPSILON degr� d'�cart max, on consid�re qu'il a atteint sa position. Angle en degr�.
	#define PINCE_GAUCHE_AX12_MAX_TORQUE_PERCENT		100		// Couple maximum en pourcentage de l'ax12. Utilis� pour limiter le courant dans le moteur. A mettre a une valeur correcte pour pincer assez fort sans risquer d'endommager l'AX12.

//Position
	#define PINCE_GAUCHE_AX12_CLOSE_POS					77		//en degr�
	#define PINCE_GAUCHE_AX12_MID_POS					102		//en degr� 25� de diff avec IDLE POS
	#define PINCE_GAUCHE_AX12_OPEN_POS					144		//en degr�

//Position initiale de l'AX12
	#define PINCE_GAUCHE_AX12_INIT_POS					PINCE_GAUCHE_AX12_CLOSE_POS

//Extremum valeur
	#define PINCE_GAUCHE_AX12_MAX_VALUE					300
	#define PINCE_GAUCHE_AX12_MIN_VALUE					0

#endif	/* PINCE_GAUCHE_CONFIG_H */


