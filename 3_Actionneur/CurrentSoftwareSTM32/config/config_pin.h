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

/* LEDs sur FDP */
#define LED_ERROR 			GPIOD,GPIO_Pin_10
#define LED_SELFTEST 		GPIOD,GPIO_Pin_11

/* LEDs sur STM32 */
#define LED_RUN  			GPIOD,GPIO_Pin_12
#define LED_CAN  			GPIOD,GPIO_Pin_13
#define LED_UART 			GPIOD,GPIO_Pin_14
#define LED_USER 			GPIOD,GPIO_Pin_15


#define PORT_ROBOT_ID  		GPIOC->IDR13


//Test et inclusion des configs sp�cifiques au robot
#if (defined(I_AM_ROBOT_BIG) && defined(I_AM_ROBOT_SMALL)) || (!defined(I_AM_ROBOT_BIG) && !defined(I_AM_ROBOT_SMALL))
#	error "Veuillez d�finir I_AM_ROBOT_BIG ou I_AM_ROBOT_SMALL selon le robot cible dans config_global.h"
#elif defined(I_AM_ROBOT_BIG)
#	include "config_big/config_pin.h"
#elif defined(I_AM_ROBOT_SMALL)
#	include "config_small/config_pin.h"
#endif

#endif /* CONFIG_PIN_H */
