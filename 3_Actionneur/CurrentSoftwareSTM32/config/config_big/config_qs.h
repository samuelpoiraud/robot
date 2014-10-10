/*
 *  Club Robot ESEO 2013 - 2015
 *
 *  $Id$
 *
 *  Package : Carte Actionneur
 *  Description : Configuration des modules de QS
 *  Auteur : Jacen, Alexis
 */

#ifndef BIG_CONFIG_QS_H
#define BIG_CONFIG_QS_H

/* Definition des périphériques, connectiques capteurs et actionneurs du gros robot */

/**************** Périphériques ****************/

/* Réglages PWM */
	//#define USE_PWM1
	//#define USE_PWM2
	//#define USE_PWM3
	//#define USE_PWM4
	#define FREQ_PWM_50KHZ

/* Servo-Moteurs AX12 */
	#define USE_AX12_SERVO
	#define AX12_NUMBER 23
	#define AX12_INSTRUCTION_BUFFER_SIZE 200
	#define AX12_TIMER_ID 2
	#define AX12_STATUS_RETURN_MODE AX12_STATUS_RETURN_ALWAYS	//Permet de savoir quand l'AX12 n'est pas bien connecté ou ne répond pas.
	#define AX12_STATUS_RETURN_CHECK_CHECKSUM
	#define AX12_UART_ID 3
	#define AX12_RX24_UART_ID 2
	//#define AX12_DEBUG_PACKETS

/* Servo-Moteurs standart */
	#define SERVO_TIMER 5
	#define USE_SERVO

#endif /* BIG_CONFIG_QS_H */
