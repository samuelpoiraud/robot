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
	#include "QS/QS_types.h"

	//mode d'activation des sorties debug_printf
	#define VERBOSE_MODE
	
	/* Pour certaines config particulieres, il faut definir qui on est
	 * a l'aide d'une des valeurs du type cartes_e de QS_types.h */
	#define I_AM CARTE_ASSER
	#define I_AM_CARTE_ASSER
	/* Il faut choisir à quelle frequence on fait tourner le PIC */
	#define FREQ_10MHZ
	
	#define FDP_2013
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
	#define PORT_E_IO_MASK	0xFFF0
		#define SENS_MOTEUR_1 LATEbits.LATE0
		#define SENS_MOTEUR_2 LATEbits.LATE2
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
	#define QS_CAN_RX_IT_PRI	CAN_INT_PRI_6	//Modif de la priorité de l'IT can pour rendre la priorité des codeurs plus grande !
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
