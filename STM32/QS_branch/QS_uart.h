/*
 *	Club Robot ESEO 2008 - 2015
 *	Archi-Tech' / Pierre & Guy / Holly & Wood
 *
 *	Fichier : QS_uart.h
 *	Package : Qualite Soft
 *	Description : fonction d'utilisation des uart pour
 *				interfacage rs232
 *	Auteur : Jacen / Alexis / Nirgal
 *	Licence : CeCILL-C (voir LICENCE.txt)
 *	Version 20100924
 */

/** ----------------  Defines possibles  --------------------
 *	USE_UART1					: Cr�er la fonction pour envoyer un message sur l'UART 1
 *	USE_UART2					: Cr�er la fonction pour envoyer un message sur l'UART 2
 *	USE_UART3					: Cr�er la fonction pour envoyer un message sur l'UART 3
 *	USE_UART4					: Cr�er la fonction pour envoyer un message sur l'UART 4
 *	USE_UART5					: Cr�er la fonction pour envoyer un message sur l'UART 5
 *	USE_UART6					: Cr�er la fonction pour envoyer un message sur l'UART 6
 *	USE_UART1RXINTERRUPT		: Cr�er la fonction pour recevoir un message sur l'UART 1
 *	USE_UART2RXINTERRUPT		: Cr�er la fonction pour recevoir un message sur l'UART 2
 *	USE_UART3RXINTERRUPT		: Cr�er la fonction pour recevoir un message sur l'UART 3
 *	USE_UART4RXINTERRUPT		: Cr�er la fonction pour recevoir un message sur l'UART 4
 *	USE_UART5RXINTERRUPT		: Cr�er la fonction pour recevoir un message sur l'UART 5
 *	USE_UART6RXINTERRUPT		: Cr�er la fonction pour recevoir un message sur l'UART 6
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
	void UART_deInit(void);
	void UART_set_baudrate(Uint8 uart_id, Uint32 baudrate);

	#ifdef USE_UART1

		/*	fonction envoyant un octet sur l'uart1 */
		void UART1_putc(Uint8 mes);
		#ifdef USE_UART1RXINTERRUPT
			/* fonction indiquant si il y a un octet � lire */
			bool_e UART1_data_ready();

			/*	fonction lisant le prochain octet � lire dans le
			 *	buffer de reception uart1
			 */
			Uint8 UART1_get_next_msg();
		#endif /* def USE_UART1RXINTERRUPT */

	#endif
	#ifdef USE_UART2
		/*	fonction envoyant un octet sur l'uart2 */
		void UART2_putc(Uint8 mes);
		#ifdef USE_UART2RXINTERRUPT
			/* fonction indiquant si il y a un octet � lire */
			bool_e UART2_data_ready();

			/*	fonction lisant le prochain octet � lire dans le
			 *	buffer de reception uart2
			 */
			Uint8 UART2_get_next_msg();
		#endif /* def USE_UART2RXINTERRUPT */
	#endif
	#ifdef USE_UART3
		/*	fonction envoyant un octet sur l'uart3 */
		void UART3_putc(Uint8 mes);
		#ifdef USE_UART3RXINTERRUPT
			/* fonction indiquant si il y a un octet � lire */
			bool_e UART3_data_ready();

			/*	fonction lisant le prochain octet � lire dans le
			 *	buffer de reception uart3
			 */
			Uint8 UART3_get_next_msg();
		#endif /* def USE_UART3RXINTERRUPT */
	#endif
	#ifdef USE_UART4
		/*	fonction envoyant un octet sur l'uart4 */
		void UART4_putc(Uint8 mes);
		#ifdef USE_UART4RXINTERRUPT
			/* fonction indiquant si il y a un octet � lire */
			bool_e UART4_data_ready();

			/*	fonction lisant le prochain octet � lire dans le
			 *	buffer de reception uart4
			 */
			Uint8 UART4_get_next_msg();
		#endif /* def USE_UART4RXINTERRUPT */
	#endif
	#ifdef USE_UART5
		/*	fonction envoyant un octet sur l'uart5 */
		void UART5_putc(Uint8 mes);
		#ifdef USE_UART5RXINTERRUPT
			/* fonction indiquant si il y a un octet � lire */
			bool_e UART5_data_ready();

			/*	fonction lisant le prochain octet � lire dans le
			 *	buffer de reception uart5
			 */
			Uint8 UART5_get_next_msg();
		#endif /* def USE_UART5RXINTERRUPT */
	#endif
	#ifdef USE_UART6
		/*	fonction envoyant un octet sur l'uart6 */
		void UART6_putc(Uint8 mes);
		#ifdef USE_UART6RXINTERRUPT
			/* fonction indiquant si il y a un octet � lire */
			bool_e UART6_data_ready();

			/*	fonction lisant le prochain octet � lire dans le
			 *	buffer de reception uart6
			 */
			Uint8 UART6_get_next_msg();
		#endif /* def USE_UART6RXINTERRUPT */
	#endif
#endif /* ndef QS_UART_H */
