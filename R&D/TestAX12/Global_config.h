/*
 *	Club Robot ESEO 2010 - 2011
 *	Check Norris
 *
 *  Fichier : Global_config.h
 *  Package : Actionneur
 *  Description : Configuration de la carte actionneur
 *  Auteur : Aurélien
 *  Version 20110225
 */

#ifndef GLOBAL_CONFIG_H
	#define GLOBAL_CONFIG_H
	#include "QS/QS_types.h"

//	#define VERBOSE_MODE

	/* ID de la carte: cf le type cartes_e de QS_types.h */
	#define I_AM CARTE_ACT		//A voir avec Gwenn pour changer
	#define I_AM_CARTE_ACT
	
	/* Il faut choisir à quelle frequence on fait tourner le PIC */
	#define FREQ_10MHZ
	
	/* Les instructions ci dessous définissent le comportement des
	 * entrees sorties du pic. une configuration en entree correspond
	 * a un bit a 1 (Input) dans le masque, une sortie a un bit a
	 * 0 (Output).
	 * Toute connection non utilisee doit etre configuree en entree
	 * (risque de griller ou de faire bruler le pic) 
	 */

	#define PORT_A_IO_MASK	0xFFFF		
	#define PORT_B_IO_MASK	0xFFFF
	#define PORT_C_IO_MASK	0xFFFF
	#define PORT_D_IO_MASK	0x30FF
	#define PORT_E_IO_MASK	0xFFFF
		
	#define PORT_F_IO_MASK	0xFFFF
	#define PORT_G_IO_MASK	0xFEFF

/* Réglages UART */
	#define USE_UART1
	#define USE_UART1RXINTERRUPT
	#define UART_RX_BUF_SIZE	12
	
/* AX-12 */
	#define USE_AX12_SERVO
	#define AX12_NUMBER 1
	#define AX12_DIRECTION_PORT _RG8
        #define AX12_STATUS_RETURN_MODE AX12_STATUS_RETURN_ONREAD
        #define AX12_TIMER_ID 1
        #define AX12_UART_BAUDRATE 56700

        #define USE_BUTTONS
        #define BUTTONS_TIMER 3

#endif /* ndef GLOBAL_CONFIG_H */
