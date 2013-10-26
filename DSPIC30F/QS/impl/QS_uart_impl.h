/*
 *  Club Robot ESEO 2013 - 2014
 *
 *  $Id$
 *
 *  Package : Qualit� Soft
 *  Description : Impl�mentation des fonctions UART pour dspic30f
 *  Auteur : amurzeau
 */


#ifndef QS_IMPL_UART_H_
#define QS_IMPL_UART_H_

#include "../QS_types.h"

#define UART1_ID 0
#define UART2_ID 1
#define UART3_ID 2

typedef enum {
	UART_I_StopBit_1,
	UART_I_StopBit_1_5,
	UART_I_StopBit_2
} UART_IMPL_stop_bits_e;

typedef enum {
	UART_I_Parity_None,
	UART_I_Parity_Odd,
	UART_I_Parity_Even
} UART_IMPL_parity_e;

// uart_id: num�ro de l'uart, 0 ou 1
// baudrate: vitesse, typiquement 9600
// rx_irq_priority: priorit� de l'interruption RX, si n�gatif, l'interruption est d�sactiv�e (de 0 � 15)
// tx_irq_priority: priorit� de l'interruption TX, si n�gatif, l'interruption est d�sactiv�e (de 0 � 15)
// Pas de parit�, 1 bit de stop
bool_e UART_IMPL_init(Uint8 uart_id, Uint32 baudrate, Sint8 rx_irq_priority, Sint8 tx_irq_priority);

//Comme UART_IMPL_init mais avec indication sur la parit� et le nombre de bits de stop
bool_e UART_IMPL_init_ex(Uint8 uart_id, Uint32 baudrate, Sint8 rx_irq_priority, Sint8 tx_irq_priority, UART_IMPL_stop_bits_e stop_bits, UART_IMPL_parity_e parity_mode);

/* Interruptions UART:
 * UART1_RX_Interrupt
 * UART1_TX_Interrupt
 * UART2_RX_Interrupt
 * UART2_TX_Interrupt
 * UART3_RX_Interrupt
 * UART3_TX_Interrupt
 *
 * Utilisez UART_IMPL_setRxItEnabled & UART_IMPL_setTxItEnabled pour les activer (d�sactiv� par d�faut)
 */


void   UART_IMPL_write(Uint8 uart_id, Uint8 data);
Uint8  UART_IMPL_read(Uint8 uart_id);
bool_e UART_IMPL_isRxEmpty(Uint8 uart_id);
bool_e UART_IMPL_isTxFull(Uint8 uart_id);
bool_e UART_IMPL_isIdle(Uint8 uart_id);
bool_e UART_IMPL_hasOverflow(Uint8 uart_id);
bool_e UART_IMPL_hasFrameError(Uint8 uart_id);
bool_e UART_IMPL_resetErrors(Uint8 uart_id);	//retourne TRUE si il y avait des erreurs

//Emp�che le declenchement d'une IT, pour eviter d'�tre pr�empt� pendant l'execution de code critique
void UART_IMPL_setRxItPaused(Uint8 uart_id, bool_e pause);
void UART_IMPL_setTxItPaused(Uint8 uart_id, bool_e pause);

//D�sactive ou active les IT. A utiliser quand on ne veut plus avoir l'IT du tout ne mani�re prolong� dans le temps (r�activer une IT ne la d�clenchera pas quoiqu'il arrive)
void UART_IMPL_setRxItEnabled(Uint8 uart_id, bool_e enable);
void UART_IMPL_setTxItEnabled(Uint8 uart_id, bool_e enable);

//Acquite une IT
void   UART_IMPL_ackRxIt(Uint8 uart_id);
void   UART_IMPL_ackTxIt(Uint8 uart_id);
bool_e UART_IMPL_isRxItTriggered(Uint8 uart_id);
bool_e UART_IMPL_isTxItTriggered(Uint8 uart_id);

#endif /* QS_IMPL_UART_H_ */
