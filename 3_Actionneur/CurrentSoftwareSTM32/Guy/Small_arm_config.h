/*  Club Robot ESEO 2013 - 2014
 *	SMALL
 *
 *	Fichier : PSmallArm_config.c
 *	Package : Carte actionneur
 *	Description : Gestion du petit bras
 *  Auteur : Arnaud
 *  Version 20130219
 *  Robot : SMALL
 */

#ifndef GSMALL_ARM_CONFIG_H
#define	GSMALL_ARM_CONFIG_H

/** Si l'ax12 n'atteint pas la position demandée avant ce temps, on considère qu'il y a un problème. Temps en centaine de ms. */
	#define SMALL_ARM_AX12_ASSER_TIMEOUT       10      //unité: 0.1s  (une valeur de 20 correspond à 0.2s soit 200ms)
/** Lorsque l'ax12 atteint sa position avec +/- SMALL_ARM_AX12_ASSER_POS_EPSILON degré d'écart max, on considère qu'il a atteint sa position. Angle en degré. */
	#define SMALL_ARM_AX12_ASSER_POS_EPSILON   2     //en degré
/** Si le déplacement de l'ax12 timeout mais que sa position est à +/- SMALL_ARM_AX12_ASSER_POS_LARGE_EPSILON degré d'écart max, on considère qu'il a atteint sa position. Angle en degré. */
	#define SMALL_ARM_AX12_ASSER_POS_LARGE_EPSILON   10     //en degré
/** Position en degré de l'ax12 lorsque SMALL_ARM est rentré  */
	#define SMALL_ARM_AX12_IDLE_POS            230    //en degré
/** Position en degré de l'ax12 lorsque SMALL_ARM est mi sortie */
	#define SMALL_ARM_AX12_MID_POS			   180    //en degré
/** Position en degré de l'ax12 lorsque SMALL_ARM est sortie */
	#define SMALL_ARM_AX12_DEPLOYED_POS        156   //en degré
/** Tension maximale en pourcentage de l'ax12. Utilisé pour limiter le courant dans le moteur  */
	#define SMALL_ARM_AX12_MAX_TORQUE_PERCENT  100     //A mettre a une valeur correcte pour pincer assez fort sans risquer d'endommager l'AX12.
/** Position initiale de l'AX12. Doit être un define du type PLATE_PLIER_AX12_* */
	#define SMALL_ARM_AX12_INIT_POS        SMALL_ARM_AX12_IDLE_POS

	#define SMALL_ARM_AX12_MAX_VALUE	270
	#define SMALL_ARM_AX12_MIN_VALUE	60


#endif	/* GSMALL_ARM_CONFIG_H */


