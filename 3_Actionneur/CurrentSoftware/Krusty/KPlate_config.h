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
/** Coefficient intégral de l'asservissement du moteur DC pour la rotation */
	#define PLATE_ASSER_KI                       0
/** Coefficient dérivé de l'asservissement du moteur DC pour la rotation (rarement utilisé) */
	#define PLATE_ASSER_KD                       0
/** Si le moteur n'atteint pas la position demandée avant ce temps, on considèer qu'il y a un problème. Temps en ms */
	#define PLATE_ASSER_TIMEOUT                  0   //en ms
/** Lorsque le moteur atteint sa position avec +/- PLATE_ASSER_POS_EPSILON d'unité potar d'écart max, on considère qu'il a atteint sa position. Angle en unité potar. */
	#define PLATE_ASSER_POS_EPSILON              60
/** Position en unité potar lorsque la pince à assiette est en position horizontale */
	#define PLATE_HORIZONTAL_POS                 40
/** Position en unité potar lorsque la pince à assiette est en position oblique (utilisé pour préparer une action ou pour taper les cadeaux) */
	#define PLATE_PREPARE_POS                    170
/** Position en unité potar lorsque la pince à assiette est en position verticale */
	#define PLATE_VERTICAL_POS                   390
/** Tension maximale en pourcentage de 24V autorisée dans le sens horizontal -> vertical */
	#define PLATE_DCMOTOR_MAX_PWM_WAY0           30
/** Tension maximale en pourcentage de 24V autorisée dans le sens vertical -> horizontal */
	#define PLATE_DCMOTOR_MAX_PWM_WAY1           30
/** Si l'ax12 (pour pincer l'assiette et la tenir) n'atteint pas la position demandée avant ce temps, on considèer qu'il y a un problème. Temps en centaine de ms. */
	#define PLATE_PLIER_AX12_ASSER_TIMEOUT       5      //unité: 0.1s  (une valeur de 20 correspond à 0.2s soit 200ms)
/** Lorsque l'ax12 atteint sa position avec +/- PLATE_PLIER_AX12_ASSER_POS_EPSILON degré d'écart max, on considère qu'il a atteint sa position. Angle en degré. */
	#define PLATE_PLIER_AX12_ASSER_POS_EPSILON   2     //en degré
/** Position en degré de l'ax12 lorsque la pince est ouverte */
	#define PLATE_PLIER_AX12_OPEN_POS            105    //en degré
/** Position en degré de l'ax12 lorsque la pince est fermée ou serre une assiette */
	#define PLATE_PLIER_AX12_CLOSED_POS          195    //en degré
/** Tension maximale en pourcentage de l'ax12. Utilisé pour limiter le courant dans le moteur lorsque l'on serre une assiette en continu */
	#define PLATE_PLIER_AX12_MAX_TORQUE_PERCENT  50     //A mettre a une valeur correcte pour pincer assez fort sans risquer d'endommager l'AX12.

#endif	/* KPLATE_CONFIG_H */

