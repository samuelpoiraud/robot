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
	void any_match(ia_fun_t, time32_t match_duration);

	#ifdef BRAIN_C
		//Pourquoi tant de complication ? Les includes qui étaient ici sont dans 'brain.c'. mailto:alexis.murzeau@reseau.eseo.fr si jamais yavais une raison valable pour mettre les includes uniquement utilisés par le .c ici car je vois pas pourquoi a l'instant ou j'écris cette ligne de commentaire de 15 km.
	#endif /* def BRAIN_C */
	
#endif /* ndef BRAIN_H */
