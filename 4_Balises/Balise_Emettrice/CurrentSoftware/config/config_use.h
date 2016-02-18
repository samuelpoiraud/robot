/*
 *  Club Robot ESEO 2015
 *
 *  Package : Balise émettrice
 *  Description : Activation de modules et fonctionnalités
 *  Auteur : Arnaud
 */

#ifndef CONFIG_USE_H
	#define CONFIG_USE_H

//////////////////////////////////////////////////////////////////
//-------------------------MODE ET USE--------------------------//
//////////////////////////////////////////////////////////////////


	#define NOMBRE_BALISES_EMETTRICES 	(2)

	#define USE_HOKUYO					//Active le module HOKUYO et la détection des ennemis... !

	#define VERBOSE_MODE				//Activation du verbose

	#define CAN_VERBOSE_MODE			//Activation de la verbosité des message CAN

	//#define DISABLE_WHO_AM_I	//Désactive la détection du robot.


//////////////////////////////////////////////////////////////////
//----------------------------QS--------------------------------//
//////////////////////////////////////////////////////////////////

/* Define pour identifier la carte */
	#define I_AM_CARTE_BALISE

/* Il faut choisir à quelle frequence on fait tourner la STM32 */
	#define HCLK_FREQUENCY_HZ		168000000	//168Mhz, Max: 168Mhz
	#define PCLK1_FREQUENCY_HZ		42000000	//42Mhz,  Max: 42Mhz
	#define PCLK2_FREQUENCY_HZ		84000000	//84Mhz,  Max: 84Mhz
	#define CPU_EXTERNAL_CLOCK_HZ	8000000		//8Mhz,   Fréquence de l'horloge externe

/* CAN */
	#define USE_CAN
	#define CAN_BUF_SIZE		32	//Nombre de messages CAN conservés pour traitement hors interuption

/* Réglages UART */
	#define USE_UART1
	#define USE_UART1RXINTERRUPT
	#define UART1_BAUDRATE		230400
	#define USE_UART1TXINTERRUPT
	#define BUFFER_U1TX_SIZE	128

	#define UART_RX_BUF_SIZE	512	//Taille de la chaine de caracteres memorisant les caracteres recus sur UART

/* Réglages watchdog */
	#define USE_WATCHDOG
	#define WATCHDOG_TIMER 4
	#define WATCHDOG_MAX_COUNT 5
	#define WATCHDOG_QUANTUM 1

/* Réglages entrées analogiques */
	#define USE_AN8				// Mesure tension batterie
	#define ADC_CHANNEL_MEASURE_BAT		ADC_8

/* Réglages PWM */
	#define USE_PWM1	//Contrôle forward
		#define PWM_FORWARD		1
	#define USE_PWM2	//Contrôle reverse
		#define PWM_REVERSE		2
	//#define FREQ_PWM_50HZ
	//#define FREQ_PWM_1KHZ
	//#define FREQ_PWM_10KHZ
	//#define FREQ_PWM_20KHZ
	#define FREQ_PWM_50KHZ

/* Réglages FIFO */
	#define USE_FIFO

/* Réglages RF */
	#define USE_RF
	#define RF_UART				UART3_ID
	#define RF_USE_WATCHDOG
	#define SYNCHROBALISE_TIMER	4
	#define RF_CAN_BUF_SIZE		32

/* Réglages QEI */
	#define USE_QUEI1

/* Récapitulatif TIMERs :
 * TIMER 1 : IT principale		(it.c/h)
 * TIMER 2 : Modulation 3.3KHz
 * TIMER 3 : ...
 * TIMER 4 : Watchdog		(QS_watchdog.c/h)
 * TIMER 5 : ...
 */

/* Récapitulatif IT priorité :
 * 15 : TIMER_5
 * 14 : TIMER_4
 * 13 : TIMER_3
 * 11 : TIMER_2
 *  9 : TIMER_1
 *  3 : USART_1
 *  2 : CAN
 *  1 : USB
 *  0 : Systick
 */

#endif /* CONFIG_USE_H */
