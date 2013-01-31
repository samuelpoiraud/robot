/*
 *  Club Robot ESEO 2009 - 2010
 *  Chomp
 *
 *  Fichier : QS_timer.h
 *  Package : Qualité Soft
 *  Description : Timers
 *  Auteur : Gwenn
 *  Version 20100424
 */

#ifndef QS_TIMER_H
	#define QS_TIMER_H
	
	#include "QS_all.h"
	#include "QS_sys.h"
	
	/* Choix de l'unité */
	#define UNIT_MS 2
	#define UNIT_US UNIT_MS / 1024
	
	/** Initialise les timers **/
	void TIMER_init(void);
	
	/** Lance TIMx pour une certaine durée
		* Appelle l'IRQH 	void TIMx_IRQHandler(void)
		* A la fin de l'IRQH: TIM_ClearITPendingBit(TIMx, TIM_IT_Update)
		* param period Durée en ms avant la levée de l'interruption - MAX 32s
	**/
	void TIMER2_run(Uint16 period);
	void TIMER2_run_fine(Uint16 period);
	void TIMER2_stop(void);
	
	void TIMER5_run(Uint16 period);
	void TIMER5_run_fine(Uint16 period);
	void TIMER5_stop(void);
	
	void TIMER6_run(Uint16 period);
	void TIMER6_run_fine(Uint16 period);
	void TIMER6_stop(void);
	
	void TIMER7_run(Uint16 period);
	void TIMER7_run_fine(Uint16 period);
	void TIMER7_stop(void);	
	
	
#endif /* ndef QS_TIMER_H */
