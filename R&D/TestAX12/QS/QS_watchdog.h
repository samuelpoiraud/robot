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

/* 	Ce module ne supporte les créations de Watchdog en IT que sur les IT :
	INT0 IC1 IC2 OC1 OC2 T1 T2 T3 SPI1 U1RX U1TX AD NVM SI2C MI2C et CN */

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
	
	#endif /* def USE_WATCHDOG */
#endif
