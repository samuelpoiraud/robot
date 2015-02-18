/*
 *  Club Robot ESEO 2014
 *
 *  Fichier : it.h
 *  Package : IHM
 *  Description : Routine d'interruption ex�cut�e en IT (timer 5ms)
 *  Auteur : Anthony inspir� du code de la prop
 *  Version 201203
 */

#ifndef _IT_H
#define _IT_H

	#include "QS/QS_all.h"

	//IT Timer 1, ex�cut�e toutes les 10ms
	//ATTENTION : 10ms pr�cisement, pas de printf dans toutes les fonctions li�es !!!
	//void _ISR _T1Interrupt(void); juste pour rappel, pas a mettre dans le code

	void IT_init(void);
	void _ISR _T1Interrupt(void); //Sur interruption timer 1...

#endif	//def _IT_H
