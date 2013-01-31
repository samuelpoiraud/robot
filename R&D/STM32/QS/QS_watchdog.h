/*
 *  Club Robot ESEO 2009 - 2010
 *  Chomp
 *
 *  Fichier : QS_watchdog.h
 *  Package : Qualité Soft
 *  Description : Timers abstraits / watchdog sur timer
 *  Auteur : Gwenn
 *  Version 20100415
 */

#ifndef QS_WATCHDOG_H
	#define QS_WATCHDOG_H

	#include "QS_all.h"

	#ifdef USE_WATCHDOG

	#include "QS_timer.h"
	
	/* Nécessite de paramétrer WATCHDOG_TIMER dans Global_config.h
		* Utilisable sur TIM2, TIM5, TIM6 ou TIM7
	*/	
	
	/* Paramétrage */
	#define QUANTUM 25						// Granularité du temps
	#define WATCHDOG_MAX_COUNT 16			// Nombre maximal de watchdogs en exécution simultannée
	
	/* Types */
	typedef Uint16 timeout_t;
	typedef Uint8 watchdog_id_t;
	typedef void(*func_ptr_t)(void);
	
	
	/* Initialise les watchdogs */
	void WATCHDOG_init(void);
	
	
	/** Ajoute un watchdog : x = WATCHDOG_create(4200, foobar);
		* param t Temps avant exécution, en millisecondes, avec t > QUANTUM
		* param g Fonction appellée au timeout, de prototype void foobar(void);
		* return Identifiant du watchdog pour désarmement, 254 si échec
	 **/
	watchdog_id_t WATCHDOG_create(timeout_t t, func_ptr_t f);
	
	
	/** Ajoute un watchdog : x = WATCHDOG_create(4200, foobar);
		* param t Temps avant exécution, en millisecondes, avec t > QUANTUM
		* param g Flag mis à "TRUE" au timeout, de type bool_e
		* return Identifiant du watchdog pour désarmement, 254 si échec
	 **/
	watchdog_id_t WATCHDOG_create_flag(timeout_t t, bool_e* f);
	
	
	/** Désarme un watchdog
		* param id Identifiant du watchdog à annuler
	 **/
	void WATCHDOG_stop(watchdog_id_t id);
	
	
	/* Variables privées */
	#ifdef QS_WATCHDOG_C

		timeout_t 	Watchdog_timeout_list[WATCHDOG_MAX_COUNT];	// Stockage des timeouts
		func_ptr_t	Watchdog_function_list[WATCHDOG_MAX_COUNT];	// Stockage des actions associées
		bool_e*		Watchdog_flag_list[WATCHDOG_MAX_COUNT];		// Stockage des flags associés
		
	#endif /* def QS_WATCHDOG_C */

	#endif /* def USE_WATCHDOG */
#endif
