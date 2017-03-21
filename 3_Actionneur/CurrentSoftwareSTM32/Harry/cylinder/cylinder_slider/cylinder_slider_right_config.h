/*  Club Robot ESEO 2016 - 2017
*
*	Fichier : CYLINDER_SLIDER_RIGHT_rx24_config.c
*	Package : Carte actionneur
*	Description : Gestion de l'actionneur RX24 CYLINDER_SLIDER_RIGHT
*	Auteur :
*	Version 2017
*	Robot : BIG
*/

#ifndef CYLINDER_SLIDER_RIGHT_CONFIG_H
#define	CYLINDER_SLIDER_RIGHT_CONFIG_H

//Config
    #define CYLINDER_SLIDER_RIGHT_RX24_ASSER_TIMEOUT				2000	// Si le RX24 n'atteint pas la position demand�e avant ce temps, on consid�re qu'il y a un probl�me. Temps en ms.
    #define CYLINDER_SLIDER_RIGHT_RX24_ASSER_POS_EPSILON			6		// Lorsque le RX24 atteint sa position avec +/- POS_EPSILON degr� d'�cart max, on consid�re qu'il a atteint sa position. Angle en degr�.
    #define CYLINDER_SLIDER_RIGHT_RX24_ASSER_POS_LARGE_EPSILON	28		// Si le d�placement de le RX24 timeout mais que sa position est � +/- POS_LARGE_EPSILON degr� d'�cart max, on consid�re qu'il a atteint sa position. Angle en degr�.
    #define CYLINDER_SLIDER_RIGHT_RX24_MAX_TORQUE_PERCENT		    50		// Couple maximum en pourcentage de le RX24. Utilis� pour limiter le courant dans le moteur. A mettre a une valeur correcte pour pincer assez fort sans risquer d'endommager le RX24.
    #define CYLINDER_SLIDER_RIGHT_RX24_MAX_TEMPERATURE			60		// Temp�rature maximale en degr�

//Position
    #define CYLINDER_SLIDER_RIGHT_RX24_IDLE_POS								821
    #define CYLINDER_SLIDER_RIGHT_RX24_IN_POS						     	821	//en degr�
    #define CYLINDER_SLIDER_RIGHT_RX24_OUT_POS								333	//en degr�
    #define CYLINDER_SLIDER_RIGHT_RX24_ALMOST_OUT_POS						785	//en degr�
    #define CYLINDER_SLIDER_RIGHT_RX24_ALMOST_OUT_WITH_CYLINDER_POS			383	//en degr�

//Position initiale de l'RX24
    #define CYLINDER_SLIDER_RIGHT_RX24_INIT_POS					CYLINDER_SLIDER_RIGHT_RX24_IDLE_POS

//Extremum valeur
    #define CYLINDER_SLIDER_RIGHT_RX24_MIN_VALUE					0
    #define CYLINDER_SLIDER_RIGHT_RX24_MAX_VALUE					1024

#endif	/* CYLINDER_SLIDER_RIGHT_CONFIG_H */


