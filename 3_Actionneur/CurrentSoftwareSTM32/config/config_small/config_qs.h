/*
 *  Club Robot ESEO 2013 - 2014
 *
 *  $Id$
 *
 *  Package : Carte Actionneur
 *  Description : Configuration des modules de QS
 *  Auteur : Jacen, Alexis
 */

#ifndef TINY_CONFIG_QS_H
#define TINY_CONFIG_QS_H

/* Les instructions ci dessous d�finissent le comportement des
 * entrees sorties du pic. une configuration en entree correspond
 * a un bit a 1 (Input) dans le masque, une sortie a un bit a
 * 0 (Output).
 * Toute connection non utilisee doit etre configuree en entree
 * (risque de griller ou de faire bruler le pic)
 */

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

/* R�glages PWM */
#define USE_PWM1
#define USE_PWM2
#define USE_PWM3
#define USE_PWM4
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
#define AX12_STATUS_RETURN_MODE AX12_STATUS_RETURN_ALWAYS	//Permet de savoir quand l'AX12 n'est pas bien connect� ou ne r�pond pas.

/* Capteur Tritronics CW2 */
#define USE_CW_SENSOR
#define CW_SENSOR_NUMBER 1

#endif /* TINY_CONFIG_QS_H */
