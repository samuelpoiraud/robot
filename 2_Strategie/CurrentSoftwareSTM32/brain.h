/*
 *	Club Robot ESEO 2008 - 2012
 *	Archi'Tech, CHOMP, CheckNorris, Shark & Fish
 *
 *	Fichier : brain.h
 *	Package : Carte Principale
 *	Description : 	Fonctions de génération des ordres
 *	Auteur : Jacen, modifié par Gonsevi
 *	Version 2012/01/14
 */

#include "QS/QS_all.h"

#ifndef BRAIN_H
	#define BRAIN_H

	/* Les procedures d'ia possibles : */
	typedef void(*ia_fun_t)(void);


	/* 	execute un match de match_duration secondes à partir de la 
		liberation de la biroute. Arrete le robot à la fin du match.
		Appelle une autre routine pour l'IA pendant le match.
		Une durée de 0 indique un match infini
	*/
	void any_match(time32_t match_duration);

	//Fonction qui lit la strat choisie: FDP2013
	Uint8 strat_number();
	
#endif /* ndef BRAIN_H */
