/*
 *  Club Robot ESEO 2009 - 2010
 *
 *  Fichier : config.h
 *  Package : Qualité Soft
 *  Description : 	Configuration d'un STM32
 *  Auteur : Gwenn
 *  Version 20100131
 */

#ifndef CONFIG_H
	#define CONFIG_H

	#include "stm32f10x_lib.h"
	#include "stm32f10x_usart.h"
	#include "stm32f10x_tim.h"

	void RCC_config();
	void NVIC_config();
	void GPIO_config();
	void UART_config();
	void TIMER_run(u16 period);

#endif /* CONFIG_H */
