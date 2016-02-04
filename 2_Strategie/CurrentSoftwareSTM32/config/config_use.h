/*
 *  Club Robot ESEO 2013 - 2014
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
	#warning 'ATTENTION CE MODE EST STRICTEMENT INTERDIT EN MATCH NE SOYEZ PAS INCONSCIENT!'
#endif

//#define MODE_SIMULATION		// N'utilise pas le CAN
#ifdef MODE_SIMULATION
	#define ROBOT_VIRTUEL_PARFAIT
	#warning 'ATTENTION CE MODE EST STRICTEMENT INTERDIT EN MATCH NE SOYEZ PAS INCONSCIENT!'
#endif

#define FAST_COS_SIN			//Calcul rapide des cos et sin à l'aide d'un GRAND tableau de valeur

#define USE_LCD					//Activation de l'écran LCD

#define USE_RTC					//Activation de la RTC

#define VERBOSE_MODE			//Activation du verbose

//#define FDP_2016				//Mode FDP nouvelle génération 2016 !

//#define USE_POLYGON			//Utilise le mode de déplacement avec les polygones
	//Si cette ligne est commentée, on utilise par défaut le mode de déplacement avec pathfind

#define USE_ASTAR				//Activation de l'algorithme ASTAR, avec pathfind par polygones

#define USE_PROP_MULTI_POINT	//Utilise le buffer de l'asser pour les trajectoires multi-points et permet d'utiliser les courbes

#define PROP_PULL_EVEN_WHEN_FAR_FROM_DESTINATION	//Les PROP_TRAJECTOIRE_FINIE dépilent les asser goto meme si on est loin de la destination

#define SD_ENABLE				//Activation de la carte SD

//#define USE_SYNC_ELEMENTS

// Utilisation de la carte Mosfets
#define USE_MOSFETS			// déclarer l'utilisation du pilote
#define NB_MOSFETS     5    //Nombre de mosfets à piloter (max = 8)

//#define DISABLE_WHO_AM_I	//Désactive la détection du robot.

//#define XBEE_SIMULATION  // Répete les messages XBEE sur le CAN
#ifdef XBEE_SIMULATION
	#warning 'ATTENTION CE MODE EST STRICTEMENT INTERDIT EN MATCH NE SOYEZ PAS INCONSCIENT!'
#endif

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
	#define SPI_R_BUF_SIZE 16

/* Réglages I2C */
	#define USE_I2C2

/* Réglages CAN */
	#define USE_CAN
	#define CAN_BUF_SIZE		32		//Nombre de messages CAN conservés pour traitement hors interuption
	#define CAN_SEND_TIMEOUT_ENABLE

/* Réglages UART */
	#define UART1_BAUDRATE	230400
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

	#define UART_RX_BUF_SIZE	32		//Taille de la chaine de caracteres memorisant les caracteres recus sur UART

/* Réglages Boutons */
	#define USE_BUTTONS
	//#define BUTTONS_TIMER 2			//Utilise le timer 2 pour les boutons
	#define BUTTONS_TIMER_USE_WATCHDOG

/* Réglages PWM */
	#define USE_PWM4
	//#define FREQ_PWM_50HZ
	#define FREQ_PWM_50KHZ


/* Réglages entrées analogiques */
	#define USE_AN8				// Capteur mesure 24V
	#define USE_AN_13           // Capteur laser gauche
	#define USE_AN_14           // Capteur laser droite
	#define ADC_CHANNEL_MEASURE24		ADC_8
	#define ADC_SENSOR_BIG_XUK_LEFT     ADC_13
	#define ADC_SENSOR_BIG_XUK_RIGHT    ADC_14

/* Réglages XBEE */
	#define USE_XBEE
	#define XBEE_PLUGGED_ON_UART2

/* Réglages FIFO */
	#define USE_FIFO

/* Réglages RF */
	#define USE_RF
	#define RF_UART UART3_ID
	#define RF_USE_WATCHDOG
	#define SYNCHROBALISE_TIMER 4

/* Récapitulatif TIMERs :
 * TIMER 1 : Clock			(clock.c/h)
 * TIMER 2 : ...
 * TIMER 3 : Watchdog		(QS_watchdog.c/h)
 * TIMER 4 : Synchro balise	(Synchro_balises.c/h)
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
