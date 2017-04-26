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
    #define ORE_ROLLER_ARM_RX24_ASSER_TIMEOUT					600		// Si le RX24 n'atteint pas la position demand�e avant ce temps, on consid�re qu'il y a un probl�me. Temps en ms.
    #define ORE_ROLLER_ARM_RX24_ASSER_POS_EPSILON				6		// Lorsque le RX24 atteint sa position avec +/- POS_EPSILON degr� d'�cart max, on consid�re qu'il a atteint sa position. Angle en degr�.
    #define ORE_ROLLER_ARM_RX24_ASSER_POS_LARGE_EPSILON	    	28		// Si le d�placement de le RX24 timeout mais que sa position est � +/- POS_LARGE_EPSILON degr� d'�cart max, on consid�re qu'il a atteint sa position. Angle en degr�.
    #define ORE_ROLLER_ARM_RX24_MAX_TORQUE_PERCENT		        50		// Couple maximum en pourcentage de le RX24. Utilis� pour limiter le courant dans le moteur. A mettre a une valeur correcte pour pincer assez fort sans risquer d'endommager le RX24.
    #define ORE_ROLLER_ARM_RX24_MAX_TEMPERATURE			    	65		// Temp�rature maximale en degr�
    #define ORE_ROLLER_ARM_RX24_MAX_SPEED                      	50      // Vitesse maximale en pourcent
	#define ORE_ROLLER_ARM_RX24_SPEED							100		// Vitesse par d�faut du RX24

//Position
    #define ORE_ROLLER_ARM_L_RX24_IDLE_POS					    984
    #define ORE_ROLLER_ARM_L_RX24_OUT_POS			     	    650
    #define ORE_ROLLER_ARM_L_RX24_IN_POS				    	984
    #define ORE_ROLLER_ARM_L_RX24_DEPOSE_POS				    785


    #define ORE_ROLLER_ARM_R_RX24_IDLE_POS					    654
    #define ORE_ROLLER_ARM_R_RX24_OUT_POS			     	    983
    #define ORE_ROLLER_ARM_R_RX24_IN_POS				    	654
    #define ORE_ROLLER_ARM_R_RX24_DEPOSE_POS				    857


//Position initiale de l'RX24
    #define ORE_ROLLER_ARM_L_RX24_INIT_POS					ORE_ROLLER_ARM_L_RX24_IDLE_POS
    #define ORE_ROLLER_ARM_R_RX24_INIT_POS					ORE_ROLLER_ARM_R_RX24_IDLE_POS

//Extremum valeur
    #define ORE_ROLLER_ARM_RX24_MIN_VALUE					0
    #define ORE_ROLLER_ARM_RX24_MAX_VALUE					1024

#endif	/* ORE_ROLLER_ARM_CONFIG_H */


