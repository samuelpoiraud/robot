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

#ifndef PFRUIT_CONFIG_H
#define	PFRUIT_CONFIG_H


/********* Position du AX12 labium diff�rent �tat *********/
/** Si l'ax12 n'atteint pas la position demand�e avant ce temps, on consid�er qu'il y a un probl�me. Temps en centaine de ms. */
	#define FRUIT_AX12_ASSER_TIMEOUT       10      //unit�: 0.1s  (une valeur de 20 correspond � 0.2s soit 200ms)
/** Lorsque l'ax12 atteint sa position avec +/- PLATE_PLIER_AX12_ASSER_POS_EPSILON degr� d'�cart max, on consid�re qu'il a atteint sa position. Angle en degr�. */
	#define FRUIT_AX12_ASSER_POS_EPSILON   2     //en degr�
/** Position en degr� de l'ax12 lorsque la pince est ouverte */
	#define FRUIT_AX12_LABIUM_OPEN_POS		123    //en degr�
/** Position en degr� de l'ax12 lorsque la pince est ferm�e ou serre une assiette */
	#define FRUIT_AX12_LABIUM_CLOSE_POS		0   //en degr�
/** Tension maximale en pourcentage de l'ax12. Utilis� pour limiter le courant dans le moteur lorsque l'on serre une assiette en continu */
	#define FRUIT_AX12_MAX_TORQUE_PERCENT  100     //A mettre a une valeur correcte pour pincer assez fort sans risquer d'endommager l'AX12.
/** Position initiale de l'AX12. Doit �tre un define du type PLATE_PLIER_AX12_* */
	#define FRUIT_AX12_LABIUM_INIT_POS		FRUIT_AX12_LABIUM_CLOSE_POS

#define FRUIT_LABIUM_AX12_MAX_VALUE	123
#define FRUIT_LABIUM_AX12_MIN_VALUE	0

/******** POMPE ********/

	#define FRUIT_POMPE_ASSER_TIMEOUT		50 //en centaine de msec, 10 = 1s
	#define FRUIT_POMPE_MAX_PWM_WAY			30  // rapport cyclique de la pwm
#endif	/* PFRUIT_CONFIG_H */

