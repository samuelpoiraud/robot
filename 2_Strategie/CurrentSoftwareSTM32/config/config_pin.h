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
#include "../QS/QS_ports.h"
#include "../stm32f4xx/stm32f4xx_gpio.h"

////////////////////////////////////////////////////////////////////////////////////
//--------------------------------------PORT A------------------------------------//
////////////////////////////////////////////////////////////////////////////////////
	#define PORT_IO_A_0			PORT_IO_INPUT
	#define PORT_OPT_A_0		PORT_OPT_NO_PULL
		#define BUTTON0_PORT		GPIOA->IDR0

	#define PORT_IO_A_1			PORT_IO_INPUT
	#define PORT_OPT_A_1		PORT_OPT_NO_PULL
		//

	#define PORT_IO_A_2			PORT_IO_INPUT
	#define PORT_OPT_A_2		PORT_OPT_NO_PULL
		// XBEE/BT/U2TX

	#define PORT_IO_A_3			PORT_IO_INPUT
	#define PORT_OPT_A_3		PORT_OPT_NO_PULL
		// XBEE/BT/U2RX

	#define PORT_IO_A_4			PORT_IO_INPUT
	#define PORT_OPT_A_4		PORT_OPT_NO_PULL
		//

	#define PORT_IO_A_5			PORT_IO_OUTPUT
	#define PORT_OPT_A_5		PORT_OPT_NO_PULL
		#define XBEE_RESET			GPIOA,GPIO_Pin_5

	#define PORT_IO_A_6			PORT_IO_OUTPUT
	#define PORT_OPT_A_6		PORT_OPT_NO_PULL
		#define PIN_RF_CONFIG		GPIOA,GPIO_Pin_6

	#define PORT_IO_A_7			PORT_IO_INPUT
	#define PORT_OPT_A_7		PORT_OPT_NO_PULL
		//

	#define PORT_IO_A_8			PORT_IO_INPUT
	#define PORT_OPT_A_8		PORT_OPT_NO_PULL
		//

	#define PORT_IO_A_9			PORT_IO_INPUT
	#define PORT_OPT_A_9		PORT_OPT_NO_PULL
		// USB

	#define PORT_IO_A_10		PORT_IO_INPUT
	#define PORT_OPT_A_10		PORT_OPT_NO_PULL
		// USB

	#define PORT_IO_A_11		PORT_IO_INPUT
	#define PORT_OPT_A_11		PORT_OPT_NO_PULL
		// USB

	#define PORT_IO_A_12		PORT_IO_INPUT
	#define PORT_OPT_A_12		PORT_OPT_NO_PULL
		// USB

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
		// Mesure 24V

	#define PORT_IO_B_1			PORT_IO_INPUT
	#define PORT_OPT_B_1		PORT_OPT_NO_PULL
		//

	#define PORT_IO_B_2			PORT_IO_INPUT
	#define PORT_OPT_B_2		PORT_OPT_NO_PULL
		#define POWER_WATCH_INT		GPIOB->IDR2

	#define PORT_IO_B_3			PORT_IO_INPUT
	#define PORT_OPT_B_3		PORT_OPT_NO_PULL
		// Programmation

	#define PORT_IO_B_4			PORT_IO_INPUT
	#define PORT_OPT_B_4		PORT_OPT_NO_PULL

	#define PORT_IO_B_5			PORT_IO_INPUT
	#define PORT_OPT_B_5		PORT_OPT_NO_PULL

	#define PORT_IO_B_6			PORT_IO_INPUT
	#define PORT_OPT_B_6		PORT_OPT_NO_PULL
		// U1TX

	#define PORT_IO_B_7			PORT_IO_INPUT
	#define PORT_OPT_B_7		PORT_OPT_NO_PULL
		// U1RX

	#define PORT_IO_B_8			PORT_IO_INPUT
	#define PORT_OPT_B_8		PORT_OPT_NO_PULL
		//

	#define PORT_IO_B_9			PORT_IO_INPUT
	#define PORT_OPT_B_9		PORT_OPT_NO_PULL
		//

	#define PORT_IO_B_10		PORT_IO_INPUT
	#define PORT_OPT_B_10		PORT_OPT_NO_PULL
		// I2C RTC/LCD

	#define PORT_IO_B_11		PORT_IO_INPUT
	#define PORT_OPT_B_11		PORT_OPT_NO_PULL
		// I2C RTC/LCD

	#define PORT_IO_B_12		PORT_IO_OUTPUT
	#define PORT_OPT_B_12		PORT_OPT_NO_PULL
		#define SD_CS				GPIOB,GPIO_Pin_12

	#define PORT_IO_B_13		PORT_IO_INPUT
	#define PORT_OPT_B_13		PORT_OPT_NO_PULL
		// SD_SCK

	#define PORT_IO_B_14		PORT_IO_INPUT
	#define PORT_OPT_B_14		PORT_OPT_NO_PULL
		// SD_MISO

	#define PORT_IO_B_15		PORT_IO_INPUT
	#define PORT_OPT_B_15		PORT_OPT_NO_PULL
		// SD_MOSI



////////////////////////////////////////////////////////////////////////////////////
//--------------------------------------PORT C------------------------------------//
////////////////////////////////////////////////////////////////////////////////////
	#define PORT_IO_C_0			PORT_IO_OUTPUT
	#define PORT_OPT_C_0		PORT_OPT_NO_PULL
		#define	USB_POWER_ON	 	GPIOC,GPIO_Pin_0

	#define PORT_IO_C_1			PORT_IO_INPUT
	#define PORT_OPT_C_1		PORT_OPT_NO_PULL
		//

	#define PORT_IO_C_2			PORT_IO_INPUT
	#define PORT_OPT_C_2		PORT_OPT_NO_PULL
		#define RECALAGE_AR_G		!GPIOC->IDR2

	#define PORT_IO_C_3			PORT_IO_INPUT
	#define PORT_OPT_C_3		PORT_OPT_NO_PULL

	#define PORT_IO_C_4			PORT_IO_INPUT
	#define PORT_OPT_C_4		PORT_OPT_NO_PULL
		#define RECALAGE_AR_D		!GPIOC->IDR2

	#define PORT_IO_C_5			PORT_IO_INPUT
	#define PORT_OPT_C_5		PORT_OPT_NO_PULL
		//

	#define PORT_IO_C_6			PORT_IO_INPUT
	#define PORT_OPT_C_6		PORT_OPT_PULL_DOWN
		// CH0 left capteur couleur 2017
		//#define MOSFET_2_PIN		GPIOC,GPIO_Pin_6

	#define PORT_IO_C_7			PORT_IO_INPUT
	#define PORT_OPT_C_7		PORT_OPT_PULL_DOWN
		// CH0 right capteur couleur 2017
		//#define MOSFET_1_PIN		GPIOC,GPIO_Pin_7

	#define PORT_IO_C_8			PORT_IO_OUTPUT
	#define PORT_OPT_C_8		PORT_OPT_NO_PULL
		#define SYNCHRO_BEACON		GPIOC,GPIO_Pin_8

	#define PORT_IO_C_9			PORT_IO_OUTPUT
	#define PORT_OPT_C_9		PORT_OPT_NO_PULL
		#define BUZZER				GPIOC,GPIO_Pin_9

	#define PORT_IO_C_10		PORT_IO_INPUT
	#define PORT_OPT_C_10		PORT_OPT_PULL_DOWN
		// CH1 left capteur couleur 2017
		//#define MOSFET_4_PIN		GPIOC,GPIO_Pin_10

	#define PORT_IO_C_11		PORT_IO_INPUT
	#define PORT_OPT_C_11		PORT_OPT_PULL_DOWN
		// CH1 right capteur couleur 2017
		//#define MOSFET_3_PIN		GPIOC,GPIO_Pin_11

	#define PORT_IO_C_12		PORT_IO_INPUT
	#define PORT_OPT_C_12		PORT_OPT_NO_PULL
		//

	#define PORT_IO_C_13		PORT_IO_INPUT
	#define PORT_OPT_C_13		PORT_OPT_NO_PULL
		#define PORT_ROBOT_ID  		GPIOC->IDR13

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

	#define PORT_IO_D_2			PORT_IO_INPUT
	#define PORT_OPT_D_2		PORT_OPT_PULL_DOWN
		// CH2 left capteur couleur 2017
		//#define MOSFET_5_PIN		GPIOD,GPIO_Pin_2

	#define PORT_IO_D_3			PORT_IO_INPUT			//Pour activer le mosfet, mettre en OUTPUT
	#define PORT_OPT_D_3		PORT_OPT_PULL_DOWN
		// CH2 right capteur couleur 2017
		//#define MOSFET_6_PIN			GPIOD,GPIO_Pin_3

	#define PORT_IO_D_4			PORT_IO_INPUT			//Pour activer le mosfet, mettre en OUTPUT
	#define PORT_OPT_D_4		PORT_OPT_NO_PULL
		//#define MOSFET_7_PIN			GPIOD,GPIO_Pin_4

	#define PORT_IO_D_5			PORT_IO_INPUT			//Pour activer le mosfet, mettre en OUTPUT
	#define PORT_OPT_D_5		PORT_OPT_NO_PULL
		// Usb LED red
		//#define MOSFET_8_PIN			GPIOD,GPIO_Pin_5

	#define PORT_IO_D_6			PORT_IO_INPUT
	#define PORT_OPT_D_6		PORT_OPT_NO_PULL
		//

	#define PORT_IO_D_7			PORT_IO_INPUT
	#define PORT_OPT_D_7		PORT_OPT_NO_PULL
		//

	#define PORT_IO_D_8			PORT_IO_INPUT
	#define PORT_OPT_D_8		PORT_OPT_NO_PULL
		// HOKUYO U3TX

	#define PORT_IO_D_9			PORT_IO_INPUT
	#define PORT_OPT_D_9		PORT_OPT_NO_PULL
		// HOKUYO U3RX

	#define PORT_IO_D_10		PORT_IO_OUTPUT
	#define PORT_OPT_D_10		PORT_OPT_NO_PULL
		#define LED_ERROR 			GPIOD,GPIO_Pin_10

	#define PORT_IO_D_11		PORT_IO_OUTPUT
	#define PORT_OPT_D_11		PORT_OPT_NO_PULL
		#define LED_SELFTEST 		GPIOD,GPIO_Pin_11

	#define PORT_IO_D_12		PORT_IO_OUTPUT
	#define PORT_OPT_D_12		PORT_OPT_NO_PULL
		#define LED_RUN  			GPIOD,GPIO_Pin_12	// Led verte carte STM
		#define LED_GREEN 			LED_RUN

	#define PORT_IO_D_13		PORT_IO_OUTPUT
	#define PORT_OPT_D_13		PORT_OPT_NO_PULL
		#define LED_CAN  			GPIOD,GPIO_Pin_13	// Led orange carte STM
		#define LED_ORANGE			LED_CAN

	#define PORT_IO_D_14		PORT_IO_OUTPUT
	#define PORT_OPT_D_14		PORT_OPT_NO_PULL
		#define LED_UART 			GPIOD,GPIO_Pin_14	// Led rouge carte STM
		#define LED_ROUGE			LED_UART

	#define PORT_IO_D_15		PORT_IO_OUTPUT
	#define PORT_OPT_D_15		PORT_OPT_NO_PULL
		#define LED_USER 			GPIOD,GPIO_Pin_15	// Led bleue carte STM
		#define LED_BLEU			LED_USER


////////////////////////////////////////////////////////////////////////////////////
//--------------------------------------PORT E------------------------------------//
////////////////////////////////////////////////////////////////////////////////////
	#define PORT_IO_E_0			PORT_IO_INPUT
	#define PORT_OPT_E_0		PORT_OPT_NO_PULL
		//

	#define PORT_IO_E_1			PORT_IO_INPUT
	#define PORT_OPT_E_1		PORT_OPT_NO_PULL
		//

	#define PORT_IO_E_2			PORT_IO_INPUT
	#define PORT_OPT_E_2		PORT_OPT_NO_PULL
		// Programmation

	#define PORT_IO_E_3			PORT_IO_INPUT
	#define PORT_OPT_E_3		PORT_OPT_NO_PULL
		// Programmation

	#define PORT_IO_E_4			PORT_IO_INPUT
	#define PORT_OPT_E_4		PORT_OPT_NO_PULL
		// Programmation

	#define PORT_IO_E_5			PORT_IO_INPUT
	#define PORT_OPT_E_5		PORT_OPT_NO_PULL
		// Programmation

	#define PORT_IO_E_6			PORT_IO_INPUT
	#define PORT_OPT_E_6		PORT_OPT_NO_PULL
		// Programmation

	#define PORT_IO_E_7			PORT_IO_INPUT
	#define PORT_OPT_E_7		PORT_OPT_NO_PULL
		#define PORT_I_AM_STRAT		GPIOE,GPIO_Pin_7
		#define	READ_I_AM_STRAT		GPIOE->IDR7

	#define PORT_IO_E_8			PORT_IO_INPUT
	#define PORT_OPT_E_8		PORT_OPT_PULL_DOWN
		#define ACT_IS_READY			GPIOE->IDR8

	#define PORT_IO_E_9			PORT_IO_INPUT
	#define PORT_OPT_E_9		PORT_OPT_PULL_DOWN
		#define PROP_IS_READY			GPIOE->IDR9

	#define PORT_IO_E_10		PORT_IO_INPUT
	#define PORT_OPT_E_10		PORT_OPT_PULL_DOWN
		#define IHM_IS_READY			GPIOE->IDR10

	#define PORT_IO_E_11		PORT_IO_INPUT
	#define PORT_OPT_E_11		PORT_OPT_NO_PULL
		//

	#define PORT_IO_E_12		PORT_IO_INPUT
	#define PORT_OPT_E_12		PORT_OPT_NO_PULL
		//

	#define PORT_IO_E_13		PORT_IO_INPUT
	#define PORT_OPT_E_13		PORT_OPT_NO_PULL
		//

	#define PORT_IO_E_14		PORT_IO_INPUT
	#define PORT_OPT_E_14		PORT_OPT_NO_PULL
		//

	#define PORT_IO_E_15		PORT_IO_INPUT
	#define PORT_OPT_E_15		PORT_OPT_NO_PULL
		//

#define RECALAGE_AV_D		1
#define RECALAGE_AV_G		1

#endif /* CONFIG_PIN_H */
