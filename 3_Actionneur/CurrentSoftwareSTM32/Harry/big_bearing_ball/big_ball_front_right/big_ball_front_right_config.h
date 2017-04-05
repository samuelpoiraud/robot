/*  Club Robot ESEO 2016 - 2017
 *
 *	Fichier : BIG_BALL_FRONT_RIGHT_ax12_config.h
 *	Package : Carte actionneur
 *	Description : Gestion de l'actionneur AX12 BIG_BALL_FRONT_RIGHT
 *  Auteur :
 *  Version 2017
 *  Robot : BIG
 */


#ifndef BIG_BALL_FRONT_RIGHT_CONFIG_H
#define	BIG_BALL_FRONT_RIGHT_CONFIG_H

//Config
    #define BIG_BALL_FRONT_RIGHT_AX12_ASSER_TIMEOUT				2000	// Si l'ax12 n'atteint pas la position demandée avant ce temps, on considère qu'il y a un problème. Temps en ms.
    #define BIG_BALL_FRONT_RIGHT_AX12_ASSER_POS_EPSILON			6		// Lorsque l'ax12 atteint sa position avec +/- POS_EPSILON degré d'écart max, on considère qu'il a atteint sa position. Angle en degré.
    #define BIG_BALL_FRONT_RIGHT_AX12_ASSER_POS_LARGE_EPSILON	28		// Si le déplacement de l'ax12 timeout mais que sa position est à +/- POS_LARGE_EPSILON degré d'écart max, on considère qu'il a atteint sa position. Angle en degré.
    #define BIG_BALL_FRONT_RIGHT_AX12_MAX_TORQUE_PERCENT		50		// Couple maximum en pourcentage de l'ax12. Utilisé pour limiter le courant dans le moteur. A mettre a une valeur correcte pour pincer assez fort sans risquer d'endommager l'AX12.
    #define BIG_BALL_FRONT_RIGHT_AX12_MAX_TEMPERATURE			60		// Température maximale en degré
	#define BIG_BALL_FRONT_RIGHT_AX12_SPEED						100		// Vitesse par défaut de l'AX12

//Position
    #define BIG_BALL_FRONT_RIGHT_AX12_IDLE_POS					722
    #define BIG_BALL_FRONT_RIGHT_AX12_UP_POS				    431     //en degré
    #define BIG_BALL_FRONT_RIGHT_AX12_DOWN_POS					722		//en degré

//Position initiale de l'AX12
    #define BIG_BALL_FRONT_RIGHT_AX12_INIT_POS					BIG_BALL_FRONT_RIGHT_AX12_IDLE_POS

//Extremum valeur
    #define BIG_BALL_FRONT_RIGHT_AX12_MIN_VALUE					0
    #define BIG_BALL_FRONT_RIGHT_AX12_MAX_VALUE					1024

#endif	/* BIG_BALL_FRONT_RIGHT_CONFIG_H */


