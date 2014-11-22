/*  Club Robot ESEO 2014 - 2015
 *	HOLLY - WOOD
 *
 *	Fichier : Pop_collect_right.h
 *	Package : Carte actionneur
 *	Description : Gestion de l'ax12 de la depose des popcorns gauche
 *  Auteur : Anthony
 *  Version 20130219
 *  Robot : HOLLY
 */

#ifndef POP_COLLECT_RIGHT_CONFIG_H
#define	POP_COLLECT_RIGHT_CONFIG_H

	#define POP_COLLECT_RIGHT_AX12_ASSER_TIMEOUT				1000			// ms	/** Si l'ax12 n'atteint pas la position demand�e avant ce temps, on consid�re qu'il y a un probl�me. Temps en centaine de ms. */
	#define POP_COLLECT_RIGHT_AX12_ASSER_POS_EPSILON			2				//en degr�	/** Lorsque l'ax12 atteint sa position avec +/- POP_COLLECT_RIGHT_AX12_ASSER_POS_EPSILON degr� d'�cart max, on consid�re qu'il a atteint sa position. Angle en degr�. */
	#define POP_COLLECT_RIGHT_AX12_ASSER_POS_LARGE_EPSILON		10				//en degr�	/** Si le d�placement de l'ax12 timeout mais que sa position est � +/- POP_COLLECT_RIGHT_AX12_ASSER_POS_LARGE_EPSILON degr� d'�cart max, on consid�re qu'il a atteint sa position. Angle en degr�. */

	#define POP_COLLECT_RIGHT_AX12_IDLE_POS						110				//en degr�	/** Position en degr� de l'ax12 lorsque POP_COLLECT_RIGHT est rentr�  */
	#define POP_COLLECT_RIGHT_AX12_MID_POS						110				//en degr�	/** Position en degr� de l'ax12 lorsque POP_COLLECT_RIGHT est mi sortie */
	#define POP_COLLECT_RIGHT_AX12_DEPLOYED_POS					163				//en degr�	/** Position en degr� de l'ax12 lorsque POP_COLLECT_RIGHT est sortie */

	#define POP_COLLECT_RIGHT_AX12_MAX_TORQUE_PERCENT			100				//A mettre a une valeur correcte pour pincer assez fort sans risquer d'endommager l'AX12.	/** Tension maximale en pourcentage de l'ax12. Utilis� pour limiter le courant dans le moteur  */
	#define POP_COLLECT_RIGHT_AX12_INIT_POS        POP_COLLECT_RIGHT_AX12_IDLE_POS		/** Position initiale de l'AX12. Doit �tre un define du type POP_COLLECT_RIGHT_AX12_* */

	#define POP_COLLECT_RIGHT_AX12_MAX_VALUE		270
	#define POP_COLLECT_RIGHT_AX12_MIN_VALUE		60

#endif	/* EXEMPLE_CONFIG_H */



