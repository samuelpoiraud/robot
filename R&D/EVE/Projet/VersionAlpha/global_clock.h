/*
 *	PFE Simulateur QSx86 2011-2012
 *	Logiciel EVE
 *
 *	Fichier : global_clock.h
 *	Package : EVE
 *	Description : Horloge générale de gestion du temps
 *	Auteur : Julien Franchineau & François Even
 *	Version 20111215
 */

#ifndef GLOBAL_CLOCK_H
	#define GLOBAL_CLOCK_H

	#include "EVE_QS/EVE_all.h"

	// Macro à passer surement dans le fichier eve.config
	// Durée entre 2 incrémentations de l'horloge générale
	// Cela permet de cadencer l'évolution temporelle de la simulation
	// Dans l'idéal, ce temps est à 1 milliseconde, temps d'exécution réel
	#define GLOBAL_CLOCK_TIME_PROGRESS	1000	// en microsecondes

	// Thread de la gestion du temps général
	void* GLOBAL_CLOCK_thread();

	#ifdef GLOBAL_CLOCK_C

		// Fonction d'écriture pour mettre l'horloge à l'heure
		static Uint16 GLOBAL_CLOCK_update(char* memory_reference, EVE_time_t new_time);

		// Fonction de destruction du temps général
		// En fait, on délie le segment mémoire de ce thread (voir EVE_messages.h)
		// param : memory_reference est l'indicateur du segment mémoire
		// return END_SUCCESS en cas de succès
		// return ERROR_UNBOUND_CLOCK en cas d'échec
		static Uint16 GLOBAL_CLOCK_unbound_memory(char* memory_reference);

		// Gestion des erreurs pour global_clock
		// return la valeur 'error_id' pour faire remonter l'erreur
		static Uint16 GLOBAL_CLOCK_errors(Uint16 error_id, char* commentary);
		
	#endif /* GLOBAL_CLOCK_C */

#endif /* GLOBAL_CLOCK_H */
