/*
 *  Club Robot ESEO 2013 - 2014
 *
 *  $Id$
 *
 *  Package : Carte Actionneur
 *  Description : Configuration des modules de QS
 *  Auteur : Jacen, Alexis
 */

#ifndef PIERRE_CONFIG_QS_H
#define PIERRE_CONFIG_QS_H

/* Definition des périphériques, connectiques capteurs et actionneurs de Krusty */

/* Entrées analogiques: attention à la renumérotation des entrées... */
	#define USE_ANALOG_EXT_VREF

	// !!!!!!!!!!!!!!!! ADC 2 & 3 sont sur les pins de l'uart 2 !!!!!!!!!!!!!!!!
//	#define USE_AN2
	#define AN2_ID  0
//	#define USE_AN3
	#define AN3_ID  1
	//#define USE_AN9
	//#define AN9_ID  2

/**************** Périphériques ****************/

/* QEI sur IT */
/*
	#define USE_QEI_ON_IT
	#define QEI_ON_IT_QA		1
	#define QEI_ON_IT_QB		2
*/

/* Réglages PWM */
	#define USE_PWM1
	#define USE_PWM2
	#define USE_PWM3
	#define USE_PWM4
	#define FREQ_PWM_50KHZ

/* Asservissement en position/vitesse de moteurs CC */
	/* déclarer l'utilisation du pilote */
	#define USE_DCMOTOR2
	/* définir le nombre d'actionneurs asservis */
	#define DCM_NUMBER			1
	/* Période d'asservisement (en ms) */
	#define DCM_TIME_PERIOD	10
	/* nombre maximum de positions à gérer par moteur */
	#define DCMOTOR_NB_POS		1

/* Servo-Moteurs AX12 */
// !!!!!!!!!!!!!!!! ADC 2 & 3 sont sur les pins de l'uart 2 !!!!!!!!!!!!!!!!
	#define USE_AX12_SERVO
	#define AX12_NUMBER 22
	#define AX12_INSTRUCTION_BUFFER_SIZE 200
	//#define AX12_USE_WATCHDOG
	#define AX12_TIMER_ID 2
	#define AX12_STATUS_RETURN_MODE AX12_STATUS_RETURN_ALWAYS	//Permet de savoir quand l'AX12 n'est pas bien connecté ou ne répond pas.
	#define AX12_STATUS_RETURN_CHECK_CHECKSUM
	#define AX12_UART_ID 3
	#define AX12_RX24_UART_ID 2
	//#define AX12_DEBUG_PACKETS

	#define USE_WATCHDOG
	#define WATCHDOG_TIMER 3
	#define WATCHDOG_MAX_COUNT 5
	#define WATCHDOG_QUANTUM 1

#endif /* PIERRE_CONFIG_QS_H */
