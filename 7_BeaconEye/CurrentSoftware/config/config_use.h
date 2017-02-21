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


	////////////////////////////////////////////////////////////////////
	////-------------------------MODE ET USE--------------------------//
	////////////////////////////////////////////////////////////////////

	#define USE_BEACON_EYE // Afin de savoir si nous sommes sur la baseboard ou non
	#ifndef USE_BEACON_EYE
		#warning "L'I2C utilisé sur la baseboard est différent de celui sur la beacon Eye... Ce define doit correspondre à la carte programmée pour que le tactile fonctionne."
	#endif

	#define VERBOSE_MODE			//Activation du verbose

	#define FAST_COS_SIN			//Calcul rapide des cos et sin à l'aide d'un GRAND tableau de valeu

	//#define CAN_VERBOSE_MODE		//Activation de la verbosité des messages CAN

	#define DISABLE_WHO_AM_I		//Désactive la détection du robot.

	#define USE_RGB_565				//Type d'encodage des images sur l'écran LCD




	////////////////////////////////////////////////////////////////////
	////----------------------------QS--------------------------------//
	////////////////////////////////////////////////////////////////////


/* Define pour identifier la carte */
	#define I_AM_CARTE_BEACON_EYE

/* Il faut choisir à quelle frequence on fait tourner le STM */
	#define HCLK_FREQUENCY_HZ		168000000	//168Mhz, Max: 168Mhz
	#define PCLK1_FREQUENCY_HZ		42000000	//42Mhz,  Max: 42Mhz
	#define PCLK2_FREQUENCY_HZ		84000000	//84Mhz,  Max: 84Mhz
	#define CPU_EXTERNAL_CLOCK_HZ	8000000		//8Mhz,   Fréquence de l'horloge externe


/* Réglages UART */
	#define USE_UART1
	#define USE_UART1RXINTERRUPT
	#define UART1_BAUDRATE			230400
	#define USE_UART1TXINTERRUPT
	#define BUFFER_U1TX_SIZE		128

	#define USE_UART2
	#define USE_UART2RXINTERRUPT
	#define UART2_BAUDRATE			9600

	#define UART_RX_BUF_SIZE		512		//Taille de la chaine de caracteres memorisant les caracteres recus sur UART

/* Réglages XBEE */
	#define USE_XBEE
	#define XBEE_ENABLE_PING_ALWAYS
	#define XBEE_PLUGGED_ON_UART2

/* Réglages HOKUYO */
#ifdef USE_BEACON_EYE
	#define USE_HOKUYO
	//#define USE_CCMRAM
#endif

/* Réglages WATCHDOG */
	#define USE_WATCHDOG
	#define WATCHDOG_TIMER 3
	#define WATCHDOG_MAX_COUNT 5
	#define WATCHDOG_QUANTUM 1

/* Réglages Boutons */
	#define USE_BUTTONS
	//#define BUTTONS_TIMER 2
	#define BUTTONS_TIMER_USE_WATCHDOG

/* Réglages entrées analogiques */
	#define USE_ANALOG_EXT_VREF

/* Réglages FIFO */
	#define USE_FIFO

/* Réglages I2C */
#ifdef USE_BEACON_EYE
	#define USE_I2C2
#else
	#define USE_I2C1
#endif

/* Réglages PWM */
	#define USE_PWM_MODULE
	#define PWM_FREQ	50000
	#define USE_PWM4

/* Réglages LCD */
	#define USE_IRQ_TOUCH_VALIDATION


#endif /* CONFIG_USE_H */
