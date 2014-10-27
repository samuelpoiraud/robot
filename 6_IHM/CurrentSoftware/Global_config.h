/*
 *  Club Robot ESEO 2014
 *
 *  Fichier : Global_config.h
 *  Package : Standard_project
 *  Description : Configuration des modules de QS
 *  Auteur : Anthony
 *  Version 20100415
 */

#ifndef GLOBAL_CONFIG_H
	#define GLOBAL_CONFIG_H

	//mode d'activation des sorties debug_printf
	#define VERBOSE_MODE
	#define VERBOSE_ELEMENT
	#define OUTPUTLOG_DEFAULT_MAX_LOG_LEVEL LOG_LEVEL_Debug


#define USE_LCD
#define USE_I2C2
#define USE_WATCHDOG
#define WATCHDOG_TIMER 3
#define WATCHDOG_MAX_COUNT 5
#define WATCHDOG_QUANTUM 1

	/* Pour certaines config particulieres, il faut definir qui on est
	 * a l'aide d'une des valeurs du type cartes_e de QS_types.h */
	#define I_AM CARTE_IHM
	#define I_AM_CARTE_IHM

#if defined(STM32F40XX)
	#define HCLK_FREQUENCY_HZ     168000000	//40Mhz, Max: 168Mhz
	#define PCLK1_FREQUENCY_HZ    42000000	//10Mhz, Max: 42Mhz
	#define PCLK2_FREQUENCY_HZ    84000000	//40Mhz, Max: 84Mhz
	#define CPU_EXTERNAL_CLOCK_HZ 8000000	//8Mhz, Fréquence de l'horloge externe
#else
	/* Il faut choisir à quelle frequence on fait tourner le PIC */
	#define FREQ_10MHZ
#endif

	/* Les instructions ci dessous définissent le comportement des
	 * entrees sorties du pic. une configuration en entree correspond
	 * a un bit a 1 (Input) dans le masque, une sortie a un bit a
	 * 0 (Output).
	 * Toute connection non utilisee doit etre configuree en entree
	 * (risque de griller ou de faire bruler le pic)
	 */

#define PORT_A_IO_MASK	0xFFFF
	#define BUTTON_SELFTEST_PORT	GPIOA->IDR0
	#define BUTTON_CALIBRATION_PORT	GPIOA->IDR1
	//									2
	#define BUTTON_PRINTMATCH_PORT	GPIOA->IDR3
	#define BUTTON0_PORT			GPIOA->IDR4
	//									5
	//									6
	#define BUTTON1_PORT			GPIOA->IDR7
	//									8
	#define BUTTON2_PORT			GPIOA->IDR9
	#define BUTTON3_PORT			GPIOA->IDR10
	#define BUTTON4_PORT			GPIOA->IDR11
	#define BUTTON5_PORT			GPIOA->IDR12
	//	-	programmation				13
	//	-	programmation				14
	//									15
#define PORT_B_IO_MASK	0xFFFF
	#define SWITCH_LCD_PORT			GPIOB->IDR0
	#define BUTTON_OK_PORT			GPIOB->IDR1
	#define BUTTON_UP_PORT			GPIOB->IDR2
	// - programmation -				3
	#define BUTTON_DOWN_PORT		GPIOB->IDR4
	#define BUTTON_SET_PORT			GPIOB->IDR5
	//	U1TX							6
	//	U1RX							7
	//	-								8
	#define LCD_RESET_PORT			GPIOB,GPIO_PinSource9
	//	I2C RTC/LCD						10
	//	I2C RTC/LCD						11
	//  SPI								12
	//  SPI								13
	//  SPI								14
	//  SPI								15


#define PORT_C_IO_MASK	0xFC3F
	//									0
	#define SWITCH16_PORT			GPIOC->IDR1
	#define SWITCH17_PORT			GPIOC->IDR2
	#define SWITCH18_PORT			GPIOC->IDR3
	#define BUTTON_RFU_PORT			GPIOC->IDR4
	#define SWITCH2_PORT			GPIOC->IDR5
	#define LED_IHM_OK				GPIOC,GPIO_PinSource6
	#define LED_IHM_UP				GPIOC,GPIO_PinSource7
	#define LED_IHM_DOWN			GPIOC,GPIO_PinSource8
	#define LED_IHM_SET				GPIOC,GPIO_PinSource9
	//	-								10
	//	-								11
	//	-								12
	#define PORT_ROBOT_ID			GPIOC->IDR13
	//	-								14
	//	-								15


#define PORT_D_IO_MASK	0x0CE3
	//	CAN_RX							0
	//	CAN_TX							1
	#define RED_LED					GPIOD,GPIO_PinSource2
	  #define LED_UART				RED_LED
	#define GREEN_LED				GPIOD,GPIO_PinSource3
	#define BLUE_LED				GPIOD,GPIO_PinSource4
	//	-								5
	#define SWITCH_COLOR_PORT		GPIOD->IDR6
	#define BIROUTE_PORT			(!GPIOD->IDR7)	//La biroute doit être par défaut dans l'état NON par défaut... pour qu'on soit sur qu'elle est là.
	//									8
	//									9
	#define LED0_PORT				GPIOD,GPIO_PinSource10
	#define LED1_PORT				GPIOD,GPIO_PinSource11
	#define LED2_PORT				GPIOD,GPIO_PinSource12
	#define LED3_PORT				GPIOD,GPIO_PinSource13
	#define LED4_PORT				GPIOD,GPIO_PinSource14
	#define LED5_PORT				GPIOD,GPIO_PinSource15


#define PORT_E_IO_MASK	0xFFFF
	#define SWITCH0_PORT			GPIOE->IDR0
	#define SWITCH1_PORT			GPIOE->IDR1
	// - programmation -			  2
	#define SWITCH3_PORT			GPIOE->IDR3
	#define SWITCH4_PORT			GPIOE->IDR4
	#define SWITCH5_PORT			GPIOE->IDR5
	#define SWITCH6_PORT			GPIOE->IDR6
	#define SWITCH7_PORT			GPIOE->IDR7
	#define SWITCH8_PORT			GPIOE->IDR8
	#define SWITCH9_PORT			GPIOE->IDR9
	#define SWITCH10_PORT			GPIOE->IDR10
	#define SWITCH11_PORT			GPIOE->IDR11
	#define SWITCH12_PORT			GPIOE->IDR12
	#define SWITCH13_PORT			GPIOE->IDR13
	#define SWITCH14_PORT			GPIOE->IDR14
	#define SWITCH15_PORT			GPIOE->IDR15

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
	#define UART1_BAUDRATE	1382400
	#define USE_UART1TXINTERRUPT
	#define BUFFER_U1TX_SIZE	128

/*	Taille de la chaine de caracteres memorisant
	les caracteres recus sur UART */
	#define UART_RX_BUF_SIZE	512


/* choix de la fréquence des PWM */
//	#define FREQ_PWM_50HZ
//	#define FREQ_PWM_1KHZ
//	#define FREQ_PWM_10KHZ
//	#define FREQ_PWM_20KHZ
	#define FREQ_PWM_50KHZ

	#define USE_AN4		// Mesure 12V hokuyo
	#define USE_AN6		// Mesure 24V puissance
	#define USE_AN10	// Mesure 24V permanence

#endif /* ndef GLOBAL_CONFIG_H */
