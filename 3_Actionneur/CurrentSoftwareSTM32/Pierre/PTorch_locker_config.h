/*  Club Robot ESEO 2013 - 2014
 *
 *	Fichier : PSmallArm_config.c
 *	Package : Carte actionneur
 *	Description : Gestion du syst�me de v�rouillage des torches
 *  Auteur : Arnaud
 *  Version 20130219
 *  Robot : BIG
 */

#ifndef PTORCH_LOCKER_CONFIG_H
#define	PTORCH_LOCKER_CONFIG_H

/** Si l'ax12 n'atteint pas la position demand�e avant ce temps, on consid�re qu'il y a un probl�me. Temps en centaine de ms. */
	#define TORCH_LOCKER_AX12_1_ASSER_TIMEOUT       1000      // ms
/** Lorsque l'ax12 atteint sa position avec +/- TORCH_LOCKER_AX12_ASSER_POS_EPSILON degr� d'�cart max, on consid�re qu'il a atteint sa position. Angle en degr�. */
	#define TORCH_LOCKER_AX12_1_ASSER_POS_EPSILON   2     //en degr�
/** Si le d�placement de l'ax12 timeout mais que sa position est � +/- TORCH_LOCKER_AX12_ASSER_POS_LARGE_EPSILON degr� d'�cart max, on consid�re qu'il a atteint sa position. Angle en degr�. */
	#define TORCH_LOCKER_AX12_1_ASSER_POS_LARGE_EPSILON   10     //en degr�
/** Position en degr� de l'ax12 lorsque TORCH_LOCKER est rentr�  */
	#define TORCH_LOCKER_AX12_1_LOCK_POS            98    //en degr�
/** Position en degr� de l'ax12 lorsque TORCH_LOCKER est mi sortie */
	#define TORCH_LOCKER_AX12_1_UNLOCK_POS			150    //en degr�
/** Position en degr� de l'ax12 lorsque TORCH_LOCKER est mi en position rentrer */
	#define TORCH_LOCKER_AX12_1_INSIDE_POS			60    //en degr�
/** Tension maximale en pourcentage de l'ax12. Utilis� pour limiter le courant dans le moteur  */
	#define TORCH_LOCKER_AX12_1_MAX_TORQUE_PERCENT  95     //A mettre a une valeur correcte pour pincer assez fort sans risquer d'endommager l'AX12.
/** Position initiale de l'AX12. */
	#define TORCH_LOCKER_AX12_1_INIT_POS        TORCH_LOCKER_AX12_1_INSIDE_POS

	#define TORCH_LOCKER_AX12_1_MAX_VALUE	300
	#define TORCH_LOCKER_AX12_1_MIN_VALUE	0


/** Si l'ax12 n'atteint pas la position demand�e avant ce temps, on consid�re qu'il y a un probl�me. Temps en centaine de ms. */
	#define TORCH_LOCKER_AX12_2_ASSER_TIMEOUT       1000	// ms
/** Lorsque l'ax12 atteint sa position avec +/- TORCH_LOCKER_AX12_ASSER_POS_EPSILON degr� d'�cart max, on consid�re qu'il a atteint sa position. Angle en degr�. */
	#define TORCH_LOCKER_AX12_2_ASSER_POS_EPSILON   2     //en degr�
/** Si le d�placement de l'ax12 timeout mais que sa position est � +/- TORCH_LOCKER_AX12_ASSER_POS_LARGE_EPSILON degr� d'�cart max, on consid�re qu'il a atteint sa position. Angle en degr�. */
	#define TORCH_LOCKER_AX12_2_ASSER_POS_LARGE_EPSILON   10     //en degr�
/** Position en degr� de l'ax12 lorsque TORCH_LOCKER est rentr�  */
	#define TORCH_LOCKER_AX12_2_LOCK_POS            198    //en degr�
/** Position en degr� de l'ax12 lorsque TORCH_LOCKER est mi sortie */
	#define TORCH_LOCKER_AX12_2_UNLOCK_POS			150    //en degr�
/** Position en degr� de l'ax12 lorsque TORCH_LOCKER est mi en position rentrer */
	#define TORCH_LOCKER_AX12_2_INSIDE_POS			240    //en degr�
/** Tension maximale en pourcentage de l'ax12. Utilis� pour limiter le courant dans le moteur  */
	#define TORCH_LOCKER_AX12_2_MAX_TORQUE_PERCENT  95     //A mettre a une valeur correcte pour pincer assez fort sans risquer d'endommager l'AX12.
/** Position initiale de l'AX12. */
	#define TORCH_LOCKER_AX12_2_INIT_POS        TORCH_LOCKER_AX12_2_INSIDE_POS

	#define TORCH_LOCKER_AX12_2_MAX_VALUE	300
	#define TORCH_LOCKER_AX12_2_MIN_VALUE	0


#endif	/* PTORCH_LOCKER_CONFIG_H */


