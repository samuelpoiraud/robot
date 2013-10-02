/*
 *  Club Robot ESEO 2008 - 2010
 *  Archi'Tech, PACMAN
 *
 *  Fichier : Global_config.h
 *  Package : Carte Principale
 *  Description : Configuration des modules de QS
 *  Auteur : Jacen
 *  Version 20110227
 */

#ifndef GLOBAL_CONFIG_H
	#define GLOBAL_CONFIG_H

	#define VERBOSE_MODE

	/* Pour certaines config particulieres, il faut definir qui on est
	 * a l'aide d'une des valeurs du type cartes_e de QS_types.h */
	#define I_AM CARTE_STRAT
	#define I_AM_CARTE_SUPER //I_AM_CARTE_STRAT

	#define FDP_2013

	/* Il faut choisir à quelle frequence on fait tourner le STM32 */
	#define HCLK_FREQUENCY_HZ     160000000	//40Mhz, Max: 168Mhz
	#define PCLK1_FREQUENCY_HZ    40000000	//10Mhz, Max: 42Mhz
	#define PCLK2_FREQUENCY_HZ    80000000	//40Mhz, Max: 84Mhz
	#define CPU_EXTERNAL_CLOCK_HZ 8000000	//8Mhz, Fréquence de l'horloge externe

	/* Les instructions ci dessous définissent le comportement des
	 * entrees sorties du pic. une configuration en entree correspond
	 * a un bit a 1 (Input) dans le masque, une sortie a un bit a
	 * 0 (Output).
	 * Toute connection non utilisee doit etre configuree en entree
	 * (risque de griller ou de faire bruler le pic)
	 */
	
	#define PORT_A_IO_MASK	0xFFFF
	#define PORT_B_IO_MASK	0xF7FF
	#define PORT_C_IO_MASK	0xFFBF	//C9: MO2 debug clock
	#define PORT_D_IO_MASK	0x03FF	//LEDs
	#define PORT_E_IO_MASK	0xFFFF

	#define TRIS_ROBOT_ID_OUTPUT GPIOC->MODER13
	#define LAT_ROBOT_ID_OUTPUT  GPIOC->ODR13
	#define TRIS_ROBOT_ID_INPUT  GPIOC->MODER13
	#define PORT_ROBOT_ID_INPUT  GPIOC->IDR12
	#define PORT_ROBOT_ID GPIOC->IDR12
	
	/* Les instructions suivantes permettent de configurer certaines
	 * entrees/sorties du pic pour realiser des fonctionnalites
	 * particulieres comme une entree analogique
	 */


	#define USE_CAN
/*	Nombre de messages CAN conservés
	pour traitement hors interuption */
	#define CAN_BUF_SIZE		32

	#define USE_UART1
	#define USE_UART1RXINTERRUPT
	#define BUFFER_TX_SIZE 100
	//#define USE_UART1TXINTERRUPT
	
//	#define USE_UART2
//	#define USE_UART2RXINTERRUPT
	
/*	Taille de la chaine de caracteres memorisant
	les caracteres recus sur UART */
	#define UART_RX_BUF_SIZE	12
	
/* Utilisation des entrées analogiques */
	#define USE_ANALOG_EXT_VREF

	#define USE_AN8

			#define LED_ERROR 			GPIOD->ODR10
			#define LED_SELFTEST 		GPIOD->ODR11
			#define LED_RUN  			GPIOD->ODR12
			#define LED_CAN  			GPIOD->ODR13
			#define LED_UART 			GPIOD->ODR14
			#define LED_USER 			GPIOD->ODR15

	//utilisation du module BUTTONS
	#define USE_BUTTONS
	//utilise le timer 2 pour les boutons
	//#define BUTTONS_TIMER 3
	#define BUTTONS_TIMER_USE_WATCHDOG
#define BUTTON0_PORT GPIOA->IDR0

	//Test debug QS_STM32
	//On active tout

	#define USE_WATCHDOG
	#define WATCHDOG_TIMER 3
	#define WATCHDOG_MAX_COUNT 5
	#define WATCHDOG_QUANTUM 1

	#define USE_DCMOTOR2
	#define DCM_NUMBER 3
	#define DCM_TIMER_PERIOD 10
	#define DCMOTOR_NB_POS 3
	#define DCM_TIMER 4

	#define AX12_UART_ID 2
	#define USE_AX12_SERVO
	#define AX12_NUMBER 7
	#define AX12_TIMER_ID 2
	#define AX12_DIRECTION_PORT GPIOB->ODR11
	#define AX12_STATUS_RETURN_MODE AX12_STATUS_RETURN_ALWAYS

//
//	#define USE_DCMOTOR2
//	#define DCM_NUMBER 3
//	#define DCMOTOR_NB_POS 5
//	#define DCM_TIMER 3
//	#define DCM_TIMER_PERIOD 10
//
//	#define USE_SERVO
//	#define SERVO_TIMER 4

//	#define USE_STEP_MOTOR
//	#define STEP_MOTOR_TIMER 3
//	#define STEP_MOTOR_MS_PER_STEP 5
//	#define STEP_MOTOR1 GPIOE->ODR0
//	#define STEP_MOTOR2 GPIOE->ODR1
//	#define STEP_MOTOR3 GPIOE->ODR2
//	#define STEP_MOTOR4 GPIOE->ODR3

//	#define USE_SPI
//	#define USE_SPI2

#define FREQ_PWM_50HZ


#endif /* ndef GLOBAL_CONFIG_H */
