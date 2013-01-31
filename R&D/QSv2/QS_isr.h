/*
 *  Club Robot ESEO 2009 - 2010
 *  Archi-Tech'
 *
 *  Fichier : QS_isr.h
 *  Package : Qualité Soft
 *  Description : Généralités sur les interruptions.
 *  Auteur : Jacen
 *  Version 20090808
 */

#ifndef QS_ISR_H
	#define QS_ISR_H

	#include "QS_all.h"

	/*
	 *	Défini si l'IT doit être aquitée avant ou après
	 *	l'execution de la routine d'interruption.
	 *	On peut également acquiter avant et après.
	 *	Si vous ne savez pas quoi choisir, demandez une expliquation
	 *	à quelqu'un qui sait. Si cette personne n'est pas disponible,
	 *	acquittez après pour le moment. La plupart du temps ce n'est
	 *	pas important.
	 */
	typedef enum
	{
		AQUITE_IT_BEFORE_ISR=1,
		AQUITE_IT_AFTER_ISR=2,
		AQUITE_IT_BEFORE_AND_AFTER_IT=3
	}acquaintance_mode_e; 

	/*
	 *	Définit le type des routines d'interruption executées
	 *	sur IT timer. Le type est void function(void);
	 */
	typedef void(*isr_fun_t)();

#endif /* ndef QS_ISR_H */
