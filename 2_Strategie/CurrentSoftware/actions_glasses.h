/*
 *	Club Robot ESEO 2012 - 2013
 *	Krusty & Tiny
 *
 *	Fichier : actions_glasses.h
 *	Package : Carte Strategie
 *	Description : Contient des sub_actions ou micro strat concernant la gestion des verres par Krusty
 *	Auteur : amurzeau
 *	Version 26 avril 2013
 */

#ifndef ACTIONS_GLASSES_H
#define	ACTIONS_GLASSES_H

#include "QS/QS_all.h"
#include "act_functions.h"

typedef struct {
	avoidance_type_e avoidance_type;
	Uint8 nb_points;
	Uint8 move_points_begin_index;
} displacement_block_t;

error_e K_STRAT_sub_glasses_alexis();
error_e K_STRAT_micro_do_glasses(Uint8 trajectory_to_home_number, const displacement_block_t trajectories_to_home[], Uint8 trajectory_number, const displacement_block_t trajectories[], displacement_t move_points[]);
error_e K_STRAT_micro_grab_glass(bool_e reset_state, ACT_lift_pos_t lift_pos);


#endif	/* ACTIONS_GLASSES_H */

