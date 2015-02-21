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
#define ELEVATOR_KP							10
#define ELEVATOR_KI							10
#define ELEVATOR_KD							10
#define ELEVATOR_KP2						10
#define ELEVATOR_KI2						10
#define ELEVATOR_KD2						10

#define ELEVATOR_PWM_NUM					1
#define ELEVATOR_PORT_WAY					GPIOA
#define ELEVATOR_PORT_WAY_BIT				8
#define ELEVATOR_MAX_PWM_WAY0				80
#define ELEVATOR_MAX_PWM_WAY1				80

#define ELEVATOR_ASSER_TIMEOUT				2000
#define ELEVATOR_ASSER_POS_EPSILON			20


// Position
#define ACT_ELEVATOR_BOT_POS				10
#define ACT_ELEVATOR_MID_POS				10
#define ACT_ELEVATOR_PRE_TOP_POS			10
#define ACT_ELEVATOR_TOP_POS				10

#define ACT_ELEVATOR_INIT_POS				ACT_ELEVATOR_BOT_POS

#endif	/* ELEVATOR_CONFIG_H */


