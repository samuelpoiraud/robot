/*
 *	Club Robot ESEO 2008 - 2012
 *	Archi'Tech, CHOMP, CheckNorris, Shark & Fish
 *
 *	Fichier : brain.h
 *	Package : Carte Principale
 *	Description : 	Fonctions de g�n�ration des ordres
 *	Auteur : Jacen, modifi� par Gonsevi
 *	Version 2012/01/14
 */

#include "QS/QS_all.h"

#ifndef BRAIN_H
	#define BRAIN_H

	/* Les procedures d'ia possibles : */
	typedef void(*ia_fun_t)(void);


	/* 	execute un match de match_duration secondes � partir de la 
		liberation de la biroute. Arrete le robot � la fin du match.
		Appelle une autre routine pour l'IA pendant le match.
	*/
	void any_match(void);

	//Fonction qui lit la strat choisie: FDP2013
	Uint8 strat_number();
	
#endif /* ndef BRAIN_H */
