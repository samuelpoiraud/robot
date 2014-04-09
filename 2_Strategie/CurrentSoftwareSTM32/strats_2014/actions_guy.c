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
#include "../act_functions.h"
#include <math.h>

static void REACH_POINT_GET_OUT_INIT_send_request();

// Par defaut si les deux defines suivant sont desactivés le guy va passer par le foyer centrale
//#define WAY_INIT_TREE_SIDE  // Est prioritaire sur le chemin par le mammouth
//#define WAY_INIT_MAMMOUTH_SIDE


//#define FALL_FIRST_FIRE // Si on souhaite faire tomber le premier feux dés le début

// Fonctionne que pour les chemins MAMMOUTH_SIDE et HEART_SIDE (chemin par defaut si aucun define)
bool_e rush_to_torch = FALSE;  // Si FALSE va faire tomber un ou des triangle(s) avant
bool_e fall_fire_wall_adv = TRUE;  // Va aller faire tomber le feu si on sait que l'ennemis ne le fais pas tomber des le debut


#define DIM_START_TRAVEL_TORCH 200
#define ARM_TIMEOUT 10000

/* ----------------------------------------------------------------------------- */
/* 							Fonctions d'homologation			                 */
/* ----------------------------------------------------------------------------- */



/* ----------------------------------------------------------------------------- */
/* 							Autre strats de test             			 */
/* ----------------------------------------------------------------------------- */


void strat_inutile_guy(void){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_GUY_INUTILE,
		IDLE,
		POS_DEPART,
		RAMEMENER_TORCH,
		DEPLOY_TORCH,
		DO_TORCH,
		DONE,
		ERROR
	);

	switch(state){
		case IDLE:
			state = DEPLOY_TORCH;
			break;
		case POS_DEPART:
			state = try_going_until_break(global.env.pos.x,COLOR_Y(450),POS_DEPART,DO_TORCH,ERROR,FAST,BACKWARD,NO_AVOIDANCE);
			break;

		case RAMEMENER_TORCH:
			state = check_sub_action_result(travel_torch_line(OUR_TORCH,FILED_FRESCO,1750,250),RAMEMENER_TORCH,DONE,ERROR);
			break;

		case DO_TORCH:
			state = check_sub_action_result(do_torch(OUR_TORCH,HEARTH_OUR),DO_TORCH,DONE,ERROR);
			break;

		case DEPLOY_TORCH:
			state = check_sub_action_result(ACT_arm_deploy_torche(OUR_TORCH,HEARTH_OUR),DEPLOY_TORCH,DONE,ERROR);
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
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_GUY_XBEE,
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

	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_GUY_DETECTION_TRIANGLE,
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
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_GUY_TEST_WARNER_TRIANGLE,
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
	CREATE_MAE_WITH_VERBOSE(SM_ID_SUB_GUY_INITIALE,
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
			state  = try_going_until_break(700,COLOR_Y(300),GET_OUT_POS_START,TAKE_DECISION_FIRST_WAY, TAKE_DECISION_FIRST_WAY,FAST,ANY_WAY,NO_DODGE_AND_WAIT);
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
			state  = try_going_multipoint(points,4,GOTO_TREE_INIT,FALL_FIRE_WALL_TREE,ERROR,ANY_WAY,NO_DODGE_AND_WAIT, END_AT_BREAK);
			break;

		case FALL_FIRE_WALL_TREE:
			state = GOTO_TORCH_ADVERSARY;
			break;

		case GOTO_HEART_INIT:
			state  = try_going_multipoint(points,4,GOTO_HEART_INIT,(rush_to_torch == TRUE)? GOTO_TORCH_ADVERSARY : FALL_FIRE_MOBILE_TREE_ADV,ERROR,ANY_WAY,NO_DODGE_AND_WAIT, END_AT_BREAK);
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

			state  = try_going_multipoint(points,4,GOTO_MAMMOUTH_INIT,(rush_to_torch == TRUE)? GOTO_TORCH_ADVERSARY : FALL_FIRE_MOBILE_MM_ADV,ERROR,ANY_WAY,NO_DODGE_AND_WAIT, END_AT_BREAK);

			if(global.env.asser.reach_x)
				ZONE_unlock(MZ_MAMMOUTH_OUR);

			break;

		case FALL_FIRE_MOBILE_MM_ADV:
			state = GOTO_TORCH_ADVERSARY;
			break;

		case GOTO_TORCH_ADVERSARY:
			state = try_going_until_break(1100,COLOR_Y(1900),GOTO_TORCH_ADVERSARY,(fall_fire_wall_adv == TRUE)? FALL_FIRE_WALL_ADV : DONE,(fall_fire_wall_adv == TRUE)? FALL_FIRE_WALL_ADV : ERROR,FAST,ANY_WAY,NO_DODGE_AND_WAIT);
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

error_e do_torch(torch_choice_e torch_choice,torch_filed_e filed){
	CREATE_MAE_WITH_VERBOSE(SM_ID_SUB_GUY_TRAVEL_TORCH_LINE,
		IDLE,
		PUSH_TORCH,
		RECULE,
		PLACEMENT,
		DEPLOY_TORCH,
		ERROR,
		DONE
	);



	switch(state){
		case IDLE :
			state = PUSH_TORCH;
			break;

		case PUSH_TORCH:
			state = check_sub_action_result(travel_torch_line(torch_choice,filed,0, 0),PUSH_TORCH,RECULE,RECULE);
			break;

		case RECULE:
			state = try_going(1630, 300, RECULE, PLACEMENT, ERROR, FAST, BACKWARD, NO_DODGE_AND_WAIT);
			break;

		case PLACEMENT:
			state = try_going(1740, 180, PLACEMENT, DEPLOY_TORCH, DEPLOY_TORCH, FAST, FORWARD, NO_DODGE_AND_WAIT);
			break;

		case DEPLOY_TORCH:
			state = check_sub_action_result(ACT_arm_deploy_torche(torch_choice,filed),DEPLOY_TORCH,DONE,ERROR);
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



error_e travel_torch_line(torch_choice_e torch_choice,torch_filed_e choice,Sint16 posEndxIn, Sint16 posEndyIn){
	CREATE_MAE_WITH_VERBOSE(SM_ID_SUB_GUY_TRAVEL_TORCH_LINE,
		IDLE,
		PLACEMENT,
		POS_START_TORCH,
		MOVE_TORCH,
		END_TORCH,
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

			if(choice == ANYWHERE){
				posEnd.x = posEndxIn;
				posEnd.y = posEndyIn;
			}else if(choice == FILED_FRESCO){
				posEnd.x = 300;
				posEnd.y = 1500;
			}else if(choice == HEARTH_OUR){
				posEnd.x = 1800;
				posEnd.y = COLOR_Y(360);
			}else if(choice == HEARTH_CENTRAL){
				posEnd.x = 800;
				posEnd.y = COLOR_Y(1300);
			}else{ // PUSH_CAVERN_ADV
				posEnd.x = 400;
				posEnd.y = COLOR_Y(200);
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
			state = PATHFIND_try_going(node, PLACEMENT, POS_START_TORCH, ERROR, ANY_WAY, FAST, DODGE_AND_WAIT, END_AT_BREAK);
			break;

		case POS_START_TORCH:
			state = try_going(posStart.x, posStart.y, POS_START_TORCH, MOVE_TORCH, ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT);
			break;

		case MOVE_TORCH :
			state = try_going_until_break(eloignement.x, eloignement.y, MOVE_TORCH, END_TORCH, ERROR, SLOW, FORWARD, NO_DODGE_AND_WAIT);
			break;

		case END_TORCH:
			state = try_going_until_break(posEnd.x, posEnd.y, END_TORCH, REMOTENESS, REMOTENESS, SLOW, FORWARD, NO_DODGE_AND_WAIT);
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

error_e ACT_arm_deploy_torche(torch_choice_e choiceTorch, torch_filed_e filed){
	CREATE_MAE_WITH_VERBOSE(SM_ID_SUB_GUY_TRAVEL_TORCH_LINE,
		IDLE,
		OPEN,
		TORCHE,
		DOWN_ARM,
		UP_ARM,
		OPEN_2,
		FILED_TRIANGLE,
		WAIT_TRIANGLE_BREAK,
		BACK,
		PREPARE_RETURN,
		DOWN_RETURN,
		RETURN,
		INVERSE_PUMP,
		DOWN_RETURN_2,
		PREPARE_RETURN_2,
		SMALL_ARM_DEPLOYED,
		SMALL_ARM_PARKED,
		TAKE_RETURN,
		GRAP_TRIANGLE,
		OPEN_3,
		ADVANCE,
		OPEN_NOT_HANDLED,
		PARKED_NOT_HANDLED,
		ERROR,
		DONE
	);

	static GEOMETRY_point_t torch;

	static GEOMETRY_point_t point[3];
	static Uint8 i = 0;

	if(filed == HEARTH_OUR){
		point[0] = (GEOMETRY_point_t){global.env.pos.x+150,global.env.pos.y-150};
		point[1] = (GEOMETRY_point_t){global.env.pos.x,global.env.pos.y-175};
		point[2] = (GEOMETRY_point_t){global.env.pos.x+175,global.env.pos.y-175};



		/*point[0] = (GEOMETRY_point_t){1800,COLOR_Y(200)};
		point[1] = (GEOMETRY_point_t){1900,COLOR_Y(250)};
		point[2] = (GEOMETRY_point_t){1900,COLOR_Y(250)};*/
	}

	switch(state){
		case IDLE :
			//torch = TORCH_get_position(choiceTorch);

			torch.x = global.env.pos.x + 200;
			torch.y = global.env.pos.y;

			state = OPEN;
			i = 0;
			break;


		case OPEN :
			state = ACT_arm_move(ACT_ARM_POS_OPEN,0, 0, OPEN, TORCHE, PARKED_NOT_HANDLED);
			break;

		case TORCHE :
			state = ACT_arm_move(ACT_ARM_POS_ON_TORCHE,torch.x, torch.y, TORCHE, DOWN_ARM, OPEN_NOT_HANDLED);
			break;

		case DOWN_ARM: // Commentaire à enlever quand on aura le moteur DC sur le bras
			if(entrance){
				ACT_pompe_order(ACT_POMPE_NORMAL, 100);
				//ACT_arm_updown_goto(50); // gera niveau avec i
			}
			//if(ACT_get_last_action_result(ACT_QUEUE_Arm) == ACT_FUNCTION_Done){ // TODO faire une action plus haut niveau avec time out
				state = ELEMENT_wait_pump_capture_object(DOWN_ARM, UP_ARM, UP_ARM); // L'error est un TIMEOUT on passe quand même à l'étape suivante
			//}
			break;

		case UP_ARM: // Commentaire à enlever quand on aura le moteur DC sur le bras
			if((i == 1 && choiceTorch == OUR_TORCH) || ((i == 0 || i == 2) && choiceTorch == ADVERSARY_TORCH)) // Va retourne le deuxieme triangle
				state = BACK;//ACT_elevator_arm_move(100, UP_ARM, BACK, OPEN_NOT_HANDLED);
			else
				state = OPEN_2;//ACT_elevator_arm_move(100, UP_ARM, OPEN_2, OPEN_NOT_HANDLED);
			break;

		case OPEN_2:
			state = ACT_arm_move(ACT_ARM_POS_OPEN,0, 0, OPEN_2, FILED_TRIANGLE, OPEN_NOT_HANDLED);
			break;

		case FILED_TRIANGLE :
			if(entrance)
				i++;

			state = ACT_arm_move(ACT_ARM_POS_ON_TORCHE,point[i-1].x, point[i-1].y, FILED_TRIANGLE, WAIT_TRIANGLE_BREAK, OPEN_NOT_HANDLED);

			break;

		case WAIT_TRIANGLE_BREAK : // Attendre que le triangle soit relacher avant de faire autre chose
			if(entrance)
				ACT_pompe_order(ACT_POMPE_STOP, 0);
			//On peut avoir l'information avec le WT100 qui sera dans la main du robot
			state = (i>=2)? DONE:OPEN;
			break;

		case BACK:
			state = try_going(global.env.pos.x, global.env.pos.y+300, BACK, PREPARE_RETURN, OPEN_NOT_HANDLED, SLOW, BACKWARD, NO_DODGE_AND_WAIT);
			break;

		case PREPARE_RETURN:
			state = ACT_arm_move(ACT_ARM_POS_TO_PREPARE_RETURN,0, 0, PREPARE_RETURN, DOWN_RETURN, OPEN_NOT_HANDLED);
			break;

		case DOWN_RETURN:
			state = ACT_arm_move(ACT_ARM_POS_TO_DOWN_RETURN,0, 0, DOWN_RETURN, RETURN, OPEN_NOT_HANDLED);
			break;

		case RETURN:
			state = ACT_arm_move(ACT_ARM_POS_TO_RETURN,0, 0, RETURN, INVERSE_PUMP, OPEN_NOT_HANDLED);
			break;

		case INVERSE_PUMP:
			if(entrance)
				ACT_pompe_order(ACT_POMPE_REVERSE, 100);

			state = ELEMENT_wait_pump_capture_object(INVERSE_PUMP, DOWN_RETURN_2, DOWN_RETURN_2);
			break;

		case DOWN_RETURN_2:
			state = ACT_arm_move(ACT_ARM_POS_TO_DOWN_RETURN,0, 0, DOWN_RETURN_2, PREPARE_RETURN_2, OPEN_NOT_HANDLED);
			break;

		case PREPARE_RETURN_2:
			state = ACT_arm_move(ACT_ARM_POS_TO_PREPARE_RETURN,0, 0, PREPARE_RETURN_2, SMALL_ARM_DEPLOYED, OPEN_NOT_HANDLED);
			break;

		case SMALL_ARM_DEPLOYED:
			state = ACT_small_arm_move(ACT_SMALL_ARM_DEPLOYED, SMALL_ARM_DEPLOYED, SMALL_ARM_PARKED, OPEN_NOT_HANDLED);
			break;

		case SMALL_ARM_PARKED:
			if(entrance)
				ACT_pompe_order(ACT_POMPE_STOP, 0);

			state = ACT_small_arm_move(ACT_SMALL_ARM_IDLE, SMALL_ARM_PARKED, TAKE_RETURN, OPEN_NOT_HANDLED);
			break;

		case TAKE_RETURN:
			state = ACT_arm_move(ACT_ARM_POS_TO_TAKE_RETURN,0, 0, TAKE_RETURN, GRAP_TRIANGLE, OPEN_NOT_HANDLED);
			break;

		case GRAP_TRIANGLE: // prise du triangle au sol
			if(entrance){
				ACT_pompe_order(ACT_POMPE_NORMAL, 100);
			}
			state = ELEMENT_wait_pump_capture_object(GRAP_TRIANGLE, OPEN_3, OPEN_3); // L'error est un TIMEOUT on passe quand même à l'étape suivante
			break;

		case OPEN_3:
			state = ACT_arm_move(ACT_ARM_POS_OPEN,0, 0, OPEN_3, ADVANCE, OPEN_NOT_HANDLED);
			break;

		case ADVANCE:
			state = try_going(global.env.pos.x, global.env.pos.y-300, ADVANCE, OPEN_2, OPEN_NOT_HANDLED, SLOW, FORWARD, NO_DODGE_AND_WAIT);
			break;

		case OPEN_NOT_HANDLED:
			state = ACT_arm_move(ACT_ARM_POS_OPEN,0, 0, OPEN_NOT_HANDLED, PARKED_NOT_HANDLED, ERROR);
			break;

		case PARKED_NOT_HANDLED:
			state = ACT_arm_move(ACT_ARM_POS_PARKED,0, 0, PARKED_NOT_HANDLED, ERROR, ERROR);
			break;

		case DONE:
			ACT_pompe_order(ACT_POMPE_STOP, 0);
			state = IDLE;
			return END_OK;
			break;

		case ERROR:
			ACT_pompe_order(ACT_POMPE_STOP, 0);
			state = IDLE;
			return NOT_HANDLED;
			break;
	}

	return IN_PROGRESS;
}


error_e ACT_arm_move(ARM_state_e state_arm, Sint16 x, Sint16 y, Uint8 in_progress, Uint8 success_state, Uint8 fail_state){
	static time32_t begin_time;
	static bool_e entrance = TRUE;

	if(entrance){
		begin_time = global.env.match_time;

		if(state_arm == ACT_ARM_POS_ON_TORCHE || state_arm == ACT_ARM_POS_ON_TRIANGLE)
			ACT_arm_goto_XY(state_arm, x, y);
		else
			ACT_arm_goto(state_arm);


		entrance = FALSE;
	}

	if(global.env.match_time >= begin_time + ARM_TIMEOUT || ACT_get_last_action_result(ACT_QUEUE_Arm) == ACT_FUNCTION_ActDisabled || ACT_get_last_action_result(ACT_QUEUE_Arm) == ACT_FUNCTION_RetryLater){
		entrance = TRUE;
		return fail_state;
	}

	if(ACT_get_last_action_result(ACT_QUEUE_Arm) == ACT_FUNCTION_Done){
		entrance = TRUE;
		return success_state;
	}

	return in_progress;
}

error_e ACT_small_arm_move(Uint8 state_arm, Uint8 in_progress, Uint8 success_state, Uint8 fail_state){
	static time32_t begin_time;
	static bool_e entrance = TRUE;

	if(entrance){
		begin_time = global.env.match_time;
		ACT_small_arm_goto(state_arm);
		entrance = FALSE;
	}

	if(global.env.match_time >= begin_time + ARM_TIMEOUT || ACT_get_last_action_result(ACT_QUEUE_Small_arm) == ACT_FUNCTION_ActDisabled || ACT_get_last_action_result(ACT_QUEUE_Small_arm) == ACT_FUNCTION_RetryLater){
		entrance = TRUE;
		return fail_state;
	}

	if(ACT_get_last_action_result(ACT_QUEUE_Small_arm) == ACT_FUNCTION_Done){
		entrance = TRUE;
		return success_state;
	}

	return in_progress;
}

error_e ACT_elevator_arm_move(Uint8 state_arm, Uint8 in_progress, Uint8 success_state, Uint8 fail_state){
	static time32_t begin_time;
	static bool_e entrance = TRUE;

	if(entrance){
		begin_time = global.env.match_time;
		ACT_arm_updown_goto(state_arm);
		entrance = FALSE;
	}

	if(global.env.match_time >= begin_time + ARM_TIMEOUT || ACT_get_last_action_result(ACT_QUEUE_Arm) == ACT_FUNCTION_ActDisabled || ACT_get_last_action_result(ACT_QUEUE_Arm) == ACT_FUNCTION_RetryLater){
		entrance = TRUE;
		return fail_state;
	}

	if(ACT_get_last_action_result(ACT_QUEUE_Arm) == ACT_FUNCTION_Done){
		entrance = TRUE;
		return success_state;
	}

	return in_progress;
}

void strat_test_arm(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_SUB_GUY_TEST_ARM,
		IDLE,
		TRIANGLE,
		OPEN1,
		TORCHE,
		OPEN2,
		RETURN,
		OPEN3,
		PARKED,
		WAIT1,
		WAIT2,
		DONE
	);

	static time32_t time;

	switch(state){
		case IDLE :
			ASSER_set_position(500, 500, 0);
			state = OPEN1;
			break;


		case OPEN1 :
			if(entrance)
				ACT_arm_goto(ACT_ARM_POS_OPEN);
			if(ACT_get_last_action_result(ACT_QUEUE_Arm) == ACT_FUNCTION_Done)
				state = TORCHE;
			break;

		case TORCHE :
			if(entrance)
				ACT_arm_goto_XY(ACT_ARM_POS_ON_TORCHE, 500, 680);
			if(ACT_get_last_action_result(ACT_QUEUE_Arm) == ACT_FUNCTION_Done)
				state = WAIT1;
		break;

		case WAIT1 :
			if(entrance)
				time = global.env.match_time;
			if(global.env.match_time - time > 2000)
				state = OPEN2;
			break;

		case OPEN2:
			if(entrance)
				ACT_arm_goto(ACT_ARM_POS_OPEN);
			if(ACT_get_last_action_result(ACT_QUEUE_Arm) == ACT_FUNCTION_Done)
				state = TRIANGLE;
			break;

		case TRIANGLE :
			if(entrance)
				ACT_arm_goto_XY(ACT_ARM_POS_ON_TRIANGLE, 650, 500);
			if(ACT_get_last_action_result(ACT_QUEUE_Arm) == ACT_FUNCTION_Done)
				state = DONE;
			break;


		/*case TORCHE :
			if(entrance)
				ACT_arm_goto(ACT_ARM_POS_ON_TORCHE);
			if(ACT_get_last_action_result(ACT_QUEUE_Arm) == ACT_FUNCTION_Done)
				state = OPEN2;
			break;

		case OPEN2 :
			if(entrance)
				ACT_arm_goto(ACT_ARM_POS_OPEN);
			if(ACT_get_last_action_result(ACT_QUEUE_Arm) == ACT_FUNCTION_Done)
				state = RETURN;
			break;

		case RETURN :
			if(entrance)
				ACT_arm_goto(ACT_ARM_POS_TO_RETURN_TRIANGLE);
			if(ACT_get_last_action_result(ACT_QUEUE_Arm) == ACT_FUNCTION_Done)
				state = OPEN3;
			break;

		case OPEN3 :
			if(entrance)
				ACT_arm_goto(ACT_ARM_POS_OPEN);
			if(ACT_get_last_action_result(ACT_QUEUE_Arm) == ACT_FUNCTION_Done)
				state = PARKED;
			break;

		case PARKED :
			if(entrance)
				ACT_arm_goto(ACT_ARM_POS_PARKED);
			if(ACT_get_last_action_result(ACT_QUEUE_Arm) == ACT_FUNCTION_Done)
				state = OPEN1;
			break;*/

		case DONE :
			break;
	}
}
