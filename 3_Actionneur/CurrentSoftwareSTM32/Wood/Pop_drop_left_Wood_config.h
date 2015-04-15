/*  Club Robot ESEO 2014 - 2015
 *	SMALL
 *
 *	Fichier : Pop_drop_left_Wood_config.c
 *	Package : Carte actionneur
 *	Description : Gestion du bras pour faire tomber les pop corns dans le gobelet gauche
 *  Auteur : Valentin
 *  Version 2015
 *  Robot : SMALL
 */

#ifndef POP_DROP_LEFT_WOOD_CONFIG_H
#define	POP_DROP_LEFT_WOOD_CONFIG_H



//Config
	#define POP_DROP_LEFT_WOOD_AX12_ASSER_TIMEOUT				1000			// Si l'ax12 n'atteint pas la position demandée avant ce temps, on considère qu'il y a un problème. Temps en ms.
	#define POP_DROP_LEFT_WOOD_AX12_ASSER_POS_EPSILON			6				// Lorsque l'ax12 atteint sa position avec +/- POS_EPSILON degré d'écart max, on considère qu'il a atteint sa position. Angle en degré.
	#define POP_DROP_LEFT_WOOD_AX12_ASSER_POS_LARGE_EPSILON		28				// Si le déplacement de l'ax12 timeout mais que sa position est à +/- POS_LARGE_EPSILON degré d'écart max, on considère qu'il a atteint sa position. Angle en degré.
	#define POP_DROP_LEFT_WOOD_AX12_MAX_TORQUE_PERCENT			30				// Couple maximum en pourcentage de l'ax12. Utilisé pour limiter le courant dans le moteur. A mettre a une valeur correcte pour pincer assez fort sans risquer d'endommager l'AX12.
	#define POP_DROP_LEFT_WOOD_AX12_MAX_TEMPERATURE            60              // Température maximale en degré
//Position
	#define POP_DROP_LEFT_WOOD_AX12_CLOSE_POS					375				//en degré
	#define POP_DROP_LEFT_WOOD_AX12_OPEN_POS					556				//en degré

//Position initiale de l'AX12
	#define POP_DROP_LEFT_WOOD_AX12_INIT_POS        POP_DROP_LEFT_WOOD_AX12_CLOSE_POS		/** Position initiale de l'AX12. Doit être un define du type POP_DROP_LEFT_WOOD_AX12_* */

//Extremum valeur
	#define POP_DROP_LEFT_WOOD_AX12_MIN_VALUE		0
	#define POP_DROP_LEFT_WOOD_AX12_MAX_VALUE		1024



#endif	/* POP_DROP_LEFT_WOOD_CONFIG_H */


