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

	#ifndef NEW_CONFIG_ORGANISATION
		#include "../Global_config.h"	/*	On charge d'abord la configuration globale		*/
	#endif

	#ifdef NEW_CONFIG_ORGANISATION
		#include "config_global.h"
		#include "config_qs.h"
	#endif
	
	#include "QS_types.h"					/*	Nos types										*/
	#include "QS_macro.h"					/*	quelques macro pratiques						*/
	#include "QS_global_vars.h"				/*	Les variables globales (notamment les drapeaux) */
	
	#ifdef VERBOSE_MODE
		#include <stdio.h>					/* pour beneficier de printf en test				*/
	#endif /* def VERBOSE_MODE */

	#ifndef NULL
		#define NULL 0
	#endif

#endif /* ndef QS_ALL_H */
