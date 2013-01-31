/*
 *	PFE Simulateur QSx86 2011-2012
 *	Logiciel EVE
 *
 *	Fichier : EVE_all.h
 *	Package : EVE_QS
 *	Description : Header à inclure dans tous les codes
 *	Auteur : Julien Franchineau & François Even
 *	Version 20111208
 */

#ifndef EVE_ALL_H
	#define EVE_ALL_H

	// Librairies C	
	#include <stdio.h>
	#include <unistd.h>

	#include <stdlib.h>
	// Include pour les Threads
	#include <pthread.h>

	// Nouveaux types et liste des erreurs
	#include "EVE_types.h"

	// Fonctions générales très utiles
	#include "EVE_functions.h"

	// Fonctions de communication entre processus
	#include "EVE_messages.h"

	// Inclusion des variables globales
	#include <gtk/gtk.h>
	#include "../global_vars.h"

#endif /* EVE_ALL_H */

