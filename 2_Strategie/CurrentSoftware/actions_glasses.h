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

error_e K_STRAT_micro_grab_glass(bool_e reset_state, ACT_lift_pos_t lift_pos);
error_e K_STRAT_sub_glasses_alexis(void);


#endif	/* ACTIONS_GLASSES_H */

