/*
 *	Club Robot ESEO 2013 - 2014
 *	Pierre & Guy
 *
 *	Fichier : actions_guy.h
 *	Package : Carte S²/strats2013
 *	Description : Tests des actions réalisables par le robot
 *	Auteur : Herzaeone, modifié par .
 *	Version 2013/10/03
 */

#include "../QS/QS_all.h"

#ifndef ACTIONS_GUY_H
#define ACTIONS_GUY_H


#include "../asser_functions.h"
#include "../act_functions.h"
#include "../Geometry.h"
#include "../elements.h"

void strat_inutile_guy(void);
void strat_xbee_guy(void);

void Strat_Detection_Triangle(void);
void strat_test_warner_triangle(void);
void strat_test_arm(void);
void strat_belgique_guy(void);

error_e sub_action_initiale_guy();
error_e ACT_arm_move(ARM_state_e state_arm, Sint16 x, Sint16 y, Uint8 in_progress, Uint8 success_state, Uint8 fail_state);
error_e ACT_arm_deploy_torche(torch_choice_e choiceTorch, torch_dispose_zone_e filed);
error_e do_torch(torch_choice_e torch_choice, torch_dispose_zone_e dispose_zone, torch_dispose_zone_e if_fail_dispose_zone);
error_e ACT_small_arm_move(Uint8 state_arm, Uint8 in_progress, Uint8 success_state, Uint8 fail_state);
error_e ACT_elevator_arm_move(Uint8 state_arm, Uint8 in_progress, Uint8 success_state, Uint8 fail_state);

/* Fait tomber le premier triangle par une rotation,
 * puis prend le 2éme et l'emmène soit sur le foyer
 * ou bien au pied d'un arbre
 */
error_e do_triangles_between_trees();


/* Scanne la zone de l'adversaire pour savoir
 * de quel sens est son premier triangle
 */
error_e do_triangle_start_adversary();

#endif /* ACTIONS_GUY_H_ */
