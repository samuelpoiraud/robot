/*
 *  Club Robot ESEO 2008 - 2009
 *  Archi-Tech'
 *
 *  Fichier : QS_timer.h
 *  Package : Qualit� Soft
 *  Description : Header de la gestion des timers
 *  Auteur : Jacen
 *	Licence : CeCILL-C (voir LICENCE.txt)
 *  Version 20100620
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

#endif
