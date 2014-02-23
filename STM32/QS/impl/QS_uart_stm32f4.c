/*
 *  Club Robot ESEO 2013 - 2014
 *
 *  Fichier : QS_impl_uart.c
 *  Package : Qualité Soft
 *  Description : Implémentation des fonctions UART pour stm32f4
 *  Auteur : amurzeau
 *  Version 3 juin 2013
 */

#include "QS_uart_impl.h"
#include "stm32f4xx_usart.h"
#include "../QS_macro.h"

typedef struct t {
	USART_TypeDef* usart_ptr;
	uint32_t usart_irq;
	uint8_t rcc_apb_id;
	uint32_t clock_periph;
} UART_IMPL_data_t;


#define UART_IMPL_NUM 3
static const UART_IMPL_data_t usart_infos[UART_IMPL_NUM] = {
	{USART1, USART1_IRQn, 2, RCC_APB2Periph_USART1},
	{USART2, USART2_IRQn, 1, RCC_APB1Periph_USART2},
	{USART3, USART3_IRQn, 1, RCC_APB1Periph_USART3}
};

bool_e UART_IMPL_init(Uint8 uart_id, Uint32 baudrate, Sint8 rx_irq_priority, Sint8 tx_irq_priority) {
	return UART_IMPL_init_ex(uart_id, baudrate, rx_irq_priority, tx_irq_priority, UART_I_StopBit_1, UART_I_Parity_None);
}

bool_e UART_IMPL_init_ex(Uint8 uart_id, Uint32 baudrate, Sint8 rx_irq_priority, Sint8 tx_irq_priority, UART_IMPL_stop_bits_e stop_bits, UART_IMPL_parity_e parity_mode) {
	assert(uart_id < UART_IMPL_NUM);

	NVIC_InitTypeDef NVIC_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannel = usart_infos[uart_id].usart_irq;
	if(rx_irq_priority >= 0 || tx_irq_priority >= 0) {
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = MIN(15 - rx_irq_priority, 15 - tx_irq_priority);
	} else {
		NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 15;
	}
	NVIC_Init(&NVIC_InitStructure);


	//PORTS_uarts_init();

	if(usart_infos[uart_id].rcc_apb_id == 1)
		RCC_APB1PeriphClockCmd(usart_infos[uart_id].clock_periph, ENABLE);
	else
		RCC_APB2PeriphClockCmd(usart_infos[uart_id].clock_periph, ENABLE);
	USART_OverSampling8Cmd(usart_infos[uart_id].usart_ptr, ENABLE);

	USART_InitStructure.USART_BaudRate = baudrate;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	switch(stop_bits) {
		default:
		case UART_I_StopBit_1:   USART_InitStructure.USART_StopBits = USART_StopBits_1;   break;
		case UART_I_StopBit_1_5: USART_InitStructure.USART_StopBits = USART_StopBits_1_5; break;
		case UART_I_StopBit_2:   USART_InitStructure.USART_StopBits = USART_StopBits_2;   break;
	}
	switch(parity_mode) {
		default:
		case UART_I_Parity_None: USART_InitStructure.USART_Parity = USART_Parity_No;   break;
		case UART_I_Parity_Odd:  USART_InitStructure.USART_Parity = USART_Parity_Odd;  break;
		case UART_I_Parity_Even: USART_InitStructure.USART_Parity = USART_Parity_Even; break;
	};
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(usart_infos[uart_id].usart_ptr, &USART_InitStructure);

	/* Enable USART */
	USART_Cmd(usart_infos[uart_id].usart_ptr, ENABLE);

	return TRUE;
}

void UART_IMPL_write(Uint8 uart_id, Uint8 data) {
	assert(uart_id < UART_IMPL_NUM);
	USART_SendData(usart_infos[uart_id].usart_ptr, data);
}

Uint8 UART_IMPL_read(Uint8 uart_id) {
	assert(uart_id < UART_IMPL_NUM);
	return (Uint8)USART_ReceiveData(usart_infos[uart_id].usart_ptr);
}

bool_e UART_IMPL_isRxEmpty(Uint8 uart_id) {
	assert(uart_id < UART_IMPL_NUM);
	return !USART_GetFlagStatus(usart_infos[uart_id].usart_ptr, USART_FLAG_RXNE);
}

bool_e UART_IMPL_isTxFull(Uint8 uart_id) {
	assert(uart_id < UART_IMPL_NUM);
	return !USART_GetFlagStatus(usart_infos[uart_id].usart_ptr, USART_FLAG_TXE);
}

bool_e UART_IMPL_isIdle(Uint8 uart_id) {
	assert(uart_id < UART_IMPL_NUM);
	return USART_GetFlagStatus(usart_infos[uart_id].usart_ptr, USART_FLAG_TC);
}

bool_e UART_IMPL_hasOverflow(Uint8 uart_id) {
	assert(uart_id < UART_IMPL_NUM);
	return USART_GetFlagStatus(usart_infos[uart_id].usart_ptr, USART_FLAG_ORE);
}

bool_e UART_IMPL_hasFrameError(Uint8 uart_id) {
	assert(uart_id < UART_IMPL_NUM);
	return USART_GetFlagStatus(usart_infos[uart_id].usart_ptr, USART_FLAG_FE) |
			USART_GetFlagStatus(usart_infos[uart_id].usart_ptr, USART_FLAG_NE);
}

bool_e UART_IMPL_resetErrors(Uint8 uart_id) {
	if(UART_IMPL_hasOverflow(uart_id) || UART_IMPL_hasFrameError(uart_id)) {
		UART_IMPL_read(uart_id);
		return TRUE;
	}
	return FALSE;
}

void UART_IMPL_setRxItPaused(Uint8 uart_id, bool_e pause) {
	assert(uart_id < UART_IMPL_NUM);
	if(pause)
		NVIC_DisableIRQ(usart_infos[uart_id].usart_irq);
	else
		NVIC_EnableIRQ(usart_infos[uart_id].usart_irq);
}

void UART_IMPL_setTxItPaused(Uint8 uart_id, bool_e pause) {
	UART_IMPL_setRxItPaused(uart_id, pause);
}

void UART_IMPL_setRxItEnabled(Uint8 uart_id, bool_e enable) {
	assert(uart_id < UART_IMPL_NUM);
	USART_ITConfig(usart_infos[uart_id].usart_ptr, USART_IT_RXNE, enable);
}

void UART_IMPL_setTxItEnabled(Uint8 uart_id, bool_e enable) {
	assert(uart_id < UART_IMPL_NUM);
	USART_ITConfig(usart_infos[uart_id].usart_ptr, USART_IT_TXE, enable);
}

//Ne fait rien car c'est la lecture/ecriture du buffer uart qui déclenche des interruptions (level-triggered interrupt)
void UART_IMPL_ackRxIt(Uint8 uart_id) {
//	assert(uart_id < UART_IMPL_NUM);
//	USART_ClearITPendingBit(usart_infos[uart_id].usart_ptr, USART_IT_RXNE);
}

void UART_IMPL_ackTxIt(Uint8 uart_id) {
//	assert(uart_id < UART_IMPL_NUM);
//	USART_ClearITPendingBit(usart_infos[uart_id].usart_ptr, USART_IT_TXE);
}

bool_e UART_IMPL_isRxItTriggered(Uint8 uart_id) {
	return !UART_IMPL_isRxEmpty(uart_id);
}

bool_e UART_IMPL_isTxItTriggered(Uint8 uart_id) {
	return !UART_IMPL_isTxFull(uart_id);
}

__attribute__((weak)) void UART1_RX_Interrupt() {}
__attribute__((weak)) void UART1_TX_Interrupt() {}
__attribute__((weak)) void UART2_RX_Interrupt() {}
__attribute__((weak)) void UART2_TX_Interrupt() {}
__attribute__((weak)) void UART3_RX_Interrupt() {}
__attribute__((weak)) void UART3_TX_Interrupt() {}

__attribute__((weak)) void USART1_IRQHandler() {
	if(USART_GetITStatus(USART1, USART_IT_RXNE))
		UART1_RX_Interrupt();
	if(USART_GetITStatus(USART1, USART_IT_TXE))
		UART1_TX_Interrupt();
}

__attribute__((weak)) void USART2_IRQHandler() {
	if(USART_GetITStatus(USART2, USART_IT_RXNE))
		UART2_RX_Interrupt();
	if(USART_GetITStatus(USART2, USART_IT_TXE))
		UART2_TX_Interrupt();
}

__attribute__((weak)) void USART3_IRQHandler() {
	if(USART_GetITStatus(USART3, USART_IT_RXNE))
		UART3_RX_Interrupt();
	if(USART_GetITStatus(USART3, USART_IT_TXE))
		UART3_TX_Interrupt();
}
