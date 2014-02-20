/*  Club Robot ESEO 2012 - 2013
 *	Krusty
 *
 *  Fichier : KPlate_config.h
 *  Package : Actionneur
 *  Description : Configuration du lanceur de cerises
 *  Auteur : Alexis
 *  Version 20130505
 *  Robot : Krusty
 */

#ifndef KBALL_LAUNCHER_CONFIG_H
#define	KBALL_LAUNCHER_CONFIG_H

	#define BALLLAUNCHER_ASSER_KP                10
	#define BALLLAUNCHER_ASSER_KI                60
	#define BALLLAUNCHER_ASSER_KD                0
	#define BALLLAUNCHER_ASSER_TIMEOUT           3000       //en ms
	#define BALLLAUNCHER_ASSER_POS_EPSILON       100		//en tr/min
	#define BALLLAUNCHER_DCMOTOR_MAX_PWM_WAY0    50
	#define BALLLAUNCHER_DCMOTOR_MAX_PWM_WAY1    0
	#define BALLLAUNCHER_EDGE_PER_ROTATION       1			//nombre d'inversion de champ magnétique par tour (2 aimants de sens opposé = 2 inversions / tour)

#endif	/* KBALL_LAUNCHER_CONFIG_H */

