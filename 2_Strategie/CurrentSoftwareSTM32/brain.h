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

	void BRAIN_init(void);

	/* 	execute un match de match_duration secondes à partir de la 
		liberation de la biroute. Arrete le robot à la fin du match.
		Appelle une autre routine pour l'IA pendant le match.
	*/
	void any_match(void);


	bool_e BRAIN_get_strat_updated(void);
	char * BRAIN_get_current_strat_name(void);
	ia_fun_t BRAIN_get_current_strat_function(void);

	void BRAIN_set_strategy(ia_fun_t function);
	ia_fun_t BRAIN_get_strat_function(Uint8 i);
	char * BRAIN_get_strat_name(Uint8 i);
	Uint8 BRAIN_get_number_strategy();


#endif /* ndef BRAIN_H */
