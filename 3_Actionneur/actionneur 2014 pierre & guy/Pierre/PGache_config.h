/*  Club Robot ESEO 2013 - 2014
 *	BIG
 *
 *  Fichier : PGache_config.h
 *  Package : Actionneur
 *  Description : Configuration du proto_gache
 *  Auteur : Arnaud
 *  Version 20130505
 *  Robot : BIG
 */


#ifndef PGACHE_CONFIG_H
#define	PGACHE_CONFIG_H

#include "../clock.h"

/** Si l'ax12 n'atteint pas la position demandée avant ce temps, on considère qu'il y a un problème. Temps en centaine de ms. */
	#define GACHE_AX12_ASSER_TIMEOUT       10      //unité: 0.1s  (une valeur de 20 correspond à 0.2s soit 200ms)
/** Lorsque l'ax12 atteint sa position avec +/- PLATE_PLIER_AX12_ASSER_POS_EPSILON degré d'écart max, on considère qu'il a atteint sa position. Angle en degré. */
	#define GACHE_AX12_ASSER_POS_EPSILON   2     //en degré
/** Position en degré de l'ax12 lorsque GACHE pas lancé  */
	#define GACHE_AX12_IDLE_POS            70    //en degré
/** Position en degré de l'ax12 lorsque GACHE lancé */
	#define GACHE_AX12_LAUNCHED_POS          120    //en degré
/** Tension maximale en pourcentage de l'ax12. Utilisé pour limiter le courant dans le moteur  */
	#define GACHE_AX12_MAX_TORQUE_PERCENT  40     //A mettre a une valeur correcte pour pincer assez fort sans risquer d'endommager l'AX12.
/** Position initiale de l'AX12. Doit être un define du type PLATE_PLIER_AX12_* */
	#define GACHE_AX12_INIT_POS        GACHE_AX12_IDLE_POS

#define GACHE_AX12_MAX_VALUE	130
#define GACHE_AX12_MIN_VALUE	70

#endif	/* PGACHE_CONFIG_H */


