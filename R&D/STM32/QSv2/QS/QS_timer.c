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

//Interruptions
__attribute__((weak)) void _T1Interrupt() {}
__attribute__((weak)) void _T2Interrupt() {}
__attribute__((weak)) void _T3Interrupt() {}
__attribute__((weak)) void _T4Interrupt() {}

/* Configuation de l'ensemble du bloc timer */
void TIMER_init(void){
	static bool_e initialized = FALSE;
	if (initialized) return;

	/* Horloges */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM11, ENABLE);
	RCC_APB1PeriphClockCmd(
		  RCC_APB1Periph_TIM12
		| RCC_APB1Periph_TIM13
		| RCC_APB1Periph_TIM14, ENABLE);

	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Period = 0xFFFF;	//Le moins rapide par défaut
	TIM_TimeBaseStructure.TIM_Prescaler = 0xFFFF;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;

	//On s'arrange pour que la clock sur TIM11 soit la même que sur TIM12-14, voir manuel de réference STM32F4xx page 114
#if (PCLK2_FREQUENCY_HZ/PCLK1_FREQUENCY_HZ) == 1
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
#elif (PCLK2_FREQUENCY_HZ/PCLK1_FREQUENCY_HZ) == 2
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
#elif (PCLK2_FREQUENCY_HZ/PCLK1_FREQUENCY_HZ) == 4
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV2;
#elif (PCLK2_FREQUENCY_HZ/PCLK1_FREQUENCY_HZ) == 8
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV4;
#else
#error "Incorrect HCLK/PCLK1 ratio, must be 1, 2, 4 or 8"
#endif
	TIM_TimeBaseInit(TIM11, &TIM_TimeBaseStructure);

	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInit(TIM12, &TIM_TimeBaseStructure);
	TIM_TimeBaseInit(TIM13, &TIM_TimeBaseStructure);
	TIM_TimeBaseInit(TIM14, &TIM_TimeBaseStructure);

	/* NVIC: initialisation des IT timer avec les niveaux de priorité adéquats*/
	NVIC_InitTypeDef NVICInit;
	NVICInit.NVIC_IRQChannelCmd = ENABLE;
	NVICInit.NVIC_IRQChannelSubPriority = 0;

	NVICInit.NVIC_IRQChannelPreemptionPriority = 1;
	NVICInit.NVIC_IRQChannel = TIM1_TRG_COM_TIM11_IRQn;
	NVIC_Init(&NVICInit);

	NVICInit.NVIC_IRQChannelPreemptionPriority = 5;
	NVICInit.NVIC_IRQChannel = TIM8_BRK_TIM12_IRQn;
	NVIC_Init(&NVICInit);

	NVICInit.NVIC_IRQChannelPreemptionPriority = 6;
	NVICInit.NVIC_IRQChannel = TIM8_UP_TIM13_IRQn;
	NVIC_Init(&NVICInit);

	NVICInit.NVIC_IRQChannelPreemptionPriority = 7;
	NVICInit.NVIC_IRQChannel = TIM8_TRG_COM_TIM14_IRQn;
	NVIC_Init(&NVICInit);
	
	TIM_ITConfig(TIM11, TIM_IT_Update, ENABLE);
	TIM_ITConfig(TIM12, TIM_IT_Update, ENABLE);
	TIM_ITConfig(TIM13, TIM_IT_Update, ENABLE);
	TIM_ITConfig(TIM14, TIM_IT_Update, ENABLE);

	/* Fin de la configuration */
	initialized = TRUE;
}

void TIMER1_run(Uint8 period /* en millisecondes */) {
	TIM_PrescalerConfig(TIM11, TIMER_MS_PRESCALER, TIM_PSCReloadMode_Immediate);
	TIM_SetAutoreload(TIM11,  ((Uint16)period) * TIMER_PULSE_PER_MS);
	TIM_Cmd(TIM11, ENABLE);
}

void TIMER1_run_us (Uint16 period /* en microsecondes */) {
	TIM_PrescalerConfig(TIM11, TIMER_US_PRESCALER, TIM_PSCReloadMode_Immediate);
	TIM_SetAutoreload(TIM11,  period * TIMER_PULSE_PER_US);
	TIM_Cmd(TIM11, ENABLE);
}

void TIMER1_stop(void) {
	TIM_Cmd(TIM11, DISABLE);
}

void TIMER2_run(Uint8 period /* en millisecondes */) {
	TIM_PrescalerConfig(TIM12, TIMER_MS_PRESCALER, TIM_PSCReloadMode_Immediate);
	TIM_SetAutoreload(TIM12,  ((Uint16)period) * TIMER_PULSE_PER_MS);
	TIM_Cmd(TIM12, ENABLE);
}

void TIMER2_run_us (Uint16 period /* en microsecondes */) {
	TIM_PrescalerConfig(TIM12, TIMER_US_PRESCALER, TIM_PSCReloadMode_Immediate);
	TIM_SetAutoreload(TIM12,  period * TIMER_PULSE_PER_US);
	TIM_Cmd(TIM12, ENABLE);
}

void TIMER2_stop(void) {
	TIM_Cmd(TIM12, DISABLE);
}

void TIMER3_run(Uint8 period /* en millisecondes */) {
	TIM_PrescalerConfig(TIM13, TIMER_MS_PRESCALER, TIM_PSCReloadMode_Immediate);
	TIM_SetAutoreload(TIM13,  ((Uint16)period) * TIMER_PULSE_PER_MS);
	TIM_Cmd(TIM13, ENABLE);
}

void TIMER3_run_us (Uint16 period /* en microsecondes */) {
	TIM_PrescalerConfig(TIM13, TIMER_US_PRESCALER, TIM_PSCReloadMode_Immediate);
	TIM_SetAutoreload(TIM13,  period * TIMER_PULSE_PER_US);
	TIM_Cmd(TIM13, ENABLE);
}

void TIMER3_stop(void) {
	TIM_Cmd(TIM13, DISABLE);
}

void TIMER4_run(Uint8 period /* en millisecondes */) {
	TIM_PrescalerConfig(TIM14, TIMER_MS_PRESCALER, TIM_PSCReloadMode_Immediate);
	TIM_SetAutoreload(TIM14,  ((Uint16)period) * TIMER_PULSE_PER_MS);
	TIM_Cmd(TIM14, ENABLE);
}

void TIMER4_run_us(Uint16 period /* en microsecondes */) {
	TIM_PrescalerConfig(TIM14, TIMER_US_PRESCALER, TIM_PSCReloadMode_Immediate);
	TIM_SetAutoreload(TIM14,  period * TIMER_PULSE_PER_US);
	TIM_Cmd(TIM14, ENABLE);
}

void TIMER4_stop(void) {
	TIM_Cmd(TIM14, DISABLE);
}

//Interrupts management and redirection
void TIM1_TRG_COM_TIM11_IRQHandler() {
	if(TIM_GetITStatus(TIM11, TIM_IT_Update))
		_T1Interrupt();
}

void TIM8_BRK_TIM12_IRQHandler() {
	if(TIM_GetITStatus(TIM12, TIM_IT_Update))
		_T2Interrupt();
}

void TIM8_UP_TIM13_IRQHandler() {
	if(TIM_GetITStatus(TIM13, TIM_IT_Update))
		_T3Interrupt();
}

void TIM8_TRG_COM_TIM14_IRQHandler() {
	if(TIM_GetITStatus(TIM14, TIM_IT_Update))
		_T4Interrupt();
}
