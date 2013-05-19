/*
 *  Club Robot ESEO 2009 - 2010
 *  Chomp
 *
 *  Fichier : QS_watchdog.h
 *  Package : Qualité Soft
 *  Description : Timers abstraits / watchdog
 *  Auteur : Gwenn, Jacen
 *	Licence : CeCILL-C (voir LICENCE.txt)
 *  Version 20100620
 */

#ifndef QS_WATCHDOG_H
	#define QS_WATCHDOG_H

	#include "QS_all.h"

	#ifdef USE_WATCHDOG
	
	/* Types */
	typedef Uint16 timeout_t;
	typedef Uint8 watchdog_id_t;
	typedef void(*watchdog_callback_fun_t)(void);
	
	
	/* Initialise les watchdogs */
	void WATCHDOG_init(void);
	
	
	/** Ajoute un watchdog :
		* param t Temps avant exécution, en millisecondes, avec t > QUANTUM
		* param g Fonction appellée au timeout, de prototype void foobar(void);
		* return Identifiant du watchdog pour désarmement, 255 si échec
	 **/
	watchdog_id_t WATCHDOG_create(timeout_t t, watchdog_callback_fun_t f);

	/** Ajoute un watchdog :
		* param t Temps avant exécution, en millisecondes, avec t > QUANTUM
		* param g Flag mis à "TRUE" au timeout, de type bool_e
		* return Identifiant du watchdog pour désarmement, 254 si échec
	 **/
	watchdog_id_t WATCHDOG_create_flag(timeout_t t, bool_e* f);

	/** Désarme un watchdog
		* param id Identifiant du watchdog à annuler
	 **/
	void WATCHDOG_stop(watchdog_id_t id);

	/** Desactive le déclenchement du watchdog lors d'un timeout.
	 * Le declenchement est effectué lorsqu'il sera réactivé si le timeout arrive alors que le watchdog est désactivé
	 */
	void WATCHDOG_disable_timeout(watchdog_id_t id);

	/** Réactive le declenchement du watchdog lors d'un timeout.
	 */
	void WATCHDOG_enable_timeout(watchdog_id_t id);

	#endif /* def USE_WATCHDOG */
#endif
