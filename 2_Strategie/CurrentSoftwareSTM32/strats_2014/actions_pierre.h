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
#include "../config/config_pin.h"


void strat_inutile(void);
void strat_lannion(void);
void strat_test_point();
void strat_test_fresque();
void strat_tourne_en_rond(void);
error_e strat_manage_fresco();
error_e strat_file_fresco(Sint16 posY);
error_e strat_file_fruit();
error_e strat_test_ramasser_fruit_arbre1_double(bool_e sens);
error_e strat_test_ramasser_fruit_arbre2_double(bool_e sens);
error_e strat_lance_launcher();
error_e strat_lance_launcher_ennemy();
error_e strat_launch_net();

void strat_test_filet();


#endif /* ACTIONS_PIERRE_H_ */
