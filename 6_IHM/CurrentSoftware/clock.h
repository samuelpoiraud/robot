/*
 *  Club Robot ESEO 2014
 *
 *  Fichier : clock.h
 *  Package : IHM
 *  Description : Gestion de l'IT pour la clock
 *  Auteur : Arnaud
 *  Version 201203
 */

#ifndef _CLOCK_H
#define _CLOCK_H

	#include "QS/QS_all.h"

	void CLOCK_init(void);
	void _ISR _T2Interrupt(void); //Sur interruption timer 2...

#endif	//def _CLOCK_H
