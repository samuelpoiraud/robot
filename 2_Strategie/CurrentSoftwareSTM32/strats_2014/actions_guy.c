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
#include "../QS/QS_can_over_xbee.h"

static void REACH_POINT_GET_OUT_INIT_send_request();

// Par defaut si les deux defines suivant sont desactivés le guy va passer par le foyer centrale
//#define WAY_INIT_TREE_SIDE  // Est prioritaire sur le chemin par le mammouth
#define WAY_INIT_MAMMOUTH_SIDE


//#define FALL_FIRST_FIRE // Si on souhaite faire tomber le premier feux dés le début

// Fonctionne que pour les chemins MAMMOUTH_SIDE et HEART_SIDE (chemin par defaut si aucun define)
bool_e rush_to_torch = FALSE;  // Si FALSE va faire tomber un ou des triangle(s) avant
bool_e fall_fire_wall_adv = TRUE;  // Va aller faire tomber le feu si on sait que l'ennemis ne le fais pas tomber des le debut


#define DIM_START_TRAVEL_TORCH 200


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
			state = check_sub_action_result(travel_torch_line(OUR_TORCH,PUSH_FRESCO,1750,250),RAMEMENER_TORCH,DONE,ERROR);
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


error_e sub_action_initiale_guy(){
	CREATE_MAE_WITH_VERBOSE(0,
		IDLE,
		SUCESS,
		GET_OUT_POS_START,
		FALL_FIRST_FIRE,
		TAKE_DECISION_FIRST_WAY,
		GOTO_TREE_INIT,
		FALL_FIRE_WALL_TREE,  // Fait tomber les deux feux contre le mur milieu
		WAIT_GOTO_MAMMOUTH_INIT,
		GOTO_MAMMOUTH_INIT,
		FALL_MOBILE_MM_ADV,
		GOTO_HEART_INIT,
		FALL_FIRE_MOBILE_TREE_ADV,
		GOTO_TORCH_ADVERSARY,
		FALL_FIRE_MOBILE_MM_ADV,
		FALL_FIRE_WALL_ADV,
		DONE,
		ERROR
	);

	static displacement_t points[4];
	static bool_e pierre_reach_point_C1 = FALSE;

	if(global.env.reach_point_C1)
		pierre_reach_point_C1 = TRUE;

	#ifdef WAY_INIT_TREE_SIDE
		points[0] = (displacement_t){{950,COLOR_Y(580)},FAST};
		points[1] = (displacement_t){{1350,COLOR_Y(780)},FAST};
		points[2] = (displacement_t){{1395,COLOR_Y(1070)},FAST};
		points[3] = (displacement_t){{1780,COLOR_Y(1390)},FAST};
	#elif defined WAY_INIT_MAMMOUTH_SIDE
		points[0] = (displacement_t){{560,COLOR_Y(580)},FAST};
		points[1] = (displacement_t){{530,COLOR_Y(975)},FAST};
		points[2] = (displacement_t){{590,COLOR_Y(1530)},FAST};
		points[3] = (displacement_t){{700,COLOR_Y(1720)},FAST};
	#else
		points[0] = (displacement_t){{950,COLOR_Y(580)},FAST};
		points[1] = (displacement_t){{1350,COLOR_Y(780)},FAST};
		points[2] = (displacement_t){{1395,COLOR_Y(1070)},FAST};
		points[3] = (displacement_t){{1400,COLOR_Y(1710)},FAST};
	#endif



	switch(state){

		case IDLE:
			#if FALL_FIRST_FIRE
				state = FALL_FIRST_FIRE;
			#else
				state = GET_OUT_POS_START;
			#endif
			break;

		case FALL_FIRST_FIRE:
			state = GET_OUT_POS_START;
			break;

		case GET_OUT_POS_START:
			state  = try_going_until_break(700,COLOR_Y(300),GET_OUT_POS_START,TAKE_DECISION_FIRST_WAY, TAKE_DECISION_FIRST_WAY,FAST,ANY_WAY,DODGE_AND_NO_WAIT);
			break;

		case TAKE_DECISION_FIRST_WAY:
			if(entrance)
				REACH_POINT_GET_OUT_INIT_send_request();

			#ifdef WAY_INIT_TREE_SIDE
				state = GOTO_TREE_INIT;
			#elif defined WAY_INIT_MAMMOUTH_SIDE
				state = WAIT_GOTO_MAMMOUTH_INIT;
			#else
				state = GOTO_HEART_INIT;
			#endif
			break;

		case GOTO_TREE_INIT:
			state  = try_going_multipoint(points,4,GOTO_TREE_INIT,FALL_FIRE_WALL_TREE,ERROR,ANY_WAY,DODGE_AND_NO_WAIT, END_AT_BREAK);
			break;

		case FALL_FIRE_WALL_TREE:
			state = GOTO_TORCH_ADVERSARY;
			break;

		case GOTO_HEART_INIT:
			state  = try_going_multipoint(points,4,GOTO_HEART_INIT,(rush_to_torch == TRUE)? GOTO_TORCH_ADVERSARY : FALL_FIRE_MOBILE_TREE_ADV,ERROR,ANY_WAY,DODGE_AND_NO_WAIT, END_AT_BREAK);
			break;

		case FALL_FIRE_MOBILE_TREE_ADV:
			state = GOTO_TORCH_ADVERSARY;
			break;

		case WAIT_GOTO_MAMMOUTH_INIT:{  // Attend le passage de pierre pour pouvoir passer à son tour
			static Uint16 last_time;
			if(entrance)
				last_time = global.env.match_time;

			if(pierre_reach_point_C1 || global.env.match_time > last_time + 5000)
				state = GOTO_MAMMOUTH_INIT;

			}break;

		case GOTO_MAMMOUTH_INIT:
			if(entrance)
				ASSER_WARNER_arm_y(1200);

			state  = try_going_multipoint(points,4,GOTO_MAMMOUTH_INIT,(rush_to_torch == TRUE)? GOTO_TORCH_ADVERSARY : FALL_FIRE_MOBILE_MM_ADV,ERROR,ANY_WAY,DODGE_AND_NO_WAIT, END_AT_BREAK);

			if(global.env.asser.reach_x)
				ZONE_unlock(MZ_MAMMOUTH_OUR);

			break;

		case FALL_FIRE_MOBILE_MM_ADV:
			state = GOTO_TORCH_ADVERSARY;
			break;

		case GOTO_TORCH_ADVERSARY:
			state = try_going_until_break(1100,COLOR_Y(1900),GOTO_TORCH_ADVERSARY,(fall_fire_wall_adv == TRUE)? FALL_FIRE_WALL_ADV : DONE,(fall_fire_wall_adv == TRUE)? FALL_FIRE_WALL_ADV : ERROR,FAST,ANY_WAY,DODGE_AND_NO_WAIT);
			break;

		case FALL_FIRE_WALL_ADV:
			state = DONE;
			break;

		case DONE:
			return END_OK;
			break;

		case ERROR:
			return NOT_HANDLED;
			break;

		default:
			break;
	}

	return IN_PROGRESS;
}


error_e travel_torch_line(torch_choice_e torch_choice,torch_push_e choice,Sint16 posEndxIn, Sint16 posEndyIn){
	CREATE_MAE_WITH_VERBOSE(0,
		IDLE,
		PLACEMENT,
		POS_START_TORCH,
		MOVE_TORCH,
		SLOW_MOTION,
		REMOTENESS,
		ERROR,
		DONE
	);


	static GEOMETRY_point_t posStart,posEnd;

	// S'éloigne à la fin de la pousser mais ralenti aussi avant la fin de la pousser de la torche
	static GEOMETRY_point_t eloignement;

	static pathfind_node_id_t node;


	switch(state){
		case IDLE :{
			GEOMETRY_point_t torch;

			torch = TORCH_get_position(torch_choice);

			if(choice == PUSH_CHOICE){
				posEnd.x = posEndxIn;
				posEnd.y = posEndyIn;
			}else if(choice == PUSH_FRESCO){
				posEnd.x = 300;
				posEnd.y = 1500;
			}else if(choice == PUSH_HEARTH_ADV){
				posEnd.x = 1600;
				posEnd.y = 300;
			}else if(choice == PUSH_HEARTH_CENTRAL){
				posEnd.x = 800;
				posEnd.y = 1300;
			}else{ // PUSH_CAVERN_ADV
				posEnd.x = 400;
				posEnd.y = 200;
			}

			Uint16 norm = GEOMETRY_distance(torch,posEnd);

			float coefx = (torch.x - posEnd.x)/(norm*1.);
			float coefy = (torch.y - posEnd.y)/(norm*1.);

			posStart.x = torch.x + DIM_START_TRAVEL_TORCH*coefx;
			posStart.y = torch.y + DIM_START_TRAVEL_TORCH*coefy;

			node = PATHFIND_closestNode(posStart.x, posStart.y, 0);

			eloignement.x = posEnd.x + DIM_START_TRAVEL_TORCH*coefx;
			eloignement.y = posEnd.y + DIM_START_TRAVEL_TORCH*coefy;

			state = PLACEMENT;

		}	break;

		case PLACEMENT:
			state = PATHFIND_try_going(node, PLACEMENT, POS_START_TORCH, ERROR, ANY_WAY, FAST, NO_DODGE_AND_WAIT, END_AT_BREAK);
			break;

		case POS_START_TORCH:
			state = try_going(posStart.x, posStart.y, POS_START_TORCH, MOVE_TORCH, ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT);
			break;

		case MOVE_TORCH :
			state = try_going_until_break(eloignement.x, eloignement.y, MOVE_TORCH, SLOW_MOTION, ERROR, SLOW, FORWARD, NO_DODGE_AND_WAIT);
			break;

		case SLOW_MOTION:
			state = try_going(posEnd.x, posEnd.y, SLOW_MOTION, REMOTENESS, ERROR, SLOW, FORWARD, NO_DODGE_AND_WAIT);
			break;

		case REMOTENESS:  // eloignement
			if(entrance)
				TORCH_new_position(choice);

			state = try_going(eloignement.x, eloignement.y, REMOTENESS, DONE, ERROR, FAST, BACKWARD, NO_DODGE_AND_WAIT);
			break;

		case DONE:
			state = IDLE;
			return END_OK;
			break;

		case ERROR:
			TORCH_new_position(choice);
			state = IDLE;
			return NOT_HANDLED;
			break;
	}

	return IN_PROGRESS;
}


static void REACH_POINT_GET_OUT_INIT_send_request() {
	CAN_msg_t msg;

	msg.sid = XBEE_REACH_POINT_GET_OUT_INIT;
	msg.size = 0;

	CANMsgToXbee(&msg,FALSE);
}
