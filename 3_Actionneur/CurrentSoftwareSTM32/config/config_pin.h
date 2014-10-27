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

/* Definition des connectiques capteurs et actionneurs COMMUN � Holly & Wood */
// D�finir les configurations pin propre � chaque robot dans config_big/config_pin.h ou config_small/config_pin.h !

/* LEDs */
#define LED_ERROR 			GPIOD,GPIO_PinSource10
#define LED_SELFTEST 		GPIOD,GPIO_PinSource11
#define LED_RUN  			GPIOD,GPIO_PinSource12
#define LED_CAN  			GPIOD,GPIO_PinSource13
#define LED_UART 			GPIOD,GPIO_PinSource14
#define LED_USER 			GPIOD,GPIO_PinSource15

/* Buttons */
#define BUTTON1_PORT		(!GPIOE->IDR12)
#define BUTTON2_PORT		(!GPIOE->IDR13)
#define BUTTON3_PORT		(!GPIOE->IDR14)
#define BUTTON4_PORT		(!GPIOE->IDR15)
#define PORT_ROBOT_ID  		GPIOC->IDR13

/* Switchs */
#define SWITCH_RG0 FALSE
#define SWITCH_RG1 FALSE

//Test et inclusion des configs sp�cifiques au robot
#if (defined(I_AM_ROBOT_BIG) && defined(I_AM_ROBOT_SMALL)) || (!defined(I_AM_ROBOT_BIG) && !defined(I_AM_ROBOT_SMALL))
#	error "Veuillez d�finir I_AM_ROBOT_BIG ou I_AM_ROBOT_SMALL selon le robot cible dans config_global.h"
#elif defined(I_AM_ROBOT_BIG)
#	include "config_big/config_pin.h"
#elif defined(I_AM_ROBOT_SMALL)
#	include "config_small/config_pin.h"
#endif

#endif /* CONFIG_PIN_H */
