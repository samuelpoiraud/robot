/*
 *	Club Robot ESEO 2012 - 2013
 *	Krusty & Tiny
 *
 *	Fichier : actions_gifts.h
 *	Package : Carte Strategie
 *	Description : Contient des sub_actions ou micro strat concernant la gestion des cadeaux pour TINY
 *	Auteur : nirgal
 *	Version 201305
 */



#ifndef ACTIONS_GIFTS_H
#define	ACTIONS_GIFTS_H

#include "../QS/QS_all.h"
#include "../avoidance.h"
#include "../environment.h"

error_e TINY_open_all_gifts_without_pause(void);
error_e TINY_forgotten_gift(map_goal_e forgotten_gift);
void TINY_hammer_open_all_gift(bool_e reset);
error_e TINY_open_all_gifts_homolog(void);
error_e TINY_open_all_gifts(void);




#endif	/* ACTIONS_GIFTS_H */

