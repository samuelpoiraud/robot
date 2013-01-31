/*
 *	PFE Simulateur QSx86 2011-2012
 *	Logiciel EVE
 *
 *	Fichier : manager.h
 *	Package : EVE
 *	Description : Gestionnaire des threads et processus
 *	Auteur : Julien Franchineau
 *	Version 20111201
 */

#ifndef MANAGER_H
	#define MANAGER_H

	#include "EVE_QS/EVE_all.h"

	// Programme principal de gestion de threads
	Uint16 MANAGER_main();

	#ifdef MANAGER_C
		#include <string.h>

		#include "IHM/IHM_display.h"
		#include "IHM/IHM_receiver.h"
		#include "global_clock.h"
		#include "can.h"

		// Macros pour la lecture du fichier config
		#define END_OF_FILE "$END_OF_FILE"
		#define COMPILATION "$USE"

		// Compilation de toutes les cartes demandées, chargement des options
		// return END_SUCCESS si la fonction s'est correctement déroulée
		// return ERROR_CONFIG si le fichier eve.config est manquant
		// return ERROR_MAKEFILE si le fichier Makefile est manquant
		// return ERROR_UNKNOWN_CARD si une carte demandée est inconnue
		// return ERROR_COMPILATION si la compilation contient une erreur
		static Uint16 MANAGER_init();
	
		// Programme de compilation d'une carte
		// return END_SUCCESS si la compilation s'est correctement déroulée
		// return ERROR_UNKNOWN_CARD si la carte demandée n'existe pas
		// return ERROR_COMPILATION si la compilation contient une erreur
		static Uint16 MANAGER_compile(cards_id_e card_id);

		// Permet de lire le fichier de configuration et de charger options
		// return END_SUCCESS si le chargement s'est bien déroulé
		// return ERROR_UNKNOWN_CARD si le fichier config contient une carte inconnue
		static Uint16 MANAGER_load_configuration_file(FILE* config_file);

/********************************************** GESTION DES THREADS **********************************************/
		// Note sur l'initialisation des attributs des threads
		// Par défaut, ils sont en mode normal avec notamment une politique "d'état joignable"
		// Cela nécessite de se synchroniser avec le thread pour correctement le terminer
		// Ici, on souhaite le rendre "détachable", et pouvoir le lancer sans plus jamais s'en préoccuper
		// Il se terminera quand le signal d'arrêt arrivera de l'IHM directement
		// On utilise le paramètre 'attributes' pour réaliser cet effet

/**** GESTION DES THREADS CARTES ****/
		// Permet de lancer les différents threads liés aux cartes
		// return END_SUCCESS en cas de succès de démarrage de tous les threads requis
		// return ERROR_THREAD_CREATION si le thread n'a pas pu être créé
		static Uint16 MANAGER_cards_threads();

		// Thread de gestion de la carte ACTIONNEURS
		static void* MANAGER_actionneurs_thread();

		// Thread de gestion de la carte BALISES
		static void* MANAGER_balises_thread();

		// Thread de gestion de la carte PROPULSION
		static void* MANAGER_propulsion_thread();

		// Thread de gestion de la carte STRATEGIE
		static void* MANAGER_strategie_thread();

		// Thread de gestion de la carte SUPERVISION
		static void* MANAGER_supervision_thread();

/**** GESTION DES THREADS AUTRES ****/
		// Thread de gestion de GLOBAL CLOCK
		static Uint16 MANAGER_global_clock_thread();
		
		// Thread de gestion du CAN
		static Uint16 MANAGER_can_thread();

		//Thread de gestion de IHM
		static Uint16 MANAGER_ihm_thread();

		// Gestion des retours d'erreurs
		// return la valeur 'error_id' permettant de la faire remonter
		static Uint16 MANAGER_errors(int error_id, char* commentary);

	#endif /* MANAGER_C */


#endif /* MANAGER_H */

