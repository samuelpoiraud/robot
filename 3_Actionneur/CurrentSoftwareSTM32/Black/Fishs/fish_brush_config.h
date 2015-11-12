/*  Club Robot ESEO 2013 - 2014
 *	SMALL
 *
 *	Fichier : FISH_BRUSH_config.c
 *	Package : Carte actionneur
 *	Description : Gestion de l'actionneur d'FISH_BRUSH
 *  Auteur : Arnaud
 *  Version 20130219
 *  Robot : SMALL
 */

#ifndef FISH_BRUSH_CONFIG_H
#define	FISH_BRUSH_CONFIG_H



//Config
	#define FISH_BRUSH_AX12_ASSER_TIMEOUT				1000			// Si l'ax12 n'atteint pas la position demand�e avant ce temps, on consid�re qu'il y a un probl�me. Temps en ms.
	#define FISH_BRUSH_AX12_ASSER_POS_EPSILON			2				// Lorsque l'ax12 atteint sa position avec +/- POS_EPSILON degr� d'�cart max, on consid�re qu'il a atteint sa position. Angle en degr�.
	#define FISH_BRUSH_AX12_ASSER_POS_LARGE_EPSILON	10				// Si le d�placement de l'ax12 timeout mais que sa position est � +/- POS_LARGE_EPSILON degr� d'�cart max, on consid�re qu'il a atteint sa position. Angle en degr�.
	#define FISH_BRUSH_AX12_MAX_TORQUE_PERCENT		100				// Couple maximum en pourcentage de l'ax12. Utilis� pour limiter le courant dans le moteur. A mettre a une valeur correcte pour pincer assez fort sans risquer d'endommager l'AX12.

//Position
	#define FISH_BRUSH_AX12_IDLE_POS					0				//en degr�
	#define FISH_BRUSH_AX12_OPEN_POS					90				//en degr�
	#define FISH_BRUSH_AX12_CLOSED_POS					1				//en degr�

//Position initiale de l'AX12
	#define FISH_BRUSH_AX12_INIT_POS        FISH_BRUSH_AX12_IDLE_POS		/** Position initiale de l'AX12. Doit �tre un define du type FISH_BRUSH_AX12_* */

//Extremum valeur
	#define FISH_BRUSH_AX12_MAX_VALUE		300
	#define FISH_BRUSH_AX12_MIN_VALUE		0



#endif	/* FISH_BRUSH_CONFIG_H */


