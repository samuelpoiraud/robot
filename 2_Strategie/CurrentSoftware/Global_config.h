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

	#define OUTPUT_LOG  //utilisé dans act_function et queue
	#define OUTPUT_LOG_DEFAULT_MAX_LOG_LEVEL LOG_LEVEL_Info    //Pour connaitre les valeurs possibles, voir output_log.h (enum log_level_e)
//	#define OUTPUT_LOG_PRINT_ALL_COMPONENTS  //Si défini, affiche les messages de tous les composants (OUTPUT_LOG_COMPONENT_* ne sont pas pris en compte, les niveau de débuggage le sont par contre)
	#define OUTPUT_LOG_COMPONENT_ACTFUNCTION         LOG_PRINT_On  //LOG_PRINT_On: activé, LOG_PRINT_Off: désactivé
	#define OUTPUT_LOG_COMPONENT_QUEUE               LOG_PRINT_Off
	#define OUTPUT_LOG_COMPONENT_STRAT_STATE_CHANGES LOG_PRINT_On   //Changement d'état de la strat

	/* Pour certaines config particulieres, il faut definir qui on est
	 * a l'aide d'une des valeurs du type cartes_e de QS_types.h */
	#define I_AM CARTE_STRAT
	#define I_AM_CARTE_STRAT

	#define FDP_2013
	#define DISABLE_WHO_AM_I	//Désactive la détection du robot.

	#ifdef __dsPIC30F__

		#define FREQ_10MHZ
		#define PORT_A_IO_MASK	0xFFFF
		#define PORT_B_IO_MASK	0xFFFF
		#define PORT_C_IO_MASK	0xFFFF
		#define PORT_D_IO_MASK	0xC0FF
		#define PORT_E_IO_MASK	0xFFFF
		#define PORT_F_IO_MASK	0xFFFF
		#define PORT_G_IO_MASK	0xFF3F
		#define SWITCH_STRAT_1	PORTEbits.RE0
		#define SWITCH_STRAT_2	PORTEbits.RE1
		#define SWITCH_STRAT_3	PORTEbits.RE2
		#define SWITCH_LAST_POS	PORTEbits.RE3
		#define SWITCH_COLOR	PORTEbits.RE4
		#define SWITCH_STRAT_4	PORTEbits.RE6
		#define LAT_ROBOT_ID_OUTPUT		LATEbits.LATE7
		#define TRIS_ROBOT_ID_OUTPUT	TRISEbits.TRISE7
		#define PORT_ROBOT_ID_INPUT		PORTEbits.RE5
		#define TRIS_ROBOT_ID_INPUT		TRISEbits.TRISE5
		#define SWITCH_RG0	PORTGbits.RG0
		#define SWITCH_RG1	PORTGbits.RG1
		#define BLUE_LEDS	LATGbits.LATG7
		#define RED_LEDS	LATGbits.LATG6
		/*#else
			#define BLUE_LEDS	LATGbits.LATG6
			#define RED_LEDS	LATGbits.LATG7
		#endif
		*/
		#define PIN_BIROUTE	PORTGbits.RG8

		#define SICK_SENSOR_FOE_GLASSES_PIN PORTBbits.RB15   //sur Tiny
		#define GLASS_SENSOR_LEFT			(!PORTBbits.RB3)    //sur Krusty, en logique inversée
		#define GLASS_SENSOR_RIGHT			(!PORTBbits.RB5)    //sur Krusty, en logique inversée
		
	#else // STM32F4XX
		/* Les instructions ci dessous définissent le comportement des
	 * entrees sorties du pic. une configuration en entree correspond
	 * a un bit a 1 (Input) dans le masque, une sortie a un bit a
	 * 0 (Output).
	 * Toute connection non utilisee doit etre configuree en entree
	 * (risque de griller ou de faire bruler le pic)
	 */
		#define PORT_A_IO_MASK	0xFFFF
		#define PORT_B_IO_MASK	0xFFFF
		#define PORT_C_IO_MASK	0xFFBF	//C9: MO2 debug clock
		#define PORT_D_IO_MASK	0x0FFF	//LEDs
		#define PORT_E_IO_MASK	0xFFFF
		/* Il faut choisir à quelle frequence on fait tourner le STM32 */
		#define HCLK_FREQUENCY_HZ     40000000	//40Mhz, Max: 168Mhz
		#define PCLK1_FREQUENCY_HZ    10000000	//10Mhz, Max: 42Mhz
		#define PCLK2_FREQUENCY_HZ    20000000	//40Mhz, Max: 84Mhz
		#define CPU_EXTERNAL_CLOCK_HZ 8000000	//10Mhz, Fréquence de l'horloge externe
	
		#define TRIS_ROBOT_ID_OUTPUT GPIOB->MODER11
		#define LAT_ROBOT_ID_OUTPUT  GPIOB->ODR11
		#define TRIS_ROBOT_ID_INPUT  GPIOB->MODER12
		#define PORT_ROBOT_ID_INPUT  GPIOB->IDR12
		
		#define SWITCH_STRAT_1	GPIOB->IDR1
		#define SWITCH_STRAT_2	GPIOB->IDR2
		#define SWITCH_STRAT_3	GPIOB->IDR3
		#define SWITCH_LAST_POS	GPIOB->IDR4
		#define SWITCH_COLOR	GPIOB->IDR5
		#define SWITCH_STRAT_4	GPIOB->IDR6

		#define SWITCH_RG0	GPIOB->IDR7
		#define SWITCH_RG1	GPIOB->IDR8
		#define TRIS_ROBOT_ID_OUTPUT GPIOB->MODER11
		#define LAT_ROBOT_ID_OUTPUT  GPIOB->ODR11
		#define TRIS_ROBOT_ID_INPUT  GPIOB->MODER12
		#define PORT_ROBOT_ID_INPUT  GPIOB->IDR12
		#define BLUE_LEDS	GPIOB->ODR9
		#define RED_LEDS	GPIOB->ODR10
		#define PIN_BIROUTE	GPIOA->IDR0
		
		#define SICK_SENSOR_FOE_GLASSES_PIN FALSE//PORTBbits.RB15   //sur Tiny
		#define GLASS_SENSOR_LEFT			FALSE//(!PORTBbits.RB3)    //sur Krusty, en logique inversée
		#define GLASS_SENSOR_RIGHT			FALSE//(!PORTBbits.RB5)    //sur Krusty, en logique inversée

		#define USE_WATCHDOG
		#define WATCHDOG_TIMER 3
		#define WATCHDOG_MAX_COUNT 5
		#define WATCHDOG_QUANTUM 1
	
	#endif
		

	
	
	
	
	/* Les instructions suivantes permettent de configurer certaines
	 * entrees/sorties du pic pour realiser des fonctionnalites
	 * particulieres comme une entree analogique
	 */

	#define USE_CAN
/*	Nombre de messages CAN conservés
	pour traitement hors interuption */
	#define CAN_BUF_SIZE		32
	#define CAN_SEND_TIMEOUT_ENABLE

	#define UART_BAUDRATE	115200//9600
	#define USE_UART1
	#define USE_UART1RXINTERRUPT
	
	#define USE_UART2
	#define USE_UART2RXINTERRUPT
	
/*	Taille de la chaine de caracteres memorisant
	les caracteres recus sur UART */
	#define UART_RX_BUF_SIZE	12
	
/* Utilisation des entrées analogiques */
	#define USE_ANALOG_EXT_VREF

	#define USE_AN8


	//utilisation du module BUTTONS
	#define USE_BUTTONS
	//utilise le timer 2 pour les boutons
	//#define BUTTONS_TIMER 3
	#define BUTTONS_TIMER_USE_WATCHDOG

	#define ADC_PIN_DT10_GLASSES	0
	#define USE_AN15 			// DT10 glasses sur TINY
		
	//Test debug QS_STM32
	//On active tout
//	#define USE_WATCHDOG
//	#define WATCHDOG_TIMER 3
//	#define WATCHDOG_MAX_COUNT 5
//	#define WATCHDOG_QUANTUM 1

//	#define USE_DCMOTOR2
//	#define DCM_NUMBER 3
//	#define DCM_TIMER_PERIOD 10
//	#define DCMOTOR_NB_POS 3
//	#define DCM_TIMER 4

//
//	#define USE_DCMOTOR2
//	#define DCM_NUMBER 3
//	#define DCMOTOR_NB_POS 5
//	#define DCM_TIMER 3
//	#define DCM_TIMER_PERIOD 10
//
//	#define USE_SERVO
//	#define SERVO_TIMER 4

//	#define USE_STEP_MOTOR
//	#define STEP_MOTOR_TIMER 3
//	#define STEP_MOTOR_MS_PER_STEP 5
//	#define STEP_MOTOR1 GPIOE->ODR0
//	#define STEP_MOTOR2 GPIOE->ODR1
//	#define STEP_MOTOR3 GPIOE->ODR2
//	#define STEP_MOTOR4 GPIOE->ODR3

//	#define USE_SPI
//	#define USE_SPI2

//#define FREQ_PWM_50HZ
	#define FREQ_PWM_50KHZ
	/*
	 *
	 *	La section config carte P
	 *
	 *
	 */

	#define STACKS_SIZE 32 //doit être < à 256

	//Utilisé par act_function
	#define QUEUE_SIZE	16

	//utilisation du module BUTTONS
	#define USE_BUTTONS
	//utilise le timer 2 pour les boutons
	#define BUTTONS_TIMER 2
		
	//demande à l'asser d'envoyer regulierement la position du robot
	//utilisé en 2010-2011 pour la détection des pions avec les capteurs de distance SICK (cf. brain.c)
	#define USE_SCHEDULED_POSITION_REQUEST
	
	//#define USE_TELEMETER
	#ifdef USE_TELEMETER
		//utilisation des télémètres laser SICK
		//#define USE_SICK_DT50_LEFT
		//#define USE_SICK_DT50_RIGHT
		//#define USE_SICK_DT50_TOP
		#define USE_SICK_DT10_0
		#define USE_SICK_DT10_1
		#define USE_SICK_DT10_2
		#define USE_SICK_DT10_3
	#endif
	
	//Module de gestion des capteurs SICK pour scan de la zone de jeu... Ce module est très dépendant du règlement et du robot !
	//#define USE_SICK
	
	//utilisation du lecteur de codes-barres
//	#define USE_BARCODE_ON_UART1
	//#define USE_BARCODE_ON_UART2
	
	//envoi des messages de debug de timeout et activité des piles
//	#define USE_SEND_CAN_DEBUG

	//envoie la position de l'adversaire sur le bus à chaque update (max 1 fois par seconde (a l'ecriture de ce commentaire))
//	#define USE_FOE_POS_CAN_DEBUG
	
	//envoie de l'ajout, la mise a jour ou la suppresion d'un element de jeu sur le CAN
//	#define USE_ELEMENT_CAN_DEBUG


	//utilise le mode de déplacement avec les polygones
	//si cette ligne est commentée, on utilise par défaut le mode de déplacement avec pathfind
	//#define USE_POLYGON
	
	//utilise le buffer de l'asser pour les trajectoires multi-points et permet d'utiliser les courbes
	#define USE_ASSER_MULTI_POINT
	
	//les ASSER_TRAJECTOIRE_FINIE dépilent les asser goto meme si on est loin de la destination
	#define ASSER_PULL_EVEN_WHEN_FAR_FROM_DESTINATION
	
	//active les debug_printf pour les sections concernées
//	#define DEBUG_ASSER_FUN
//	#define DEBUG_ACT_FUN
//	#define DEBUG_SICK
//	#define DEBUG_ELEMENTS
//	#define DEBUG_BARCODE
//	#define DEBUG_DETECTION
//	#define DEBUG_AVOIDANCE
//	#define DEBUG_POLYGON
//	#define DEBUG_TELEMETER

	//définition de la précision et des modes de calcul des sinus et cosinus
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
