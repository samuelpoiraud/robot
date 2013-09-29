/*
 *  Club Robot ESEO 2013 - 2014
 *
 *
 *  Fichier : QS_external_it.c
 *  Package : Qualité Soft
 *  Description : Gestion des interruptions externes
 *  Auteur : Alexis
 *  Version 20130929
 */

#include "QS_external_it.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_syscfg.h"
#include "stm32f4xx_exti.h"

EXTERNALIT_callback_it_t callbacks[16];

void EXTERNALIT_init() {
	int i;

	for(i = 0; i < 16; i++) {
		callbacks[i] = NULL;
	}

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
}

void EXTERNALIT_configure(EXTERNALIT_port_e port, Uint8 pin, EXTERNALIT_edge_e edge, EXTERNALIT_callback_it_t callback) {
	NVIC_InitTypeDef NVIC_InitStructure;

	if(pin >= 16)
		return;

	SYSCFG_EXTILineConfig(port, pin);

	EXTERNALIT_set_edge(port, pin, edge);

	callbacks[pin] = callback;

	if(pin < 5)
		NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn + pin;
	else if(pin < 10)
		NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
	else
		NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;

	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

	NVIC_Init(&NVIC_InitStructure);
}

void EXTERNALIT_set_edge(EXTERNALIT_port_e port, Uint8 pin, EXTERNALIT_edge_e edge) {
	EXTI_InitTypeDef EXTI_InitStructure;

	port = port;

	EXTI_InitStructure.EXTI_Line = 1 << pin;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;

	switch(edge) {
		case EXTIT_Edge_Rising:
			EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
			break;

		case EXTIT_Edge_Falling:
			EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
			break;

		case EXTIT_Edge_Both:
			EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
			break;
	}

	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
}

void EXTERNALIT_set_priority(EXTERNALIT_port_e port, Uint8 pin, Uint8 priority) {
	NVIC_InitTypeDef NVIC_InitStructure;

	if(pin < 5)
		NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn + pin;
	else if(pin < 10)
		NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
	else
		NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;

	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = priority;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

	NVIC_Init(&NVIC_InitStructure);
}

void EXTERNALIT_disable(EXTERNALIT_port_e port, Uint8 pin) {
	EXTI_InitTypeDef EXTI_InitStructure;

	port = port;

	EXTI_InitStructure.EXTI_Line = 1 << pin;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_LineCmd = DISABLE;
	EXTI_Init(&EXTI_InitStructure);
}

void EXTERNALIT_set_it_enabled(EXTERNALIT_port_e port, Uint8 pin, bool_e enabled) {
	IRQn_Type irq;

	if(pin < 5)
		irq = EXTI0_IRQn + pin;
	else if(pin < 10)
		irq = EXTI9_5_IRQn;
	else
		irq = EXTI15_10_IRQn;

	if(enabled)
		NVIC_EnableIRQ(irq);
	else
		NVIC_DisableIRQ(irq);
}
