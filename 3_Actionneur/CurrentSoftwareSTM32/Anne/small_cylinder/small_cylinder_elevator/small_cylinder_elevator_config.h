/*  Club Robot ESEO 2016 - 2017
*
*	Fichier : small_cylinder_elevator_config.c
*	Package : Carte actionneur
*	Description : Gestion de l'actionneur RX24 SMALL_CYLINDER_ELEVATOR
*	Auteur :
*	Version 2017
*	Robot : BIG
*/

#ifndef SMALL_CYLINDER_ELEVATOR_CONFIG_H
#define	SMALL_CYLINDER_ELEVATOR_CONFIG_H

//Config
    #define SMALL_CYLINDER_ELEVATOR_RX24_ASSER_TIMEOUT				2000	// Si le RX24 n'atteint pas la position demand�e avant ce temps, on consid�re qu'il y a un probl�me. Temps en ms.
    #define SMALL_CYLINDER_ELEVATOR_RX24_ASSER_POS_EPSILON			6		// Lorsque le RX24 atteint sa position avec +/- POS_EPSILON degr� d'�cart max, on consid�re qu'il a atteint sa position. Angle en degr�.
    #define SMALL_CYLINDER_ELEVATOR_RX24_ASSER_POS_LARGE_EPSILON	28		// Si le d�placement de le RX24 timeout mais que sa position est � +/- POS_LARGE_EPSILON degr� d'�cart max, on consid�re qu'il a atteint sa position. Angle en degr�.
    #define SMALL_CYLINDER_ELEVATOR_RX24_MAX_TORQUE_PERCENT		    50		// Couple maximum en pourcentage de le RX24. Utilis� pour limiter le courant dans le moteur. A mettre a une valeur correcte pour pincer assez fort sans risquer d'endommager le RX24.
    #define SMALL_CYLINDER_ELEVATOR_RX24_MAX_TEMPERATURE			60		// Temp�rature maximale en degr�
	#define SMALL_CYLINDER_ELEVATOR_RX24_SPEED						30		// Vitesse par d�faut du RX24

//Position
    #define SMALL_CYLINDER_ELEVATOR_RX24_IDLE_POS					502
    #define SMALL_CYLINDER_ELEVATOR_RX24_TOP_POS			     	814 //806	//en degr�
    #define SMALL_CYLINDER_ELEVATOR_RX24_WAIT_FOR_SLOPE		     	744	//en degr�
    #define SMALL_CYLINDER_ELEVATOR_RX24_BOTTOM_POS					502	//en degr�
    #define SMALL_CYLINDER_ELEVATOR_RX24_LOCK_WITH_CYLINDER_POS		620 //en degr�

	#define SMALL_CYLINDER_ELEVATOR_RX24_WARNER_POS					700

//Position initiale de l'RX24
    #define SMALL_CYLINDER_ELEVATOR_RX24_INIT_POS					SMALL_CYLINDER_ELEVATOR_RX24_IDLE_POS

//Extremum valeur
    #define SMALL_CYLINDER_ELEVATOR_RX24_MIN_VALUE					0
    #define SMALL_CYLINDER_ELEVATOR_RX24_MAX_VALUE					1024

#endif	/* SMALL_CYLINDER_ELEVATOR_CONFIG_H */


