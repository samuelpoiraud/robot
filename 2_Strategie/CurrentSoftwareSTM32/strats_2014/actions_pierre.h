/*
 *	Club Robot ESEO 2013 - 2014
 *	Pierre & Guy
 *
 *	Fichier : actions_pierre.h
 *	Package : Carte S�/strats2013
 *	Description : Tests des actions r�alisables par le robot
 *	Auteur : Herzaeone, modifi� par .
 *	Version 2013/10/03
 */

#include "../QS/QS_all.h"

#ifndef ACTIONS_PIERRE_H
#define ACTIONS_PIERRE_H


#include "../asser_functions.h"
#include "../act_functions.h"
#include "../QS/QS_types.h"

void strat_test_triangle_cote_rouge(void);
void strat_test_point();
error_e strat_test_deposser_fruit();
void strat_test_fresque();
void strat_JPO();
error_e strat_manage_fresco();
error_e strat_file_fresco(Sint16 posY);
error_e strat_test_ramasser_fruit_arbre1();
error_e strat_test_ramasser_fruit_arbre2();
error_e strat_test_deposser_fruit_rouge();
error_e strat_test_deposser_fruit_jaune();

#endif /* ACTIONS_PIERRE_H_ */
