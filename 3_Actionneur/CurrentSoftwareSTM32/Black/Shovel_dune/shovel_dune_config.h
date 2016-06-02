/*  Club Robot ESEO 2015 - 2016
 *	BIG
 *
 *	Fichier : shovel_dune_config.h
 *	Package : Carte actionneur
 *	Description : Gestion de l'actionneur RX24 SHOVEL_DUNE
 *  Auteur : Arnaud
 *  Version 2016
 *  Robot : BIG
 */

#ifndef SHOVEL_DUNE_CONFIG_H
#define	SHOVEL_DUNE_CONFIG_H

//Config
	#define SHOVEL_DUNE_RX24_ASSER_TIMEOUT					600		// Si le RX24 n'atteint pas la position demandée avant ce temps, on considère qu'il y a un problème. Temps en ms.
	#define SHOVEL_DUNE_RX24_ASSER_POS_EPSILON				6		// Lorsque le RX24 atteint sa position avec +/- POS_EPSILON degré d'écart max, on considère qu'il a atteint sa position. Angle en degré.
	#define SHOVEL_DUNE_RX24_ASSER_POS_LARGE_EPSILON	    28		// Si le déplacement de le RX24 timeout mais que sa position est à +/- POS_LARGE_EPSILON degré d'écart max, on considère qu'il a atteint sa position. Angle en degré.
	#define SHOVEL_DUNE_RX24_MAX_TORQUE_PERCENT		        99		// Couple maximum en pourcentage de le RX24. Utilisé pour limiter le courant dans le moteur. A mettre a une valeur correcte pour pincer assez fort sans risquer d'endommager le RX24.
	#define SHOVEL_DUNE_RX24_MAX_TEMPERATURE			    65		// Température maximale en degré
	#define SHOVEL_DUNE_RX24_MAX_SPEED                      99      // Vitesse maximale en pourcent

//Position
	#define SHOVEL_DUNE_L_RX24_IDLE_POS					    635
	#define SHOVEL_DUNE_L_RX24_TAKE_POS			     	    294
	#define SHOVEL_DUNE_L_RX24_STORE_POS				    326
	#define SHOVEL_DUNE_L_RX24_RESCUE_POS				    326

	#define SHOVEL_DUNE_R_RX24_IDLE_POS					    529
	#define SHOVEL_DUNE_R_RX24_TAKE_POS			     	    871
	#define SHOVEL_DUNE_R_RX24_STORE_POS				    835
	#define SHOVEL_DUNE_R_RX24_RESCUE_POS					835

//Position initiale de l'RX24
	#define SHOVEL_DUNE_L_RX24_INIT_POS						SHOVEL_DUNE_L_RX24_IDLE_POS
	#define SHOVEL_DUNE_R_RX24_INIT_POS						SHOVEL_DUNE_R_RX24_IDLE_POS

//Extremum valeur
	#define SHOVEL_DUNE_RX24_MIN_VALUE						0
	#define SHOVEL_DUNE_RX24_MAX_VALUE						1024

//Compliance Slope
	#define SHOVEL_DUNE_RX24_COMPLIANCE_SLOPE_VALUE			16
	#define SHOVEL_DUNE_RX24_COMPLIANCE_MARGIN_VALUE		1

#endif	/* SHOVEL_DUNE_CONFIG_H */


