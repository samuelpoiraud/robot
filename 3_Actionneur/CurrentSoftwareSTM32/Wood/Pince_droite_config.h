/*  Club Robot ESEO 2014 - 2015
 *	SMALL
 *
 *	Fichier : Pince_droite_config.c
 *	Package : Carte actionneur
 *	Description : Gestion de l'actionneur de la pince droite
 *  Auteur : Valentin
 *  Version 20130219
 *  Robot : SMALL
 */

#ifndef PINCE_DROITE_CONFIG_H
#define	PINCE_DROITE_CONFIG_H

#if 1

/** Si l'ax12 n'atteint pas la position demandée avant ce temps, on considère qu'il y a un problème. Temps en centaine de ms. */
	#define PINCE_DROITE_AX12_ASSER_TIMEOUT       1000		// ms
/** Lorsque l'ax12 atteint sa position avec +/- PINCE_DROITE_AX12_ASSER_POS_EPSILON degré d'écart max, on considère qu'il a atteint sa position. Angle en degré. */
	#define PINCE_DROITE_AX12_ASSER_POS_EPSILON   2			//en degré
/** Si le déplacement de l'ax12 timeout mais que sa position est à +/- PINCE_DROITE_AX12_ASSER_POS_LARGE_EPSILON degré d'écart max, on considère qu'il a atteint sa position. Angle en degré. */
	#define PINCE_DROITE_AX12_ASSER_POS_LARGE_EPSILON   10	//en degré
/** Position en degré de l'ax12 lorsque PINCE_DROITE est rentré  */
	#define PINCE_DROITE_AX12_IDLE_POS            163		//en degré
/** Position en degré de l'ax12 lorsque PINCE_DROITE est mi sortie */
	#define PINCE_DROITE_AX12_MID_POS			   136		//en degré
/** Position en degré de l'ax12 lorsque PINCE_DROITE est sortie */
	#define PINCE_DROITE_AX12_DEPLOYED_POS        74		//en degré
/** Tension maximale en pourcentage de l'ax12. Utilisé pour limiter le courant dans le moteur  */
	#define PINCE_DROITE_AX12_MAX_TORQUE_PERCENT  100		//A mettre a une valeur correcte pour pincer assez fort sans risquer d'endommager l'AX12.
/** Position initiale de l'AX12. Doit être un define du type PINCE_DROITE_AX12_* */
	#define PINCE_DROITE_AX12_INIT_POS        PINCE_DROITE_AX12_IDLE_POS

	#define PINCE_DROITE_AX12_MAX_VALUE	270
	#define PINCE_DROITE_AX12_MIN_VALUE	60

#endif

#endif	/* PINCE_DROITE_CONFIG_H */


