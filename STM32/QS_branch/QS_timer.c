/*
 *  Club Robot ESEO 2009 - 2010
 *  Chomp
 *
 *  Fichier : QS_timer.c
 *  Package : Qualit� Soft
 *  Description : Timers
 *  Auteur : Gwenn
 *  Version 20100424
 */


#include "QS_timer.h"
#include "QS_outputlog.h"
#include "stm32f4xx_tim.h"

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

#ifndef TIMER_1_PRIORITY
	#define TIMER_1_PRIORITY 9
#endif
#ifndef TIMER_2_PRIORITY
	#define TIMER_2_PRIORITY 11
#endif
#ifndef TIMER_3_PRIORITY
	#define TIMER_3_PRIORITY 13
#endif
#ifndef TIMER_4_PRIORITY
	#define TIMER_4_PRIORITY 14
#endif
#ifndef TIMER_5_PRIORITY
	#define TIMER_5_PRIORITY 15
#endif

#if TIMER_1_PRIORITY < 0 || TIMER_1_PRIORITY > 255
	#error "TIMER_1_PRIORITY is not >= 0 or <= 255"
#endif
#if TIMER_2_PRIORITY < 0 || TIMER_2_PRIORITY > 255
	#error "TIMER_2_PRIORITY is not >= 0 or <= 255"
#endif
#if TIMER_3_PRIORITY < 0 || TIMER_3_PRIORITY > 255
	#error "TIMER_3_PRIORITY is not >= 0 or <= 255"
#endif
#if TIMER_4_PRIORITY < 0 || TIMER_4_PRIORITY > 255
	#error "TIMER_4_PRIORITY is not >= 0 or <= 255"
#endif
#if TIMER_5_PRIORITY < 0 || TIMER_5_PRIORITY > 255
	#error "TIMER_5_PRIORITY is not >= 0 or <= 255"
#endif

//Interruptions
__attribute__((weak)) void _T1Interrupt() {TIMER1_AckIT();}
__attribute__((weak)) void _T2Interrupt() {TIMER2_AckIT();}
__attribute__((weak)) void _T3Interrupt() {TIMER3_AckIT();}
__attribute__((weak)) void _T4Interrupt() {TIMER4_AckIT();}
__attribute__((weak)) void _T5Interrupt() {TIMER5_AckIT();}

static bool_e initialized = FALSE;

/* Configuation de l'ensemble du bloc timer */
void TIMER_init(void){
	if (initialized) return;

	/* Horloges */
	RCC_APB2PeriphClockCmd(
		  RCC_APB2Periph_TIM10
		| RCC_APB2Periph_TIM11, ENABLE);
	RCC_APB1PeriphClockCmd(
		  RCC_APB1Periph_TIM12
		| RCC_APB1Periph_TIM13
		| RCC_APB1Periph_TIM14, ENABLE);

	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Period = 0xFFFF;	//Le moins rapide par d�faut
	TIM_TimeBaseStructure.TIM_Prescaler = 0xFFFF;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;

	//On s'arrange pour que la clock sur TIM11 soit la m�me que sur TIM12-14, voir manuel de r�ference STM32F4xx page 114
#if (PCLK2_FREQUENCY_HZ/PCLK1_FREQUENCY_HZ) != 1 && (PCLK2_FREQUENCY_HZ/PCLK1_FREQUENCY_HZ) != 2 && (PCLK2_FREQUENCY_HZ/PCLK1_FREQUENCY_HZ) != 4
#error "Incorrect HCLK/PCLK1 ratio, must be 1, 2 or 4"
#endif

	TIM_TimeBaseInit(TIM10, &TIM_TimeBaseStructure);
	TIM_TimeBaseInit(TIM11, &TIM_TimeBaseStructure);
	TIM_TimeBaseInit(TIM12, &TIM_TimeBaseStructure);
	TIM_TimeBaseInit(TIM13, &TIM_TimeBaseStructure);
	TIM_TimeBaseInit(TIM14, &TIM_TimeBaseStructure);

	TIM_UpdateRequestConfig(TIM10, TIM_UpdateSource_Regular);
	TIM_UpdateRequestConfig(TIM11, TIM_UpdateSource_Regular);
	TIM_UpdateRequestConfig(TIM12, TIM_UpdateSource_Regular);
	TIM_UpdateRequestConfig(TIM13, TIM_UpdateSource_Regular);
	TIM_UpdateRequestConfig(TIM14, TIM_UpdateSource_Regular);

	TIM_ITConfig(TIM10, TIM_IT_Update, ENABLE);
	TIM_ITConfig(TIM11, TIM_IT_Update, ENABLE);
	TIM_ITConfig(TIM12, TIM_IT_Update, ENABLE);
	TIM_ITConfig(TIM13, TIM_IT_Update, ENABLE);
	TIM_ITConfig(TIM14, TIM_IT_Update, ENABLE);

	TIM_ClearITPendingBit(TIM10, TIM_IT_Update);
	TIM_ClearITPendingBit(TIM11, TIM_IT_Update);
	TIM_ClearITPendingBit(TIM12, TIM_IT_Update);
	TIM_ClearITPendingBit(TIM13, TIM_IT_Update);
	TIM_ClearITPendingBit(TIM14, TIM_IT_Update);

	/* NVIC: initialisation des IT timer avec les niveaux de priorit� ad�quats*/
	NVIC_InitTypeDef NVICInit;
	NVICInit.NVIC_IRQChannelCmd = ENABLE;
	NVICInit.NVIC_IRQChannelSubPriority = 0;

	NVICInit.NVIC_IRQChannelPreemptionPriority = TIMER_1_PRIORITY;
	NVICInit.NVIC_IRQChannel = TIM1_TRG_COM_TIM11_IRQn;
	NVIC_Init(&NVICInit);

	NVICInit.NVIC_IRQChannelPreemptionPriority = TIMER_2_PRIORITY;
	NVICInit.NVIC_IRQChannel = TIM8_BRK_TIM12_IRQn;
	NVIC_Init(&NVICInit);

	NVICInit.NVIC_IRQChannelPreemptionPriority = TIMER_3_PRIORITY;
	NVICInit.NVIC_IRQChannel = TIM8_UP_TIM13_IRQn;
	NVIC_Init(&NVICInit);

	NVICInit.NVIC_IRQChannelPreemptionPriority = TIMER_4_PRIORITY;
	NVICInit.NVIC_IRQChannel = TIM8_TRG_COM_TIM14_IRQn;
	NVIC_Init(&NVICInit);

	NVICInit.NVIC_IRQChannelPreemptionPriority = TIMER_5_PRIORITY;
	NVICInit.NVIC_IRQChannel = TIM1_UP_TIM10_IRQn;
	NVIC_Init(&NVICInit);

	/* Fin de la configuration */
	initialized = TRUE;
}

void TIMER_run(TIM_TypeDef* TIMx, Uint8 period /* en millisecondes */) {
	if(!initialized){
		error_printf("TIMER non initialis� ! Appeller TIMER_init\n");
		return;
	}

	RCC_ClocksTypeDef clocksSpeed;
	Uint32 prescaler_mul = 1;
	RCC_GetClocksFreq(&clocksSpeed);


	if(TIMx == TIM10 || TIMx == TIM11) {
		if(clocksSpeed.SYSCLK_Frequency / clocksSpeed.PCLK2_Frequency > 1)
			prescaler_mul = 2;
		prescaler_mul *= clocksSpeed.PCLK2_Frequency / clocksSpeed.PCLK1_Frequency;
	} else {
		if(clocksSpeed.SYSCLK_Frequency / clocksSpeed.PCLK1_Frequency > 1)
			prescaler_mul = 2;
	}

	TIM_PrescalerConfig(TIMx, (prescaler_mul*TIMER_MS_PRESCALER) - 1, TIM_PSCReloadMode_Immediate);
	TIM_SetAutoreload(TIMx,  ((Uint16)period) * TIMER_PULSE_PER_MS);
//	TIM_PrescalerConfig(TIMx, 38, TIM_PSCReloadMode_Immediate);
//	TIM_SetAutoreload(TIMx,  64000);
	TIM_SetCounter(TIMx, 0);
	TIM_Cmd(TIMx, ENABLE);
}

void TIMER_run_us(TIM_TypeDef* TIMx, Uint16 period /* en microsecondes */) {
	if(!initialized){
		error_printf("TIMER non initialis� ! Appeller TIMER_init\n");
		return;
	}

	RCC_ClocksTypeDef clocksSpeed;
	Uint32 prescaler_mul = 1;
	RCC_GetClocksFreq(&clocksSpeed);


	if(TIMx == TIM10 || TIMx == TIM11) {
		if(clocksSpeed.SYSCLK_Frequency / clocksSpeed.PCLK2_Frequency > 1)
			prescaler_mul = 2;
		prescaler_mul *= clocksSpeed.PCLK2_Frequency / clocksSpeed.PCLK1_Frequency;
	} else {
		if(clocksSpeed.SYSCLK_Frequency / clocksSpeed.PCLK1_Frequency > 1)
			prescaler_mul = 2;
	}

	TIM_PrescalerConfig(TIMx, (prescaler_mul*TIMER_US_PRESCALER) - 1, TIM_PSCReloadMode_Immediate);
	TIM_SetAutoreload(TIMx, period * TIMER_PULSE_PER_US);
	TIM_SetCounter(TIMx, 0);
	TIM_Cmd(TIMx, ENABLE);
}

void TIMER_stop(TIM_TypeDef* TIMx) {
	if(!initialized){
		error_printf("TIMER non initialis� ! Appeller TIMER_init\n");
		return;
	}

	TIM_Cmd(TIMx, DISABLE);
}

void TIMER_disableInt(TIM_TypeDef* TIMx) {
	if(!initialized){
		error_printf("TIMER non initialis� ! Appeller TIMER_init\n");
		return;
	}

	switch((int)TIMx) {
		case (int)TIM10:
			NVIC_DisableIRQ(TIM1_UP_TIM10_IRQn);
			break;

		case (int)TIM11:
			NVIC_DisableIRQ(TIM1_TRG_COM_TIM11_IRQn);
			break;

		case (int)TIM12:
			NVIC_DisableIRQ(TIM8_BRK_TIM12_IRQn);
			break;

		case (int)TIM13:
			NVIC_DisableIRQ(TIM8_UP_TIM13_IRQn);
			break;

		case (int)TIM14:
			NVIC_DisableIRQ(TIM8_TRG_COM_TIM14_IRQn);
			break;
	}
}

void TIMER_enableInt(TIM_TypeDef* TIMx) {
	if(!initialized){
		error_printf("TIMER non initialis� ! Appeller TIMER_init\n");
		return;
	}

	switch((int)TIMx) {
		case (int)TIM10:
			NVIC_EnableIRQ(TIM1_UP_TIM10_IRQn);
			break;

		case (int)TIM11:
			NVIC_EnableIRQ(TIM1_TRG_COM_TIM11_IRQn);
			break;

		case (int)TIM12:
			NVIC_EnableIRQ(TIM8_BRK_TIM12_IRQn);
			break;

		case (int)TIM13:
			NVIC_EnableIRQ(TIM8_UP_TIM13_IRQn);
			break;

		case (int)TIM14:
			NVIC_EnableIRQ(TIM8_TRG_COM_TIM14_IRQn);
			break;
	}
}


//D�finitions par num�ro de timer

void TIMER1_run(Uint8 period /* en millisecondes */) {
	TIMER_run(TIM11, period);
}

void TIMER1_run_us (Uint16 period /* en microsecondes */) {
	TIMER_run_us(TIM11, period);
}

void TIMER1_stop(void) {
	TIMER_stop(TIM11);
}

void TIMER1_disableInt() {
	TIMER_disableInt(TIM11);
}

void TIMER1_enableInt() {
	TIMER_enableInt(TIM11);
}

int TIMER1_getCounter() {
	return TIM_GetCounter(TIM11);
}

void TIMER2_run(Uint8 period /* en millisecondes */) {
	TIMER_run(TIM12, period);
}

void TIMER2_run_us (Uint16 period /* en microsecondes */) {
	TIMER_run_us(TIM12, period);
}

void TIMER2_stop(void) {
	TIMER_stop(TIM12);
}

void TIMER2_disableInt() {
	TIMER_disableInt(TIM12);
}

void TIMER2_enableInt() {
	TIMER_enableInt(TIM12);
}

int TIMER2_getCounter() {
	return TIM_GetCounter(TIM12);
}

void TIMER3_run(Uint8 period /* en millisecondes */) {
	TIMER_run(TIM13, period);
}

void TIMER3_run_us (Uint16 period /* en microsecondes */) {
	TIMER_run_us(TIM13, period);
}

void TIMER3_stop(void) {
	TIMER_stop(TIM13);
}

void TIMER3_disableInt() {
	TIMER_disableInt(TIM13);
}

void TIMER3_enableInt() {
	TIMER_enableInt(TIM13);
}

int TIMER3_getCounter() {
	return TIM_GetCounter(TIM13);
}

void TIMER4_run(Uint8 period /* en millisecondes */) {
	TIMER_run(TIM14, period);
}

void TIMER4_run_us(Uint16 period /* en microsecondes */) {
	TIMER_run_us(TIM14, period);
}

void TIMER4_stop(void) {
	TIMER_stop(TIM14);
}

void TIMER4_disableInt() {
	TIMER_disableInt(TIM14);
}

void TIMER4_enableInt() {
	TIMER_enableInt(TIM14);
}

int TIMER4_getCounter() {
	return TIM_GetCounter(TIM14);
}

void TIMER5_run(Uint8 period /* en millisecondes */) {
	TIMER_run(TIM10, period);
}

void TIMER5_run_us (Uint16 period /* en microsecondes */) {
	TIMER_run_us(TIM10, period);
}

void TIMER5_stop(void) {
	TIMER_stop(TIM10);
}

void TIMER5_disableInt() {
	TIMER_disableInt(TIM10);
}

void TIMER5_enableInt() {
	TIMER_enableInt(TIM10);
}

int TIMER5_getCounter() {
	return TIM_GetCounter(TIM10);
}

//Interrupts management and redirection
void TIM1_UP_TIM10_IRQHandler() {
	if(TIM_GetITStatus(TIM10, TIM_IT_Update))
		_T5Interrupt();
}

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