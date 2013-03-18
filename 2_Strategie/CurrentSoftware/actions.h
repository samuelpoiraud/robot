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


/********************************************************************************* 
					Actions elementaires specifiques 2013
**********************************************************************************/

	
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
		
	#endif
//A ne pas r�utiliser
void STRAT_Lannion(void);

	#ifdef ACTIONS_C
	
		#include "Asser_functions.h"
		#include "act_functions.h"
		#include "actions_tests.h"

	#endif /* def ACTION_C */

#endif /* ndef ACTIONS_H */
