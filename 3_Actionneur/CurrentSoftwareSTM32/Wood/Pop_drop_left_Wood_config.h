/*  Club Robot ESEO 2014 - 2015
 *	SMALL
 *
 *	Fichier : Pop_drop_left_Wood_config.c
 *	Package : Carte actionneur
 *	Description : Gestion du bras pour faire tomber les pop corns dans le gobelet gauche
 *  Auteur : Valentin
 *  Version 2015
 *  Robot : SMALL
 */

#ifndef POP_DROP_LEFT_WOOD_CONFIG_H
#define	POP_DROP_LEFT_WOOD_CONFIG_H



//Config
	#define POP_DROP_LEFT_WOOD_AX12_ASSER_TIMEOUT				1000			// Si l'ax12 n'atteint pas la position demand�e avant ce temps, on consid�re qu'il y a un probl�me. Temps en ms.
	#define POP_DROP_LEFT_WOOD_AX12_ASSER_POS_EPSILON			2				// Lorsque l'ax12 atteint sa position avec +/- POS_EPSILON degr� d'�cart max, on consid�re qu'il a atteint sa position. Angle en degr�.
	#define POP_DROP_LEFT_WOOD_AX12_ASSER_POS_LARGE_EPSILON		10				// Si le d�placement de l'ax12 timeout mais que sa position est � +/- POS_LARGE_EPSILON degr� d'�cart max, on consid�re qu'il a atteint sa position. Angle en degr�.
	#define POP_DROP_LEFT_WOOD_AX12_MAX_TORQUE_PERCENT			100				// Couple maximum en pourcentage de l'ax12. Utilis� pour limiter le courant dans le moteur. A mettre a une valeur correcte pour pincer assez fort sans risquer d'endommager l'AX12.

//Position
	#define POP_DROP_LEFT_WOOD_AX12_IDLE_POS					110				//en degr�
	#define POP_DROP_LEFT_WOOD_AX12_MID_POS						110				//en degr�
	#define POP_DROP_LEFT_WOOD_AX12_DEPLOYED_POS				163				//en degr�

//Position initiale de l'AX12
	#define POP_DROP_LEFT_WOOD_AX12_INIT_POS        POP_DROP_LEFT_WOOD_AX12_IDLE_POS		/** Position initiale de l'AX12. Doit �tre un define du type POP_DROP_LEFT_WOOD_AX12_* */

//Extremum valeur
	#define POP_DROP_LEFT_WOOD_AX12_MAX_VALUE		300
	#define POP_DROP_LEFT_WOOD_AX12_MIN_VALUE		0



#endif	/* POP_DROP_LEFT_WOOD_CONFIG_H */


