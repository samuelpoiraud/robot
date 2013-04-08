/*  Club Robot ESEO 2012 - 2013
 *	Krusty
 *
 *  Fichier : KPlate_config.h
 *  Package : Actionneur
 *  Description : Configuration des assensseurs de verre
 *  Auteur : Alexis
 *  Version 20130505
 *  Robot : Krusty
 */

#ifndef KLIFT_CONFIG_H
#define	KLIFT_CONFIG_H

//Pour des indications sur les constantes ici

	#define LIFT_LEFT_ASSER_KP                       300
	#define LIFT_LEFT_ASSER_KI                       0
	#define LIFT_LEFT_ASSER_KD                       0
	#define LIFT_LEFT_ASSER_TIMEOUT                  3000   //en ms
	#define LIFT_LEFT_ASSER_POS_EPSILON              75
	#define LIFT_LEFT_UP_POS                         -950   //unité: potarland
	#define LIFT_LEFT_MID_POS                        -425
	#define LIFT_LEFT_DOWN_POS                       -75
	#define LIFT_LEFT_DCMOTOR_MAX_PWM_WAY0           40
	#define LIFT_LEFT_DCMOTOR_MAX_PWM_WAY1           40
	#define LIFT_LEFT_PLIER_AX12_ASSER_TIMEOUT       20     //unité: 0.1s  (une valeur de 20 correspond à 0.2s soit 200ms)
	#define LIFT_LEFT_PLIER_AX12_ASSER_POS_EPSILON   2     //en degré
	#define LIFT_LEFT_PLIER_AX12_OPEN_POS            105      //en degré
	#define LIFT_LEFT_PLIER_AX12_CLOSED_POS          165      //en degré
	#define LIFT_LEFT_PLIER_AX12_MAX_TORQUE_PERCENT  50     //A mettre a une valeur correcte pour pincer assez fort sans risquer d'endommager l'AX12.
	#define LIFT_LEFT_ACT_GO_INIT_POS                ACT_LIFT_GO_DOWN
	#define LIFT_LEFT_ACT_PLIER_AX12_INIT_POS        LIFT_LEFT_PLIER_AX12_OPEN_POS

	#define LIFT_RIGHT_ASSER_KP                      300
	#define LIFT_RIGHT_ASSER_KI                      0
	#define LIFT_RIGHT_ASSER_KD                      0
	#define LIFT_RIGHT_ASSER_TIMEOUT                 3000   //en ms
	#define LIFT_RIGHT_ASSER_POS_EPSILON             75
	#define LIFT_RIGHT_UP_POS                        -800   //unité: potarland
	#define LIFT_RIGHT_MID_POS                       -400
	#define LIFT_RIGHT_DOWN_POS                      -22
	#define LIFT_RIGHT_DCMOTOR_MAX_PWM_WAY0          40
	#define LIFT_RIGHT_DCMOTOR_MAX_PWM_WAY1          40
	#define LIFT_RIGHT_PLIER_AX12_ASSER_TIMEOUT      20    //unité: 0.1s  (une valeur de 20 correspond à 0.2s soit 200ms)
	#define LIFT_RIGHT_PLIER_AX12_ASSER_POS_EPSILON  2     //en degré
	#define LIFT_RIGHT_PLIER_AX12_OPEN_POS           195   //en degré
	#define LIFT_RIGHT_PLIER_AX12_CLOSED_POS         125   //en degré
	#define LIFT_RIGHT_PLIER_AX12_MAX_TORQUE_PERCENT 50    //A mettre a une valeur correcte pour pincer assez fort sans risquer d'endommager l'AX12.
	#define LIFT_RIGHT_ACT_GO_INIT_POS               ACT_LIFT_GO_DOWN
	#define LIFT_RIGHT_ACT_PLIER_AX12_INIT_POS       LIFT_RIGHT_PLIER_AX12_OPEN_POS

#endif	/* KLIFT_CONFIG_H */

