/*
 *  Club Robot ESEO 2015
 *
 *  Package : Carte Balise réceptrice
 *  Description : Activation de modules et fonctionnalités
 *  Auteur : Arnaud
 */

#ifndef CONFIG_USE_H
#define CONFIG_USE_H

//////////////////////////////////////////////////////////////////
//-------------------------MODE ET USE--------------------------//
//////////////////////////////////////////////////////////////////

	//#define USE_HOKUYO					//Active le module HOKUYO et la détection des ennemis... !

	#define VERBOSE_MODE				//Activation du verbose

	//#define CAN_VERBOSE_MODE			//Activation de la verbosité des message CAN

	//#define DISABLE_WHO_AM_I			//Désactive la détection du robot.

	//#define CONFIG_RF_RC1240			//Activation de la configuration du module Radiocrafts RC1240

	//#define DEBUG_TSOP_MATLAB

///************************************************************************



	#define MINIMUM_DURATION_BETWEEN_TWO_DETECTIONS 11// =1,43ms //[130us] si l'on a rien reçu pendant cette durée, on inaugure la détection suivante.  Unité : [TMR3]


	//Période d'échantillonage des réceptions.
	//Attention, si trop petit le PIC est pas assez rapide pour éxécuter l'IT...
	//la frequence minimum vitale est 6,66kHz = 150µs...(shannon).... (=2*période de l'émetteur)
	//je choisis environ 7,7kHz	 > 6,66... il fauat être sur qu'un éventuel rapport cyclique !=50% ne nous gène pas...
	//Ensuite, on compte chaque changement d'état...
	//plus d'infos ? >> samuelp5@gmail.com
	#define	PERIOD_IT_GET_TSOP_US		130			// [µs]

	#define PERIOD_IT_SECRETARY			100			// [ms]

	#define PERIOD_IT_LEDS_MUX			250			// [µs]
	#define PERIOD_IT_LEDS_MANAGER		10000		// [µs]

	//Durée minimale entre deux envois vers la carte stratégie...
	#define DUREE_MINI_ENTRE_DEUX_ENVOIS 250	//ms (maximum = 255 !)

	//pour info, utilisé dans le code, figé et imposé par la carte.
	//Le code n'est pas cependant suffisamment générique pour permette que cette seule modifications soit répercutés aux endroits concernés...
	#define NOMBRE_TSOP 				16


	#define SEUIL_SIGNAL_TACHE			10
	//TODO regler !!! > si un récepteur à reçu une "puissance" d'au moins ceci, il peut appartenir à la tache


	#define SEUIL_SIGNAL_INSUFFISANT	4
	//TODO regler !!! > si aucun récepteur n'a recu une "puissance" d'au moins ceci, on déclare l'état d'erreur de signal insuffisant !


	#define SEUIL_TROP_DE_SIGNAL		300
	// > quel maximum a t'on au plus près pour un récepteur
	//Si un récepteur à recu plus que ce signal en un cycle, on lève l'état d'erreur de TROP_DE_SIGNAL !


	#define TAILLE_TACHE_MAXIMALE		12
	// > combien de récepteurs captent au plus près
	//Si ce nombre de récepteur ont recu du signal (ils appartiennent tous à la tache) >> état d'erreur levé !

	#define PRODUIT_DISTANCE_EVIT_PUISSANCE_EVIT	10800
	//  = "distance d'évitement souhaitée" * "PuissanceReçue mesurée à cette distance"
	//Ce coefficient permet la mesure de distance...
	//La distance étant déterminée par une courbe en 1/P, il faut connaitre le produit Puissance*Distance
	//Pour plus de précision, on détermine ce produit sur la distance que l'on souhaite la plus précise : la distance d'évitement !

///************************************************************************


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

/* Réglages SPI */
	#define USE_SPI2
	#define SPI_R_BUF_SIZE 16

/* Réglages UART */
	#define USE_UART1
	#define USE_UART1RXINTERRUPT
	#define UART1_BAUDRATE		230400
	#define USE_UART1TXINTERRUPT
	#define BUFFER_U1TX_SIZE	128

//	#define USE_UART3
//	#define USE_UART3RXINTERRUPT
//	#define UART3_BAUDRATE		4800
//	#define USE_UART3TXINTERRUPT
//	#define BUFFER_U3TX_SIZE	128
	#define USE_USART3_ON_PB10_PB11

	#define UART_RX_BUF_SIZE	512	//Taille de la chaine de caracteres memorisant les caracteres recus sur UART


/* Réglages watchdog */
	#define USE_WATCHDOG
	#define WATCHDOG_TIMER 		4
	#define WATCHDOG_MAX_COUNT 	5
	#define WATCHDOG_QUANTUM 	1

/* Réglages RF */
	#define USE_RF
	#define RF_UART				UART3_ID
	#define RF_USE_WATCHDOG
	#define SYNCHROBALISE_TIMER	3
	#define RF_CAN_BUF_SIZE		32

/* Réglages FIFO */
	#define USE_FIFO

/* Récapitulatif TIMERs :
 * TIMER 1 : Récupération valeur TSOP
 * TIMER 2 : Eyes
 * TIMER 3 : ...
 * TIMER 4 : Watchdog		(QS_watchdog.c/h)
 * TIMER 5 : Leds
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
