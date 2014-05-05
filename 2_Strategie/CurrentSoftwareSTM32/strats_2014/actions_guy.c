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
#include "../Geometry.h"
#include "../Pathfind.h"
#include "../zone_mutex.h"
#include "../QS/QS_can_over_xbee.h"
#include "../act_functions.h"
#include "../config/config_pin.h"
#include "../maths_home.h"
#include <math.h>

static void REACH_POINT_GET_OUT_INIT_send_request();

// Par defaut si les deux defines suivant sont desactiv�s le guy va passer par le foyer centrale
//#define WAY_INIT_TREE_SIDE  // Est prioritaire sur le chemin par le mammouth
//#define WAY_INIT_MAMMOUTH_SIDE


//#define DROP_TRIANGLE_UNDER_TREE    // Va deposer l'un des deux triangles sous les arbres


//#define FALL_FIRST_FIRE // Si on souhaite faire tomber le premier feux d�s le d�but

// Fonctionne que pour les chemins MAMMOUTH_SIDE et HEART_SIDE (chemin par defaut si aucun define)
bool_e rush_to_torch = FALSE;  // Si FALSE va faire tomber un ou des triangle(s) avant
bool_e fall_fire_wall_adv = TRUE;  // Va aller faire tomber le feu si on sait que l'ennemis ne le fais pas tomber des le debut


#define DIM_START_TRAVEL_TORCH 200
#define ARM_TIMEOUT 10000

/* ----------------------------------------------------------------------------- */
/* 							Fonctions d'homologation			                 */
/* ----------------------------------------------------------------------------- */


/* ----------------------------------------------------------------------------- */
/* 							Fonctions Belgique					                 */
/* ----------------------------------------------------------------------------- */

Uint8 putTorchInMidle(Uint8 currentState, Uint8 successState, Uint8 failState);
Uint8 putTorchInAdversary(Uint8 currentState, Uint8 successState, Uint8 failState);

void strat_belgique_guy(void){
	CREATE_MAE_WITH_VERBOSE(SM_ID_BELGIUM_STRAT,
		IDLE,
		EXTRACT_FROM_ZONE,
		WAIT,
		AVANCE,
		TOURNE,
		TOURNE2,
		OPEN_ARM,
		DEPLOY_ARM,
		CLOSE_ARM,
		PARK_ARM,
		RANGER,
		OPEN_END,
		CLOSE_END,
		TRIANGLE_1,
		TRIANGLE_1SHOOT,
		TRIANGLE_2,
		TRIANGLE_2SHOOT,
		TRIANGLE_3,
		TRIANGLE_3SHOOT,
		GO_FRESCO,
		BEFORE_WALL,
		PUSH_MOVE,
		WAIT_END_OF_MOVE,
		EXTRACTION,
		PASSAGE,
		AVANCE2,
		TOURNE3,
		TOURNE4,
		OPEN_ARM2,
		DEPLOY_ARM2,
		CLOSE_ARM2,
		PARK_ARM3,
		GO_TORCHE,
		TOURNE5,
		OPEN_ARM3,
		DEPLOY_ARM3,
		TOURNE6,
		CLOSE_ARM3,
		PARK_ARM2,
		/*DO_FIRST_TRIANGLE,
		CLOSE_ARM,
		PARK_ARM,
		POS_MID,
		TORCH,*/
		DONE,
		ERROR
	);
	static time32_t time;
	static bool_e timeout=FALSE;

	switch(state){
		case IDLE:
			state = EXTRACT_FROM_ZONE;
			break;

		case EXTRACT_FROM_ZONE:
			state = try_going(997,COLOR_Y(150),EXTRACT_FROM_ZONE,WAIT,EXTRACT_FROM_ZONE,
							  FAST,ANY_WAY,NO_DODGE_AND_NO_WAIT); // Pas d'�vitement on sort de la zone de d�part, il ne faut pas bloquer Pierre
			break;

		case WAIT :
			if(entrance)
				time = global.env.match_time;
			if(global.env.match_time - time > 5000)
				state = AVANCE;
			break;

		case AVANCE :
			state = try_going(610,COLOR_Y(150),AVANCE,TOURNE,RANGER,
							  FAST,ANY_WAY,NO_DODGE_AND_NO_WAIT);
			break;

		case TOURNE :
			if(global.env.color == RED)
				state = try_go_angle(-PI4096/2, TOURNE, OPEN_ARM, TRIANGLE_1, FAST);
			else
				state = try_go_angle(PI4096/2, TOURNE, OPEN_ARM, TRIANGLE_1, FAST);
			break;

		case OPEN_ARM:
			state = ACT_arm_move(ACT_ARM_POS_OPEN,0, 0, OPEN_ARM, DEPLOY_ARM, ERROR);
			break;

		case DEPLOY_ARM:
			if(global.env.color == RED)
				state = ACT_arm_move(ACT_ARM_POS_TO_PREPARE_RETURN,0, 0, DEPLOY_ARM, CLOSE_ARM, CLOSE_ARM);
			else
				state = ACT_arm_move(ACT_ARM_POS_TO_PREPARE_RETURN,0, 0, DEPLOY_ARM, TOURNE2, CLOSE_ARM);
			break;

		case TOURNE2:
			state = try_go_angle(0, TOURNE2, CLOSE_ARM, CLOSE_ARM, FAST);
			break;

		case CLOSE_ARM:
			state = ACT_arm_move(ACT_ARM_POS_OPEN,0, 0, CLOSE_ARM, PARK_ARM, PARK_ARM);
			break;

		case PARK_ARM:
			state = ACT_arm_move(ACT_ARM_POS_PARKED,0, 0, PARK_ARM, TRIANGLE_1, TRIANGLE_1);
			break;

		case RANGER :
			state = try_going(150, COLOR_Y(150), RANGER, OPEN_END, OPEN_END, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT);
			break;

		case OPEN_END :
			state = ACT_arm_move(ACT_ARM_POS_OPEN,0, 0, OPEN_END, CLOSE_END, CLOSE_END);
			break;

		case CLOSE_END :
			state = ACT_arm_move(ACT_ARM_POS_PARKED,0, 0, CLOSE_END, DONE, DONE);
			break;



		/*case DO_FIRST_TRIANGLE:
			state = try_going(1444, COLOR_Y(298), DO_FIRST_TRIANGLE, CLOSE_ARM, CLOSE_ARM, FAST, FORWARD, DODGE_AND_NO_WAIT);
			break;

		case CLOSE_ARM:
			state = ACT_arm_move(ACT_ARM_POS_OPEN,0, 0, CLOSE_ARM, PARK_ARM, PARK_ARM);
			break;

		case PARK_ARM:
			state = ACT_arm_move(ACT_ARM_POS_PARKED,0, 0, PARK_ARM, TORCH, TORCH);
			break;

		case TORCH:
			state = putTorchInAdversary(TORCH,POS_MID,ERROR);
			break;

		case POS_MID:
			state = try_going(1500, 1500, POS_MID, DONE, ERROR, FAST, ANY_WAY, DODGE_AND_WAIT);*/

		case TRIANGLE_1:
			state = try_going(825, COLOR_Y(389), TRIANGLE_1, TRIANGLE_1SHOOT, ERROR, FAST, FORWARD, NO_DODGE_AND_NO_WAIT);
			break;

		case TRIANGLE_1SHOOT:
			state = try_going(1109, COLOR_Y(380), TRIANGLE_1SHOOT, PASSAGE, ERROR, FAST, FORWARD, NO_DODGE_AND_NO_WAIT);
			break;

		case TRIANGLE_2:
			state = try_going(596, COLOR_Y(500), TRIANGLE_2, TRIANGLE_2SHOOT, ERROR, FAST, FORWARD, NO_DODGE_AND_NO_WAIT);
			break;

		case TRIANGLE_2SHOOT:
			state = try_going(596, COLOR_Y(858), TRIANGLE_2SHOOT, GO_FRESCO, ERROR, FAST, FORWARD, NO_DODGE_AND_NO_WAIT);
			break;

		case GO_FRESCO:
			state = try_going(500, COLOR_Y(1300), GO_FRESCO, BEFORE_WALL, ERROR, FAST, FORWARD, NO_DODGE_AND_NO_WAIT);
			break;

		case BEFORE_WALL:
			state = try_going(300, COLOR_Y(1300), BEFORE_WALL, PUSH_MOVE, ERROR, FAST, FORWARD, NO_DODGE_AND_NO_WAIT);
			break;

		case PUSH_MOVE://Le fait forcer contre le mur pour poser la fresque
			ASSER_push_rush_in_the_wall(FORWARD,TRUE,PI4096,TRUE);//Le fait forcer en marche arriere
			state = WAIT_END_OF_MOVE;
			break;

		case WAIT_END_OF_MOVE:
			if(STACKS_wait_end_auto_pull(ASSER, &timeout)){
				state = EXTRACTION;
			}
			break;

		case EXTRACTION:
			state = try_going(500, COLOR_Y(1300), EXTRACTION, PASSAGE, ERROR, FAST, BACKWARD, NO_DODGE_AND_NO_WAIT);
			break;

		case PASSAGE:
			state = try_going(1350, COLOR_Y(1000), PASSAGE, TRIANGLE_3, ERROR, FAST, BACKWARD, NO_DODGE_AND_NO_WAIT);
			break;

		case TRIANGLE_3:
			state = try_going(1600, COLOR_Y(1100), TRIANGLE_3, TRIANGLE_3SHOOT, ERROR, FAST, FORWARD, NO_DODGE_AND_NO_WAIT);
			break;

		case TRIANGLE_3SHOOT:
			state = try_going(1600, COLOR_Y(750), TRIANGLE_3SHOOT, AVANCE2, ERROR, FAST, FORWARD, NO_DODGE_AND_NO_WAIT);
			break;

		case AVANCE2 :
			state = try_going(2000-140,COLOR_Y(1500),AVANCE2,TOURNE3,ERROR,FAST,ANY_WAY,NO_DODGE_AND_NO_WAIT);
			break;

		case TOURNE3 :
			state = try_go_angle(0, TOURNE3, OPEN_ARM2, ERROR, FAST);
			break;

		case OPEN_ARM2:
			state = ACT_arm_move(ACT_ARM_POS_OPEN,0, 0, OPEN_ARM2, DEPLOY_ARM2, ERROR);
			break;

		case DEPLOY_ARM2:
			state = ACT_arm_move(ACT_ARM_POS_ON_TRIANGLE,global.env.pos.x + 200, global.env.pos.y, DEPLOY_ARM2, TOURNE4, TOURNE4);
			break;

		case TOURNE4 :
			if(global.env.color == RED)
				state = try_go_angle(PI4096/2, TOURNE4, CLOSE_ARM2, ERROR, FAST);
			else
				state = try_go_angle(-PI4096/2, TOURNE4, CLOSE_ARM2, ERROR, FAST);
			break;

		case CLOSE_ARM2:
			state = ACT_arm_move(ACT_ARM_POS_OPEN,0, 0, CLOSE_ARM2, PARK_ARM3, DONE);
			break;

		case PARK_ARM3:
			state = ACT_arm_move(ACT_ARM_POS_PARKED, 0, 0, PARK_ARM3, DONE, ERROR);
			break;


		case DONE:
			break;

		case ERROR:
			break;

		default:
			break;
	}
}


Uint8 putTorchInMidle(Uint8 currentState, Uint8 successState, Uint8 failState){
	CREATE_MAE_WITH_VERBOSE(SM_ID_BELGIUM_STRAT,
		IDLE,
		TORCHE_MOI_LA_FLAMME,
		CARRY_TORCH,
		EXTRACT_FROM_TORCH,
		BYPASS_TORCH,
		TORCHE_LA,
		DEPILE,
		DONE,
		ERROR
	);


	const displacement_t points_torcheMoiLaFlamme[] = {(displacement_t){{1000,COLOR_Y(600)},FAST},
													   (displacement_t){{1320,COLOR_Y(1200)},SLOW},
													   (displacement_t){{1380, COLOR_Y(1320)},SLOW}
													  };
	const displacement_t points_bypassTorch[] = {(displacement_t){{1570,COLOR_Y(1310)},FAST},
												 (displacement_t){{1510,COLOR_Y(1750)},FAST}};
	switch (state){
		case IDLE:
			state = TORCHE_MOI_LA_FLAMME;
			break;
		case TORCHE_MOI_LA_FLAMME:
			state = try_going_multipoint(points_torcheMoiLaFlamme,2,TORCHE_MOI_LA_FLAMME,CARRY_TORCH,ERROR,FORWARD,NO_DODGE_AND_WAIT,END_AT_BREAK);
			break;
		case CARRY_TORCH:
			state = try_going(1380,COLOR_Y(1320),CARRY_TORCH,EXTRACT_FROM_TORCH,ERROR,SLOW,FORWARD,NO_DODGE_AND_WAIT);
			break;
		case EXTRACT_FROM_TORCH:
			state = try_going(1390,COLOR_Y(1260),EXTRACT_FROM_TORCH,BYPASS_TORCH,ERROR,BACKWARD,FAST,NO_DODGE_AND_WAIT);
			break;
		case BYPASS_TORCH:
			state = try_going_multipoint(points_bypassTorch,2,BYPASS_TORCH,TORCHE_LA,ERROR,BACKWARD,NO_DODGE_AND_WAIT,END_AT_BREAK);
			break;
		case TORCHE_LA:
			state = try_going(1380,COLOR_Y(1650),TORCHE_LA,DEPILE,ERROR,SLOW,FORWARD,NO_DODGE_AND_WAIT);
			break;
		case DEPILE:
			state = check_sub_action_result(do_torch(OUR_TORCH,HEARTH_CENTRAL),DEPILE,DONE,ERROR);
			state = DONE;
			break;
		case DONE:
			return successState;
			break;
		case ERROR:
			return failState;
			break;
		default:
			break;
	}
	return currentState;
}

Uint8 putTorchInAdversary(Uint8 currentState, Uint8 successState, Uint8 failState){
	CREATE_MAE_WITH_VERBOSE(SM_ID_BELGIUM_STRAT,
		IDLE,
		TORCHE_MOI_LA_FLAMME,
		CARRY_TORCH,
		EXTRACT_FROM_TORCH,
		BYPASS_TORCH,
		TORCHE_LA,
		DEPILE,
		DONE,
		ERROR
	);


	const displacement_t points_torcheMoiLaFlamme[] = {(displacement_t){{900,COLOR_Y(600)},FAST},
													   (displacement_t){{1150,COLOR_Y(930)},FAST},
													   (displacement_t){{1600, COLOR_Y(1470)},FAST},
													   (displacement_t){{1730, COLOR_Y(1520)},FAST},
													   (displacement_t){{1640, COLOR_Y(2590)},FAST}
													  };
	const displacement_t points_bypassTorch[] = {(displacement_t){{1570,COLOR_Y(1310)},FAST},
												 (displacement_t){{1510,COLOR_Y(1750)},FAST}};
	switch (state){
		case IDLE:
			state = TORCHE_MOI_LA_FLAMME;
			break;
		case TORCHE_MOI_LA_FLAMME:
			state = try_going_multipoint(points_torcheMoiLaFlamme,5,TORCHE_MOI_LA_FLAMME,EXTRACT_FROM_TORCH,ERROR,FORWARD,NO_AVOIDANCE,END_AT_BREAK);
			break;
		case CARRY_TORCH:
			state = try_going(1600,COLOR_Y(2420),CARRY_TORCH,EXTRACT_FROM_TORCH,ERROR,SLOW,FORWARD,NO_DODGE_AND_WAIT);
			break;
		case EXTRACT_FROM_TORCH:
			state = try_going(1600,COLOR_Y(2420),EXTRACT_FROM_TORCH,DONE,ERROR,BACKWARD,FAST,NO_DODGE_AND_WAIT);
			break;
		case BYPASS_TORCH:
			state = try_going_multipoint(points_bypassTorch,2,BYPASS_TORCH,TORCHE_LA,ERROR,BACKWARD,NO_DODGE_AND_WAIT,END_AT_BREAK);
			break;
		case TORCHE_LA:
			state = try_going(1380,COLOR_Y(1650),TORCHE_LA,DEPILE,ERROR,SLOW,FORWARD,NO_DODGE_AND_WAIT);
			break;
		case DEPILE:
			//state = check_sub_action_result(do_torch(OUR_TORCH,HEARTH_CENTRAL),DEPILE,DONE,ERROR);
			state = DONE;
			break;
		case DONE:
			return successState;
			break;
		case ERROR:
			return failState;
			break;
		default:
			break;
	}
	return currentState;
}


/* ----------------------------------------------------------------------------- */
/* 							Autre strats de test			         			 */
/* ----------------------------------------------------------------------------- */


void strat_inutile_guy(void){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_GUY_INUTILE,
		IDLE,
		POS_DEPART,
		RAMEMENER_TORCH,
		DEPLOY_TORCH,
		DO_TORCH,
		POINT_1,
		POINT_2,
		POINT_3,
		DONE,
		ERROR
	);

	switch(state){
		case IDLE:
			state = POS_DEPART;
			break;
		case POS_DEPART:
			state = try_going_until_break(global.env.pos.x,COLOR_Y(450),POS_DEPART,POINT_1,ERROR,FAST,BACKWARD,NO_DODGE_AND_WAIT);
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

		case POINT_1:
			state = try_going(1350,400,POINT_1,POINT_2,POINT_1,FAST,ANY_WAY,NO_DODGE_AND_WAIT);
			break;

		case POINT_2:
			state = try_going(1650,1100,POINT_2,POINT_3,POINT_2,FAST,ANY_WAY,NO_DODGE_AND_WAIT);
			break;

		case POINT_3:
			state = try_going(520,1100,POINT_3,POINT_1,POINT_3,FAST,ANY_WAY,NO_DODGE_AND_WAIT);
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


// Strat�gie de test des d�ctections de triangle
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
				debug_printf("triangle n�3 pr�sent\n");
			else
				debug_printf("triangle n�3 non pr�sent\n");
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

		case WAIT_GOTO_MAMMOUTH_INIT:{  // Attend le passage de pierre pour pouvoir passer � son tour
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


/* Fait tomber le premier triangle par une rotation,
 * puis prend le 2�me et l'emm�ne soit sur le foyer
 * ou bien au pied d'un arbre
 */
error_e do_triangles_between_trees(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_SUB_GUY_DO_TRIANGLES_BETWEEN_TREES,
		IDLE,
		GET_IN,
		POS_START,
		ORIENTATION,
		GET_OUT_ARM,
		TURN,
		MOVE,
		TAKE_TRIANGLE,
		MOVE_DROP_TRIANGLE,
		DROP_TRIANGLE,
		MOVE_FORCED_DROP_HEARTH,
		FORCED_DROP_HEARTH,
		DROP_TRIANGLE_ANYWHERE,
		GET_OUT_SIDE_RED,
		GET_OUT_SIDE_MIDDLE,
		GET_OUT_SIDE_YELLOW,
		GET_OUT_M2,
		GET_OUT_W2,
		GET_OUT_C2,
		GET_OUT_Y3,
		GET_OUT_B3,
		GET_OUT_C3,
		GET_OUT_W3,
		DONE,
		ERROR,
		RETURN_NOT_HANDLED
	);

	static bool_e i_have_triangle = FALSE;
	static bool_e try_drop_triangle_center = FALSE;

	switch(state){
		case IDLE:

			if(est_dans_carre(1500, 2000, 1000, 2000, (GEOMETRY_point_t){global.env.pos.x, global.env.pos.y}))
				state = POS_START;				//On se rend � la premi�re position directement
			else
				state = GET_IN;						//On se rend � la premi�re position par le pathfind
			break;

		case GET_IN:
			state = PATHFIND_try_going(M3,GET_IN, POS_START, RETURN_NOT_HANDLED, ANY_WAY, FAST, DODGE_AND_WAIT, END_AT_BREAK);
			break;

		case POS_START:
			//en cas d'�chec, on peut rendre la main o� l'on se trouve.
			state = try_going(1800, 1500, POS_START, ORIENTATION, RETURN_NOT_HANDLED, FAST, FORWARD, NO_DODGE_AND_WAIT);
			break;

		case ORIENTATION:
			state = try_go_angle(0,ORIENTATION,GET_OUT_ARM,RETURN_NOT_HANDLED,FAST);
			break;

		case GET_OUT_ARM:

			// A remplir

			state = TURN;
			break;

		// Tourne pour faire tomber le premier trinagle
		case TURN:
			state = try_go_angle((global.env.color == RED)?PI4096/2:-PI4096/2,TURN,MOVE,RETURN_NOT_HANDLED,FAST);
			break;

		case MOVE: // Bouge vers le second triangle
			state = try_going(1800,(global.env.color == RED)?1300:1700,MOVE,TAKE_TRIANGLE,ERROR,FAST,ANY_WAY,NO_DODGE_AND_WAIT);
			break;

		case TAKE_TRIANGLE:
			state = MOVE_DROP_TRIANGLE;
			i_have_triangle = TRUE;
			break;

		case MOVE_DROP_TRIANGLE:
#ifdef DROP_TRIANGLE_UNDER_TREE
			state = try_going(1750,(global.env.color == RED)?2300:700,MOVE_DROP_TRIANGLE,TAKE_TRIANGLE,ERROR,FAST,ANY_WAY,NO_DODGE_AND_WAIT);
#else
			state = try_going(1350,1500,MOVE_DROP_TRIANGLE,TAKE_TRIANGLE,ERROR,FAST,ANY_WAY,NO_DODGE_AND_WAIT);
			try_drop_triangle_center = TRUE;
#endif
			break;

		case DROP_TRIANGLE:
			// A partir d'ici les get_out partent dans tous les sens
			//Pose le triangle � l'endroit voulu
#ifdef DROP_TRIANGLE_UNDER_TREE
			if(global.env.color == RED){
				state = GET_OUT_Y3;
			}else{
				state = GET_OUT_B3;
			}
#else
			state = GET_OUT_M2;
#endif
			i_have_triangle = FALSE;
			break;

		case DROP_TRIANGLE_ANYWHERE:
			state = RETURN_NOT_HANDLED;
			i_have_triangle = FALSE;
			break;


		// Si l'action de poser le triangle sous l'arbre a echou�, on r�ssaye sur le foyer
		case MOVE_FORCED_DROP_HEARTH:
			state = try_going(1350,1500,MOVE_FORCED_DROP_HEARTH,FORCED_DROP_HEARTH,ERROR,FAST,ANY_WAY,NO_DODGE_AND_WAIT);
			break;
		case FORCED_DROP_HEARTH:
			state = GET_OUT_M2;
			i_have_triangle = FALSE;
			break;

		// Sorti du bord
		case GET_OUT_SIDE_RED: // C3
			state = try_going(1650,1100,GET_OUT_SIDE_RED,RETURN_NOT_HANDLED,GET_OUT_SIDE_MIDDLE,FAST,ANY_WAY,NO_DODGE_AND_WAIT);
			break;
		case GET_OUT_SIDE_MIDDLE: // M3
			state = try_going(1700,1500,GET_OUT_SIDE_MIDDLE,RETURN_NOT_HANDLED,(global.env.color == RED)?GET_OUT_SIDE_RED:GET_OUT_SIDE_YELLOW,FAST,ANY_WAY,NO_DODGE_AND_WAIT);
			break;
		case GET_OUT_SIDE_YELLOW: // W3
			state = try_going(1650,1900,GET_OUT_SIDE_YELLOW,RETURN_NOT_HANDLED,GET_OUT_SIDE_MIDDLE,FAST,ANY_WAY,NO_DODGE_AND_WAIT);
			break;

		// Sorti du milieu va chercher a rejoindre soit le point M2 ou un point situ� chez l'adversaire pour ne pas d�ranger pierre
		case GET_OUT_M2:
			state = try_going(1450,1500,GET_OUT_M2,RETURN_NOT_HANDLED,(global.env.color == RED)?GET_OUT_W2:GET_OUT_C2,FAST,ANY_WAY,NO_DODGE_AND_WAIT);
			break;
		case GET_OUT_W2:
			state = try_going(1250,1900,GET_OUT_W2,RETURN_NOT_HANDLED,GET_OUT_M2,FAST,ANY_WAY,NO_DODGE_AND_WAIT);
			break;
		case GET_OUT_C2:
			state = try_going(1250,1100,GET_OUT_C2,RETURN_NOT_HANDLED,GET_OUT_M2,FAST,ANY_WAY,NO_DODGE_AND_WAIT);
			break;

		// Sorti de sous l'arbre jaune
		case GET_OUT_Y3:
			state = try_going(1600,2250,GET_OUT_Y3,(last_state == DROP_TRIANGLE)? DONE:RETURN_NOT_HANDLED,GET_OUT_W3,FAST,ANY_WAY,NO_DODGE_AND_WAIT);
			break;
		case GET_OUT_W3:
			state = try_going(1650,1900,GET_OUT_W3,RETURN_NOT_HANDLED,GET_OUT_Y3,FAST,ANY_WAY,NO_DODGE_AND_WAIT);
			break;

		// Sorti de sous l'arbre rouge
		case GET_OUT_B3:
			state = try_going(1600,750,GET_OUT_B3,(last_state == DROP_TRIANGLE)? DONE:RETURN_NOT_HANDLED,GET_OUT_C3,FAST,ANY_WAY,NO_DODGE_AND_WAIT);
			break;
		case GET_OUT_C3:
			state = try_going(1650,1100,GET_OUT_C3,RETURN_NOT_HANDLED,GET_OUT_B3,FAST,ANY_WAY,NO_DODGE_AND_WAIT);
			break;


		case DONE:
			state = IDLE;
			return END_OK;
			break;

		case ERROR:
			if(est_dans_carre(1400, 1750, 600, 2400, (GEOMETRY_point_t){global.env.pos.x, global.env.pos.y})) // Est sur le pathfind
				state = RETURN_NOT_HANDLED;
			else if(est_dans_carre(1750, 2000, 1200, 1800, (GEOMETRY_point_t){global.env.pos.x, global.env.pos.y})) // Est sur le bord
				state = GET_OUT_SIDE_MIDDLE;
			else if(est_dans_carre(1750, 2000, 500, 1000, (GEOMETRY_point_t){global.env.pos.x, global.env.pos.y})) // sous l'arbre rouge
				state = GET_OUT_B3;
			else if(est_dans_carre(1750, 2000, 2000, 2500, (GEOMETRY_point_t){global.env.pos.x, global.env.pos.y})) // sous l'arbre jaune
				state = GET_OUT_Y3;
			else
				state = GET_OUT_M2;

			break;

		case RETURN_NOT_HANDLED :
			if(try_drop_triangle_center == FALSE && i_have_triangle == TRUE){ // Si nous avons essayer de poser le triangle sous l'arbre et l'action est echoue
				try_drop_triangle_center = TRUE;
				state = MOVE_FORCED_DROP_HEARTH;
			}else if(i_have_triangle == TRUE){ // Si nous avons toujours un triangle, on le pose ou nous sommes
				state = DROP_TRIANGLE_ANYWHERE;
			}else{
				state = IDLE;
				return NOT_HANDLED;
			}
			break;

		default:
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

	// S'�loigne � la fin de la pousser mais ralenti aussi avant la fin de la pousser de la torche
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

			torch.x = global.env.pos.x + 150;
			torch.y = global.env.pos.y;

			state = OPEN;
			i = 0;
			break;


		case OPEN :
			state = ACT_arm_move(ACT_ARM_POS_OPEN,0, 0, OPEN, TORCHE, PARKED_NOT_HANDLED);
			break;

		case TORCHE :
			state = ACT_arm_move(ACT_ARM_POS_ON_TORCHE,torch.x, torch.y, TORCHE, DOWN_ARM, PARKED_NOT_HANDLED);
			break;

		case DOWN_ARM: // Commentaire � enlever quand on aura le moteur DC sur le bras
			if(entrance){
				ACT_pompe_order(ACT_POMPE_NORMAL, 100);
				ACT_arm_updown_rush_in_the_floor(120-i*30); // gera niveau avec i
			}
			if(ACT_get_last_action_result(ACT_QUEUE_Arm) == ACT_FUNCTION_Done){ // TODO faire une action plus haut niveau avec time out
				state = ELEMENT_wait_pump_capture_object(DOWN_ARM, UP_ARM, UP_ARM); // L'error est un TIMEOUT on passe quand m�me � l'�tape suivante
			}
			break;

		case UP_ARM: // Commentaire � enlever quand on aura le moteur DC sur le bras
			if((i == 1 && choiceTorch == OUR_TORCH) || ((i == 0 || i == 2) && choiceTorch == ADVERSARY_TORCH)) // Va retourne le deuxieme triangle
				state = BACK;//ACT_elevator_arm_move(100, UP_ARM, BACK, OPEN_NOT_HANDLED);
			else
				state = OPEN_2;//ACT_elevator_arm_move(100, UP_ARM, OPEN_2, OPEN_NOT_HANDLED);
			break;

		case OPEN_2:
			state = ACT_arm_move(ACT_ARM_POS_OPEN,0, 0, OPEN_2, FILED_TRIANGLE, PARKED_NOT_HANDLED);
			break;

		case FILED_TRIANGLE :
			if(entrance)
				i++;

			state = ACT_arm_move(ACT_ARM_POS_ON_TORCHE,point[i-1].x, point[i-1].y, FILED_TRIANGLE, WAIT_TRIANGLE_BREAK, PARKED_NOT_HANDLED);

			break;

		case WAIT_TRIANGLE_BREAK : // Attendre que le triangle soit relacher avant de faire autre chose
			if(entrance)
				ACT_pompe_order(ACT_POMPE_STOP, 0);
			//On peut avoir l'information avec le WT100 qui sera dans la main du robot
			state = (i>=2)? DONE:OPEN;
			break;

		case BACK:
			state = try_going(global.env.pos.x, global.env.pos.y+300, BACK, PREPARE_RETURN, PARKED_NOT_HANDLED, SLOW, BACKWARD, NO_DODGE_AND_WAIT);
			break;

		case PREPARE_RETURN:
			state = ACT_arm_move(ACT_ARM_POS_TO_PREPARE_RETURN,0, 0, PREPARE_RETURN, DOWN_RETURN, PARKED_NOT_HANDLED);
			break;

		case DOWN_RETURN:
			state = ACT_arm_move(ACT_ARM_POS_TO_DOWN_RETURN,0, 0, DOWN_RETURN, RETURN, PARKED_NOT_HANDLED);
			break;

		case RETURN:
			state = ACT_arm_move(ACT_ARM_POS_TO_RETURN,0, 0, RETURN, INVERSE_PUMP, PARKED_NOT_HANDLED);
			break;

		case INVERSE_PUMP:
			if(entrance)
				ACT_pompe_order(ACT_POMPE_REVERSE, 100);

			state = ELEMENT_wait_pump_capture_object(INVERSE_PUMP, DOWN_RETURN_2, DOWN_RETURN_2);
			break;

		case DOWN_RETURN_2:
			state = ACT_arm_move(ACT_ARM_POS_TO_DOWN_RETURN,0, 0, DOWN_RETURN_2, PREPARE_RETURN_2, PARKED_NOT_HANDLED);
			break;

		case PREPARE_RETURN_2:
			state = ACT_arm_move(ACT_ARM_POS_TO_PREPARE_RETURN,0, 0, PREPARE_RETURN_2, SMALL_ARM_DEPLOYED, PARKED_NOT_HANDLED);
			break;

		case SMALL_ARM_DEPLOYED:
			state = ACT_small_arm_move(ACT_SMALL_ARM_DEPLOYED, SMALL_ARM_DEPLOYED, SMALL_ARM_PARKED, PARKED_NOT_HANDLED);
			break;

		case SMALL_ARM_PARKED:
			if(entrance)
				ACT_pompe_order(ACT_POMPE_STOP, 0);

			state = ACT_small_arm_move(ACT_SMALL_ARM_IDLE, SMALL_ARM_PARKED, TAKE_RETURN, PARKED_NOT_HANDLED);
			break;

		case TAKE_RETURN:
			state = ACT_arm_move(ACT_ARM_POS_TO_TAKE_RETURN,0, 0, TAKE_RETURN, GRAP_TRIANGLE, PARKED_NOT_HANDLED);
			break;

		case GRAP_TRIANGLE: // prise du triangle au sol
			if(entrance){
				ACT_pompe_order(ACT_POMPE_NORMAL, 100);
			}
			state = ELEMENT_wait_pump_capture_object(GRAP_TRIANGLE, OPEN_3, OPEN_3); // L'error est un TIMEOUT on passe quand m�me � l'�tape suivante
			break;

		case OPEN_3:
			state = ACT_arm_move(ACT_ARM_POS_OPEN,0, 0, OPEN_3, ADVANCE, PARKED_NOT_HANDLED);
			break;

		case ADVANCE:
			state = try_going(global.env.pos.x, global.env.pos.y-300, ADVANCE, OPEN_2, PARKED_NOT_HANDLED, SLOW, FORWARD, NO_DODGE_AND_WAIT);
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
//		RETURN,
//		OPEN3,
//		PARKED,
		WAIT1,
//		WAIT2,
		DONE
	);

	static time32_t time;

	switch(state){
		case IDLE :
			ASSER_set_position(500, 500, 0);
			global.env.pos.x = 500;
			global.env.pos.y = 500;
			global.env.pos.angle = 0;

			state = OPEN1;
			break;


		case OPEN1 :
			state = check_sub_action_result(ACT_arm_deploy_torche(OUR_TORCH, HEARTH_OUR), OPEN1, DONE, DONE);
			break;

		/*case TORCHE :
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
			break;*/


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
