/*
 *  Club Robot ESEO 2008 - 2010
 *  Archi-Tech', PACMAN
 *
 *  Fichier : Global_config.h
 *  Package : Standard_Project
 *  Description : Configuration des modules de QS
 *  Auteur : Jacen
 *  Version 20100415
 */

#ifndef GLOBAL_CONFIG_H
	#define GLOBAL_CONFIG_H
	#include "QS/QS_types.h"
	
	/* Le mode verbeux active debug_printf */
	#define VERBOSE_MODE

	/* Pour certaines config particulieres, il faut definir qui on est
	 * a l'aide d'une des valeurs du type cartes_e de QS_types.h */
	#define I_AM CARTE_SUPER
	#define I_AM_CARTE_SUPER

	/* Il faut choisir à quelle frequence on fait tourner le PIC */
	#define FREQ_10MHZ
	
	/* Les instructions ci dessous définissent le comportement des
	 * entrees sorties du pic. une configuration en entree correspond
	 * a un bit a 1 (Input) dans le masque, une sortie a un bit a
	 * 0 (Output).
	 * Toute connection non utilisee doit etre configuree en entree
	 * (risque de griller ou de faire bruler le pic) 
	 */

	#define PORT_A_IO_MASK	0xFFFF
	#define PORT_B_IO_MASK	0xFFFF
	#define PORT_C_IO_MASK	0xFFFF
	#define PORT_D_IO_MASK	0xC0FF
	#define PORT_E_IO_MASK	0xFFFF
	#define PORT_F_IO_MASK	0xFFFF
	#define PORT_G_IO_MASK	0xFFFF
	/* Les instructions suivantes permettent de configurer certainesF
	 * entrees/sorties du pic pour realiser des fonctionnalites
	 * particulieres comme une entree analogique 
	 */

	#define USE_CAN
	#define CAN_SEND_TIMEOUT_ENABLE
//	#define USE_CAN2
/*	Nombre de messages CAN conservés 
	pour traitement hors interuption */
	#define CAN_BUF_SIZE		8

	#define USE_UART1
	#define USE_UART1RXINTERRUPT
	#define USE_UART2
	#define USE_UART2RXINTERRUPT
/*	Taille de la chaine de caracteres memorisant
	les caracteres recus sur UART */
	#define UART_RX_BUF_SIZE	42
	
	
	/* utilisation du module de watchdog */
	#define USE_WATCHDOG
	/* timer utilisé pour le watchdog (1, 2, 3 ou 4) */
	#define WATCHDOG_TIMER		1
	/* granularité du temps en ms */
	#define WATCHDOG_QUANTUM	1
	/* nombre maximal de watchdogs en exécution */
	#define WATCHDOG_MAX_COUNT  3	
	
	#define USE_AN2
	#define USE_AN3
	#define USE_AN4
	#define USE_AN5
	#define USE_AN8
	#define USE_AN9
	#define USE_AN10
	#define USE_AN11
	#define USE_AN12
	#define USE_AN13
	#define USE_AN14
	#define USE_AN15
	
#endif /* ndef GLOBAL_CONFIG_H */
