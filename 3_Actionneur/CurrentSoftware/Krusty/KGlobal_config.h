/*  Club Robot ESEO 2012 - 2013
 *	Krusty
 *
 *  Fichier : Global_config.h
 *  Package : Actionneur
 *  Description : Configuration de la carte actionneur
 *  Auteur : Aurélien
 *  Version 20130208
 *  Robot : Krusty
 */

/************** Configuration Globale pour le robot KRUSTY ******************/

#if !defined(KRUSTY_GLOBAL_CONFIG_H) && defined(I_AM_ROBOT_KRUSTY)
	#define KRUSTY_GLOBAL_CONFIG_H

//#define AX12_DEBUG_PACKETS

	/* Les instructions ci dessous définissent le comportement des
	 * entrees sorties du pic. une configuration en entree correspond
	 * a un bit a 1 (Input) dans le masque, une sortie a un bit a
	 * 0 (Output).
	 * Toute connection non utilisee doit etre configuree en entree
	 * (risque de griller ou de faire bruler le pic) 
	 */

	#define PORT_A_IO_MASK	0xFFFF
	#define PORT_B_IO_MASK	0xFFFF
	#define PORT_C_IO_MASK	0xFFFF
	#define PORT_D_IO_MASK	0xC0FF  //C0xx: LED de la carte en sortie
	#define PORT_E_IO_MASK	0xFF00  //xx00: PWMx/sensx en sortie
		
	#define PORT_F_IO_MASK	0xFFFF
	#define PORT_G_IO_MASK	0xFEFF  //G8 en sortie: direction AX12

/* Definition des périphériques, connectiques capteurs et actionneurs de Krusty */

/* Entrées analogiques: attention à la renumérotation des entrées... */
	#define USE_ANALOG_EXT_VREF

	#define USE_AN2
	#define AN2_ID  0
	#define USE_AN3
	#define AN3_ID  1
	#define USE_AN9
	#define AN9_ID  2

/**************** Périphériques ****************/

/* QEI sur IT */
/*
	#define USE_QEI_ON_IT
	#define QEI_ON_IT_QA		1
	#define QEI_ON_IT_QB		2
*/

/* Réglages PWM */
	#define FREQ_PWM_50KHZ

/* Asservissement en position/vitesse de moteurs CC */
	/* déclarer l'utilisation du pilote */
	#define USE_DCMOTOR2
	/* timer utilisé pour l'asservissement (1, 2, ou 3) */
	#define DCM_TIMER			1
	/* définir le nombre d'actionneurs asservis */
	#define DCM_NUMBER			4
	/* Période d'asservisement (en ms) */
	#define DCM_TIMER_PERIOD	2  //Bas pour le lanceur de balle ...
	/* nombre maximum de positions à gérer par moteur */
	#define DCMOTOR_NB_POS		3

/* Servo-Moteurs AX12 */
	#define USE_AX12_SERVO
	#define AX12_NUMBER 7
	#define AX12_INSTRUCTION_BUFFER_SIZE 200
	//#define AX12_USE_WATCHDOG
	#define AX12_TIMER_ID 2
	#define AX12_DIRECTION_PORT GPIOB->ODR11
	#define AX12_STATUS_RETURN_MODE AX12_STATUS_RETURN_ALWAYS	//Permet de savoir quand l'AX12 n'est pas bien connecté ou ne répond pas.
	#define AX12_STATUS_RETURN_CHECK_CHECKSUM
	//#define AX12_DEBUG_PACKETS

/* Définition de la précision et des modes de calcul des sinus et cosinus (cf. maths_home.c/h) */
	//	#define FAST_COS_SIN
	//	#define COS_SIN_16384

/**************** Actionneurs ****************/

// Cette partie ne contient que les infos globales des actionneurs (comme les ports utilisés, les ID de moteur qui doivent être unique, ...)
// Le reste est dans le fichier *_config.h associé a l'actionneur (permet une compilation plus rapide en cas de changement)

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
	#define LIFT_LEFT_PLIER_AX12_ID                  4


	#define LIFT_RIGHT_DCMOTOR_ID                     3      //Doit être unique !
	#define LIFT_RIGHT_DCMOTOR_PWM_NUM                2
	#define LIFT_RIGHT_DCMOTOR_PORT_WAY               GPIOC->ODR
	#define LIFT_RIGHT_DCMOTOR_PORT_WAY_BIT           10
	#define LIFT_RIGHT_TRANSLATION_POTAR_ADC_ID       AN2_ID
	#define LIFT_RIGHT_PLIER_AX12_ID                  1

/**************** Capteurs ****************/


#endif /* ndef KRUSTY_GLOBAL_CONFIG_H && def I_AM_ROBOT_KRUSTY */
