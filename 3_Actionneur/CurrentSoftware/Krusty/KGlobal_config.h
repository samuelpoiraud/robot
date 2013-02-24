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

//	#define VERBOSE_MODE
#define OUTPUT_LOG
	
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
	#define PORT_D_IO_MASK	0xC0FF
	#define PORT_E_IO_MASK	0xFF80
		
	#define PORT_F_IO_MASK	0xFFFF
	#define PORT_G_IO_MASK	0xFEFF //Pour avoir la masse sur les switch ascenceur

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
	#define DCM_NUMBER			6 // A voir en fonction du futur asservissement en courant
	/* Période d'asservisement (en ms) */
	#define DCM_TIMER_PERIOD	10
	/* Seuil d'acceptation d'arrivée */
	#define DCM_EPSILON			10
	/* nombre maximum de positions à gérer par moteur */
	#define DCMOTOR_NB_POS		5

/* Servo-Moteurs AX12 */
	#define AX12_STATUS_RETURN_MODE AX12_STATUS_RETURN_ALWAYS	//Permet de savoir quand l'AX12 n'est pas bien connecté ou ne répond pas.

/* Définition de la précision et des modes de calcul des sinus et cosinus (cf. maths_home.c/h) */
	//	#define FAST_COS_SIN
	//	#define COS_SIN_16384

/**************** Actionneurs ****************/

/* Config BALLLAUNCHER */	//TODO: Changer le numéro de PWM à 4 !!!!!
	#define BALLLAUNCHER_ASSER_KP                10
	#define BALLLAUNCHER_ASSER_KI                80
	#define BALLLAUNCHER_ASSER_KD                0
	#define BALLLAUNCHER_TARGET_SPEED            6000		//en tr/min
	#define BALLLAUNCHER_DCMOTOR_ID              0			//Doit être unique !
	#define BALLLAUNCHER_DCMOTOR_PWM_NUM         4
	#define BALLLAUNCHER_DCMOTOR_PORT_WAY        PORTE
	#define BALLLAUNCHER_DCMOTOR_PORT_WAY_BIT    6
	#define BALLLAUNCHER_DCMOTOR_MAX_PWM_WAY0    100
	#define BALLLAUNCHER_DCMOTOR_MAX_PWM_WAY1    0
	#define BALLLAUNCHER_EDGE_PER_ROTATION       2			//nombre d'inversion de champ magnétique par tour (2 aimants de sens opposé = 2 inversions / tour)
	#define BALLLAUNCHER_HALLSENSOR_INT_FLAG     IFS1bits.INT1IF
	#define BALLLAUNCHER_HALLSENSOR_INT_PRIORITY IPC4bits.INT1IP
	#define BALLLAUNCHER_HALLSENSOR_INT_ENABLE   IEC1bits.INT1IE
	#define BALLLAUNCHER_HALLSENSOR_INT_EDGE     INTCON2bits.INT1EP
	#define BALLLAUNCHER_HALLSENSOR_INT_ISR      __attribute__((interrupt, no_auto_psv)) _INT1Interrupt

/* Config BALLLAUNCHER */
	#define PLATE_ASSER_KP                       1
	#define PLATE_ASSER_KI                       0
	#define PLATE_ASSER_KD                       0
	#define PLATE_HORIZONTAL_POS                 100	//TODO: ajuster ces valeurs !
	#define PLATE_PREPARE_POS                    300
	#define PLATE_PARKED_POS                     4000
	#define PLATE_VERTICAL_POS                   5000	/////////////////////////////
	#define PLATE_DCMOTOR_ID                     1			//Doit être unique !
	#define PLATE_DCMOTOR_PWM_NUM                4
	#define PLATE_DCMOTOR_PORT_WAY               PORTE
	#define PLATE_DCMOTOR_PORT_WAY_BIT           6
	#define PLATE_DCMOTOR_MAX_PWM_WAY0           100
	#define PLATE_DCMOTOR_MAX_PWM_WAY1           0
	#define PLATE_ROTATION_POTAR_ADC_ID          AN9_ID
	#define PLATE_PLIER_AX12_ID                  0	//TODO: changer ces valeurs !!!
	#define PLATE_PLIER_AX12_POS_EPSILON         5
	#define PLATE_PLIER_AX12_OPEN_POS            0
	#define PLATE_PLIER_AX12_CLOSED_POS          0
	#define PLATE_PLIER_AX12_PARKED_POS          0
	
/**************** Capteurs ****************/


#endif /* ndef KRUSTY_GLOBAL_CONFIG_H && def I_AM_ROBOT_KRUSTY */
