/*
 *	Club Robot ESEO 2013 - 2014
 *
 *	Fichier : QS_setTimerSource.h
 *	Package : Qualité Software
 *	Description : Défini des constantes permettant l'utilisation d'un timer parmi les timer dispo et le watchdog
 *	Auteur : Jacen
 *	Licence : CeCILL-C (voir LICENCE.txt)
 *	Version 20130520
 */

#ifndef QS_SETTIMERSOURCE_H_
#define QS_SETTIMERSOURCE_H_

//Constante paramètres:
//TIMER_SRC_ID <id>	Utilisation du timer numéro <id>
//ou
//TIMER_SRC_USE_WATCHDOG Utilisation d'un watchdog (de QS_watchdog). Les interruptions seront moins précises.

//Constante définie par ce header:
//TIMER_SRC_TIMER_init()    Initialisation du timer
//TIMER_SRC_TIMER_interrupt Nom de la fonction appelée lors d'une interruption du timer
//TIMER_SRC_TIMER_resetFlag() Acquittement du flag d'IT
//TIMER_SRC_TIMER_start_ms(period_ms) Démarrage du timer avec période en ms
//TIMER_SRC_TIMER_start_us(period_us) Démarrage du timer avec une période en us. Non disponible lors de l'utilisation du watchdog
//TIMER_SRC_TIMER_stop()    Arret du timer
//TIMER_SRC_TIMER_DisableIT() Désactivation de interruption du timer (mais il continu de compter)
//TIMER_SRC_TIMER_EnableIT() Réactivation de l'interruption du timer


#if (!defined(TIMER_SRC_TIMER_ID) || TIMER_SRC_TIMER_ID > 4 || TIMER_SRC_TIMER_ID < 1) && !defined(TIMER_SRC_USE_WATCHDOG)
	#error "TIMERSRC: TIMER_SRC_TIMER_ID non défini ou invalide, vous devez choisir le numéro du timer entre 1 et 4 inclus, ou utiliser le watchdog avec TIMER_SRC_USE_WATCHDOG"
#elif defined(TIMER_SRC_USE_WATCHDOG)
	#include "QS_watchdog.h"

	static volatile Uint8 TIMER_SRC_watchdog_timeout_id;
	static void TIMER_SRC_watchdog_timeout_recv();

	#define TIMER_SRC_TIMER_init() WATCHDOG_init()
	#define TIMER_SRC_TIMER_interrupt TIMER_SRC_watchdog_timeout_recv
	#define TIMER_SRC_TIMER_resetFlag()
	#define TIMER_SRC_TIMER_start_ms(period_ms) TIMER_SRC_watchdog_timeout_id = WATCHDOG_create(period_ms, &TIMER_SRC_watchdog_timeout_recv, TRUE)
//	#define TIMER_SRC_TIMER_start_us(period_us) TIMER_SRC_watchdog_timeout_id = WATCHDOG_create(period_us/1000, &TIMER_SRC_watchdog_timeout_recv, TRUE)
	#define TIMER_SRC_TIMER_stop() WATCHDOG_stop(TIMER_SRC_watchdog_timeout_id)
	#define TIMER_SRC_TIMER_DisableIT() WATCHDOG_disable_timeout(TIMER_SRC_watchdog_timeout_id)
	#define TIMER_SRC_TIMER_EnableIT() WATCHDOG_enable_timeout(TIMER_SRC_watchdog_timeout_id)
#else
	#include "QS_timer.h"

	//Pour plus d'info sur la concaténation de variable dans des macros, voir http://stackoverflow.com/questions/1489932/c-preprocessor-and-concatenation
	#define TIMER_SRC_TEMP_CONCAT_WITH_PREPROCESS(a,b,c) a##b##c
	#define TIMER_SRC_TEMP_CONCAT(a,b,c) TIMER_SRC_TEMP_CONCAT_WITH_PREPROCESS(a,b,c)

	#define TIMER_SRC_TIMER_interrupt _ISR TIMER_SRC_TEMP_CONCAT(_T, TIMER_SRC_TIMER_ID, Interrupt)
	#define TIMER_SRC_TIMER_resetFlag() TIMER_SRC_TEMP_CONCAT(TIMER, TIMER_SRC_TIMER_ID, _AckIT)()
	#define TIMER_SRC_TIMER_init() TIMER_init()
	#define TIMER_SRC_TIMER_start_ms(period_ms) TIMER_SRC_TEMP_CONCAT(TIMER, TIMER_SRC_TIMER_ID, _run)(period_ms)
	#define TIMER_SRC_TIMER_start_us(period_us) TIMER_SRC_TEMP_CONCAT(TIMER, TIMER_SRC_TIMER_ID, _run_us)(period_us)
	#define TIMER_SRC_TIMER_stop() TIMER_SRC_TEMP_CONCAT(TIMER, TIMER_SRC_TIMER_ID, _stop)()
	#define TIMER_SRC_TIMER_DisableIT() TIMER_SRC_TEMP_CONCAT(TIMER, TIMER_SRC_TIMER_ID, _disableInt)()
	#define TIMER_SRC_TIMER_EnableIT()  TIMER_SRC_TEMP_CONCAT(TIMER, TIMER_SRC_TIMER_ID, _enableInt)()
#endif


#else	//Ce fichier ne devrait jamais être inclu plusieurs fois ! Uniquement dans les fichiers .c qui veulent pouvoir utiliser un timer défini par l'utilisateur
#warning "QS_setTimerSource.h a deja été inclu !!!!"
#endif
