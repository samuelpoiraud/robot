/*
 *  Club Robot ESEO 2013 - 2014
 *
 *  $Id$
 *
 *  Package : Carte Actionneur
 *  Description : Configuration des pins
 *  Auteur : Jacen, Alexis
 */

#ifndef SMALL_CONFIG_PIN_H
#define SMALL_CONFIG_PIN_H



////////////////////////////////////////////////////////////////////////////////////
//--------------------------------------PORT A------------------------------------//
////////////////////////////////////////////////////////////////////////////////////
	#define PORT_IO_A_0			PORT_IO_INPUT
		#define BUTTON0_PORT		GPIOA->IDR0

	#define PORT_IO_A_1			PORT_IO_INPUT
		//

	#define PORT_IO_A_2			PORT_IO_INPUT
		// AX12/U2TX

	#define PORT_IO_A_3			PORT_IO_INPUT
		// AX12/U2RX

	#define PORT_IO_A_4			PORT_IO_INPUT
		//

	#define PORT_IO_A_5			PORT_IO_OUTPUT
		#define AX12_DIRECTION_PORT_RX24 GPIOA,GPIO_Pin_5

	#define PORT_IO_A_6			PORT_IO_INPUT
		//

	#define PORT_IO_A_7			PORT_IO_INPUT
		//

	#define PORT_IO_A_8			PORT_IO_OUTPUT
		#define CUP_NIPPER_DCM_SENS		 GPIOA,GPIO_Pin_8

	#define PORT_IO_A_9			PORT_IO_INPUT
		// USB

	#define PORT_IO_A_10		PORT_IO_INPUT
		// USB

	#define PORT_IO_A_11		PORT_IO_INPUT
		// USB

	#define PORT_IO_A_12		PORT_IO_INPUT
		// USB

	#define PORT_IO_A_13		PORT_IO_INPUT
		// Programmation

	#define PORT_IO_A_14		PORT_IO_INPUT
		// Programmation

	#define PORT_IO_A_15		PORT_IO_INPUT

////////////////////////////////////////////////////////////////////////////////////
//--------------------------------------PORT B------------------------------------//
////////////////////////////////////////////////////////////////////////////////////
	#define PORT_IO_B_0			PORT_IO_INPUT
		#define WT100_GOBELET_RIGHT		(!(GPIOB->IDR0))

	#define PORT_IO_B_1			PORT_IO_INPUT
		#define WT100_GOBELET_LEFT		(!(GPIOB->IDR1))

	#define PORT_IO_B_2			PORT_IO_INPUT
		//

	#define PORT_IO_B_3			PORT_IO_INPUT
		// Programmation

	#define PORT_IO_B_4			PORT_IO_INPUT
		//

	#define PORT_IO_B_5			PORT_IO_INPUT
		//

	#define PORT_IO_B_6			PORT_IO_INPUT
		// U1TX

	#define PORT_IO_B_7			PORT_IO_INPUT
		// U1RX

	#define PORT_IO_B_8			PORT_IO_OUTPUT
		#define I_AM_READY			GPIOB,GPIO_Pin_8

	#define PORT_IO_B_9			PORT_IO_INPUT
		//

	#define PORT_IO_B_10		PORT_IO_INPUT
		//

	#define PORT_IO_B_11		PORT_IO_OUTPUT
		#define AX12_DIRECTION_PORT_AX12	GPIOB,GPIO_Pin_11

	#define PORT_IO_B_12		PORT_IO_INPUT
		//

	#define PORT_IO_B_13		PORT_IO_INPUT
		//

	#define PORT_IO_B_14		PORT_IO_INPUT
		//

	#define PORT_IO_B_15		PORT_IO_INPUT
		//


////////////////////////////////////////////////////////////////////////////////////
//--------------------------------------PORT C------------------------------------//
////////////////////////////////////////////////////////////////////////////////////
	#define PORT_IO_C_0			PORT_IO_INPUT
		//

	#define PORT_IO_C_1			PORT_IO_INPUT
		//

	#define PORT_IO_C_2			PORT_IO_INPUT
		//

	#define PORT_IO_C_3			PORT_IO_INPUT
		//

	#define PORT_IO_C_4			PORT_IO_INPUT
		//

	#define PORT_IO_C_5			PORT_IO_INPUT
		//

	#define PORT_IO_C_6			PORT_IO_INPUT
		//

	#define PORT_IO_C_7			PORT_IO_INPUT
		//

	#define PORT_IO_C_8			PORT_IO_INPUT
		//

	#define PORT_IO_C_9			PORT_IO_INPUT
		//

	#define PORT_IO_C_10		PORT_IO_INPUT
		//

	#define PORT_IO_C_11		PORT_IO_INPUT
		//

	#define PORT_IO_C_12		PORT_IO_OUTPUT
		#define POMPE_WOOD_SENS	GPIOC,GPIO_Pin_12

	#define PORT_IO_C_13		PORT_IO_INPUT
		#define PORT_ROBOT_ID  		GPIOC->IDR13

	#define PORT_IO_C_14		PORT_IO_INPUT
		// OSC32_in

	#define PORT_IO_C_15		PORT_IO_INPUT
		// OSC32_out

////////////////////////////////////////////////////////////////////////////////////
//--------------------------------------PORT D------------------------------------//
////////////////////////////////////////////////////////////////////////////////////
	#define PORT_IO_D_0			PORT_IO_INPUT
		// CAN_RX

	#define PORT_IO_D_1			PORT_IO_INPUT
		// CAN_TX

	#define PORT_IO_D_2			PORT_IO_INPUT
		//

	#define PORT_IO_D_3			PORT_IO_INPUT
		//

	#define PORT_IO_D_4			PORT_IO_INPUT
		//

	#define PORT_IO_D_5			PORT_IO_INPUT
		// Usb LED red

	#define PORT_IO_D_6			PORT_IO_INPUT
		//

	#define PORT_IO_D_7			PORT_IO_INPUT
		//

	#define PORT_IO_D_8			PORT_IO_INPUT
		// U3TX

	#define PORT_IO_D_9			PORT_IO_INPUT
		// U3RX

	#define PORT_IO_D_10		PORT_IO_OUTPUT
		#define LED_ERROR 			GPIOD,GPIO_Pin_10

	#define PORT_IO_D_11		PORT_IO_OUTPUT
		#define LED_SELFTEST 		GPIOD,GPIO_Pin_11

	#define PORT_IO_D_12		PORT_IO_OUTPUT
		#define LED_RUN  			GPIOD,GPIO_Pin_12 // Led verte carte STM
		#define LED_GREEN 			LED_RUN

	#define PORT_IO_D_13		PORT_IO_OUTPUT
		#define LED_CAN  			GPIOD,GPIO_Pin_13 // Led orange carte STM
		#define LED_ORANGE			LED_CAN

	#define PORT_IO_D_14		PORT_IO_OUTPUT
		#define LED_UART 			GPIOD,GPIO_Pin_14 // Led rouge carte STM
		#define LED_ROUGE			LED_UART

	#define PORT_IO_D_15		PORT_IO_OUTPUT
		#define LED_USER 			GPIOD,GPIO_Pin_15 // Led bleue carte STM
		#define LED_BLEU			LED_USER


////////////////////////////////////////////////////////////////////////////////////
//--------------------------------------PORT E------------------------------------//
////////////////////////////////////////////////////////////////////////////////////
	#define PORT_IO_E_0			PORT_IO_INPUT
		//

	#define PORT_IO_E_1			PORT_IO_INPUT
		//

	#define PORT_IO_E_2			PORT_IO_INPUT
		// Programmation

	#define PORT_IO_E_3			PORT_IO_INPUT
		// Programmation

	#define PORT_IO_E_4			PORT_IO_INPUT
		// Programmation

	#define PORT_IO_E_5			PORT_IO_INPUT
		// Programmation

	#define PORT_IO_E_6			PORT_IO_INPUT
		// Programmation

	#define PORT_IO_E_7			PORT_IO_INPUT
		#define PORT_I_AM_STRAT		GPIOE,GPIO_Pin_7
		#define	READ_I_AM_STRAT		GPIOE->IDR7

	#define PORT_IO_E_8			PORT_IO_INPUT
		#define PORT_I_AM_ACT		GPIOE,GPIO_Pin_8
		#define	READ_I_AM_ACT		GPIOE->IDR8

	#define PORT_IO_E_9			PORT_IO_INPUT
		#define PORT_I_AM_PROP		GPIOE,GPIO_Pin_9
		#define	READ_I_AM_PROP		GPIOE->IDR9

	#define PORT_IO_E_10		PORT_IO_INPUT
		//

	#define PORT_IO_E_11		PORT_IO_INPUT
		//

	#define PORT_IO_E_12		PORT_IO_INPUT
		//

	#define PORT_IO_E_13		PORT_IO_INPUT
		//

	#define PORT_IO_E_14		PORT_IO_INPUT
		//

	#define PORT_IO_E_15		PORT_IO_INPUT
		//

//--------------------Configuration des ID des actionneurs-----------------------

/* ID des différents AX12 */
	#define LEFT_ARM_AX12_ID                       11

#define POMPE_WOOD_PWM_NUM		4

#endif /* SMALL_CONFIG_PIN_H */
