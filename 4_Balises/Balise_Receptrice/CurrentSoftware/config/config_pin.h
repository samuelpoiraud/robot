/*
 *  Club Robot ESEO 2015
 *
 *  Package : Carte Balise réceptrice
 *  Description : Configuration des pins
 *  Auteur : Arnaud
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
		//

	#define PORT_IO_A_1			PORT_IO_INPUT
	#define PORT_OPT_A_1		PORT_OPT_NO_PULL
		#define PORT_ROBOT_ID		GPIOA->IDR1

	#define PORT_IO_A_2			PORT_IO_INPUT	//PORT controllé par la carte STRATEGIE !
	#define PORT_OPT_A_2		PORT_OPT_NO_PULL
		// U2TX

	#define PORT_IO_A_3			PORT_IO_INPUT	//PORT controllé par la carte STRATEGIE !
	#define PORT_OPT_A_3		PORT_OPT_NO_PULL
		// U2RX

	#define PORT_IO_A_4			PORT_IO_INPUT	//PORT controllé par la carte STRATEGIE !
	#define PORT_OPT_A_4		PORT_OPT_NO_PULL
		#define RESET_XBEE			GPIOA,GPIO_Pin_4

	#define PORT_IO_A_5			PORT_IO_OUTPUT
	#define PORT_OPT_A_5		PORT_OPT_NO_PULL
		#define RF_RESET			GPIOA,GPIO_Pin_5

	#define PORT_IO_A_6			PORT_IO_OUTPUT
	#define PORT_OPT_A_6		PORT_OPT_NO_PULL
		#define RF_CONFIG			GPIOA,GPIO_Pin_6

	#define PORT_IO_A_7			PORT_IO_INPUT
	#define PORT_OPT_A_7		PORT_OPT_NO_PULL
		//

	#define PORT_IO_A_8			PORT_IO_OUTPUT
	#define PORT_OPT_A_8		PORT_OPT_NO_PULL
		#define LED_R_SYNC			GPIOA,GPIO_Pin_8

	#define PORT_IO_A_9			PORT_IO_OUTPUT
	#define PORT_OPT_A_9		PORT_OPT_NO_PULL
		#define LED_G_SYNC			GPIOA,GPIO_Pin_9

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
		//

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
		#define I_AM_READY			GPIOB,GPIO_Pin_8

	#define PORT_IO_B_9			PORT_IO_INPUT
	#define PORT_OPT_B_9		PORT_OPT_NO_PULL
		//

	#define PORT_IO_B_10		PORT_IO_INPUT
	#define PORT_OPT_B_10		PORT_OPT_NO_PULL
		//

	#define PORT_IO_B_11		PORT_IO_INPUT
	#define PORT_OPT_B_11		PORT_OPT_NO_PULL
		//

	#define PORT_IO_B_12		PORT_IO_INPUT
	#define PORT_OPT_B_12		PORT_OPT_NO_PULL
		//

	#define PORT_IO_B_13		PORT_IO_INPUT
	#define PORT_OPT_B_13		PORT_OPT_NO_PULL
		//

	#define PORT_IO_B_14		PORT_IO_INPUT
	#define PORT_OPT_B_14		PORT_OPT_NO_PULL
		//

	#define PORT_IO_B_15		PORT_IO_INPUT
	#define PORT_OPT_B_15		PORT_OPT_NO_PULL
		//


////////////////////////////////////////////////////////////////////////////////////
//--------------------------------------PORT C------------------------------------//
////////////////////////////////////////////////////////////////////////////////////
	#define PORT_IO_C_0			PORT_IO_OUTPUT
	#define PORT_OPT_C_0		PORT_OPT_NO_PULL
		#define LED_S0				GPIOC,GPIO_Pin_0

	#define PORT_IO_C_1			PORT_IO_OUTPUT
	#define PORT_OPT_C_1		PORT_OPT_NO_PULL
		#define LED_S1				GPIOC,GPIO_Pin_1

	#define PORT_IO_C_2			PORT_IO_OUTPUT
	#define PORT_OPT_C_2		PORT_OPT_NO_PULL
		#define LED_S2				GPIOC,GPIO_Pin_2

	#define PORT_IO_C_3			PORT_IO_OUTPUT
	#define PORT_OPT_C_3		PORT_OPT_NO_PULL
		#define LED_S3				GPIOC,GPIO_Pin_3

	#define PORT_IO_C_4			PORT_IO_OUTPUT
	#define PORT_OPT_C_4		PORT_OPT_NO_PULL
		#define LED_S4				GPIOC,GPIO_Pin_4

	#define PORT_IO_C_5			PORT_IO_OUTPUT
	#define PORT_OPT_C_5		PORT_OPT_NO_PULL
		#define LED_S5				GPIOC,GPIO_Pin_5

	#define PORT_IO_C_6			PORT_IO_OUTPUT
	#define PORT_OPT_C_6		PORT_OPT_NO_PULL
		#define LED_S6				GPIOC,GPIO_Pin_6

	#define PORT_IO_C_7			PORT_IO_OUTPUT
	#define PORT_OPT_C_7		PORT_OPT_NO_PULL
		#define LED_S7				GPIOC,GPIO_Pin_7

	#define PORT_IO_C_8			PORT_IO_OUTPUT
	#define PORT_OPT_C_8		PORT_OPT_NO_PULL
		#define LED_S8				GPIOC,GPIO_Pin_8

	#define PORT_IO_C_9			PORT_IO_OUTPUT
	#define PORT_OPT_C_9		PORT_OPT_NO_PULL
		#define LED_S9				GPIOC,GPIO_Pin_9

	#define PORT_IO_C_10		PORT_IO_OUTPUT
	#define PORT_OPT_C_10		PORT_OPT_NO_PULL
		#define LED_S10				GPIOC,GPIO_Pin_10

	#define PORT_IO_C_11		PORT_IO_OUTPUT
	#define PORT_OPT_C_11		PORT_OPT_NO_PULL
		#define LED_S11				GPIOC,GPIO_Pin_11

	#define PORT_IO_C_12		PORT_IO_OUTPUT
	#define PORT_OPT_C_12		PORT_OPT_NO_PULL
		#define LED_S12				GPIOC,GPIO_Pin_12

	#define PORT_IO_C_13		PORT_IO_OUTPUT
	#define PORT_OPT_C_13		PORT_OPT_NO_PULL
		#define LED_S13				GPIOC,GPIO_Pin_13

	#define PORT_IO_C_14		PORT_IO_OUTPUT
	#define PORT_OPT_C_14		PORT_OPT_NO_PULL
		#define LED_S14				GPIOC,GPIO_Pin_14

	#define PORT_IO_C_15		PORT_IO_OUTPUT
	#define PORT_OPT_C_15		PORT_OPT_NO_PULL
		#define LED_S15				GPIOC,GPIO_Pin_15

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
		#define LED_SGREEN			GPIOD,GPIO_Pin_2

	#define PORT_IO_D_3			PORT_IO_OUTPUT
	#define PORT_OPT_D_3		PORT_OPT_NO_PULL
		#define LED_SRED			GPIOD,GPIO_Pin_3

	#define PORT_IO_D_4			PORT_IO_OUTPUT
	#define PORT_OPT_D_4		PORT_OPT_NO_PULL
		#define LED_SBLUE			GPIOD,GPIO_Pin_4


	#define PORT_IO_D_5			PORT_IO_INPUT
	#define PORT_OPT_D_5		PORT_OPT_NO_PULL
		// Usb LED red

	#define PORT_IO_D_6			PORT_IO_INPUT
	#define PORT_OPT_D_6		PORT_OPT_NO_PULL
		//

	#define PORT_IO_D_7			PORT_IO_INPUT
	#define PORT_OPT_D_7		PORT_OPT_NO_PULL
		//

	#define PORT_IO_D_8			PORT_IO_INPUT
	#define PORT_OPT_D_8		PORT_OPT_NO_PULL
		// U3TX

	#define PORT_IO_D_9			PORT_IO_INPUT
	#define PORT_OPT_D_9		PORT_OPT_NO_PULL
		// U3RX

	#define PORT_IO_D_10		PORT_IO_OUTPUT
	#define PORT_OPT_D_10		PORT_OPT_NO_PULL
		#define LED_RUN  			GPIOD,GPIO_Pin_10

	#define PORT_IO_D_11		PORT_IO_INPUT
	#define PORT_OPT_D_11		PORT_OPT_NO_PULL
		//

	#define PORT_IO_D_12		PORT_IO_INPUT
	#define PORT_OPT_D_12		PORT_OPT_NO_PULL
		//

	#define PORT_IO_D_13		PORT_IO_OUTPUT
	#define PORT_OPT_D_13		PORT_OPT_NO_PULL
		#define LED_CAN  			GPIOD,GPIO_Pin_13

	#define PORT_IO_D_14		PORT_IO_INPUT	//Pas de led UART
	#define PORT_OPT_D_14		PORT_OPT_NO_PULL
		#define LED_UART 			GPIOD,GPIO_Pin_14

	#define PORT_IO_D_15		PORT_IO_OUTPUT
	#define PORT_OPT_D_15		PORT_OPT_NO_PULL
		#define LED_USER 			GPIOD,GPIO_Pin_15



////////////////////////////////////////////////////////////////////////////////////
//--------------------------------------PORT E------------------------------------//
////////////////////////////////////////////////////////////////////////////////////

		#define PORT_TSOP			GPIOE->IDR

	#define PORT_IO_E_0			PORT_IO_INPUT
	#define PORT_OPT_E_0		PORT_OPT_NO_PULL
		#define TSOP0				GPIOE->IDR0

	#define PORT_IO_E_1			PORT_IO_INPUT
	#define PORT_OPT_E_1		PORT_OPT_NO_PULL
		#define TSOP1				GPIOE->IDR1

	#define PORT_IO_E_2			PORT_IO_INPUT
	#define PORT_OPT_E_2		PORT_OPT_NO_PULL
		#define TSOP2				GPIOE->IDR2

	#define PORT_IO_E_3			PORT_IO_INPUT
	#define PORT_OPT_E_3		PORT_OPT_NO_PULL
		#define TSOP3				GPIOE->IDR03

	#define PORT_IO_E_4			PORT_IO_INPUT
	#define PORT_OPT_E_4		PORT_OPT_NO_PULL
		#define TSOP4				GPIOE->IDR4

	#define PORT_IO_E_5			PORT_IO_INPUT
	#define PORT_OPT_E_5		PORT_OPT_NO_PULL
		#define TSOP5				GPIOE->IDR5

	#define PORT_IO_E_6			PORT_IO_INPUT
	#define PORT_OPT_E_6		PORT_OPT_NO_PULL
		#define TSOP6				GPIOE->IDR6

	#define PORT_IO_E_7			PORT_IO_INPUT
	#define PORT_OPT_E_7		PORT_OPT_NO_PULL
		#define TSOP7				GPIOE->IDR7

	#define PORT_IO_E_8			PORT_IO_INPUT
	#define PORT_OPT_E_8		PORT_OPT_NO_PULL
		#define TSOP8				GPIOE->IDR8

	#define PORT_IO_E_9			PORT_IO_INPUT
	#define PORT_OPT_E_9		PORT_OPT_NO_PULL
		#define TSOP9				GPIOE->IDR9

	#define PORT_IO_E_10		PORT_IO_INPUT
	#define PORT_OPT_E_10		PORT_OPT_NO_PULL
		#define TSOP10				GPIOE->IDR10

	#define PORT_IO_E_11		PORT_IO_INPUT
	#define PORT_OPT_E_11		PORT_OPT_NO_PULL
		#define TSOP11				GPIOE->IDR11

	#define PORT_IO_E_12		PORT_IO_INPUT
	#define PORT_OPT_E_12		PORT_OPT_NO_PULL
		#define TSOP12				GPIOE->IDR12

	#define PORT_IO_E_13		PORT_IO_INPUT
	#define PORT_OPT_E_13		PORT_OPT_NO_PULL
		#define TSOP13				GPIOE->IDR13

	#define PORT_IO_E_14		PORT_IO_INPUT
	#define PORT_OPT_E_14		PORT_OPT_NO_PULL
		#define TSOP14				GPIOE->IDR14

	#define PORT_IO_E_15		PORT_IO_INPUT
	#define PORT_OPT_E_15		PORT_OPT_NO_PULL
		#define TSOP15				GPIOE->IDR15

#endif /* CONFIG_PIN_H */
