/*
 *  Club Robot ESEO 2013 - 2014
 *
 *  $Id: config_use.h 2706 2014-10-04 13:06:44Z aguilmet $
 *
 *  Package : Carte Strategie
 *  Description : Activation de modules et fonctionnalités
 *  Auteur : Jacen, Alexis
 */

#ifndef CONFIG_USE_H
#define CONFIG_USE_H

//////////////////////////////////////////////////////////////////
//-------------------------MODE ET USE--------------------------//
//////////////////////////////////////////////////////////////////

#define VERBOSE_MODE			//Activation du verbose

#define CAN_VERBOSE_MODE		//Activation de la verbosité des messages CAN

//#define USE_UART_OVER_LCD
//	#define UART_OVER_LCD__UART_ID 3

//////////////////////////////////////////////////////////////////
//----------------------------QS--------------------------------//
//////////////////////////////////////////////////////////////////

/* Define pour identifier la carte */
	#define I_AM_CARTE_IHM

/* Il faut choisir à quelle frequence on fait tourner le PIC */
	#define HCLK_FREQUENCY_HZ		168000000	//168Mhz, Max: 168Mhz
	#define PCLK1_FREQUENCY_HZ		42000000	//42Mhz,  Max: 42Mhz
	#define PCLK2_FREQUENCY_HZ		84000000	//84Mhz,  Max: 84Mhz
	#define CPU_EXTERNAL_CLOCK_HZ	8000000		//8Mhz,   Fréquence de l'horloge externe

/* CAN */
	#define USE_CAN
	#define CAN_BUF_SIZE		32	//Nombre de messages CAN conservés pour traitement hors interuption
	#define CAN_SEND_TIMEOUT_ENABLE

/* UART */
	#define USE_UART1
		#define UART1_BAUDRATE	230400
		#define USE_UART1RXINTERRUPT
		#define USE_UART1TXINTERRUPT
		#define BUFFER_U1TX_SIZE	128


//	#define USE_UART3
//		#define UART3_BAUDRATE	19200
//		#define USE_UART3RXINTERRUPT
//		#define USE_UART3TXINTERRUPT
//		#define BUFFER_U3TX_SIZE	128

	#define UART_RX_BUF_SIZE	512 //Taille de la chaine de caracteres memorisant les caracteres recus sur UART

/* SPI */
	#define USE_SPI2
	#define SPI2_ON_DMA

/* Réglages watchdog */
	#define USE_WATCHDOG
		#define WATCHDOG_TIMER 3
		#define WATCHDOG_MAX_COUNT 5
		#define WATCHDOG_QUANTUM 1

/* Réglages entrées analogiques */
	#define USE_AN5		// Mesure 12V hokuyo
	#define USE_AN6		// Mesure 24V puissance
	#define USE_AN10	// Mesure 24V permanence

/* Réglages LCD */
	#define LCD_DMA
	#define USE_IRQ_TOUCH_VALIDATION

/* Récapitulatif TIMERs :
 * TIMER 1 : IT				(it.c/h)
 * TIMER 2 : ...
 * TIMER 3 : Watchdog		(QS_watchdog.c/h)
 * TIMER 4 : ...
 * TIMER 5 : ...
 */

/* Récapitulatif IT priorité :
 * 15 : TIMER_5
 * 14 : TIMER_4
 * 13 : TIMER_3
 * 11 : TIMER_2
 *  9 : TIMER_1
 *  3 : USART_1
 *  1 : CAN
 *  0 : Systick
 */

#endif /* CONFIG_USE_H */
