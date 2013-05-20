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

	//Définitions faites ici pour permettre l'utilisation de ces valeurs hors de ce module. Il est parfois nécessaire d'avoir ces nombres pour d'autre module.
	//En étant ici, les définitions restent centralisés.
	//TIMER_ a été ajouté au nom pour éviter de polluer l'espace de noms global, car l'écologie c'est le bien.

	#define TIMER_MS_PRESCALER  (PCLK1_FREQUENCY_HZ / ((65536 / 255) * 1000))
	#define TIMER_US_PRESCALER  (PCLK1_FREQUENCY_HZ / (1000000))
	#define TIMER_PULSE_PER_MS	(PCLK1_FREQUENCY_HZ / (1000 * TIMER_MS_PRESCALER))
	#define TIMER_PULSE_PER_US	(PCLK1_FREQUENCY_HZ / (1000000 * TIMER_US_PRESCALER))

	/*	Initialise les timers, l'interruption du timer1 est
	 *	prioritaire sur celle du timer2, qui est prioritaire
	 *	sur celle du timer3...
	 */
	void TIMER_init(void);


	/*	TIMER1 utilise l'interuption
	 *	void _ISR _T1Interrupt()
	 *	et le flag d'IT IFS0bits.T1IF
	 */
	void TIMER1_run(Uint8 period /* en millisecondes */);
	void TIMER1_run_us (Uint16 period /* en microsecondes */);
	void TIMER1_stop(void);

	/*	TIMER2 utilise l'interuption
	 *	void _ISR _T2Interrupt()
	 *	et le flag d'IT IFS0bits.T2IF
	 */
	void TIMER2_run(Uint8 period /* en millisecondes */);
	void TIMER2_stop(void);
	void TIMER2_run_us (Uint16 period /* en microsecondes */);

	/*	TIMER3 utilise l'interuption
	 *	void _ISR _T3Interrupt()
	 *	et le flag d'IT IFS0bits.T3IF
	 */
	void TIMER3_run(Uint8 period /* en millisecondes */);
	void TIMER3_stop(void);
	void TIMER3_run_us (Uint16 period /* en microsecondes */);

	/*	TIMER4 utilise l'interuption
	 *	void _ISR _T4Interrupt()
	 *	et le flag d'IT IFS1bits.T4IF
	 */
	void TIMER4_run(Uint8 period /* en millisecondes */);
	void TIMER4_stop(void);
	void TIMER4_run_us (Uint16 period /* en microsecondes */);

	//Acuittement des IT timer
	#define TIMER1_AckIT() TIM_ClearITPendingBit(TIM1, TIM_IT_Update)
	#define TIMER2_AckIT() TIM_ClearITPendingBit(TIM2, TIM_IT_Update)
	#define TIMER3_AckIT() TIM_ClearITPendingBit(TIM3, TIM_IT_Update)
	#define TIMER4_AckIT() TIM_ClearITPendingBit(TIM4, TIM_IT_Update)

#endif
