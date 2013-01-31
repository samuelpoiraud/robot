/*
 *	Club Robot ESEO 2008 - 2009
 *	Archi-Tech'
 *
 *	Fichier : QS_Step_motor.h
 *	Package : Qualité Software
 *	Description : 	fonction de gestion du moteur pas à pas
 *	Auteur : Jacen
 *	Licence : CeCILL-C (voir LICENCE.txt)
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
 */

#ifndef QS_STEP_MOTOR_H
	#define QS_STEP_MOTOR_H
	
	#include "../QS/QS_all.h"
	#include "../QS/QS_timer.h"
	
	#ifdef USE_STEP_MOTOR
	
		void STEP_MOTOR_init();
		
		void STEP_MOTOR_add_steps(Sint8 nb_of_steps);

	#endif /* def USE_STEP_MOTOR */	
#endif /* ndef STEP_MOTOR_H */
