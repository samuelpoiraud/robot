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

	typedef enum
	{
		ZONE_TO_PROTECT_SMART_CHOICE,	//On laisse la fonction choisir ce qu'elle préfère protéger...
		ZONE_TO_PROTECT_OUR_FULL_SIDE,
		ZONE_TO_PROTECT_MIDDLE_MOONBASE,
		ZONE_TO_PROTECT_ADV_CENTER_MOONBASE,
		ZONE_TO_PROTECT_OUR_CENTER_MOONBASE,
		ZONE_TO_PROTECT_OUR_SIDE_MOONBASE,
		ZONE_TO_PROTECT_ADV_SIDE_MOONBASE
	}zone_to_protect_e;

	error_e sub_protect(zone_to_protect_e zone);

	// Subaction de test de l'évitement (A ne pas jouer en match)
	error_e strat_test_avoidance(void);

	// Fonction de calcul du point de prise de la fusée et de l'angle théorique de prise
	void compute_take_point_rocket(GEOMETRY_point_t *take_point, Sint16 *take_angle, GEOMETRY_point_t store_point, Sint16 angle_robot, Uint16 dist);

	void compute_take_point_rocket_to_handle_error(GEOMETRY_point_t *take_point, Sint16 *take_angle, GEOMETRY_point_t store_point, Sint16 angle_robot, Uint16 dist, Uint16 error_dist, GEOMETRY_point_t robot_pos);

#endif

