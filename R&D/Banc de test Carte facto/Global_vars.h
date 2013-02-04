/*
 *  Club Robot ESEO 2008 - 2010
 *  Archi-Tech'
 *
 *  Fichier : Global_config.h
 *  Package : Standard_Project
 *  Description : Variables globales définies specifiquement pour
					le code de la carte l'executant.
 *  Auteur : Jacen (inspiré du package QS d'Axel Voitier)
 *  Version 20081010
 */

#ifndef GLOBAL_VARS_H
	#define GLOBAL_VARS_H

	#ifndef QS_GLOBAL_VARS_H
		#error "Il est interdit d'inclure directement Global_vars.h, lire le CQS."
	#endif
		
	volatile bool_e flag_pwm1;
	volatile bool_e flag_pwm2;
	volatile bool_e flag_pwm3;
	volatile bool_e flag_pwm4;
	volatile bool_e flag_toutes_pwm;
	
		
	/*  Note : Variables globales communes à tous les codes
	 *
	 *	Buffer de reception de l'UART 1
	 *	Uint8 u1rxbuf[UART_RX_BUF_SIZE];
	 *
	 *	Buffer de reception de l'UART 2
	 *	Uint8 u2rxbuf[UART_RX_BUF_SIZE];
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
	 */
	 
	 
#endif /* ndef GLOBAL_VARS_H */
