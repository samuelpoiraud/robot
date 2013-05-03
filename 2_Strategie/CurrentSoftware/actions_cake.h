/*
 *	Club Robot ESEO 2012 - 2013
 *	Krusty & Tiny
 *
 *	Fichier : actions_cake.h
 *	Package : Carte Strategie
 *	Description : Contient des sub_actions ou micro strat concernant la gestion du gateau pour TINY.
 *	Auteur : nirgal
 *	Version 201305
 */

#ifndef ACTIONS_CAKE_H
#define	ACTIONS_CAKE_H

#include "QS/QS_all.h"
#include "avoidance.h"

error_e TINY_warner_around_cake(Sint8 way);
error_e TINY_warner_blow_all_candles(void);
error_e TINY_warner_blow_one_candle(bool_e reset);
error_e TINY_blow_all_candles(void);
error_e TINY_blow_one_candle(Uint8 i, Sint8 way);

typedef enum
{
	CAKE_PART_QUATER_BLUE,
	CAKE_PART_MIDLE_BLUE,
	CAKE_PART_MIDLE_RED,
	CAKE_PART_QUATER_RED
}cake_part_e;
error_e TINY_blow_quater(cake_part_e cake_part);
error_e old_TINY_all_candles(void);


#endif	/* ACTIONS_CAKE_H */

