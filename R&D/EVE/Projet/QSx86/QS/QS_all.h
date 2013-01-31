/*
 *	PFE Simulateur QSx86 2011-2012
 *	Logiciel EVE
 *
 *	Fichier : QS_all.h
 *	Package : QSx86
 *	Description : Header nécessaire pour tout code du robot
 *	Auteur : Julien Franchineau & François Even
 *	Version 20111206
 */

#ifndef QS_ALL_H
	#define QS_ALL_H

	// On utilise les cartes avec la QS adaptée pour EVE
	#define USE_QSx86

	#include "EVE_config.h"
	#include "EVE_QS/EVE_all_x86.h"			// Fonctions QS EVE 
	#include "QS_EVE_manager.h"				// Fonctions EVE pour la nouvelle QSx86 des cartes


	#include "../Global_config.h"	/*	On charge d'abord la configuration globale		*/
	#include "QS_configCheck.h"				/*	On verifie que le programmeur a selectionné les	*/
											/*	elements indispensables 						*/

	#include "QS_types.h"					/*	Nos types										*/
	#include "QS_macro.h"					/*	quelques macro pratiques						*/
	#include "QS_global_vars.h"				/*	Les variables globales (notamment les drapeaux) */
	
	#ifdef VERBOSE_MODE
		#include <stdio.h>					/* pour beneficier de printf en test				*/
		#include <string.h>					// Rajout pour la version QSx86
	#endif /* def VERBOSE_MODE */

	#ifndef NULL
		#define NULL 0
	#endif
#endif /* ndef QS_ALL_H */
