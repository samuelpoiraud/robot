/*
 *	Club Robot ESEO 2013 - 2014
 *	Pierre & Guy
 *
 *	Fichier : actions_pierre.h
 *	Package : Carte S²/strats2013
 *	Description : Tests des actions réalisables par le robot
 *	Auteur : Herzaeone, modifié par .
 *	Version 2013/10/03
 */

#include "../QS/QS_all.h"

#ifndef ACTIONS_PIERRE_H
#define ACTIONS_PIERRE_H


#include "../asser_functions.h"
#include "../act_functions.h"

void strat_test_triangle_cote_rouge(void);
void strat_test_point();
error_e strat_test_ramasser_fruit_arbre1();
error_e strat_test_ramasser_fruit_arbre1A();
error_e strat_test_ramasser_fruit_arbre1B();
error_e strat_test_ramasser_fruit_arbre2();
error_e strat_test_deposser_fruit_rouge();
error_e strat_test_deposser_fruit_jaune();

#endif /* ACTIONS_PIERRE_H_ */
