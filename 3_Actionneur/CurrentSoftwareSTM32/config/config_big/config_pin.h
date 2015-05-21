/*
 *  Club Robot ESEO 2013 - 2014
 *
 *  $Id$
 *
 *  Package : Carte Actionneur
 *  Description : Configuration des pins
 *  Auteur : Jacen, Alexis
 */

#ifndef BIG_CONFIG_PIN_H
#define BIG_CONFIG_PIN_H

#include "../config_use.h"

#define PORT_A_IO_MASK	0xFEDF
	#define BUTTON0_PORT		GPIOA->IDR0
	//	-				 			  1
	//	AX12/U2TX					  2
	//	AX12/U2RX					  3
	//	-				 			  4
	#define AX12_DIRECTION_PORT_RX24 GPIOA,GPIO_Pin_5
	//	-				 			  6
	//	-				 			  7
	#define CUP_NIPPER_DCM_SENS		 GPIOA,GPIO_Pin_8
	//	-	usb			 			  9
	//	-	usb			 			  10
	//	-	usb			 			  11
	//	-	usb			 			  12
	//	-	programmation			  13
	//	-	programmation 			  14
	//	-				 			  15

#define PORT_B_IO_MASK	0xF7FF
	#define ELEVATOR_FDC			 !(GPIOB->IDR0)
	//	-				 			  1
	//	-				 			  2
	// - programmation -			  3
	//	-				 			  4
	//	-				 			  5
	//	U1TX						  6
	//	U1RX						  7
	#define I_AM_READY				 GPIOB,GPIO_Pin_8
	//	-			 	  			  9
	//	-			 	  			  10
	#define AX12_DIRECTION_PORT_AX12 GPIOB,GPIO_Pin_11
	#define CLAP_FDP_LEFT_CONF			 GPIOB,GPIO_Pin_12
	#define CLAP_FDP_RIGHT_CONF			 GPIOB,GPIO_Pin_13
	#define CLAP_FDP_LEFT			 (GPIOB->IDR12)
	#define CLAP_FDP_RIGHT			 (GPIOB->IDR13)
	//	-			 	  			  14
	//	-			 	  			  15

#define PORT_C_IO_MASK	0xE3FF
	//  -							  0
	#define CUP_NIPPER_FDC			 !(GPIOC->IDR1)
	//	-				 			  2
	#define CLAP_SENSOR_R			(GPIOC->IDR3)
	#define CLAP_SENSOR_L			(GPIOC->IDR4)
	//	-				 			  5
	//	-				 			  6
	//  -							  7
	//	-				 			  8
	//	-				 			  9
	#define ELEVATOR_DCM_SENS		GPIOC,GPIO_Pin_10
	#define CLAP_GOLLY_DCM_SENS		GPIOC,GPIO_Pin_11
	#define SPOT_POMPE_RIGHT_SENS	GPIOC,GPIO_Pin_12
	//	-				 			  13
	//	-	OSC32_in 			  	  14
	//	-	OSC32_out 			  	  15


#define PORT_D_IO_MASK	0x03EF
	//	CAN_RX						  0
	//	CAN_TX						  1
	// -							  2
	//	-				 			  3
	//	-							  4
	//	-	usb led red				  5
	//	-							  6
	//	-							  7
	//	U3TX						  8
	//	U3RX						  9
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


	#define PORT_E_IO_MASK	0x7FFF
	//	-				 			  0
	//	-				 			  1
	// - programmation -			  2
	// - programmation -			  3
	// - programmation -			  4

	//	-							  7
	//	-							  8
	//	-							  9
	//	-							  10
	//	-							  11
	//	-							  12
	//	-							  13
	//	-							  14
	#define SPOT_POMPE_LEFT_SENS	GPIOE,GPIO_Pin_15


//--------------------Configuration des ID des actionneurs-----------------------

#define POP_COLLECT_LEFT_AX12_ID		2
#define POP_COLLECT_RIGHT_AX12_ID		3

#define POP_DROP_LEFT_AX12_ID			4
#define POP_DROP_RIGHT_AX12_ID			5

#define BACK_SPOT_LEFT_AX12_ID			6
#define BACK_SPOT_RIGHT_AX12_ID			7

#define CARPET_LAUNCHER_LEFT_AX12_ID	8
#define CARPET_LAUNCHER_RIGHT_AX12_ID	9

#define CUP_NIPPER_AX12_ID				10

#define PINCEMIR_R_RX24_ID				26
#define PINCEMIR_L_RX24_ID				25
#define PINCEMIL_R_RX24_ID				23
#define PINCEMIL_L_RX24_ID				24

#define STOCKR_R_F1_RX24_ID				29
#define STOCKR_L_F1_RX24_ID				30
#define STOCKL_R_F1_RX24_ID				27
#define STOCKL_L_F1_RX24_ID				28

#define STOCKR_R_F2_RX24_ID				33
#define STOCKR_L_F2_RX24_ID				34
#define STOCKL_R_F2_RX24_ID				31
#define STOCKL_L_F2_RX24_ID				32

#define SPOT_POMPE_RIGHT_PWM_NUM		4
//#define SPOT_POMPE_LEFT_PWM_NUM		CUSTOM ! Laisser les en commentaires

#define ELEVATOR_ID						0
#define CUP_NIPPER_ID					1
#define CLAP_HOLLY_ID					2
#define CLAP_HOLLY_ADC_SENSOR			ADC_13

#endif /* BIG_CONFIG_PIN_H */
