/*  Club Robot ESEO 2012 - 2013
 *	Tiny
 *
 *  Fichier : Global_config.h
 *  Package : Actionneur
 *  Description : Configuration de la carte actionneur
 *  Auteur : Aurélien
 *  Version 20110225
 *  Robot : Tiny
 */

/************** Configuration Globale pour le robot TINY ******************/

#if !defined(TINY_GLOBAL_CONFIG_H) && defined(I_AM_ROBOT_TINY)
	#define TINY_GLOBAL_CONFIG_H
	
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
	#define PORT_G_IO_MASK	0xFFFF

/* Definition des périphériques, connectiques capteurs et actionneurs de Tiny */

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
	/*
	// QEI sur IT
	#define USE_QEI_ON_IT
	#define QEI_ON_IT_QA		1
	#define QEI_ON_IT_QB		2
	*/

	/* Réglages PWM */
	//#define DISABLE_PWM4H
	#define FREQ_PWM_50KHZ

	/* asservissement en position/vitesse de moteurs CC */
	/* déclarer l'utilisation du pilote */
	#define USE_DCMOTOR2
	/* timer utilisé pour l'asservissement (1, 2, ou 3) */
	#define DCM_TIMER			1
	/* définir le nombre d'actionneurs asservis */
	#define DCM_NUMBER			6 // A voir en fonction du futur asservissement en courant
	/* Période d'asservisement (en ms) */
	#define DCM_TIMER_PERIOD	8
	/* Seuil d'acceptation d'arrivée */
	#define DCM_EPSILON			8
	/* nombre maximum de positions à gérer par moteur */
	#define DCMOTOR_NB_POS		5

	/* Définition de la précision et des modes de calcul des sinus et cosinus (cf. maths_home.c/h) */
	//	#define FAST_COS_SIN
	//	#define COS_SIN_16384

/**************** Actionneurs ****************/

/* Config HAMMER: bras long utilisé pour éteindre les bougies du 1er étage */
	#define HAMMER_DCMOTOR_ID           0		//Utilisé pour le module DCMotor
	#define HAMMER_SENSOR_ADC_ID        AN3_ID	//Utilisé par ADC_getValue(x), A DEFINIR !!!
	#define HAMMER_ASSER_KP             2
	#define HAMMER_ASSER_KI             0
	#define HAMMER_ASSER_KD             0
	#define HAMMER_ASSER_TIMEOUT        3000    //en ms
	#define HAMMER_ASSER_POS_EPSILON    20
	#define HAMMER_DCMOTOR_PWM_NUM      2		//PWM1
	#define HAMMER_DCMOTOR_PORT_WAY     PORTE	//sens1: RE0
	#define HAMMER_DCMOTOR_PORT_WAY_BIT 2
	#define HAMMER_DCMOTOR_MAX_PWM_WAY0 30		//en %
	#define HAMMER_DCMOTOR_MAX_PWM_WAY1 30		//en %
/********************************************************************************/

/* Config LONGHAMMER: bras long utilisé pour éteindre les bougies du 2ème étage */
	#define LONGHAMMER_DCMOTOR_ID           1		//Utilisé pour le module DCMotor
	#define LONGHAMMER_SENSOR_ADC_ID        AN3_ID	//Utilisé par ADC_getValue(x)
	#define LONGHAMMER_ASSER_KP             2
	#define LONGHAMMER_ASSER_KI             0
	#define LONGHAMMER_ASSER_KD             0
	#define LONGHAMMER_ASSER_TIMEOUT        3000    //en ms
	#define LONGHAMMER_ASSER_POS_EPSILON    20
	#define LONGHAMMER_TARGET_POS_DOWN      0
	#define LONGHAMMER_TARGET_POS_UP        300
	#define LONGHAMMER_TARGET_POS_PARKED    0		//Position quand non utilisé (doit assurer un diamètre du robot minimal)
	#define LONGHAMMER_DCMOTOR_PWM_NUM      1		//PWM1
	#define LONGHAMMER_DCMOTOR_PORT_WAY     PORTE	//sens1: RE0
	#define LONGHAMMER_DCMOTOR_PORT_WAY_BIT 0
	#define LONGHAMMER_DCMOTOR_MAX_PWM_WAY0 30		//en %
	#define LONGHAMMER_DCMOTOR_MAX_PWM_WAY1 30		//en %
/********************************************************************************/

/* Config BALLINFLATER: gonfleur du ballon */
	#define BALLINFLATER_PIN                PORTGbits.RG6
	#define BALLINFLATER_ON                 1
	#define BALLINFLATER_OFF                0

	
/**************** Capteurs ****************/
	
	//Configuration des télémètres choisie (à voir dans telemeter.h)
	#define TELE_CONFIG TELE_CONFIG_2

#endif /* ndef TINY_GLOBAL_CONFIG_H && def I_AM_ROBOT_TINY */
