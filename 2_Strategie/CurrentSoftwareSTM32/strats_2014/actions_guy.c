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
		PLACEMENT_FOYER_GAUCHE,
		SCAN_FOYER_GAUCHE,
		PLACEMENT_FOYER_MILIEU,
		SCAN_FOYER_MILIEU,
		PLACEMENT_FOYER_DROIT,
		SCAN_FOYER_DROIT,
		DONE,
		ERROR
	);

	CAN_msg_t msg;

	switch (state){
		case INIT :
			state = AVANCER;
			break;

		case AVANCER :
			state = try_going(1000, 2500, AVANCER, PLACEMENT_FOYER_GAUCHE, PLACEMENT_FOYER_GAUCHE, SLOW, ANY_WAY, NO_AVOIDANCE);
			break;

		case PLACEMENT_FOYER_GAUCHE :
			state = try_going(1500, 2500, PLACEMENT_FOYER_GAUCHE, SCAN_FOYER_GAUCHE, SCAN_FOYER_GAUCHE, SLOW, FORWARD, NO_AVOIDANCE);
			break;

		case SCAN_FOYER_GAUCHE :
			if(entrance){
				msg.sid = ASSER_LAUNCH_SCAN_TRIANGLE;
				msg.size = 0;
				CAN_send(&msg);
			}

			state = DONE;
			break;

		case PLACEMENT_FOYER_MILIEU :
			state = try_going(1200, 1500, PLACEMENT_FOYER_GAUCHE, SCAN_FOYER_GAUCHE, SCAN_FOYER_GAUCHE, SLOW, FORWARD, NO_AVOIDANCE);
			break;

		case SCAN_FOYER_MILIEU :
			if(entrance){
				msg.sid = ASSER_LAUNCH_SCAN_TRIANGLE;
				msg.size = 0;
				CAN_send(&msg);
			}

			state = DONE;
			break;

		case PLACEMENT_FOYER_DROIT :
			state = try_going(1500, 500, PLACEMENT_FOYER_GAUCHE, SCAN_FOYER_GAUCHE, SCAN_FOYER_GAUCHE, SLOW, FORWARD, NO_AVOIDANCE);
			break;

		case SCAN_FOYER_DROIT :
			if(entrance){
				msg.sid = ASSER_LAUNCH_SCAN_TRIANGLE;
				msg.size = 0;
				CAN_send(&msg);
			}

			state = DONE;
			break;



		case DONE :
		break;
	}
}
