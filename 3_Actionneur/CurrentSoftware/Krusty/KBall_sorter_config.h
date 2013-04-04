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

	#define BALLSORTER_AX12_MAX_TORQUE_PERCENT   100
	#define BALLSORTER_AX12_ASSER_POS_EPSILON    1      //en degr�
	#define BALLSORTER_AX12_ASSER_TIMEOUT        30     //unit�: 0.1s  (une valeur de 20 correspond � 0.2s soit 200ms)
	#define BALLSORTER_AX12_EJECT_CHERRY_POS     132      //TODO: changer ces valeurs !!! (en degr�)
	#define BALLSORTER_AX12_NEXT_CHERRY_POS      220
	#define BALLSORTER_AX12_DETECT_CHERRY_POS    160
	#define BALLSORTER_DETECT_TIME               0   //Attend x * 0.1 s apr�s que l'AX12 soit rendu � sa position avant de capter (unit�: 0.1s  (une valeur de 20 correspond � 0.2s soit 200ms))

#endif	/* KBALL_SORTER_CONFIG_H */

