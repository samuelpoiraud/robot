/*
 *  Club Robot ESEO 2008 - 2010
 *  Archi-Tech', PACMAN, KRUSTY & TINY
 *
 *  Fichier : Global_config.h
 *  Package : Standard_Project
 *  Description : Configuration des modules de QS
 *  Auteur : Jacen
 *  Version 20100415
 */

#ifndef GLOBAL_CONFIG_H
	#define GLOBAL_CONFIG_H
	#include "QS/QS_types.h"

	//#define FDP_2013	//Pour utilisation avec le fond de panier 2013
	
	/* Belle interface graphique !*/  	//Obsolète...
//	#define INTERFACE_GRAPHIQUE
	
	/* Le mode verbeux active debug_printf */
	#define VERBOSE_MODE

	/* Option pour tester le hard de la carte Interface SD */
	#define TEST_HARD

	/* Pour certaines config particulieres, il faut definir qui on est
	 * a l'aide d'une des valeurs du type cartes_e de QS_types.h */
	#define I_AM CARTE_SUPER
	#define I_AM_CARTE_SUPER

	/* Il faut choisir à quelle frequence on fait tourner le PIC */
	#define FREQ_10MHZ
	
	/* Les instructions ci dessous définissent le comportement des
	 * entrees sorties du pic. une configuration en entree correspond
	 * a un bit a 1 (Input) dans le masque, une sortie a un bit a
	 * 0 (Output).
	 * Toute connection non utilisee doit etre configuree en entree
	 * (risque de griller ou de faire bruler le pic) 
	 */

	#define PORT_A_IO_MASK	0x7FFF
		//LATA15 : reset XBee
	#define PORT_B_IO_MASK	0x00CF
		#define LED_PROP_OK		LATBbits.LATB5
		#define LED_PROP_KO		LATBbits.LATB4
		#define LED_ACT_OK		LATBbits.LATB15
		#define LED_ACT_KO		LATBbits.LATB14
		#define LED_STRAT_OK	LATBbits.LATB13
		#define LED_STRAT_KO	LATBbits.LATB12
		#define LED_BALISE_OK	LATBbits.LATB11
		#define	LED_BALISE_KO	LATBbits.LATB10
		#define	LED_ROBOT_OK	LATBbits.LATB9
		#define LED_ROBOT_KO	LATBbits.LATB8
		#define ALIM_WATCHER	PORTBbits.RB2
		#define USE_AN2
	#define PORT_C_IO_MASK	0xFFFF
	#define PORT_D_IO_MASK	0xC0FF
	#define PORT_E_IO_MASK	0xFCFF
		#define EEPROM_SPI2_HOLD	LATEbits.LATE8
		#define EEPROM_SPI2_WP		LATEbits.LATE9
		#define PORT_SWITCH_XBEE	PORTEbits.RE0
		#define PORT_SWITCH_EEPROM	PORTEbits.RE1
		#define LAT_ROBOT_ID_OUTPUT		LATEbits.LATE7
		#define TRIS_ROBOT_ID_OUTPUT	TRISEbits.TRISE7
		#define PORT_ROBOT_ID_INPUT		PORTEbits.RE5
		#define TRIS_ROBOT_ID_INPUT		TRISEbits.TRISE5
	#define PORT_F_IO_MASK	0xFFFF
	#define PORT_G_IO_MASK	0xFD53
		#define SWITCH_RG0		PORTGbits.RG0
		#define SWITCH_RG1		PORTGbits.RG1
		#define	RTC_I2C_SCL		PORTGbits.RG2
		#define	RTC_I2C_SDA		PORTGbits.RG3
		#define EEPROM_SPI2_SCK	PORTGbits.RG6
		#define EEPROM_SPI2_SDI	PORTGbits.RG7
		#define EEPROM_SPI2_SDO	PORTGbits.RG8
	 	#define EEPROM_SPI2_CS 	LATGbits.LATG9
	/* Les instructions suivantes permettent de configurer certainesF
	 * entrees/sorties du pic pour realiser des fonctionnalites
	 * particulieres comme une entree analogique 
	 */

	#define USE_CAN
//	#define USE_CAN2
/*	Nombre de messages CAN conservés 
	pour traitement hors interuption */
	#define CAN_BUF_SIZE		16

	#define USE_UART1
	#define USE_UART1RXINTERRUPT
	#define USE_UART2
	#define USE_UART2RXINTERRUPT

	#define USE_UART1TXINTERRUPT
	#define BUFFER_TX_SIZE (2048)
/*	Taille de la chaine de caracteres memorisant
	les caracteres recus sur UART */
	#define UART_RX_BUF_SIZE	42



	#define ENABLE_XBEE
	#ifdef ENABLE_XBEE
		#define XBEE_PLUGGED_ON_UART2
		
		#include "QS/QS_can_over_xbee.h"
		#define RESET_XBEE	LATAbits.LATA15
	#endif

	//Taille du buffer de mémorisation des caractères de l'interface texte ...
	#define TAILLE_BUFFER_INTERFACE_TEXTE 20
	
	
	//Taille du buffer de mémorisation des messages CAN pendant le match...
	//ATTENTION : il faut qu'il soit grand... mais pas trop, sinon il n'y a plus assez de place pour la pile...
	#define BUFFER_SIZE		20
	
	/* utilisation du module de watchdog */
	#define USE_WATCHDOG
	/* timer utilisé pour le watchdog (1, 2, 3 ou 4) */
	#define WATCHDOG_TIMER		2
	/* granularité du temps en ms */
	#define WATCHDOG_QUANTUM	1
	/* nombre maximal de watchdogs en exécution */
	#define WATCHDOG_MAX_COUNT  3	

	//activation du module BUTTONS
	#define USE_BUTTONS
	//timer 3 pour le module BUTTONS
	#define BUTTONS_TIMER 3
	
	//Activation du module d'enregistrement des messages CAN reçus pendant les matchs en mémoire EEPROM.
	#define EEPROM_CAN_MSG_ENABLE
	
	#ifdef EEPROM_CAN_MSG_ENABLE
		#define USE_SPI2
		#define SPI_R_BUF_SIZE 16
	#endif
	
	#define USE_I2C
	
#endif /* ndef GLOBAL_CONFIG_H */
