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

#define PORT_A_IO_MASK	0xFFFF
	#define BUTTON0_PORT		GPIOA->IDR0
	//	-				 			  1
	//	AX12/U2TX					  2
	//	AX12/U2RX					  3
	//	-				 			  4
	//	-				 			  5
	//	-				 			  6
	//	-				 			  7
	//	-	sortie libre 			  8
	//	-	usb			 			  9
	//	-	usb			 			  10
	//	-	usb			 			  11
	//	-	usb			 			  12
	//	-	programmation			  13
	//	-	programmation 			  14
	//	-				 			  15
	
#define PORT_B_IO_MASK	0x01FF
	//	-							  0
	//	-							  1
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


#define PORT_D_IO_MASK	0x01FF
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


/* Config BALLLAUNCHER */
	#define BALLLAUNCHER_DCMOTOR_ID              0			//Doit être unique !
	#define BALLLAUNCHER_DCMOTOR_PWM_NUM         4
	#define BALLLAUNCHER_DCMOTOR_PORT_WAY        GPIOC->ODR
	#define BALLLAUNCHER_DCMOTOR_PORT_WAY_BIT    12
//	#define BALLLAUNCHER_HALLSENSOR_INT_FLAG     IFS1bits.INT1IF
//	#define BALLLAUNCHER_HALLSENSOR_INT_PRIORITY IPC4bits.INT1IP
//	#define BALLLAUNCHER_HALLSENSOR_INT_ENABLE   IEC1bits.INT1IE
//	#define BALLLAUNCHER_HALLSENSOR_INT_EDGE     INTCON2bits.INT1EP
//	#define BALLLAUNCHER_HALLSENSOR_INT_ISR      __attribute__((interrupt, no_auto_psv)) _INT1Interrupt

/* Config PLATE */
	#define PLATE_DCMOTOR_ID                     1      //Doit être unique !
	#define PLATE_DCMOTOR_PWM_NUM                3
	#define PLATE_DCMOTOR_PORT_WAY               GPIOC->ODR
	#define PLATE_DCMOTOR_PORT_WAY_BIT           11
	#define PLATE_ROTATION_POTAR_ADC_ID          AN9_ID
	#define PLATE_PLIER_AX12_ID                  3

/* Config BALLSORTER */
	#define BALLSORTER_AX12_ID                   5
	#define BALLSORTER_SENSOR_PIN_WHITE_CHERRY   GPIOB->ODR12  //Si utilisation du gros sick (capteur couleur, voir BALLSORTER_USE_SICK_SENSOR dans KBall_sorter_config.h)
	#define BALLSORTER_SENSOR_PIN_RED_CHERRY     GPIOB->ODR13
	#define BALLSORTER_SENSOR_PIN_BLUE_CHERRY    GPIOB->ODR14
	#define BALLSORTER_SENSOR_PIN                GPIOB->ODR12  //Si utilisation du sick wt100
	#define BALLSORTER_SENSOR_DETECTED_LEVEL     0      //Niveau de BALLSORTER_SENSOR_PIN quand une cerise blanche est détectée

/* Config LIFT */
	#define LIFT_LEFT_DCMOTOR_ID                     2    //Doit être unique !
	#define LIFT_LEFT_DCMOTOR_PWM_NUM                1
	#define LIFT_LEFT_DCMOTOR_PORT_WAY               GPIOA->ODR
	#define LIFT_LEFT_DCMOTOR_PORT_WAY_BIT           8
	#define LIFT_LEFT_TRANSLATION_POTAR_ADC_ID       AN3_ID
	#define LIFT_LEFT_PLIER_AX12_ID                  6


	#define LIFT_RIGHT_DCMOTOR_ID                     3      //Doit être unique !
	#define LIFT_RIGHT_DCMOTOR_PWM_NUM                2
	#define LIFT_RIGHT_DCMOTOR_PORT_WAY               GPIOC->ODR
	#define LIFT_RIGHT_DCMOTOR_PORT_WAY_BIT           10
	#define LIFT_RIGHT_TRANSLATION_POTAR_ADC_ID       AN2_ID
	#define LIFT_RIGHT_PLIER_AX12_ID                  1

/* Config proto Fruit_mouth*/
	#define FRUIT_MOUTH_AX12_ID						  4

/* config watchdog lance lanceur*/
	#define LANCELAUNCHER_TIMER_USE_WATCHDOG

#endif /* KRUSTY_CONFIG_PIN_H */
