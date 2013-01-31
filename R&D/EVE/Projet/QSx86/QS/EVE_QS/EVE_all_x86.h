/*
 *	PFE Simulateur QSx86 2011-2012
 *	Logiciel EVE
 *
 *	Fichier : EVE_all_x86.h
 *	Package : EVE_QS
 *	Description : Header à inclure dans la QSx86
 *	Auteur : Julien Franchineau & François Even
 *	Version 20120113
 */

#ifndef EVE_ALL_H
	#define EVE_ALL_H

	#define USE_QSx86

	// Librairies C	
	#include <stdio.h>
	#include <unistd.h>

	// Nouveaux types et liste des erreurs
	#include "EVE_types.h"

	// Fonctions générales très utiles
	#include "EVE_functions.h"

	// Fonctions de communication entre processus
	#include "EVE_messages.h"

#endif /* EVE_ALL_H */

