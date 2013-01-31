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
	#include "../QS/QS_timer.h"
	
	#ifdef USE_STEP_MOTOR
		void STEP_MOTOR_init();
	
	
		/* pour fonctionnement interne */
		#ifdef QS_STEP_MOTOR_C
			void STEP_MOTOR_apply_step(Uint8 step);
		
			#define STEP_SEQUENCE_SIZE	8
			#if STEP_MOTOR_TIMER == 1
					#define STEP_MOTOR_TIMER_RUN TIMER1_run
					#define STEP_MOTOR_TIMER_IT		_T1Interrupt
					#define STEP_MOTOR_TIMER_FLAG	IFS0bits.T1IF
			#elif STEP_MOTOR_TIMER == 2
					#define STEP_MOTOR_TIMER_RUN TIMER2_run
					#define STEP_MOTOR_TIMER_IT		_T2Interrupt
					#define STEP_MOTOR_TIMER_FLAG	IFS0bits.T2IF
			#elif STEP_MOTOR_TIMER == 3
					#define STEP_MOTOR_TIMER_RUN TIMER3_run
					#define STEP_MOTOR_TIMER_IT		_T3Interrupt
					#define STEP_MOTOR_TIMER_FLAG	IFS0bits.T3IF
			#else
				#error "STEP_MOTOR_TIMER doit etre 1 2 ou 3"
			#endif /* STEP_MOTOR_TIMER == n */
		#else
			#if (STEP_MOTOR_TIMER != 1)&&(STEP_MOTOR_TIMER != 2)&&(STEP_MOTOR_TIMER != 3)
				#error "STEP_MOTOR_TIMER doit etre 1 2 ou 3"
			#endif /*(STEP_MOTOR_TIMER!=1)&&(STEP_MOTOR_TIMER!=2)&&(STEP_MOTOR_TIMER!=3)*/
		#endif /* defined QS_STEP_MOTOR_C, else */
	#endif /* def USE_STEP_MOTOR */	
#endif /* ndef STEP_MOTOR_H */
