#include "action_big.h"
#include "../../propulsion/movement.h"
#include "../../propulsion/astar.h"
#include "../../QS/QS_stateMachineHelper.h"
#include  "../../utils/generic_functions.h"
#include "../../actuator/act_functions.h"
#include "../../actuator/queue.h"
#include "../../utils/actionChecker.h"

error_e sub_harry_take_big_crater(ELEMENTS_property_e mimerais){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_TAKE_BIG_CRATER,
			INIT,
			SUB_MOVE_POS_YELLOW,
			SUB_TAKE_YELLOW_MIDDLE,
			SUB_TAKE_YELLOW_FUSE,
			SUB_TAKE_YELLOW_CORNER,
			SUB_MOVE_POS_BLUE,
			SUB_TAKE_BLUE_MIDDLE,
			SUB_TAKE_BLUE_FUSE,
			SUB_TAKE_BLUE_CORNER,
			COULEUR,
			ERROR,
			DONE
		);

	switch(state){
		case INIT:
			state = COULEUR;
			break;

		case ERROR:
			RESET_MAE();
			return NOT_HANDLED;
			break;

		case DONE:
			RESET_MAE();
			return END_OK;
			break;


		case COULEUR: //decision ou je vais ?  ///// modifier !  quel couleur ?, quel cote ?
					if(global.color == YELLOW){
						state = SUB_MOVE_POS_YELLOW;
					}else if(global.color == BLUE){
						state = SUB_MOVE_POS_BLUE;
					}else{
						state = ERROR;
					}
					break;

		case SUB_MOVE_POS_YELLOW:
			state = check_sub_action_result(sub_harry_take_big_crater_move_pos_yellow(), state, SUB_TAKE_YELLOW_MIDDLE, ERROR);
			break;

		case SUB_TAKE_YELLOW_MIDDLE:
			state = check_sub_action_result(sub_harry_take_big_crater_yellow_middle(), state, SUB_TAKE_YELLOW_FUSE, ERROR);
			break;

		case SUB_TAKE_YELLOW_FUSE:
			state = check_sub_action_result(sub_harry_take_big_crater_yellow_fuse(), state, SUB_TAKE_YELLOW_CORNER, ERROR);
			break;

		case SUB_TAKE_YELLOW_CORNER:
			state = check_sub_action_result(sub_harry_take_big_crater_yellow_corner(), state, DONE, ERROR);
			break;



		case SUB_MOVE_POS_BLUE:
			state = check_sub_action_result(sub_harry_take_big_crater_move_pos_blue(), state, SUB_TAKE_BLUE_MIDDLE, ERROR);
			break;

		case SUB_TAKE_BLUE_MIDDLE:
			state = check_sub_action_result(sub_harry_take_big_crater_blue_middle(), state, SUB_TAKE_BLUE_FUSE, ERROR);
			break;

		case SUB_TAKE_BLUE_FUSE:
			state = check_sub_action_result(sub_harry_take_big_crater_blue_fuse(), state, SUB_TAKE_BLUE_CORNER, ERROR);
			break;

		case SUB_TAKE_BLUE_CORNER:
			state = check_sub_action_result(sub_harry_take_big_crater_blue_corner(), state, DONE, ERROR);
			break;



	}

	return IN_PROGRESS;
}

//sous sub deplacement
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



//cratere jaune modifier !
error_e sub_harry_take_big_crater_move_pos_yellow(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_TAKE_BIG_CRATER_MOVE_POS_YELLOW,
			INIT,
			GO_OUR_CRATER_FROM_ADV_SQUARE,
			ERROR_GO_OUR_CRATER_FROM_ADV_SQUARE,
			GO_OUR_CRATER_FROM_MIDDLE_SQUARE,
			ERROR_GO_OUR_CRATER_FROM_MIDDLE_SQUARE,
			GO_OUR_CRATER_FROM_OUR_SQUARE,
			ERROR_GO_OUR_CRATER_FROM_OUR_SQUARE,
			ASTAR_GO_OUR_CRATER,
			ERROR_ASTAR,
			GET_IN,
			ERROR,
			DONE

		);

	const displacement_t leave_middle_square[2] = { {(GEOMETRY_point_t){1000, COLOR_Y(800)}, FAST},
										  {(GEOMETRY_point_t){1390, COLOR_Y(400)}, FAST},
										  };

	const displacement_t leave_adv_square[3] = { {(GEOMETRY_point_t){1000, COLOR_Y(2200)}, FAST},
										  {(GEOMETRY_point_t){1000, COLOR_Y(800)}, FAST},
										  {(GEOMETRY_point_t){1390, COLOR_Y(400)}, FAST}
										  };

	switch(state){
	// deplacement robot
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
			state = ASTAR_GO_OUR_CRATER;
		}
		break;


	case GO_OUR_CRATER_FROM_ADV_SQUARE:
		state = try_going_multipoint( leave_adv_square, 3, state, DONE, ERROR_GO_OUR_CRATER_FROM_ADV_SQUARE, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);
		break;

	case GO_OUR_CRATER_FROM_MIDDLE_SQUARE:
		state = try_going_multipoint( leave_middle_square, 2, state, DONE, ERROR_GO_OUR_CRATER_FROM_MIDDLE_SQUARE, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);
		break;

	case GO_OUR_CRATER_FROM_OUR_SQUARE:
		state = try_going(1390, COLOR_Y(400), state, DONE, ERROR_GO_OUR_CRATER_FROM_OUR_SQUARE, SLOW, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);
		break;

	case ASTAR_GO_OUR_CRATER:
		ASTAR_try_going(1390, COLOR_Y(400), state, DONE,  ERROR_ASTAR, SLOW, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);
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
		state = ASTAR_try_going(1390, COLOR_Y(400), state, DONE,  ERROR_ASTAR, SLOW, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);
		break;

	case ERROR_ASTAR:
		//action impossible abandon;
		state = DONE;
		break;

	case ERROR:
		RESET_MAE();
		return NOT_HANDLED;
		break;

	case DONE:
		RESET_MAE();
		return END_OK;
		break;


	}

	return IN_PROGRESS;
}



//cratere bleu
error_e sub_harry_take_big_crater_move_pos_blue(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_TAKE_BIG_CRATER_MOVE_POS_BLUE,
			INIT,
			GO_OUR_CRATER_FROM_ADV_SQUARE,
			ERROR_GO_OUR_CRATER_FROM_ADV_SQUARE,
			GO_OUR_CRATER_FROM_MIDDLE_SQUARE,
			ERROR_GO_OUR_CRATER_FROM_MIDDLE_SQUARE,
			GO_OUR_CRATER_FROM_OUR_SQUARE,
			ERROR_GO_OUR_CRATER_FROM_OUR_SQUARE,
			ASTAR_GO_OUR_CRATER,
			ERROR_ASTAR,
			GET_IN,
			ERROR,
			DONE

		);

	const displacement_t leave_middle_square[2] = { {(GEOMETRY_point_t){1000, COLOR_Y(800)}, FAST},
										  {(GEOMETRY_point_t){1390, COLOR_Y(400)}, FAST},
										  };

	const displacement_t leave_adv_square[3] = { {(GEOMETRY_point_t){1000, COLOR_Y(2200)}, FAST},
										  {(GEOMETRY_point_t){1000, COLOR_Y(800)}, FAST},
										  {(GEOMETRY_point_t){1390, COLOR_Y(400)}, FAST}
										  };

	switch(state){
	// deplacement robot
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
			state = ASTAR_GO_OUR_CRATER;
		}
		break;


	case GO_OUR_CRATER_FROM_ADV_SQUARE:
		state = try_going_multipoint( leave_adv_square, 3, state, DONE, ERROR_GO_OUR_CRATER_FROM_ADV_SQUARE, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);
		break;

	case GO_OUR_CRATER_FROM_MIDDLE_SQUARE:
		state = try_going_multipoint( leave_middle_square, 2, state, DONE, ERROR_GO_OUR_CRATER_FROM_MIDDLE_SQUARE, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);
		break;

	case GO_OUR_CRATER_FROM_OUR_SQUARE:
		state = try_going(1390, COLOR_Y(400), state, DONE, ERROR_GO_OUR_CRATER_FROM_OUR_SQUARE, SLOW, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);
		break;

	case ASTAR_GO_OUR_CRATER:
		ASTAR_try_going(1390, COLOR_Y(400), state, DONE,  ERROR_ASTAR, SLOW, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);
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
		state = ASTAR_try_going(1390, COLOR_Y(400), state, DONE,  ERROR_ASTAR, SLOW, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);
		break;

	case ERROR_ASTAR:
		//action impossible abandon;
		state = DONE;
		break;

	case ERROR:
		RESET_MAE();
		return NOT_HANDLED;
		break;

	case DONE:
		RESET_MAE();
		return END_OK;
		break;


	}

	return IN_PROGRESS;
}






//sous sub ramassage
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// 1
// COLLECT_YELLOW_MIDDLE
error_e sub_harry_take_big_crater_yellow_middle(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_TAKE_BIG_CRATER_YELLOW_MIDDLE,
			INIT,
			COLLECT_YELLOW_MIDDLE_POSITION,
			COLLECT_YELLOW_MIDDLE_LINE,
			COLLECT_YELLOW_MIDDLE_SHOVEL_DOWN,
			COLLECT_YELLOW_MIDDLE_MOVE_FOWARD,
			COLLECT_YELLOW_MIDDLE_SHOVEL_UP,
			COLLECT_YELLOW_MIDDLE_POSITION_LEFT,
			COLLECT_YELLOW_MIDDLE_VALIDATE,
			GET_OUT_COLLECT_YELLOW_MIDDLE,
			DONE,
			ERROR
		);

	switch(state){
		case INIT:
			state = COLLECT_YELLOW_MIDDLE_POSITION;
			break;

		case COLLECT_YELLOW_MIDDLE_POSITION:
			state = try_going(1390, 2600, state, COLLECT_YELLOW_MIDDLE_LINE,  ERROR, SLOW, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case COLLECT_YELLOW_MIDDLE_LINE:
			// rotate ??
			state = try_going(1390, 2600, state, COLLECT_YELLOW_MIDDLE_SHOVEL_DOWN,  ERROR, SLOW, FORWARD, DODGE_AND_WAIT, END_AT_LAST_POINT);
			//remonter les billes porteuses et descendre la roue
			ACT_push_order(ACT_BIG_BALL_FRONT_LEFT,  ACT_BIG_BALL_FRONT_LEFT_UP);
			ACT_push_order(ACT_BIG_BALL_FRONT_RIGHT,  ACT_BIG_BALL_FRONT_RIGHT_UP);
			ACT_push_order(ACT_BIG_BALL_BACK_LEFT,  ACT_BIG_BALL_FRONT_LEFT_UP);
			ACT_push_order(ACT_BIG_BALL_BACK_RIGHT,  ACT_BIG_BALL_BACK_RIGHT_UP);
			ACT_push_order(ACT_BEARING_BALL_WHEEL,  ACT_BEARING_BALL_WHEEL_DOWN);
			// valide ??
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
			state = try_going(1390, 2600, state, COLLECT_YELLOW_MIDDLE_VALIDATE,  ERROR, SLOW, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);
			//lien avec action suivante
			break;

		case COLLECT_YELLOW_MIDDLE_VALIDATE:
			//dessendre les billes porteuses et monter la roue
			ACT_push_order(ACT_BIG_BALL_FRONT_LEFT,  ACT_BIG_BALL_FRONT_LEFT_DOWN);
			ACT_push_order(ACT_BIG_BALL_FRONT_RIGHT,  ACT_BIG_BALL_FRONT_RIGHT_DOWN);
			ACT_push_order(ACT_BIG_BALL_BACK_LEFT,  ACT_BIG_BALL_FRONT_LEFT_DOWN);
			ACT_push_order(ACT_BIG_BALL_BACK_RIGHT,  ACT_BIG_BALL_BACK_RIGHT_DOWN);
			ACT_push_order(ACT_BEARING_BALL_WHEEL,  ACT_BEARING_BALL_WHEEL_UP);
			//lien avec action suivante
			state = GET_OUT_COLLECT_YELLOW_MIDDLE;
			break;

		case GET_OUT_COLLECT_YELLOW_MIDDLE:
			state = DONE;
			break;
			//COLLECT_YELLOW_FUSE_POSITION == GET_OUT_COLLECT_YELLOW_MIDDLE

		case ERROR:
			RESET_MAE();
			return NOT_HANDLED;
			break;

		case DONE:
			RESET_MAE();
			return END_OK;
			break;
	}

	return IN_PROGRESS;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// 2
// COLLECT_YELLOW_FUSE
error_e sub_harry_take_big_crater_yellow_fuse(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_TAKE_BIG_CRATER_YELLOW_FUSE,
			INIT,
			COLLECT_YELLOW_FUSE_POSITION,
			COLLECT_YELLOW_FUSE_LINE,
			COLLECT_YELLOW_FUSE_SHOVEL_DOWN,
			COLLECT_YELLOW_FUSE_MOVE_FOWARD,
			COLLECT_YELLOW_FUSE_SHOVEL_UP,
			COLLECT_YELLOW_FUSE_POSITION_LEFT,
			COLLECT_YELLOW_FUSE_VALIDATE,
			GET_OUT_COLLECT_YELLOW_FUSE,
			DONE,
			ERROR
		);

	switch(state){
		case INIT:
			state = COLLECT_YELLOW_FUSE_POSITION;
			break;

		case COLLECT_YELLOW_FUSE_POSITION:
			state = try_going(1600, 2300, state, COLLECT_YELLOW_FUSE_LINE,  ERROR, SLOW, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case COLLECT_YELLOW_FUSE_LINE:
			state = try_going(1600, 2300, state,  COLLECT_YELLOW_FUSE_SHOVEL_DOWN,  ERROR, SLOW, FORWARD, DODGE_AND_WAIT, END_AT_LAST_POINT);
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
			state = try_going(1600, 2300, state, COLLECT_YELLOW_FUSE_VALIDATE,  ERROR, SLOW, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);;
			//lien avec action suivante
			break;

		case COLLECT_YELLOW_FUSE_VALIDATE:
			//dessendre les billes porteuses et monter la roue
			ACT_push_order(ACT_BIG_BALL_FRONT_LEFT,  ACT_BIG_BALL_FRONT_LEFT_DOWN);
			ACT_push_order(ACT_BIG_BALL_FRONT_RIGHT,  ACT_BIG_BALL_FRONT_RIGHT_DOWN);
			ACT_push_order(ACT_BIG_BALL_BACK_LEFT,  ACT_BIG_BALL_FRONT_LEFT_DOWN);
			ACT_push_order(ACT_BIG_BALL_BACK_RIGHT,  ACT_BIG_BALL_BACK_RIGHT_DOWN);
			ACT_push_order(ACT_BEARING_BALL_WHEEL,  ACT_BEARING_BALL_WHEEL_UP);
			//lien avec action suivante
			state = GET_OUT_COLLECT_YELLOW_FUSE;
			break;

		case GET_OUT_COLLECT_YELLOW_FUSE:
			state = DONE;
			break;

		case ERROR:
			RESET_MAE();
			return NOT_HANDLED;
			break;

		case DONE:
			RESET_MAE();
			return END_OK;
			break;

			//COLLECT_YELLOW_CORNER_POSITION == GET_OUT_COLLECT_YELLOW_FUSE
	}

	return IN_PROGRESS;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// 3
//COLLECT_YELLOW_CORNER
	error_e sub_harry_take_big_crater_yellow_corner(){
		CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_TAKE_BIG_CRATER_YELLOW_CORNER,
				INIT,
				COLLECT_YELLOW_CORNER_POSITION,
				COLLECT_YELLOW_CORNER_LINE,
				COLLECT_YELLOW_CORNER_SHOVEL_DOWN,
				COLLECT_YELLOW_CORNER_MOVE_FOWARD,
				COLLECT_YELLOW_CORNER_SHOVEL_UP,
				COLLECT_YELLOW_CORNER_POSITION_LEFT,
				COLLECT_YELLOW_CORNER_VALIDATE,
				GET_OUT_COLLECT_YELLOW_CORNER,
				DONE,
				ERROR
			);

	switch(state){
		case INIT:
			state = COLLECT_YELLOW_CORNER_POSITION;
			break;

		case COLLECT_YELLOW_CORNER_POSITION:
			state = try_going(1780, 2200, state, COLLECT_YELLOW_CORNER_LINE,  ERROR, SLOW, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case COLLECT_YELLOW_CORNER_LINE:
			state = try_going(1780, 2200, state, COLLECT_YELLOW_CORNER_SHOVEL_DOWN,  ERROR, SLOW, FORWARD, DODGE_AND_WAIT, END_AT_LAST_POINT);
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
			state = try_going(1780, 2200, state, COLLECT_YELLOW_CORNER_VALIDATE,  ERROR, SLOW, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);
			//lien avec action suivante
			break;

		case COLLECT_YELLOW_CORNER_VALIDATE:
			//dessendre les billes porteuses et monter la roue
			ACT_push_order(ACT_BIG_BALL_FRONT_LEFT,  ACT_BIG_BALL_FRONT_LEFT_DOWN);
			ACT_push_order(ACT_BIG_BALL_FRONT_RIGHT,  ACT_BIG_BALL_FRONT_RIGHT_DOWN);
			ACT_push_order(ACT_BIG_BALL_BACK_LEFT,  ACT_BIG_BALL_FRONT_LEFT_DOWN);
			ACT_push_order(ACT_BIG_BALL_BACK_RIGHT,  ACT_BIG_BALL_BACK_RIGHT_DOWN);
			ACT_push_order(ACT_BEARING_BALL_WHEEL,  ACT_BEARING_BALL_WHEEL_UP);
			//lien avec action suivante
			state = GET_OUT_COLLECT_YELLOW_CORNER;
			break;

		case GET_OUT_COLLECT_YELLOW_CORNER:
			state = DONE;
			break;

		case ERROR:
			RESET_MAE();
			return NOT_HANDLED;
			break;

		case DONE:
			RESET_MAE();
			return END_OK;
			break;

			//GET_OUT == GET_OUT_COLLECT_YELLOW_CORNER
	}

	return IN_PROGRESS;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// 4
//COLLECT_BLUE_MIDDLE
error_e sub_harry_take_big_crater_blue_middle(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_TAKE_BIG_CRATER_BLUE_MIDDLE,
			INIT,
			COLLECT_BLUE_MIDDLE_POSITION,
			COLLECT_BLUE_MIDDLE_LINE,
			COLLECT_BLUE_MIDDLE_SHOVEL_DOWN,
			COLLECT_BLUE_MIDDLE_MOVE_FOWARD,
			COLLECT_BLUE_MIDDLE_SHOVEL_UP,
			COLLECT_BLUE_MIDDLE_POSITION_LEFT,
			COLLECT_BLUE_MIDDLE_VALIDATE,
			GET_OUT_COLLECT_BLUE_MIDDLE,
			DONE,
			ERROR
		);

	switch(state){
		case INIT:
			state = COLLECT_BLUE_MIDDLE_POSITION;
			break;

		case COLLECT_BLUE_MIDDLE_POSITION:
			state = try_going(1390, 400, state, COLLECT_BLUE_MIDDLE_LINE,  ERROR, SLOW, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case COLLECT_BLUE_MIDDLE_LINE:
			state = try_going(1390, 400, state, COLLECT_BLUE_MIDDLE_SHOVEL_DOWN,  ERROR, SLOW, FORWARD, DODGE_AND_WAIT, END_AT_LAST_POINT);
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
			state = try_going(1390, 400, state, COLLECT_BLUE_MIDDLE_VALIDATE,  ERROR, SLOW, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);;
			//lien avec action suivante
			break;

		case COLLECT_BLUE_MIDDLE_VALIDATE:
			//dessendre les billes porteuses et monter la roue
			ACT_push_order(ACT_BIG_BALL_FRONT_LEFT,  ACT_BIG_BALL_FRONT_LEFT_DOWN);
			ACT_push_order(ACT_BIG_BALL_FRONT_RIGHT,  ACT_BIG_BALL_FRONT_RIGHT_DOWN);
			ACT_push_order(ACT_BIG_BALL_BACK_LEFT,  ACT_BIG_BALL_FRONT_LEFT_DOWN);
			ACT_push_order(ACT_BIG_BALL_BACK_RIGHT,  ACT_BIG_BALL_BACK_RIGHT_DOWN);
			ACT_push_order(ACT_BEARING_BALL_WHEEL,  ACT_BEARING_BALL_WHEEL_UP);
			//lien avec action suivante
			state = GET_OUT_COLLECT_BLUE_MIDDLE;
			break;

		case GET_OUT_COLLECT_BLUE_MIDDLE:
			state = DONE;
			break;

		case ERROR:
			RESET_MAE();
			return NOT_HANDLED;
			break;

		case DONE:
			RESET_MAE();
			return END_OK;
			break;
			// COLLECT_BLUE_FUSE_POSITION = GET_OUT_COLLECT_BLUE_MIDDLE
	}

	return IN_PROGRESS;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// 5
//COLLECT_BLUE_FUSE


error_e sub_harry_take_big_crater_blue_fuse(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_TAKE_BIG_CRATER_BLUE_FUSE,
			INIT,
			COLLECT_BLUE_FUSE_POSITION,
			COLLECT_BLUE_FUSE_LINE,
			COLLECT_BLUE_FUSE_SHOVEL_DOWN,
			COLLECT_BLUE_FUSE_MOVE_FOWARD,
			COLLECT_BLUE_FUSE_SHOVEL_UP,
			COLLECT_BLUE_FUSE_POSITION_LEFT,
			COLLECT_BLUE_FUSE_VALIDATE,
			GET_OUT_COLLECT_BLUE_FUSE,
			DONE,
			ERROR
		);

	switch(state){
		case INIT:
			state = COLLECT_BLUE_FUSE_POSITION;
			break;

		case COLLECT_BLUE_FUSE_POSITION:
			state = try_going(1600, 700, state, COLLECT_BLUE_FUSE_LINE,  ERROR, SLOW, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case COLLECT_BLUE_FUSE_LINE:
			state = try_going(1600, 700, state, COLLECT_BLUE_FUSE_SHOVEL_DOWN,  ERROR, SLOW, FORWARD, DODGE_AND_WAIT, END_AT_LAST_POINT);
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
			state = try_going(1600, 700, state, COLLECT_BLUE_FUSE_VALIDATE,  ERROR, SLOW, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);;
			//lien avec action suivante
			break;

		case COLLECT_BLUE_FUSE_VALIDATE:
			//dessendre les billes porteuses et monter la roue
			ACT_push_order(ACT_BIG_BALL_FRONT_LEFT,  ACT_BIG_BALL_FRONT_LEFT_DOWN);
			ACT_push_order(ACT_BIG_BALL_FRONT_RIGHT,  ACT_BIG_BALL_FRONT_RIGHT_DOWN);
			ACT_push_order(ACT_BIG_BALL_BACK_LEFT,  ACT_BIG_BALL_FRONT_LEFT_DOWN);
			ACT_push_order(ACT_BIG_BALL_BACK_RIGHT,  ACT_BIG_BALL_BACK_RIGHT_DOWN);
			ACT_push_order(ACT_BEARING_BALL_WHEEL,  ACT_BEARING_BALL_WHEEL_UP);
			//lien avec action suivante
			state = GET_OUT_COLLECT_BLUE_FUSE;
			break;

		case GET_OUT_COLLECT_BLUE_FUSE:
			state = DONE;
			break;

		case ERROR:
			RESET_MAE();
			return NOT_HANDLED;
			break;

		case DONE:
			RESET_MAE();
			return END_OK;
			break;

			//  COLLECT_BLUE_CORNER_POSITION = GET_OUT_COLLECT_BLUE_FUSE
	}

	return IN_PROGRESS;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// 6
//COLLECT_BLUE_CORNER
error_e sub_harry_take_big_crater_blue_corner(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_TAKE_BIG_CRATER_BLUE_CORNER,
			INIT,
			COLLECT_BLUE_CORNER_POSITION,
			COLLECT_BLUE_CORNER_LINE,
			COLLECT_BLUE_CORNER_SHOVEL_DOWN,
			COLLECT_BLUE_CORNER_MOVE_FOWARD,
			COLLECT_BLUE_CORNER_SHOVEL_UP,
			COLLECT_BLUE_CORNER_POSITION_LEFT,
			COLLECT_BLUE_CORNER_VALIDATE,
			GET_OUT_COLLECT_BLUE_CORNER,
			DONE,
			ERROR
		);

	switch(state){
		case INIT:
			state = COLLECT_BLUE_CORNER_POSITION;
			break;

		case COLLECT_BLUE_CORNER_POSITION:
			state = try_going(1780, 800, state, COLLECT_BLUE_CORNER_LINE,  ERROR, SLOW, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case COLLECT_BLUE_CORNER_LINE:
			state = try_going(1780, 800, state, COLLECT_BLUE_CORNER_SHOVEL_DOWN,  ERROR, SLOW, FORWARD, DODGE_AND_WAIT, END_AT_LAST_POINT);
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
			state = COLLECT_BLUE_CORNER_POSITION_LEFT;
			break;

		case COLLECT_BLUE_CORNER_POSITION_LEFT:
			state = try_going(1780, 800, state, COLLECT_BLUE_CORNER_VALIDATE,  ERROR, SLOW, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);
			//lien avec action suivante 1850
			break;

		case COLLECT_BLUE_CORNER_VALIDATE:
			//dessendre les billes porteuses et monter la roue
			ACT_push_order(ACT_BIG_BALL_FRONT_LEFT,  ACT_BIG_BALL_FRONT_LEFT_DOWN);
			ACT_push_order(ACT_BIG_BALL_FRONT_RIGHT,  ACT_BIG_BALL_FRONT_RIGHT_DOWN);
			ACT_push_order(ACT_BIG_BALL_BACK_LEFT,  ACT_BIG_BALL_FRONT_LEFT_DOWN);
			ACT_push_order(ACT_BIG_BALL_BACK_RIGHT,  ACT_BIG_BALL_BACK_RIGHT_DOWN);
			ACT_push_order(ACT_BEARING_BALL_WHEEL,  ACT_BEARING_BALL_WHEEL_UP);
			//lien avec action suivante
			state = GET_OUT_COLLECT_BLUE_CORNER;
			break;

		case GET_OUT_COLLECT_BLUE_CORNER:
			state = DONE;
			break;

		case ERROR:
			RESET_MAE();
			return NOT_HANDLED;
			break;

		case DONE:
			RESET_MAE();
			return END_OK;
			break;
			//GET_OUT_COLLECT_BLUE_CORNER valide sorti

			// GET_OUT= GET_OUT_COLLECT_BLUE_CORNER
	}

	return IN_PROGRESS;
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// faire les nom sub et ajout ,deplacement revoir tout les sub
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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

		 case RAMASSAGE_JAUNE_PASS4_1*:
			state = try_going(1600, COLOR_Y(700), state, DEBUT_RAMASSAGE,  ERROR, SLOW, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;
		*/


//cote bleu
//1er passage
/*
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

					 case RAMASSAGE_JAUNE_PASS4_1:
						state = try_going(1600, COLOR_Y(700), state, DEBUT_RAMASSAGE,  ERROR, SLOW, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);
						break;
					*/
















// fin de subaction sortie
/*
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



int NBERROR = 0;



*/
