/*
 *	Club Robot ESEO 2008 - 2010
 *	Archi-Tech', PACMAN
 *
 *	Fichier : QS_all.h
 *	Package : Qualite Soft
 *	Description : Header necessaire pour tout code du robot
 *	Auteur : Jacen
 *	Licence : CeCILL-C (voir LICENCE.txt)
 *	Version 20100620
 */

#ifndef QS_ALL_H
	#define QS_ALL_H

	#include "../Global_config.h"	/*	On charge d'abord la configuration globale		*/
	#include "QS_configCheck.h"				/*	On verifie que le programmeur a selectionné les	*/
											/*	elements indispensables 						*/

	#include "p30f6010A.h"					/* 	Toujours utile... _ISR redéfini en auto_psv		*/
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
