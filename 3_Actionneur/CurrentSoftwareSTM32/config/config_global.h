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

/** Choix du robot cible. Une seule d�finition n'est possible, commentez l'autre.
 * Si I_AM_ROBOT_BIG est d�fini, le code sp�cifique au robot est pris dans le dossier BIG.
 * Si I_AM_ROBOT_SMALL est d�fini, le code  s�pcifique au robot est pris dans le dossier SMALL.
 * Pour ceux qui veulent, il est possible de pr�d�finir PREDEF_BUILD_SMALL ou PREDEF_BUILD_BIG avec le switch -DPREDEF_BUILD_SMALL ou -DPREDEF_BUILD_BIG de gcc, permet la creation de plusieurs target dans mplab X au moins.
 * PREDEF_BUILD_SMALL et PREDEF_BUILD_BIG sont prioritaire sur la d�finition manuelle.
 */

#define I_AM_ROBOT_BIG
//#define I_AM_ROBOT_SMALL


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
