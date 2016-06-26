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

/** ----------------  Defines possibles  --------------------
 *	TIMER_1_PRIORITY			: Priorité du timer 1
 *	TIMER_2_PRIORITY			: Priorité du timer 2
 *	TIMER_3_PRIORITY			: Priorité du timer 3
 *	TIMER_4_PRIORITY			: Priorité du timer 4
 *	TIMER_5_PRIORITY			: Priorité du timer 5
 *
 * ----------------  Choses à savoir  --------------------
 * Plus la priorité est basse plus elle est prioritaire
 * Les priorité vont de 0 à 255
 */

#ifndef QS_TIMER_H
	#define QS_TIMER_H

	#include "QS_all.h"
	#include "../stm32f4xx_hal/stm32f4xx_hal_tim.h"

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

	/*	TIMER5 utilise l'interuption
	 *	void _ISR _T5Interrupt()
	 *	et le flag d'IT .....
	 */
	void TIMER5_run(Uint8 period /* en millisecondes */);
	void TIMER5_stop(void);
	void TIMER5_run_us (Uint16 period /* en microsecondes */);
	void TIMER5_disableInt();
	void TIMER5_enableInt();
	int TIMER5_getCounter();

	//Acuittement des IT timer
	void TIMER1_AckIT();
	void TIMER2_AckIT();
	void TIMER3_AckIT();
	void TIMER4_AckIT();
	void TIMER5_AckIT();

	//Verification de l'état d'une IT, pour detecter un eventuel recouvrement IT
	void TIMER1_getITStatus();
	void TIMER2_getITStatus();
	void TIMER3_getITStatus();
	void TIMER4_getITStatus();
	void TIMER5_getITStatus();

#endif
