/*
 *	Club Robot ESEO 2008 - 2012
 *	Archi'Tech, CHOMP, Shark & Fish, Krusty & Tiny
 *
 *	Fichier : actions_tests_tiny.c
 *	Package : Carte Principale
 *	Description : Tests des actions réalisables par le robot
 *	Auteur : Cyril, modifié par Vincent , Antoine , LMG , Herzaeone , Hoobbes
 *	Version 2012/01/14
 */

#define ACTIONS_TEST_TINY_C

#include "actions_tests_tiny.h"


#define DEFAULT_SPEED	(SLOW)

#define USE_CURVE	0



/* ----------------------------------------------------------------------------- */
/* 							Fonctions d'homologation			                 */
/* ----------------------------------------------------------------------------- */

/*
	Stratégie d'homologation. Marque quelques points faciles.
	Evite l'adversaire à coup sur !
*/
void TEST_STRAT_T_homologation(void)
{
	#warning "TODO stratégie simple d'homologation... reprendre un bout des stratégies plus compliquées"
	/*
		REMARQUE : en homologation, l'idéal est de produire une stratégie semblable à celle visée en match.
		Cependant, si on se prend une remarque, ou qu'un truc foire, il peut être TRES intéressant d'avoir sous le coude
		 une stratégie élémentaire qui permet d'être homologable
		 	- sortir de la zone, marquer 1 point
		 	- éviter correctement un adversaire (donc il faut un minimum de déplacement quand même)
	*/
}


/* ----------------------------------------------------------------------------- */
/* 								Stratégies de test                     			 */
/* ----------------------------------------------------------------------------- */

/* ----------------------------------------------------------------------------- */
/* 							Tests state_machines multiple              			 */
/* ----------------------------------------------------------------------------- */
