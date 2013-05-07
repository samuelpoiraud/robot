/*  Club Robot ESEO 2012 - 2013
 *	Tiny
 *
 *  Fichier : Global_config.h
 *  Package : Actionneur
 *  Description : Configuration de la carte actionneur
 *  Auteur : Aur�lien
 *  Version 20110225
 *  Robot : Tiny
 */

/************** Configuration Globale pour le robot TINY ******************/

#if !defined(TINY_GLOBAL_CONFIG_H) && defined(I_AM_ROBOT_TINY)
	#define TINY_GLOBAL_CONFIG_H
	
	/* Les instructions ci dessous d�finissent le comportement des
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
	#define PORT_G_IO_MASK	0xFEBF  //Port RG6 en sortie pour le gonfleur, RG8 en sortie pour le sens du bus des AX12

/* Definition des p�riph�riques, connectiques capteurs et actionneurs de Tiny */

/* Entr�es analogiques: attention � la renum�rotation des entr�es... */
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

/**************** P�riph�riques ****************/
	/*
	// QEI sur IT
	#define USE_QEI_ON_IT
	#define QEI_ON_IT_QA		1
	#define QEI_ON_IT_QB		2
	*/

	/* R�glages PWM */
	//#define DISABLE_PWM4H
	#define FREQ_PWM_50KHZ

	/* asservissement en position/vitesse de moteurs CC */
	/* d�clarer l'utilisation du pilote */
	#define USE_DCMOTOR2
	/* timer utilis� pour l'asservissement (1, 2, ou 3) */
	#define DCM_TIMER			1
	/* d�finir le nombre d'actionneurs asservis */
	#define DCM_NUMBER			1
	/* P�riode d'asservisement (en ms) */
	#define DCM_TIMER_PERIOD	8
	/* nombre maximum de positions � g�rer par moteur */
	#define DCMOTOR_NB_POS		3

/* Servo-Moteurs AX12 */
	#define USE_AX12_SERVO
	#define AX12_NUMBER 7
	//#define AX12_USE_WATCHDOG
	#define AX12_TIMER_ID 2
	#define AX12_DIRECTION_PORT LATGbits.LATG8
	#define AX12_STATUS_RETURN_MODE AX12_STATUS_RETURN_ALWAYS	//Permet de savoir quand l'AX12 n'est pas bien connect� ou ne r�pond pas.

	/* D�finition de la pr�cision et des modes de calcul des sinus et cosinus (cf. maths_home.c/h) */
	//	#define FAST_COS_SIN
	//	#define COS_SIN_16384

/**************** Actionneurs ****************/

/* Config HAMMER: bras long utilis� pour �teindre les bougies du 1er �tage */
	#define HAMMER_DCMOTOR_ID           0		//Utilis� pour le module DCMotor
	#define HAMMER_DCMOTOR_PWM_NUM      1		//PWM1
	#define HAMMER_DCMOTOR_PORT_WAY     PORTE	//sens1: RE0
	#define HAMMER_DCMOTOR_PORT_WAY_BIT 0
	#define HAMMER_SENSOR_ADC_ID        AN3_ID	//Utilis� par ADC_getValue(x)
	#define HAMMER_ACT_MOVE_TO_INIT_POS 85      //en degr�, 90� = vertical vers le bas, 0� = horizontal sorti
/********************************************************************************/

/* Config LONGHAMMER: bras long utilis� pour �teindre les bougies du 2�me �tage */
// Non utilis�
/*
	#define LONGHAMMER_DCMOTOR_ID           1		//Utilis� pour le module DCMotor
	#define LONGHAMMER_SENSOR_ADC_ID        AN3_ID	//Utilis� par ADC_getValue(x)
	#define LONGHAMMER_ASSER_KP             2
	#define LONGHAMMER_ASSER_KI             0
	#define LONGHAMMER_ASSER_KD             0
	#define LONGHAMMER_ASSER_TIMEOUT        3000    //en ms
	#define LONGHAMMER_ASSER_POS_EPSILON    20
	#define LONGHAMMER_TARGET_POS_DOWN      0
	#define LONGHAMMER_TARGET_POS_UP        300
	#define LONGHAMMER_TARGET_POS_PARKED    0		//Position quand non utilis� (doit assurer un diam�tre du robot minimal)
	#define LONGHAMMER_DCMOTOR_PWM_NUM      2		//PWM1
	#define LONGHAMMER_DCMOTOR_PORT_WAY     PORTE	//sens1: RE0
	#define LONGHAMMER_DCMOTOR_PORT_WAY_BIT 2
	#define LONGHAMMER_DCMOTOR_MAX_PWM_WAY0 30		//en %
	#define LONGHAMMER_DCMOTOR_MAX_PWM_WAY1 30		//en %
*/
/********************************************************************************/

/* Config BALLINFLATER: gonfleur du ballon *******************/
	#define BALLINFLATER_TIMER_ID           3
	#define BALLINFLATER_PIN                PORTGbits.RG6
	#define BALLINFLATER_ON                 1
	#define BALLINFLATER_OFF                0
/*************************************************************/

/* Config CANDLECOLOR: capteur tritronics CW2 pour la couleur des bougies */
	#define CANDLECOLOR_CW_ID                   0
	#define CANDLECOLOR_CW_PIN_ADC_X            AN13_ID
	#define CANDLECOLOR_CW_PIN_ADC_Y            AN12_ID
	#define CANDLECOLOR_CW_PIN_ADC_Z            AN2_ID
	#define CANDLECOLOR_CW_PIN_CHANNEL0         &PORTE
	#define CANDLECOLOR_CW_PIN_CHANNEL0_BIT     8
	#define CANDLECOLOR_CW_PIN_CHANNEL1         &PORTE
	#define CANDLECOLOR_CW_PIN_CHANNEL1_BIT     9
	#define CANDLECOLOR_AX12_ID                 6
/**************************************************************************/

/* Config PLIER: pince devant Tiny permettant de prendre des verres */
	#define PLIER_LEFT_AX12_ID                  0
	#define PLIER_RIGHT_AX12_ID                 2
/**************************************************************************/

	
/**************** Capteurs ****************/

/* Capteur Tritronics CW2 */
	#define USE_CW_SENSOR
	#define CW_SENSOR_NUMBER 1

#endif /* ndef TINY_GLOBAL_CONFIG_H && def I_AM_ROBOT_TINY */
