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

#include "_Propulsion_config.h"

	//mode d'activation des sorties debug_printf
	#define VERBOSE_MODE
	#define OUTPUTLOG_DEFAULT_MAX_LOG_LEVEL LOG_LEVEL_Debug

	/* Pour certaines config particulieres, il faut definir qui on est
	 * a l'aide d'une des valeurs du type cartes_e de QS_types.h */
	#define I_AM CARTE_ASSER
	#define I_AM_CARTE_ASSER

#if defined(STM32F40XX)
	#define HCLK_FREQUENCY_HZ     168000000	//40Mhz, Max: 168Mhz
	#define PCLK1_FREQUENCY_HZ    42000000	//10Mhz, Max: 42Mhz
	#define PCLK2_FREQUENCY_HZ    84000000	//40Mhz, Max: 84Mhz
	#define CPU_EXTERNAL_CLOCK_HZ 8000000	//8Mhz, Fréquence de l'horloge externe
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

	#define PORT_A_IO_MASK	0xFE9B
		#define BUTTON0_PORT		GPIOA->IDR0
			//	XBEE/BT/U2TX				  2
			//	XBEE/BT/U2RX				  3
			//	-				 			  4
			//	-				 			  6
			//	-	sortie libre 			  8
			//	-	usb			 			  9
			//	-	usb			 			  10
			//	-	usb			 			  11
			//	-	usb			 			  12
			//	-	programmation			  13
			//	-	programmation 			  14

	#define PORT_B_IO_MASK	0x00BF
			//	U1TX						  6
			//	U1RX						  7

	#define PORT_C_IO_MASK	0x200E
			#define	USB_POWER_ON	 	GPIOC->ODR0
			//	Capteur			 			  1
			//	Capteur			 			  2
			//	Capteur			 			  3
			#define PWM_MOTEUR_1  3			//8
			#define PWM_MOTEUR_2  4			//9
			#define SENS_MOTEUR_1 	GPIOC->ODR11
			#define SENS_MOTEUR_2 	GPIOC->ODR12
			#define PORT_ROBOT_ID  	(!GPIOC->IDR13)
			//	-	OSC32_in 			  	  14
			//	-	OSC32_out 			  	  15

	#define PORT_D_IO_MASK	0x02C3
			//	CAN_RX						  0
			//	CAN_TX						  1
		#define LED_ERROR 			GPIOD->ODR10
		#define LED_SELFTEST 		GPIOD->ODR11
		#define LED_USER2			LED_SELFTEST
		#define LED_RUN  			GPIOD->ODR12
#if defined (SIMULATION_VIRTUAL_PERFECT_ROBOT) // Dans le cas du robot parfait on suppose que l'écran est branché on évite les conflits avec les ports de l'écran
		#define LED_CAN  			LED_RUN
		#define LED_UART 			LED_RUN
		#define LED_USER 			LED_RUN
#else
		#define LED_CAN  			GPIOD->ODR13
		#define LED_UART 			GPIOD->ODR14
		#define LED_USER 			GPIOD->ODR15
#endif
	// Pour les leds inclues sur la carte STM32
		#define LED_GREEN 			LED_RUN
		#define LED_ORANGE			LED_CAN
		#define LED_ROUGE			LED_UART
		#define LED_BLEU			LED_USER

	#define PORT_E_IO_MASK	0xFFFF
		#define BUTTON1_PORT		(!GPIOE->IDR12)	//Selftest
		#define BUTTON2_PORT		(!GPIOE->IDR13)	//Calibration
		#define BUTTON3_PORT		(!GPIOE->IDR14)	//LCD Menu +
		#define BUTTON4_PORT		(!GPIOE->IDR15)	//LCD Menu -


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
	#define UART1_BAUDRATE	115200
	#define USE_UART1TXINTERRUPT
	#define BUFFER_U1TX_SIZE	128

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

	/* Utilisation des entrées analogiques */
	#define USE_AN11	// Capteur DT10 détection triangle
	#define USE_AN12	// Capteur DT10 détection triangle
	#define USE_AN14	// Capteur DT10 détection triangle

	#include "_Propulsion_config.h"

	#ifdef USE_CODEUR_SUR_IT_ET_QE
		#define USE_QEI_ON_IT
		#define QEI_ON_IT_QA 1
		#define QEI_ON_IT_QB 2
	#endif

	#define USE_FIFO

#endif /* ndef GLOBAL_CONFIG_H */
