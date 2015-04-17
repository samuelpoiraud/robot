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

#define PORT_A_IO_MASK	0xFFDF
	#define BUTTON0_PORT		GPIOA->IDR0
	//	-				 			  1
	//	AX12/U2TX					  2
	//	AX12/U2RX					  3
	//	-				 			  4
	#define AX12_DIRECTION_PORT_RX24 GPIOA,GPIO_Pin_5
	//	-				 			  6
	//	-				 			  7
	//	-				 			  8
	//	-	usb			 			  9
	//	-	usb			 			  10
	//	-	usb			 			  11
	//	-	usb			 			  12
	//	-	programmation			  13
	//	-	programmation 			  14
	//	-				 			  15

#define PORT_B_IO_MASK	0xF7FF
	#define WT100_GOBELET_RIGHT		(GPIOB->IDR0)
	//	-							  1
	//	-				 			  2
	// - programmation -			  3
	//	-				 			  4
	//	-				 			  5
	//	U1TX						  6
	//	U1RX						  7
	#define I_AM_READY				GPIOB->IDR8
	//	-				 			  9
	//	-	 					  	  10
	#define AX12_DIRECTION_PORT_AX12 GPIOB,GPIO_Pin_11
	//	-				 			  12
	//	-				 			  13
	//	-				 			  14
	//	-				 			  15

#define PORT_C_IO_MASK	0xFFFF
	//	-				 			  0
	//	-				 			  1
	#define WT100_GOBELET_LEFT		(GPIOC->IDR2)
	#define WT100_GOBELET_FRONT		(GPIOC->IDR2)
	//	-				 			  4
	//	-				 			  5
	//	-				 			  6
	//	-				 			  7
	//	-				 			  8
	//	-				 			  9
	//	-				 			  10
	//	-				 			  11
	//	-				 			  12
	//	-				 			  13
	//	-	OSC32_in 			  	  14
	//	-	OSC32_out 			  	  15


#define PORT_D_IO_MASK	0x03FF
	//	CAN_RX						  0
	//	CAN_TX						  1
	//	-				 			  2
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


#define PORT_E_IO_MASK	0xFFFF
	//	-				 			  0
	//	-				 			  1
	// - programmation -			  2
	// - programmation -			  3
	// - programmation -			  4
	// - programmation -			  5
	// - programmation -			  6
	//	-							  7
	//	-							  8
	//	-							  9
	//	-							  10
	//	-							  11
#ifdef FDP_2014
	#define BUTTON1_PORT		(!GPIOE->IDR12)
	#define BUTTON2_PORT		(!GPIOE->IDR13)
	#define BUTTON3_PORT		(!GPIOE->IDR14)
	#define BUTTON4_PORT		(!GPIOE->IDR15)
#else

#endif

//--------------------Configuration des ID des actionneurs-----------------------

/* ID des différents AX12 */
	#define PINCE_GAUCHE_AX12_ID						11
	#define PINCE_DROITE_AX12_ID						12
	#define CLAP_AX12_ID						        11
	#define POP_DROP_LEFT_WOOD_AX12_ID                  13
	#define POP_DROP_RIGHT_WOOD_AX12_ID                 14

#endif /* SMALL_CONFIG_PIN_H */
