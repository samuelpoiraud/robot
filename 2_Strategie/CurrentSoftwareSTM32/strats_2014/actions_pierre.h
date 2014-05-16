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
	OUR_FIRES,
	ADVERSARY_FIRES
}protected_fires_e;

void strat_inutile(void);
void strat_lannion(void);
void strat_test_fresque();
void strat_tourne_en_rond(void);
error_e strat_manage_fresco();
error_e strat_file_fresco(Sint16 posY);
error_e strat_lance_launcher(bool_e lanceAll, color_e mammouth);
//error_e strat_lance_launcher_ennemy();
error_e action_recalage_x(way_e sens, Sint16 angle, Sint16 wanted_x);
error_e action_recalage_y(way_e sens, Sint16 angle, Sint16 wanted_y);
error_e recalage_begin_zone(color_e begin_zone_color);
error_e protected_fires(protected_fires_e fires);

error_e sub_action_initiale();
void strat_homologation();

void strat_test_filet();
void strat_test_small_arm();
void strat_test_vide();
void strat_test_evitement();
void SOS_belgique(void);

#endif /* ACTIONS_PIERRE_H_ */
