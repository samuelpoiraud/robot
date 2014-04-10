/*
 *  Club Robot ESEO 2013 - 2014
 *
 *  $Id$
 *
 *  Package : Carte Actionneur
 *  Description : Configuration des pins
 *  Auteur : Jacen, Alexis
 */

#ifndef PIERRE_CONFIG_PIN_H
#define PIERRE_CONFIG_PIN_H

#define PORT_A_IO_MASK	0xFEDF
	#define BUTTON0_PORT		GPIOA->IDR0
	//	-				 			  1
	//	AX12/U2TX					  2
	//	AX12/U2RX					  3
	//	-				 			  4
	#define AX12_DIRECTION_PORT_RX24 GPIOA->ODR5
	//	-				 			  6
	#define PRESENCE_FILET		GPIOA->IDR7
	//	-				 			  7
	#define POMPE_SENS			GPIOA->ODR8
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
	#define AX12_DIRECTION_PORT_AX12 GPIOB->ODR11
	#define LANCELAUNCHER_PIN_3 GPIOB->ODR12
	#define LANCELAUNCHER_PIN_4 GPIOB->ODR13
	#define LANCELAUNCHER_PIN_5 GPIOB->ODR14
	#define LANCELAUNCHER_PIN_6 GPIOB->ODR15

#define PORT_C_IO_MASK	0xF7FF
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
	#define FRUIT_POMPE_SENS	GPIOC->ODR11
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
	#define FRUIT_LABIUM_AX12_ID					  9

/* Config proto filet*/
	#define FILET_AX12_ID							  4
	#define GACHE_AX12_ID							  1

/* config watchdog lance lanceur*/
	#define LANCELAUNCHER_TIMER_USE_WATCHDOG

/* Config proto petit bras*/
	#define SMALL_ARM_AX12_ID						  0

/* Config proto pompe*/
	#define POMPE_PWM_NUM							  1

/* Config torch locker*/
	#define TORCH_LOCKER_AX12_1_ID					 7
	#define TORCH_LOCKER_AX12_2_ID					 11

	// ID des moteurs du bras
	#define ARM_ACT_UPDOWN_ID                        0
	#define ARM_ACT_RX24_ID                          21
	#define ARM_ACT_AX12_MID_ID                      8
	#define ARM_ACT_AX12_TRIANGLE_ID                 3

#endif /* PIERRE_CONFIG_PIN_H */
