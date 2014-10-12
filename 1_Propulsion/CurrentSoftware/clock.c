/*
 *	Club Robot ESEO 2008 - 2009
 *	Archi'Tech
 *
 *	Fichier : clock.c
 *	Package : Carte Principale
 *	Description : 	Fonctions de gestion de l'horloge
 *	Auteur : Jacen
 *	Version 20090322
 */

#include "clock.h"
#include "QS/QS_timer.h"

#define CLOCK_IT_MS		1

void CLOCK_init(){
	bool_e initialized = FALSE;
	if(initialized)
		return;
	initialized = TRUE;

	global.absolute_time = 0;

	TIMER_init();
	TIMER5_run_us(1000*CLOCK_IT_MS);
}

void _ISR _T5Interrupt(void){
	global.absolute_time += CLOCK_IT_MS;
	TIMER5_AckIT();
}
