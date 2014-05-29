/*
 *  Club Robot ESEO 2008 - 2014
 *
 *  $Id$
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
 * Si I_AM_ROBOT_BIG est défini, le code spécifique au robot est pris dans le dossier BIG.
 * Si I_AM_ROBOT_SMALL est défini, le code  sépcifique au robot est pris dans le dossier SMALL.
 * Pour ceux qui veulent, il est possible de prédéfinir PREDEF_BUILD_SMALL ou PREDEF_BUILD_BIG avec le switch -DPREDEF_BUILD_SMALL ou -DPREDEF_BUILD_BIG de gcc, permet la creation de plusieurs target dans mplab X au moins.
 * PREDEF_BUILD_SMALL et PREDEF_BUILD_BIG sont prioritaire sur la définition manuelle.
 */
#define I_AM_ROBOT_BIG
//#define I_AM_ROBOT_SMALL


//Compatibilité avec les fichiers qui utilise l'ancien #define pour KRUSTY et TINY
#ifdef I_AM_ROBOT_BIG
#define I_AM_ROBOT_KRUSTY
#endif

#ifdef I_AM_ROBOT_SMALL
#define I_AM_ROBOT_TINY
#endif


#if defined(PREDEF_BUILD_BIG)
# ifndef I_AM_ROBOT_BIG
#  define I_AM_ROBOT_BIG
# endif
# ifdef I_AM_ROBOT_SMALL
#  undef I_AM_ROBOT_SMALL
# endif
#elif defined(PREDEF_BUILD_SMALL)
# ifndef I_AM_ROBOT_SMALL
#  define I_AM_ROBOT_SMALL
# endif
# ifdef I_AM_ROBOT_BIG
#  undef I_AM_ROBOT_BIG
# endif
#endif

#endif /* ndef GLOBAL_CONFIG_H */
