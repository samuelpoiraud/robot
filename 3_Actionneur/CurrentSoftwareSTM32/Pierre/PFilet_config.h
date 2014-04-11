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

#include "../clock.h"

/** Si l'ax12 n'atteint pas la position demand�e avant ce temps, on consid�re qu'il y a un probl�me. Temps en centaine de ms. */
	#define FILET_AX12_ASSER_TIMEOUT       10      //unit�: 0.1s  (une valeur de 20 correspond � 0.2s soit 200ms)
/** Lorsque l'ax12 atteint sa position avec +/- PLATE_PLIER_AX12_ASSER_POS_EPSILON degr� d'�cart max, on consid�re qu'il a atteint sa position. Angle en degr�. */
	#define FILET_AX12_ASSER_POS_EPSILON   2     //en degr�
/** Position en degr� de l'ax12 lorsque filet pas lanc�  */
	#define FILET_AX12_IDLE_POS            68    //en degr�
/** Position en degr� de l'ax12 lorsque filet lanc� */
	#define FILET_AX12_LAUNCHED_POS          240    //en degr�
/** Tension maximale en pourcentage de l'ax12. Utilis� pour limiter le courant dans le moteur  */
	#define FILET_AX12_MAX_TORQUE_PERCENT  100     //A mettre a une valeur correcte pour pincer assez fort sans risquer d'endommager l'AX12.
/** Position initiale de l'AX12. Doit �tre un define du type PLATE_PLIER_AX12_* */
	#define FILET_AX12_INIT_POS        FILET_AX12_IDLE_POS

#define QUANTUM_IT						QUANTUM_CLOCK_IT  //[ms], unit� de temps de clock.h
#define TIME_FILET_IT					(1*QUANTUM_IT)
#define TIME_BEFORE_REARM				500  //[ms]
#define TIME_BEFORE_DETECT				TIME_BEFORE_REARM
#define TIME_BEFORE_FREE_STRING			1000  //[ms]
#define TIME_OSC_STRING					200  //[ms]  (p�riode)
#define NB_OSC_STRING					6			//((3500-TIME_BEFORE_FREE_STRING)/TIME_OSC_STRING) pour avoir des oscillations jusqu'a la fin du match
#define EPSILON_POS_FILET				8

#define FILET_AX12_MAX_VALUE	240
#define FILET_AX12_MIN_VALUE	68

#endif	/* PFILET_CONFIG_H */


