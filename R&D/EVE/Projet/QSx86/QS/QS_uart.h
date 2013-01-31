/*
 *	PFE Simulateur QSx86 2011-2012
 *	Logiciel EVE
 *
 *	Fichier : QS_uart.h
 *	Package : QSx86
 *	Description : Fonctions d'utilisation UART pour interfaçage RS232
 *	Auteurs : Julien Franchineau & François Even
 *	Version 20111206
 */

#ifndef QS_UART_H
	#define QS_UART_H

	#include "QS_all.h"

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
			/* fonction indiquant si il y a un octet à lire */
			bool_e UART1_data_ready();
			
			/*	fonction lisant le prochain octet à lire dans le
			 *	buffer de reception uart1
			 */
			Uint8 UART1_get_next_msg();
		#endif /* def USE_UART1RXINTERRUPT */

	#endif
	#ifdef USE_UART2
		/*	fonction envoyant un octet sur l'uart2 */
		void UART2_putc(Uint8 mes);
		#ifdef USE_UART2RXINTERRUPT
			/* fonction indiquant si il y a un octet à lire */
			bool_e UART2_data_ready();
			
			/*	fonction lisant le prochain octet à lire dans le
			 *	buffer de reception uart2
			 */
			Uint8 UART2_get_next_msg();
		#endif /* def USE_UART2RXINTERRUPT */
	#endif

#endif /* ndef QS_UART_H */
