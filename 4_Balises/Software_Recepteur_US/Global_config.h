/*
 *  Club Robot ESEO 2008 - 2009
 *  Archi'Tech, PACMAN
 *
 *  Fichier : Global_config.h
 *  Package : Standard_Project
 *  Description : Configuration des modules de QS
 *  Auteur : Jacen
 *  Version 20090223
 */

#include "QS/QS_config_doc.h"

#ifndef GLOBAL_CONFIG_H
	#define GLOBAL_CONFIG_H
	#include "QS/QS_types.h"

	/* Le mode verbeux active debug_printf */
	#define VERBOSE_MODE

	//Envoi des msg can sur l'UART1 (format standard...)
	//#define CAN_SEND_OVER_UART1

	//Envoi textuel des msg can sur l'UART1 (compréhensible par un humain normal)
	//#define VERBOSE_CAN_OVER_UART1

	#define MY_BEACON_ID 	BEACON_ID_MOTHER	
	//#define MY_BEACON_ID 	BEACON_ID_CORNER	
	//#define MY_BEACON_ID 	BEACON_ID_MIDLE  	
	//#define MY_BEACON_ID 	BEACON_ID_ROBOT_1 	
	//#define MY_BEACON_ID 	BEACON_ID_ROBOT_2 	
	
	/* Pour certaines config particulieres, il faut definir qui on est
	 * a l'aide d'une des valeurs du type cartes_e de QS_types.h */
	#define I_AM CARTE_BALISE
	#define I_AM_CARTE_BALISE
	
	/* Il faut choisir à quelle frequence on fait tourner le PIC */
	#define FREQ_10MHZ
	//#define FREQ_INTERNAL_CLK
	
	/* Les instructions ci dessous définissent le comportement des
	 * entrees sorties du pic. une configuration en entree correspond
	 * a un bit a 1 (Input) dans le masque, une sortie a un bit a
	 * 0 (Output).
	 * Toute connection non utilisee doit etre configuree en entree
	 * (risque de griller ou de faire bruler le pic) 
	 */

/* Masquage à effectuer lorsque les nouvelles cartes actionneurs seront arrivées */

	#define PORT_A_IO_MASK	0xFFFF		
	#define PORT_B_IO_MASK	0xFFFF
	#define PORT_C_IO_MASK	0xFFFF
	#define PORT_D_IO_MASK	0xF0FF
	#define PORT_E_IO_MASK	0xFFFF
		/*  Bouton-poussoir de test */
		#define PUSHBUTTON	!PORTEbits.RE9
	#define PORT_F_IO_MASK	0xFFFF
	#define PORT_G_IO_MASK	0xFFFF

	#define USE_SPI2
	#define SPI_R_BUF_SIZE 		16
	
	/* CAN */
	#define USE_CAN
	#define CAN_SEND_TIMEOUT_ENABLE
	#define CAN_BUF_SIZE		16
	#define QS_CAN_RX_IT_PRI	CAN_INT_PRI_5
	#define DISABLE_BROADCAST_FILTER
	
	/* Réglages UART */
	#define USE_UART1
	#define USE_UART1RXINTERRUPT
	#define UART_RX_BUF_SIZE	12
	
	/* Réglages PWM */
	//#define DISABLE_PWM4H
	#define FREQ_PWM_40KHZ
	#define USE_PWM_H_AND_L
	
	#define USE_AN8  //utilisation de AN8 du module ADC
	#define USE_AN15
	//#define ADC_MODE_FRACTIONAL
	
#endif /* ndef GLOBAL_CONFIG_H */
