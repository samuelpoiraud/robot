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

#ifdef FDP_2014
#define PORT_A_IO_MASK	0xFE9F
	#define BUTTON0_PORT		GPIOA->IDR0
	#define SWITCH_DEBUG_PORT	GPIOA->IDR1
	//	XBEE/BT/U2TX				  2
	//	XBEE/BT/U2RX				  3
	//	-				 			  4
	#define XBEE_RESET			GPIOA,GPIO_PinSource5
	#define PIN_RF_CONFIG		GPIOA,GPIO_PinSource6
	#define SWITCH_VERBOSE_PORT	GPIOA->IDR7
	#define LED_IHM_SET			GPIOA,GPIO_PinSource8
	//	-	usb			 			  9
	//	-	usb			 			  10
	//	-	usb			 			  11
	//	-	usb			 			  12
	//	-	programmation			  13
	//	-	programmation 			  14
	#define BUTTON5_PORT		(!GPIOA->IDR15)


#define PORT_B_IO_MASK	0x0DFF
	#define FRESCO_3			GPIOB->IDR1
	//	Capteur						  1
	#define POWER_WATCH_INT		GPIOB->IDR2
	// - programmation -			  3
	#define SWITCH_XBEE_PORT	GPIOB->IDR4
	#define SWITCH_SAVE_PORT	GPIOB->IDR5
	//	U1TX						  6
	//	U1RX						  7
	//	-			 	  			  8
	#define LCD_RESET_PORT		GPIOB,GPIO_PinSource9
	//	I2C RTC/LCD	 			  	  10
	//	I2C RTC/LCD	 			  	  11
	#define SD_CS				GPIOB,GPIO_PinSource12
	#define EEPROM_CS			GPIOB,GPIO_PinSource12
	#define EEPROM_SCK			GPIOB,GPIO_PinSource13
	#define EEPROM_SDO			GPIOB,GPIO_PinSource14
	#define EEPROM_SDI			GPIOB,GPIO_PinSource15


#define PORT_C_IO_MASK	0xE03E
	#define	USB_POWER_ON	 	GPIOC,GPIO_PinSource0
	#define RECALAGE_1			GPIOC->IDR1
	#define RECALAGE_2			GPIOC->IDR2
	//	Capteur						  3
	//	Capteur			 			  4
	#define BUTTON6_PORT		(!GPIOC->IDR5)
	#define LED_BEACON_IR_GREEN	GPIOC,GPIO_PinSource6
	#define LED_BEACON_IR_RED	GPIOC,GPIO_PinSource7
	#define SYNCHRO_BEACON		GPIOC,GPIO_PinSource8
	#define BUZZER				GPIOC,GPIO_PinSource9
	#define LED_IHM_OK			GPIOC,GPIO_PinSource10
	#define EEPROM_WP			GPIOC,GPIO_PinSource11
	#define LED_IHM_UP			GPIOC,GPIO_PinSource11
	#define LED_IHM_DOWN		GPIOC,GPIO_PinSource12

	//#define 				GPIOC->ODR11
	#define EEPROM_HOLD			GPIOC,GPIO_PinSource12
	#define PORT_ROBOT_ID  		GPIOC->IDR13
	//	-	OSC32_in 			  	  14
	//	-	OSC32_out 			  	  15


#define PORT_D_IO_MASK	0x03E3
	//	CAN_RX						  0
	//	CAN_TX						  1
	#define RED_LEDS			GPIOD,GPIO_PinSource2
	#define GREEN_LEDS			GPIOD,GPIO_PinSource3
	#define BLUE_LEDS			GPIOD,GPIO_PinSource4
	//	-	usb led red				  5
	#define SWITCH_COLOR_PORT	GPIOD->IDR6
	#define BIROUTE_PORT		(!GPIOD->IDR7)	//La biroute doit être par défaut dans l'état NON par défaut... pour qu'on soit sur qu'elle est là.
	//	HOKUYO U3TX					  8
	//	HOKUYO U3RX					  9
	#define LED_ERROR 			GPIOD,GPIO_PinSource10
	#define LED_SELFTEST 		GPIOD,GPIO_PinSource11
	#define LED_RUN  			GPIOD,GPIO_PinSource12	// Led verte carte STM
		#define LED_GREEN 			LED_RUN
	#define LED_CAN  			GPIOD,GPIO_PinSource13	// Led orange carte STM
		#define LED_ORANGE			LED_CAN
	#define LED_UART 			GPIOD,GPIO_PinSource14	// Led rouge carte STM
		#define LED_ROUGE			LED_UART
	#define LED_USER 			GPIOD,GPIO_PinSource15	// Led bleue carte STM
		#define LED_BLEU			LED_USER


#define PORT_E_IO_MASK	0xFFFF
	//	-				 			  0
	//	-				 			  1
	// - programmation -			  2
	// - programmation -			  3
	// - programmation -			  4
	// - programmation -			  5
	// - programmation -			  6
	#define SWITCH_LCD_PORT			GPIOE->IDR7
	#define SWITCH_EVIT_PORT		GPIOE->IDR8
	#define SWITCH_STRAT_1_PORT		GPIOE->IDR9
	#define SWITCH_STRAT_2_PORT		GPIOE->IDR10
	#define SWITCH_STRAT_3_PORT		GPIOE->IDR11
	#define BUTTON1_PORT		(!GPIOE->IDR12)	//Selftest
	#define BUTTON2_PORT		(!GPIOE->IDR13)	//LCD OK
	#define BUTTON3_PORT		(!GPIOE->IDR14)	//LCD Menu +
	#define BUTTON4_PORT		(!GPIOE->IDR15)	//LCD Menu -

#else


#define PORT_A_IO_MASK	0xFE9F
	#define BUTTON0_PORT		GPIOA->IDR0
	//								1
	//	XBEE/BT/U2TX				2
	//	XBEE/BT/U2RX				3
	//	-							4
	#define XBEE_RESET			GPIOA,GPIO_PinSource5
	#define PIN_RF_CONFIG		GPIOA,GPIO_PinSource6
	//								7
	#define LED_IHM_SET			GPIOA,GPIO_PinSource8
	//	-	usb						9
	//	-	usb						10
	//	-	usb						11
	//	-	usb						12
	//	-	programmation			13
	//	-	programmation			14
	//								15


#define PORT_B_IO_MASK	0x0DFF
	//	Mesure 24V					0
	// -							1
	#define POWER_WATCH_INT		GPIOB->IDR2
	// - programmation -			3
	//								4
	//								5
	//	U1TX						6
	//	U1RX						7
	//	-							8
	#define LCD_RESET_PORT		GPIOB,GPIO_PinSource9
	//	I2C RTC/LCD	 				10
	//	I2C RTC/LCD	 				11
	#define SD_CS				GPIOB,GPIO_PinSource12
	#define EEPROM_CS			GPIOB,GPIO_PinSource12
	#define EEPROM_SCK			GPIOB,GPIO_PinSource13
	#define EEPROM_SDO			GPIOB,GPIO_PinSource14
	#define EEPROM_SDI			GPIOB,GPIO_PinSource15


#define PORT_C_IO_MASK	0xE03E
	#define	USB_POWER_ON	 	GPIOC,GPIO_PinSource0
	#define RECALAGE_1			GPIOC->IDR1
	#define RECALAGE_2			GPIOC->IDR2
	//	Capteur						3
	//	Capteur						4
	//								5
	#define LED_BEACON_IR_GREEN	GPIOC,GPIO_PinSource6
	#define LED_BEACON_IR_RED	GPIOC,GPIO_PinSource7
	#define SYNCHRO_BEACON		GPIOC,GPIO_PinSource8
	#define BUZZER				GPIOC,GPIO_PinSource9
	#define LED_IHM_OK			GPIOC,GPIO_PinSource10
	#define EEPROM_WP			GPIOC,GPIO_PinSource11
	#define LED_IHM_UP			GPIOC,GPIO_PinSource11
	#define LED_IHM_DOWN		GPIOC,GPIO_PinSource12

	//#define 				GPIOC->ODR11
	#define EEPROM_HOLD			GPIOC,GPIO_PinSource12
	#define PORT_ROBOT_ID  		GPIOC->IDR13
	//	-	OSC32_in				14
	//	-	OSC32_out				15


#define PORT_D_IO_MASK	0x03E3
	//	CAN_RX						0
	//	CAN_TX						1
	#define RED_LEDS			GPIOD,GPIO_PinSource2
	#define GREEN_LEDS			GPIOD,GPIO_PinSource3
	#define BLUE_LEDS			GPIOD,GPIO_PinSource4
	//	-	usb led red				5
	//								6
	//								7
	//	HOKUYO U3TX					8
	//	HOKUYO U3RX					9
	#define LED_ERROR 			GPIOD,GPIO_PinSource10
	#define LED_SELFTEST 		GPIOD,GPIO_PinSource11
	#define LED_RUN  			GPIOD,GPIO_PinSource12	// Led verte carte STM
		#define LED_GREEN 			LED_RUN
	#define LED_CAN  			GPIOD,GPIO_PinSource13	// Led orange carte STM
		#define LED_ORANGE			LED_CAN
	#define LED_UART 			GPIOD,GPIO_PinSource14	// Led rouge carte STM
		#define LED_ROUGE			LED_UART
	#define LED_USER 			GPIOD,GPIO_PinSource15	// Led bleue carte STM
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
	//								8
	//								9
	//								10
	//								11
	//								12
	//								13
	//								14
	//								15

#endif


#endif /* CONFIG_PIN_H */
