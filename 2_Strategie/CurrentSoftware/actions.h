/*
 *	Club Robot ESEO 2008 - 2010
 *	Archi'Tech - CHOMP
 *
 *	Fichier : actions.h
 *	Package : Carte Principale
 *	Description : Actions r�alisables par le robot sur commande de brain.c
 *	Auteur : Jacen (Modifi� par Julien)
 *	Version 20110313
 */

#include "QS/QS_all.h"

/*	Toutes les fonctions suivantes ont des machines d'�tat interne � r�initialiser
 *	entre chaque utilisation. Cette r�initialisation est automatique et s'effectue
 *	chaque fois que la fonction renvoie un signal de fin, c'est � dire NOT_HANDLED
 *	END_OK ou END_WITH_TIMEOUT (� l'�criture de ce commentaire. Veuillez consulter
 *	les commentaires des actions, qui devraient pr�ciser les valeurs de retour des
 *	actions, au cas par cas.
 */

#ifndef ACTIONS_H
#define ACTIONS_H

#include "avoidance.h"
#include "act_functions.h"

error_e STRAT_debut_match();

void STRAT_match_global();

error_e STRAT_hit_bottle_middle();

void STRAT_money_time();


error_e STRAT_retour_de_bouteille_avec_adversaire_proche();
/********************************************************************************* 
					Actions elementaires specifiques 2012
**********************************************************************************/
	//Prend des cd et des lingots � proximit� de la face sud de notre totem
	error_e STRAT_own_totem_south();
	//Prend des cd et des lingots � proximit� de la face nord de notre totem
	error_e STRAT_own_totem_north();
	//va taper la bouteille num�ro 2, commencant en 1700, 742
	error_e STRAT_hit_bottle_middle();
	
	error_e STRAT_hit_bottle_deck_and_take_own_south_totem();
	
	error_e STRAT_bullion_own_totem_south();
	
	error_e STRAT_bullion_own_totem_north();
	
	void TEST_strat_test();

	error_e rammassage_devant_carte();
	
/* ----------------------------------------------------------------------------- */
/* 								Fonctions d'homologation		  		         */
/* ----------------------------------------------------------------------------- */
	#ifdef MODE_HOMOLOGATION
		/**
		 * Fonction d'homologation du robot
		 * pre : �tre � la coupe
		 * return IN_PROGRESS : En cours
		 * return END_OK : Termin�
		 * return NOT_HANDLED : Action impossible
		 * return END_WITH_TIMEOUT : Timeout
		 *
		 * post : robot homologu�
		 */
		error_e ACTION_homologation();
	#endif

void STRAT_derniere_chance();

	#ifdef ACTIONS_C
	
		#include "Asser_functions.h"
		#include "act_functions.h"
		#include "actions_tests.h"

	#endif /* def ACTION_C */

#endif /* ndef ACTIONS_H */
