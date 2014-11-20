/*
 *  Club Robot ESEO 2013 - 2014
 *
 *  $Id: config_pin.h 2829 2014-11-07 10:41:44Z aguilmet $
 *
 *  Package : Carte Strategie
 *  Description : Configuration des pins
 *  Auteur : Jacen, Alexis
 */

#ifndef CONFIG_PIN_H
#define CONFIG_PIN_H

	/* Les instructions ci dessous définissent le comportement des
	 * entrees sorties du pic. une configuration en entree correspond
	 * a un bit a 1 (Input) dans le masque, une sortie a un bit a
	 * 0 (Output).
	 * Toute connection non utilisee doit etre configuree en entree
	 * (risque de griller ou de faire bruler le pic)
	 */

#include "../main.h"
#include "../stm32f4xx/stm32f4xx_gpio.h"


#if defined(STM32F40XX)

	#define PORT_A_IO_MASK	0xFFFF
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


	#define PORT_B_IO_MASK	0x0FFFF
			//	-				 			  0
			//	-				 			  1
			//	-				 			  2
			//	-				 			  3
			//	-				 			  4
			//	-				 			  5
			//	-				 			  6
			//	-				 			  7
		#define I_AM_READY			GPIOB,GPIO_Pin_8
			//	-				 			  9
			//	-				 			  10
			//	-				 			  11
		#define GYRO_CS				GPIOB,GPIO_Pin_12
		#define GYRO_SCK			GPIOB,GPIO_Pin_13
		#define GYRO_MISO			GPIOB,GPIO_Pin_14
		#define GYRO_MOSI			GPIOB,GPIO_Pin_15


	#define PORT_C_IO_MASK	0xE4FE
			#define	USB_POWER_ON	GPIOC,GPIO_Pin_0
			//	Capteur			 			  1
			//	Capteur			 			  2
			//	Capteur			 			  3
			//	-				 			  4
			//	-				 			  5
			//	-				 			  6
			//	-				 			  7
			#define PWM_MOTEUR_1  3			//8
			#define PWM_MOTEUR_2  4			//9
			//	-				 			  10
			#define SENS_MOTEUR_1 	GPIOC,GPIO_Pin_11
			#define SENS_MOTEUR_2 	GPIOC,GPIO_Pin_12
			#define PORT_ROBOT_ID  	GPIOC->IDR13
			//	-	OSC32_in 			  	  14
			//	-	OSC32_out 			  	  15


	#define PORT_D_IO_MASK	0x03FF
			//	CAN_RX						  0
			//	CAN_TX						  1
			//	-				 			  2
			//	-				 			  3
			//	-				 			  4
			//	-				 			  5
			//	-				 			  6
			//	-				 			  7
			//	-				 			  8
			//	-				 			  9
		#define LED_ERROR 			GPIOD,GPIO_Pin_10
		#define LED_SELFTEST 		GPIOD,GPIO_Pin_11
		#define LED_RUN  			GPIOD,GPIO_Pin_12

#ifdef LCD_TOUCH // Pour éviter les conflits avec les ports de l'écran
		#define LED_CAN  			LED_RUN
		#define LED_UART 			LED_RUN
		#define LED_USER 			LED_RUN
#else
		#define LED_CAN  			GPIOD,GPIO_Pin_13
		#define LED_UART 			GPIOD,GPIO_Pin_14
		#define LED_USER 			GPIOD,GPIO_Pin_15
#endif

	// Pour les leds inclues sur la carte STM32
		#define LED_GREEN 			LED_RUN
		#define LED_ORANGE			LED_CAN
		#define LED_ROUGE			LED_UART
		#define LED_BLEU			LED_USER

	#define PORT_E_IO_MASK	0xFFFF
		#define SWITCH_MIDDLE_PORT	(GPIOE->IDR7)
		#define SWITCH_LEFT_PORT	(GPIOE->IDR8)
		#define SWITCH_RIGHT_PORT	(GPIOE->IDR9)
		#define BUTTON1_PORT		(!GPIOE->IDR12)	//Calibration


#else
	#define PORT_A_IO_MASK	0xFFFF
	#define PORT_B_IO_MASK	0x0FFF
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


#endif /* CONFIG_PIN_H */
