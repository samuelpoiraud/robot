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

#if TIMER_MS_PRESCALER <= 0 || TIMER_MS_PRESCALER > 65535
#error "TIMER_MS_PRESCALER is not > 0 or < 65536"
#endif
#if TIMER_US_PRESCALER <= 0 || TIMER_US_PRESCALER > 65535
#error "TIMER_US_PRESCALER is not > 0 or < 65536"
#endif
#if TIMER_PULSE_PER_MS <= 0
#error "TIMER_PULSE_PER_MS is <= 0"
#endif
#if TIMER_PULSE_PER_US <= 0
#error "TIMER_PULSE_PER_US is <= 0"
#endif


/* Configuation de l'ensemble du bloc timer */
void TIMER_init(void){
	static bool_e initialized = FALSE;
	if (initialized) return;

	/* Horloges */
	RCC_AHB1PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
	RCC_APB1PeriphClockCmd(
		  RCC_APB1Periph_TIM2
		| RCC_APB1Periph_TIM3
		| RCC_APB1Periph_TIM4, ENABLE);

	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Period = 0xFFFF;	//Le moins rapide par défaut
	TIM_TimeBaseStructure.TIM_Prescaler = 0xFFFF;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;

	//On s'arrange pour que la clock sur TIM1 soit la même que sur TIM2-4, voir manuel de réference STM32F4xx page 114
#if (HCLK_FREQUENCY_HZ/PCLK1_FREQUENCY_HZ) == 1
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
#elif (HCLK_FREQUENCY_HZ/PCLK1_FREQUENCY_HZ) == 2
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
#elif (HCLK_FREQUENCY_HZ/PCLK1_FREQUENCY_HZ) == 4
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV2;
#elif (HCLK_FREQUENCY_HZ/PCLK1_FREQUENCY_HZ) == 8
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV4;
#else
#error "Incorrect HCLK/PCLK1 ratio, must be 1, 2, 4 or 8"
#endif
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

	/* NVIC: initialisation des IT timer avec les niveaux de priorité adéquats*/
	NVIC_InitTypeDef NVICInit;
	NVICInit.NVIC_IRQChannelCmd = ENABLE;
	NVICInit.NVIC_IRQChannelSubPriority = 0;

	NVICInit.NVIC_IRQChannelPreemptionPriority = 7;
	NVICInit.NVIC_IRQChannel = TIM4_IRQn;
	NVIC_Init(&NVICInit);

	NVICInit.NVIC_IRQChannelPreemptionPriority = 6;
	NVICInit.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_Init(&NVICInit);
	
	NVICInit.NVIC_IRQChannelPreemptionPriority = 5;
	NVICInit.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_Init(&NVICInit);
	
	NVICInit.NVIC_IRQChannelPreemptionPriority = 1;
	NVICInit.NVIC_IRQChannel = TIM1_UP_TIM10_IRQn;
	NVIC_Init(&NVICInit);
	
	TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);

	/* Fin de la configuration */
	initialized = TRUE;
}

void TIMER1_run(Uint8 period /* en millisecondes */) {
	TIM_PrescalerConfig(TIM1, TIMER_MS_PRESCALER, TIM_PSCReloadMode_Immediate);
	TIM_SetAutoreload(TIM1,  ((Uint16)period) * TIMER_PULSE_PER_MS);
	TIM_Cmd(TIM1, ENABLE);
}

void TIMER1_run_us (Uint16 period /* en microsecondes */) {
	TIM_PrescalerConfig(TIM1, TIMER_US_PRESCALER, TIM_PSCReloadMode_Immediate);
	TIM_SetAutoreload(TIM1,  period * TIMER_PULSE_PER_US);
	TIM_Cmd(TIM1, ENABLE);
}

void TIMER1_stop(void) {
	TIM_Cmd(TIM1, DISABLE);
}

void TIMER2_run(Uint8 period /* en millisecondes */) {
	TIM_PrescalerConfig(TIM2, TIMER_MS_PRESCALER, TIM_PSCReloadMode_Immediate);
	TIM_SetAutoreload(TIM2,  ((Uint16)period) * TIMER_PULSE_PER_MS);
	TIM_Cmd(TIM2, ENABLE);
}

void TIMER2_run_us (Uint16 period /* en microsecondes */) {
	TIM_PrescalerConfig(TIM2, TIMER_US_PRESCALER, TIM_PSCReloadMode_Immediate);
	TIM_SetAutoreload(TIM2,  period * TIMER_PULSE_PER_US);
	TIM_Cmd(TIM2, ENABLE);
}

void TIMER2_stop(void) {
	TIM_Cmd(TIM2, DISABLE);
}

void TIMER3_run(Uint8 period /* en millisecondes */) {
	TIM_PrescalerConfig(TIM3, TIMER_MS_PRESCALER, TIM_PSCReloadMode_Immediate);
	TIM_SetAutoreload(TIM3,  ((Uint16)period) * TIMER_PULSE_PER_MS);
	TIM_Cmd(TIM3, ENABLE);
}

void TIMER3_run_us (Uint16 period /* en microsecondes */) {
	TIM_PrescalerConfig(TIM3, TIMER_US_PRESCALER, TIM_PSCReloadMode_Immediate);
	TIM_SetAutoreload(TIM3,  period * TIMER_PULSE_PER_US);
	TIM_Cmd(TIM3, ENABLE);
}

void TIMER3_stop(void) {
	TIM_Cmd(TIM3, DISABLE);
}

void TIMER4_run(Uint8 period /* en millisecondes */) {
	TIM_PrescalerConfig(TIM4, TIMER_MS_PRESCALER, TIM_PSCReloadMode_Immediate);
	TIM_SetAutoreload(TIM4,  ((Uint16)period) * TIMER_PULSE_PER_MS);
	TIM_Cmd(TIM4, ENABLE);
}

void TIMER4_run_us(Uint16 period /* en microsecondes */) {
	TIM_PrescalerConfig(TIM4, TIMER_US_PRESCALER, TIM_PSCReloadMode_Immediate);
	TIM_SetAutoreload(TIM4,  period * TIMER_PULSE_PER_US);
	TIM_Cmd(TIM4, ENABLE);
}

void TIMER4_stop(void) {
	TIM_Cmd(TIM4, DISABLE);
}
