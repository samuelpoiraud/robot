/*  Club Robot ESEO 2014 - 2015
 *	SMALL
 *
 *	Fichier : CLAP_config.h
 *	Package : Carte actionneur
 *	Description : Gestion de l'ax12 du clap
 *  Auteur : Valentin
 *  Version 20130219
 *  Robot : SMALL
 */

#ifndef CALP_CONFIG_H
#define	CLAP_CONFIG_H

//Config
	#define CLAP_AX12_ASSER_TIMEOUT				1000	// Si l'ax12 n'atteint pas la position demand�e avant ce temps, on consid�re qu'il y a un probl�me. Temps en ms.
	#define CLAP_AX12_ASSER_POS_EPSILON			6		// Lorsque l'ax12 atteint sa position avec +/- POS_EPSILON degr� d'�cart max, on consid�re qu'il a atteint sa position. Angle en degr�.
	#define CLAP_AX12_ASSER_POS_LARGE_EPSILON	28		// Si le d�placement de l'ax12 timeout mais que sa position est � +/- POS_LARGE_EPSILON degr� d'�cart max, on consid�re qu'il a atteint sa position. Angle en degr�.
	#define CLAP_AX12_MAX_TORQUE_PERCENT		100		// Couple maximum en pourcentage de l'ax12. Utilis� pour limiter le courant dans le moteur. A mettre a une valeur correcte pour pincer assez fort sans risquer d'endommager l'AX12.

//Position
	#define CLAP_AX12_OPEN_POS					512		//en degr�
	#define CLAP_AX12_TAKE_CUP_POS				216		//en degr�
	#define CLAP_AX12_CLOSE_POS					216		//en degr�

//Position initiale de l'AX12
	#define CLAP_AX12_INIT_POS					CLAP_AX12_CLOSE_POS

//Extremum valeur
	#define CLAP_AX12_MIN_VALUE					216
	#define CLAP_AX12_MAX_VALUE					512

#endif	/* CLAP_CONFIG_H */


