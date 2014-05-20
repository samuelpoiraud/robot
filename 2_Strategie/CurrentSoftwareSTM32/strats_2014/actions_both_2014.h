/*
 *	Club Robot ESEO 2013-2014
 *	Pierre & Guy
 *
 *	Fichier : actions_both_2014.h
 *	Package : Carte S�/strats2014
 *	Description : Test des actions r�alisables par le robot
 *	Auteur : Herzaeone, modifi� .
 *	Version 2013/10/03
 */

#include "../QS/QS_all.h"

#ifndef ACTIONS_BOTH_2014_H
#define ACTIONS_BOTH_2014_H

#include "../asser_functions.h"
#include "../act_functions.h"

void strat_reglage_odo_rotation(void);
void strat_reglage_odo_translation(void);
void strat_reglage_odo_symetrie(void);

void TEST_pathfind(void);

void test_strat_robot_virtuel(void);

void test_Pathfind(void);

void test_strat_robot_virtuel_with_avoidance(void);

void strat_reglage_asser(void);

error_e sub_wait(Sint16 x, Sint16 y);

#endif
