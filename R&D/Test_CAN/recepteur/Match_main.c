/*
 *	Club Robot ESEO 2008 - 2009
 *	Archi-Tech'
 *
 *	Fichier : Match_main.c
 *	Package : Projet Standard
 *	Description : fonction principale d'exemple pour le projet 
 *				standard construit par la QS pour exemple, pour
 *				utilisation en Match
 *	Auteur : Jacen
 *	Version 20080924
 */

#define MATCH_MAIN_C
#include "Match_main.h"
#ifdef MATCH_MODE
	int main (void)
	{
		ports_init();
		return 0;
	}
#endif /* def MATCH_MODE */
