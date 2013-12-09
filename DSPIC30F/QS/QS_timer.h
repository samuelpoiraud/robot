/*
 *  Club Robot ESEO 2008 - 2009
 *  Archi-Tech'
 *
 *  Fichier : QS_timer.h
 *  Package : Qualité Soft
 *  Description : Header de la gestion des timers
 *  Auteur : Jacen
 *	Licence : CeCILL-C (voir LICENCE.txt)
 *  Version 20100620
 */

#ifndef QS_TIMER_H
	#define QS_TIMER_H

	#include "QS_all.h"
	#include <timer.h>

	//Définitions faites ici pour permettre l'utilisation de ces valeurs hors de ce module. Il est parfois nécessaire d'avoir ces nombres pour d'autre module.
	//En étant ici, les définitions restent centralisés.
	//TIMER_ a été ajouté au nom pour éviter de polluer l'espace de noms global, car l'écologie c'est le bien.
	#ifdef FREQ_10MHZ
		#define TIMER_PULSE_PER_MS	39.0625
		#define TIMER_PULSE_PER_US	10
	#elif defined (FREQ_20MHZ)
		#define TIMER_PULSE_PER_MS	78.125
		#define TIMER_PULSE_PER_US	20
	#elif defined (FREQ_INTERNAL_CLK)
		#define TIMER_PULSE_PER_MS	30.2342
		#define TIMER_PULSE_PER_US	7.74
	#else //40MHz
		#define TIMER_PULSE_PER_MS	156.25
		#define TIMER_PULSE_PER_US	40
	#endif /* def FREQ_XXMHZ */

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
	#define TIMER1_disableInt() DisableIntT1
	#define TIMER1_enableInt()  EnableIntT1
	#define TIMER1_getCounter() TMR1
	#define TIMER1_setCounter(newValue) (TMR1 = newValue)

	/*	TIMER2 utilise l'interuption 
	 *	void _ISR _T2Interrupt()
	 *	et le flag d'IT IFS0bits.T2IF
	 */
	void TIMER2_run(Uint8 period /* en millisecondes */);
	void TIMER2_stop(void);
	void TIMER2_run_us (Uint16 period /* en microsecondes */);
		/* max 65535 / FREQ_DHORLOGE (en MHz) */
	#define TIMER2_disableInt() DisableIntT2
	#define TIMER2_enableInt()  EnableIntT2
	#define TIMER2_getCounter() TMR2
	#define TIMER2_setCounter(newValue) (TMR2 = newValue)

	/*	TIMER3 utilise l'interuption 
	 *	void _ISR _T3Interrupt()
	 *	et le flag d'IT IFS0bits.T3IF
	 */
	void TIMER3_run(Uint8 period /* en millisecondes */);
	void TIMER3_stop(void);
	void TIMER3_run_us (Uint16 period /* en microsecondes */);
		/* max 65535 / FREQ_DHORLOGE (en MHz) */
	#define TIMER3_disableInt() DisableIntT3
	#define TIMER3_enableInt()  EnableIntT3
	#define TIMER3_getCounter() TMR3
	#define TIMER3_setCounter(newValue) (TMR3 = newValue)

	/*	TIMER4 utilise l'interuption 
	 *	void _ISR _T4Interrupt()
	 *	et le flag d'IT IFS1bits.T4IF
	 */
	void TIMER4_run(Uint16 period /* en millisecondes */);
	/* read ne renvoie peut etre pas le bon resultat */
	Uint16 TIMER4_read(); /* retour en ms*/
	void TIMER4_stop(void);
	#define TIMER4_disableInt() DisableIntT4
	#define TIMER4_enableInt()  EnableIntT4
	#define TIMER4_getCounter() TMR4
	#define TIMER4_setCounter(newValue) (TMR4 = newValue)


	//Acuittement des IT timer
	#define TIMER1_AckIT() (IFS0bits.T1IF = 0)
	#define TIMER2_AckIT() (IFS0bits.T2IF = 0)
	#define TIMER3_AckIT() (IFS0bits.T3IF = 0)
	#define TIMER4_AckIT() (IFS1bits.T4IF = 0)

	//Verification de l'état d'une IT, pour detecter un eventuel recouvrement IT
	#define TIMER1_getITStatus() (IFS0bits.T1IF)
	#define TIMER2_getITStatus() (IFS0bits.T2IF)
	#define TIMER3_getITStatus() (IFS0bits.T3IF)
	#define TIMER4_getITStatus() (IFS1bits.T4IF)

#endif
