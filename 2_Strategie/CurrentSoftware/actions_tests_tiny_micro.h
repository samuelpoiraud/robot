/*
 *	Club Robot ESEO 2008 - 2010
 *	Archi'Tech - CHOMP, Shark & Fish
 *
 *	Fichier : actions_tests_tiny_micro.h
 *	Package : Carte Principale
 *	Description : Test des actions réalisables par le robot
 *	Auteur : Cyril, modifié par Gonsevi
 *	Version 2012/01/14
 */

#include "QS/QS_all.h"

#ifndef ACTIONS_TESTS_TINY_MICRO_H
#define ACTIONS_TESTS_TINY_MICRO_H

#include "actions.h"
#include "Asser_functions.h"
#include "act_functions.h"

//Insérez ici vos micro_strat et découpes de strat


void T_BALLINFLATER_start(void);

error_e TINY_blow_all_candles(void);
error_e TINY_blow_one_candle(Uint8 i, Sint8 way);


error_e TINY_open_all_gifts(void);
error_e TINY_open_all_gifts_without_pause();

error_e old_TINY_all_candles(void);
error_e TINY_open_all_gifts_homolog(void);

error_e TINY_forgotten_gift(map_goal_e forgotten_gift);



error_e TINY_warner_blow_one_candle(bool_e reset);
error_e TINY_warner_around_cake(Sint8 way);
error_e TINY_warner_blow_all_candles(void);

#endif /* ACTIONS_TESTS_TINY_MICRO_H */




