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
	#define HAMMER_SENSOR_ADC_ID        AN3_ID	//Utilis� par ADC_getValue(x), A DEFINIR !!!
	#define HAMMER_ASSER_KP             2
	#define HAMMER_ASSER_KI             0
	#define HAMMER_ASSER_KD             0
	#define HAMMER_ASSER_TIMEOUT        3000    //en ms
	#define HAMMER_ASSER_POS_EPSILON    20
	#define HAMMER_UNITS_PER_128_DEGRE  100     //TODO: A changer
	#define HAMMER_UNITS_AT_0_DEGRE     0
	#define HAMMER_DCMOTOR_PWM_NUM      1		//PWM1
	#define HAMMER_DCMOTOR_PORT_WAY     PORTE	//sens1: RE0
	#define HAMMER_DCMOTOR_PORT_WAY_BIT 0
	#define HAMMER_DCMOTOR_MAX_PWM_WAY0 30		//en %
	#define HAMMER_DCMOTOR_MAX_PWM_WAY1 30		//en %
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
	#define BALLINFLATER_PIN                PORTGbits.RG6
	#define BALLINFLATER_ON                 1
	#define BALLINFLATER_OFF                0
/*************************************************************/

/* Config CANDLECOLOR: capteur tritronics CW2 pour la couleur des bougies */
	#define CANDLECOLOR_CW_ID                   0
	#define CANDLECOLOR_CW_PIN_ADC_x            AN2_ID  //TODO: a changer
	#define CANDLECOLOR_CW_PIN_ADC_y            AN12_ID
	#define CANDLECOLOR_CW_PIN_ADC_Y            AN13_ID

	//Detection digitale des couleurs (entre 0 et 3 inclus)
	//#define CANDLECOLOR_CW_USE_DIGITAL   //Si d�fini, on utilise la detection digitale des couleurs. Si aucune n'est detect�e, une detection analogique sera faite si activ�
	#define CANDLECOLOR_CW_CHANNEL_WHITE        0
	#define CANDLECOLOR_CW_CHANNEL_BLUE         1
	#define CANDLECOLOR_CW_CHANNEL_RED          2
	#define CANDLECOLOR_CW_CHANNEL_YELLOW       3

	//Detection analogique des couleurs
	#define CANDLECOLOR_CW_USE_ANALOG   //Si d�fini, on utilise la detection analogique des couleurs (seulement si aucune couleur n'ont t� detect� par le mode digital)
	//Pour les couleurs, voir http://www.efg2.com/Lab/Graphics/Colors/CIE1931.jpg
	//Les valeurs sont en pourcentage*1024
	//Defines dans l'ordre de detection, la premiere est prioritaire
	//TODO: ajuster les valeurs
	#define CANDLECOLOR_CW_YELLOW_MIN_xy        409   //valeur mini de x et y pour pouvoir detecter du jaune (CANDLECOLOR_CW_YELLOW_MAX_DIFF_xy doit corespondre aussi pour detecter du jaune)
	#define CANDLECOLOR_CW_YELLOW_MAX_DIFF_xy   51    //diff�rence maximale entre x et y  pour pouvoir detecter du jaune
	#define CANDLECOLOR_CW_RED_MIN_x            512   //valeur mini de x pour detecter du rouge (x+y+z=1)
	#define CANDLECOLOR_CW_BLUE_MIN_z           512   //valeur mini de z pour detecter du bleue
	#define CANDLECOLOR_CW_GREEN_MIN_y          512   //valeur mini de y pour detecter du vert
	#define CANDLECOLOR_CW_WHITE_MIN_Y          512   //Valeur mini de Y pour detecter du blanc (il ne faut pas qu'on detecte une autre couleur pour le detecter)

	#define CANDLECOLOR_AX12_ID                 6
	#define CANDLECOLOR_AX12_TIMEOUT            30  //en centaine de ms, une valeur de 20 correspond � 2secondes
	#define CANDLECOLOR_AX12_POS_EPSILON        5
	#define CANDLECOLOR_AX12_HIGH_CANDLE_POS    0  //TODO: a changer
	#define CANDLECOLOR_AX12_LOW_CANDLE_POS     0
	#define CANDLECOLOR_AX12_ANGLE_MIN          0
	#define CANDLECOLOR_AX12_ANGLE_MAX          300
	#define CANDLECOLOR_AX12_MAX_TORQUE_PERCENT 100
/**************************************************************************/

	
/**************** Capteurs ****************/

/* Capteur Tritronics CW2 */
	#define USE_CW_SENSOR
	#define CW_SENSOR_NUMBER 1

#endif /* ndef TINY_GLOBAL_CONFIG_H && def I_AM_ROBOT_TINY */
