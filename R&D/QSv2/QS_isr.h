/*
 *  Club Robot ESEO 2009 - 2010
 *  Archi-Tech'
 *
 *  Fichier : QS_isr.h
 *  Package : Qualit� Soft
 *  Description : G�n�ralit�s sur les interruptions.
 *  Auteur : Jacen
 *  Version 20090808
 */

#ifndef QS_ISR_H
	#define QS_ISR_H

	#include "QS_all.h"

	/*
	 *	D�fini si l'IT doit �tre aquit�e avant ou apr�s
	 *	l'execution de la routine d'interruption.
	 *	On peut �galement acquiter avant et apr�s.
	 *	Si vous ne savez pas quoi choisir, demandez une expliquation
	 *	� quelqu'un qui sait. Si cette personne n'est pas disponible,
	 *	acquittez apr�s pour le moment. La plupart du temps ce n'est
	 *	pas important.
	 */
	typedef enum
	{
		AQUITE_IT_BEFORE_ISR=1,
		AQUITE_IT_AFTER_ISR=2,
		AQUITE_IT_BEFORE_AND_AFTER_IT=3
	}acquaintance_mode_e; 

	/*
	 *	D�finit le type des routines d'interruption execut�es
	 *	sur IT timer. Le type est void function(void);
	 */
	typedef void(*isr_fun_t)();

#endif /* ndef QS_ISR_H */
