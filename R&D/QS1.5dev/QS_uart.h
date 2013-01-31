/*
 *	Club Robot ESEO 2008 - 2009
 *	Archi-Tech'
 *
 *	Fichier : QS_uart.h
 *	Package : Qualite Soft
 *	Description : fonction d'utilisation des uart pour 
 *				interfacage rs232
 *	Auteur : Jacen
 *	Version 20081130
 */

#ifndef QS_UART_H
	#define QS_UART_H

	#include "QS_all.h"
	#if !(defined(USE_UART1) || defined(USE_UART2) || defined(QS_UART_C))
		#error "QS_uart.h inclus sans declaration d'utilisation d'un uart"
	#endif

	/*	fonction initialisant les uart choisis
		vitesse : 9600 bauds
		bits de donnees : 8
		parite : aucune
		bit de stop : 1
		pas de controle de flux
	*/
	void UART_init(void);

	#ifdef USE_UART1

		/*	fonction envoyant un octet sur l'uart1 */
		void UART1_putc(Uint8 mes);
		#ifdef USE_UART1RXINTERRUPT
			/*	fonction lisant le prochain octet à lire dans le
			 *	buffer de reception uart1. Ne prends pas en compte
			 *	les lectures du buffer ne passant pas par elle.
			 */
			Uint8 UART1_get_next_msg();
		#endif /* def USE_UART1RXINTERRUPT */

	#endif
	#ifdef USE_UART2
		/*	fonction envoyant un octet sur l'uart2 */
		void UART2_putc(Uint8 mes);
		#ifdef USE_UART2RXINTERRUPT
			/*	fonction lisant le prochain octet à lire dans le
			 *	buffer de reception uart1. Ne prends pas en compte
			 *	les lectures du buffer ne passant pas par elle.
			 */
			Uint8 UART2_get_next_msg();
		#endif /* def USE_UART2RXINTERRUPT */
	#endif
	
	
	#ifdef QS_UART_C
		#include <uart.h>
	#endif /* def QS_UART_C */


#endif /* ndef QS_UART_H */
