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

#define PORT_B_IO_MASK	0xFCFF
	#define SWITCH_LCD_PORT			GPIOB->IDR0
	#define BUTTON_OK_PORT			GPIOB->IDR1
	#define BUTTON_UP_PORT			GPIOB->IDR2
	// - programmation -				3
	#define BUTTON_DOWN_PORT		GPIOB->IDR4
	#define BUTTON_SET_PORT			GPIOB->IDR5
	//	U1TX							6
	//	U1RX							7
	#define I_AM_READY				GPIOB,GPIO_Pin_8
	#define LCD_RESET_PORT			GPIOB,GPIO_Pin_9
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
	#define LED_IHM_OK				GPIOC,GPIO_Pin_6
	#define LED_IHM_SET				GPIOC,GPIO_Pin_7
	#define LED_IHM_UP				GPIOC,GPIO_Pin_8
	#define LED_IHM_DOWN			GPIOC,GPIO_Pin_9
	//	-								10
	//	-								11
	//	-								12
	#define PORT_ROBOT_ID			GPIOC->IDR13
	//	-								14
	//	-								15

#define PORT_D_IO_MASK	0x03E3
	//	CAN_RX							0
	//	CAN_TX							1
	#define GREEN_LED				GPIOD,GPIO_Pin_2
	  #define LED_UART				GREEN_LED
	#define RED_LED					GPIOD,GPIO_Pin_3
	#define BLUE_LED				GPIOD,GPIO_Pin_4
	//	-								5
	#define SWITCH_COLOR_PORT		GPIOD->IDR6
	#define BIROUTE_PORT			(!GPIOD->IDR7)	//La biroute doit être par défaut dans l'état NON par défaut... pour qu'on soit sur qu'elle est là.
	//									8
	//									9
	#define LED0_PORT				GPIOD,GPIO_Pin_10
	#define LED1_PORT				GPIOD,GPIO_Pin_11
	#define LED2_PORT				GPIOD,GPIO_Pin_12
	#define LED3_PORT				GPIOD,GPIO_Pin_13
	#define LED4_PORT				GPIOD,GPIO_Pin_14
	#define LED5_PORT				GPIOD,GPIO_Pin_15

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


#endif /* CONFIG_PIN_H */
