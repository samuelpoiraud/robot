/*  Club Robot ESEO 2013 - 2014
 *	BIG
 *
 *  Fichier : PFruit_config.h
 *  Package : Actionneur
 *  Description : Configuration du proto_fruit_mouth
 *  Auteur : Amaury
 *  Version 20130505
 *  Robot : BIG
 */


#ifndef PFILET_CONFIG_H
#define	PFILET_CONFIG_H

/** Si l'ax12 n'atteint pas la position demandée avant ce temps, on considère qu'il y a un problème. Temps en centaine de ms. */
	#define FILET_AX12_ASSER_TIMEOUT       10      //unité: 0.1s  (une valeur de 20 correspond à 0.2s soit 200ms)
/** Lorsque l'ax12 atteint sa position avec +/- PLATE_PLIER_AX12_ASSER_POS_EPSILON degré d'écart max, on considère qu'il a atteint sa position. Angle en degré. */
	#define FILET_AX12_ASSER_POS_EPSILON   2     //en degré
/** Position en degré de l'ax12 lorsque filet pas lancé  */
	#define FILET_AX12_IDLE_POS            130    //en degré
/** Position en degré de l'ax12 lorsque filet lancé */
	#define FILET_AX12_LAUNCHED_POS          210    //en degré
/** Tension maximale en pourcentage de l'ax12. Utilisé pour limiter le courant dans le moteur  */
	#define FILET_AX12_MAX_TORQUE_PERCENT  50     //A mettre a une valeur correcte pour pincer assez fort sans risquer d'endommager l'AX12.
/** Position initiale de l'AX12. Doit être un define du type PLATE_PLIER_AX12_* */
	#define FILET_AX12_INIT_POS        FILET_AX12_IDLE_POS


#endif	/* PFILET_CONFIG_H */


