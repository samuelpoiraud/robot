/*
 *	Club Robot ESEO 2013 - 2014
 *	Pierre & Guy
 *
 *	Fichier : actions_guy.c
 *	Package : Carte S²/strats2013
 *	Description : Tests des actions réalisables par le robot
 *	Auteur : Herzaeone, modifié par .
 *	Version 2013/10/03
 */

#include "actions_guy.h"
#include "../QS/QS_outputlog.h"
#include "../state_machine_helper.h"
#include "../elements.h"



/* ----------------------------------------------------------------------------- */
/* 							Fonctions d'homologation			                 */
/* ----------------------------------------------------------------------------- */



/* ----------------------------------------------------------------------------- */
/* 							Autre strats de test             			 */
/* ----------------------------------------------------------------------------- */

// Stratégie de test des déctections de triangle
void Strat_Detection_Triangle(void){
	// x = 1000
	// y = 120 led verte / 2880 led rouge

	CREATE_MAE_WITH_VERBOSE(0,
		INIT,
		AVANCER,
		SCAN_FOYER_GAUCHE,
		PLACEMENT_FOYER_MILIEU,
		SCAN_FOYER_MILIEU,
		LIBERE_MILIEU,
		PLACEMENT_FOYER_DROIT,
		SCAN_FOYER_DROIT,
		DONE,
		ERROR
	);


	switch (state){
		case INIT :
			state = AVANCER;
			break;

		case AVANCER :
			state = try_going(1000, 2500, AVANCER, SCAN_FOYER_GAUCHE, SCAN_FOYER_GAUCHE, SLOW, ANY_WAY, NO_AVOIDANCE);
			break;

		case SCAN_FOYER_GAUCHE :
			state = try_going_and_rotate_scan(0, -PI4096/2, 90,
							 1600, 2600, SCAN_FOYER_GAUCHE, PLACEMENT_FOYER_MILIEU, PLACEMENT_FOYER_MILIEU, FAST, FORWARD, NO_AVOIDANCE);
			break;

		case PLACEMENT_FOYER_MILIEU :
			state = try_going_and_rotate_scan(3*PI4096/4, -3*PI4096/4, 90,
							  1000, 1900, PLACEMENT_FOYER_MILIEU, LIBERE_MILIEU, LIBERE_MILIEU, SLOW, FORWARD, NO_AVOIDANCE);
			break;

		case LIBERE_MILIEU :
			state = try_going(1500, 1900, LIBERE_MILIEU, PLACEMENT_FOYER_DROIT, PLACEMENT_FOYER_DROIT, SLOW, FORWARD, NO_AVOIDANCE);
			break;

		case PLACEMENT_FOYER_DROIT :
			state =  try_going_and_rotate_scan(-PI4096/2, -PI4096, 90,
							   1600, 400, PLACEMENT_FOYER_DROIT, SCAN_FOYER_GAUCHE, SCAN_FOYER_GAUCHE, SLOW, FORWARD, NO_AVOIDANCE);
			break;

		case DONE :
			break;

		default :
			break;
	}
}

