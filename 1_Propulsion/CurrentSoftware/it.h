/*
 *  Club Robot ESEO 2008 - 2012
 *
 *  Fichier : it.h
 *  Package : Asser
 *  Description : Routine d'interruption exécutée en IT (timer 5ms)
 *  Auteur : Nirgal 2009, inspiré par le code de Val' 2007
 *  Version 201203
 */
 
#ifndef _IT_H
	#define _IT_H

	#include "QS/QS_all.h"
	
	//IT Timer 1, exécutée toutes les 5ms, où la mise à jour du point fictif, le correcteur PD et l'odométrie sont calculés..
	//ATTENTION : 5ms précisement, pas de printf dans toutes les fonctions liées !!!
	//void _ISR _T1Interrupt(void); juste pour rappel, pas a mettre dans le code

		void IT_init(void);

	void PILOT_referential_init(void);
	
	
	
	
	#ifdef X86	
		void IT_software_interrupt_routine(void);
	#else
		void _ISR _T1Interrupt(void); //Sur interruption timer 1...
	#endif
	
#endif	//def _IT_H
