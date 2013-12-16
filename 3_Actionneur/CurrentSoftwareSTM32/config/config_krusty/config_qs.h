/*
 *  Club Robot ESEO 2013 - 2014
 *
 *  $Id$
 *
 *  Package : Carte Actionneur
 *  Description : Configuration des modules de QS
 *  Auteur : Jacen, Alexis
 */

#ifndef KRUSTY_CONFIG_QS_H
#define KRUSTY_CONFIG_QS_H

/* Definition des p�riph�riques, connectiques capteurs et actionneurs de Krusty */

/* Entr�es analogiques: attention � la renum�rotation des entr�es... */
	#define USE_ANALOG_EXT_VREF

	#define USE_AN2
	#define AN2_ID  0
	#define USE_AN3
	#define AN3_ID  1
	#define USE_AN9
	#define AN9_ID  2

/**************** P�riph�riques ****************/

/* QEI sur IT */
/*
	#define USE_QEI_ON_IT
	#define QEI_ON_IT_QA		1
	#define QEI_ON_IT_QB		2
*/

/* R�glages PWM */
	#define FREQ_PWM_50KHZ

/* Asservissement en position/vitesse de moteurs CC */
	/* d�clarer l'utilisation du pilote */
	#define USE_DCMOTOR2
	/* timer utilis� pour l'asservissement (1, 2, ou 3) */
	#define DCM_TIMER			1
	/* d�finir le nombre d'actionneurs asservis */
	#define DCM_NUMBER			4
	/* P�riode d'asservisement (en ms) */
	#define DCM_TIMER_PERIOD	2  //Bas pour le lanceur de balle ...
	/* nombre maximum de positions � g�rer par moteur */
	#define DCMOTOR_NB_POS		3

/* Servo-Moteurs AX12 */
	#define USE_AX12_SERVO
	#define AX12_NUMBER 7
	#define AX12_INSTRUCTION_BUFFER_SIZE 200
	//#define AX12_USE_WATCHDOG
	#define AX12_TIMER_ID 2
	#define AX12_STATUS_RETURN_MODE AX12_STATUS_RETURN_ALWAYS	//Permet de savoir quand l'AX12 n'est pas bien connect� ou ne r�pond pas.
	#define AX12_STATUS_RETURN_CHECK_CHECKSUM
	//#define AX12_DEBUG_PACKETS

#endif /* KRUSTY_CONFIG_QS_H */
