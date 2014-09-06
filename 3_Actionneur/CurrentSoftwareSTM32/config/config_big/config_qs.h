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

/* Definition des p�riph�riques, connectiques capteurs et actionneurs de Krusty */

/* Entr�es analogiques: attention � la renum�rotation des entr�es... */
	#define USE_ANALOG_EXT_VREF

	// !!!!!!!!!!!!!!!! ADC 2 & 3 sont sur les pins de l'uart 2 !!!!!!!!!!!!!!!!
//	#define USE_AN2
	#define AN2_ID  0
//	#define USE_AN3
	#define AN3_ID  1
	//#define USE_AN9
	//#define AN9_ID  2

/**************** P�riph�riques ****************/

/* QEI sur IT */
/*
	#define USE_QEI_ON_IT
	#define QEI_ON_IT_QA		1
	#define QEI_ON_IT_QB		2
*/

/* R�glages PWM */
	#define USE_PWM1
	#define USE_PWM2
	#define USE_PWM3
	#define USE_PWM4
	#define FREQ_PWM_50KHZ

/* Asservissement en position/vitesse de moteurs CC */
	/* d�clarer l'utilisation du pilote */
	#define USE_DCMOTOR2
	/* d�finir le nombre d'actionneurs asservis */
	#define DCM_NUMBER			1
	/* P�riode d'asservisement (en ms) */
	#define DCM_TIME_PERIOD	10
	/* nombre maximum de positions � g�rer par moteur */
	#define DCMOTOR_NB_POS		1

/* Servo-Moteurs AX12 */
// !!!!!!!!!!!!!!!! ADC 2 & 3 sont sur les pins de l'uart 2 !!!!!!!!!!!!!!!!
	#define USE_AX12_SERVO
	#define AX12_NUMBER 23
	#define AX12_INSTRUCTION_BUFFER_SIZE 200
	//#define AX12_USE_WATCHDOG
	#define AX12_TIMER_ID 2
	#define AX12_STATUS_RETURN_MODE AX12_STATUS_RETURN_ALWAYS	//Permet de savoir quand l'AX12 n'est pas bien connect� ou ne r�pond pas.
	#define AX12_STATUS_RETURN_CHECK_CHECKSUM
	#define AX12_UART_ID 3
	#define AX12_RX24_UART_ID 2
	//#define AX12_DEBUG_PACKETS

	#define USE_WATCHDOG
	#define WATCHDOG_TIMER 3
	#define WATCHDOG_MAX_COUNT 5
	#define WATCHDOG_QUANTUM 1

	#define USE_SERVO
	#define SERVO_TIMER 0

#endif /* PIERRE_CONFIG_QS_H */
