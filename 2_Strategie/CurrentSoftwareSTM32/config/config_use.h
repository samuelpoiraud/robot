/*
 *  Club Robot ESEO 2013 - 2017
 *
 *  $Id$
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

//#define ROBOT_VIRTUEL_PARFAIT		// Afin que les actionneurs et certaines fonction en strat renvoi true
#ifdef ROBOT_VIRTUEL_PARFAIT
	#define ACT_NO_ERROR_HANDLING
	#warning 'ATTENTION CE MODE EST STRICTEMENT INTERDIT EN MATCH NE SOYEZ PAS INCONSCIENT!'
#endif

//#define MODE_SIMULATION		// N'utilise pas le CAN
#ifdef MODE_SIMULATION
	#define ROBOT_VIRTUEL_PARFAIT
	#warning 'ATTENTION CE MODE EST STRICTEMENT INTERDIT EN MATCH NE SOYEZ PAS INCONSCIENT!'
#endif

//#define DISABLE_WHO_AM_I	//Désactive la détection du robot.
#ifdef DISABLE_WHO_AM_I
	#warning 'ATTENTION CE MODE EST STRICTEMENT INTERDIT EN MATCH NE SOYEZ PAS INCONSCIENT!'
#endif

//#define XBEE_SIMULATION  // Répete les messages XBEE sur le CAN
#ifdef XBEE_SIMULATION
	#warning 'ATTENTION CE MODE EST STRICTEMENT INTERDIT EN MATCH NE SOYEZ PAS INCONSCIENT!'
#endif

//#define DISABLE_SECURE_GPIO_INIT	//Peut être utile sur d'anciens FdP.
#ifdef DISABLE_SECURE_GPIO_INIT
	#warning 'ATTENTION SECURITE DU GPIO DESACTIVEE'
#endif

#define FAST_COS_SIN			//Calcul rapide des cos et sin à l'aide d'un GRAND tableau de valeur

#define USE_LCD					//Activation de l'écran LCD

#define USE_RTC					//Activation de la RTC

#define VERBOSE_MODE			//Activation du verbose

//#define DISPLAY_STOCKS		// Activation de l'affichage des STCOKS pour debug

//#define USE_FOE_ANALYSER

#define USE_ASTAR				//Activation de l'algorithme ASTAR, avec pathfind par polygones

#define SD_ENABLE				//Activation de la carte SD

#define USE_SYNC_ELEMENTS
//#define USE_HARDFLAGS


//////////////////////////////////////////////////////////////////
//----------------------------QS--------------------------------//
//////////////////////////////////////////////////////////////////

/* Define pour identifier la carte */
	#define I_AM_CARTE_STRAT

/* Il faut choisir à quelle frequence on fait tourner la STM32 */
	#define HCLK_FREQUENCY_HZ		168000000	//168Mhz, Max: 168Mhz
	#define PCLK1_FREQUENCY_HZ		42000000	//42Mhz,  Max: 42Mhz
	#define PCLK2_FREQUENCY_HZ		84000000	//84Mhz,  Max: 84Mhz
	#define CPU_EXTERNAL_CLOCK_HZ	8000000		//8Mhz,   Fréquence de l'horloge externe

/* Réglages WATCHDOG */
	#define USE_WATCHDOG
		#define WATCHDOG_TIMER 3
		#define WATCHDOG_MAX_COUNT 5
		#define WATCHDOG_QUANTUM 1

/* Réglages SPI */
	#define USE_SPI2

/* Réglages I2C */
	#define USE_I2C2

/* Réglages CAN */
	#define USE_CAN
		#define CAN_BUF_SIZE		32		//Nombre de messages CAN conservés pour traitement hors interuption
		#define CAN_SEND_TIMEOUT_ENABLE

/* Réglages UART */
	#define USE_UART1
		#define UART1_BAUDRATE				230400

		#define USE_UART1_TX_BUFFER
			#define UART1_TX_BUFFER_SIZE 	1280

		#define USE_UART1_RX_BUFFER
			#define UART1_RX_BUFFER_SIZE 	32

	#define USE_UART2
		#define UART2_BAUDRATE				9600

		#define USE_UART2_TX_BUFFER
			#define UART2_TX_BUFFER_SIZE 	128

		#define USE_UART2_RX_BUFFER
			#define UART2_RX_BUFFER_SIZE 	32

	#ifdef USE_LCD
		#define USE_UART3
			#define UART3_BAUDRATE				9600

			#define USE_UART3_TX_BUFFER
				#define UART3_TX_BUFFER_SIZE 	128

			#define USE_UART3_RX_BUFFER
				#define UART3_RX_BUFFER_SIZE	256
	#endif

/* Réglages Boutons */
	#define USE_BUTTONS
	//#define BUTTONS_TIMER 2			//Utilise le timer 2 pour les boutons
	#define BUTTONS_TIMER_USE_WATCHDOG

/* Réglages PWM */
	#define USE_PWM_MODULE
		#define PWM_FREQ	50000
		#define USE_PWM4

/* Réglages de la carte Mosfets */
	//#define USE_MOSFETS_MODULE
	//	#define USE_MOSFET_1

/* Réglages LCD via UART */
	#ifdef USE_LCD
		#define USE_LCD_OVER_UART
			#define LCD_OVER_UART__UART_ID	3
	#endif

#ifdef MAIN_IR_RCVA
	#define USE_PWM1
	#define USE_AN9				// PB1...
	#define ADC_PHOTOTRANSISTOR		ADC_9

	#define ADC_CHANNEL_MEASURE24		ADC_9	//UNUSED !!
#else

/* Réglages entrées analogiques */
	#define USE_AN8				// Capteur mesure 24V
		#define ADC_CHANNEL_MEASURE24		ADC_8
#endif

/* Réglages XBEE */
	#define USE_XBEE_OLD
		#define XBEE_PLUGGED_ON_UART2

/* Réglages FIFO */
	#define USE_FIFO

/* Réglages Capteur couleur */
	#define USE_CW_SENSOR

/* Récapitulatif TIMERs :
 * TIMER 1 : Clock			(clock.c/h)
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
 * 10 : USART_3
 *  9 : TIMER_1
 *  7 : I2C
 *  3 : USART_2
 *  3 : USART_1
 *  1 : CAN
 */

#endif /* CONFIG_USE_H */
