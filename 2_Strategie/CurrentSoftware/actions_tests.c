/*
 *	Club Robot ESEO 2008 - 2010
 *	Archi'Tech, CHOMP, Shark & Fish
 *
 *	Fichier : actions_tests.c
 *	Package : Carte Principale
 *	Description : Tests des actions réalisables par le robot
 *	Auteur : Cyril, modifié par Gonsevi
 *	Version 2012/01/14
 */

#define ACTIONS_TEST_C

#include "actions_tests.h"


#define DEFAULT_SPEED	(SLOW)

#define USE_CURVE	0



/* ----------------------------------------------------------------------------- */
/* 							Fonctions d'homologation			                 */
/* ----------------------------------------------------------------------------- */


#ifdef MODE_HOMOLOGATION
//match d'homologation
void TEST_STRAT_homolagation()
{
	static enum
	{
		START,
		DO_POINTS,
		AVOIDANCE
	}state = START;

	static error_e sub_action;
	
	switch(state)
	{
		case START: 
			sub_action = TEST_STRAT_leave_home();
			switch (sub_action)
			{
				case END_OK:
					state = DO_POINTS;
					break;	
				case END_WITH_TIMEOUT:
					state = DO_POINTS;
					break;
				case NOT_HANDLED:
					state = DO_POINTS;
					break;
				case IN_PROGRESS:
					break;
				default:
					state = 0;
					break;
			}
			break;
			
		case DO_POINTS:
			sub_action = TEST_STRAT_hit_bottle_deck();
			switch (sub_action)
			{
				case END_OK:
					state = AVOIDANCE;
					break;	
				case END_WITH_TIMEOUT:
					state = AVOIDANCE;
					break;
				case NOT_HANDLED:
					state = AVOIDANCE;
					break;
				case IN_PROGRESS:
					break;
				default:
					state = 0;
					break;
			}
			break;
		case AVOIDANCE:
			TEST_STRAT_all_node();
			break;
	}
}
#endif


/* ----------------------------------------------------------------------------- */
/* 								Stratégies de test                     			 */
/* ----------------------------------------------------------------------------- */

//Fonction permettant de tester l'évitement en faisant le tour du terrain
