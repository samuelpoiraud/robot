/*
 *  Club Robot ESEO 2013 - 2014
 *
 *  $Id$
 *
 *  Package : Carte Actionneur
 *  Description : Activation de modules et fonctionnalités
 *  Auteur : Jacen, Alexis
 */

#ifndef CONFIG_USE_H
#define CONFIG_USE_H
#include "config_global.h"

//////////////////////////////////////////////////////////////////
//-------------------------MODE ET USE--------------------------//
//////////////////////////////////////////////////////////////////

//#define MODE_SIMULATION		//N'utilise pas le CAN
#ifdef MODE_SIMULATION
	#warning 'ATTENTION CE MODE EST STRICTEMENT INTERDIT EN MATCH NE SOYEZ PAS INCONSCIENT!'
#endif

#define FAST_COS_SIN			//Calcul rapide des cos et sin à l'aide d'un GRAND tableau de valeur

#define VERBOSE_MODE			//Activation du verbose

#define CAN_VERBOSE_MODE		//Activation de la verbosité des messages CAN

//#define FDP_2014

//////////////////////////////////////////////////////////////////
//----------------------------QS--------------------------------//
//////////////////////////////////////////////////////////////////

// Définir les configurations QS propre à chaque robot dans config_big/config_use.h ou config_small/config_use.h !


/* ID de la carte: cf le type cartes_e de QS_types.h */
	#define I_AM CARTE_ACT		//A voir avec Gwenn pour changer
	#define I_AM_CARTE_ACT

/* Il faut choisir à quelle frequence on fait tourner le PIC */
	#define HCLK_FREQUENCY_HZ     160000000	//40Mhz, Max: 168Mhz
	#define PCLK1_FREQUENCY_HZ    40000000	//10Mhz, Max: 42Mhz
	#define PCLK2_FREQUENCY_HZ    80000000	//40Mhz, Max: 84Mhz
	#define CPU_EXTERNAL_CLOCK_HZ 8000000	//8Mhz, Fréquence de l'horloge externe

/* Clock */
	#define CLOCK_USE_WATCHDOG
	#define CLOCK_TIMER_ID 4
	#define CLOCK_UPDATE_BUTTONS_PRESS_TIME

/* UART */
	#define USE_UART

/* CAN */
	#define USE_CAN
	#define CAN_BUF_SIZE	32
	#define CAN_SEND_TIMEOUT_ENABLE
	#define QS_CAN_RX_IT_PRI 2

/* Réglages UART */
	#define USE_UART1
	#define UART1_BAUDRATE		230400
	#define USE_UART1RXINTERRUPT
	#define UART_RX_BUF_SIZE	12

/* Bouton */
	#define I_ASSUME_I_WILL_CALL_BUTTONS_PROCESS_IT_ON_MY_OWN //Fait par clock.h/c
	#define USE_BUTTONS

/* Réglages watchdog */
	#define USE_WATCHDOG
	#define WATCHDOG_TIMER 3
	#define WATCHDOG_MAX_COUNT 5
	#define WATCHDOG_QUANTUM 1

/* Servo-Moteurs AX12 */
	#define USE_AX12_SERVO
	#define AX12_NUMBER 50
	#define AX12_INSTRUCTION_BUFFER_SIZE 200
	#define AX12_TIMER_ID 2
	#define AX12_STATUS_RETURN_MODE AX12_STATUS_RETURN_ALWAYS	//Permet de savoir quand l'AX12 n'est pas bien connecté ou ne répond pas.
	#define AX12_STATUS_RETURN_CHECK_CHECKSUM
	#define AX12_UART_ID 3
	#define AX12_RX24_UART_ID 2


//Test et inclusion des configs spécifiques au robot
#if (defined(I_AM_ROBOT_BIG) && defined(I_AM_ROBOT_SMALL)) || (!defined(I_AM_ROBOT_BIG) && !defined(I_AM_ROBOT_SMALL))
#	error "Veuillez définir I_AM_ROBOT_BIG ou I_AM_ROBOT_SMALL selon le robot cible dans config_global.h"
#elif defined(I_AM_ROBOT_BIG)
#	include "config_big/config_use.h"
#elif defined(I_AM_ROBOT_SMALL)
#	include "config_small/config_use.h"
#endif

/* Récapitulatif TIMERs :
 * TIMER 1 : ...
 * TIMER 2 : AX12			(QS_ax12.c/h)
 * TIMER 3 : Watchdog		(QS_watchdog.c/h)
 * TIMER 4 : Clock			(clock.c/h)
 * TIMER 5 : Servo			(QS_servo.c/h)
 */

/* Récapitulatif IT priorité :
 * 15 : TIMER_5
 * 14 : TIMER_4
 * 13 : TIMER_3
 * 11 : TIMER_2
 *  9 : TIMER_1
 *  3 : USART_1
 *  3 : USART_2
 *  3 : USART_3
 *  2 : CAN
 *  0 : Systick
 */

#endif /* CONFIG_USE_H */
