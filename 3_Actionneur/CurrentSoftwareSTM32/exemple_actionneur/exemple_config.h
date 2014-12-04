/*  Club Robot ESEO 2013 - 2014
 *	SMALL
 *
 *	Fichier : exemple_config.c
 *	Package : Carte actionneur
 *	Description : Gestion de l'actionneur d'exemple
 *  Auteur : Arnaud
 *  Version 20130219
 *  Robot : SMALL
 */

#ifndef EXEMPLE_CONFIG_H
#define	EXEMPLE_CONFIG_H

#if 0

//Config
	#define EXEMPLE_AX12_ASSER_TIMEOUT				1000			// Si l'ax12 n'atteint pas la position demandée avant ce temps, on considère qu'il y a un problème. Temps en ms.
	#define EXEMPLE_AX12_ASSER_POS_EPSILON			2				// Lorsque l'ax12 atteint sa position avec +/- POS_EPSILON degré d'écart max, on considère qu'il a atteint sa position. Angle en degré.
	#define EXEMPLE_AX12_ASSER_POS_LARGE_EPSILON	10				// Si le déplacement de l'ax12 timeout mais que sa position est à +/- POS_LARGE_EPSILON degré d'écart max, on considère qu'il a atteint sa position. Angle en degré.
	#define EXEMPLE_AX12_MAX_TORQUE_PERCENT		100				// Couple maximum en pourcentage de l'ax12. Utilisé pour limiter le courant dans le moteur. A mettre a une valeur correcte pour pincer assez fort sans risquer d'endommager l'AX12.

//Position
	#define EXEMPLE_AX12_IDLE_POS					110				//en degré
	#define EXEMPLE_AX12_MID_POS					110				//en degré
	#define EXEMPLE_AX12_DEPLOYED_POS				163				//en degré

//Position initiale de l'AX12
	#define EXEMPLE_AX12_INIT_POS        EXEMPLE_AX12_IDLE_POS		/** Position initiale de l'AX12. Doit être un define du type EXEMPLE_AX12_* */

//Extremum valeur
	#define EXEMPLE_AX12_MAX_VALUE		300
	#define EXEMPLE_AX12_MIN_VALUE		0

#endif

#endif	/* EXEMPLE_CONFIG_H */


