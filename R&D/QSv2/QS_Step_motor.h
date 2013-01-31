/*
 *	Club Robot ESEO 2008 - 2009
 *	Archi-Tech'
 *
 *	Fichier : QS_Step_motor.h
 *	Package : Qualit� Software
 *	Description : 	fonction de gestion du moteur pas � pas
 *	Auteur : Jacen
 *	Version 20090220
 */
 
/*	Notes d'exploitation :
 *	Ce module a pour fonction de controler un moteur pas � pas
 *	En appliquant les niveaux de tension ad�quats.
 *	On consid�re un moteur pas � pas � quatres acc�s nomm�s
 *	STEP_MOTOR1, STEP_MOTOR2, STEP_MOTOR3 et STEP_MOTOR4
 *	dans Global_config.
 *	Attribuez les pins de mani�res � ce que STEP_MOTOR1 soit
 *	l'entr�e oppos�e de STEP_MOTOR3.
 *	Ce pilote ne v�rifie pas que les pins utilis�es sont bien
 *	configur�es en sorties.
 *	Si vous utilisez un pont en H (exm: L298N), ce pilote n'active
 *	pas les ENABLE du pont.
 *
 *	Ce pilote travaille avec une consigne en demi-pas.
 *	Une fois le pilote initialis�, vous n'avez plus qu'� travailler
 *	sur la consigne global.STEP_MOTOR_order par incr�mentation
 *	ou par d�cr�mentation. Ce pilote ne garantit pas le bon
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
