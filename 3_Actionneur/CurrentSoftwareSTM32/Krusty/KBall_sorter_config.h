/*  Club Robot ESEO 2012 - 2013
 *	Krusty
 *
 *  Fichier : KPlate_config.h
 *  Package : Actionneur
 *  Description : Configuration du s�quenceur de cerise
 *  Auteur : Alexis
 *  Version 20130505
 *  Robot : Krusty
 */

#ifndef KBALL_SORTER_CONFIG_H
#define	KBALL_SORTER_CONFIG_H

	#define BALLSORTER_AX12_MAX_TORQUE_PERCENT      100
	#define BALLSORTER_AX12_ASSER_POS_EPSILON       2      //en degr�
	#define BALLSORTER_AX12_ASSER_POS_LARGE_EPSILON 10     //en degr�, pour savoir si on a atteint la position ou pas lors d'un timeout (il faut �tre moins pr�cis ici, il n'y a pas de timing en jeu)
	#define BALLSORTER_AX12_ASSER_TIMEOUT           10     //unit�: 0.1s  (une valeur de 20 correspond � 0.2s soit 200ms)
	#define BALLSORTER_AX12_EJECT_CHERRY_POS        132
	#define BALLSORTER_AX12_NEXT_CHERRY_POS         250
	#define BALLSORTER_AX12_DETECT_CHERRY_POS       183
	#define BALLSORTER_DETECT_TIME                  2      //Attend x * 0.1 s apr�s que l'AX12 soit rendu � sa position avant de capter la couleur de la cerise (unit�: 0.1s  (une valeur de 20 correspond � 0.2s soit 200ms))
	#define BALLSORTER_USE_SICK_SENSOR

#endif	/* KBALL_SORTER_CONFIG_H */
