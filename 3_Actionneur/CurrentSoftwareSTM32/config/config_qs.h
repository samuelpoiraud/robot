/*
 *  Club Robot ESEO 2013 - 2014
 *
 *  $Id: config_qs.h 904 2013-10-12 12:04:27Z amurzeau $
 *
 *  Package : Carte Actionneur
 *  Description : Configuration des modules de QS
 *  Auteur : Jacen, Alexis
 */

#ifndef CONFIG_QS_H
#define CONFIG_QS_H

#include "config_debug.h" //pour verbose_mode

//#define MODE_SIMULATION

/* ID de la carte: cf le type cartes_e de QS_types.h */
#define I_AM CARTE_ACT		//A voir avec Gwenn pour changer
#define I_AM_CARTE_ACT

/* Il faut choisir � quelle frequence on fait tourner le PIC */
#define HCLK_FREQUENCY_HZ     160000000	//40Mhz, Max: 168Mhz
#define PCLK1_FREQUENCY_HZ    40000000	//10Mhz, Max: 42Mhz
#define PCLK2_FREQUENCY_HZ    80000000	//40Mhz, Max: 84Mhz
#define CPU_EXTERNAL_CLOCK_HZ 8000000	//8Mhz, Fr�quence de l'horloge externe

// DEFINIR LES PORTS DANS Tiny/Global_config.h ou Krusty/Global_config.h selon le robot !


/* Watchdog */
//#define USE_WATCHDOG
//#define WATCHDOG_TIMER 4
//#define WATCHDOG_MAX_COUNT 5
//#define WATCHDOG_QUANTUM 1

/* Clock */
//#define CLOCK_USE_WATCHDOG
#define CLOCK_TIMER_ID 4
#define CLOCK_UPDATE_BUTTONS_PRESS_TIME

/* CAN */
#define USE_CAN
#define CAN_BUF_SIZE	32

/* R�glages UART */
#define USE_UART1
#define USE_UART1RXINTERRUPT
#define UART_RX_BUF_SIZE	12

/* Bouton */
#define I_ASSUME_I_WILL_CALL_BUTTONS_PROCESS_IT_ON_MY_OWN //Fait par clock.h/c
#define USE_BUTTONS

//Test et inclusion des configs sp�cifiques au robot
#if (defined(I_AM_ROBOT_KRUSTY) && defined(I_AM_ROBOT_TINY)) || (!defined(I_AM_ROBOT_KRUSTY) && !defined(I_AM_ROBOT_TINY))
#	error "Veuillez d�finir I_AM_ROBOT_KRUSTY ou I_AM_ROBOT_TINY selon le robot cible dans config_global.h"
#elif defined(I_AM_ROBOT_KRUSTY)
#	include "config_krusty/config_qs.h"
#elif defined(I_AM_ROBOT_TINY)
#	include "config_tiny/config_qs.h"
#endif

#endif /* CONFIG_QS_H */
