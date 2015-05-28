/*
 *  Club Robot ESEO 2013 - 2014
 *
 *  $Id$
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

	#define PORT_A_IO_MASK	0xFE9F
		#define BUTTON0_PORT		GPIOA->IDR0
		//								1
		//	XBEE/BT/U2TX				2
		//	XBEE/BT/U2RX				3
		//	-							4
		#define XBEE_RESET			GPIOA,GPIO_Pin_5
		#define PIN_RF_CONFIG		GPIOA,GPIO_Pin_6
		//								7
		//								8
		//	-	usb						9
		//	-	usb						10
		//	-	usb						11
		//	-	usb						12
		//	-	programmation			13
		//	-	programmation			14
		//								15


	#define PORT_B_IO_MASK	0x0DFF
		//	Mesure 24V					0
		#define RECALAGE_AR_D		!GPIOB->IDR1
		#define POWER_WATCH_INT		GPIOB->IDR2
		// - programmation -			3
		#define PRESENCE_PIED_PINCE_GAUCHE_HOLLY 		GPIOB->IDR4
		#define PRESENCE_PIED_PINCE_DROITE_HOLLY 		GPIOB->IDR5
		//	U1TX						6
		//	U1RX						7
		//	-							8
		//								9
		//	I2C RTC/LCD	 				10
		//	I2C RTC/LCD	 				11
		#define SD_CS				GPIOB,GPIO_Pin_12
		//	SD_SCK						13
		//	SD_MISO						14
		//	SD_MOSI						15


	#define PORT_C_IO_MASK	0xE03E
		#define	USB_POWER_ON	 	GPIOC,GPIO_Pin_0
		#define RECALAGE_AV_G		!GPIOC->IDR1
		#define RECALAGE_AV_D		!GPIOC->IDR2
		#define WT100_GOBELET_FRONT	(!(GPIOC->IDR3))
		//								4
		#define RECALAGE_AR_G		!GPIOC->IDR5
		#define LED_BEACON_IR_GREEN	GPIOC,GPIO_Pin_6
		#define LED_BEACON_IR_RED	GPIOC,GPIO_Pin_7
		#define SYNCHRO_BEACON		GPIOC,GPIO_Pin_8
		#define BUZZER				GPIOC,GPIO_Pin_9
		//								10
		//								11
		//								12
		#define PORT_ROBOT_ID  		GPIOC->IDR13
		//	-	OSC32_in				14
		//	-	OSC32_out				15


	#define PORT_D_IO_MASK	0x03E3
		//	CAN_RX						0
		//	CAN_TX						1
		#define RED_LEDS			GPIOD,GPIO_Pin_2
		#define GREEN_LEDS			GPIOD,GPIO_Pin_3
		#define BLUE_LEDS			GPIOD,GPIO_Pin_4
		//	-	usb led red				5
		//								6
		//								7
		//	HOKUYO U3TX					8
		//	HOKUYO U3RX					9
		#define LED_ERROR 			GPIOD,GPIO_Pin_10
		#define LED_SELFTEST 		GPIOD,GPIO_Pin_11
		#define LED_RUN  			GPIOD,GPIO_Pin_12	// Led verte carte STM
			#define LED_GREEN 			LED_RUN
		#define LED_CAN  			GPIOD,GPIO_Pin_13	// Led orange carte STM
			#define LED_ORANGE			LED_CAN
		#define LED_UART 			GPIOD,GPIO_Pin_14	// Led rouge carte STM
			#define LED_ROUGE			LED_UART
		#define LED_USER 			GPIOD,GPIO_Pin_15	// Led bleue carte STM
			#define LED_BLEU			LED_USER


	#define PORT_E_IO_MASK	0xFFFF
		//	-							0
		//	-							1
		// - programmation -			2
		// - programmation -			3
		// - programmation -			4
		// - programmation -			5
		// - programmation -			6
		//								7
		#define ACT_IS_READY			GPIOE,GPIO_Pin_8
		#define PROP_IS_READY			GPIOE,GPIO_Pin_9
		#define IHM_IS_READY			GPIOE,GPIO_Pin_10
		//								11
		//								12
		//								13
		//								14
		//								15

#endif /* CONFIG_PIN_H */
