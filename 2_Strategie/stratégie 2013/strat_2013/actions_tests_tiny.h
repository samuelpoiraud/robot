/*
 *	Club Robot ESEO 2008 - 2010
 *	Archi'Tech - CHOMP, Shark & Fish
 *
 *	Fichier : actions_tests_tiny.h
 *	Package : Carte Principale
 *	Description : Test des actions réalisables par le robot
 *	Auteur : Cyril, modifié par Gonsevi
 *	Version 2012/01/14
 */

#include "../QS/QS_all.h"

#ifndef ACTIONS_TESTS_TINY_H
#define ACTIONS_TESTS_TINY_H

#include "actions.h"
#include "../asser_functions.h"
#include "../act_functions.h"
#include "actions_tests_tiny_micro.h"
#include "actions_cake.h"
#include "actions_gifts.h"

void STRAT_TINY_gifts_cake_and_steal(void);
void TEST_STRAT_T_homologation(void);
void STRAT_TINY_test_avoidance_goto_pos_no_dodge_and_wait(void);
void STRAT_TINY_all_candles(void);
void STRAT_TINY_4_gifts_only(void);
void STRAT_TINY_test_steals(void);
void STRAT_TINY_test_moisson(void);

error_e TEST_STRAT_in_da_wall(void);

#endif /* ACTIONS_TESTS_TINY_H */
