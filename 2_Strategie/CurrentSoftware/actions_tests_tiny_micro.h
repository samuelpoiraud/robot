/*
 *	Club Robot ESEO 2008 - 2010
 *	Archi'Tech - CHOMP, Shark & Fish
 *
 *	Fichier : actions_tests_tiny_micro.h
 *	Package : Carte Principale
 *	Description : Test des actions r�alisables par le robot
 *	Auteur : Cyril, modifi� par Gonsevi
 *	Version 2012/01/14
 */

#include "QS/QS_all.h"

#ifndef ACTIONS_TESTS_TINY_MICRO_H
#define ACTIONS_TESTS_TINY_MICRO_H

#include "actions.h"
#include "Asser_functions.h"
#include "act_functions.h"


#ifdef ACTIONS_TEST_C
        #include "QS/QS_CANmsgList.h"
#endif /* def ACTION_TEST_C */

//Ins�rez ici vos micro_strat et d�coupes de strat
void T_BALLINFLATER_start(void);

#endif /* ACTIONS_TESTS_TINY_MICRO_H */




