/*
 *	Club Robot ESEO 2012 - 2013
 *	Krusty & Tiny
 *
 *	Fichier : actions_utils.h
 *	Package : Carte Stratégie
 *	Description : Offre des fonctions simplifiant le code de strategie
 *	Auteur : amurzeau
 *	Version 27 avril 2013
 */

#ifndef ACTIONS_UTILS_H
#define	ACTIONS_UTILS_H

#include "../QS/QS_types.h"

#define HAMMER_POSITION_UP		(0)
#define HAMMER_POSITION_CANDLE	(35)
#define HAMMER_POSITION_DOWN	(85)
#define HAMMER_POSITION_HOME	(90)

Uint16 wait_hammer(Uint16 progress, Uint16 success, Uint16 fail);

bool_e all_gifts_done(void);

#endif	/* ACTIONS_UTILS_H */

