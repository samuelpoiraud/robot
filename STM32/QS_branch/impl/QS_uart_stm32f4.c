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
#include "../../stm32f4xx_hal/stm32f4xx_hal_uart.h"
#include "../QS_macro.h"

typedef struct t {
	USART_TypeDef* usart_ptr;
	uint32_t usart_irq;
} UART_IMPL_data_t;


#define UART_IMPL_NUM 3
static const UART_IMPL_data_t usart_infos[UART_IMPL_NUM] = {
	{USART1, USART1_IRQn},
	{USART2, USART2_IRQn},
	{USART3, USART3_IRQn}
};

static UART_HandleTypeDef UART_HandleStructure;

bool_e UART_IMPL_init(Uint8 uart_id, Uint32 baudrate, Sint8 rx_irq_priority, Sint8 tx_irq_priority) {
	return UART_IMPL_init_ex(uart_id, baudrate, rx_irq_priority, tx_irq_priority, UART_I_StopBit_1, UART_I_Parity_None);
}

bool_e UART_IMPL_init_ex(Uint8 uart_id, Uint32 baudrate, Sint8 rx_irq_priority, Sint8 tx_irq_priority, UART_IMPL_stop_bits_e stop_bits, UART_IMPL_parity_e parity_mode) {
	assert(uart_id < UART_IMPL_NUM);

	Uint32 preemptionPriority;

	if(rx_irq_priority >= 0 || tx_irq_priority >= 0) {
		HAL_NVIC_SetPriority(usart_infos[uart_id].usart_irq, preemptionPriority, 0);
		HAL_NVIC_EnableIRQ(usart_infos[uart_id].usart_irq);
	} else {
		HAL_NVIC_SetPriority(usart_infos[uart_id].usart_irq, preemptionPriority, 0);
		HAL_NVIC_DisableIRQ(usart_infos[uart_id].usart_irq);
	}


	//PORTS_uarts_init();

	if(usart_infos[uart_id].usart_ptr == USART1)
		__HAL_RCC_USART1_CLK_ENABLE();
	else if(usart_infos[uart_id].usart_ptr == USART2)
		__HAL_RCC_USART2_CLK_ENABLE();
	else //(usart_infos[uart_id].usart_ptr == USART3
		__HAL_RCC_USART3_CLK_ENABLE();

	UART_HandleStructure.Instance = usart_infos[uart_id].usart_ptr;
	UART_HandleStructure.Init.BaudRate = baudrate;
	UART_HandleStructure.Init.WordLength = UART_WORDLENGTH_8B;
	UART_HandleStructure.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	UART_HandleStructure.Init.Mode = UART_MODE_TX_RX;
	UART_HandleStructure.Init.OverSampling = UART_OVERSAMPLING_8;

	switch(stop_bits) {
		default:
		case UART_I_StopBit_1:   UART_HandleStructure.Init.StopBits = UART_STOPBITS_1;  break;
		case UART_I_StopBit_2:   UART_HandleStructure.Init.StopBits = UART_STOPBITS_2;   break;
	}
	switch(parity_mode) {
		default:
		case UART_I_Parity_None: UART_HandleStructure.Init.Parity = UART_PARITY_NONE; break;
		case UART_I_Parity_Odd:  UART_HandleStructure.Init.Parity = UART_PARITY_ODD;  break;
		case UART_I_Parity_Even: UART_HandleStructure.Init.Parity = UART_PARITY_EVEN; break;
	};

	/*On applique les parametres d'initialisation ci-dessus */
	HAL_UART_Init(&UART_HandleStructure);

	/*Activation de l'UART */
	__HAL_UART_ENABLE(&UART_HandleStructure);

	return TRUE;
}

void UART_IMPL_write(Uint8 uart_id, Uint8 data) {
	assert(uart_id < UART_IMPL_NUM);
	HAL_UART_Transmit_IT(&UART_HandleStructure, &data, 1);
}

Uint8 UART_IMPL_read(Uint8 uart_id) {
	assert(uart_id < UART_IMPL_NUM);
	Uint8 data;
	return (Uint8)HAL_UART_Receive_IT(&UART_HandleStructure, &data, 1);
}

bool_e UART_IMPL_isRxEmpty(Uint8 uart_id) {
	assert(uart_id < UART_IMPL_NUM);
	return !__HAL_UART_GET_FLAG(&UART_HandleStructure, UART_FLAG_RXNE);
}

bool_e UART_IMPL_isTxFull(Uint8 uart_id) {
	assert(uart_id < UART_IMPL_NUM);
	return !__HAL_UART_GET_FLAG(&UART_HandleStructure, UART_FLAG_TXE);
}

bool_e UART_IMPL_isIdle(Uint8 uart_id) {
	assert(uart_id < UART_IMPL_NUM);
	return __HAL_UART_GET_FLAG(&UART_HandleStructure, UART_FLAG_TC);
}

bool_e UART_IMPL_hasOverflow(Uint8 uart_id) {
	assert(uart_id < UART_IMPL_NUM);
	return __HAL_UART_GET_FLAG(&UART_HandleStructure, UART_FLAG_ORE);
}

bool_e UART_IMPL_hasFrameError(Uint8 uart_id) {
	assert(uart_id < UART_IMPL_NUM);
	return __HAL_UART_GET_FLAG(&UART_HandleStructure, UART_FLAG_FE) |
			__HAL_UART_GET_FLAG(&UART_HandleStructure, UART_FLAG_NE);
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
		HAL_NVIC_DisableIRQ(usart_infos[uart_id].usart_irq);
	else
		HAL_NVIC_EnableIRQ(usart_infos[uart_id].usart_irq);
}

void UART_IMPL_setTxItPaused(Uint8 uart_id, bool_e pause) {
	UART_IMPL_setRxItPaused(uart_id, pause);
}

void UART_IMPL_setRxItEnabled(Uint8 uart_id, bool_e enable) {
	assert(uart_id < UART_IMPL_NUM);
	if(enable)
		__HAL_UART_ENABLE_IT(&UART_HandleStructure, UART_IT_RXNE);
	else
		__HAL_UART_DISABLE_IT(&UART_HandleStructure, UART_IT_RXNE);
}

void UART_IMPL_setTxItEnabled(Uint8 uart_id, bool_e enable) {
	assert(uart_id < UART_IMPL_NUM);
	if(enable)
		__HAL_UART_ENABLE_IT(&UART_HandleStructure, UART_IT_TXE);
	else
		__HAL_UART_DISABLE_IT(&UART_HandleStructure, UART_IT_TXE);
}

//Ne fait rien car c'est la lecture/ecriture du buffer uart qui déclenche des interruptions (level-triggered interrupt)
void UART_IMPL_ackRxIt(Uint8 uart_id) {
//	assert(uart_id < UART_IMPL_NUM);
//	__HAL_UART_DISABLE_IT(&UART_HandleStructure, UART_IT_RXNE);
}

void UART_IMPL_ackTxIt(Uint8 uart_id) {
//	assert(uart_id < UART_IMPL_NUM);
//	__HAL_UART_DISABLE_IT(&UART_HandleStructure, UART_IT_TXE);
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
	if(__HAL_UART_GET_IT_SOURCE(&UART_HandleStructure, UART_IT_RXNE))
		UART1_RX_Interrupt();
	if(__HAL_UART_GET_IT_SOURCE(&UART_HandleStructure, UART_IT_TXE))
		UART1_TX_Interrupt();
}

__attribute__((weak)) void USART2_IRQHandler() {
	if(__HAL_UART_GET_IT_SOURCE(&UART_HandleStructure, UART_IT_RXNE))
		UART2_RX_Interrupt();
	if(__HAL_UART_GET_IT_SOURCE(&UART_HandleStructure, UART_IT_TXE))
		UART2_TX_Interrupt();
}
#ifndef USE_UART3
#if AX12_UART_ID != 3
void USART3_IRQHandler() {
	if(__HAL_UART_GET_IT_SOURCE(&UART_HandleStructure, UART_IT_RXNE))
		UART3_RX_Interrupt();
	if(__HAL_UART_GET_IT_SOURCE(&UART_HandleStructure, UART_IT_TXE))
		UART3_TX_Interrupt();
}
#endif
#endif
