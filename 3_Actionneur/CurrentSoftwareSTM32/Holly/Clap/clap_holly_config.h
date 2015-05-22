/*  Club Robot ESEO 2014 - 2015
 *	HOLLY - WOOD
 *
 *	Fichier : clap_holly_config.h
 *	Package : Carte actionneur
 *	Description : Gestion de l'actionneur clap
 *  Auteur : Arnaud
 *  Version 20130219
 *  Robot : HOLLY
 */

#ifndef CLAP_HOLLY_CONFIG_H
#define	CLAP_HOLLY_CONFIG_H

// Configuration de l'asservissement PID
#define CLAP_HOLLY_KP						2000
#define CLAP_HOLLY_KI						0//1000
#define CLAP_HOLLY_KD						3000//3000

#define CLAP_HOLLY_PWM_NUM					3
#define CLAP_HOLLY_PORT_WAY					GPIOC
#define CLAP_HOLLY_PORT_WAY_BIT				GPIO_Pin_11
#define CLAP_HOLLY_MAX_PWM_WAY0				80
#define CLAP_HOLLY_MAX_PWM_WAY1				80

#define CLAP_HOLLY_ASSER_TIMEOUT			500
#define CLAP_HOLLY_ASSER_POS_EPSILON		10
#define CLAP_HOLLY_ASSER_POS_LARGE_EPSILON	CLAP_HOLLY_ASSER_POS_EPSILON



#define CLAP_MAX_VALUE						815
#define CLAP_MIN_VALUE						335
#define CLAP_ID								3

// Position
#define ACT_CLAP_HOLLY_IDLE_POS				565
#define ACT_CLAP_HOLLY_MIDDLE_RIGHT_POS		422
#define ACT_CLAP_HOLLY_RIGHT_POS			360
#define ACT_CLAP_HOLLY_MIDDLE_LEFT_POS		681
#define ACT_CLAP_HOLLY_LEFT_POS				720

#define ACT_CLAP_HOLLY_INIT_POS				ACT_CLAP_HOLLY_IDLE_POS
#define ACT_CLAP_HOLLY_SPEED				100
#define ACT_CLAP_HOLLY_TIMEOUT				400

#endif	/* CLAP_HOLLY_CONFIG_H */


