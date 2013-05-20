/*
 *  Club Robot ESEO 2008 - 2010
 *  Archi'Tech, PACMAN
 *
 *  Fichier : QS_config_doc.h
 *  Package : Standard_Project
 *  Description : Liste des configurations des modules de QS
 *  Auteur : Jacen
 *	Licence : CeCILL-C (voir LICENCE.txt)
 *  Version 20101010
 */



/*	Ce fichier récapitule les différentes configurations
 *	disponibles pour les modules de la QS
 *	
 *	On trouvera dans l'ordre :
 *	L'indication de la carte faisant tourner le code
 *	La frequence du pic
 *	Les TRIS (config entrée sortie)
 *	Le CAN
 *	L'UART
 *	Les PWM
 *	L'activation des entrées analogiques
 *	La configuration de la commande de moteur pas à pas
 *	La configuration du module de controle de servomoteurs
 *	La configuration du module d'asservissement de moteurs CC
 *	La configuration du module de gestion de codeur sur IT
 * 	La configuration du module Watchdog
 * 	La configuration du module dédié à la gestion des servomoteurs AX12/AX12+
 */
 
#if 0

	/* Le mode verbeux active debug_printf */
	#define VERBOSE_MODE

	/* Pour certaines config particulieres, il faut definir qui on est
	 * a l'aide d'une des valeurs du type cartes_e de QS_types.h */
	#define I_AM CARTE_STRAT

	/* Il faut choisir à quelle frequence on fait tourner le STM32 */
	#define HCLK_FREQUENCY_HZ     48000000	//Max: 168Mhz
	#define PCLK1_FREQUENCY_HZ    24000000	//Max: 42Mhz
	#define PCLK2_FREQUENCY_HZ    48000000	//Max: 84Mhz
	#define CPU_EXTERNAL_CLOCK_HZ 10000000	//Fréquence de l'horloge externe
	
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
	#define PORT_D_IO_MASK	0xFFFF
	#define PORT_E_IO_MASK	0xFFFF

	/* Les instructions suivantes permettent de configurer certaines
	 * entrees/sorties du pic pour realiser des fonctionnalites
	 * particulieres comme une entree analogique 
	 */

//	#define USE_CAN
//	#define USE_CAN2
/*	Nombre de messages CAN conservés
	pour traitement hors interuption */
	#define CAN_BUF_SIZE		4
	
/*  Possibilité de redéfinir la priorité du CAN
	(Ex : pour la carte propulsion afin de gérer les codeurs incrémentaux) */
	#define QS_CAN_RX_IT_PRI	CAN_INT_PRI_6 /* CAN_INT_PRI_7,CAN_INT_PRI_6,CAN_INT_PRI_5 ... */
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
/* choix de la fréquence des PWM */
//	#define FREQ_PWM_50HZ
//	#define FREQ_PWM_1KHZ
//	#define FREQ_PWM_10KHZ
//	#define FREQ_PWM_20KHZ
	#define FREQ_PWM_50KHZ
//
//
/* utilisation des entrées analogiques */
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

	/* 	reglage moteur pas à pas */
	/* 	se référer à QS_Step_motor.h pour plus de renseignements sur
	 *	le module
	 */
	/* premierement, indiquer l'utilisation du pilote de moteur pas a pas */
	#define USE_STEP_MOTOR
	/* numero du timer à utiliser (1, 2 ou 3) */
	#define STEP_MOTOR_TIMER 			2
	/* temps entre chaque pas (en ms) */
	#define	STEP_MOTOR_MS_PER_STEP		5
	/* affectation des pins de controle du pas à pas */
	#define STEP_MOTOR1		PORTEbits.RE1
	#define STEP_MOTOR2		PORTEbits.RE5
	#define STEP_MOTOR3		PORTEbits.RE3
	#define STEP_MOTOR4		PORTEbits.RE7
	
	
	/* 	Controle servo par timer */
	/* déclarer l'utilisation du pilote */
	#define USE_SERVO
	/* numero du timer à utiliser (1, 2 ou 3) */
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
	/* déclarer l'utilisation du pilote */
	#define USE_DCMOTOR
	/* timer utilisé pour l'asservissement (1, 2, 3 ou 4) */
	#define DCM_TIMER			3
	/* définir le nombre d'actionneurs asservis */
	#define DCM_NUMBER			3
	/* Période d'asservisement (en ms) */
	#define DCM_TIMER_PERIOD	10
	/* Seuil d'acceptation d'arrivée */
	#define DCM_EPSILON			10				
	/* nombre maximum de positions à gérer par moteur */
	#define DCMOTOR_NB_POS		10
	
	/* watchdog */
	/* utilisation du module de watchdog */
	#define USE_WATCHDOG
	/* timer utilisé pour le watchdog (1, 2, 3 ou 4) */
	#define WATCHDOG_TIMER			4
	/* granularité du temps en ms */
	#define WATCHDOG_QUANTUM	1
	/* nombre maximal de watchdogs en exécution */
	#define WATCHDOG_MAX_COUNT	16
	
	/* utilisation du module de gestion des boutons */
	#define USE_BUTTONS
	/* timer utilisé pour les bouttons (1, 2, 3 ou 4) */
	#define BUTTONS_TIMER		4	
	
	/* Servomoteurs AX12/AX12+ */
	/* ! Attention ! Ce module utilise l'UART2 du PIC, il est donc interdit de définir USE_UART2 ... */
	/* utilisation du module dédié à la gestion des servomoteurs AX12/AX12+ */
	//Pour utiliser l'AX12, vous devez définir USE_AX12_SERVO
	#define USE_AX12_SERVO
	//Et indiquer leur nombre en définissant AX12_NUMBER
	//Exemple pour gérer 4 AX12:
	#define AX12_NUMBER 4
	//Port du PIC dédié à la gestion de l'UART en half-duplex pour indiquer le sens du bus voulu
	//Exemple pour le pour le pin RG8 (Port G, bit 8)
	#define AX12_DIRECTION_PORT PORTGBITS.RG8
	//Vous devez choisir un timer qui sera utilisé pour définir un timeout lors de l'attente du paquet de status envoyé par l'AX12
	//Exemple pour utiliser le timer 1:
	#define AX12_TIMER_ID 1
	//Le choix du temps avant timeout est fait avec AX12_STATUS_RETURN_TIMEOUT (recommandé, non obligatoire, valeur par défaut: 5ms)
	//Exemple pour une attente de maximum 50ms:
	#define AX12_STATUS_RETURN_TIMEOUT 50
	//Pour choisir le baudrate utilisé par les AX12 à commander, définissez AX12_UART_BAUDRATE
	//Exemple pour une vitesse de 56700 bauds:
	#define AX12_UART_BAUDRATE 56700
	//Pour d'autre options de l'AX12, voir QS_ax12.h

#endif /* 0 */
