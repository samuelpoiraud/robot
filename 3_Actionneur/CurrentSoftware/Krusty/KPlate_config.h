/*  Club Robot ESEO 2012 - 2013
 *	Krusty
 *
 *  Fichier : KPlate_config.h
 *  Package : Actionneur
 *  Description : Configuration de l'assiette
 *  Auteur : Alexis
 *  Version 20130505
 *  Robot : Krusty
 */

#ifndef KPLATE_CONFIG_H
#define	KPLATE_CONFIG_H

	#define PLATE_ASSER_KP                       400
	#define PLATE_ASSER_KI                       0
	#define PLATE_ASSER_KD                       0
	#define PLATE_ASSER_TIMEOUT                  0   //en ms
	#define PLATE_ASSER_POS_EPSILON              60  //TODO: à ajuster plus correctement
	#define PLATE_HORIZONTAL_POS                 500
	#define PLATE_PREPARE_POS                    680
	#define PLATE_VERTICAL_POS                   870   /////////////////////////////
	#define PLATE_DCMOTOR_MAX_PWM_WAY0           30
	#define PLATE_DCMOTOR_MAX_PWM_WAY1           30
	#define PLATE_PLIER_AX12_ASSER_TIMEOUT       5      //unité: 0.1s  (une valeur de 20 correspond à 0.2s soit 200ms)
	#define PLATE_PLIER_AX12_ASSER_POS_EPSILON   2     //en degré
	#define PLATE_PLIER_AX12_OPEN_POS            105    //en degré
	#define PLATE_PLIER_AX12_CLOSED_POS          195    //en degré
	#define PLATE_PLIER_AX12_MAX_TORQUE_PERCENT  50     //A mettre a une valeur correcte pour pincer assez fort sans risquer d'endommager l'AX12.

#endif	/* KPLATE_CONFIG_H */

