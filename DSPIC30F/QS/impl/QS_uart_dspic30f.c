/*
 *  Club Robot ESEO 2013 - 2014
 *
 *  $Id$
 *
 *  Package : Qualité Soft
 *  Description : Implémentation des fonctions UART pour dspic30f
 *  Auteur : amurzeau
 */

#include "QS_uart_impl.h"
#include "../QS_all.h"
#include <uart.h>

//Converti une priorité entre 0 et 15 (15 = priorité max) en un flag (voir UART_RX_INT_PR0 et UART_TX_INT_PR0 et suivant dans uart.h)
#define RX_IT_PRI_FLAG(x) (0xFFF8 |  (((x) >> 1) & 0x07) )
#define TX_IT_PRI_FLAG(x) (0xFF8F | ((((x) >> 1) & 0x07) << 4) )

#ifdef FREQ_10MHZ
	#define INSTR_FREQ 10000000
#elif (defined FREQ_20MHZ)
	#define INSTR_FREQ 20000000
#elif (defined FREQ_40MHZ)
	#define INSTR_FREQ 40000000
#elif (defined FREQ_INTERNAL_CLK)
	#define INSTR_FREQ 8000000
#endif


#define UART_IMPL_NUM 2

#define call_uart_function(func, uart_id, ...) \
((uart_id)? func##UART2(__VA_ARGS__) : func##UART1(__VA_ARGS__))

#define get_uart_register(reg, uart_id) \
((uart_id)? U2##reg : U1##reg)

#define set_uart_register(reg, uart_id, val) \
((uart_id)? (U2##reg = val) : (U1##reg = val))



bool_e UART_IMPL_init(Uint8 uart_id, Uint32 baudrate, Sint8 rx_irq_priority, Sint8 tx_irq_priority) {
	return UART_IMPL_init_ex(uart_id, baudrate, rx_irq_priority, tx_irq_priority, UART_I_StopBit_1, UART_I_Parity_None);
}

bool_e UART_IMPL_init_ex(Uint8 uart_id, Uint32 baudrate, Sint8 rx_irq_priority, Sint8 tx_irq_priority, UART_IMPL_stop_bits_e stop_bits, UART_IMPL_parity_e parity_mode) {
	assert(uart_id < UART_IMPL_NUM);

	Sint16 config1;
	Sint16 config2;
	Sint16 config3;

	//baudrate_value = INSTR_FREQ/16 / baudrate - 1 + 0.5
	// <=> baudrate_value = (INSTR_FREQ/8 + baudrate) / 2 / baudrate - 1
	// <=> baudrate_value = (INSTR_FREQ/800 + baudrate/100) / 2 / (baudrate/100) - 1
	//__builtin_divud == division hardware (rapide)
	if(baudrate > 1000000) {
		baudrate = 1000000;
	}
	Uint16 reduced_baudrate = __builtin_divud(baudrate, 100);
	Uint16 baudrate_value = __builtin_divud(((Uint32)INSTR_FREQ/800 + reduced_baudrate) >> 1, reduced_baudrate) - 1;

	config1 = UART_EN & UART_IDLE_CON &
		UART_DIS_WAKE & UART_DIS_LOOPBACK &
		UART_DIS_ABAUD;

	switch(stop_bits) {
		default:
		case UART_I_StopBit_1:   config1 &= UART_1STOPBIT;  break;
		case UART_I_StopBit_1_5: config1 &= UART_2STOPBITS; break;
		case UART_I_StopBit_2:   config1 &= UART_2STOPBITS; break;
	}
	switch(parity_mode) {
		default:
		case UART_I_Parity_None: config1 &= UART_NO_PAR_8BIT;   break;
		case UART_I_Parity_Odd:  config1 &= UART_ODD_PAR_8BIT;  break;
		case UART_I_Parity_Even: config1 &= UART_EVEN_PAR_8BIT; break;
	};

	config2 = UART_INT_TX &
		UART_TX_PIN_NORMAL &
		UART_TX_ENABLE &
		UART_INT_RX_CHAR &
		UART_ADR_DETECT_DIS &
		UART_RX_OVERRUN_CLEAR;

	config3 = UART_RX_INT_DIS & UART_TX_INT_DIS & RX_IT_PRI_FLAG(rx_irq_priority) & TX_IT_PRI_FLAG(tx_irq_priority);

// 	UART_IMPL_setRxItEnabled(uart_id, FALSE);
// 	UART_IMPL_setTxItEnabled(uart_id, FALSE);

	switch(uart_id) {
		case 0:
			OpenUART1(config1, config2, baudrate_value);
			ConfigIntUART1(config3);
			break;

		case 1:
			OpenUART2(config1, config2, baudrate_value);
			ConfigIntUART2(config3);
			break;
	}

	return TRUE;
}

void UART_IMPL_write(Uint8 uart_id, Uint8 data) {
	assert(uart_id < UART_IMPL_NUM);
	call_uart_function(Write, uart_id, data);
}

Uint8 UART_IMPL_read(Uint8 uart_id) {
	assert(uart_id < UART_IMPL_NUM);
	return call_uart_function(Read, uart_id);
}

bool_e UART_IMPL_isRxEmpty(Uint8 uart_id) {
	assert(uart_id < UART_IMPL_NUM);
	return !get_uart_register(STAbits.URXDA, uart_id);
}

bool_e UART_IMPL_isTxFull(Uint8 uart_id) {
	assert(uart_id < UART_IMPL_NUM);
	return get_uart_register(STAbits.UTXBF, uart_id);
}

bool_e UART_IMPL_isIdle(Uint8 uart_id) {
	assert(uart_id < UART_IMPL_NUM);
	return get_uart_register(STAbits.TRMT, uart_id);
}

bool_e UART_IMPL_hasOverflow(Uint8 uart_id) {
	assert(uart_id < UART_IMPL_NUM);
	return get_uart_register(STAbits.OERR, uart_id);
}

bool_e UART_IMPL_hasFrameError(Uint8 uart_id) {
	assert(uart_id < UART_IMPL_NUM);
	return get_uart_register(STAbits.FERR, uart_id);
}

bool_e UART_IMPL_resetErrors(Uint8 uart_id) {
	if(get_uart_register(STAbits.FERR, uart_id) || get_uart_register(STAbits.PERR, uart_id)) {
		UART_IMPL_read(uart_id);
		return TRUE;
	}
	if(get_uart_register(STAbits.OERR, uart_id)) {
		set_uart_register(STAbits.OERR, uart_id, 0);
		return TRUE;
	}
	return FALSE;
}

void UART_IMPL_setRxItPaused(Uint8 uart_id, bool_e pause) {
	assert(uart_id < UART_IMPL_NUM);
	if(uart_id == 0) {
		if(pause)
			DisableIntU1RX;
		else
			EnableIntU1RX;
		_U1RXIF = 1;
	} else {
		if(pause)
			DisableIntU2RX;
		else
			EnableIntU2RX;
		_U2RXIF = 1;
	}
}

void UART_IMPL_setTxItPaused(Uint8 uart_id, bool_e pause) {
	assert(uart_id < UART_IMPL_NUM);
	if(uart_id == 0) {
		if(pause)
			DisableIntU1TX;
		else
			EnableIntU1TX;
		_U1TXIF = 1;
	} else {
		if(pause)
			DisableIntU2TX;
		else
			EnableIntU2TX;
		_U2TXIF = 1;
	}
}

void UART_IMPL_setRxItEnabled(Uint8 uart_id, bool_e enable) {
	UART_IMPL_ackRxIt(uart_id);
	UART_IMPL_setRxItPaused(uart_id, enable == FALSE);
}

void UART_IMPL_setTxItEnabled(Uint8 uart_id, bool_e enable) {
	UART_IMPL_ackTxIt(uart_id);
	UART_IMPL_setTxItPaused(uart_id, enable == FALSE);
}

void UART_IMPL_ackRxIt(Uint8 uart_id) {
	assert(uart_id < UART_IMPL_NUM);
	if(uart_id == 0)
		IFS0bits.U1RXIF = 0;
	else
		IFS1bits.U2RXIF = 0;
}

void UART_IMPL_ackTxIt(Uint8 uart_id) {
	assert(uart_id < UART_IMPL_NUM);
	if(uart_id == 0)
		IFS0bits.U1TXIF = 0;
	else
		IFS1bits.U2TXIF = 0;
}

bool_e UART_IMPL_isRxItTriggered(Uint8 uart_id) {
	assert(uart_id < UART_IMPL_NUM);
	if(uart_id == 0)
		return IFS0bits.U1RXIF;
	else
		return IFS1bits.U2RXIF;
}

bool_e UART_IMPL_isTxItTriggered(Uint8 uart_id) {
	assert(uart_id < UART_IMPL_NUM);
	if(uart_id == 0)
		return IFS0bits.U1TXIF;
	else
		return IFS1bits.U2TXIF;
}

__attribute__((weak)) void UART1_RX_Interrupt() {}
__attribute__((weak)) void UART1_TX_Interrupt() {}
__attribute__((weak)) void UART2_RX_Interrupt() {}
__attribute__((weak)) void UART2_TX_Interrupt() {}

__attribute__((weak)) void _ISR _U1RXInterrupt() {
	UART_IMPL_ackRxIt(UART1_ID);

	//On transforme l'IT en level triggered
	while(!UART_IMPL_isRxEmpty(UART1_ID) && _U1RXIE == 1)
		UART1_RX_Interrupt();
}
__attribute__((weak)) void _ISR _U1TXInterrupt() {
	UART_IMPL_ackTxIt(UART1_ID);

	//On transforme l'IT en level triggered
	while(!UART_IMPL_isTxFull(UART1_ID) && _U1TXIE == 1)
		UART1_TX_Interrupt();
}

__attribute__((weak)) void _ISR _U2RXInterrupt() {
	UART_IMPL_ackRxIt(UART2_ID);

	//On transforme l'IT en level triggered
	while(!UART_IMPL_isRxEmpty(UART2_ID) && _U2RXIE == 1)
		UART2_RX_Interrupt();
}
__attribute__((weak)) void _ISR _U2TXInterrupt() {
	UART_IMPL_ackTxIt(UART2_ID);

	//On transforme l'IT en level triggered
	while(!UART_IMPL_isTxFull(UART2_ID) && _U2TXIE == 1)
		UART2_TX_Interrupt();
}
