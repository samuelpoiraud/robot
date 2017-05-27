/*  Club Robot ESEO 2016 - 2017
*
*	Fichier : cylinder_arm_right_config.c
*	Package : Carte actionneur
*	Description : Gestion de l'actionneur RX24 CYLINDER_ARM_RIGHT
*	Auteur :
*	Version 2017
*	Robot : BIG
*/

#ifndef CYLINDER_ARM_RIGHT_CONFIG_H
#define	CYLINDER_ARM_RIGHT_CONFIG_H

//Config
    #define CYLINDER_ARM_RIGHT_RX24_ASSER_TIMEOUT				2000	// Si le RX24 n'atteint pas la position demandée avant ce temps, on considère qu'il y a un problème. Temps en ms.
    #define CYLINDER_ARM_RIGHT_RX24_ASSER_POS_EPSILON			6		// Lorsque le RX24 atteint sa position avec +/- POS_EPSILON degré d'écart max, on considère qu'il a atteint sa position. Angle en degré.
    #define CYLINDER_ARM_RIGHT_RX24_ASSER_POS_LARGE_EPSILON		28		// Si le déplacement de le RX24 timeout mais que sa position est à +/- POS_LARGE_EPSILON degré d'écart max, on considère qu'il a atteint sa position. Angle en degré.
    #define CYLINDER_ARM_RIGHT_RX24_MAX_TORQUE_PERCENT		    90		// Couple maximum en pourcentage de le RX24. Utilisé pour limiter le courant dans le moteur. A mettre a une valeur correcte pour pincer assez fort sans risquer d'endommager le RX24.
    #define CYLINDER_ARM_RIGHT_RX24_MAX_TEMPERATURE				60		// Température maximale en degré
	#define CYLINDER_ARM_RIGHT_RX24_SPEED						15		// Vitesse par défaut du RX24

//Position
    #define CYLINDER_ARM_RIGHT_RX24_IDLE_POS					388
    #define CYLINDER_ARM_RIGHT_RX24_IN_POS			     		401	//en degré (+15 par rapport à IDLE_POS)
	#define CYLINDER_ARM_RIGHT_RX24_PREPARE_TO_TAKE_POS			488	//en degré
	#define CYLINDER_ARM_RIGHT_RX24_TAKE_POS			     	457	//en degré
	#define CYLINDER_ARM_RIGHT_RX24_DISPOSE_POS			     	495	//en degré (-20 par rapport à OUT_POS)
    #define CYLINDER_ARM_RIGHT_RX24_OUT_POS						515	//en degré (+130 par rapport à IN_POS)

//Position initiale de l'RX24
    #define CYLINDER_ARM_RIGHT_RX24_INIT_POS					CYLINDER_ARM_RIGHT_RX24_IDLE_POS

//Extremum valeur
    #define CYLINDER_ARM_RIGHT_RX24_MIN_VALUE					0
    #define CYLINDER_ARM_RIGHT_RX24_MAX_VALUE					1024

#endif	/* CYLINDER_ARM_RIGHT_CONFIG_H */


