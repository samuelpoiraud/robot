/*
 *  Club Robot ESEO 2008 - 2010
 *  Archi'Tech, PACMAN
 *
 *  Fichier : Global_config.h
 *  Package : Standard_project
 *  Description : Configuration des modules de QS
 *  Auteur : Jacen - Nirgal 2009
 *  Version 20100415
 */

#ifndef GLOBAL_CONFIG_H
	#define GLOBAL_CONFIG_H

	//mode d'activation des sorties debug_printf
	#define VERBOSE_MODE

	/* Pour certaines config particulieres, il faut definir qui on est
	 * a l'aide d'une des valeurs du type cartes_e de QS_types.h */
	#define I_AM CARTE_ASSER
	#define I_AM_CARTE_ASSER

#if defined(STM32F40XX)
	#define HCLK_FREQUENCY_HZ     40000000	//40Mhz, Max: 168Mhz
	#define PCLK1_FREQUENCY_HZ    10000000	//10Mhz, Max: 42Mhz
	#define PCLK2_FREQUENCY_HZ    20000000	//40Mhz, Max: 84Mhz
	#define CPU_EXTERNAL_CLOCK_HZ 8000000	//10Mhz, Fréquence de l'horloge externe
#else
	/* Il faut choisir à quelle frequence on fait tourner le PIC */
	#define FREQ_10MHZ
#endif

#if !defined(STM32F40XX)
	#define FDP_2013
#endif
	/* Les instructions ci dessous définissent le comportement des
	 * entrees sorties du pic. une configuration en entree correspond
	 * a un bit a 1 (Input) dans le masque, une sortie a un bit a
	 * 0 (Output).
	 * Toute connection non utilisee doit etre configuree en entree
	 * (risque de griller ou de faire bruler le pic)
	 */

#if defined(STM32F40XX)
	#define PORT_A_IO_MASK	0xFFFF
	#define PORT_B_IO_MASK	0xFFFF
	#define PORT_C_IO_MASK	0xFCCF
	#define PORT_D_IO_MASK	0x0FFF
	#define PORT_E_IO_MASK	0xFFFF
		#define SENS_MOTEUR_1 GPIOC->ODR4
		#define SENS_MOTEUR_2 GPIOC->ODR5
		#define PWM_MOTEUR_1  3
		#define PWM_MOTEUR_2  4

		//TODO: a définir
		#define LAT_ROBOT_ID_OUTPUT
		#define TRIS_ROBOT_ID_OUTPUT
		#define PORT_ROBOT_ID_INPUT
		#define TRIS_ROBOT_ID_INPUT

//		#define SEL_OCTET_CPLD	 	PORTGbits.RG8
//		#define SEL_ROUE_CPLD 	 	PORTGbits.RG7
//		#define RAZ_CPLD  		 	PORTGbits.RG6

	//Utilisé ou ?
	//#define PORT_COMPT PORTB

	//#define PORT_LEDS PORTD
#else
	#define PORT_A_IO_MASK	0xFFFF
	#define PORT_B_IO_MASK	0xFFFF
	#define PORT_C_IO_MASK	0xFFFF
	#define PORT_D_IO_MASK	0xC0FF
	#define PORT_E_IO_MASK	0xFFF0
		#define SENS_MOTEUR_1 LATEbits.LATE0
		#define SENS_MOTEUR_2 LATEbits.LATE2
		#define PWM_MOTEUR_1  1
		#define PWM_MOTEUR_2  2
		#define LAT_ROBOT_ID_OUTPUT		LATEbits.LATE7
		#define TRIS_ROBOT_ID_OUTPUT	TRISEbits.TRISE7
		#define PORT_ROBOT_ID_INPUT		PORTEbits.RE5
		#define TRIS_ROBOT_ID_INPUT		TRISEbits.TRISE5
	#define PORT_F_IO_MASK	0xFFF5	/*(can tx et uart tx)*/
	#define PORT_G_IO_MASK	0xFE3F
		#define SEL_OCTET_CPLD	 	PORTGbits.RG8
		#define SEL_ROUE_CPLD 	 	PORTGbits.RG7
		#define RAZ_CPLD  		 	PORTGbits.RG6
	#define PORT_COMPT PORTB

	#define PORT_LEDS PORTD
#endif

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
	#define USE_UART1
	#define USE_UART1RXINTERRUPT
//	#define USE_UART2
//	#define USE_UART2RXINTERRUPT

/*	Taille de la chaine de caracteres memorisant
	les caracteres recus sur UART */
	#define UART_RX_BUF_SIZE	128
//
	#define USE_BUTTONS
	#define BUTTONS_TIMER 3

/* choix de la fréquence des PWM */
//	#define FREQ_PWM_50HZ
//	#define FREQ_PWM_1KHZ
//	#define FREQ_PWM_10KHZ
//	#define FREQ_PWM_20KHZ
	#define FREQ_PWM_50KHZ


	#include "_Propulsion_config.h"

	#ifdef USE_CODEUR_SUR_IT_ET_QE
		#define USE_QEI_ON_IT
		#define QEI_ON_IT_QA 1
		#define QEI_ON_IT_QB 2
	#endif

#endif /* ndef GLOBAL_CONFIG_H */
