/*  Club Robot ESEO 2015 - 2016
 *	BIG
 *
 *	Fichier : cone_dune_config.h
 *	Package : Carte actionneur
 *	Description : Gestion de l'actionneur RX24 CONE_DUNE
 *  Auteur : Valentin
 *  Version 2016
 *  Robot : BIG
 */

#ifndef CONE_DUNE_CONFIG_H
#define	CONE_DUNE_CONFIG_H

//Config
	#define CONE_DUNE_RX24_ASSER_TIMEOUT				600	// Si le RX24 n'atteint pas la position demandée avant ce temps, on considère qu'il y a un problème. Temps en ms.
	#define CONE_DUNE_RX24_ASSER_POS_EPSILON			6	// Lorsque le RX24 atteint sa position avec +/- POS_EPSILON degré d'écart max, on considère qu'il a atteint sa position. Angle en degré.
	#define CONE_DUNE_RX24_ASSER_POS_LARGE_EPSILON	    28	// Si le déplacement de le RX24 timeout mais que sa position est à +/- POS_LARGE_EPSILON degré d'écart max, on considère qu'il a atteint sa position. Angle en degré.
	#define CONE_DUNE_RX24_MAX_TORQUE_PERCENT		    100	// Couple maximum en pourcentage de le RX24. Utilisé pour limiter le courant dans le moteur. A mettre a une valeur correcte pour pincer assez fort sans risquer d'endommager le RX24.
	#define CONE_DUNE_RX24_MAX_TEMPERATURE			    65	// Température maximale en degré

//Position
	#define CONE_DUNE_RX24_IDLE_POS						615
	#define CONE_DUNE_RX24_LOCK_POS						399	//en degré
	#define CONE_DUNE_RX24_UNLOCK_POS					615	//en degré

//Position initiale de l'RX24
	#define CONE_DUNE_RX24_INIT_POS					CONE_DUNE_RX24_IDLE_POS

//Extremum valeur
	#define CONE_DUNE_RX24_MIN_VALUE				0
	#define CONE_DUNE_RX24_MAX_VALUE				1024

#endif	/* CONE_DUNE_CONFIG_H */


