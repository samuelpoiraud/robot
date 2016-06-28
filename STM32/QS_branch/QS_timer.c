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
#include "QS_outputlog.h"
#include "../stm32f4xx_hal/stm32f4xx_hal_tim.h"

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

static TIM_HandleTypeDef* TIMER_getHandle(TIM_TypeDef *TIMx);

static bool_e initialized = FALSE;
static TIM_HandleTypeDef TIM_HandleStructure_10;
static TIM_HandleTypeDef TIM_HandleStructure_11;
static TIM_HandleTypeDef TIM_HandleStructure_12;
static TIM_HandleTypeDef TIM_HandleStructure_13;
static TIM_HandleTypeDef TIM_HandleStructure_14;


/* Configuation de l'ensemble du bloc timer */
void TIMER_init(void){
	if (initialized) return;

	/* Horloges */
	__HAL_RCC_TIM10_CLK_ENABLE();
	__HAL_RCC_TIM11_CLK_ENABLE();
	__HAL_RCC_TIM12_CLK_ENABLE();
	__HAL_RCC_TIM13_CLK_ENABLE();
	__HAL_RCC_TIM14_CLK_ENABLE();

	static TIM_Base_InitTypeDef TIM_InitStructure;

	TIM_InitStructure.CounterMode = TIM_COUNTERMODE_UP;
	TIM_InitStructure.Period = 0xFFFF;	//Le moins rapide par défaut
	TIM_InitStructure.Prescaler = 0xFFFF;
	TIM_InitStructure.RepetitionCounter = 0;
	TIM_InitStructure.ClockDivision = TIM_CLOCKDIVISION_DIV1;

	//On s'arrange pour que la clock sur TIM11 soit la même que sur TIM12-14, voir manuel de réference STM32F4xx page 114
#if (PCLK2_FREQUENCY_HZ/PCLK1_FREQUENCY_HZ) != 1 && (PCLK2_FREQUENCY_HZ/PCLK1_FREQUENCY_HZ) != 2 && (PCLK2_FREQUENCY_HZ/PCLK1_FREQUENCY_HZ) != 4
#error "Incorrect HCLK/PCLK1 ratio, must be 1, 2 or 4"
#endif

	//Initialisation TIM10
	TIM_HandleStructure_10.Instance = TIM10;
	TIM_HandleStructure_10.Init = TIM_InitStructure;
	HAL_TIM_Base_Init(&TIM_HandleStructure_10);

	//Initialisation TIM11
	TIM_HandleStructure_11.Instance = TIM11;
	TIM_HandleStructure_11.Init = TIM_InitStructure;
	HAL_TIM_Base_Init(&TIM_HandleStructure_11);

	//Initialisation TIM12
	TIM_HandleStructure_12.Instance = TIM12;
	TIM_HandleStructure_12.Init = TIM_InitStructure;
	HAL_TIM_Base_Init(&TIM_HandleStructure_12);

	//Initialisation TIM13
	TIM_HandleStructure_13.Instance = TIM13;
	TIM_HandleStructure_13.Init = TIM_InitStructure;
	HAL_TIM_Base_Init(&TIM_HandleStructure_13);

	//Initialisation TIM14
	TIM_HandleStructure_14.Instance = TIM14;
	TIM_HandleStructure_14.Init = TIM_InitStructure;
	HAL_TIM_Base_Init(&TIM_HandleStructure_14);

	//Update request source enable
	__HAL_TIM_URS_ENABLE(&TIM_HandleStructure_10);
	__HAL_TIM_URS_ENABLE(&TIM_HandleStructure_11);
	__HAL_TIM_URS_ENABLE(&TIM_HandleStructure_12);
	__HAL_TIM_URS_ENABLE(&TIM_HandleStructure_13);
	__HAL_TIM_URS_ENABLE(&TIM_HandleStructure_14);

	__HAL_TIM_ENABLE_IT(&TIM_HandleStructure_10, TIM_IT_UPDATE);
	__HAL_TIM_ENABLE_IT(&TIM_HandleStructure_11, TIM_IT_UPDATE);
	__HAL_TIM_ENABLE_IT(&TIM_HandleStructure_12, TIM_IT_UPDATE);
	__HAL_TIM_ENABLE_IT(&TIM_HandleStructure_13, TIM_IT_UPDATE);
	__HAL_TIM_ENABLE_IT(&TIM_HandleStructure_14, TIM_IT_UPDATE);

	__HAL_TIM_CLEAR_IT(&TIM_HandleStructure_10, TIM_IT_UPDATE);
	__HAL_TIM_CLEAR_IT(&TIM_HandleStructure_11, TIM_IT_UPDATE);
	__HAL_TIM_CLEAR_IT(&TIM_HandleStructure_12, TIM_IT_UPDATE);
	__HAL_TIM_CLEAR_IT(&TIM_HandleStructure_13, TIM_IT_UPDATE);
	__HAL_TIM_CLEAR_IT(&TIM_HandleStructure_14, TIM_IT_UPDATE);

	/* NVIC: initialisation des IT timer avec les niveaux de priorité adéquats*/
	HAL_NVIC_SetPriority(TIM1_TRG_COM_TIM11_IRQn, TIMER_1_PRIORITY, 0);
	HAL_NVIC_EnableIRQ(TIM1_TRG_COM_TIM11_IRQn);

	HAL_NVIC_SetPriority(TIM8_BRK_TIM12_IRQn, TIMER_2_PRIORITY, 0);
	HAL_NVIC_EnableIRQ(TIM8_BRK_TIM12_IRQn);

	HAL_NVIC_SetPriority(TIM8_UP_TIM13_IRQn, TIMER_3_PRIORITY, 0);
	HAL_NVIC_EnableIRQ(TIM8_UP_TIM13_IRQn);

	HAL_NVIC_SetPriority(TIM8_TRG_COM_TIM14_IRQn, TIMER_4_PRIORITY, 0);
	HAL_NVIC_EnableIRQ(TIM8_TRG_COM_TIM14_IRQn);

	HAL_NVIC_SetPriority(TIM1_UP_TIM10_IRQn, TIMER_5_PRIORITY, 0);
	HAL_NVIC_EnableIRQ(TIM1_UP_TIM10_IRQn);

	/* Fin de la configuration */
	initialized = TRUE;
}

static TIM_HandleTypeDef* TIMER_getHandle(TIM_TypeDef *TIMx){
	TIM_HandleTypeDef *TIM_HandleStructure_x;
	switch((int)TIMx){
		case TIM11: //TIMER1
			TIM_HandleStructure_x = &TIM_HandleStructure_11;
			break;

		case TIM12: //TIMER2
			TIM_HandleStructure_x = &TIM_HandleStructure_12;
			break;

		case TIM13: //TIMER3
			TIM_HandleStructure_x = &TIM_HandleStructure_13;
			break;

		case TIM14: //TIMER4
			TIM_HandleStructure_x = &TIM_HandleStructure_14;
			break;

		case TIM10: //TIMER5
			TIM_HandleStructure_x = &TIM_HandleStructure_10;
			break;
		default:
			debug_printf("Ce timer n'existe pas !!\n");
	}
	return TIM_HandleStructure_x
}

void TIMER_run(TIM_TypeDef* TIMx, Uint8 period /* en millisecondes */) {
	if(!initialized){
		error_printf("TIMER non initialisé ! Appeller TIMER_init\n");
		return;
	}

	TIM_HandleTypeDef *TIM_HandleStructure_x = TIMER_getHandle(TIMx);

	Uint32 SYSCLKFreq =  HAL_RCC_GetSysClockFreq();
	Uint32 PCLK1Freq = HAL_RCC_GetPCLK1Freq();
	Uint32 PCLK2Freq = HAL_RCC_GetPCLK2Freq();

	if(TIMx == TIM10 || TIMx == TIM11) {
		if(SYSCLKFreq / PCLK2Freq > 1)
			prescaler_mul = 2;
		prescaler_mul *= PCLK2Freq / PCLK1Freq;
	} else {
		if(SYSCLKFreq / PCLK1Freq > 1)
			prescaler_mul = 2;
	}

	__HAL_TIM_SET_PRESCALER(TIM_HandleStructure_x, (prescaler_mul*TIMER_MS_PRESCALER) - 1);
	__HAL_TIM_SET_AUTORELOAD(TIM_HandleStructure_x, ((Uint16)period) * TIMER_PULSE_PER_MS);
	__HAL_TIM_SET_COUNTER(TIM_HandleStructure_x, 0);
	__HAL_TIM_ENABLE(TIM_HandleStructure_x);
}

void TIMER_run_us(TIM_TypeDef* TIMx, Uint16 period /* en microsecondes */) {
	if(!initialized){
		error_printf("TIMER non initialisé ! Appeller TIMER_init\n");
		return;
	}

	TIM_HandleTypeDef *TIM_HandleStructure_x = TIMER_getHandle(TIMx);

	Uint32 SYSCLKFreq =  HAL_RCC_GetSysClockFreq();
	Uint32 PCLK1Freq = HAL_RCC_GetPCLK1Freq();
	Uint32 PCLK2Freq = HAL_RCC_GetPCLK2Freq();

	if(TIMx == TIM10 || TIMx == TIM11) {
		if(SYSCLKFreq / PCLK2Freq > 1)
			prescaler_mul = 2;
		prescaler_mul *= PCLK2Freq / PCLK1Freq;
	} else {
		if(SYSCLKFreq / PCLK1Freq > 1)
			prescaler_mul = 2;
	}

	__HAL_TIM_SET_PRESCALER(TIM_HandleStructure_x, (prescaler_mul*TIMER_US_PRESCALER) - 1);
	__HAL_TIM_SET_AUTORELOAD(TIM_HandleStructure_x, ((Uint16)period) * TIMER_PULSE_PER_US);
	__HAL_TIM_SET_COUNTER(TIM_HandleStructure_x, 0);
	__HAL_TIM_ENABLE(TIM_HandleStructure_x);
}

void TIMER_stop(TIM_TypeDef* TIMx) {
	if(!initialized){
		error_printf("TIMER non initialisé ! Appeller TIMER_init\n");
		return;
	}

	__HAL_TIM_DISABLE(TIMER_getHandle(TIMx));
}

void TIMER_disableInt(TIM_TypeDef* TIMx) {
	if(!initialized){
		error_printf("TIMER non initialisé ! Appeller TIMER_init\n");
		return;
	}

	switch((int)TIMx) {
		case (int)TIM10:
			HAL_NVIC_DisableIRQ(TIM1_UP_TIM10_IRQn);
			break;

		case (int)TIM11:
			HAL_NVIC_DisableIRQ(TIM1_TRG_COM_TIM11_IRQn);
			break;

		case (int)TIM12:
			HAL_NVIC_DisableIRQ(TIM8_BRK_TIM12_IRQn);
			break;

		case (int)TIM13:
			HAL_NVIC_DisableIRQ(TIM8_UP_TIM13_IRQn);
			break;

		case (int)TIM14:
			HAL_NVIC_DisableIRQ(TIM8_TRG_COM_TIM14_IRQn);
			break;
	}
}

void TIMER_enableInt(TIM_TypeDef* TIMx) {
	if(!initialized){
		error_printf("TIMER non initialisé ! Appeller TIMER_init\n");
		return;
	}

	switch((int)TIMx) {
		case (int)TIM10:
			HAL_NVIC_EnableIRQ(TIM1_UP_TIM10_IRQn);
			break;

		case (int)TIM11:
			HAL_NVIC_EnableIRQ(TIM1_TRG_COM_TIM11_IRQn);
			break;

		case (int)TIM12:
			HAL_NVIC_EnableIRQ(TIM8_BRK_TIM12_IRQn);
			break;

		case (int)TIM13:
			HAL_NVIC_EnableIRQ(TIM8_UP_TIM13_IRQn);
			break;

		case (int)TIM14:
			HAL_NVIC_EnableIRQ(TIM8_TRG_COM_TIM14_IRQn);
			break;
	}
}


//Définitions par numéro de timer

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
	return __HAL_TIM_GET_COUNTER(&TIM_HandleStructure_11);
}

void TIMER1_AckIT(){
	__HAL_TIM_CLEAR_IT(&TIM_HandleStructure_11, TIM_IT_UPDATE);
}

void TIMER1_getITStatus(){
	__HAL_TIM_GET_IT_SOURCE(&TIM_HandleStructure_11, TIM_IT_UPDATE);
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
	return __HAL_TIM_GET_COUNTER(&TIM_HandleStructure_12);
}

void TIMER2_AckIT(){
	__HAL_TIM_CLEAR_IT(&TIM_HandleStructure_12, TIM_IT_UPDATE);
}

void TIMER2_getITStatus(){
	__HAL_TIM_GET_IT_SOURCE(&TIM_HandleStructure_12, TIM_IT_UPDATE);
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
	return __HAL_TIM_GET_COUNTER(&TIM_HandleStructure_13);
}

void TIMER3_AckIT(){
	__HAL_TIM_CLEAR_IT(&TIM_HandleStructure_13, TIM_IT_UPDATE);
}

void TIMER3_getITStatus(){
	__HAL_TIM_GET_IT_SOURCE(&TIM_HandleStructure_13, TIM_IT_UPDATE);
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
	return __HAL_TIM_GET_COUNTER(&TIM_HandleStructure_14);
}

void TIMER4_AckIT(){
	__HAL_TIM_CLEAR_IT(&TIM_HandleStructure_14, TIM_IT_UPDATE);
}

void TIMER4_getITStatus(){
	__HAL_TIM_GET_IT_SOURCE(&TIM_HandleStructure_14, TIM_IT_UPDATE);
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
	return __HAL_TIM_GET_COUNTER(&TIM_HandleStructure_10);
}

void TIMER5_AckIT(){
	__HAL_TIM_CLEAR_IT(&TIM_HandleStructure_10, TIM_IT_UPDATE);
}

void TIMER5_getITStatus(){
	__HAL_TIM_GET_IT_SOURCE(&TIM_HandleStructure_10, TIM_IT_UPDATE);
}

//Interrupts management and redirection
void TIM1_UP_TIM10_IRQHandler() {
	if(__HAL_TIM_GET_IT_SOURCE(&TIM_HandleStructure_10, TIM_IT_UPDATE))
		_T5Interrupt();
}

void TIM1_TRG_COM_TIM11_IRQHandler() {
	if(__HAL_TIM_GET_IT_SOURCE(&TIM_HandleStructure_11, TIM_IT_UPDATE))
		_T1Interrupt();
}

void TIM8_BRK_TIM12_IRQHandler() {
	if(__HAL_TIM_GET_IT_SOURCE(&TIM_HandleStructure_12, TIM_IT_UPDATE))
		_T2Interrupt();
}

void TIM8_UP_TIM13_IRQHandler() {
	if(__HAL_TIM_GET_IT_SOURCE(&TIM_HandleStructure_13, TIM_IT_UPDATE))
		_T3Interrupt();
}

void TIM8_TRG_COM_TIM14_IRQHandler() {
	if(__HAL_TIM_GET_IT_SOURCE(&TIM_HandleStructure_14, TIM_IT_UPDATE))
		_T4Interrupt();
}
