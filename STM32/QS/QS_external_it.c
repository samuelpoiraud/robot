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
#include "stm32f4xx_syscfg.h"
#include "stm32f4xx_exti.h"


#ifdef USE_EXTERNAL_IT
#warning "SOYEZ PRUDENT AVEC LES IT EXTERNES"
// Elles font facilement planté la carte. Ne pas laisser une pin en l'air possédant une it externe sinon risque de plantage

EXTERNALIT_callback_it_t callbacks[16];


void EXTERNALIT_init() {
	static bool_e initialized = FALSE;
	if (initialized)
		return;

	Uint8 i;

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

	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
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



//Interrupts management and redirection

void EXTI0_IRQHandler() {
	if(EXTI_GetITStatus(EXTI_Line0))
		callbacks[0]();

	EXTI_ClearITPendingBit(EXTI_Line0);
}


void EXTI1_IRQHandler() {
	if(EXTI_GetITStatus(EXTI_Line1))
		callbacks[1]();

	EXTI_ClearITPendingBit(EXTI_Line1);
}

void EXTI2_IRQHandler() {
	if(EXTI_GetITStatus(EXTI_Line2))
		callbacks[2]();

	EXTI_ClearITPendingBit(EXTI_Line2);

}

void EXTI3_IRQHandler() {
	if(EXTI_GetITStatus(EXTI_Line3))
		callbacks[3]();

	EXTI_ClearITPendingBit(EXTI_Line3);
}

void EXTI4_IRQHandler() {
	if(EXTI_GetITStatus(EXTI_Line4))
		callbacks[4]();

	EXTI_ClearITPendingBit(EXTI_Line4);
}

void EXTI9_5_IRQHandler() {
	if(EXTI_GetFlagStatus(EXTI_Line5)){ // External Interrupt 5
		callbacks[5]();
		EXTI_ClearITPendingBit(EXTI_Line5);
	}

	if(EXTI_GetFlagStatus(EXTI_Line6)){ // External Interrupt 6
		callbacks[6]();
		EXTI_ClearITPendingBit(EXTI_Line6);
	}

	if(EXTI_GetFlagStatus(EXTI_Line7)){ // External Interrupt 7
		callbacks[7]();
		EXTI_ClearITPendingBit(EXTI_Line7);
	}

	if(EXTI_GetFlagStatus(EXTI_Line8)){ // External Interrupt 8
		callbacks[8]();
		EXTI_ClearITPendingBit(EXTI_Line8);
	}

	if(EXTI_GetFlagStatus(EXTI_Line9)){ // External Interrupt 9
		callbacks[9]();
		EXTI_ClearITPendingBit(EXTI_Line9);
	}
}

void EXTI15_10_IRQHandler() {
	if(EXTI_GetFlagStatus(EXTI_Line10)){ // External Interrupt 10
		callbacks[10]();
		EXTI_ClearITPendingBit(EXTI_Line10);
	}

	if(EXTI_GetFlagStatus(EXTI_Line11)){ // External Interrupt 11
		callbacks[11]();
		EXTI_ClearITPendingBit(EXTI_Line11);
	}

	if(EXTI_GetFlagStatus(EXTI_Line12)){ // External Interrupt 12
		callbacks[12]();
		EXTI_ClearITPendingBit(EXTI_Line12);
	}

	if(EXTI_GetFlagStatus(EXTI_Line13)){ // External Interrupt 13
		callbacks[13]();
		EXTI_ClearITPendingBit(EXTI_Line13);
	}

	if(EXTI_GetFlagStatus(EXTI_Line14)){ // External Interrupt 14
		callbacks[14]();
		EXTI_ClearITPendingBit(EXTI_Line14);
	}

	if(EXTI_GetFlagStatus(EXTI_Line15)){ // External Interrupt 15
		callbacks[15]();
		EXTI_ClearITPendingBit(EXTI_Line15);
	}
}

#endif //USE_EXTERNAL_IT
