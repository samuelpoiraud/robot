/*  Club Robot ESEO 2015 - 2016
 *	BIG
 *
 *	Fichier : ORE_ROLLER_ARM_config.h
 *	Package : Carte actionneur
 *	Description : Gestion de l'actionneur RX24 ORE_ROLLER_ARM
 *  Auteur : Corentin
 *  Version 2017
 *  Robot : BIG
 */

#ifndef ORE_ROLLER_ARM_CONFIG_H
#define	ORE_ROLLER_ARM_CONFIG_H

//Config
    #define ORE_ROLLER_ARM_RX24_ASSER_TIMEOUT					600		// Si le RX24 n'atteint pas la position demandée avant ce temps, on considère qu'il y a un problème. Temps en ms.
    #define ORE_ROLLER_ARM_RX24_ASSER_POS_EPSILON				6		// Lorsque le RX24 atteint sa position avec +/- POS_EPSILON degré d'écart max, on considère qu'il a atteint sa position. Angle en degré.
    #define ORE_ROLLER_ARM_RX24_ASSER_POS_LARGE_EPSILON	    28		// Si le déplacement de le RX24 timeout mais que sa position est à +/- POS_LARGE_EPSILON degré d'écart max, on considère qu'il a atteint sa position. Angle en degré.
    #define ORE_ROLLER_ARM_RX24_MAX_TORQUE_PERCENT		        50		// Couple maximum en pourcentage de le RX24. Utilisé pour limiter le courant dans le moteur. A mettre a une valeur correcte pour pincer assez fort sans risquer d'endommager le RX24.
    #define ORE_ROLLER_ARM_RX24_MAX_TEMPERATURE			    65		// Température maximale en degré
    #define ORE_ROLLER_ARM_RX24_MAX_SPEED                      50      // Vitesse maximale en pourcent

//Position
    #define ORE_ROLLER_ARM_L_RX24_IDLE_POS					    743
    #define ORE_ROLLER_ARM_L_RX24_OUT_POS			     	    388
    #define ORE_ROLLER_ARM_L_RX24_UNLOCK_POS				    743
    #define ORE_ROLLER_ARM_L_RX24_MID_POS				        (743 - 180)
    #define ORE_ROLLER_ARM_L_RX24_RESCUE_POS					436
    #define ORE_ROLLER_ARM_L_RX24_CHECK_POS					360

    #define ORE_ROLLER_ARM_R_RX24_IDLE_POS					    277
    #define ORE_ROLLER_ARM_R_RX24_OUT_POS			     	    630
    #define ORE_ROLLER_ARM_R_RX24_UNLOCK_POS				    277
    #define ORE_ROLLER_ARM_R_RX24_MID_POS						(277 + 180)
    #define ORE_ROLLER_ARM_R_RX24_RESCUE_POS					582
    #define ORE_ROLLER_ARM_R_RX24_CHECK_POS					658

//Position initiale de l'RX24
    #define ORE_ROLLER_ARM_L_RX24_INIT_POS					ORE_ROLLER_ARM_L_RX24_IDLE_POS
    #define ORE_ROLLER_ARM_R_RX24_INIT_POS					ORE_ROLLER_ARM_R_RX24_IDLE_POS

//Extremum valeur
    #define ORE_ROLLER_ARM_RX24_MIN_VALUE					0
    #define ORE_ROLLER_ARM_RX24_MAX_VALUE					1024

#endif	/* ORE_ROLLER_ARM_CONFIG_H */


