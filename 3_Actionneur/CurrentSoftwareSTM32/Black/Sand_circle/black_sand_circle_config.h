/*  Club Robot ESEO 2015 - 2016
 *	BIG
 *
 *	Fichier : black_sand_circle_config.h
 *	Package : Carte actionneur
 *	Description : Gestion de l'actionneur RX24 BLACK_SAND_CIRCLE
 *  Auteur : Valentin
 *  Version 2016
 *  Robot : BIG
 */

#ifndef BLACK_SAND_CIRCLE_CONFIG_H
#define	BLACK_SAND_CIRCLE_CONFIG_H

//Config
	#define BLACK_SAND_CIRCLE_RX24_ASSER_TIMEOUT				600		// Si le RX24 n'atteint pas la position demand�e avant ce temps, on consid�re qu'il y a un probl�me. Temps en ms.
	#define BLACK_SAND_CIRCLE_RX24_ASSER_POS_EPSILON			6		// Lorsque le RX24 atteint sa position avec +/- POS_EPSILON degr� d'�cart max, on consid�re qu'il a atteint sa position. Angle en degr�.
	#define BLACK_SAND_CIRCLE_RX24_ASSER_POS_LARGE_EPSILON		28		// Si le d�placement de le RX24 timeout mais que sa position est � +/- POS_LARGE_EPSILON degr� d'�cart max, on consid�re qu'il a atteint sa position. Angle en degr�.
	#define BLACK_SAND_CIRCLE_RX24_MAX_TORQUE_PERCENT			100		// Couple maximum en pourcentage de le RX24. Utilis� pour limiter le courant dans le moteur. A mettre a une valeur correcte pour pincer assez fort sans risquer d'endommager le RX24.
	#define BLACK_SAND_CIRCLE_RX24_MAX_TEMPERATURE				65		// Temp�rature maximale en degr�

//Position
	#define BLACK_SAND_CIRCLE_RX24_IDLE_POS						870
	#define BLACK_SAND_CIRCLE_RX24_LOCK_POS			     	    490
	#define BLACK_SAND_CIRCLE_RX24_UNLOCK_POS					870

//Position initiale de l'RX24
	#define BLACK_SAND_CIRCLE_RX24_INIT_POS						BLACK_SAND_CIRCLE_RX24_IDLE_POS

//Extremum valeur
	#define BLACK_SAND_CIRCLE_RX24_MIN_VALUE					0
	#define BLACK_SAND_CIRCLE_RX24_MAX_VALUE					1024

#endif	/* BLACK_SAND_CIRCLE_CONFIG_H */


