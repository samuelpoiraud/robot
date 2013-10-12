/*
 *  Club Robot ESEO 2008 - 2014
 *
 *  $Id: config_global.h 904 2013-10-12 12:04:27Z amurzeau $
 *
 *  Package : Carte Actionneur
 *  Description : Configuration globale
 *  Auteur : Jacen
 */

#ifndef CONFIG_GLOBAL_H
#define CONFIG_GLOBAL_H

#ifndef __dsPIC30F__
#	define STM32F40XX
#endif

/** Choix du robot cible. Une seule définition n'est possible, commentez l'autre.
 * Si I_AM_ROBOT_KRUSTY est défini, le code spécifique au robot est pris dans le dossier Krusty.
 * Si I_AM_ROBOT_TINY est défini, le code  sépcifique au robot est pris dans le dossier Tiny.
 * Pour ceux qui veulent, il est possible de prédéfinir PREDEF_BUILD_TINY ou PREDEF_BUILD_KRUSTY avec le switch -DPREDEF_BUILD_TINY ou -DPREDEF_BUILD_KRUSTY de gcc, permet la creation de plusieurs target dans mplab X au moins.
 * PREDEF_BUILD_TINY et PREDEF_BUILD_KRUSTY sont prioritaire sur la définition manuelle.
 */
//#define I_AM_ROBOT_KRUSTY
#define I_AM_ROBOT_TINY



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

#endif /* ndef GLOBAL_CONFIG_H */
