/*  Club Robot ESEO 2013 - 2014
 *	SMALL
 *
 *	Fichier : PSmallArm_config.c
 *	Package : Carte actionneur
 *	Description : Gestion du petit bras
 *  Auteur : Arnaud
 *  Version 20130219
 *  Robot : BIG
 */

#ifndef PSMALL_ARM_CONFIG_H
#define	PSMALL_ARM_CONFIG_H

/** Si l'ax12 n'atteint pas la position demand�e avant ce temps, on consid�re qu'il y a un probl�me. Temps en centaine de ms. */
	#define SMALL_ARM_AX12_ASSER_TIMEOUT       10      //unit�: 0.1s  (une valeur de 20 correspond � 0.2s soit 200ms)
/** Lorsque l'ax12 atteint sa position avec +/- SMALL_ARM_AX12_ASSER_POS_EPSILON degr� d'�cart max, on consid�re qu'il a atteint sa position. Angle en degr�. */
	#define SMALL_ARM_AX12_ASSER_POS_EPSILON   2     //en degr�
/** Si le d�placement de l'ax12 timeout mais que sa position est � +/- SMALL_ARM_AX12_ASSER_POS_LARGE_EPSILON degr� d'�cart max, on consid�re qu'il a atteint sa position. Angle en degr�. */
	#define SMALL_ARM_AX12_ASSER_POS_LARGE_EPSILON   10     //en degr�
/** Position en degr� de l'ax12 lorsque SMALL_ARM est rentr�  */
	#define SMALL_ARM_AX12_IDLE_POS            170    //en degr�
/** Position en degr� de l'ax12 lorsque SMALL_ARM est mi sortie */
	#define SMALL_ARM_AX12_MID_POS			   154    //en degr�
/** Position en degr� de l'ax12 lorsque SMALL_ARM est sortie */
	#define SMALL_ARM_AX12_DEPLOYED_POS        219   //en degr�
/** Tension maximale en pourcentage de l'ax12. Utilis� pour limiter le courant dans le moteur  */
	#define SMALL_ARM_AX12_MAX_TORQUE_PERCENT  50     //A mettre a une valeur correcte pour pincer assez fort sans risquer d'endommager l'AX12.
/** Position initiale de l'AX12. Doit �tre un define du type PLATE_PLIER_AX12_* */
	#define SMALL_ARM_AX12_INIT_POS        SMALL_ARM_AX12_DEPLOYED_POS

	#define SMALL_ARM_AX12_MAX_VALUE	230
	#define SMALL_ARM_AX12_MIN_VALUE	100


#endif	/* PSMALL_ARM_CONFIG_H */


