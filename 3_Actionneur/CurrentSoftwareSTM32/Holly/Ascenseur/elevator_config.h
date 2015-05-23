/*  Club Robot ESEO 2014 - 2015
 *	HOLLY - WOOD
 *
 *	Fichier : elevator_config.h
 *	Package : Carte actionneur
 *	Description : Gestion de l'ascenseur de l'actionneur spot
 *  Auteur : Arnaud
 *  Version 20130219
 *  Robot : HOLLY
 */

#ifndef ELEVATOR_CONFIG_H
#define	ELEVATOR_CONFIG_H

// Configuration de l'asservissement PID
#define ELEVATOR_KP							90
#define ELEVATOR_KI							15
#define ELEVATOR_KD							1
#define ELEVATOR_KP2						0
#define ELEVATOR_KI2						0
#define ELEVATOR_KD2						0

#define ELEVATOR_PWM_NUM					2
#define ELEVATOR_PORT_WAY					GPIOC
#define ELEVATOR_PORT_WAY_BIT				GPIO_Pin_10
#define ELEVATOR_MAX_PWM_WAY0				100
#define ELEVATOR_MAX_PWM_WAY1				100
#define ELEVATOR_QEI_COEF					0x0001

#define ELEVATOR_ASSER_TIMEOUT				2000
#define ELEVATOR_ASSER_POS_EPSILON			150
#define ELEVATOR_ASSER_POS_LARGE_EPSILON	500

#define ELEVATOR_MAX_VALUE					8000
#define ELEVATOR_MIN_VALUE					50


// Position
#define ACT_ELEVATOR_BOT_POS				50
#define ACT_ELEVATOR_MID_POS				2225
#define ACT_ELEVATOR_MID_LOW_POS			1000
#define ACT_ELEVATOR_ESTRAD_POS             3000
#define ACT_ELEVATOR_PRE_TOP_POS			5000
#define ACT_ELEVATOR_TOP_POS				7700

#define ACT_ELEVATOR_INIT_POS				ACT_ELEVATOR_MID_POS
#define ACT_ELEVATOR_SPEED					0

#endif	/* ELEVATOR_CONFIG_H */


