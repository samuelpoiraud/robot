/*
 *	Club Robot ESEO 2008 - 2012
 *	Archi'Tech, CHOMP, Shark & Fish, Krusty & Tiny
 *
 *	Fichier : actions_tests_tiny.c
 *	Package : Carte Principale
 *	Description : Tests des actions r�alisables par le robot
 *	Auteur : Cyril, modifi� par Vincent , Antoine , LMG , Herzaeone , Hoobbes
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
	Strat�gie d'homologation. Marque quelques points faciles.
	Evite l'adversaire � coup sur !
*/
void TEST_STRAT_T_homologation(void)
{
	#warning "TODO strat�gie simple d'homologation... reprendre un bout des strat�gies plus compliqu�es"
	/*
		REMARQUE : en homologation, l'id�al est de produire une strat�gie semblable � celle vis�e en match.
		Cependant, si on se prend une remarque, ou qu'un truc foire, il peut �tre TRES int�ressant d'avoir sous le coude
		 une strat�gie �l�mentaire qui permet d'�tre homologable
		 	- sortir de la zone, marquer 1 point
		 	- �viter correctement un adversaire (donc il faut un minimum de d�placement quand m�me)
	*/
}


/* ----------------------------------------------------------------------------- */
/* 								Strat�gies de test                     			 */
/* ----------------------------------------------------------------------------- */

/* ----------------------------------------------------------------------------- */
/* 							Tests state_machines multiple              			 */
/* ----------------------------------------------------------------------------- */
