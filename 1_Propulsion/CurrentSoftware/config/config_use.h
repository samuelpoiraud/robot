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

//#define MODE_SIMULATION
#ifdef MODE_SIMULATION
#warning 'ATTENTION CE MODE EST STRICTEMENT INTERDIT EN MATCH NE SOYEZ PAS INCONSCIENT!'
#endif
/* Pour certaines config particulieres, il faut definir qui on est
 * a l'aide d'une des valeurs du type cartes_e de QS_types.h */



/* Les instructions suivantes permettent de configurer certaines
* entrees/sorties du pic pour realiser des fonctionnalites
* particulieres comme une entree analogique
*/

#define USE_CAN
#ifndef USE_CAN
	#warning "can désactivé !"
#endif

/*	Nombre de messages CAN conservés
pour traitement hors interuption */
#define CAN_BUF_SIZE		32
#if defined(STM32F40XX)
#define QS_CAN_RX_IT_PRI	5	//Plus faible = plus prioritaire
#else
#define QS_CAN_RX_IT_PRI	CAN_INT_PRI_6	//Modif de la priorité de l'IT can pour rendre la priorité des codeurs plus grande !
#endif

#define I_AM CARTE_PROP		//A voir avec Gwenn pour changer
#define I_AM_CARTE_PROP

#if defined(STM32F40XX)
#define HCLK_FREQUENCY_HZ     168000000	//40Mhz, Max: 168Mhz
#define PCLK1_FREQUENCY_HZ    42000000	//10Mhz, Max: 42Mhz
#define PCLK2_FREQUENCY_HZ    84000000	//40Mhz, Max: 84Mhz
#define CPU_EXTERNAL_CLOCK_HZ 8000000	//8Mhz, Fréquence de l'horloge externe
#else
/* Il faut choisir à quelle frequence on fait tourner le PIC */
#define FREQ_10MHZ
#endif

#define USE_UART1
#define USE_UART1RXINTERRUPT
#define UART1_BAUDRATE		1382400
#define USE_UART1TXINTERRUPT
#define BUFFER_U1TX_SIZE	128

#define USE_SPI2 // GYROSCOPE

//	#define USE_UART2
//	#define USE_UART2RXINTERRUPT

/*	Taille de la chaine de caracteres memorisant
les caracteres recus sur UART */
#define UART_RX_BUF_SIZE	512
//
#define USE_BUTTONS
#define BUTTONS_TIMER 3

/* choix de la fréquence des PWM */
#define USE_PWM3    //moteur droite
#define USE_PWM4    //moteur gauche
//	#define FREQ_PWM_50HZ
//	#define FREQ_PWM_1KHZ
//	#define FREQ_PWM_10KHZ
//	#define FREQ_PWM_20KHZ
#define FREQ_PWM_50KHZ

#include "../_Propulsion_config.h"

#ifdef USE_CODEUR_SUR_IT_ET_QE
#define USE_QEI_ON_IT
#define QEI_ON_IT_QA 1
#define QEI_ON_IT_QB 2
#endif

#define USE_WATCHDOG
#define WATCHDOG_TIMER 4
#define WATCHDOG_MAX_COUNT 5
#define WATCHDOG_QUANTUM 1

#define USE_FIFO

/* Récapitulatif IT :
* TIMER 1 : Gyroscope		(it.c/h)
* TIMER 2 : IT principale	(it.c/h)
* TIMER 3 : Boutons		(QS_buttons.c/h)
* TIMER 4 : Watchdog		(QS_watchdog.c/h)
* TIMER 5 : Clock			(clock.c/h)
*/

#endif /* CONFIG_USE_H */
