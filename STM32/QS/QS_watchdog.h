/*
 *  Club Robot ESEO 2009 - 2010
 *  Chomp
 *
 *  Fichier : QS_watchdog.h
 *  Package : Qualit� Soft
 *  Description : Timers abstraits / watchdog
 *  Auteur : Gwenn, Jacen
 *	Licence : CeCILL-C (voir LICENCE.txt)
 *  Version 20100620
 */

/* 	Ce module ne supporte les cr�ations de Watchdog en IT que sur les IT :
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
		* param t Temps avant ex�cution, en millisecondes, avec t > QUANTUM
		* param g Fonction appell�e au timeout, de prototype void foobar(void);
		* return Identifiant du watchdog pour d�sarmement, 255 si �chec
	 **/
	watchdog_id_t WATCHDOG_create(timeout_t t, watchdog_callback_fun_t f, bool_e is_periodic);

	/** Ajoute un watchdog :
		* param t Temps avant ex�cution, en millisecondes, avec t > QUANTUM
		* param g Flag mis � "TRUE" au timeout, de type bool_e
		* return Identifiant du watchdog pour d�sarmement, 254 si �chec
	 **/
	watchdog_id_t WATCHDOG_create_flag(timeout_t t, volatile bool_e* f);

	/** D�sarme un watchdog
		* param id Identifiant du watchdog � annuler
	 **/
	void WATCHDOG_stop(watchdog_id_t id);

	/** Desactive le d�clenchement du watchdog lors d'un timeout.
	 * Le declenchement est effectu� lorsqu'il sera r�activ� si le timeout arrive alors que le watchdog est d�sactiv�
	 */
	void WATCHDOG_disable_timeout(watchdog_id_t id);

	/** R�active le declenchement du watchdog lors d'un timeout.
	 */
	void WATCHDOG_enable_timeout(watchdog_id_t id);
	
	#endif /* def USE_WATCHDOG */
#endif
