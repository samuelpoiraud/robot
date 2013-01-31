/*
 *	Club Robot ESEO 2008 - 2009
 *	Archi-Tech'
 *
 *	Fichier : QS_all.h
 *	Package : Qualite Soft
 *	Description : Header necessaire pour tout code du robot
 *	Auteur : Jacen
 *	Version 20091019
 */

#ifndef QS_ALL_H
	#define QS_ALL_H

	#include "../project/Global_config.h"	/*	On charge d'abord la configuration globale		*/

	#ifdef TEST_MODE
		#define VERBOSE_MODE
	#endif

	#include "QS_configCheck.h"				/*	On verifie que le programmeur a selectionné les	*/
											/*	elements indispensables 						*/

	#include "p30f6010a.h"					/* 	Toujours utile... _ISR redéfini en auto_psv		*/

	#include "QS_configBits.h"				/*	configuration interne du pic ajoutée dans le	*/
											/*	fichier definissant QS_GLOBAL_VARS_C 			*/

	#include "QS_types.h"					/*	Nos types										*/
	#include "QS_macro.h"					/*	quelques macro pratiques						*/
	#include "QS_global_vars.h"				/*	Les variables globales (notamment les drapeaux) */

	#define LED_RUN  LATDbits.LATD11
	#define LED_CAN  LATDbits.LATD10
	#define LED_USER LATDbits.LATD9
	#define LED_TEST LATDbits.LATD8

	#ifdef VERBOSE_MODE
		#include <stdio.h>					/* pour beneficier de printf en test				*/
	#endif /* def VERBOSE_MODE */

#endif /* ndef QS_ALL_H */
