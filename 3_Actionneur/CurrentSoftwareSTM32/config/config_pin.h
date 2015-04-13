/*
 *  Club Robot ESEO 2013 - 2014
 *
 *  $Id$
 *
 *  Package : Carte Actionneur
 *  Description : Configuration des pins
 *  Auteur : Jacen, Alexis
 */

#ifndef CONFIG_PIN_H
#define CONFIG_PIN_H
#include "config_global.h"
#include "../stm32f4xx/stm32f4xx_gpio.h"

/* Definition des connectiques capteurs et actionneurs COMMUN à Holly & Wood */
// Définir les configurations pin propre à chaque robot dans config_big/config_pin.h ou config_small/config_pin.h !

/* LEDs */
#define LED_ERROR 			GPIOD,GPIO_Pin_10
#define LED_SELFTEST 		GPIOD,GPIO_Pin_11
#define LED_RUN  			GPIOD,GPIO_Pin_12
#define LED_CAN  			GPIOD,GPIO_Pin_13
#define LED_UART 			GPIOD,GPIO_Pin_14
#define LED_USER 			GPIOD,GPIO_Pin_15

/* Buttons */
#ifdef FDP_2014
	#define BUTTON1_PORT		(!GPIOE->IDR12)
	#define BUTTON2_PORT		(!GPIOE->IDR13)
	#define BUTTON3_PORT		(!GPIOE->IDR14)
	#define BUTTON4_PORT		(!GPIOE->IDR15)
	#define SWITCH_PE7			(GPIOE->IDR7)
	#define SWITCH_PE8			(GPIOE->IDR8)
	#define SWITCH_PE9			(GPIOE->IDR9)
	#define SWITCH_PE10			(GPIOE->IDR10)
	#define SWITCH_PE11			(GPIOE->IDR11)

#endif
#define PORT_ROBOT_ID  		GPIOC->IDR13

/* Switchs */


//Test et inclusion des configs spécifiques au robot
#if (defined(I_AM_ROBOT_BIG) && defined(I_AM_ROBOT_SMALL)) || (!defined(I_AM_ROBOT_BIG) && !defined(I_AM_ROBOT_SMALL))
#	error "Veuillez définir I_AM_ROBOT_BIG ou I_AM_ROBOT_SMALL selon le robot cible dans config_global.h"
#elif defined(I_AM_ROBOT_BIG)
#	include "config_big/config_pin.h"
#elif defined(I_AM_ROBOT_SMALL)
#	include "config_small/config_pin.h"
#endif

#endif /* CONFIG_PIN_H */
