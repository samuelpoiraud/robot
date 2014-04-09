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

error_e travel_torch_line(torch_choice_e torch_choice,torch_filed_e choice,Sint16 posEndxIn, Sint16 posEndyIn);
error_e sub_action_initiale_guy();
error_e ACT_arm_move(ARM_state_e state_arm, Sint16 x, Sint16 y, Uint8 in_progress, Uint8 success_state, Uint8 fail_state);
error_e ACT_arm_deploy_torche(torch_choice_e choiceTorch, torch_filed_e filed);
error_e do_torch(torch_choice_e torch_choice,torch_filed_e filed);
error_e ACT_small_arm_move(Uint8 state_arm, Uint8 in_progress, Uint8 success_state, Uint8 fail_state);
error_e ACT_elevator_arm_move(Uint8 state_arm, Uint8 in_progress, Uint8 success_state, Uint8 fail_state);

#endif /* ACTIONS_GUY_H_ */
