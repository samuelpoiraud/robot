/*
 *  Club Robot ESEO 2008 - 2010
 *  Archi-Tech'
 *
 *  Fichier : QS_timer.h
 *  Package : Qualit� Soft
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
	 *	d'init est n�cessaire avant toute utilisation d'une autre
	 *	fonction du module.
	 */
	void TIMER_init(void);
	
	/*
	 *	Lance le timer dont le num�ro est pass� en argument, qui
	 *	doit etre compris entre 0 et 4 inclus. Le timer bouclera
	 *	toutes les period ms. Si une fonction est pass�e en
	 *	argument isr, elle sera execut�e en interruption quand le
	 *	timer atteindra la valeur de consigne. L'IT sera acquit�e
	 *	conform�m�net au mode d'acquitement indiqu�. Si 0 est
	 *	pass� en tant qu'argument isr, aucune IT ne sera lev�e.
	 */
	void TIMER_run(	Uint8 timerID, 
				Uint8 period /* en millisecondes */,
				isr_fun_t isr,
				acquaintance_mode_e acquaintance_mode);

	/*
	 *	M�me comportement que ci dessus, sauf que la p�riode est
	 *	en microsecondes, et ne doit pas exc�der 6553 us.
	 */
	void TIMER_run_us(	Uint8 timerID, 
				Uint16 period /* en microsecondes */,
				isr_fun_t isr,
				acquaintance_mode_e acquaintance_mode);
	
	/*
	 *	Arrete le d�compte du timer pass� en argument. 
	 *	D�sactive l'IT associ�e, et acquitte l'IT si elle �tait
	 *	lev�e.
	 */
	void TIMER_stop(Uint8 timerID);

	/*
	 *	Ces deux fonctions permettent de lire le contenu du timer
	 *	pass� en argument. Si le timer d�compte en ms, la
	 *	premi�re indiquera le compte en ms. Si le timer a �t� lanc�
	 *	en microsecondes, la seconde reverra le compte en us.
	 *	L'utilisation crois�e d'une fonction en ms et en us
	 *	renverra des r�sultats erron�s.
	 */
	Uint8 TIMER_read(Uint8 timerID);
	Uint16 TIMER_read_us(Uint8 timerID);
#endif
