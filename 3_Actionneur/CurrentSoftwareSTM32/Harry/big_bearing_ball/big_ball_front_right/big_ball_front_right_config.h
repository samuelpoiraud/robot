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
    #define BIG_BALL_FRONT_RIGHT_AX12_ASSER_TIMEOUT				2000	// Si l'ax12 n'atteint pas la position demand�e avant ce temps, on consid�re qu'il y a un probl�me. Temps en ms.
    #define BIG_BALL_FRONT_RIGHT_AX12_ASSER_POS_EPSILON			6		// Lorsque l'ax12 atteint sa position avec +/- POS_EPSILON degr� d'�cart max, on consid�re qu'il a atteint sa position. Angle en degr�.
    #define BIG_BALL_FRONT_RIGHT_AX12_ASSER_POS_LARGE_EPSILON	28		// Si le d�placement de l'ax12 timeout mais que sa position est � +/- POS_LARGE_EPSILON degr� d'�cart max, on consid�re qu'il a atteint sa position. Angle en degr�.
    #define BIG_BALL_FRONT_RIGHT_AX12_MAX_TORQUE_PERCENT		50		// Couple maximum en pourcentage de l'ax12. Utilis� pour limiter le courant dans le moteur. A mettre a une valeur correcte pour pincer assez fort sans risquer d'endommager l'AX12.
    #define BIG_BALL_FRONT_RIGHT_AX12_MAX_TEMPERATURE			60		// Temp�rature maximale en degr�
	#define BIG_BALL_FRONT_RIGHT_AX12_SPEED						100		// Vitesse par d�faut de l'AX12

//Position
    #define BIG_BALL_FRONT_RIGHT_AX12_IDLE_POS					722
    #define BIG_BALL_FRONT_RIGHT_AX12_UP_POS				    431     //en degr�
    #define BIG_BALL_FRONT_RIGHT_AX12_DOWN_POS					722		//en degr�

//Position initiale de l'AX12
    #define BIG_BALL_FRONT_RIGHT_AX12_INIT_POS					BIG_BALL_FRONT_RIGHT_AX12_IDLE_POS

//Extremum valeur
    #define BIG_BALL_FRONT_RIGHT_AX12_MIN_VALUE					0
    #define BIG_BALL_FRONT_RIGHT_AX12_MAX_VALUE					1024

#endif	/* BIG_BALL_FRONT_RIGHT_CONFIG_H */


