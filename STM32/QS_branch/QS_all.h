/*
 *	Club Robot ESEO 2008 - 2010
 *	Archi-Tech', PACMAN
 *
 *	Fichier : QS_all.h
 *	Package : Qualite Soft
 *	Description : Header necessaire pour tout code du robot
 *	Auteur : Jacen, Gwenn
 *	Version 20100418
 */

#ifndef QS_ALL_H
	#define QS_ALL_H

	/*
	 * - config_global.h est inclu par tout le monde,
	 *    Il devrait y avoir le moins de chose possible.
	 *
	 * - config_qs.h contient la config de la QS.
	 *
	 * - config_pin.h contient la config des entrées sorties et les masques IO (ex: PORT_A_IO_MASK).
	 *
	 * - config_use.h contient des #define activant ou non des parties du code de la carte.
	 *               (Les #define gérant du code de debug ne sont pas ici)
	 *
	 * - config_debug.h contient les #define liés à l'activation de debug
	 *
	 * Les configurations lié à un fichier (comme STACK_SIZE) doivent être mis dans le .h correspondant (comme Stack.h)
	 *
	 */

	#include "../stm32f4xx_hal/stm32f4xx.h"			/* 	Toujours utile...								*/
	#include "../stm32f4xx_hal/stm32f407xx.h"		/* 	Toujours utile...								*/
	#define _ISR							/*  attribut d'interruption non utilisé sous STM32  */

	#include "../config/config_global.h"
	#include "../config/config_use.h"
	#include "../config/config_debug.h"
	#include "../config/config_pin.h"
	#include "../config/config_global_vars.h"

	#include "QS_types.h"					/* Nos types										*/
	#include "QS_macro.h"					/* Quelques macros pratiques						*/
	#include "QS_measure.h"					/* Les defines importants							*/

	#ifdef VERBOSE_MODE
		#include <stdio.h>					/* pour beneficier de printf en test				*/
	#endif /* def VERBOSE_MODE */

	#ifndef NULL
		#define NULL 0
	#endif

#endif /* ndef QS_ALL_H */
