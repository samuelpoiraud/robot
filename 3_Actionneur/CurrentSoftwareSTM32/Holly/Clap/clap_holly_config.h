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
#define CLAP_HOLLY_KP						10
#define CLAP_HOLLY_KI						10
#define CLAP_HOLLY_KD						10

#define CLAP_HOLLY_PWM_NUM					3
#define CLAP_HOLLY_PORT_WAY					GPIOC
#define CLAP_HOLLY_PORT_WAY_BIT				GPIO_Pin_11
#define CLAP_HOLLY_MAX_PWM_WAY0				80
#define CLAP_HOLLY_MAX_PWM_WAY1				80

#define CLAP_HOLLY_ASSER_TIMEOUT			2000
#define CLAP_HOLLY_ASSER_POS_EPSILON		20


// Position
#define ACT_CLAP_HOLLY_IDLE_POS				10
#define ACT_CLAP_HOLLY_RIGHT_POS			10
#define ACT_CLAP_HOLLY_LEFT_POS				10

#define ACT_CLAP_HOLLY_INIT_POS				ACT_CLAP_HOLLY_IDLE_POS
#define ACT_CLAP_HOLLY_SPEED				20

#endif	/* CLAP_HOLLY_CONFIG_H */


