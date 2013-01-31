/*
 *  Club Robot ESEO 2009 - 2010
 *  Chomp
 *
 *  Fichier : QS_watchdog.h
 *  Package : Qualit� Soft
 *  Description : Timers abstraits / watchdog sur timer
 *  Auteur : Gwenn
 *  Version 20100415
 */

#ifndef QS_WATCHDOG_H
	#define QS_WATCHDOG_H

	#include "QS_all.h"

	#ifdef USE_WATCHDOG

	#include "QS_timer.h"
	
	/* N�cessite de param�trer WATCHDOG_TIMER dans Global_config.h
		* Utilisable sur TIM2, TIM5, TIM6 ou TIM7
	*/	
	
	/* Param�trage */
	#define QUANTUM 25						// Granularit� du temps
	#define WATCHDOG_MAX_COUNT 16			// Nombre maximal de watchdogs en ex�cution simultann�e
	
	/* Types */
	typedef Uint16 timeout_t;
	typedef Uint8 watchdog_id_t;
	typedef void(*func_ptr_t)(void);
	
	
	/* Initialise les watchdogs */
	void WATCHDOG_init(void);
	
	
	/** Ajoute un watchdog : x = WATCHDOG_create(4200, foobar);
		* param t Temps avant ex�cution, en millisecondes, avec t > QUANTUM
		* param g Fonction appell�e au timeout, de prototype void foobar(void);
		* return Identifiant du watchdog pour d�sarmement, 254 si �chec
	 **/
	watchdog_id_t WATCHDOG_create(timeout_t t, func_ptr_t f);
	
	
	/** Ajoute un watchdog : x = WATCHDOG_create(4200, foobar);
		* param t Temps avant ex�cution, en millisecondes, avec t > QUANTUM
		* param g Flag mis � "TRUE" au timeout, de type bool_e
		* return Identifiant du watchdog pour d�sarmement, 254 si �chec
	 **/
	watchdog_id_t WATCHDOG_create_flag(timeout_t t, bool_e* f);
	
	
	/** D�sarme un watchdog
		* param id Identifiant du watchdog � annuler
	 **/
	void WATCHDOG_stop(watchdog_id_t id);
	
	
	/* Variables priv�es */
	#ifdef QS_WATCHDOG_C

		timeout_t 	Watchdog_timeout_list[WATCHDOG_MAX_COUNT];	// Stockage des timeouts
		func_ptr_t	Watchdog_function_list[WATCHDOG_MAX_COUNT];	// Stockage des actions associ�es
		bool_e*		Watchdog_flag_list[WATCHDOG_MAX_COUNT];		// Stockage des flags associ�s
		
	#endif /* def QS_WATCHDOG_C */

	#endif /* def USE_WATCHDOG */
#endif
