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

#include "../QS/QS_all.h"
#include "../act_functions.h"
typedef enum {
	//STRAT_LC_PositionFar,		//Position non implémentée
	STRAT_LC_PositionMid,
	STRAT_LC_PositionNear,
	STRAT_LC_NumberOfPosition	//Pas une position, utilisé pour connaitre le nombre de position dispo
} STRAT_launch_cherries_positions_e;

typedef enum {
	LP_Near,
	LP_Far
} line_pos_t;

typedef enum {
	STRAT_PGA_Y,		//Axe Y
	STRAT_PGA_XPos,		//Axe X, déplacement pour prendre l'assiette vers le gateau
	STRAT_PGA_XNeg		//Axe X, déplacement pour prendre l'assiette vers les cadeaux
} STRAT_plate_grap_axis_e;

typedef struct {
	//Position X de l'assiette
	Sint16 x;
	//Lignes de déplacement pour évitement (proche / loin)
	Sint16 y_near;
	Sint16 y_far;
	//Doit on verrouiller la zone sous le gateau pour passer sur la ligne Far ?
	bool_e far_line_check_tiny;
} plate_info_t;

plate_info_t PLATE_get_info(Uint8 plate_id);

error_e K_STRAT_sub_cherries_alexis();
error_e K_STRAT_micro_move_to_plate(Uint8 plate_goal, line_pos_t line_goal, bool_e immediate_fail);
error_e K_STRAT_micro_grab_plate(STRAT_plate_grap_axis_e axis, STRAT_plate_grap_axis_e drop_plate_axis, bool_e keep_plate, bool_e auto_pull, Sint16 plate_x_position, Sint16 plate_y_position);
error_e K_STRAT_micro_launch_cherries(STRAT_launch_cherries_positions_e position, Uint8 expected_cherry_number, bool_e smooth_shake);
error_e K_STRAT_micro_drop_plate(bool_e turn_before_drop, Sint16 angle);

#endif	/* ACTIONS_CHERRIES_H */

