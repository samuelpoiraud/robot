/*
 *	Club Robot ESEO 2010 - 2013
 *	Krusty & Tiny
 *
 *  Fichier : Global_config.h
 *  Package : Actionneur
 *  Description : Configuration des carte actionneur COMMUNE entre les robots
 *  Auteur : Aur�lien
 *  Version 20130208
 *  Robot : Tous
 */

#ifndef GLOBAL_CONFIG_H
	#define GLOBAL_CONFIG_H

	#define VERBOSE_MODE
	#define OUTPUT_LOG
	#define ACT_DEBUG_DEFAULT_MAX_LOG_LEVEL LOG_LEVEL_Debug    //Pour connaitre les valeurs possibles, voir output_log.h (enum log_level_e)

	/* ID de la carte: cf le type cartes_e de QS_types.h */
	#define I_AM CARTE_ACT		//A voir avec Gwenn pour changer
	#define I_AM_CARTE_ACT

	/** Choix du robot cible. Une seule d�finition n'est possible, commentez l'autre.
	 * Si I_AM_ROBOT_KRUSTY est d�fini, le code sp�cifique au robot est pris dans le dossier Krusty.
	 * Si I_AM_ROBOT_TINY est d�fini, le code  s�pcifique au robot est pris dans le dossier Tiny.
	 * Pour ceux qui veulent, il est possible de pr�d�finir PREDEF_BUILD_TINY ou PREDEF_BUILD_KRUSTY avec le switch -DPREDEF_BUILD_TINY ou -DPREDEF_BUILD_KRUSTY de gcc, permet la creation de plusieurs target dans mplab X au moins.
	 * PREDEF_BUILD_TINY et PREDEF_BUILD_KRUSTY sont prioritaire sur la d�finition manuelle.
	 */
	#define I_AM_ROBOT_KRUSTY
	//#define I_AM_ROBOT_TINY
	
	/* Il faut choisir � quelle frequence on fait tourner le PIC */
	#define FREQ_10MHZ

	 // DEFINIR LES PORTS DANS Tiny/Global_config.h ou Krusty/Global_config.h selon le robot !

/* Definition des connectiques capteurs et actionneurs COMMUN � Tiny et Krusty */


/* Actionneurs */
	// Files
	#define CLOCK_TIMER_run		TIMER3_run
	#define CLOCK_TIMER_isr		_T3Interrupt
	#define CLOCK_TIMER_flag	IFS0bits.T3IF
	
/* CAN */
	#define USE_CAN
	#define CAN_BUF_SIZE	32

/* R�glages UART */
	#define USE_UART1
	#define USE_UART1RXINTERRUPT
	#define UART_RX_BUF_SIZE	12
	
/* Bouton */
	#define BUTTONS_TIMER 4
	#define USE_BUTTONS

/* D�finition de la pr�cision et des modes de calcul des sinus et cosinus (cf. maths_home.c/h) */
//	#define FAST_COS_SIN
//	#define COS_SIN_16384


/**************************************************************************************************\
|   Test du robot cible choisi et prise en charge des targets Krusty et Tiny avec PREDEF_BUILD_*   |
\**************************************************************************************************/

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

//Test et inclusion des configs sp�cifiques au robot
#if (defined(I_AM_ROBOT_KRUSTY) && defined(I_AM_ROBOT_TINY)) || (!defined(I_AM_ROBOT_KRUSTY) && !defined(I_AM_ROBOT_TINY))
#error "Veuillez d�finir I_AM_ROBOT_KRUSTY ou I_AM_ROBOT_TINY selon le robot cible. (Voir ligne ~25)"
#endif
#if defined(I_AM_ROBOT_KRUSTY)
#include "./Krusty/KGlobal_config.h"
#endif
#if defined(I_AM_ROBOT_TINY)
#include "./Tiny/TGlobal_config.h"
#endif

#endif /* ndef GLOBAL_CONFIG_H */
