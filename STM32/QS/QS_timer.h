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
	#include "stm32f4xx_tim.h"

	//Définitions faites ici pour permettre l'utilisation de ces valeurs hors de ce module. Il est parfois nécessaire d'avoir ces nombres pour d'autre module.
	//En étant ici, les définitions restent centralisés.
	//TIMER_ a été ajouté au nom pour éviter de polluer l'espace de noms global, car l'écologie c'est le bien.

	#define TIMER_MS_PRESCALER  (PCLK1_FREQUENCY_HZ / ((65536 / 257) * 1000))
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
	void TIMER1_disableInt();
	void TIMER1_enableInt();
	int TIMER1_getCounter();

	/*	TIMER2 utilise l'interuption
	 *	void _ISR _T2Interrupt()
	 *	et le flag d'IT IFS0bits.T2IF
	 */
	void TIMER2_run(Uint8 period /* en millisecondes */);
	void TIMER2_stop(void);
	void TIMER2_run_us (Uint16 period /* en microsecondes */);
	void TIMER2_disableInt();
	void TIMER2_enableInt();
	int TIMER2_getCounter();

	/*	TIMER3 utilise l'interuption
	 *	void _ISR _T3Interrupt()
	 *	et le flag d'IT IFS0bits.T3IF
	 */
	void TIMER3_run(Uint8 period /* en millisecondes */);
	void TIMER3_stop(void);
	void TIMER3_run_us (Uint16 period /* en microsecondes */);
	void TIMER3_disableInt();
	void TIMER3_enableInt();
	int TIMER3_getCounter();

	/*	TIMER4 utilise l'interuption
	 *	void _ISR _T4Interrupt()
	 *	et le flag d'IT IFS1bits.T4IF
	 */
	void TIMER4_run(Uint8 period /* en millisecondes */);
	void TIMER4_stop(void);
	void TIMER4_run_us (Uint16 period /* en microsecondes */);
	void TIMER4_disableInt();
	void TIMER4_enableInt();
	int TIMER4_getCounter();

	//Acuittement des IT timer
	#define TIMER1_AckIT() TIM_ClearITPendingBit(TIM11, TIM_IT_Update)
	#define TIMER2_AckIT() TIM_ClearITPendingBit(TIM12, TIM_IT_Update)
	#define TIMER3_AckIT() TIM_ClearITPendingBit(TIM13, TIM_IT_Update)
	#define TIMER4_AckIT() TIM_ClearITPendingBit(TIM14, TIM_IT_Update)

	//Verification de l'état d'une IT, pour detecter un eventuel recouvrement IT
	#define TIMER1_getITStatus() TIM_GetITStatus(TIM11, TIM_IT_Update)
	#define TIMER2_getITStatus() TIM_GetITStatus(TIM12, TIM_IT_Update)
	#define TIMER3_getITStatus() TIM_GetITStatus(TIM13, TIM_IT_Update)
	#define TIMER4_getITStatus() TIM_GetITStatus(TIM14, TIM_IT_Update)

#endif
