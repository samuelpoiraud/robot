/*
 *  Club Robot ESEO 2009 - 2010
 *  Chomp
 *
 *  Fichier : QS_timer.c
 *  Package : Qualité Soft
 *  Description : Timers
 *  Auteur : Gwenn
 *  Version 20100424
 */


#include "QS_timer.h"


/** Sécurité d'initialisation **/
static bool_e initialized = FALSE;


/** Lance le timer - fonction complète
	* param TIMx nom du timer
	* param period Temps à donner au timer
	* param unit Multiplicateur pour ms ou µs: UNIT_MS ou UNIT_US
**/
void TIMER_run_unit(TIM_TypeDef* TIMx, Uint16 period, Uint16 unit);


/** Configure une structure de réglage timer
	* param Tinit Structure à régler
	* param period Temps à donner au timer
	* param unit Multiplicateur pour ms ou µs: UNIT_MS ou UNIT_US
**/
void TIMER_set(TIM_TimeBaseInitTypeDef* TInit, Uint16 period, Uint16 unit);



/* Configuation de l'ensemble du bloc timer */
void TIMER_init(void){
	if (initialized) return;

	/* Horloges */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
	RCC_APB1PeriphClockCmd(
		  RCC_APB1Periph_TIM2
		| RCC_APB1Periph_TIM3
		| RCC_APB1Periph_TIM4
		| RCC_APB1Periph_TIM5
		| RCC_APB1Periph_TIM6
		| RCC_APB1Periph_TIM7, ENABLE);
	
	/* NVIC: initialisation des IT timer avec les niveaux de priorité adéquats*/
	NVIC_InitTypeDef NVICInit;
	NVICInit.NVIC_IRQChannelCmd = ENABLE;
	NVICInit.NVIC_IRQChannelSubPriority = 0;

	NVICInit.NVIC_IRQChannelPreemptionPriority = 7;
	NVICInit.NVIC_IRQChannel = TIM6_IRQn;
	NVIC_Init(&NVICInit);

	NVICInit.NVIC_IRQChannelPreemptionPriority = 6;
	NVICInit.NVIC_IRQChannel = TIM7_IRQn;
	NVIC_Init(&NVICInit);
	
	NVICInit.NVIC_IRQChannelPreemptionPriority = 5;
	NVICInit.NVIC_IRQChannel = TIM5_IRQn;
	NVIC_Init(&NVICInit);
	
	NVICInit.NVIC_IRQChannelPreemptionPriority = 1;
	NVICInit.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_Init(&NVICInit);
	
	/* Fin de la configuration */
	initialized = TRUE;
}


void TIMER_run_unit(TIM_TypeDef* TIMx, Uint16 period, Uint16 unit){
	
	//TODO assert(initialized);
	
	TIM_TimeBaseInitTypeDef TInit;
	TIMER_set (&TInit, period, unit);
	
	TIM_TimeBaseInit(TIMx, &TInit);
	TIM_ITConfig(TIMx, TIM_IT_Update, ENABLE);
	TIM_Cmd(TIMx, ENABLE);
}


/* Calcul du diviseur et de la période */
void TIMER_set(TIM_TimeBaseInitTypeDef* TInit, Uint16 period, Uint16 unit){
	TIM_TimeBaseStructInit(TInit);
	TInit->TIM_Period = period * unit;
	TInit->TIM_Prescaler = FREQUENCY << 9;
	TInit->TIM_ClockDivision = TIM_CKD_DIV4;
	TInit->TIM_CounterMode = TIM_CounterMode_Up;
}




/* Opérations de manipulation des timer */

/* TIMER2 */
void TIMER2_run(Uint16 period){
	TIMER_run_unit(TIM2, period, UNIT_MS);
}
void TIMER2_run_fine(Uint16 period){
	TIMER_run_unit(TIM2, period, UNIT_US);
}
void TIMER2_stop(void){
	TIM_Cmd(TIM2, DISABLE);
}

/* TIMER5 */
void TIMER5_run(Uint16 period){
	TIMER_run_unit(TIM5, period, UNIT_MS);
}
void TIMER5_run_fine(Uint16 period){
	TIMER_run_unit(TIM5, period, UNIT_US);
}
void TIMER5_stop(void){
	TIM_Cmd(TIM5, DISABLE);
}

/* TIMER6 */
void TIMER6_run(Uint16 period){
	TIMER_run_unit(TIM6, period, UNIT_MS);
}
void TIMER6_run_fine(Uint16 period){
	TIMER_run_unit(TIM6, period, UNIT_US);
}
void TIMER6_stop(void){
	TIM_Cmd(TIM6, DISABLE);
}

/* TIMER7 */
void TIMER7_run(Uint16 period){
	TIMER_run_unit(TIM7, period, UNIT_MS);
}
void TIMER7_run_fine(Uint16 period){
	TIMER_run_unit(TIM7, period, UNIT_US);
}
void TIMER7_stop(void){
	TIM_Cmd(TIM7, DISABLE);
}

