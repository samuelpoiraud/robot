/*
 *  Club Robot ESEO 2009-2010
 *  
 *
 *  Fichier : Global_vars.h
 *  Package : Balises 2010
 *  Description : Variables globales d�finies specifiquement pour
					le code de la carte l'executant.
 *  Auteur : Jacen (inspir� du package QS d'Axel Voitier)
 *  Version 200907
 */

#ifndef GLOBAL_VARS_H
	#define GLOBAL_VARS_H

	#ifndef QS_GLOBAL_VARS_H
		#error "Il est interdit d'inclure directement Global_vars.h, lire le CQS."
	#endif
	
	/*  Note : Variables globales communes � tous les codes
	 *
	 *	Buffer de reception de l'UART 1
	 *	Uint8 u1rxbuf[UART_RX_BUF_SIZE];
	 *	Position du dernier octet recu
	 *	volatile Uint32 u1rxnum;
	 *
	 *	Buffer de reception de l'UART 2
	 *	Uint8 u2rxbuf[UART_RX_BUF_SIZE];
	 *	Position du dernier octet recu
	 *	volatile Uint32 u2rxnum;
	 *
	 *	Buffer de reception des messages du bus CAN
	 *	CAN_msg_t can_buffer[CAN_BUF_SIZE];
	 *	Position du dernier message recu
	 *	volatile Uint16 can_rx_num;
	 *
	 *	Buffer de reception des messages du bus CAN2
	 *	CAN_msg_t can2_buffer[CAN_BUF_SIZE];
	 *	Position du dernier message recu
	 *	volatile Uint16 can2_rx_num;
	 *	
	 *	Tableau des r�sulats du dernier echantillonage.
	 *	Uint16 ADCResult[16];
	 *	
	 */		
	bool_e flag_100ms;
	
#endif /* ndef GLOBAL_VARS_H */
