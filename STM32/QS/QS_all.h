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

	#include "stm32f4xx.h"					/* 	Toujours utile...								*/
	#define _ISR							/*  attribut d'interruption non utilisé sous STM32  */

	#include "../config/config_global.h"
	#include "../config/config_use.h"
	#include "../config/config_debug.h"

	#include "QS_types.h"					/*	Nos types										*/
	#include "QS_macro.h"					/*	quelques macro pratiques						*/
	#include "QS_global_vars.h"				/*	Les variables globales (notamment les drapeaux) */
	#include "QS_measure.h"					/* Les defines importants							*/

	#ifdef VERBOSE_MODE
		#include <stdio.h>					/* pour beneficier de printf en test				*/
	#endif /* def VERBOSE_MODE */

	#ifndef NULL
		#define NULL 0
	#endif

#endif /* ndef QS_ALL_H */
