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
#include "actions_cake.h"
#include "actions_gifts.h"

void T_BALLINFLATER_start(void);
error_e STRAT_TINY_scan_and_steal_adversary_glasses(bool_e reset);
error_e STRAT_TINY_goto_forgotten_gift(void);
error_e STRAT_TINY_goto_cake_and_blow_candles(void);
error_e TINY_rush();


	typedef struct
	{
		Uint8 nb_glasses;
		Sint16 x_begin;
		Sint16 x_end;
		Sint32 y_middle;
	}girafe_t;
	//Une girafe est un ensemble de verres... au deuxième étage.

bool_e scan_for_glasses(bool_e reset);
girafe_t * look_for_the_best_girafe(void);



#endif /* ACTIONS_TESTS_TINY_MICRO_H */




