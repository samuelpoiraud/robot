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


/********* Position du AX12 labium différent état *********/
/** Si l'ax12 n'atteint pas la position demandée avant ce temps, on considèer qu'il y a un problème. Temps en centaine de ms. */
	#define FRUIT_AX12_ASSER_TIMEOUT       10      //unité: 0.1s  (une valeur de 20 correspond à 0.2s soit 200ms)
/** Lorsque l'ax12 atteint sa position avec +/- PLATE_PLIER_AX12_ASSER_POS_EPSILON degré d'écart max, on considère qu'il a atteint sa position. Angle en degré. */
	#define FRUIT_AX12_ASSER_POS_EPSILON   2     //en degré
/** Position en degré de l'ax12 lorsque la pince est ouverte */
	#define FRUIT_AX12_LABIUM_OPEN_POS		240    //en degré
/** Position en degré de l'ax12 lorsque la pince est fermée ou serre une assiette */
	#define FRUIT_AX12_LABIUM_CLOSE_POS		60   //en degré
/** Tension maximale en pourcentage de l'ax12. Utilisé pour limiter le courant dans le moteur lorsque l'on serre une assiette en continu */
	#define FRUIT_AX12_MAX_TORQUE_PERCENT  50     //A mettre a une valeur correcte pour pincer assez fort sans risquer d'endommager l'AX12.
/** Position initiale de l'AX12. Doit être un define du type PLATE_PLIER_AX12_* */
	#define FRUIT_AX12_LABIUM_INIT_POS		FRUIT_AX12_LABIUM_CLOSE_POS



/******** POMPE ********/

	#define FRUIT_POMPE_ASSER_TIMEOUT		3000
	#define FRUIT_POMPE_MAX_PWM_WAY			100  // rapport cyclique de la pwm
#endif	/* PFRUIT_CONFIG_H */

