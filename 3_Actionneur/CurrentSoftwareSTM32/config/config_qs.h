/*
 *  Club Robot ESEO 2013 - 2014
 *
 *  $Id$
 *
 *  Package : Carte Actionneur
 *  Description : Configuration des modules de QS
 *  Auteur : Jacen, Alexis
 */

#ifndef CONFIG_QS_H
#define CONFIG_QS_H

#include "config_global.h"

//#define MODE_SIMULATION

/* ID de la carte: cf le type cartes_e de QS_types.h */
#define I_AM CARTE_ACT		//A voir avec Gwenn pour changer
#define I_AM_CARTE_ACT

/* Il faut choisir à quelle frequence on fait tourner le PIC */
#define HCLK_FREQUENCY_HZ     160000000	//40Mhz, Max: 168Mhz
#define PCLK1_FREQUENCY_HZ    40000000	//10Mhz, Max: 42Mhz
#define PCLK2_FREQUENCY_HZ    80000000	//40Mhz, Max: 84Mhz
#define CPU_EXTERNAL_CLOCK_HZ 8000000	//8Mhz, Fréquence de l'horloge externe

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

/* UART */
#define USE_UART

/* CAN */
#define USE_CAN
#define CAN_BUF_SIZE	32

/* Réglages UART */
#define USE_UART1
#define USE_UART1RXINTERRUPT
#define UART_RX_BUF_SIZE	12

/* Bouton */
#define I_ASSUME_I_WILL_CALL_BUTTONS_PROCESS_IT_ON_MY_OWN //Fait par clock.h/c
#define USE_BUTTONS

//Test et inclusion des configs spécifiques au robot
#if (defined(I_AM_ROBOT_BIG) && defined(I_AM_ROBOT_SMALL)) || (!defined(I_AM_ROBOT_BIG) && !defined(I_AM_ROBOT_SMALL))
#	error "Veuillez définir I_AM_ROBOT_BIG ou I_AM_ROBOT_SMALL selon le robot cible dans config_global.h"
#elif defined(I_AM_ROBOT_BIG)
#	include "config_big/config_qs.h"
#elif defined(I_AM_ROBOT_SMALL)
#	include "config_small/config_qs.h"
#endif

#endif /* CONFIG_QS_H */
