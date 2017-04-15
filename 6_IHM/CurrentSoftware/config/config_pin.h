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


#include "../QS/QS_ports.h"
#include "../stm32f4xx/stm32f4xx_gpio.h"

////////////////////////////////////////////////////////////////////////////////////
//--------------------------------------PORT A------------------------------------//
////////////////////////////////////////////////////////////////////////////////////
	#define PORT_IO_A_0			PORT_IO_INPUT
	#define PORT_OPT_A_0		PORT_OPT_NO_PULL
		#define BUTTON_SELFTEST_PORT	GPIOA->IDR0

	#define PORT_IO_A_1			PORT_IO_INPUT
	#define PORT_OPT_A_1		PORT_OPT_NO_PULL
		#define BUTTON_CALIBRATION_PORT	GPIOA->IDR1

	#define PORT_IO_A_2			PORT_IO_INPUT
	#define PORT_OPT_A_2		PORT_OPT_NO_PULL
		//

	#define PORT_IO_A_3			PORT_IO_INPUT
	#define PORT_OPT_A_3		PORT_OPT_NO_PULL
		#define BUTTON_PRINTMATCH_PORT	GPIOA->IDR3

	#define PORT_IO_A_4			PORT_IO_INPUT
	#define PORT_OPT_A_4		PORT_OPT_NO_PULL
		#define BUTTON0_PORT			GPIOA->IDR4

	#define PORT_IO_A_5			PORT_IO_INPUT
	#define PORT_OPT_A_5		PORT_OPT_NO_PULL
		// Mesure 12V Hokuyo

	#define PORT_IO_A_6			PORT_IO_INPUT
	#define PORT_OPT_A_6		PORT_OPT_NO_PULL
		// Mesure 24V Puissance

	#define PORT_IO_A_7			PORT_IO_INPUT
	#define PORT_OPT_A_7		PORT_OPT_NO_PULL
		#define BUTTON1_PORT			GPIOA->IDR7

	#define PORT_IO_A_8			PORT_IO_INPUT
	#define PORT_OPT_A_8		PORT_OPT_NO_PULL
		//

	#define PORT_IO_A_9			PORT_IO_INPUT
	#define PORT_OPT_A_9		PORT_OPT_NO_PULL
		#define BUTTON2_PORT			GPIOA->IDR9

	#define PORT_IO_A_10		PORT_IO_INPUT
	#define PORT_OPT_A_10		PORT_OPT_NO_PULL
		#define BUTTON3_PORT			GPIOA->IDR10

	#define PORT_IO_A_11		PORT_IO_INPUT
	#define PORT_OPT_A_11		PORT_OPT_NO_PULL
		#define BUTTON4_PORT			GPIOA->IDR11

	#define PORT_IO_A_12		PORT_IO_INPUT
	#define PORT_OPT_A_12		PORT_OPT_NO_PULL
		#define BUTTON5_PORT			GPIOA->IDR12

	#define PORT_IO_A_13		PORT_IO_INPUT
	#define PORT_OPT_A_13		PORT_OPT_NO_PULL
		// Programmation

	#define PORT_IO_A_14		PORT_IO_INPUT
	#define PORT_OPT_A_14		PORT_OPT_NO_PULL
		// Programmation

	#define PORT_IO_A_15		PORT_IO_INPUT
	#define PORT_OPT_A_15		PORT_OPT_NO_PULL
		//


////////////////////////////////////////////////////////////////////////////////////
//--------------------------------------PORT B------------------------------------//
////////////////////////////////////////////////////////////////////////////////////
	#define PORT_IO_B_0			PORT_IO_INPUT
	#define PORT_OPT_B_0		PORT_OPT_NO_PULL
		#define SWITCH_LCD_PORT			GPIOB->IDR0

	#define PORT_IO_B_1			PORT_IO_INPUT
	#define PORT_OPT_B_1		PORT_OPT_NO_PULL
		//

	#define PORT_IO_B_2			PORT_IO_INPUT
	#define PORT_OPT_B_2		PORT_OPT_NO_PULL
		//

	#define PORT_IO_B_3			PORT_IO_INPUT
	#define PORT_OPT_B_3		PORT_OPT_NO_PULL
		// Programmation

	#define PORT_IO_B_4			PORT_IO_INPUT
	#define PORT_OPT_B_4		PORT_OPT_NO_PULL
		//

	#define PORT_IO_B_5			PORT_IO_INPUT
	#define PORT_OPT_B_5		PORT_OPT_NO_PULL
		//

	#define PORT_IO_B_6			PORT_IO_INPUT
	#define PORT_OPT_B_6		PORT_OPT_NO_PULL
		// U1TX

	#define PORT_IO_B_7			PORT_IO_INPUT
	#define PORT_OPT_B_7		PORT_OPT_NO_PULL
		// U1RX

	#define PORT_IO_B_8			PORT_IO_OUTPUT
	#define PORT_OPT_B_8		PORT_OPT_NO_PULL
		#define I_AM_READY				GPIOB,GPIO_Pin_8

	#define PORT_IO_B_9			PORT_IO_OUTPUT
	#define PORT_OPT_B_9		PORT_OPT_NO_PULL
		#define LCD_RESET_PORT			GPIOB,GPIO_Pin_9

	#define PORT_IO_B_10		PORT_IO_INPUT
	#define PORT_OPT_B_10		PORT_OPT_NO_PULL
		//

	#define PORT_IO_B_11		PORT_IO_INPUT
	#define PORT_OPT_B_11		PORT_OPT_NO_PULL
		//

	#define PORT_IO_B_12		PORT_IO_OUTPUT
	#define PORT_OPT_B_12		PORT_OPT_NO_PULL
		#define LCD_CS					GPIOB,GPIO_Pin_12

	#define PORT_IO_B_13		PORT_IO_INPUT
	#define PORT_OPT_B_13		PORT_OPT_NO_PULL
		// SPI

	#define PORT_IO_B_14		PORT_IO_INPUT
	#define PORT_OPT_B_14		PORT_OPT_NO_PULL
		// SPI

	#define PORT_IO_B_15		PORT_IO_INPUT
	#define PORT_OPT_B_15		PORT_OPT_NO_PULL
		// SPI



////////////////////////////////////////////////////////////////////////////////////
//--------------------------------------PORT C------------------------------------//
////////////////////////////////////////////////////////////////////////////////////
	#define PORT_IO_C_0			PORT_IO_INPUT
	#define PORT_OPT_C_0		PORT_OPT_NO_PULL
		//

	#define PORT_IO_C_1			PORT_IO_INPUT
	#define PORT_OPT_C_1		PORT_OPT_NO_PULL
		#define SWITCH16_PORT			GPIOC->IDR1

	#define PORT_IO_C_2			PORT_IO_INPUT
	#define PORT_OPT_C_2		PORT_OPT_NO_PULL
		#define SWITCH17_PORT			GPIOC->IDR2

	#define PORT_IO_C_3			PORT_IO_INPUT
	#define PORT_OPT_C_3		PORT_OPT_NO_PULL
		#define SWITCH18_PORT			GPIOC->IDR3

	#define PORT_IO_C_4			PORT_IO_INPUT
	#define PORT_OPT_C_4		PORT_OPT_NO_PULL
		#define BUTTON_SUSPEND_RESUMSE_PORT	GPIOC->IDR4

	#define PORT_IO_C_5			PORT_IO_INPUT
	#define PORT_OPT_C_5		PORT_OPT_NO_PULL
		#define SWITCH2_PORT			GPIOC->IDR5

	#define PORT_IO_C_6			PORT_IO_INPUT
	#define PORT_OPT_C_6		PORT_OPT_NO_PULL
		//

	#define PORT_IO_C_7			PORT_IO_INPUT
	#define PORT_OPT_C_7		PORT_OPT_NO_PULL
		//

	#define PORT_IO_C_8			PORT_IO_INPUT
	#define PORT_OPT_C_8		PORT_OPT_NO_PULL
		//

	#define PORT_IO_C_9			PORT_IO_INPUT
	#define PORT_OPT_C_9		PORT_OPT_NO_PULL
		//

	#define PORT_IO_C_10		PORT_IO_OUTPUT
	#define PORT_OPT_C_10		PORT_OPT_NO_PULL
		#define LCD_CS_TOUCH			GPIOC,GPIO_Pin_10

	#define PORT_IO_C_11		PORT_IO_OUTPUT
	#define PORT_OPT_C_11		PORT_OPT_NO_PULL
		#define LCD_D_C_PORT			GPIOC,GPIO_Pin_11

	#define PORT_IO_C_12		PORT_IO_INPUT
	#define PORT_OPT_C_12		PORT_OPT_NO_PULL
		//

	#define PORT_IO_C_13		PORT_IO_INPUT
	#define PORT_OPT_C_13		PORT_OPT_NO_PULL
		#define PORT_ROBOT_ID			GPIOC->IDR13

	#define PORT_IO_C_14		PORT_IO_INPUT
	#define PORT_OPT_C_14		PORT_OPT_NO_PULL
		// OSC32_in

	#define PORT_IO_C_15		PORT_IO_INPUT
	#define PORT_OPT_C_15		PORT_OPT_NO_PULL
		// OSC32_out



////////////////////////////////////////////////////////////////////////////////////
//--------------------------------------PORT D------------------------------------//
////////////////////////////////////////////////////////////////////////////////////
	#define PORT_IO_D_0			PORT_IO_INPUT
	#define PORT_OPT_D_0		PORT_OPT_NO_PULL
		// CAN_RX

	#define PORT_IO_D_1			PORT_IO_INPUT
	#define PORT_OPT_D_1		PORT_OPT_NO_PULL
		// CAN_TX

	#define PORT_IO_D_2			PORT_IO_OUTPUT
	#define PORT_OPT_D_2		PORT_OPT_NO_PULL
		#define GREEN_LED				GPIOD,GPIO_Pin_2
		#define GREEN_LED_MASK			GPIO_Pin_2

	#define PORT_IO_D_3			PORT_IO_OUTPUT
	#define PORT_OPT_D_3		PORT_OPT_NO_PULL
		#define RED_LED					GPIOD,GPIO_Pin_3
		#define RED_LED_MASK			GPIO_Pin_3

	#define PORT_IO_D_4			PORT_IO_OUTPUT
	#define PORT_OPT_D_4		PORT_OPT_NO_PULL
		#define BLUE_LED				GPIOD,GPIO_Pin_4
		#define BLUE_LED_MASK			GPIO_Pin_4

	#define PORT_IO_D_5			PORT_IO_INPUT
	#define PORT_OPT_D_5		PORT_OPT_NO_PULL
		#define IRQ_TOUCH			GPIOD->IDR5

	#define PORT_IO_D_6			PORT_IO_INPUT
	#define PORT_OPT_D_6		PORT_OPT_NO_PULL
		#define SWITCH_COLOR_PORT		GPIOD->IDR6

	#define PORT_IO_D_7			PORT_IO_INPUT
	#define PORT_OPT_D_7		PORT_OPT_NO_PULL
		#define BIROUTE_PORT			GPIOD->IDR7	//La biroute doit être par défaut dans l'état NON par défaut... pour qu'on soit sur qu'elle est là.

	#define PORT_IO_D_8			PORT_IO_INPUT
	#define PORT_OPT_D_8		PORT_OPT_NO_PULL
		// U3TX

	#define PORT_IO_D_9			PORT_IO_INPUT
	#define PORT_OPT_D_9		PORT_OPT_NO_PULL
		// U3RX

	#define PORT_IO_D_10		PORT_IO_OUTPUT
	#define PORT_OPT_D_10		PORT_OPT_NO_PULL
		#define LED0_PORT				GPIOD,GPIO_Pin_10

	#define PORT_IO_D_11		PORT_IO_OUTPUT
	#define PORT_OPT_D_11		PORT_OPT_NO_PULL
		#define LED1_PORT				GPIOD,GPIO_Pin_11

	#define PORT_IO_D_12		PORT_IO_OUTPUT
	#define PORT_OPT_D_12		PORT_OPT_NO_PULL
		#define LED2_PORT				GPIOD,GPIO_Pin_12

	#define PORT_IO_D_13		PORT_IO_OUTPUT
	#define PORT_OPT_D_13		PORT_OPT_NO_PULL
		#define LED3_PORT				GPIOD,GPIO_Pin_13

	#define PORT_IO_D_14		PORT_IO_OUTPUT
	#define PORT_OPT_D_14		PORT_OPT_NO_PULL
		#define LED4_PORT				GPIOD,GPIO_Pin_14

	#define PORT_IO_D_15		PORT_IO_OUTPUT
	#define PORT_OPT_D_15		PORT_OPT_NO_PULL
		#define LED5_PORT				GPIOD,GPIO_Pin_15
		#define RELAY_RESISTOR			GPIOD,GPIO_Pin_15


////////////////////////////////////////////////////////////////////////////////////
//--------------------------------------PORT E------------------------------------//
////////////////////////////////////////////////////////////////////////////////////
	#define PORT_IO_E_0			PORT_IO_INPUT
	#define PORT_OPT_E_0		PORT_OPT_NO_PULL
		#define SWITCH0_PORT			GPIOE->IDR0

	#define PORT_IO_E_1			PORT_IO_INPUT
	#define PORT_OPT_E_1		PORT_OPT_NO_PULL
		#define SWITCH1_PORT			GPIOE->IDR1

	#define PORT_IO_E_2			PORT_IO_INPUT
	#define PORT_OPT_E_2		PORT_OPT_NO_PULL
		// Programmation

	#define PORT_IO_E_3			PORT_IO_INPUT
	#define PORT_OPT_E_3		PORT_OPT_NO_PULL
		#define SWITCH3_PORT			GPIOE->IDR3

	#define PORT_IO_E_4			PORT_IO_INPUT
	#define PORT_OPT_E_4		PORT_OPT_NO_PULL
		#define SWITCH4_PORT			GPIOE->IDR4

	#define PORT_IO_E_5			PORT_IO_INPUT
	#define PORT_OPT_E_5		PORT_OPT_NO_PULL
		#define SWITCH5_PORT			GPIOE->IDR5

	#define PORT_IO_E_6			PORT_IO_INPUT
	#define PORT_OPT_E_6		PORT_OPT_NO_PULL
		#define SWITCH6_PORT			GPIOE->IDR6

	#define PORT_IO_E_7			PORT_IO_INPUT
	#define PORT_OPT_E_7		PORT_OPT_NO_PULL
		#define SWITCH7_PORT			GPIOE->IDR7

	#define PORT_IO_E_8			PORT_IO_INPUT
	#define PORT_OPT_E_8		PORT_OPT_NO_PULL
		#define SWITCH8_PORT			GPIOE->IDR8

	#define PORT_IO_E_9			PORT_IO_INPUT
	#define PORT_OPT_E_9		PORT_OPT_NO_PULL
		#define SWITCH9_PORT			GPIOE->IDR9

	#define PORT_IO_E_10		PORT_IO_INPUT
	#define PORT_OPT_E_10		PORT_OPT_NO_PULL
		#define SWITCH10_PORT			GPIOE->IDR10

	#define PORT_IO_E_11		PORT_IO_INPUT
	#define PORT_OPT_E_11		PORT_OPT_NO_PULL
		#define SWITCH11_PORT			GPIOE->IDR11

	#define PORT_IO_E_12		PORT_IO_INPUT
	#define PORT_OPT_E_12		PORT_OPT_NO_PULL
		#define SWITCH12_PORT			GPIOE->IDR12

	#define PORT_IO_E_13		PORT_IO_INPUT
	#define PORT_OPT_E_13		PORT_OPT_NO_PULL
		#define SWITCH13_PORT			GPIOE->IDR13

	#define PORT_IO_E_14		PORT_IO_INPUT
	#define PORT_OPT_E_14		PORT_OPT_NO_PULL
		#define SWITCH14_PORT			GPIOE->IDR14

	#define PORT_IO_E_15		PORT_IO_INPUT
	#define PORT_OPT_E_15		PORT_OPT_NO_PULL
		#define SWITCH15_PORT			GPIOE->IDR15


#endif /* CONFIG_PIN_H */
