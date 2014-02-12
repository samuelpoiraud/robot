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
#define PORT_A_IO_MASK	0xFE9B
	#define BUTTON0_PORT		GPIOA->IDR0
	#define SWITCH_DEBUG		GPIOA->IDR1
	//	XBEE/BT/U2TX				  2
	//	XBEE/BT/U2RX				  3
	//	-				 			  4
	#define XBEE_RESET			GPIOA->ODR5
	//	-				 			  6
	#define SWITCH_VERBOSE		GPIOA->IDR7
	#define LED_IHM_SET			GPIOA->ODR8
	//	-	usb			 			  9
	//	-	usb			 			  10
	//	-	usb			 			  11
	//	-	usb			 			  12
	//	-	programmation			  13
	//	-	programmation 			  14
	#define BUTTON5_PORT		(!GPIOA->IDR15)
#define PORT_B_IO_MASK	0x00BF
	//	Capteur						  0
	//	Capteur						  1
	#define POWER_WATCH_INT		GPIOB->IDR2
	// - programmation -			  3
	#define SWITCH_XBEE			GPIOB->IDR4
	#define SWITCH_SAVE			GPIOB->IDR5
	//	U1TX						  6
	//	U1RX						  7
	//	-			 	  			  8
	#define LCD_RESET_PORT		GPIOB->ODR9
	//	I2C RTC/LCD	 			  	  10
	//	I2C RTC/LCD	 			  	  11
	#define SD_CS				GPIOB->ODR12
	#define EEPROM_CS			GPIOB->ODR12
	#define EEPROM_SCK			GPIOB->ODR13
	#define EEPROM_SDO			GPIOB->ODR14
	#define EEPROM_SDI			GPIOB->ODR15


#define PORT_C_IO_MASK	0x202E
	#define	USB_POWER_ON	 	GPIOC->ODR0
	#define FRESCO_1			GPIOC->IDR1
	#define FRESCO_2			GPIOC->IDR2
	//	Capteur			 			  3
	//	Capteur			 			  4
	#define BUTTON6_PORT		(!GPIOC->IDR5)
	#define LED_BEACON_IR_GREEN	GPIOC->ODR6
	#define LED_BEACON_IR_RED	GPIOC->ODR7
	#define SYNCHRO_BEACON		GPIOC->ODR8
	#define BUZZER				GPIOC->ODR9
	#define LED_IHM_OK			GPIOC->ODR10
	#define EEPROM_WP			GPIOC->ODR11
	#define LED_IHM_UP			GPIOC->ODR11
	#define LED_IHM_DOWN			GPIOC->ODR12

	//#define 				GPIOC->ODR11
	#define EEPROM_HOLD			GPIOC->ODR12
	#define PORT_ROBOT_ID  		GPIOC->IDR13
	//	-	OSC32_in 			  	  14
	//	-	OSC32_out 			  	  15


#define PORT_D_IO_MASK	0x02C3
	//	CAN_RX						  0
	//	CAN_TX						  1
	#define RED_LEDS			GPIOD->ODR2
	#define GREEN_LEDS			GPIOD->ODR3
	#define BLUE_LEDS			GPIOD->ODR4
	//	-							  4
	//	-	usb led red				  5
	#define SWITCH_COLOR		GPIOD->IDR6
	#define BIROUTE				GPIOD->IDR7
	//	HOKUYO U3TX					  8
	//	HOKUYO U3RX					  9
	#define LED_ERROR 			GPIOD->ODR10
	#define LED_SELFTEST 		GPIOD->ODR11
	#define LED_RUN  			GPIOD->ODR12	// Led verte carte STM
		#define LED_GREEN 			LED_RUN
	#define LED_CAN  			GPIOD->ODR13	// Led orange carte STM
		#define LED_ORANGE			LED_CAN
	#define LED_UART 			GPIOD->ODR14	// Led rouge carte STM
		#define LED_ROUGE			LED_UART
	#define LED_USER 			GPIOD->ODR15	// Led bleue carte STM
		#define LED_BLEU			LED_USER


#define PORT_E_IO_MASK	0xFFFF
	//	-				 			  0
	//	-				 			  1
	// - programmation -			  2
	// - programmation -			  3
	// - programmation -			  4
	// - programmation -			  5
	// - programmation -			  6
	#define SWITCH_LCD			GPIOE->IDR7
	#define SWITCH_EVIT			GPIOE->IDR8
	#define SWITCH_STRAT_1		GPIOE->IDR9
	#define SWITCH_STRAT_2		GPIOE->IDR10
	#define SWITCH_STRAT_3		GPIOE->IDR11
	#define BUTTON1_PORT		(!GPIOE->IDR12)	//Selftest
	#define BUTTON2_PORT		(!GPIOE->IDR13)	//LCD OK
	#define BUTTON3_PORT		(!GPIOE->IDR14)	//LCD Menu +
	#define BUTTON4_PORT		(!GPIOE->IDR15)	//LCD Menu -

//Nulle part...
#define SWITCH_STRAT_4		FALSE	//Temporaire
#define SWITCH_STRAT_5		FALSE	//Temporaire



#define SICK_SENSOR_FOE_GLASSES_PIN FALSE//PORTBbits.RB15   //sur Tiny
#define GLASS_SENSOR_LEFT			FALSE//(!PORTBbits.RB3)    //sur Krusty, en logique inversée
#define GLASS_SENSOR_RIGHT			FALSE//(!PORTBbits.RB5)    //sur Krusty, en logique inversée

#define PORT_SWITCH_XBEE	GPIOB->IDR4
//#define PORT_SWITCH_XBEE	FALSE	//Utiliser cette ligne pour désactiver le XBee...

#endif /* CONFIG_PIN_H */
