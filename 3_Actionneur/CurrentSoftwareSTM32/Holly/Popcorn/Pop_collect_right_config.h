/*  Club Robot ESEO 2014 - 2015
 *	HOLLY - WOOD
 *
 *	Fichier : Pop_collect_right.h
 *	Package : Carte actionneur
 *	Description : Gestion de l'ax12 de la depose des popcorns gauche
 *  Auteur : Anthony
 *  Version 20130219
 *  Robot : HOLLY
 */

#ifndef POP_COLLECT_RIGHT_CONFIG_H
#define	POP_COLLECT_RIGHT_CONFIG_H

//Config
	#define POP_COLLECT_RIGHT_AX12_ASSER_TIMEOUT			1000			// Si l'ax12 n'atteint pas la position demandée avant ce temps, on considère qu'il y a un problème. Temps en ms.
	#define POP_COLLECT_RIGHT_AX12_ASSER_POS_EPSILON		6				// Lorsque l'ax12 atteint sa position avec +/- POS_EPSILON degré d'écart max, on considère qu'il a atteint sa position. Angle en degré.
	#define POP_COLLECT_RIGHT_AX12_ASSER_POS_LARGE_EPSILON	28				// Si le déplacement de l'ax12 timeout mais que sa position est à +/- POS_LARGE_EPSILON degré d'écart max, on considère qu'il a atteint sa position. Angle en degré.
	#define POP_COLLECT_RIGHT_AX12_MAX_TORQUE_PERCENT		100				// Couple maximum en pourcentage de l'ax12. Utilisé pour limiter le courant dans le moteur. A mettre a une valeur correcte pour pincer assez fort sans risquer d'endommager l'AX12.
	#define POP_COLLECT_RIGHT_AX12_MAX_TEMPERATURE           60				// Température maximale en degré

//Position
	#define POP_COLLECT_RIGHT_AX12_IDLE_POS					122				//en degré
	#define POP_COLLECT_RIGHT_AX12_MID_POS					316				//en degré
	#define POP_COLLECT_RIGHT_AX12_OPEN_POS				390				//en degré

//Position initiale de l'AX12
	#define POP_COLLECT_RIGHT_AX12_INIT_POS					POP_COLLECT_RIGHT_AX12_IDLE_POS

//Extremum valeur
	#define POP_COLLECT_RIGHT_AX12_MIN_VALUE				0
	#define POP_COLLECT_RIGHT_AX12_MAX_VALUE				1024

#endif	/* POP_COLLECT_RIGHT_CONFIG_H */



