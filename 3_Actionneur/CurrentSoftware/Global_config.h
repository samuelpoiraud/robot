/*
 *	Club Robot ESEO 2010 - 2011
 *	Check Norris
 *
 *  Fichier : Global_config.h
 *  Package : Actionneur
 *  Description : Configuration de la carte actionneur
 *  Auteur : Aurélien
 *  Version 20110225
 */

#ifndef GLOBAL_CONFIG_H
	#define GLOBAL_CONFIG_H
	#include "QS/QS_types.h"

//	#define VERBOSE_MODE

	/* ID de la carte: cf le type cartes_e de QS_types.h */
	#define I_AM CARTE_ACT		//A voir avec Gwenn pour changer
	#define I_AM_CARTE_ACT
	
	/* Il faut choisir à quelle frequence on fait tourner le PIC */
	#define FREQ_10MHZ
	
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
	#define PORT_G_IO_MASK	0xFFF3 //Pour avoir la masse sur les switch ascenceur

/* Definition des connectiques capteurs et actionneurs de Check Norris */


/* Actionneurs */
/*config ax12 a revoir pour le Direction port*/
        #define USE_AX12_SERVO
        #define AX12_NUMBER 3
        #define AX12_TIMER_ID 2
        #define AX12_DIRECTION_PORT LATEbits.LATE6 // revoir

        #define BALL_GRABBER_ID_SERVO (Uint8) 2
        #define HAMMER_ID_SERVO  (Uint8) 1

	// Files
	#define CLOCK_TIMER_run		TIMER3_run
	#define CLOCK_TIMER_isr		_T3Interrupt
	#define CLOCK_TIMER_flag	IFS0bits.T3IF		

/* Entrées analogiques: attention à la renumérotation des entrées... */	
	#define USE_ANALOG_EXT_VREF
	
	#define USE_AN2
	#define POTAR_CLAMP_BACK	0
	#define USE_AN9
	#define TELEMETER_FRONT_LEFT_ADC 1
	#define USE_AN10
	#define TELEMETER_FRONT_RIGHT_ADC 2
	#define USE_AN11
	#define POTAR_CLAMP_FRONT 3
	#define USE_AN12
	#define CURRENT_CLAMP_BACK  4
	#define USE_AN13
	#define CURRENT_CLAMP_FRONT 5
	#define USE_AN14
	#define TELEMETER_BACK_RIGHT_ADC 6	
	#define USE_AN15
	#define TELEMETER_BACK_LEFT_ADC	7	
	
/* Capteurs */
	
	//Configuration des télémètres choisie (à voir dans telemeter.h)
	#define TELE_CONFIG TELE_CONFIG_2

	
/*QEI sur IT*/
	#define USE_QEI_ON_IT
	#define QEI_ON_IT_QA		1
	#define QEI_ON_IT_QB		2
	
/* CAN */
	#define USE_CAN
	#define CAN_BUF_SIZE	32

/* Réglages UART */
	#define USE_UART1
	#define USE_UART1RXINTERRUPT
	#define UART_RX_BUF_SIZE	12
	
/*Bouton*/
 #define BUTTONS_TIMER 4
 #define USE_BUTTONS
	
/* Réglages PWM */
	//#define DISABLE_PWM4H
	#define FREQ_PWM_20KHZ
	
	/* asservissement en position de moteurs CC */
	/* déclarer l'utilisation du pilote */
	#define USE_DCMOTOR
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

#endif /* ndef GLOBAL_CONFIG_H */
