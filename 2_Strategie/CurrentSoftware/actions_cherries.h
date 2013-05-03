/*
 *	Club Robot ESEO 2012 - 2013
 *	Krusty & Tiny
 *
 *	Fichier : actions_cherries.h
 *	Package : Carte Strategie
 *	Description : Contient des sub_actions ou micro strat concernant la gestion des cerises et assiettes par Krusty
 *	Auteur : amurzeau
 *	Version 27 avril 2013
 */

#ifndef ACTIONS_CHERRIES_H
#define	ACTIONS_CHERRIES_H

#include "QS/QS_all.h"
#include "act_functions.h"
typedef enum {
	//STRAT_LC_PositionFar,		//Position non implémentée
	STRAT_LC_PositionMid,
	STRAT_LC_PositionNear,
	STRAT_LC_NumberOfPosition	//Pas une position, utilisé pour connaitre le nombre de position dispo
} STRAT_launch_cherries_positions_e;

error_e K_STRAT_sub_cherries_alexis();
error_e K_STRAT_micro_move_to_plate(Uint8 plate_goal_idx);
error_e K_STRAT_micro_grab_plate(bool_e keep_plate, Sint16 initial_x_position, Sint16 initial_y_position);
error_e K_STRAT_micro_launch_cherries(STRAT_launch_cherries_positions_e position, Uint8 expected_cherry_number, bool_e smooth_shake);
error_e K_STRAT_micro_drop_plate(bool_e turn_before_drop);

#endif	/* ACTIONS_CHERRIES_H */

