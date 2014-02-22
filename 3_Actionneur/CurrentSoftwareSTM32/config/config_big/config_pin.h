/*
 *  Club Robot ESEO 2013 - 2014
 *
 *  $Id$
 *
 *  Package : Carte Actionneur
 *  Description : Configuration des pins
 *  Auteur : Jacen, Alexis
 */

#ifndef KRUSTY_CONFIG_PIN_H
#define KRUSTY_CONFIG_PIN_H

#define PORT_A_IO_MASK	0xFEFF
	#define BUTTON0_PORT		GPIOA->IDR0
	//	-				 			  1
	//	AX12/U2TX					  2
	//	AX12/U2RX					  3
	//	-				 			  4
	//	-				 			  5
	//	-				 			  6
	#define PRESENCE_FILET		GPIOA->IDR7
	//	-				 			  7
	#define FRUIT_POMPE_SENS		  GPIOA->ODR8
	//	-	usb			 			  9
	//	-	usb			 			  10
	//	-	usb			 			  11
	//	-	usb			 			  12
	//	-	programmation			  13
	//	-	programmation 			  14
	//	-				 			  15

#define PORT_B_IO_MASK	0x01FF
	#define FRUIT_VERIN_OUT		GPIOB->IDR0
	#define FRUIT_VERIN_IN		GPIOB->IDR1
	//	-				 			  2
	// - programmation -			  3
	//	-				 			  4
	//	-				 			  5
	//	U1TX						  6
	//	U1RX						  7
	//	-			 	  			  8
	#define LANCELAUNCHER_PIN_1 GPIOB->ODR9
	#define LANCELAUNCHER_PIN_2 GPIOB->ODR10
	#define AX12_DIRECTION_PORT GPIOB->ODR11
	#define LANCELAUNCHER_PIN_3 GPIOB->ODR12
	#define LANCELAUNCHER_PIN_4 GPIOB->ODR13
	#define LANCELAUNCHER_PIN_5 GPIOB->ODR14
	#define LANCELAUNCHER_PIN_6 GPIOB->ODR15

#define PORT_C_IO_MASK	0xFFFF
	//	-				 			  0
	//	-				 			  1
	//	-				 			  2
	//	-				 			  3
	//	-				 			  4
	//	-				 			  5
	//	-				 			  6
	//	-				 			  7
	//	-				 			  8
	//	-				 			  9
	//	-	sortie libre 			  10
	//	-				 			  11
	//	-				 			  12
	//	-				 			  13
	//	-	OSC32_in 			  	  14
	//	-	OSC32_out 			  	  15


#define PORT_D_IO_MASK	0x03FB
	//	CAN_RX						  0
	//	CAN_TX						  1
	#define GACHE_FILET			GPIOD->ODR2
	//	-				 			  3
	//	-							  4
	//	-	usb led red				  5
	//	-							  6
	//	-							  7
	//	U3TX						  8
	//	U3RX						  9
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
	//	-							  7
	//	-							  8
	//	-							  9
	//	-							  10
	//	-							  11
	#define BUTTON1_PORT		(!GPIOE->IDR12)	//Selftest
	#define BUTTON2_PORT		(!GPIOE->IDR13)	//Calibration
	#define BUTTON3_PORT		(!GPIOE->IDR14)	//LCD Menu +
	#define BUTTON4_PORT		(!GPIOE->IDR15)	//LCD Menu -



/* Config Fruit Pompe */
#define FRUIT_POMPE_PWM_NUM				3
#define FRUIT_POMPE_TOR_OPEN			FRUIT_VERIN_OUT  // Capteur
#define FRUIT_POMPE_TOR_CLOSE			FRUIT_VERIN_IN


/* Config proto Fruit_labium*/
	#define FRUIT_LABIUM_AX12_ID					  6

/* Config proto filet*/
	#define FILET_AX12_ID							  4

/* config watchdog lance lanceur*/
	#define LANCELAUNCHER_TIMER_USE_WATCHDOG

/* Config proto petit bras*/
	#define SMALL_ARM_AX12_ID						  3

#endif /* KRUSTY_CONFIG_PIN_H */
