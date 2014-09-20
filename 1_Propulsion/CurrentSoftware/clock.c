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

void CLOCK_init(){
	bool_e initialized = FALSE;
	if(initialized)
		return;
	initialized = TRUE;

	global.absolute_time = 0;
}

void CLOCK_process_it(){
	global.absolute_time += 10; // +10ms
}
