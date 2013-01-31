/*
 *	PFE Simulateur QSx86 2011-2012
 *	Logiciel EVE
 *
 *	Fichier : QS_timer.h
 *	Package : QSx86
 *	Description : Gestion et configuration des timers
 *	Auteurs : Julien Franchineau & François Even
 *	Version 20111206
 */

#ifndef QS_TIMER_H
	#define QS_TIMER_H

	#include "QS_all.h"

	void TIMER_init(void);
	/*	Initialise les timers, l'interruption du timer1 est
	 *	prioritaire sur celle du timer2, qui est prioritaire
	 *	sur celle du timer3...
	 */

	/*	TIMER1 utilise l'interuption 
	 *	void _ISR _T1Interrupt()
	 *	et le flag d'IT IFS0bits.T1IF
	 */
	void TIMER1_run(Uint8 period /* en millisecondes */);
	void TIMER1_run_us (Uint16 period /* en microsecondes */);
		/* max 65535 / FREQ_DHORLOGE (en MHz) */
	void TIMER1_stop(void);

	/*	TIMER2 utilise l'interuption 
	 *	void _ISR _T2Interrupt()
	 *	et le flag d'IT IFS0bits.T2IF
	 */
	void TIMER2_run(Uint8 period /* en millisecondes */);
	void TIMER2_stop(void);
	void TIMER2_run_us (Uint16 period /* en microsecondes */);
		/* max 65535 / FREQ_DHORLOGE (en MHz) */

	/*	TIMER3 utilise l'interuption 
	 *	void _ISR _T3Interrupt()
	 *	et le flag d'IT IFS0bits.T3IF
	 */
	void TIMER3_run(Uint8 period /* en millisecondes */);
	void TIMER3_stop(void);
	void TIMER3_run_us (Uint16 period /* en microsecondes */);
		/* max 65535 / FREQ_DHORLOGE (en MHz) */

	/*	TIMER4 utilise l'interuption 
	 *	void _ISR _T4Interrupt()
	 *	et le flag d'IT IFS1bits.T4IF
	 */
	void TIMER4_run(Uint16 period /* en millisecondes */);
	/* read ne renvoie peut etre pas le bon resultat */
	Uint16 TIMER4_read(); /* retour en ms*/
	void TIMER4_stop(void);

	// Fonctions supplémentaires qui devraient être dans <timer.h>
	#define T1_INT_PRIOR_5 	5
	#define T1_INT_OFF		0
	Uint16 ConfigIntTimer1(Uint16 unknow_flag);

	Uint16 DisableIntT1();
	Uint16 EnableIntT1();
#endif
