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
#include "../QS/QS_types.h"
#include "../config/config_pin.h"
#include "../Pathfind.h"

typedef enum{
	HORAIRE,
	TRIGO
}tree_way_e;

void strat_inutile(void);
void strat_lannion(void);
void strat_test_point();
void strat_test_point2();
void strat_test_fresque();
void strat_tourne_en_rond(void);
error_e strat_manage_fresco();
error_e strat_file_fresco(Sint16 posY);
error_e strat_file_fruit();
error_e strat_ramasser_fruit_arbre1_double(tree_way_e sens);
error_e strat_ramasser_fruit_arbre2_double(tree_way_e sens);
error_e strat_lance_launcher(bool_e lanceAll);
error_e strat_lance_launcher_ennemy();
error_e action_recalage_x(way_e sens, Sint16 angle, Sint16 wanted_x);
error_e action_recalage_y(way_e sens, Sint16 angle, Sint16 wanted_y);
error_e recalage_begin_zone(color_e begin_zone_color);

void strat_test_filet();
void strat_test_small_arm();
void strat_test_vide();

pathfind_node_id_t min_node_dist(pathfind_node_id_t n1,pathfind_node_id_t n2);

#endif /* ACTIONS_PIERRE_H_ */
