/*
 *  Club Robot ESEO 2008 - 2009
 *  Archi'Tech, PACMAN
 *
 *  Fichier : Global_config.h
 *  Package : Standard_Project
 *  Description : Configuration des modules de QS
 *  Auteur : Jacen
 *  Version 20090223
 */

#include "QS/QS_config_doc.h"

#ifndef GLOBAL_CONFIG_H
	#define GLOBAL_CONFIG_H
	#include "QS/QS_types.h"

	/* Le mode verbeux active debug_printf */
	#define VERBOSE_MODE

	/* Pour certaines config particulieres, il faut definir qui on est
	 * a l'aide d'une des valeurs du type cartes_e de QS_types.h */
	#define I_AM CARTE_ACT1

	/* Il faut choisir à quelle frequence on fait tourner le PIC */
	#define FREQ_10MHZ
	//#define FREQ_INTERNAL_CLK
	
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
	#define PORT_D_IO_MASK	0xF0FF
	#define PORT_E_IO_MASK	0xFFFF
	#define PORT_F_IO_MASK	0xFFFF
	#define PORT_G_IO_MASK	0xFFFF

	/* Les instructions suivantes permettent de configurer certaines
	 * entrees/sorties du pic pour realiser des fonctionnalites
	 * particulieres comme une entree analogique 
	 */

	#define USE_UART1
	#define USE_UART1RXINTERRUPT
//	#define USE_UART2
//	#define USE_UART2RXINTERRUPT
/*	Taille de la chaine de caracteres memorisant
	les caracteres recus sur UART */
	#define UART_RX_BUF_SIZE	12
//

	#define USE_CAN
	#define CAN_BUF_SIZE		32

/* utilise les PWM L au lieu des H */
//	#define USE_PWM_L
/* choix de la fréquence des PWM */
//	#define FREQ_PWM_50HZ
//	#define FREQ_PWM_1KHZ
//	#define FREQ_PWM_10KHZ
//	#define FREQ_PWM_50KHZ
//	#define FREQ_PWM_20KHZ
//


	/* asservissement en position de moteurs CC */
	/* déclarer l'utilisation du pilote */
//	#define USE_DCMOTOR	
	
#endif /* ndef GLOBAL_CONFIG_H */
