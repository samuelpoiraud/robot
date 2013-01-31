/*
 *	Club Robot ESEO 2008 - 2009
 *	Archi-Tech'
 *
 *	Fichier : QS_servo.h
 *	Package : Qualite Software
 *	Description : Driver par timer pour servomoteur
 *	Auteur : Jacen
 *	Version 20090301
 */

#ifndef QS_SERVO_H
#define QS_SERVO_H
#include "../QS/QS_all.h"
#include "../QS/QS_timer.h"

#ifdef USE_SERVO
	/*-------------------------------------
		Configuration des servomoteurs
	-------------------------------------*/

	// Initialisation des servomoteurs
	void SERVO_init() ;

	// Changement de la commande (entre MIN et MAX_COMMANDE_SERVO us)
	void SERVO_set_cmd(Uint16 cmd, Uint8 num_servo);


	#ifdef QS_SERVO_C
	
		/*-------------------------------------
			Gestion des servomoteurs
		-------------------------------------*/
		
		// Fonction d'interruption du timer
		#if SERVO_TIMER == 1
				#define SERVO_TIMER_RUN_FINE	TIMER1_run_us
				#define SERVO_TIMER_IT			_T1Interrupt
				#define SERVO_TIMER_FLAG		IFS0bits.T1IF
		#elif SERVO_TIMER == 2
				#define SERVO_TIMER_RUN_FINE	TIMER2_run_us
				#define SERVO_TIMER_IT			_T2Interrupt
				#define SERVO_TIMER_FLAG		IFS0bits.T2IF
		#elif SERVO_TIMER == 3
				#define SERVO_TIMER_RUN_FINE	TIMER3_run_us
				#define SERVO_TIMER_IT			_T3Interrupt
				#define SERVO_TIMER_FLAG		IFS0bits.T3IF
		#else
			#error "SERVO_TIMER doit etre 1 2 ou 3"
		#endif /* SERVO_TIMER == n */
			
		#define MIN_INTERVAL		20
		
	#endif /* def QS_SERVO_C */
#endif /* def USE_SERVO */	
#endif /* ndef QS_SERVO_H */
