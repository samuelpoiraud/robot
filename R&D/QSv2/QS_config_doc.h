/*
 *  Club Robot ESEO 2008 - 2009
 *  Archi'Tech
 *
 *  Fichier : QS_config_doc.h
 *  Package : Standard_Project
 *  Description : Liste des configurations des modules de QS
 *  Auteur : Jacen
 *  Version 20090226
 */


/*	Ce fichier r�capitule les diff�rentes configurations
 *	disponibles pour les modules de la QS
 *	
 *	On trouvera dans l'ordre :
 *	Le mode
 *	L'indication de la carte faisant tourner le code
 *	La frequence du pic
 *	Les TRIS (config entr�e sortie)
 *	Le CAN
 *	L'UART
 *	Les PWM
 *	L'activation des entr�es analogiques
 *	La configuration de la commande de moteur pas � pas
 *	La configuration du module de controle de servomoteurs
 *	La configuration du module d'asservissement de moteurs CC
 */
#if 0

	/* En premier lieu, choisir si on souhaite executer un code de
	 * test ou le code de match */

//	#define TEST_MODE
//	#define MATCH_MODE

	/* Pour certaines config particulieres, il faut definir qui on est
	 * a l'aide d'une des valeurs du type cartes_e de QS_types.h */
	#define I_AM CARTE_P

	/* Il faut choisir � quelle frequence on fait tourner le PIC */
	#define FREQ_10MHZ
//	#define FREQ_10MHZ_FS_CLK	/* uniquement pour tester le quartz */
//	#define FREQ_20MHZ
//	#define FREQ_40MHZ
	
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
	#define PORT_D_IO_MASK	0xFFFF
	#define PORT_E_IO_MASK	0xFFFF
	#define PORT_F_IO_MASK	0xFFFF
	#define PORT_G_IO_MASK	0xFFFF

	/* Les instructions suivantes permettent de configurer certaines
	 * entrees/sorties du pic pour realiser des fonctionnalites
	 * particulieres comme une entree analogique 
	 */

//
//	#define USE_UART1
//	#define USE_UART1RXINTERRUPT
//	#define USE_UART2
//	#define USE_UART2RXINTERRUPT
/*	Taille de la chaine de caracteres memorisant
	les caracteres recus sur UART */
	#define UART_RX_BUF_SIZE	12
//
/* utilise les PWM L au lieu des H */
//	#define USE_PWM_L
/* en utilisant les PWM_H, permet d'en desactiver certaines */
//	#define DISABLE_PWM1H
//	#define DISABLE_PWM2H
//	#define DISABLE_PWM3H
//	#define DISABLE_PWM4H
/* choix de la fr�quence des PWM */
//	#define FREQ_PWM_50HZ
//	#define FREQ_PWM_1KHZ
//	#define FREQ_PWM_10KHZ
	#define FREQ_PWM_20KHZ
//	#define FREQ_PWM_50KHZ
//
//
/* utilisation des entr�es analogiques */
//	#define USE_AN0
//	#define USE_AN1
//	#define USE_AN2
//	#define USE_AN3
//	#define USE_AN4
//	#define USE_AN5
//	#define USE_AN6
//	#define USE_AN7
//	#define USE_AN8
//	#define USE_AN9
//	#define USE_AN10
//	#define USE_AN11
//	#define USE_AN12
//	#define USE_AN13
//	#define USE_AN14
//	#define USE_AN15
//	#define USE_ANALOG_EXT_VREF

	/* 	reglage moteur pas � pas */
	/* 	se r�f�rer � QS_Step_motor.h pour plus de renseignements sur
	 *	le module
	 */
	/* premierement, indiquer l'utilisation du pilote de moteur pas a pas */
	#define USE_STEP_MOTOR
	/* numero du timer � utiliser (1, 2 ou 3) */
	#define STEP_MOTOR_TIMER 			2
	/* temps entre chaque pas (en ms) */
	#define	STEP_MOTOR_MS_PER_STEP		5
	/* affectation des pins de controle du pas � pas */
	#define STEP_MOTOR1		PORTEbits.RE1
	#define STEP_MOTOR2		PORTEbits.RE5
	#define STEP_MOTOR3		PORTEbits.RE3
	#define STEP_MOTOR4		PORTEbits.RE7
	
	
	/* 	Controle servo par timer */
	/* d�clarer l'utilisation du pilote */
	#define USE_SERVO
	/* numero du timer � utiliser (1, 2 ou 3) */
	#define SERVO_TIMER 			2
	/* affectation des pins de controle des servos */
	#define SERVO0			PORTEbits.RB0
	#define SERVO1			PORTEbits.RB1
	#define SERVO2			PORTEbits.RB2
	#define SERVO3			PORTEbits.RB3
//	#define SERVO4
//	#define SERVO5
//	#define SERVO6
//	#define SERVO7
//	#define SERVO8
//	#define SERVO9

	/* asservissement en position de moteurs CC */
	/* d�clarer l'utilisation du pilote */
	#define USE_DCMOTOR
	/* timer utilis� pour l'asservissement (1, 2, ou 3) */
	#define DCM_TIMER			3
	/* d�finir le nombre d'actionneurs asservis */
	#define DCM_NUMBER			3
	/* P�riode d'asservisement (en ms) */
	#define DCM_TIMER_PERIOD	10
	/* Seuil d'acceptation d'arriv�e */
	#define DCM_EPSILON			10				
	/* nombre maximum de positions � g�rer par moteur */
	#define DCMOTOR_NB_POS		10

	
#endif /* 0 */
