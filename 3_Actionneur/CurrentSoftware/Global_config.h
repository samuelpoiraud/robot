/*
 *	Club Robot ESEO 2010 - 2013
 *	Krusty & Tiny
 *
 *  Fichier : Global_config.h
 *  Package : Actionneur
 *  Description : Configuration des carte actionneur COMMUNE entre les robots
 *  Auteur : Aurélien
 *  Version 20130208
 *  Robot : Tous
 */

#ifndef GLOBAL_CONFIG_H
	#define GLOBAL_CONFIG_H

//	#define VERBOSE_MODE

	/* ID de la carte: cf le type cartes_e de QS_types.h */
	#define I_AM CARTE_ACT		//A voir avec Gwenn pour changer
	#define I_AM_CARTE_ACT

	/** Choix du robot cible. Une seule définition n'est possible, commentez l'autre.
	 * Si I_AM_ROBOT_KRUSTY est défini, le code spécifique au robot est pris dans le dossier Krusty.
	 * Si I_AM_ROBOT_TINY est défini, le code  sépcifique au robot est pris dans le dossier Tiny.
	 * Pour ceux qui veulent, il est possible de prédéfinir PREDEF_BUILD_TINY ou PREDEF_BUILD_KRUSTY avec le switch -DPREDEF_BUILD_TINY ou -DPREDEF_BUILD_KRUSTY de gcc, permet la creation de plusieurs target dans mplab X au moins.
	 * PREDEF_BUILD_TINY et PREDEF_BUILD_KRUSTY sont prioritaire sur la définition manuelle.
	 */
	//#define I_AM_ROBOT_KRUSTY
	//#define I_AM_ROBOT_TINY
	
	/* Il faut choisir à quelle frequence on fait tourner le PIC */
	#define FREQ_10MHZ

	 // DEFINIR LES PORTS DANS Tiny/Global_config.h ou Krusty/Global_config.h selon le robot !

/* Definition des connectiques capteurs et actionneurs COMMUN à Tiny et Krusty */


/* Actionneurs */

/* AX-12 */
//TODO: a déplacer dans Krusty/Global_config.h ou Tiny/Global_config.h
	#define USE_AX12_SERVO
	#define AX12_NUMBER 3
	#define AX12_TIMER_ID 2
	#define AX12_DIRECTION_PORT LATGbits.LATG8

	#define BALL_GRABBER_ID_SERVO (Uint8) 2
	#define HAMMER_ID_SERVO  (Uint8) 1
//Fin TO DO

	// Files
	#define CLOCK_TIMER_run		TIMER3_run
	#define CLOCK_TIMER_isr		_T3Interrupt
	#define CLOCK_TIMER_flag	IFS0bits.T3IF
	
/* CAN */
	#define USE_CAN
	#define CAN_BUF_SIZE	32

/* Réglages UART */
	#define USE_UART1
	#define USE_UART1RXINTERRUPT
	#define UART_RX_BUF_SIZE	12
	
/* Bouton */
 #define BUTTONS_TIMER 4
 #define USE_BUTTONS

/* Définition de la précision et des modes de calcul des sinus et cosinus (cf. maths_home.c/h) */
//	#define FAST_COS_SIN
//	#define COS_SIN_16384



#if defined(PREDEF_BUILD_KRUSTY)
# ifndef I_AM_ROBOT_KRUSTY
#  define I_AM_ROBOT_KRUSTY
# endif
# ifdef I_AM_ROBOT_TINY
#  undef I_AM_ROBOT_TINY
# endif
#elif defined(PREDEF_BUILD_TINY)
# ifndef I_AM_ROBOT_TINY
#  define I_AM_ROBOT_TINY
# endif
# ifdef I_AM_ROBOT_KRUSTY
#  undef I_AM_ROBOT_KRUSTY
# endif
#endif

//Test et inclusion des configs spécifiques au robot
#if (defined(I_AM_ROBOT_KRUSTY) && defined(I_AM_ROBOT_TINY)) || (!defined(I_AM_ROBOT_KRUSTY) && !defined(I_AM_ROBOT_TINY))
#error "Veuillez définir I_AM_ROBOT_KRUSTY ou I_AM_ROBOT_TINY selon le robot cible."
#endif
#if defined(I_AM_ROBOT_KRUSTY)
#include "./Krusty/KGlobal_config.h"
#endif
#if defined(I_AM_ROBOT_TINY)
#include "./Tiny/TGlobal_config.h"
#endif

#endif /* ndef GLOBAL_CONFIG_H */
