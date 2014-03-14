/*
 *	Club Robot ESEO 2013 - 2014
 *	Pierre & Guy
 *
 *	Fichier : actions_pierre.h
 *	Package : Carte S�/strats2013
 *	Description : Tests des actions r�alisables par le robot
 *	Auteur : Herzaeone, modifi� par .
 *	Version 2013/10/03
 */

#include "../QS/QS_all.h"

#ifndef ACTIONS_NET_H
#define ACTIONS_NET_H


#include "../asser_functions.h"
#include "../act_functions.h"
#include "../QS/QS_types.h"
#include "../config/config_pin.h"

#define TIME_TO_NET			(MATCH_DURATION - 6500)
#define TIME_MAX_ROTATE				1500
#define TIME_MATCH_TO_NET_ROTATE	(MATCH_DURATION - TIME_MAX_ROTATE)

void strat_placement_net();


#endif /* ACTIONS_NET_H_ */
