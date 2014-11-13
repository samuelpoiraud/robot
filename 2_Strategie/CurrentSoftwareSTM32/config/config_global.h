/*
 *  Club Robot ESEO 2008 - 2014
 *
 *  $Id$
 *
 *  Package : Carte Principale
 *  Description : Configuration globale
 *  Auteur : Jacen
 */

#ifndef CONFIG_GLOBAL_H
#define CONFIG_GLOBAL_H

#ifndef __dsPIC30F__
#	define STM32F40XX
#endif

/*
 * - config_global.h est inclu par tout le monde,
 *    Il devrait y avoir le moins de chose possible.
 *
 * - config_qs.h contient la config de la QS. Il n'est pas inclu par des fichiers de strat et ne doit pas l'�tre.
 *
 * - config_pin.h contient la config des entr�es sorties et les masques IO (ex: PORT_A_IO_MASK).
 *    Il est inclu par les fichiers de strat qui ont besoin d'acc�der � un port.
 *
 * - config_use.h contient des #define activant ou non des parties du code de la carte.
 *               (Les #define g�rant du code de debug ne sont pas ici)
 *
 * - config_debug.h contient les #define li�s � l'activation de debug
 *
 * Les configurations li� � un fichier (comme STACK_SIZE) doivent �tre mis dans le .h correspondant (comme Stack.h)
 *
 */

#endif /* ndef GLOBAL_CONFIG_H */
