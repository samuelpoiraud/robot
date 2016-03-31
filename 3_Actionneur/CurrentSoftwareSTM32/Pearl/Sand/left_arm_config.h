/*  Club Robot ESEO 2015 - 2016
 *	SMALL
 *
 *	Fichier : left_arm_config.h
 *	Package : Carte actionneur
 *	Description : Gestion de l'actionneur LEFT_ARM
 *  Auteur : Julien
 *  Version 2016
 *  Robot : SMALL
 */

#ifndef LEFT_ARM_CONFIG_H
#define	LEFT_ARM_CONFIG_H

//Config
	#define LEFT_ARM_AX12_ASSER_TIMEOUT				1000	// Si l'ax12 n'atteint pas la position demandée avant ce temps, on considère qu'il y a un problème. Temps en ms.
	#define LEFT_ARM_AX12_ASSER_POS_EPSILON			6		// Lorsque l'ax12 atteint sa position avec +/- POS_EPSILON degré d'écart max, on considère qu'il a atteint sa position. Angle en degré.
	#define LEFT_ARM_AX12_ASSER_POS_LARGE_EPSILON	28		// Si le déplacement de l'ax12 timeout mais que sa position est à +/- POS_LARGE_EPSILON degré d'écart max, on considère qu'il a atteint sa position. Angle en degré.
	#define LEFT_ARM_AX12_MAX_TORQUE_PERCENT		70		// Couple maximum en pourcentage de l'ax12. Utilisé pour limiter le courant dans le moteur. A mettre a une valeur correcte pour pincer assez fort sans risquer d'endommager l'AX12.
	#define LEFT_ARM_AX12_MAX_TEMPERATURE			60		// Température maximale en degré

//Position
	#define LEFT_ARM_AX12_IDLE_POS					1000
	#define LEFT_ARM_AX12_LOCK_POS				    695
	#define LEFT_ARM_AX12_LOCK_SERRAGE_POS			714
	#define LEFT_ARM_AX12_UNLOCK_POS				480

//Position initiale de l'AX12
	#define LEFT_ARM_AX12_INIT_POS					LEFT_ARM_AX12_IDLE_POS

//Extremum valeur
	#define LEFT_ARM_AX12_MIN_VALUE					0
	#define LEFT_ARM_AX12_MAX_VALUE					1024

#endif	/* LEFT_ARM_CONFIG_H */


