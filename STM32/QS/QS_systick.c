/*
 *	Club Robot ESEO 2014 - 2015
 *	Holly & Wood
 *
 *	Fichier : QS_systick.c
 *	Package : Qualite Soft
 *	Description : Configuration de systick pour gérer une l'horloge du projet
 *	Auteur : Arnaud
 *	Version 20100421
 */

#include "QS_systick.h"
#include "../stm32f4xx/stm32f4xx_rcc.h"

static volatile time32_t* systick_counter = NULL;

void SYSTICK_init(time32_t* systick_ptr){
	RCC_ClocksTypeDef RCC_Clocks;
	RCC_GetClocksFreq(&RCC_Clocks);
	SysTick_Config(RCC_Clocks.HCLK_Frequency / 1000);
	NVIC_SetPriority(SysTick_IRQn, 0);
	systick_counter = systick_ptr;

}

void SysTick_Handler(void){
	if(systick_counter)
		(*systick_counter)++;
}
