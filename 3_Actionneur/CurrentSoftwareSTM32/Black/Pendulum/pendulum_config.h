/*  Club Robot ESEO 2015 - 2016
 *	BIG
 *
 *	Fichier : pendulum_config.h
 *	Package : Carte actionneur
 *	Description : Gestion de l'actionneur RX24 PENDULUM
 *  Auteur : Valentin
 *  Version 2016
 *  Robot : BIG
 */

#ifndef PENDULUM_CONFIG_H
#define	PENDULUM_CONFIG_H

//Config
	#define PENDULUM_RX24_ASSER_TIMEOUT				600		// Si le RX24 n'atteint pas la position demandée avant ce temps, on considère qu'il y a un problème. Temps en ms.
	#define PENDULUM_RX24_ASSER_POS_EPSILON			6		// Lorsque le RX24 atteint sa position avec +/- POS_EPSILON degré d'écart max, on considère qu'il a atteint sa position. Angle en degré.
	#define PENDULUM_RX24_ASSER_POS_LARGE_EPSILON	28		// Si le déplacement de le RX24 timeout mais que sa position est à +/- POS_LARGE_EPSILON degré d'écart max, on considère qu'il a atteint sa position. Angle en degré.
	#define PENDULUM_RX24_MAX_TORQUE_PERCENT		100		// Couple maximum en pourcentage de le RX24. Utilisé pour limiter le courant dans le moteur. A mettre a une valeur correcte pour pincer assez fort sans risquer d'endommager le RX24.
	#define PENDULUM_RX24_MAX_TEMPERATURE			65		// Température maximale en degré

//Position
	#define PENDULUM_RX24_IDLE_POS					300
	#define PENDULUM_RX24_CLOSE_POS			     	512
	#define PENDULUM_RX24_OPEN_POS					700

//Position initiale de l'RX24
	#define PENDULUM_RX24_INIT_POS					PENDULUM_RX24_IDLE_POS

//Extremum valeur
	#define PENDULUM_RX24_MIN_VALUE					0
	#define PENDULUM_RX24_MAX_VALUE					1024

#endif	/* PENDULUM_CONFIG_H */


