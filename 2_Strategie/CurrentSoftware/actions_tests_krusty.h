/*
 *	Club Robot ESEO 2008 - 2010
 *	Archi'Tech - CHOMP, Shark & Fish
 *
 *	Fichier : actions_tests_krusty.h
 *	Package : Carte Principale
 *	Description : Test des actions réalisables par le robot
 *	Auteur : Cyril, modifié par Gonsevi
 *	Version 2012/01/14
 */

#include "QS/QS_all.h"

#ifndef ACTIONS_TESTS_KRUSTY_H
#define ACTIONS_TESTS_KRUSTY_H
	
#include "actions.h"
#include "Asser_functions.h"
#include "act_functions.h"
#include "actions_tests_krusty_micro.h"


#ifdef MODE_HOMOLOGATION
/**
* marque les points d'une bouteille et parcours ensuite les noeuds pour l'évitement
*
*/
//void TEST_STRAT_homolagation();
#endif

#ifdef ACTIONS_TEST_C
 	#include "telemeter.h"
        #include "QS/QS_CANmsgList.h"
#endif /* def ACTION_TEST_C */

void TEST_STRAT_avoidance(void);
void TEST_STRAT_kdo(void);
void TEST_STRAT_assiettes(void);

void TEST_STRAT_assiettes_evitement(void);
void TEST_Launcher_ball(void);
#endif /* ACTIONS_TESTS_KRUSTY_H */
