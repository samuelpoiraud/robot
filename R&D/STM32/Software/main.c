/*
 *  Club Robot ESEO 2009 - 2010
 *
 *  Fichier : main.c
 *  Package : Qualité Soft
 *  Description : 	Test d'un STM32
 *  Auteur : Gwenn
 *  Version 20100131
 */

#define DELAY 0xAFFFF

#include "stm32f10x_lib.h"
#include "config.h"
#include "utils.h"


/*-----------------------------------------------------
                    Main
-----------------------------------------------------*/
int main(){

	// Configuration
	RCC_config();
	NVIC_config();
	GPIO_config();
	
	// Timer 1 à 1KHz
	TIMER_run(1);

	while(1){}
	return 0;
}

void TIM1_UP_IRQHandler(void)
{
	static unsigned char i = 0;
	i++;
	GPIO_Write(GPIOE, i);

	TIM_ClearITPendingBit(TIM1, TIM_IT_Update );
}

