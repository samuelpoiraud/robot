/*
 *	Club Robot ESEO 2008 - 2009
 *	Archi-Tech'
 *
 *	Fichier : QS_Step_motor.h
 *	Package : Qualité Software
 *	Description : 	fonction de gestion du moteur pas à pas
 *	Auteur : Jacen
 *	Version 20090220
 */
 
/*	Notes d'exploitation :
 *	Ce module a pour fonction de controler un moteur pas à pas
 *	En appliquant les niveaux de tension adéquats.
 *	On considère un moteur pas à pas à quatres accès nommés
 *	STEP_MOTOR1, STEP_MOTOR2, STEP_MOTOR3 et STEP_MOTOR4
 *	dans Global_config.
 *	Attribuez les pins de manières à ce que STEP_MOTOR1 soit
 *	l'entrée opposée de STEP_MOTOR3.
 *	Ce pilote ne vérifie pas que les pins utilisées sont bien
 *	configurées en sorties.
 *	Si vous utilisez un pont en H (exm: L298N), ce pilote n'active
 *	pas les ENABLE du pont.
 *
 *	Ce pilote travaille avec une consigne en demi-pas.
 *	Une fois le pilote initialisé, vous n'avez plus qu'à travailler
 *	sur la consigne global.STEP_MOTOR_order par incrémentation
 *	ou par décrémentation. Ce pilote ne garantit pas le bon
 *	fonctionnement du moteur avec des consignes absolues
 *	(exm : global.STEP_MOTOR_order = 42;), notamment pour des
 *	variations de plus de 30000 demi pas
 */

#ifndef QS_STEP_MOTOR_H
	#define QS_STEP_MOTOR_H
	
	#include "../QS/QS_all.h"
	
	#ifdef USE_STEP_MOTOR
		void STEP_MOTOR_init();
	
	
	#endif /* def USE_STEP_MOTOR */	
#endif /* ndef STEP_MOTOR_H */
