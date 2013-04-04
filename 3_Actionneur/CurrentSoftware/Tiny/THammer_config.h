/*  Club Robot ESEO 2012 - 2013
 *	Tiny
 *
 *  Fichier : KPlate_config.h
 *  Package : Actionneur
 *  Description : Configuration du bras qui souffle les bougies
 *  Auteur : Alexis
 *  Version 20130505
 *  Robot : Tiny
 */

#ifndef TBALL_INFLATER_CONFIG_H
#define	TBALL_INFLATER_CONFIG_H

	#define HAMMER_ASSER_KP             400
	#define HAMMER_ASSER_KI             0
	#define HAMMER_ASSER_KD             0
	#define HAMMER_ASSER_TIMEOUT        3000    //en ms
	#define HAMMER_ASSER_POS_EPSILON    20
	#define HAMMER_UNITS_PER_128_DEGRE  -494     //TODO: A changer
	#define HAMMER_UNITS_AT_0_DEGRE     -203
	#define HAMMER_DCMOTOR_MAX_PWM_WAY0 20		//en %
	#define HAMMER_DCMOTOR_MAX_PWM_WAY1 20		//en %

#endif	/* TBALL_INFLATER_CONFIG_H */

