/*  Club Robot ESEO 2016 - 2017
*
*	Fichier : small_cylinder_arm_config.c
*	Package : Carte actionneur
*	Description : Gestion de l'actionneur RX24 SMALL_CYLINDER_ARM
*	Auteur :
*	Version 2017
*	Robot : BIG
*/

#ifndef SMALL_CYLINDER_ARM_CONFIG_H
#define	SMALL_CYLINDER_ARM_CONFIG_H

//Config
    #define SMALL_CYLINDER_ARM_RX24_ASSER_TIMEOUT				2000	// Si le RX24 n'atteint pas la position demandée avant ce temps, on considère qu'il y a un problème. Temps en ms.
    #define SMALL_CYLINDER_ARM_RX24_ASSER_POS_EPSILON			6		// Lorsque le RX24 atteint sa position avec +/- POS_EPSILON degré d'écart max, on considère qu'il a atteint sa position. Angle en degré.
    #define SMALL_CYLINDER_ARM_RX24_ASSER_POS_LARGE_EPSILON		28		// Si le déplacement de le RX24 timeout mais que sa position est à +/- POS_LARGE_EPSILON degré d'écart max, on considère qu'il a atteint sa position. Angle en degré.
    #define SMALL_CYLINDER_ARM_RX24_MAX_TORQUE_PERCENT		    50		// Couple maximum en pourcentage de le RX24. Utilisé pour limiter le courant dans le moteur. A mettre a une valeur correcte pour pincer assez fort sans risquer d'endommager le RX24.
    #define SMALL_CYLINDER_ARM_RX24_MAX_TEMPERATURE				60		// Température maximale en degré
	#define SMALL_CYLINDER_ARM_RX24_SPEED						15		// Vitesse par défaut du RX24

//Position
    #define SMALL_CYLINDER_ARM_RX24_IDLE_POS					542
    #define SMALL_CYLINDER_ARM_RX24_IN_POS			     		538	//en degré
    #define SMALL_CYLINDER_ARM_RX24_PREPARE_TO_TAKE_POS			410	//en degré
    #define SMALL_CYLINDER_ARM_RX24_TAKE_POS			     	453	//en degré
    #define SMALL_CYLINDER_ARM_RX24_DISPOSE_POS			     	436	//en degré
    #define SMALL_CYLINDER_ARM_RX24_OUT_POS						361	//en degré
    #define SMALL_CYLINDER_ARM_RX24_PROTECT_FALL_POS			488	//en degré


//Position initiale de l'RX24
    #define SMALL_CYLINDER_ARM_RX24_INIT_POS					SMALL_CYLINDER_ARM_RX24_IDLE_POS

//Extremum valeur
    #define SMALL_CYLINDER_ARM_RX24_MIN_VALUE					0
    #define SMALL_CYLINDER_ARM_RX24_MAX_VALUE					1024

#endif	/* SMALL_CYLINDER_ARM_CONFIG_H */


