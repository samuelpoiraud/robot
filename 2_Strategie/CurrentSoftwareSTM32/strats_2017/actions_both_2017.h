/*
 *	Club Robot ESEO 2014-2015
 *	Holly & Wood
 *
 *	Fichier : actions_both_2015.h
 *	Package : Carte S²/strats2015
 *	Description : Test des actions réalisables par le robot
 *	Auteur : Arnaud
 *	Version 2013/10/03
 */


#include "../QS/QS_all.h"

#ifndef ACTIONS_BOTH_2017_H
	#define ACTIONS_BOTH_2017_H

	// Initialize color sensors
	void ColorSensor_init();

	error_e sub_cross_rocker(void);
	error_e sub_wait_1_sec(void);

	// Subaction de test de l'évitement (A ne pas jouer en match)
	error_e strat_test_avoidance(void);
	GEOMETRY_point_t compute_take_point_rocket(GEOMETRY_point_t store_point, Sint16 angle_robot, Uint16 dist);


#endif

