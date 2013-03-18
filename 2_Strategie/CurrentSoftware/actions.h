/*
 *	Club Robot ESEO 2008 - 2010
 *	Archi'Tech - CHOMP
 *
 *	Fichier : actions.h
 *	Package : Carte Principale
 *	Description : Actions réalisables par le robot sur commande de brain.c
 *	Auteur : Jacen (Modifié par Julien)
 *	Version 20110313
 */

#include "QS/QS_all.h"

/*	Toutes les fonctions suivantes ont des machines d'état interne à réinitialiser
 *	entre chaque utilisation. Cette réinitialisation est automatique et s'effectue
 *	chaque fois que la fonction renvoie un signal de fin, c'est à dire NOT_HANDLED
 *	END_OK ou END_WITH_TIMEOUT (à l'écriture de ce commentaire. Veuillez consulter
 *	les commentaires des actions, qui devraient préciser les valeurs de retour des
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
		 * pre : être à la coupe
		 * return IN_PROGRESS : En cours
		 * return END_OK : Terminé
		 * return NOT_HANDLED : Action impossible
		 * return END_WITH_TIMEOUT : Timeout
		 *
		 * post : robot homologué
		 */
		
	#endif
//A ne pas réutiliser
void STRAT_Lannion(void);

	#ifdef ACTIONS_C
	
		#include "Asser_functions.h"
		#include "act_functions.h"
		#include "actions_tests.h"

	#endif /* def ACTION_C */

#endif /* ndef ACTIONS_H */
