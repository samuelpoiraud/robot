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
	#define PORT_E_IO_MASK	0xFF80  //xx80: PWMx/sensx en sortie
		
	#define PORT_F_IO_MASK	0xFFFF
	#define PORT_G_IO_MASK	0xFEFF  //G8 en sortie: direction AX12

/* Definition des périphériques, connectiques capteurs et actionneurs de Krusty */

/* Entrées analogiques: attention à la renumérotation des entrées... */
	#define USE_ANALOG_EXT_VREF

	#define USE_AN2
	#define AN2_ID  0
	#define USE_AN3
	#define AN3_ID  1
	#define USE_AN8
	#define AN8_ID  2
	#define USE_AN9
	#define AN9_ID  3
	#define USE_AN12
	#define AN12_ID 4
	#define USE_AN13
	#define AN13_ID 5

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
	#define DCM_NUMBER			5
	/* Période d'asservisement (en ms) */
	#define DCM_TIMER_PERIOD	2  //Elevé pour le lanceur de balle ...
	/* nombre maximum de positions à gérer par moteur */
	#define DCMOTOR_NB_POS		5

/* Servo-Moteurs AX12 */
	#define USE_AX12_SERVO
	#define AX12_NUMBER 7
	#define AX12_TIMER_ID 2
	#define AX12_DIRECTION_PORT LATGbits.LATG8
	#define AX12_STATUS_RETURN_MODE AX12_STATUS_RETURN_ALWAYS	//Permet de savoir quand l'AX12 n'est pas bien connecté ou ne répond pas.

/* Définition de la précision et des modes de calcul des sinus et cosinus (cf. maths_home.c/h) */
	//	#define FAST_COS_SIN
	//	#define COS_SIN_16384

/**************** Actionneurs ****************/

/* Config BALLLAUNCHER */
	#define BALLLAUNCHER_ASSER_KP                10
	#define BALLLAUNCHER_ASSER_KI                60
	#define BALLLAUNCHER_ASSER_KD                0
	#define BALLLAUNCHER_ASSER_TIMEOUT           0       //en ms
	#define BALLLAUNCHER_ASSER_POS_EPSILON       100
	#define BALLLAUNCHER_DEFAULT_TARGET_SPEED    6000		//en tr/min, non utilisé car on définie la vitesse avec l'argument du msg CAN ...
	#define BALLLAUNCHER_DCMOTOR_ID              0			//Doit être unique !
	#define BALLLAUNCHER_DCMOTOR_PWM_NUM         4
	#define BALLLAUNCHER_DCMOTOR_PORT_WAY        PORTE
	#define BALLLAUNCHER_DCMOTOR_PORT_WAY_BIT    6
	#define BALLLAUNCHER_DCMOTOR_MAX_PWM_WAY0    50
	#define BALLLAUNCHER_DCMOTOR_MAX_PWM_WAY1    0
	#define BALLLAUNCHER_EDGE_PER_ROTATION       1			//nombre d'inversion de champ magnétique par tour (2 aimants de sens opposé = 2 inversions / tour)
	#define BALLLAUNCHER_HALLSENSOR_INT_FLAG     IFS1bits.INT1IF
	#define BALLLAUNCHER_HALLSENSOR_INT_PRIORITY IPC4bits.INT1IP
	#define BALLLAUNCHER_HALLSENSOR_INT_ENABLE   IEC1bits.INT1IE
	#define BALLLAUNCHER_HALLSENSOR_INT_EDGE     INTCON2bits.INT1EP
	#define BALLLAUNCHER_HALLSENSOR_INT_ISR      __attribute__((interrupt, no_auto_psv)) _INT1Interrupt

/* Config PLATE */
	#define PLATE_ASSER_KP                       3000
	#define PLATE_ASSER_KI                       0
	#define PLATE_ASSER_KD                       0
	#define PLATE_ASSER_TIMEOUT                  3000   //en ms
	#define PLATE_ASSER_POS_EPSILON              5      //TODO: à ajuster plus correctement
	#define PLATE_HORIZONTAL_POS                 85    //TODO: ajuster ces valeurs ! (unité: potarland)
	#define PLATE_PREPARE_POS                    263
	#define PLATE_VERTICAL_POS                   402   /////////////////////////////
	#define PLATE_DCMOTOR_ID                     1      //Doit être unique !
	#define PLATE_DCMOTOR_PWM_NUM                3
	#define PLATE_DCMOTOR_PORT_WAY               PORTE
	#define PLATE_DCMOTOR_PORT_WAY_BIT           4
	#define PLATE_DCMOTOR_MAX_PWM_WAY0           10
	#define PLATE_DCMOTOR_MAX_PWM_WAY1           10
	#define PLATE_ROTATION_POTAR_ADC_ID          AN9_ID
	#define PLATE_PLIER_AX12_ASSER_TIMEOUT       20     //unité: 0.1s  (une valeur de 20 correspond à 0.2s soit 200ms)
	#define PLATE_PLIER_AX12_ID                  0	    //TODO: changer ces valeurs !!!
	#define PLATE_PLIER_AX12_ASSER_POS_EPSILON   10     //en degré
	#define PLATE_PLIER_AX12_OPEN_POS            0      //en degré
	#define PLATE_PLIER_AX12_CLOSED_POS          0      //en degré
	#define PLATE_PLIER_AX12_MAX_TORQUE_PERCENT  50     //A mettre a une valeur correcte pour pincer assez fort sans risquer d'endommager l'AX12.

/* Config LIFT */
	#define LIFT_LEFT_ASSER_KP                       3000
	#define LIFT_LEFT_ASSER_KI                       0
	#define LIFT_LEFT_ASSER_KD                       0
	#define LIFT_LEFT_ASSER_TIMEOUT                  3000   //en ms
	#define LIFT_LEFT_ASSER_POS_EPSILON              5      //TODO: à ajuster plus correctement
	#define LIFT_LEFT_UP_POS                         85    //TODO: ajuster ces valeurs ! (unité: potarland)
	#define LIFT_LEFT_MID_POS                        263
	#define LIFT_LEFT_DOWN_POS                       402   /////////////////////////////
	#define LIFT_LEFT_DCMOTOR_ID                     1      //Doit être unique !
	#define LIFT_LEFT_DCMOTOR_PWM_NUM                3
	#define LIFT_LEFT_DCMOTOR_PORT_WAY               PORTE
	#define LIFT_LEFT_DCMOTOR_PORT_WAY_BIT           4
	#define LIFT_LEFT_DCMOTOR_MAX_PWM_WAY0           10
	#define LIFT_LEFT_DCMOTOR_MAX_PWM_WAY1           10
	#define LIFT_LEFT_TRANSLATION_POTAR_ADC_ID       AN9_ID
	#define LIFT_LEFT_PLIER_AX12_ASSER_TIMEOUT       20     //unité: 0.1s  (une valeur de 20 correspond à 0.2s soit 200ms)
	#define LIFT_LEFT_PLIER_AX12_ID                  0	    //TODO: changer ces valeurs !!!
	#define LIFT_LEFT_PLIER_AX12_ASSER_POS_EPSILON   10     //en degré
	#define LIFT_LEFT_PLIER_AX12_OPEN_POS            0      //en degré
	#define LIFT_LEFT_PLIER_AX12_CLOSED_POS          0      //en degré
	#define LIFT_LEFT_PLIER_AX12_MAX_TORQUE_PERCENT  50     //A mettre a une valeur correcte pour pincer assez fort sans risquer d'endommager l'AX12.

	#define LIFT_RIGHT_ASSER_KP                       3000
	#define LIFT_RIGHT_ASSER_KI                       0
	#define LIFT_RIGHT_ASSER_KD                       0
	#define LIFT_RIGHT_ASSER_TIMEOUT                  3000   //en ms
	#define LIFT_RIGHT_ASSER_POS_EPSILON              5      //TODO: à ajuster plus correctement
	#define LIFT_RIGHT_UP_POS                         85    //TODO: ajuster ces valeurs ! (unité: potarland)
	#define LIFT_RIGHT_MID_POS                        263
	#define LIFT_RIGHT_DOWN_POS                       402   /////////////////////////////
	#define LIFT_RIGHT_DCMOTOR_ID                     1      //Doit être unique !
	#define LIFT_RIGHT_DCMOTOR_PWM_NUM                3
	#define LIFT_RIGHT_DCMOTOR_PORT_WAY               PORTE
	#define LIFT_RIGHT_DCMOTOR_PORT_WAY_BIT           4
	#define LIFT_RIGHT_DCMOTOR_MAX_PWM_WAY0           10
	#define LIFT_RIGHT_DCMOTOR_MAX_PWM_WAY1           10
	#define LIFT_RIGHT_TRANSLATION_POTAR_ADC_ID       AN9_ID
	#define LIFT_RIGHT_PLIER_AX12_ASSER_TIMEOUT       20     //unité: 0.1s  (une valeur de 20 correspond à 0.2s soit 200ms)
	#define LIFT_RIGHT_PLIER_AX12_ID                  0	    //TODO: changer ces valeurs !!!
	#define LIFT_RIGHT_PLIER_AX12_ASSER_POS_EPSILON   10     //en degré
	#define LIFT_RIGHT_PLIER_AX12_OPEN_POS            0      //en degré
	#define LIFT_RIGHT_PLIER_AX12_CLOSED_POS          0      //en degré
	#define LIFT_RIGHT_PLIER_AX12_MAX_TORQUE_PERCENT  50     //A mettre a une valeur correcte pour pincer assez fort sans risquer d'endommager l'AX12.

/**************** Capteurs ****************/


#endif /* ndef KRUSTY_GLOBAL_CONFIG_H && def I_AM_ROBOT_KRUSTY */
