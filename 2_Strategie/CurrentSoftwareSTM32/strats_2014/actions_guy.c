/*
 *	Club Robot ESEO 2013 - 2014
 *	Pierre & Guy
 *
 *	Fichier : actions_guy.c
 *	Package : Carte S�/strats2013
 *	Description : Tests des actions r�alisables par le robot
 *	Auteur : Herzaeone, modifi� par .
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

// Strat�gie de test des d�ctections de triangle
void Strat_Detection_Triangle(void){
	// x = 1000
	// y = 120 led verte / 2880 led rouge

	CREATE_MAE_WITH_VERBOSE(0,
		INIT,
		AVANCER,
		PLACEMENT_FOYER_GAUCHE,
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
			state = try_going(1000, 2500, AVANCER, PLACEMENT_FOYER_GAUCHE, PLACEMENT_FOYER_GAUCHE, SLOW, ANY_WAY, NO_AVOIDANCE);
			break;

		case PLACEMENT_FOYER_GAUCHE :
			state = try_going(1500, 2500, PLACEMENT_FOYER_GAUCHE, SCAN_FOYER_GAUCHE, PLACEMENT_FOYER_MILIEU, SLOW, FORWARD, NO_AVOIDANCE);
			break;

		case SCAN_FOYER_GAUCHE :
			if(entrance)
				LAUNCH_SCAN_TRIANGLE();

			if(propulsion_send_triangle()){
				afficher_donnee_triangle();
				state = PLACEMENT_FOYER_MILIEU;
			}
			break;

		case PLACEMENT_FOYER_MILIEU :
			state = try_going(1000, 1900, PLACEMENT_FOYER_MILIEU, LIBERE_MILIEU, LIBERE_MILIEU, SLOW, FORWARD, NO_AVOIDANCE);
			break;

		case SCAN_FOYER_MILIEU :
			if(entrance)
				LAUNCH_SCAN_TRIANGLE();

			if(propulsion_send_triangle()){
				afficher_donnee_triangle();
				state = LIBERE_MILIEU;
			}
			break;

		case LIBERE_MILIEU :
			state = try_going(1500, 1900, LIBERE_MILIEU, PLACEMENT_FOYER_DROIT, PLACEMENT_FOYER_DROIT, SLOW, FORWARD, NO_AVOIDANCE);
			break;

		case PLACEMENT_FOYER_DROIT :
			state = try_going(1600, 400, PLACEMENT_FOYER_DROIT, PLACEMENT_FOYER_GAUCHE, PLACEMENT_FOYER_GAUCHE, SLOW, FORWARD, NO_AVOIDANCE);
			break;

		case SCAN_FOYER_DROIT :
			if(entrance)
				LAUNCH_SCAN_TRIANGLE();

			if(propulsion_send_triangle()){
				afficher_donnee_triangle();
				state = PLACEMENT_FOYER_GAUCHE;
			}
			break;

		case DONE :
			break;

		default :
			break;
	}
}

