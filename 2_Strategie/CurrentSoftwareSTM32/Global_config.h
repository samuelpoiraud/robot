/*
 *  Club Robot ESEO 2008 - 2010
 *  Archi'Tech, PACMAN
 *
 *  Fichier : Global_config.h
 *  Package : Carte Principale
 *  Description : Configuration des modules de QS
 *  Auteur : Jacen
 *  Version 20110227
 */

#ifndef GLOBAL_CONFIG_H
	#define GLOBAL_CONFIG_H

#define NEW_CONFIG_ORGANISATION
#include "config_global.h"

/*
 * - config_global.h est inclu par tout le monde,
 *    Il devrait y avoir le moins de chose possible.
 *
 * - config_qs.h contient la config de la QS. Il n'est pas inclu par des fichiers de strat et ne doit pas l'être.
 *
 * - config_pin.h contient la config des entrées sorties et les masques IO (ex: PORT_A_IO_MASK).
 *    Il est inclu par les fichiers de strat qui ont besoin d'accéder à un port.
 *
 * - config_use.h contient des #define activant ou non des parties du code de la carte.
 *               (Les #define gérant du code de debug ne sont pas ici)
 *
 * - config_debug.h contient les #define liés à l'activation de debug
 *
 * Les configurations lié à un fichier (comme STACK_SIZE) doivent être mis dans le .h correspondant (comme Stack.h)
 *
 */

#endif /* ndef GLOBAL_CONFIG_H */
