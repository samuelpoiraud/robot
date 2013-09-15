/*
 *  Club Robot ESEO 2008 - 2009
 *  Archi-Tech'
 *
 *  Fichier : QS_timer.h
 *  Package : Qualité Soft
 *  Description : Gestion et configuration des timer
 *  Auteur : Jacen
 *	Licence : CeCILL-C (voir LICENCE.txt)
 *  Version 20100620
 */

#include "QS_timer.h"

#include <timer.h>

//Les définitions du nombre de pulse par ms ou us est dans le .h maintenant.

void TIMER_init()
{
	static bool_e initialized = FALSE;
	if (!initialized)
	{
		/* les interruptions sont activées au lancement du timer */
		ConfigIntTimer1(T1_INT_PRIOR_6 & T1_INT_OFF);
		ConfigIntTimer2(T2_INT_PRIOR_3 & T2_INT_OFF);
		ConfigIntTimer3(T3_INT_PRIOR_2 & T3_INT_OFF);
		ConfigIntTimer45(T5_INT_PRIOR_1 & T5_INT_OFF);
		/* Timer45 est un timer 32 bits composé des timers 4 et 5 */
		initialized = TRUE;
	}	
}

void TIMER1_stop(void)
{	
	DisableIntT1;
	WriteTimer1(0);
	CloseTimer1();
}

void TIMER1_run(Uint8 period /* en millisecondes */)
{	
	OpenTimer1(
			T1_ON &					/* Timer1 ON */ 
			T1_IDLE_CON &			/* operate during sleep */
			T1_GATE_OFF &			/* Timer Gate time accumulation disabled */
			T1_PS_1_256 &			/* Prescaler 1:256 */
			T1_SYNC_EXT_OFF &		/* Do not synch external clk input */
			T1_SOURCE_INT,			/* Internal clock source */
			TIMER_PULSE_PER_MS * period);	/* periode en ms */
	EnableIntT1;
}

void TIMER1_run_us (Uint16 period /* en microsecondes */)
{	
	OpenTimer1(
			T1_ON &					/* Timer1 ON */ 
			T1_IDLE_CON &			/* operate during sleep */
			T1_GATE_OFF &			/* Timer Gate time accumulation disabled */
			T1_PS_1_1 &				/* Prescaler 1:1 */
			T1_SYNC_EXT_OFF &		/* Do not synch external clk input */
			T1_SOURCE_INT,			/* Internal clock source */
			TIMER_PULSE_PER_US * period);	/* periode en ms */
	EnableIntT1;
}

void TIMER2_stop(void)
{	
	DisableIntT2;
	WriteTimer2(0);
	CloseTimer2();
}

void TIMER2_run(Uint8 period /* en millisecondes */)
{
	OpenTimer2(
			T2_ON &					/* Timer2 ON */ 
			T2_IDLE_CON &			/* operate during sleep */
			T2_GATE_OFF &			/* Timer Gate time accumulation disabled */
			T2_PS_1_256 &			/* Prescaler 1:256 */
			T2_32BIT_MODE_OFF &		/* Timer 2 and Timer 3 form are 2 16 bit Timers */
			T2_SOURCE_INT,			/* Internal clock source */
			TIMER_PULSE_PER_MS * period);	/* periode en ms */
	EnableIntT2;
}

void TIMER2_run_us(Uint16 period /* en microsecondes */)
{
	OpenTimer2(
			T2_ON &					/* Timer2 ON */ 
			T2_IDLE_CON &			/* operate during sleep */
			T2_GATE_OFF &			/* Timer Gate time accumulation disabled */
			T2_PS_1_1 &				/* Prescaler 1:1 */
			T2_32BIT_MODE_OFF &		/* Timer 2 and Timer 3 form are 2 16 bit Timers */
			T2_SOURCE_INT,			/* Internal clock source */
			TIMER_PULSE_PER_US * period);	/* periode en ms */
	EnableIntT2;
}

void TIMER3_stop(void)
{	
	DisableIntT3;
	WriteTimer3(0);
	CloseTimer3();
}

void TIMER3_run(Uint8 period /* en millisecondes */)
{
	OpenTimer3(
			T3_ON &					/* Timer3 ON */ 
			T3_IDLE_CON &			/* operate during sleep */
			T3_GATE_OFF &			/* Timer Gate time accumulation disabled */
			T3_PS_1_256 &			/* Prescaler 1:256 */
			T3_SOURCE_INT,			/* Internal clock source */
			TIMER_PULSE_PER_MS * period);	/* periode en us */
	EnableIntT3;
}
void TIMER3_run_us(Uint16 period /* en microsecondes */)
{
	OpenTimer3(
			T3_ON &					/* Timer3 ON */ 
			T3_IDLE_CON &			/* operate during sleep */
			T3_GATE_OFF &			/* Timer Gate time accumulation disabled */
			T3_PS_1_1 &				/* Prescaler 1:1 */
			T3_SOURCE_INT,			/* Internal clock source */
			TIMER_PULSE_PER_US * period);	/* periode en us */
	EnableIntT3;
}

void TIMER4_stop(void)
{	
	/* Il s'agit en fait de la combinaison des Timers 4 et 5 */
	DisableIntT4;
	WriteTimer45(0);
	CloseTimer45();
}

void TIMER4_run(Uint16 period /* en millisecondes */)
{
	/* Il s'agit en fait de la combinaison des Timers 4 et 5 */
	OpenTimer4(
			T4_ON &					/* Timer4 ON */ 
			T4_IDLE_CON &			/* operate during sleep */
			T4_GATE_OFF &			/* Timer Gate time accumulation disabled */
			T4_PS_1_256 &			/* Prescaler 1:256 */
			T4_SOURCE_INT &			/* Internal clock source */
			T4_32BIT_MODE_ON,		/* Timer 4 and Timer 5 form a 32 bit Timer */
			TIMER_PULSE_PER_MS * period);	/* periode en ms */
	EnableIntT4;
}

Uint16 TIMER4_read() /* retour en ms*/
{
	return TMR4/TIMER_PULSE_PER_MS;
}
