/*
 *  Club Robot ESEO 2008 - 2010
 *  Archi'Tech, PACMAN
 *
 *  Fichier : Global_config.h
 *  Package : Carte Principale
 *  Description : Configuration des modules de QS
 *  Auteur : Jacen
 *  Version 20110227
 */

#ifndef GLOBAL_CONFIG_H
	#define GLOBAL_CONFIG_H
	
	#ifndef __dsPIC30F__
		#define STM32F40XX
	#endif

	#define VERBOSE_MODE
//	#define MODE_SIMULATION

	#define OUTPUT_LOG  //utilis� dans act_function et queue
	#define OUTPUT_LOG_DEFAULT_MAX_LOG_LEVEL LOG_LEVEL_Info    //Pour connaitre les valeurs possibles, voir output_log.h (enum log_level_e)
//	#define OUTPUT_LOG_PRINT_ALL_COMPONENTS  //Si d�fini, affiche les messages de tous les composants (OUTPUT_LOG_COMPONENT_* ne sont pas pris en compte, les niveau de d�buggage le sont par contre)
	#define OUTPUT_LOG_COMPONENT_ACTFUNCTION         LOG_PRINT_On  //LOG_PRINT_On: activ�, LOG_PRINT_Off: d�sactiv�
	#define OUTPUT_LOG_COMPONENT_QUEUE               LOG_PRINT_Off
	#define OUTPUT_LOG_COMPONENT_STRAT_STATE_CHANGES LOG_PRINT_On   //Changement d'�tat de la strat

	/* Pour certaines config particulieres, il faut definir qui on est
	 * a l'aide d'une des valeurs du type cartes_e de QS_types.h */
	#define I_AM CARTE_STRAT
	#define I_AM_CARTE_STRAT

	//#define DISABLE_WHO_AM_I	//D�sactive la d�tection du robot.


		/* Les instructions ci dessous d�finissent le comportement des
	 * entrees sorties du uc. une configuration en entree correspond
	 * a un bit a 1 (Input) dans le masque, une sortie a un bit a
	 * 0 (Output).
	 * Toute connection non utilisee doit etre configuree en entree
	 * (risque de griller ou de faire bruler le uc)
	 */

		/* Il faut choisir � quelle frequence on fait tourner le STM32 */
		#define HCLK_FREQUENCY_HZ     160000000	//40Mhz, Max: 168Mhz
		#define PCLK1_FREQUENCY_HZ    40000000	//10Mhz, Max: 42Mhz
		#define PCLK2_FREQUENCY_HZ    80000000	//40Mhz, Max: 84Mhz
		#define CPU_EXTERNAL_CLOCK_HZ 8000000	//8Mhz, Fr�quence de l'horloge externe
	
		#define PORT_A_IO_MASK	0xFC9B
			#define BUTTON0_PORT		GPIOA->IDR0
			#define SWITCH_DEBUG		GPIOA->IDR1
				//	XBEE/BT/U2TX				  2
				//	XBEE/BT/U2RX				  3
				//	-				 			  4
			#define XBEE_RESET			GPIOA->ODR5
				//	-				 			  6
			#define SWITCH_VERBOSE		GPIOA->IDR7
				//	-	sortie libre 			  8
				//	-	usb			 			  9
				//	-	usb			 			  10
				//	-	usb			 			  11
				//	-	usb			 			  12
				//	-	programmation			  13
				//	-	programmation 			  14
			#define BUTTON5_PORT		(!GPIOA->IDR15)
		#define PORT_B_IO_MASK	0x00BF
				//	Capteur						  0
				//	Capteur						  1
			#define POWER_WATCH_INT		GPIOB->IDR2
				// - programmation -			  3
			#define SWITCH_XBEE			GPIOB->IDR4
			#define SWITCH_SAVE			GPIOB->IDR5
				//	U1TX						  6
				//	U1RX						  7
				//	-			 	  			  8
			#define LCD_RESET_PORT		GPIOB->ODR9
				//	I2C RTC/LCD	 			  	  10
				//	I2C RTC/LCD	 			  	  11
			#define EEPROM_CS			GPIOB->ODR12
			#define EEPROM_SCK			GPIOB->ODR13
			#define EEPROM_SDO			GPIOB->ODR14
			#define EEPROM_SDI			GPIOB->ODR15


		#define PORT_C_IO_MASK	0x200F
				//	Capteur			 			  0
				//	Capteur			 			  1
				//	Capteur			 			  2
				//	Capteur			 			  3
				//	Capteur			 			  4
				//	Capteur			 			  5
			#define LED_BEACON_IR_GREEN	GPIOC->ODR6
			#define LED_BEACON_IR_RED	GPIOC->ODR7
			#define LED_BEACON_US_GREEN	GPIOC->ODR8
			#define LED_BEACON_US_RED	GPIOC->ODR9
				//	-	sortie libre 			  10
			#define EEPROM_WP			GPIOC->ODR11
			#define SD_CS				GPIOC->ODR11
			#define EEPROM_HOLD			GPIOC->ODR12
			#define PORT_ROBOT_ID  		GPIOC->IDR13
				//	-	OSC32_in 			  	  14
				//	-	OSC32_out 			  	  15


		#define PORT_D_IO_MASK	0x02C3
				//	CAN_RX						  0
				//	CAN_TX						  1
			#define BLUE_LEDS			GPIOD->ODR2
			#define RED_LEDS			GPIOD->ODR3
				//	-							  4
				//	-	usb led red				  5
			#define SWITCH_COLOR		GPIOD->IDR6
			#define BIROUTE				GPIOD->IDR7
				//	HOKUYO U3TX					  8
				//	HOKUYO U3RX					  9
			#define LED_ERROR 			GPIOD->ODR10
			#define LED_SELFTEST 		GPIOD->ODR11
			#define LED_RUN  			GPIOD->ODR12
			#define LED_CAN  			GPIOD->ODR13
			#define LED_UART 			GPIOD->ODR14
			#define LED_USER 			GPIOD->ODR15


		#define PORT_E_IO_MASK	0xFFFF
				//	-				 			  0
				//	-				 			  1
				// - programmation -			  2
				// - programmation -			  3
				// - programmation -			  4
				// - programmation -			  5
				// - programmation -			  6
			#define SWITCH_LCD			GPIOE->IDR7
			#define SWITCH_EVIT			GPIOE->IDR8
			#define SWITCH_STRAT_1		GPIOE->IDR9
			#define SWITCH_STRAT_2		GPIOE->IDR10
			#define SWITCH_STRAT_3		GPIOE->IDR11
			#define BUTTON1_PORT		(!GPIOE->IDR12)	//Selftest
			#define BUTTON2_PORT		(!GPIOE->IDR13)	//Calibration
			#define BUTTON3_PORT		(!GPIOE->IDR14)	//LCD Menu +
			#define BUTTON4_PORT		(!GPIOE->IDR15)	//LCD Menu -

		//Nulle part...
			#define SWITCH_STRAT_4		FALSE	//Temporaire
			#define SWITCH_STRAT_5		FALSE	//Temporaire


	//#define USE_LCD
	#ifdef USE_RTC
		#define USE_I2C2
	#endif

	#define USE_RTC
	#ifdef USE_RTC
		#define USE_I2C2
	#endif

	#define USE_SPI2
	#define SPI_R_BUF_SIZE 16



	#define SICK_SENSOR_FOE_GLASSES_PIN FALSE//PORTBbits.RB15   //sur Tiny
	#define GLASS_SENSOR_LEFT			FALSE//(!PORTBbits.RB3)    //sur Krusty, en logique invers�e
	#define GLASS_SENSOR_RIGHT			FALSE//(!PORTBbits.RB5)    //sur Krusty, en logique invers�e

	#define USE_WATCHDOG
	#define WATCHDOG_TIMER 3
	#define WATCHDOG_MAX_COUNT 5
	#define WATCHDOG_QUANTUM 1
	

		
	#define PORT_SWITCH_XBEE	GPIOB->IDR4
	//#define PORT_SWITCH_XBEE	FALSE	//Utiliser cette ligne pour d�sactiver le XBee...
	#define USE_XBEE
	#define XBEE_PLUGGED_ON_UART2
	#define RESET_XBEE	GPIOA->ODR5


	#define USE_CAN
/*	Nombre de messages CAN conserv�s
	pour traitement hors interuption */
	#define CAN_BUF_SIZE		32
	#define CAN_SEND_TIMEOUT_ENABLE

	#define UART1_BAUDRATE	115200
	#define USE_UART1
	#define USE_UART1RXINTERRUPT
	
	#define UART2_BAUDRATE	9600
	#define USE_UART2
	#define USE_UART2RXINTERRUPT
	
/*	Taille de la chaine de caracteres memorisant
	les caracteres recus sur UART */
	#define UART_RX_BUF_SIZE	32
	
/* Utilisation des entr�es analogiques */
	#define USE_ANALOG_EXT_VREF

	#define USE_AN8


	//utilisation du module BUTTONS
	#define USE_BUTTONS
	//utilise le timer 2 pour les boutons
	//#define BUTTONS_TIMER 3
	#define BUTTONS_TIMER_USE_WATCHDOG

	#define ADC_PIN_DT10_GLASSES	0
	#define USE_AN9 			// DT10 glasses sur TINY
		


//#define FREQ_PWM_50HZ
	#define FREQ_PWM_50KHZ
	/*
	 *	La section config carte Strat�gie
	 */

	//Activation du module d'enregistrement des messages CAN re�us pendant les matchs en m�moire EEPROM.
	//#define EEPROM_CAN_MSG_ENABLE
	#define SD_ENABLE
	#ifdef SD_ENABLE
		extern int SD_printf(char * s, ...);	//Enregistre � la suite du match une chaine de caract�re... Utilisable par exemple pour les changements d'�tats.
		//Permet � tout le monde d'acc�der au SD_printf() pour les logs...
	#endif

	#define STACKS_SIZE 32 //doit �tre < � 256

	//Utilis� par act_function
	#define QUEUE_SIZE	16

	//utilisation du module BUTTONS
	#define USE_BUTTONS
	//utilise le timer 2 pour les boutons
	#define BUTTONS_TIMER 2
		
	//demande � l'asser d'envoyer regulierement la position du robot
	//utilis� en 2010-2011 pour la d�tection des pions avec les capteurs de distance SICK (cf. brain.c)
	#define USE_SCHEDULED_POSITION_REQUEST
	
	//#define USE_TELEMETER
	#ifdef USE_TELEMETER
		//utilisation des t�l�m�tres laser SICK
		//#define USE_SICK_DT50_LEFT
		//#define USE_SICK_DT50_RIGHT
		//#define USE_SICK_DT50_TOP
		#define USE_SICK_DT10_0
		#define USE_SICK_DT10_1
		#define USE_SICK_DT10_2
		#define USE_SICK_DT10_3
	#endif
	
	//Module de gestion des capteurs SICK pour scan de la zone de jeu... Ce module est tr�s d�pendant du r�glement et du robot !
	//#define USE_SICK
	
	//utilisation du lecteur de codes-barres
//	#define USE_BARCODE_ON_UART1
	//#define USE_BARCODE_ON_UART2
	
	//envoi des messages de debug de timeout et activit� des piles
//	#define USE_SEND_CAN_DEBUG

	//envoie la position de l'adversaire sur le bus � chaque update (max 1 fois par seconde (a l'ecriture de ce commentaire))
//	#define USE_FOE_POS_CAN_DEBUG
	
	//envoie de l'ajout, la mise a jour ou la suppresion d'un element de jeu sur le CAN
//	#define USE_ELEMENT_CAN_DEBUG


	//utilise le mode de d�placement avec les polygones
	//si cette ligne est comment�e, on utilise par d�faut le mode de d�placement avec pathfind
	//#define USE_POLYGON
	
	//utilise le buffer de l'asser pour les trajectoires multi-points et permet d'utiliser les courbes
	#define USE_ASSER_MULTI_POINT
	
	//les ASSER_TRAJECTOIRE_FINIE d�pilent les asser goto meme si on est loin de la destination
	#define ASSER_PULL_EVEN_WHEN_FAR_FROM_DESTINATION
	
	//active les debug_printf pour les sections concern�es
//	#define DEBUG_ASSER_FUN
//	#define DEBUG_ACT_FUN
//	#define DEBUG_SICK
//	#define DEBUG_ELEMENTS
//	#define DEBUG_BARCODE
//	#define DEBUG_DETECTION
//	#define DEBUG_AVOIDANCE
//	#define DEBUG_POLYGON
//	#define DEBUG_TELEMETER

	//d�finition de la pr�cision et des modes de calcul des sinus et cosinus
//	#define FAST_COS_SIN
//	#define COS_SIN_16384




/*
CAN_send_debug("");
0 timeout asser goto
1 timeout asser goangle
2 timeout asser rush
3 timeout asser relative go angle
A pull
B flush
C asser far from destination

[a-z] temporaires
*/

#endif /* ndef GLOBAL_CONFIG_H */
