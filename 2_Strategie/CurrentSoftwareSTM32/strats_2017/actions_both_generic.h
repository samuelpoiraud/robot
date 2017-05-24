/*
 *	Club Robot ESEO 2014-2015
 *	Holly & Wood
 *
 *	Fichier : actions_both_2015.h
 *	Package : Carte S²/strats2015
 *	Description : Test des actions réalisables par le robot
 *	Auteur : Arnaud
 *	Version 2013/10/03
 */


#include "../QS/QS_all.h"
#include "../propulsion/movement.h"
#include "../QS/QS_watchdog.h"

#ifndef ACTIONS_BOTH_GENERIC_H
#define ACTIONS_BOTH_GENERIC_H

#define DIST_CALLAGE_WOOD		83
#define DIST_CALLAGE_HOLLY		155

void strat_tourne_en_rond(void);
void strat_reglage_odo_rotation(void);
void strat_reglage_odo_translation(void);
void strat_reglage_odo_symetrie(void);

void test_strat_robot_virtuel(void);

void strat_reglage_prop(void);

void strat_stop_robot();

error_e sub_wait(Sint16 x, Sint16 y, time32_t duration);

error_e func_go_to_home();

/*
 * Fait avancer le robot vers une bordure afin de le recaler en x
 *
 * pre	: la position du robot doit être à jour
 * post   : la pile asser est vidée
 * param way				Sens de déplacement
 * param angle				De l'inclinaison du robot
 * param wanted_x			Distance entre le centre du robot et la bordure pour le mettre à jour
 * param get_out			Afin de savoir si le robot devra s'extraire ou non de la bordure (Devra être gérer dans la subaction suivante si FALSE)
 * param delta_correction_x	Différence de position corrigé
 *
 * return sucess_state si le robot a réussi à se recaler
 * return fail_state si le robot n'a pas réussi à se recaler
 */
error_e action_recalage_x(way_e sens, Sint16 angle, Sint16 wanted_x, Sint16 wanted_teta, bool_e get_out, Sint16* delta_correction_x, bool_e set_pos_at_rush, bool_e set_teta_at_rush);

/*
 * Fait avancer le robot vers une bordure afin de le recaler en y
 *
 * pre	: la position du robot doit être à jour
 * post   : la pile asser est vidée
 * param way				Sens de déplacement
 * param angle				De l'inclinaison du robot
 * param wanted_y			Distance entre le centre du robot et la bordure pour le mettre à jour
 * param get_out			Afin de savoir si le robot devra s'extraire ou non de la bordure (Devra être gérer dans la subaction suivante si FALSE)
 * param delta_correction_y	Différence de position corrigé
 *
 * return sucess_state si le robot a réussi à se recaler
 * return fail_state si le robot n'a pas réussi à se recaler
 */
error_e action_recalage_y(way_e sens, Sint16 angle, Sint16 wanted_y, Sint16 wanted_teta, bool_e get_out, Sint16 * delta_correction_y, bool_e set_pos_at_rush, bool_e set_teta_at_rush);

void xbee_send_my_position(void);

#endif

