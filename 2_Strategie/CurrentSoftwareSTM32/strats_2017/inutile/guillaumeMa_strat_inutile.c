#include "strat_inutile.h"
#include "../../propulsion/movement.h"
#include "../../QS/QS_stateMachineHelper.h"
#include  "../../propulsion/astar.h"
#include  "../../utils/generic_functions.h"
#include "../../actuator/act_functions.h"
#include "../../actuator/queue.h"
#include "../../utils/actionChecker.h"
#include "../big/action_big.h"

void guillaumeMa_strat_inutile_big(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_INUTILE,

			INIT,
			ERROR,
			TEST,
			TEST_2,
			TEST_3,
			TEST_4,
			DONE
	);
	switch(state){



		case INIT:
			state = TEST;
			break;// depend du point de depard

		case ERROR:
			state = ERROR;
			break;

		case DONE:
			state = DONE;
			break;

		case TEST:
			state = check_sub_action_result(sub_harry_take_big_crater(OUR_ELEMENT), state, DONE, ERROR);
			break;

		case TEST_2:
			state = check_sub_action_result(sub_harry_take_big_crater(ADV_ELEMENT), state, DONE, ERROR);
			break;

		case TEST_3:
			state = check_sub_action_result(sub_harry_take_big_crater(NO_ELEMENT), state, DONE, ERROR); // error
			break;

		case TEST_4:
			state = check_sub_action_result(sub_harry_take_big_crater(OUR_ELEMENT), state, DONE, ERROR);
			break;
	}
}



/*
void guillaumeMa_strat_inutile_big(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_INUTILE,

			INIT,
			ERROR,
			DONE,
			ERROR_ASTAR,
			GET_IN,
			GET_OUT,
			GET_OUT2,

			//deplacement
			GO_OUR_CRATER_FROM_ADV_SQUARE,
			ERROR_GO_OUR_CRATER_FROM_ADV_SQUARE,
			GO_OUR_CRATER_FROM_MIDDLE_SQUARE,
			ERROR_GO_OUR_CRATER_FROM_MIDDLE_SQUARE,
			GO_OUR_CRATER_FROM_OUR_SQUARE,
			ERROR_GO_OUR_CRATER_FROM_OUR_SQUARE,

			//ramassage
			DEBUT_RAMASSAGE,

			COLLECT_YELLOW_MIDDLE_POSITION,
			COLLECT_YELLOW_MIDDLE_LINE,
			COLLECT_YELLOW_MIDDLE_SHOVEL_DOWN,
			COLLECT_YELLOW_MIDDLE_MOVE_FOWARD,
			COLLECT_YELLOW_MIDDLE_SHOVEL_UP,
			COLLECT_YELLOW_MIDDLE_POSITION_LEFT,


			COLLECT_YELLOW_FUSE_POSITION,
			COLLECT_YELLOW_FUSE_LINE,
			COLLECT_YELLOW_FUSE_SHOVEL_DOWN,
			COLLECT_YELLOW_FUSE_MOVE_FOWARD,
			COLLECT_YELLOW_FUSE_SHOVEL_UP,
			COLLECT_YELLOW_FUSE_POSITION_LEFT,

			COLLECT_YELLOW_CORNER_POSITION,
			COLLECT_YELLOW_CORNER_LINE,
			COLLECT_YELLOW_CORNER_SHOVEL_DOWN,
			COLLECT_YELLOW_CORNER_MOVE_FOWARD,
			COLLECT_YELLOW_CORNER_SHOVEL_UP,
			COLLECT_YELLOW_CORNER_POSITION_LEFT,


			COLLECT_BLUE_MIDDLE_POSITION,
			COLLECT_BLUE_MIDDLE_LINE,
			COLLECT_BLUE_MIDDLE_SHOVEL_DOWN,
			COLLECT_BLUE_MIDDLE_MOVE_FOWARD,
			COLLECT_BLUE_MIDDLE_SHOVEL_UP,
			COLLECT_BLUE_MIDDLE_POSITION_LEFT,

			COLLECT_BLUE_FUSE_POSITION,
			COLLECT_BLUE_FUSE_LINE,
			COLLECT_BLUE_FUSE_SHOVEL_DOWN,
			COLLECT_BLUE_FUSE_MOVE_FOWARD,
			COLLECT_BLUE_FUSE_SHOVEL_UP,
			COLLECT_BLUE_FUSE_POSITION_LEFT,

			COLLECT_BLUE_CORNER_POSITION,
			COLLECT_BLUE_CORNER_LINE,
			COLLECT_BLUE_CORNER_SHOVEL_DOWN,
			COLLECT_BLUE_CORNER_MOVE_FOWARD,
			COLLECT_BLUE_CORNER_SHOVEL_UP,
			COLLECT_BLUE_CORNER_POSITION_LEFT


		);

	const displacement_t leave_middle_square[2] = { {(GEOMETRY_point_t){1000, COLOR_Y(800)}, FAST},
										  {(GEOMETRY_point_t){1380, COLOR_Y(400)}, FAST},
										  };

	const displacement_t leave_adv_square[3] = { {(GEOMETRY_point_t){1000, COLOR_Y(2200)}, FAST},
										  {(GEOMETRY_point_t){1000, COLOR_Y(800)}, FAST},
										  {(GEOMETRY_point_t){1380, COLOR_Y(400)}, FAST}
										  };



// petit sub et dessendre les billes porteuses
//valider tout les actionneurs
// test error
//test mouvement et taille carré
	switch(state){



		case INIT:
			state = try_going(1000, COLOR_Y(1500), state,GET_IN , ERROR, SLOW, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;// depend du point de depard

		case GET_IN:
			if(i_am_in_square_color(400, 1500, COLOR_Y(0), COLOR_Y(800))){
				state = GO_OUR_CRATER_FROM_OUR_SQUARE;
			}
			else if(i_am_in_square_color(400, 1500, COLOR_Y(3000), COLOR_Y(2200))){
				state = GO_OUR_CRATER_FROM_ADV_SQUARE;
			}
			else if(i_am_in_square_color(0, 1200, 800, 2200)){
				state = GO_OUR_CRATER_FROM_MIDDLE_SQUARE;
			}else{
				state = ASTAR_try_going(1390, COLOR_Y(400), state, DEBUT_RAMASSAGE,  ERROR_ASTAR, SLOW, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);
			}
			break; //1380 400


			// deplacement robot
		case GO_OUR_CRATER_FROM_ADV_SQUARE:
			state = try_going_multipoint( leave_adv_square, 3, state, DEBUT_RAMASSAGE, ERROR_GO_OUR_CRATER_FROM_ADV_SQUARE, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case GO_OUR_CRATER_FROM_MIDDLE_SQUARE:
			state = try_going_multipoint( leave_middle_square, 2, state, DEBUT_RAMASSAGE, ERROR_GO_OUR_CRATER_FROM_MIDDLE_SQUARE, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case GO_OUR_CRATER_FROM_OUR_SQUARE:
			state = try_going(1390, COLOR_Y(400), state, DEBUT_RAMASSAGE, ERROR_GO_OUR_CRATER_FROM_OUR_SQUARE, SLOW, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

//ERROR POSITION

		case ERROR_GO_OUR_CRATER_FROM_ADV_SQUARE:
			state = ASTAR_try_going(1000, COLOR_Y(1500), state, GET_IN, ERROR_ASTAR, SLOW, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case ERROR_GO_OUR_CRATER_FROM_MIDDLE_SQUARE:
			//state = GO_OUR_CRATERE_FROM_ADV_SQUARE;
			state = GET_IN;
			break;

		case ERROR_GO_OUR_CRATER_FROM_OUR_SQUARE:
			//state = GO_OUR_CRATERE_FROM_OUR_SQUARE;
			state = ASTAR_try_going(1390, COLOR_Y(400), state, DEBUT_RAMASSAGE,  ERROR_ASTAR, SLOW, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case ERROR_ASTAR:
			//action impossible abandon;
			state = DONE;
			break;

// possion atteinte debut ramassage

		case DEBUT_RAMASSAGE:
			if(global.color == YELLOW){
				state = COLLECT_YELLOW_MIDDLE_POSITION;
			}else if(global.color == BLUE){
				state = COLLECT_BLUE_MIDDLE_POSITION;
			}else{
				state = ERROR;
			}
			break;



//cote jaune
//1er passage
		case COLLECT_YELLOW_MIDDLE_POSITION:
			state = try_going(1390, 2600, state, COLLECT_YELLOW_MIDDLE_LINE,  ERROR, SLOW, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case COLLECT_YELLOW_MIDDLE_LINE:
			state = try_going(1390, 2600, state, COLLECT_YELLOW_MIDDLE_SHOVEL_DOWN,  ERROR, SLOW, FORWARD, DODGE_AND_WAIT, END_AT_LAST_POINT);
			//remonter les billes porteuses et descendre la roue
			ACT_push_order(ACT_BIG_BALL_FRONT_LEFT,  ACT_BIG_BALL_FRONT_LEFT_UP);
			ACT_push_order(ACT_BIG_BALL_FRONT_RIGHT,  ACT_BIG_BALL_FRONT_RIGHT_UP);
			ACT_push_order(ACT_BIG_BALL_BACK_LEFT,  ACT_BIG_BALL_FRONT_LEFT_UP);
			ACT_push_order(ACT_BIG_BALL_BACK_RIGHT,  ACT_BIG_BALL_BACK_RIGHT_UP);
			ACT_push_order(ACT_BEARING_BALL_WHEEL,  ACT_BEARING_BALL_WHEEL_DOWN);
			break;

		case COLLECT_YELLOW_MIDDLE_SHOVEL_DOWN:
			// baisser la pelle
			ACT_push_order(ACT_ORE_SHOVEL,  ACT_ORE_SHOVEL_DOWN);
			state = COLLECT_YELLOW_MIDDLE_MOVE_FOWARD;
			break;

		case COLLECT_YELLOW_MIDDLE_MOVE_FOWARD:
			state = try_going(1690, 2600, state, COLLECT_YELLOW_MIDDLE_SHOVEL_UP,  ERROR, SLOW, FORWARD, DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case COLLECT_YELLOW_MIDDLE_SHOVEL_UP:
			// monter la pelle et mélanger les balles
			ACT_push_order(ACT_ORE_SHOVEL,  ACT_ORE_SHOVEL_UP);
			//mélanger les balles
			state = COLLECT_YELLOW_MIDDLE_POSITION_LEFT;
			break;

		case COLLECT_YELLOW_MIDDLE_POSITION_LEFT:
			state = try_going(1390, 2600, state, COLLECT_YELLOW_FUSE_POSITION,  ERROR, SLOW, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);
			//lien avec action suivante
			break;

//2eme passage
		case COLLECT_YELLOW_FUSE_POSITION:
			state = try_going(1600, 2300, state, COLLECT_YELLOW_FUSE_LINE,  ERROR, SLOW, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case COLLECT_YELLOW_FUSE_LINE:
			state = try_going(1600, 2310, state,  COLLECT_YELLOW_FUSE_SHOVEL_DOWN,  ERROR, SLOW, FORWARD, DODGE_AND_WAIT, END_AT_LAST_POINT);
			//remonter les billes porteuses et descendre la roue
			ACT_push_order(ACT_BIG_BALL_FRONT_LEFT,  ACT_BIG_BALL_FRONT_LEFT_UP);
			ACT_push_order(ACT_BIG_BALL_FRONT_RIGHT,  ACT_BIG_BALL_FRONT_RIGHT_UP);
			ACT_push_order(ACT_BIG_BALL_BACK_LEFT,  ACT_BIG_BALL_FRONT_LEFT_UP);
			ACT_push_order(ACT_BIG_BALL_BACK_RIGHT,  ACT_BIG_BALL_BACK_RIGHT_UP);
			ACT_push_order(ACT_BEARING_BALL_WHEEL,  ACT_BEARING_BALL_WHEEL_DOWN);
			break;

		case COLLECT_YELLOW_FUSE_SHOVEL_DOWN:
			// baisser la pelle
			ACT_push_order(ACT_ORE_SHOVEL,  ACT_ORE_SHOVEL_DOWN);
			state = COLLECT_YELLOW_FUSE_MOVE_FOWARD;
			break;

		case COLLECT_YELLOW_FUSE_MOVE_FOWARD:
			state = try_going(1600, 2690, state, COLLECT_YELLOW_FUSE_SHOVEL_UP,  ERROR, SLOW, FORWARD, DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case COLLECT_YELLOW_FUSE_SHOVEL_UP:
			// monter la pelle et mélanger les balles
			ACT_push_order(ACT_ORE_SHOVEL,  ACT_ORE_SHOVEL_UP);
			//mélanger les balles
			state = COLLECT_YELLOW_FUSE_POSITION_LEFT;
			break;

		case COLLECT_YELLOW_FUSE_POSITION_LEFT:
			state = try_going(1600, 2300, state, COLLECT_YELLOW_CORNER_POSITION,  ERROR, SLOW, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);;
			//lien avec action suivante
			break;

//3eme passage
		case COLLECT_YELLOW_CORNER_POSITION:
			state = try_going(1780, 2200, state, COLLECT_YELLOW_CORNER_LINE,  ERROR, SLOW, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case COLLECT_YELLOW_CORNER_LINE:
			state = try_going(1780, 2210, state, COLLECT_YELLOW_CORNER_SHOVEL_DOWN,  ERROR, SLOW, FORWARD, DODGE_AND_WAIT, END_AT_LAST_POINT);
			//remonter les billes porteuses et descendre la roue
			ACT_push_order(ACT_BIG_BALL_FRONT_LEFT,  ACT_BIG_BALL_FRONT_LEFT_UP);
			ACT_push_order(ACT_BIG_BALL_FRONT_RIGHT,  ACT_BIG_BALL_FRONT_RIGHT_UP);
			ACT_push_order(ACT_BIG_BALL_BACK_LEFT,  ACT_BIG_BALL_FRONT_LEFT_UP);
			ACT_push_order(ACT_BIG_BALL_BACK_RIGHT,  ACT_BIG_BALL_BACK_RIGHT_UP);
			ACT_push_order(ACT_BEARING_BALL_WHEEL,  ACT_BEARING_BALL_WHEEL_DOWN);
			break;

		case COLLECT_YELLOW_CORNER_SHOVEL_DOWN:
			// baisser la pelle
			ACT_push_order(ACT_ORE_SHOVEL,  ACT_ORE_SHOVEL_DOWN);
			state = COLLECT_YELLOW_CORNER_MOVE_FOWARD;
			break;

		case COLLECT_YELLOW_CORNER_MOVE_FOWARD:
			state = try_going(1780, 2690, state, COLLECT_YELLOW_CORNER_SHOVEL_UP,  ERROR, SLOW, FORWARD, DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case COLLECT_YELLOW_CORNER_SHOVEL_UP:
			// monter la pelle et mélanger les balles
			ACT_push_order(ACT_ORE_SHOVEL,  ACT_ORE_SHOVEL_UP);
			//mélanger les balles
			state = COLLECT_YELLOW_CORNER_POSITION_LEFT;
			break;

		case COLLECT_YELLOW_CORNER_POSITION_LEFT:
			state = try_going(1780, 2200, state, GET_OUT,  ERROR, SLOW, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);
			//lien avec action suivante
			break;

//4eme passage
		/*case RAMASSAGE_JAUNE_PASS4_1:
			state = try_going(1600, COLOR_Y(700), state, DEBUT_RAMASSAGE,  ERROR, SLOW, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;
		*/

/*
//cote bleu
//1er passage
		case COLLECT_BLUE_MIDDLE_POSITION:
			state = try_going(1390, 400, state, COLLECT_BLUE_MIDDLE_LINE,  ERROR, SLOW, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case COLLECT_BLUE_MIDDLE_LINE:
			state = try_going(1690, 400, state, COLLECT_BLUE_MIDDLE_SHOVEL_DOWN,  ERROR, SLOW, FORWARD, DODGE_AND_WAIT, END_AT_LAST_POINT);
			//remonter les billes porteuses et descendre la roue
			ACT_push_order(ACT_BIG_BALL_FRONT_LEFT,  ACT_BIG_BALL_FRONT_LEFT_UP);
			ACT_push_order(ACT_BIG_BALL_FRONT_RIGHT,  ACT_BIG_BALL_FRONT_RIGHT_UP);
			ACT_push_order(ACT_BIG_BALL_BACK_LEFT,  ACT_BIG_BALL_FRONT_LEFT_UP);
			ACT_push_order(ACT_BIG_BALL_BACK_RIGHT,  ACT_BIG_BALL_BACK_RIGHT_UP);
			ACT_push_order(ACT_BEARING_BALL_WHEEL,  ACT_BEARING_BALL_WHEEL_DOWN);
			break;

		case COLLECT_BLUE_MIDDLE_SHOVEL_DOWN:
			// baisser la pelle
			ACT_push_order(ACT_ORE_SHOVEL,  ACT_ORE_SHOVEL_DOWN);
			state = COLLECT_BLUE_MIDDLE_MOVE_FOWARD;
			break;

		case COLLECT_BLUE_MIDDLE_MOVE_FOWARD:
			state = try_going(1690, 400, state, COLLECT_BLUE_MIDDLE_SHOVEL_UP,  ERROR, SLOW, FORWARD, DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case COLLECT_BLUE_MIDDLE_SHOVEL_UP:
			// monter la pelle et mélanger les balles
			ACT_push_order(ACT_ORE_SHOVEL,  ACT_ORE_SHOVEL_UP);
			//mélanger les balles
			state = COLLECT_BLUE_MIDDLE_POSITION_LEFT;
			break;

		case COLLECT_BLUE_MIDDLE_POSITION_LEFT:
			state = try_going(1390, 400, state, COLLECT_BLUE_FUSE_POSITION,  ERROR, SLOW, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);;
			//lien avec action suivante
			break;

			//2eme passage
		case COLLECT_BLUE_FUSE_POSITION:
			state = try_going(1600, 700, state, COLLECT_BLUE_FUSE_LINE,  ERROR, SLOW, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case COLLECT_BLUE_FUSE_LINE:
			state = try_going(1600, 310, state, COLLECT_BLUE_FUSE_SHOVEL_DOWN,  ERROR, SLOW, FORWARD, DODGE_AND_WAIT, END_AT_LAST_POINT);
			//remonter les billes porteuses et descendre la roue
			ACT_push_order(ACT_BIG_BALL_FRONT_LEFT,  ACT_BIG_BALL_FRONT_LEFT_UP);
			ACT_push_order(ACT_BIG_BALL_FRONT_RIGHT,  ACT_BIG_BALL_FRONT_RIGHT_UP);
			ACT_push_order(ACT_BIG_BALL_BACK_LEFT,  ACT_BIG_BALL_FRONT_LEFT_UP);
			ACT_push_order(ACT_BIG_BALL_BACK_RIGHT,  ACT_BIG_BALL_BACK_RIGHT_UP);
			ACT_push_order(ACT_BEARING_BALL_WHEEL,  ACT_BEARING_BALL_WHEEL_DOWN);
			break;

		case COLLECT_BLUE_FUSE_SHOVEL_DOWN:
			// baisser la pelle
			ACT_push_order(ACT_ORE_SHOVEL,  ACT_ORE_SHOVEL_DOWN);
			state = COLLECT_BLUE_FUSE_MOVE_FOWARD;
			break;

		case COLLECT_BLUE_FUSE_MOVE_FOWARD:
			state = try_going(1600, 310, state, COLLECT_BLUE_FUSE_SHOVEL_UP,  ERROR, SLOW, FORWARD, DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case COLLECT_BLUE_FUSE_SHOVEL_UP:
			// monter la pelle et mélanger les balles
			ACT_push_order(ACT_ORE_SHOVEL,  ACT_ORE_SHOVEL_UP);
			//mélanger les balles
			state = COLLECT_BLUE_FUSE_POSITION_LEFT;
			break;

		case COLLECT_BLUE_FUSE_POSITION_LEFT:
			state = try_going(1600, 700, state, COLLECT_BLUE_CORNER_POSITION,  ERROR, SLOW, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);;
			//lien avec action suivante
			break;

			//3eme passage
		case COLLECT_BLUE_CORNER_POSITION:
			state = try_going(1780, 800, state, COLLECT_BLUE_CORNER_LINE,  ERROR, SLOW, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case COLLECT_BLUE_CORNER_LINE:
			state = try_going(1780, 310, state, COLLECT_BLUE_CORNER_SHOVEL_DOWN,  ERROR, SLOW, FORWARD, DODGE_AND_WAIT, END_AT_LAST_POINT);
			//remonter les billes porteuses et descendre la roue
			ACT_push_order(ACT_BIG_BALL_FRONT_LEFT,  ACT_BIG_BALL_FRONT_LEFT_UP);
			ACT_push_order(ACT_BIG_BALL_FRONT_RIGHT,  ACT_BIG_BALL_FRONT_RIGHT_UP);
			ACT_push_order(ACT_BIG_BALL_BACK_LEFT,  ACT_BIG_BALL_FRONT_LEFT_UP);
			ACT_push_order(ACT_BIG_BALL_BACK_RIGHT,  ACT_BIG_BALL_BACK_RIGHT_UP);
			ACT_push_order(ACT_BEARING_BALL_WHEEL,  ACT_BEARING_BALL_WHEEL_DOWN);
			break;

		case COLLECT_BLUE_CORNER_SHOVEL_DOWN:
			// baisser la pelle
			ACT_push_order(ACT_ORE_SHOVEL,  ACT_ORE_SHOVEL_DOWN);
			state = COLLECT_BLUE_CORNER_MOVE_FOWARD;
			break;

		case COLLECT_BLUE_CORNER_MOVE_FOWARD:
			state = try_going(1780, 310, state, COLLECT_BLUE_CORNER_SHOVEL_UP,  ERROR, SLOW, FORWARD, DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case COLLECT_BLUE_CORNER_SHOVEL_UP:
			// monter la pelle et mélanger les balles
			ACT_push_order(ACT_ORE_SHOVEL,  ACT_ORE_SHOVEL_UP);
			//mélanger les balles
			state = GET_OUT;
			break;

		case COLLECT_BLUE_CORNER_POSITION_LEFT:
			state = try_going(1850, 800, state, GET_OUT,  ERROR, SLOW, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);
			//lien avec action suivante
			break;

			//4eme passage
					/*case RAMASSAGE_JAUNE_PASS4_1:
						state = try_going(1600, COLOR_Y(700), state, DEBUT_RAMASSAGE,  ERROR, SLOW, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);
						break;
					*/














/*

// fin de subaction sortie
		case GET_OUT:
			state = try_going(1300, COLOR_Y(400), state, DONE,  GET_OUT2, SLOW, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);;
			break;

		case GET_OUT2:
			state = try_going(1500, COLOR_Y(550), state, DONE,  GET_OUT, SLOW, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);;
			break;




		case ERROR:
			//return NOT_HANDLED; //definir le type et argument bleu/jaune d'entrée !
			break;

		case DONE:
			//return END_OK;
			break;

		//return IN_PROGRESS;
	}
}


*/
int NBERROR = 0;


void guillaumeMa_strat_inutile_small(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_ANNE_INUTILE,
			INIT,
			ERROR,
			DONE,
			POINT1,
			POINT2,
			POINT3,
			POINT4,
			POINT5,
			POINT6,
			POINT7,
			POINT8,
			POINT9,
			ERROR1,
			ERROR2,
			ERROR3
		);

	switch(state){ //sub sortir bascule anne
		case INIT:
			state = try_going(180, COLOR_Y(200), state, POINT1, ERROR, SLOW, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case POINT1:
			state = try_going(180, COLOR_Y(700), state, POINT2, ERROR, SLOW, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case POINT2:
			state = try_going(200, COLOR_Y(1150), state, POINT3, ERROR, SLOW, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case POINT3:// ramassage balle fusée dépard
			state = POINT4;
			break;

		case POINT4:
			state = try_going(1000, COLOR_Y(1000), state, POINT5, ERROR1, SLOW, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case POINT5:
			state = try_going(1350, COLOR_Y(100), state, POINT6, ERROR2, SLOW, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case POINT6:
			state = try_going(1350, COLOR_Y(300), state, POINT7, ERROR3, SLOW, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case POINT7:
			state = try_going(1000, COLOR_Y(1000), state, DONE, ERROR, SLOW, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

		case POINT8:
			state = ASTAR_try_going(200, COLOR_Y(1000), state, POINT9, ERROR, SLOW, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case POINT9:
			state = ASTAR_try_going(800, COLOR_Y(200), state, DONE, ERROR, SLOW, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case ERROR:
			break;

		case DONE:
			break;

		case ERROR1:
			if(ERROR <=3){
			state = POINT5;
			NBERROR = NBERROR +1;
			break;
			}else{//pb
			state = POINT2;
			NBERROR = 0;
			break;
			}

		case ERROR2:
			state = POINT4;
			break;

		case ERROR3:
			state = POINT5;
			break;
	}
}


