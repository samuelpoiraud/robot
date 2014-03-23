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
#include "../Geometry.h"
#include "../Pathfind.h"
#include "../zone_mutex.h"

#define DIM_START_TRAVEL_TORCH 200

static GEOMETRY_point_t posTorch[2] = {{1050,900},		// Torche Rouge
									   {1050,2100}};	// Torche Jaune



/* ----------------------------------------------------------------------------- */
/* 							Fonctions d'homologation			                 */
/* ----------------------------------------------------------------------------- */



/* ----------------------------------------------------------------------------- */
/* 							Autre strats de test             			 */
/* ----------------------------------------------------------------------------- */


void strat_inutile_guy(void){
	CREATE_MAE_WITH_VERBOSE(0,
		IDLE,
		POS_DEPART,
		RAMEMENER_TORCH,
		DONE,
		ERROR
	);

	switch(state){
		case IDLE:
			state = POS_DEPART;
			break;
		case POS_DEPART:
			state = try_going_until_break(global.env.pos.x,COLOR_Y(450),POS_DEPART,RAMEMENER_TORCH,ERROR,FAST,BACKWARD,NO_AVOIDANCE);
			break;

		case RAMEMENER_TORCH:
			state = check_sub_action_result(travel_torch_line(OUR_TORCH,1750,250),RAMEMENER_TORCH,DONE,ERROR);
			break;

		case DONE:
			break;
		case ERROR:
			break;
		default:
			break;
	}

}

void strat_xbee_guy(void){
	CREATE_MAE_WITH_VERBOSE(0,
		IDLE,
		POS_DEPART,
		XBEE,
		POS_END,
		RELACHE_ZONE,
		DONE,
		ERROR
	);

	switch(state){
		case IDLE:
			state = POS_DEPART;
			break;
		case POS_DEPART:
			state = try_going_until_break(global.env.pos.x,COLOR_Y(250),POS_DEPART,XBEE,ERROR,FAST,BACKWARD,NO_AVOIDANCE);
			break;

		case XBEE:
			state = try_lock_zone(MZ_FRUIT_TRAY, 1000, XBEE, POS_END, ERROR, ERROR);
			break;

		case POS_END:
			state = try_going_until_break(global.env.pos.x,COLOR_Y(450),POS_END,RELACHE_ZONE,ERROR,FAST,BACKWARD,NO_AVOIDANCE);
			break;

		case RELACHE_ZONE:
			if(entrance)
				ZONE_unlock(MZ_FRUIT_TRAY);

			state = DONE;
			break;

		case DONE:
			break;
		case ERROR:
			break;
		default:
			break;
	}

}


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
			state = ELEMENT_try_going_and_rotate_scan(0, -PI4096/2, 90,
							 1600, 2600, SCAN_FOYER_GAUCHE, PLACEMENT_FOYER_MILIEU, PLACEMENT_FOYER_MILIEU, FAST, FORWARD, NO_AVOIDANCE);
			break;

		case PLACEMENT_FOYER_MILIEU :
			state = ELEMENT_try_going_and_rotate_scan(3*PI4096/4, -3*PI4096/4, 90,
							  1000, 1900, PLACEMENT_FOYER_MILIEU, LIBERE_MILIEU, LIBERE_MILIEU, SLOW, FORWARD, NO_AVOIDANCE);
			break;

		case LIBERE_MILIEU :
			state = try_going(1500, 1900, LIBERE_MILIEU, PLACEMENT_FOYER_DROIT, PLACEMENT_FOYER_DROIT, SLOW, FORWARD, NO_AVOIDANCE);
			break;

		case PLACEMENT_FOYER_DROIT :
			state =  ELEMENT_try_going_and_rotate_scan(-PI4096/2, -PI4096, 90,
							   1600, 400, PLACEMENT_FOYER_DROIT, SCAN_FOYER_GAUCHE, SCAN_FOYER_GAUCHE, SLOW, FORWARD, NO_AVOIDANCE);
			break;

		case DONE :
			break;

		default :
			break;
	}
}


void strat_test_warner_triangle(){
	CREATE_MAE_WITH_VERBOSE(0,
		INIT,
		AVANCER,
		PLACEMENT,
		DETECTION_TRIANGLE_3,
		DISPLAY_ANSWER,
		DONE
	);
	switch(state){
		case INIT :
			state = AVANCER;
			break;

		case AVANCER :
			state = try_going(1000, 2500, AVANCER, PLACEMENT, PLACEMENT, SLOW, ANY_WAY, NO_AVOIDANCE);
			break;

		case PLACEMENT :
			state = try_going(1600, 2300, PLACEMENT, DETECTION_TRIANGLE_3, DETECTION_TRIANGLE_3, SLOW, ANY_WAY, NO_AVOIDANCE);
			break;

		case DETECTION_TRIANGLE_3 :
			if(entrance)
				ELEMENT_launch_triangle_warner(3);
			state = try_going(1600, 650, DETECTION_TRIANGLE_3, DISPLAY_ANSWER, DISPLAY_ANSWER, SLOW, ANY_WAY, NO_AVOIDANCE);
			break;

		case DISPLAY_ANSWER :
			if(ELEMENT_triangle_present())
				debug_printf("triangle n°3 présent\n");
			else
				debug_printf("triangle n°3 non présent\n");
			state = DONE;
			break;

		case DONE :
			break;

	}

}


error_e travel_torch_line(torch_choice_e torch_choice,Sint16 posEndx, Sint16 posEndy){
	CREATE_MAE_WITH_VERBOSE(0,
		IDLE,
		PLACEMENT,
		POS_START_TORCH,
		MOVE_TORCH,
		POS_END,
		ERROR,
		DONE
	);


	static GEOMETRY_point_t posStart;

	// S'éloigne de la torche à la fin de la pousser
	static GEOMETRY_point_t eloignement;
	static pathfind_node_id_t node;


	switch(state){
		case IDLE :{
			GEOMETRY_point_t torch;

			if(global.env.color == RED){
				if(torch_choice == OUR_TORCH)
					torch = posTorch[0];
				else
					torch = posTorch[1];
			}else{
				if(torch_choice == OUR_TORCH)
					torch = posTorch[1];
				else
					torch = posTorch[0];
			}

			Uint16 norm = GEOMETRY_distance(torch,(GEOMETRY_point_t){posEndx,posEndy});

			float coefx = (torch.x - posEndx)/(norm*1.);
			float coefy = (torch.y - posEndy)/(norm*1.);

			posStart.x = torch.x + DIM_START_TRAVEL_TORCH*coefx;
			posStart.y = torch.y + DIM_START_TRAVEL_TORCH*coefy;

			node = PATHFIND_closestNode(posStart.x, posStart.y, 0);

			eloignement.x = posEndx + DIM_START_TRAVEL_TORCH*coefx;
			eloignement.y = posEndy + DIM_START_TRAVEL_TORCH*coefy;

			state = PLACEMENT;

		}	break;

		case PLACEMENT:
			state = PATHFIND_try_going(node, PLACEMENT, POS_START_TORCH, ERROR, ANY_WAY, FAST, NO_AVOIDANCE, END_AT_BREAK);
			break;

		case POS_START_TORCH:
			state = try_going(posStart.x, posStart.y, POS_START_TORCH, MOVE_TORCH, ERROR, FAST, FORWARD, NO_AVOIDANCE);
			break;

		case MOVE_TORCH :
			state = try_going(posEndx, posEndy, MOVE_TORCH, POS_END, ERROR, SLOW, FORWARD, NO_AVOIDANCE);
			break;

		case POS_END:
			state = try_going(eloignement.x, eloignement.y, POS_END, DONE, ERROR, FAST, BACKWARD, NO_AVOIDANCE);
			break;

		case DONE:
			state = IDLE;
			return END_OK;
			break;

		case ERROR:
			state = IDLE;
			return NOT_HANDLED;
			break;
	}

	return IN_PROGRESS;
}
