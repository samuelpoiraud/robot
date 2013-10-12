/*
 *  Club Robot ESEO 2013 - 2014
 *
 *  $Id: config_pin.h 897 2013-10-10 19:13:19Z amurzeau $
 *
 *  Package : Carte Actionneur
 *  Description : Configuration des pins
 *  Auteur : Jacen, Alexis
 */

#ifndef CONFIG_PIN_H
#define CONFIG_PIN_H

/* Definition des connectiques capteurs et actionneurs COMMUN à Tiny et Krusty */

/* LEDs */
#define LED_ERROR 			GPIOD->ODR10
#define LED_SELFTEST 		GPIOD->ODR11
#define LED_RUN  			GPIOD->ODR12
#define LED_CAN  			GPIOD->ODR13
#define LED_UART 			GPIOD->ODR14
#define LED_USER 			GPIOD->ODR15
#define LED_USER2 			GPIOD->ODR16

/* Buttons */
#define BUTTON1_PORT		(!GPIOE->IDR12)	//Selftest
#define BUTTON2_PORT		(!GPIOE->IDR13)	//Calibration
#define BUTTON3_PORT		(!GPIOE->IDR14)	//LCD Menu +
#define BUTTON4_PORT		(!GPIOE->IDR15)	//LCD Menu -
#define PORT_ROBOT_ID  		GPIOC->IDR13

/* Switchs */
#define SWITCH_RG0 FALSE
#define SWITCH_RG1 FALSE

//Test et inclusion des configs spécifiques au robot
#if (defined(I_AM_ROBOT_KRUSTY) && defined(I_AM_ROBOT_TINY)) || (!defined(I_AM_ROBOT_KRUSTY) && !defined(I_AM_ROBOT_TINY))
#	error "Veuillez définir I_AM_ROBOT_KRUSTY ou I_AM_ROBOT_TINY selon le robot cible dans config_global.h"
#elif defined(I_AM_ROBOT_KRUSTY)
#	include "config_krusty/config_pin.h"
#elif defined(I_AM_ROBOT_TINY)
#	include "config_tiny/config_pin.h"
#endif

#endif /* CONFIG_PIN_H */
