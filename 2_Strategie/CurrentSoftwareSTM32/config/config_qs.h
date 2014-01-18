/*
 *  Club Robot ESEO 2013 - 2014
 *
 *  $Id$
 *
 *  Package : Carte Strategie
 *  Description : Configuration des modules de QS
 *  Auteur : Jacen, Alexis
 */

#ifndef CONFIG_QS_H
#define CONFIG_QS_H

	#define MODE_SIMULATION
#ifdef MODE_SIMULATION
	#warning 'ATTENTION CE MODE EST STRICTEMENT INTERDIT EN MATCH NE SOYEZ PAS INCONSCIENT!'
#endif
/* Pour certaines config particulieres, il faut definir qui on est
	 * a l'aide d'une des valeurs du type cartes_e de QS_types.h */
#define I_AM CARTE_STRAT
#define I_AM_CARTE_STRAT

/* Les instructions ci dessous définissent le comportement des
	 * entrees sorties du uc. une configuration en entree correspond
	 * a un bit a 1 (Input) dans le masque, une sortie a un bit a
	 * 0 (Output).
	 * Toute connection non utilisee doit etre configuree en entree
	 * (risque de griller ou de faire bruler le uc)
	 */

/* Il faut choisir à quelle frequence on fait tourner le STM32 */
#define HCLK_FREQUENCY_HZ     168000000	//40Mhz, Max: 168Mhz
#define PCLK1_FREQUENCY_HZ    42000000	//10Mhz, Max: 42Mhz
#define PCLK2_FREQUENCY_HZ    84000000	//40Mhz, Max: 84Mhz
#define CPU_EXTERNAL_CLOCK_HZ 8000000	//8Mhz, Fréquence de l'horloge externe


#define VERBOSE_MODE


//#define DISABLE_WHO_AM_I	//Désactive la détection du robot.

#define USE_WATCHDOG
#define WATCHDOG_TIMER 3
#define WATCHDOG_MAX_COUNT 5
#define WATCHDOG_QUANTUM 1

#define USE_SPI2
#define SPI_R_BUF_SIZE 16

#define USE_I2C2

#define USE_CAN
/*	Nombre de messages CAN conservés
	pour traitement hors interuption */
#define CAN_BUF_SIZE		32
#define CAN_SEND_TIMEOUT_ENABLE


#define UART1_BAUDRATE	115200
#define USE_UART1
#define USE_UART1RXINTERRUPT
#define BUFFER_U1TX_SIZE 1280
#define USE_UART1TXINTERRUPT

#define UART2_BAUDRATE	9600
#define USE_UART2
#define USE_UART2RXINTERRUPT
#define BUFFER_U2TX_SIZE 128
#define USE_UART2TXINTERRUPT

//#define USE_UART3
//#define UART3_BAUDRATE	19200
//#define USE_UART3RXINTERRUPT
//#define BUFFER_U3TX_SIZE 128
//#define USE_UART3TXINTERRUPT

/*	Taille de la chaine de caracteres memorisant
	les caracteres recus sur UART */
#define UART_RX_BUF_SIZE	32


//utilisation du module BUTTONS
#define USE_BUTTONS
//utilise le timer 2 pour les boutons
//#define BUTTONS_TIMER 2
#define BUTTONS_TIMER_USE_WATCHDOG


//#define FREQ_PWM_50HZ
#define FREQ_PWM_50KHZ


/* Utilisation des entrées analogiques */
#define USE_ANALOG_EXT_VREF

#define USE_AN8
#define ADC_CHANNEL_MEASURE24	0
#define USE_AN9 			// DT10 glasses sur TINY

#define USE_XBEE
#define XBEE_PLUGGED_ON_UART2

//#define USE_FIFO
//#define USE_RF
//#define RF_UART 3
//#define RF_MODULE RF_KRUSTY
//#define RF_SYNCHRO_MASTER
//#define RF_SYNCHRO_SLAVE

#endif /* CONFIG_QS_H */
