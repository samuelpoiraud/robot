/*
 *  Club Robot ESEO 2008 - 2010
 *  Archi-Tech'
 *
 *  Fichier : QS_timer.h
 *  Package : Qualité Soft
 *  Description : Gestion et configuration des timer
 *  Auteur : Jacen
 *  Version 20090808
 */

#ifndef QS_TIMER_H
	#define QS_TIMER_H

	#include "QS_all.h"
	#include "QS_isr.h"

	/*
	 *	Initialise les timers.
	 *	Comme avec tous les modules QS, l'appel de la fonction
	 *	d'init est nécessaire avant toute utilisation d'une autre
	 *	fonction du module.
	 */
	void TIMER_init(void);
	
	/*
	 *	Lance le timer dont le numéro est passé en argument, qui
	 *	doit etre compris entre 0 et 4 inclus. Le timer bouclera
	 *	toutes les period ms. Si une fonction est passée en
	 *	argument isr, elle sera executée en interruption quand le
	 *	timer atteindra la valeur de consigne. L'IT sera acquitée
	 *	conforméménet au mode d'acquitement indiqué. Si 0 est
	 *	passé en tant qu'argument isr, aucune IT ne sera levée.
	 */
	void TIMER_run(	Uint8 timerID, 
				Uint8 period /* en millisecondes */,
				isr_fun_t isr,
				acquaintance_mode_e acquaintance_mode);

	/*
	 *	Même comportement que ci dessus, sauf que la période est
	 *	en microsecondes, et ne doit pas excèder 6553 us.
	 */
	void TIMER_run_us(	Uint8 timerID, 
				Uint16 period /* en microsecondes */,
				isr_fun_t isr,
				acquaintance_mode_e acquaintance_mode);
	
	/*
	 *	Arrete le décompte du timer passé en argument. 
	 *	Désactive l'IT associée, et acquitte l'IT si elle était
	 *	levée.
	 */
	void TIMER_stop(Uint8 timerID);

	/*
	 *	Ces deux fonctions permettent de lire le contenu du timer
	 *	passé en argument. Si le timer décompte en ms, la
	 *	première indiquera le compte en ms. Si le timer a été lancé
	 *	en microsecondes, la seconde reverra le compte en us.
	 *	L'utilisation croisée d'une fonction en ms et en us
	 *	renverra des résultats erronés.
	 */
	Uint8 TIMER_read(Uint8 timerID);
	Uint16 TIMER_read_us(Uint8 timerID);
#endif
