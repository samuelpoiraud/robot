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

/** Coefficient proportionnel de l'asservissement du moteur DC pour la rotation */
	#define PLATE_ASSER_KP                       400
/** Coefficient int�gral de l'asservissement du moteur DC pour la rotation */
	#define PLATE_ASSER_KI                       0
/** Coefficient d�riv� de l'asservissement du moteur DC pour la rotation (rarement utilis�) */
	#define PLATE_ASSER_KD                       0
/** Coefs pour la descente de l'assiette afin d'amortir le mouvement et ne pas trop taper le sol */
	#define PLATE_ASSER_AMORTIZED_KP             400
	#define PLATE_ASSER_AMORTIZED_KI             0
	#define PLATE_ASSER_AMORTIZED_KD             20000
/** Si le moteur n'atteint pas la position demand�e avant ce temps, on consid�er qu'il y a un probl�me. Temps en ms */
	#define PLATE_ASSER_TIMEOUT                  0   //en ms
/** Lorsque le moteur atteint sa position avec +/- PLATE_ASSER_POS_EPSILON d'unit� potar d'�cart max, on consid�re qu'il a atteint sa position. Angle en unit� potar. */
	#define PLATE_ASSER_POS_EPSILON              60
/** Position en unit� potar lorsque la pince � assiette est en position horizontale */
	#define PLATE_HORIZONTAL_POS                 720
/** Position en unit� potar lorsque la pince � assiette est en position oblique (utilis� pour pr�parer une action ou pour taper les cadeaux) */
	#define PLATE_PREPARE_POS                    855
/** Position en unit� potar lorsque la pince � assiette est en position verticale */
	#define PLATE_VERTICAL_POS                   1040
/** Tension maximale en pourcentage de 24V autoris�e dans le sens horizontal -> vertical */
	#define PLATE_DCMOTOR_MAX_PWM_WAY0           30
/** Tension maximale en pourcentage de 24V autoris�e dans le sens vertical -> horizontal */
	#define PLATE_DCMOTOR_MAX_PWM_WAY1           15
/** Si l'ax12 (pour pincer l'assiette et la tenir) n'atteint pas la position demand�e avant ce temps, on consid�er qu'il y a un probl�me. Temps en centaine de ms. */
	#define PLATE_PLIER_AX12_ASSER_TIMEOUT       7      //unit�: 0.1s  (une valeur de 20 correspond � 0.2s soit 200ms)
/** Lorsque l'ax12 atteint sa position avec +/- PLATE_PLIER_AX12_ASSER_POS_EPSILON degr� d'�cart max, on consid�re qu'il a atteint sa position. Angle en degr�. */
	#define PLATE_PLIER_AX12_ASSER_POS_EPSILON   2     //en degr�
/** Position en degr� de l'ax12 lorsque la pince est ouverte */
	#define PLATE_PLIER_AX12_OPEN_POS            105    //en degr�
/** Position en degr� de l'ax12 lorsque la pince est ferm�e ou serre une assiette */
	#define PLATE_PLIER_AX12_CLOSED_POS          195    //en degr�
/** Tension maximale en pourcentage de l'ax12. Utilis� pour limiter le courant dans le moteur lorsque l'on serre une assiette en continu */
	#define PLATE_PLIER_AX12_MAX_TORQUE_PERCENT  50     //A mettre a une valeur correcte pour pincer assez fort sans risquer d'endommager l'AX12.
/** Position initiale du moteur DC. Doit �tre un message CAN de commande ACT_PLATE_ROTATE_* */
	#define PLATE_ACT_ROTATE_INIT_POS            ACT_PLATE_ROTATE_VERTICALLY
/** Position initiale de l'AX12. Doit �tre un define du type PLATE_PLIER_AX12_* */
	#define PLATE_ACT_PLIER_AX12_INIT_POS        PLATE_PLIER_AX12_CLOSED_POS

#endif	/* KPLATE_CONFIG_H */

