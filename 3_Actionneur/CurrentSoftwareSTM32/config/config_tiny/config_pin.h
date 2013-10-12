/*
 *  Club Robot ESEO 2013 - 2014
 *
 *  $Id$
 *
 *  Package : Carte Actionneur
 *  Description : Configuration des pins
 *  Auteur : Jacen, Alexis
 */

#ifndef TINY_CONFIG_PIN_H
#define TINY_CONFIG_PIN_H

/* Config HAMMER: bras long utilisé pour éteindre les bougies du 1er étage */
	#define HAMMER_DCMOTOR_ID           0		//Utilisé pour le module DCMotor
	#define HAMMER_DCMOTOR_PWM_NUM      2		//PWM2
	#define HAMMER_DCMOTOR_PORT_WAY     GPIOC->ODR	//sens2: PC10
	#define HAMMER_DCMOTOR_PORT_WAY_BIT 10
	#define HAMMER_SENSOR_ADC_ID        AN3_ID	//Utilisé par ADC_getValue(x)
	#define HAMMER_ACT_MOVE_TO_INIT_POS 85      //en degré, 90° = vertical vers le bas, 0° = horizontal sorti
/********************************************************************************/

/* Config BALLINFLATER: gonfleur du ballon *******************/
	#define BALLINFLATER_TIMER_ID           3
	#define BALLINFLATER_PIN                GPIOD->ODR2
	#define BALLINFLATER_ON                 1
	#define BALLINFLATER_OFF                0
/*************************************************************/

/* Config CANDLECOLOR: capteur tritronics CW2 pour la couleur des bougies */
	#define CANDLECOLOR_CW_ID                   0
	#define CANDLECOLOR_CW_PIN_ADC_X            AN13_ID
	#define CANDLECOLOR_CW_PIN_ADC_Y            AN12_ID
	#define CANDLECOLOR_CW_PIN_ADC_Z            AN2_ID
//	#define CANDLECOLOR_CW_PIN_CHANNEL0         &PORTE
//	#define CANDLECOLOR_CW_PIN_CHANNEL0_BIT     8
//	#define CANDLECOLOR_CW_PIN_CHANNEL1         &PORTE
//	#define CANDLECOLOR_CW_PIN_CHANNEL1_BIT     9
	#define CANDLECOLOR_AX12_ID                 6
/**************************************************************************/

/* Config PLIER: pince devant Tiny permettant de prendre des verres */
	#define PLIER_LEFT_AX12_ID                  0
	#define PLIER_RIGHT_AX12_ID                 2
/**************************************************************************/

#endif /* TINY_CONFIG_PIN_H */
