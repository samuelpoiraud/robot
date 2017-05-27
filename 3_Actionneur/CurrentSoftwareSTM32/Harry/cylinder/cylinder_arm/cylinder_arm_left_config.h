/*  Club Robot ESEO 2016 - 2017
*
*	Fichier : cylinder_arm_left_config.c
*	Package : Carte actionneur
*	Description : Gestion de l'actionneur RX24 CYLINDER_ARM_LEFT
*	Auteur :
*	Version 2017
*	Robot : BIG
*/

#ifndef CYLINDER_ARM_LEFT_CONFIG_H
#define	CYLINDER_ARM_LEFT_CONFIG_H

//Config
    #define CYLINDER_ARM_LEFT_RX24_ASSER_TIMEOUT				2000	// Si le RX24 n'atteint pas la position demand�e avant ce temps, on consid�re qu'il y a un probl�me. Temps en ms.
    #define CYLINDER_ARM_LEFT_RX24_ASSER_POS_EPSILON			6		// Lorsque le RX24 atteint sa position avec +/- POS_EPSILON degr� d'�cart max, on consid�re qu'il a atteint sa position. Angle en degr�.
    #define CYLINDER_ARM_LEFT_RX24_ASSER_POS_LARGE_EPSILON		28		// Si le d�placement de le RX24 timeout mais que sa position est � +/- POS_LARGE_EPSILON degr� d'�cart max, on consid�re qu'il a atteint sa position. Angle en degr�.
    #define CYLINDER_ARM_LEFT_RX24_MAX_TORQUE_PERCENT		    90		// Couple maximum en pourcentage de le RX24. Utilis� pour limiter le courant dans le moteur. A mettre a une valeur correcte pour pincer assez fort sans risquer d'endommager le RX24.
    #define CYLINDER_ARM_LEFT_RX24_MAX_TEMPERATURE				60		// Temp�rature maximale en degr�
	#define CYLINDER_ARM_LEFT_RX24_SPEED						15		// Vitesse par d�faut du RX24

//Position
    #define CYLINDER_ARM_LEFT_RX24_IDLE_POS						517
    #define CYLINDER_ARM_LEFT_RX24_IN_POS			     		509	//en degr� (-15 par rapport � IDLE_POS)
    #define CYLINDER_ARM_LEFT_RX24_PREPARE_TO_TAKE_POS			410	//en degr�
    #define CYLINDER_ARM_LEFT_RX24_TAKE_POS			     		453	//en degr�
    #define CYLINDER_ARM_LEFT_RX24_DISPOSE_POS			     	414	//en degr� (+20 par rapport � OUT_POS)
    #define CYLINDER_ARM_LEFT_RX24_OUT_POS						384	//en degr� (-130 par rapport � IN_POS) - 10 (car � gauche on a plus de marge pour le bras)

//Position initiale de l'RX24
    #define CYLINDER_ARM_LEFT_RX24_INIT_POS						CYLINDER_ARM_LEFT_RX24_IDLE_POS

//Extremum valeur
    #define CYLINDER_ARM_LEFT_RX24_MIN_VALUE					0
    #define CYLINDER_ARM_LEFT_RX24_MAX_VALUE					1024

#endif	/* CYLINDER_ARM_LEFT_CONFIG_H */


