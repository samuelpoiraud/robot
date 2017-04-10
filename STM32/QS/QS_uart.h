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
 *	USE_UARTx					: Créer la fonction pour envoyer un message sur l'UART x
 *
 *	USE_UARTx_RX_INTERRUPT		: Réception via un listenner
 *
 *	USE_UARTx_RX_BUFFER			: Réception via un buffer
 *		UARTx_RX_BUFFER_SIZE	: Taille du buffer
 *
 *	USE_UARTx_TX_BUFFER			: Envoi via un buffer
 *		UARTx_TX_BUFFER_SIZE	: Taille du buffer
 *
 *	USE_UARTx_FLOWCONTROL		: Ajout le contrôle de flux hardware à l'UART x
 *
 ** ----------------  Choses à savoir  --------------------
 *
 *	x : de 1 à 6
 */

#ifndef QS_UART_H
	#define QS_UART_H

	#include "QS_all.h"

	typedef void (*UART_dataReceivedFunctionPtr)(Uint8 Byte);

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
	void UART_setListenner(Uint8 uart_id, UART_dataReceivedFunctionPtr function);


	#ifdef USE_UART1
		/*	fonction envoyant un octet sur l'uart1 */
		void UART1_putc(Uint8 data);

		#ifdef USE_UART1_RX_BUFFER
			/* fonction indiquant si il y a un octet à lire */
			bool_e UART1_data_ready(void);

			/*	fonction lisant le prochain octet à lire dans le
			 *	buffer de reception uart1
			 */
			Uint8 UART1_get_next_msg(void);
		#endif /* def USE_UART1_RX_BUFFER */

	#endif

	#ifdef USE_UART2
		/*	fonction envoyant un octet sur l'uart2 */
		void UART2_putc(Uint8 data);

		#ifdef USE_UART2_RX_BUFFER
			/* fonction indiquant si il y a un octet à lire */
			bool_e UART2_data_ready(void);

			/*	fonction lisant le prochain octet à lire dans le
			 *	buffer de reception uart2
			 */
			Uint8 UART2_get_next_msg(void);
		#endif /* def USE_UART2_RX_BUFFER */
	#endif

	#ifdef USE_UART3
		/*	fonction envoyant un octet sur l'uart3 */
		void UART3_putc(Uint8 data);

		#ifdef USE_UART3_RX_BUFFER
			/* fonction indiquant si il y a un octet à lire */
			bool_e UART3_data_ready(void);

			/*	fonction lisant le prochain octet à lire dans le
			 *	buffer de reception uart3
			 */
			Uint8 UART3_get_next_msg(void);
		#endif /* def USE_UART3_RX_BUFFER */
	#endif

	#ifdef USE_UART4
		/*	fonction envoyant un octet sur l'uart4 */
		void UART4_putc(Uint8 data);

		#ifdef USE_UART4_RX_BUFFER
			/* fonction indiquant si il y a un octet à lire */
			bool_e UART4_data_ready(void);

			/*	fonction lisant le prochain octet à lire dans le
			 *	buffer de reception uart4
			 */
			Uint8 UART4_get_next_msg(void);
		#endif /* def USE_UART4_RX_BUFFER */
	#endif

	#ifdef USE_UART5
		/*	fonction envoyant un octet sur l'uart5 */
		void UART5_putc(Uint8 data);

		#ifdef USE_UART5_RX_BUFFER
			/* fonction indiquant si il y a un octet à lire */
			bool_e UART5_data_ready(void);

			/*	fonction lisant le prochain octet à lire dans le
			 *	buffer de reception uart5
			 */
			Uint8 UART5_get_next_msg(void);
		#endif /* def USE_UART5_RX_BUFFER */
	#endif

	#ifdef USE_UART6
		/*	fonction envoyant un octet sur l'uart6 */
		void UART6_putc(Uint8 data);

		#ifdef USE_UART6_RX_BUFFER
			/* fonction indiquant si il y a un octet à lire */
			bool_e UART6_data_ready(void);

			/*	fonction lisant le prochain octet à lire dans le
			 *	buffer de reception uart6
			 */
			Uint8 UART6_get_next_msg(void);
		#endif /* def USE_UART6_RX_BUFFER */
	#endif
#endif /* ndef QS_UART_H */
