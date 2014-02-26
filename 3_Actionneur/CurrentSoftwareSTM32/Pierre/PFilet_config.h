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
	#define FILET_AX12_IDLE_POS            120    //en degré
/** Position en degré de l'ax12 lorsque filet lancé */
	#define FILET_AX12_LAUNCHED_POS          240    //en degré
/** Tension maximale en pourcentage de l'ax12. Utilisé pour limiter le courant dans le moteur  */
	#define FILET_AX12_MAX_TORQUE_PERCENT  100     //A mettre a une valeur correcte pour pincer assez fort sans risquer d'endommager l'AX12.
/** Position initiale de l'AX12. Doit être un define du type PLATE_PLIER_AX12_* */
	#define FILET_AX12_INIT_POS        FILET_AX12_IDLE_POS

#define QUANTUM_IT						100  //[ms], unité de temps de clock.c
#define TIME_FILET_IT					(1*QUANTUM_IT)
#define TIME_BEFORE_REARM				500  //[ms]
#define TIME_BEFORE_DETECT				TIME_BEFORE_REARM
#define TIME_BEFORE_FREE_STRING			1000  //[ms]
#define TIME_OSC_STRING					200  //[ms]  (période)
#define NB_OSC_STRING					4
#define EPSILON_POS_FILET				4


#endif	/* PFILET_CONFIG_H */


