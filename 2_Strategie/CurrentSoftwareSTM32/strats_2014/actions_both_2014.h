/*
 *	Club Robot ESEO 2013-2014
 *	Pierre & Guy
 *
 *	Fichier : actions_both_2014.h
 *	Package : Carte S²/strats2014
 *	Description : Test des actions réalisables par le robot
 *	Auteur : Herzaeone, modifié .
 *	Version 2013/10/03
 */

#include "../QS/QS_all.h"

#ifndef ACTIONS_BOTH_2014_H
#define ACTIONS_BOTH_2014_H

#include "../asser_functions.h"
#include "../act_functions.h"

typedef enum{
	OUR_FIRES,
	ADVERSARY_FIRES
}protected_fires_e;

void strat_tourne_en_rond(void);
void strat_reglage_odo_rotation(void);
void strat_reglage_odo_translation(void);
void strat_reglage_odo_symetrie(void);

void TEST_pathfind(void);

void test_strat_robot_virtuel(void);

void test_Pathfind(void);

void test_strat_robot_virtuel_with_avoidance(void);

void strat_reglage_asser(void);

error_e sub_wait(Sint16 x, Sint16 y);
error_e protected_fires(protected_fires_e fires);
error_e action_recalage_x(way_e sens, Sint16 angle, Sint16 wanted_x);
error_e action_recalage_y(way_e sens, Sint16 angle, Sint16 wanted_y);

#endif
