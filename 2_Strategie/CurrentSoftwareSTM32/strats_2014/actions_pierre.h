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


void strat_inutile(void);
void strat_lannion(void);
void strat_test_fresque();
error_e strat_manage_fresco();
error_e strat_file_fresco(Sint16 posY);
//error_e strat_lance_launcher_ennemy();
error_e do_triangle_start();
error_e do_torch_pierre();
error_e ACT_arm_deploy_torche_pierre();
error_e recalage_begin_zone(color_e begin_zone_color);
error_e protect_north_way(void);
error_e sub_action_initiale();
error_e sub_action_balayage();
void strat_homologation();

void strat_test_filet();
void strat_test_small_arm();
void strat_test_vide();
void strat_test_evitement();
void SOS_belgique(void);

#endif /* ACTIONS_PIERRE_H_ */
