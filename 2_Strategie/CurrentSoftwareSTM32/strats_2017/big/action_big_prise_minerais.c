#include "action_big.h"
#include "../../propulsion/movement.h"
#include "../../propulsion/astar.h"
#include "../../QS/QS_stateMachineHelper.h"
#include  "../../utils/generic_functions.h"
#include "../../actuator/act_functions.h"
#include "../../actuator/queue.h"
#include "../../utils/actionChecker.h"

error_e sub_harry_take_big_crater(ELEMENTS_property_e minerais){ // OUR_ELEMENT / ADV_ELEMENT
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
			COLOR,
			ERROR,
			ERROR_SUB_YELLOW,
			ERROR_SUB_BLUE,
			DONE
		);

	switch(state){
		case INIT:
			state = COLOR;
			break;

		case ERROR:
			RESET_MAE();
			return NOT_HANDLED;
			break;

		case ERROR_SUB_YELLOW:
			RESET_MAE();
			//une sous sub a error
			return NOT_HANDLED;
			break;

		case ERROR_SUB_BLUE:
			RESET_MAE();
			//une sous sub a error
			return NOT_HANDLED;
			break;

		case DONE:
			RESET_MAE();
			return END_OK;
			break;


		case COLOR:
			if (minerais != OUR_ELEMENT || minerais != ADV_ELEMENT){
				RESET_MAE();
				return NOT_HANDLED;

			}else if(OUR_ELEMENT){
				if(global.color == YELLOW){
					state = SUB_MOVE_POS_YELLOW;
				}else{
					state = SUB_MOVE_POS_BLUE;
				}

			}else{
				if(global.color == YELLOW){
					state = SUB_MOVE_POS_YELLOW;
				}else{
					state = SUB_MOVE_POS_BLUE;
				}

			}

			break;



		case SUB_MOVE_POS_YELLOW:
			state = check_sub_action_result(sub_harry_take_big_crater_move_pos_yellow(), state, SUB_TAKE_YELLOW_MIDDLE, ERROR_SUB_YELLOW);
			break;

		case SUB_TAKE_YELLOW_MIDDLE:
			state = check_sub_action_result(sub_harry_take_big_crater_yellow_middle(), state, SUB_TAKE_YELLOW_FUSE, ERROR_SUB_YELLOW);
			break;

		case SUB_TAKE_YELLOW_FUSE:
			state = check_sub_action_result(sub_harry_take_big_crater_yellow_fuse(), state, SUB_TAKE_YELLOW_CORNER, ERROR_SUB_YELLOW);
			break;

		case SUB_TAKE_YELLOW_CORNER:
			state = check_sub_action_result(sub_harry_take_big_crater_yellow_corner(), state, DONE, ERROR_SUB_YELLOW);
			break;





		case SUB_MOVE_POS_BLUE:
			state = check_sub_action_result(sub_harry_take_big_crater_move_pos_blue(), state, SUB_TAKE_BLUE_MIDDLE, ERROR_SUB_BLUE);
			break;

		case SUB_TAKE_BLUE_MIDDLE:
			state = check_sub_action_result(sub_harry_take_big_crater_blue_middle(), state, SUB_TAKE_BLUE_FUSE, ERROR_SUB_BLUE);
			break;

		case SUB_TAKE_BLUE_FUSE:
			state = check_sub_action_result(sub_harry_take_big_crater_blue_fuse(), state, SUB_TAKE_BLUE_CORNER, ERROR_SUB_BLUE);
			break;

		case SUB_TAKE_BLUE_CORNER:
			state = check_sub_action_result(sub_harry_take_big_crater_blue_corner(), state, DONE, ERROR_SUB_BLUE);
			break;



	}

	return IN_PROGRESS;
}

//sous sub deplacement
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



//aller cratere jaune
error_e sub_harry_take_big_crater_move_pos_yellow(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_TAKE_BIG_CRATER_MOVE_POS_YELLOW,
			INIT,
			GO_YELLOW_CRATER_FROM_ADV_SQUARE,
			ERROR_GO_YELLOW_CRATER_FROM_ADV_SQUARE,
			GO_YELLOW_CRATER_FROM_MIDDLE_SQUARE,
			ERROR_GO_YELLOW_CRATER_FROM_MIDDLE_SQUARE,
			GO_YELLOW_CRATER_FROM_OUR_SQUARE,
			ERROR_GO_YELLOW_CRATER_FROM_OUR_SQUARE,
			ASTAR_GO_YELLOW_CRATER,
			ERROR_ASTAR,
			GET_IN,
			ERROR,
			DONE

		);

	const displacement_t leave_middle_square[2] = { {(GEOMETRY_point_t){1000, 2200}, FAST},
										  {(GEOMETRY_point_t){1390, 2600}, FAST},
										  };

	const displacement_t leave_adv_square[3] = { {(GEOMETRY_point_t){1000, 800}, FAST},
										  {(GEOMETRY_point_t){1000, 2200}, FAST},
										  {(GEOMETRY_point_t){1390, 2600}, FAST}
										  };

	switch(state){
	// deplacement robot
	case INIT:
		state = GET_IN;
		break;

	case GET_IN:
		if(i_am_in_square_color(400, 1500, 0, 800)){
			state = GO_YELLOW_CRATER_FROM_ADV_SQUARE;
		}
		else if(i_am_in_square_color(400, 1500, 3000, 2200)){
			state = GO_YELLOW_CRATER_FROM_OUR_SQUARE;
		}
		else if(i_am_in_square_color(0, 1200, 800, 2200)){
			state = GO_YELLOW_CRATER_FROM_MIDDLE_SQUARE;
		}else{
			state = ASTAR_GO_YELLOW_CRATER;
		}
		break;


	case GO_YELLOW_CRATER_FROM_ADV_SQUARE:
		state = try_going_multipoint( leave_adv_square, 3, state, DONE, ERROR_GO_YELLOW_CRATER_FROM_ADV_SQUARE, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);
		break;

	case GO_YELLOW_CRATER_FROM_MIDDLE_SQUARE:
		state = try_going_multipoint( leave_middle_square, 2, state, DONE, ERROR_GO_YELLOW_CRATER_FROM_MIDDLE_SQUARE, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);
		break;

	case GO_YELLOW_CRATER_FROM_OUR_SQUARE:
		state = try_going(1390, 2600, state, DONE, ERROR_GO_YELLOW_CRATER_FROM_OUR_SQUARE, SLOW, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);
		break;

	case ASTAR_GO_YELLOW_CRATER:
		ASTAR_try_going(1390, 2600, state, DONE,  ERROR_ASTAR, SLOW, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);
		break;

	//ERROR POSITION

	case ERROR_GO_YELLOW_CRATER_FROM_ADV_SQUARE:
		state = ASTAR_try_going(1390, 2600, state, DONE,  ERROR_ASTAR, SLOW, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);
		break;

	case ERROR_GO_YELLOW_CRATER_FROM_MIDDLE_SQUARE:
		//state = GO_OUR_CRATERE_FROM_ADV_SQUARE;
		state = ASTAR_try_going(1390, 2600, state, DONE,  ERROR_ASTAR, SLOW, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);
		break;

	case ERROR_GO_YELLOW_CRATER_FROM_OUR_SQUARE:
		//state = GO_OUR_CRATERE_FROM_OUR_SQUARE;
		state = ASTAR_try_going(1390, 2600, state, DONE,  ERROR_ASTAR, SLOW, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);
		break;

	case ERROR_ASTAR:
		//action, impossible d'allez au cratere
		RESET_MAE();
		return NOT_HANDLED;
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



//aller cratere bleu
error_e sub_harry_take_big_crater_move_pos_blue(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_TAKE_BIG_CRATER_MOVE_POS_BLUE,
			INIT,
			GO_BLUE_CRATER_FROM_ADV_SQUARE,
			ERROR_GO_BLUE_CRATER_FROM_ADV_SQUARE,
			GO_BLUE_CRATER_FROM_MIDDLE_SQUARE,
			ERROR_GO_BLUE_CRATER_FROM_MIDDLE_SQUARE,
			GO_BLUE_CRATER_FROM_OUR_SQUARE,
			ERROR_GO_BLUE_CRATER_FROM_OUR_SQUARE,
			ASTAR_GO_BLUE_CRATER,
			ERROR_ASTAR,
			GET_IN,
			ERROR,
			DONE

		);

	const displacement_t leave_middle_square[2] = { {(GEOMETRY_point_t){1000, 800}, FAST},
										  {(GEOMETRY_point_t){1390, 400}, FAST},
										  };

	const displacement_t leave_adv_square[3] = { {(GEOMETRY_point_t){1000, 2200}, FAST},
										  {(GEOMETRY_point_t){1000, 800}, FAST},
										  {(GEOMETRY_point_t){1390, 400}, FAST}
										  };

	switch(state){
	// deplacement robot
	case INIT:
		state = GET_IN;
		break;

	case GET_IN:
		if(i_am_in_square_color(400, 1500, 0, 800)){
			state = GO_BLUE_CRATER_FROM_OUR_SQUARE;
		}
		else if(i_am_in_square_color(400, 1500, 3000, 2200)){
			state = GO_BLUE_CRATER_FROM_ADV_SQUARE;
		}
		else if(i_am_in_square_color(0, 1200, 800, 2200)){
			state = GO_BLUE_CRATER_FROM_MIDDLE_SQUARE;
		}else{
			state = ASTAR_GO_BLUE_CRATER;
		}
		break;


	case GO_BLUE_CRATER_FROM_ADV_SQUARE:
		state = try_going_multipoint( leave_adv_square, 3, state, DONE, ERROR_GO_BLUE_CRATER_FROM_ADV_SQUARE, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);
		break;

	case GO_BLUE_CRATER_FROM_MIDDLE_SQUARE:
		state = try_going_multipoint( leave_middle_square, 2, state, DONE, ERROR_GO_BLUE_CRATER_FROM_MIDDLE_SQUARE, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);
		break;

	case GO_BLUE_CRATER_FROM_OUR_SQUARE:
		state = try_going(1390, 400, state, DONE, ERROR_GO_BLUE_CRATER_FROM_OUR_SQUARE, SLOW, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);
		break;

	case ASTAR_GO_BLUE_CRATER:
		ASTAR_try_going(1390, 400, state, DONE,  ERROR_ASTAR, SLOW, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);
		break;

	//ERROR POSITION

	case ERROR_GO_BLUE_CRATER_FROM_ADV_SQUARE:
		state = ASTAR_try_going(1390, 400, state, DONE,  ERROR_ASTAR, SLOW, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);
		break;

	case ERROR_GO_BLUE_CRATER_FROM_MIDDLE_SQUARE:
		//state = GO_OUR_CRATERE_FROM_ADV_SQUARE;
		state = ASTAR_try_going(1390, 400, state, DONE,  ERROR_ASTAR, SLOW, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);
		break;

	case ERROR_GO_BLUE_CRATER_FROM_OUR_SQUARE:
		//state = GO_OUR_CRATERE_FROM_OUR_SQUARE;
		state = ASTAR_try_going(1390, 400, state, DONE,  ERROR_ASTAR, SLOW, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);
		break;

	case ERROR_ASTAR:
		//action, impossible d'allez au cratere
		RESET_MAE();
		return NOT_HANDLED;
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
			COLLECT_YELLOW_MIDDLE_ACTIVATE,
			COLLECT_YELLOW_MIDDLE_MOVE_FOWARD,
			COLLECT_YELLOW_MIDDLE_ROLLER_UP,
			COLLECT_YELLOW_MIDDLE_POSITION_LEFT,
			COLLECT_YELLOW_MIDDLE_VALIDATE,
			GET_OUT_COLLECT_YELLOW_MIDDLE,
			ERROR_COLLECT_YELLOW_MIDDLE_MOVE_FOWARD,
			ERROR_COLLECT_YELLOW_MIDDLE_POSITION_LEFT,
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
			// rotate ?? sud
			state = try_going(1390, 2600, state, COLLECT_YELLOW_MIDDLE_ACTIVATE,  ERROR, SLOW, FORWARD, DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case COLLECT_YELLOW_MIDDLE_ACTIVATE:{
			Uint8 state1=COLLECT_YELLOW_MIDDLE_ACTIVATE;
			Uint8 state2=COLLECT_YELLOW_MIDDLE_ACTIVATE;
			Uint8 state3=COLLECT_YELLOW_MIDDLE_ACTIVATE;
			Uint8 state4=COLLECT_YELLOW_MIDDLE_ACTIVATE;
			Uint8 state5=COLLECT_YELLOW_MIDDLE_ACTIVATE;
			Uint8 state6=COLLECT_YELLOW_MIDDLE_ACTIVATE;


			// descendre le rouleau
			ACT_push_order(ACT_ORE_SHOVEL,  ACT_ORE_SHOVEL_DOWN);
			//remonter les billes porteuses et descendre la roue
			ACT_push_order(ACT_BIG_BALL_FRONT_LEFT,  ACT_BIG_BALL_FRONT_LEFT_UP);
			ACT_push_order(ACT_BIG_BALL_FRONT_RIGHT,  ACT_BIG_BALL_FRONT_RIGHT_UP);
			ACT_push_order(ACT_BIG_BALL_BACK_LEFT,  ACT_BIG_BALL_FRONT_LEFT_UP);
			ACT_push_order(ACT_BIG_BALL_BACK_RIGHT,  ACT_BIG_BALL_BACK_RIGHT_UP);
			ACT_push_order(ACT_BEARING_BALL_WHEEL,  ACT_BEARING_BALL_WHEEL_DOWN);

			state1= check_act_status(ACT_QUEUE_Ore_shovel, state1, COLLECT_YELLOW_MIDDLE_MOVE_FOWARD, ERROR);
			state2= check_act_status(ACT_QUEUE_Bearing_ball_wheel, state2, COLLECT_YELLOW_MIDDLE_MOVE_FOWARD, ERROR);
			state3= check_act_status(ACT_QUEUE_Big_bearing_back_left, state3, COLLECT_YELLOW_MIDDLE_MOVE_FOWARD, ERROR);
			state4= check_act_status(ACT_QUEUE_Big_bearing_back_right, state4, COLLECT_YELLOW_MIDDLE_MOVE_FOWARD, ERROR);
			state5= check_act_status(ACT_QUEUE_Big_bearing_front_left, state5, COLLECT_YELLOW_MIDDLE_MOVE_FOWARD, ERROR);
			state6= check_act_status(ACT_QUEUE_Big_bearing_front_right, state6, COLLECT_YELLOW_MIDDLE_MOVE_FOWARD, ERROR);


if((state1==ERROR)||(state2==ERROR)||(state3==ERROR)||(state4==ERROR)||(state5==ERROR)||(state6==ERROR)){
	state=ERROR;
}
else if((state1==COLLECT_YELLOW_MIDDLE_ACTIVATE)&&(state2==COLLECT_YELLOW_MIDDLE_ACTIVATE)&&(state3==COLLECT_YELLOW_MIDDLE_ACTIVATE)&&(state4==COLLECT_YELLOW_MIDDLE_ACTIVATE)&&(state5==COLLECT_YELLOW_MIDDLE_ACTIVATE)&&(state6==COLLECT_YELLOW_MIDDLE_ACTIVATE)){
	state=COLLECT_YELLOW_MIDDLE_MOVE_FOWARD;
}

			// valide ??
		}break;

		case COLLECT_YELLOW_MIDDLE_MOVE_FOWARD:
			state = try_going(1690, 2600, state, COLLECT_YELLOW_MIDDLE_ROLLER_UP,  ERROR_COLLECT_YELLOW_MIDDLE_MOVE_FOWARD, SLOW, FORWARD, DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case COLLECT_YELLOW_MIDDLE_ROLLER_UP:
			// accelere rouleau ou arr�t ...
			state = COLLECT_YELLOW_MIDDLE_POSITION_LEFT;
			break;

		case COLLECT_YELLOW_MIDDLE_POSITION_LEFT:
			state = try_going(1390, 2600, state, COLLECT_YELLOW_MIDDLE_VALIDATE,  ERROR, SLOW, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);
			//lien avec action suivante
			break;


		case COLLECT_YELLOW_MIDDLE_VALIDATE:{
			Uint8 state1=COLLECT_YELLOW_MIDDLE_VALIDATE;
			Uint8 state2=COLLECT_YELLOW_MIDDLE_VALIDATE;
			Uint8 state3=COLLECT_YELLOW_MIDDLE_VALIDATE;
			Uint8 state4=COLLECT_YELLOW_MIDDLE_VALIDATE;
			Uint8 state5=COLLECT_YELLOW_MIDDLE_VALIDATE;
			Uint8 state6=COLLECT_YELLOW_MIDDLE_VALIDATE;


			// remonter le rouleau
			ACT_push_order(ACT_ORE_SHOVEL,  ACT_ORE_SHOVEL_UP);
			//remonter les billes porteuses et descendre la roue
			ACT_push_order(ACT_BIG_BALL_FRONT_LEFT,  ACT_BIG_BALL_FRONT_LEFT_DOWN);
			ACT_push_order(ACT_BIG_BALL_FRONT_RIGHT,  ACT_BIG_BALL_FRONT_RIGHT_DOWN);
			ACT_push_order(ACT_BIG_BALL_BACK_LEFT,  ACT_BIG_BALL_FRONT_LEFT_DOWN);
			ACT_push_order(ACT_BIG_BALL_BACK_RIGHT,  ACT_BIG_BALL_BACK_RIGHT_DOWN);
			ACT_push_order(ACT_BEARING_BALL_WHEEL,  ACT_BEARING_BALL_WHEEL_UP);

			state1= check_act_status(ACT_QUEUE_Ore_shovel, state1, COLLECT_YELLOW_MIDDLE_MOVE_FOWARD, ERROR);
			state2= check_act_status(ACT_QUEUE_Bearing_ball_wheel, state2, COLLECT_YELLOW_MIDDLE_MOVE_FOWARD, ERROR);
			state3= check_act_status(ACT_QUEUE_Big_bearing_back_left, state3, COLLECT_YELLOW_MIDDLE_MOVE_FOWARD, ERROR);
			state4= check_act_status(ACT_QUEUE_Big_bearing_back_right, state4, COLLECT_YELLOW_MIDDLE_MOVE_FOWARD, ERROR);
			state5= check_act_status(ACT_QUEUE_Big_bearing_front_left, state5, COLLECT_YELLOW_MIDDLE_MOVE_FOWARD, ERROR);
			state6= check_act_status(ACT_QUEUE_Big_bearing_front_right, state6, COLLECT_YELLOW_MIDDLE_MOVE_FOWARD, ERROR);


if((state1==ERROR)||(state2==ERROR)||(state3==ERROR)||(state4==ERROR)||(state5==ERROR)||(state6==ERROR)){
	state=ERROR;
}
else if((state1==COLLECT_YELLOW_MIDDLE_VALIDATE)&&(state2==COLLECT_YELLOW_MIDDLE_VALIDATE)&&(state3==COLLECT_YELLOW_MIDDLE_VALIDATE)&&(state4==COLLECT_YELLOW_MIDDLE_VALIDATE)&&(state5==COLLECT_YELLOW_MIDDLE_VALIDATE)&&(state6==COLLECT_YELLOW_MIDDLE_VALIDATE)){
	state=GET_OUT_COLLECT_YELLOW_MIDDLE;
}


		}break;

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

		case ERROR_COLLECT_YELLOW_MIDDLE_MOVE_FOWARD:
			// il y a quelqu'un dans le cratere
			state = try_going(1390, 2600, state, COLLECT_YELLOW_MIDDLE_MOVE_FOWARD,  COLLECT_YELLOW_MIDDLE_MOVE_FOWARD, SLOW, FORWARD, DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case ERROR_COLLECT_YELLOW_MIDDLE_POSITION_LEFT:
			// il y a quelqu'un qui bloque la sortie du cratere
			state = try_going(1690, 2600, state, COLLECT_YELLOW_MIDDLE_POSITION_LEFT,  COLLECT_YELLOW_MIDDLE_POSITION_LEFT, SLOW, FORWARD, DODGE_AND_WAIT, END_AT_LAST_POINT);
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
			COLLECT_YELLOW_FUSE_ACTIVATE,
			COLLECT_YELLOW_FUSE_MOVE_FOWARD,
			COLLECT_YELLOW_FUSE_ROLLER_UP,
			COLLECT_YELLOW_FUSE_POSITION_LEFT,
			COLLECT_YELLOW_FUSE_VALIDATE,
			GET_OUT_COLLECT_YELLOW_FUSE,
			ERROR_COLLECT_YELLOW_FUSE_MOVE_FOWARD,
			ERROR_COLLECT_YELLOW_FUSE_POSITION_LEFT,
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
			//rotate
			state = try_going(1600, 2300, state,  COLLECT_YELLOW_FUSE_ACTIVATE,  ERROR, SLOW, FORWARD, DODGE_AND_WAIT, END_AT_LAST_POINT);

			break;

		case COLLECT_YELLOW_FUSE_ACTIVATE:{
			Uint8 state1=COLLECT_YELLOW_FUSE_ACTIVATE;
			Uint8 state2=COLLECT_YELLOW_FUSE_ACTIVATE;
			Uint8 state3=COLLECT_YELLOW_FUSE_ACTIVATE;
			Uint8 state4=COLLECT_YELLOW_FUSE_ACTIVATE;
			Uint8 state5=COLLECT_YELLOW_FUSE_ACTIVATE;
			Uint8 state6=COLLECT_YELLOW_FUSE_ACTIVATE;


			// descendre le rouleau
			ACT_push_order(ACT_ORE_SHOVEL,  ACT_ORE_SHOVEL_DOWN);
			//remonter les billes porteuses et descendre la roue
			ACT_push_order(ACT_BIG_BALL_FRONT_LEFT,  ACT_BIG_BALL_FRONT_LEFT_UP);
			ACT_push_order(ACT_BIG_BALL_FRONT_RIGHT,  ACT_BIG_BALL_FRONT_RIGHT_UP);
			ACT_push_order(ACT_BIG_BALL_BACK_LEFT,  ACT_BIG_BALL_FRONT_LEFT_UP);
			ACT_push_order(ACT_BIG_BALL_BACK_RIGHT,  ACT_BIG_BALL_BACK_RIGHT_UP);
			ACT_push_order(ACT_BEARING_BALL_WHEEL,  ACT_BEARING_BALL_WHEEL_DOWN);

			state1= check_act_status(ACT_QUEUE_Ore_shovel, state1, COLLECT_YELLOW_FUSE_MOVE_FOWARD, ERROR);
			state2= check_act_status(ACT_QUEUE_Bearing_ball_wheel, state2, COLLECT_YELLOW_FUSE_MOVE_FOWARD, ERROR);
			state3= check_act_status(ACT_QUEUE_Big_bearing_back_left, state3, COLLECT_YELLOW_FUSE_MOVE_FOWARD, ERROR);
			state4= check_act_status(ACT_QUEUE_Big_bearing_back_right, state4, COLLECT_YELLOW_FUSE_MOVE_FOWARD, ERROR);
			state5= check_act_status(ACT_QUEUE_Big_bearing_front_left, state5, COLLECT_YELLOW_FUSE_MOVE_FOWARD, ERROR);
			state6= check_act_status(ACT_QUEUE_Big_bearing_front_right, state6, COLLECT_YELLOW_FUSE_MOVE_FOWARD, ERROR);


if((state1==ERROR)||(state2==ERROR)||(state3==ERROR)||(state4==ERROR)||(state5==ERROR)||(state6==ERROR)){
	state=ERROR;
}
else if((state1==COLLECT_YELLOW_FUSE_ACTIVATE)&&(state2==COLLECT_YELLOW_FUSE_ACTIVATE)&&(state3==COLLECT_YELLOW_FUSE_ACTIVATE)&&(state4==COLLECT_YELLOW_FUSE_ACTIVATE)&&(state5==COLLECT_YELLOW_FUSE_ACTIVATE)&&(state6==COLLECT_YELLOW_FUSE_ACTIVATE)){
	state=COLLECT_YELLOW_FUSE_MOVE_FOWARD;
}
			}break;

		case COLLECT_YELLOW_FUSE_MOVE_FOWARD:
			state = try_going(1600, 2690, state, COLLECT_YELLOW_FUSE_ROLLER_UP,  ERROR, SLOW, FORWARD, DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case COLLECT_YELLOW_FUSE_ROLLER_UP:
			// accelere rouleau ou arr�t ...
			state = COLLECT_YELLOW_FUSE_POSITION_LEFT;
			break;

		case COLLECT_YELLOW_FUSE_POSITION_LEFT:
			state = try_going(1600, 2300, state, COLLECT_YELLOW_FUSE_VALIDATE,  ERROR, SLOW, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);;
			//lien avec action suivante
			break;

		case COLLECT_YELLOW_FUSE_VALIDATE:{
			Uint8 state1=COLLECT_YELLOW_FUSE_VALIDATE;
			Uint8 state2=COLLECT_YELLOW_FUSE_VALIDATE;
			Uint8 state3=COLLECT_YELLOW_FUSE_VALIDATE;
			Uint8 state4=COLLECT_YELLOW_FUSE_VALIDATE;
			Uint8 state5=COLLECT_YELLOW_FUSE_VALIDATE;
			Uint8 state6=COLLECT_YELLOW_FUSE_VALIDATE;


			// remonter le rouleau
			ACT_push_order(ACT_ORE_SHOVEL,  ACT_ORE_SHOVEL_UP);
			//remonter les billes porteuses et descendre la roue
			ACT_push_order(ACT_BIG_BALL_FRONT_LEFT,  ACT_BIG_BALL_FRONT_LEFT_DOWN);
			ACT_push_order(ACT_BIG_BALL_FRONT_RIGHT,  ACT_BIG_BALL_FRONT_RIGHT_DOWN);
			ACT_push_order(ACT_BIG_BALL_BACK_LEFT,  ACT_BIG_BALL_FRONT_LEFT_DOWN);
			ACT_push_order(ACT_BIG_BALL_BACK_RIGHT,  ACT_BIG_BALL_BACK_RIGHT_DOWN);
			ACT_push_order(ACT_BEARING_BALL_WHEEL,  ACT_BEARING_BALL_WHEEL_UP);

			state1= check_act_status(ACT_QUEUE_Ore_shovel, state1, COLLECT_YELLOW_FUSE_MOVE_FOWARD, ERROR);
			state2= check_act_status(ACT_QUEUE_Bearing_ball_wheel, state2, COLLECT_YELLOW_FUSE_MOVE_FOWARD, ERROR);
			state3= check_act_status(ACT_QUEUE_Big_bearing_back_left, state3, COLLECT_YELLOW_FUSE_MOVE_FOWARD, ERROR);
			state4= check_act_status(ACT_QUEUE_Big_bearing_back_right, state4, COLLECT_YELLOW_FUSE_MOVE_FOWARD, ERROR);
			state5= check_act_status(ACT_QUEUE_Big_bearing_front_left, state5, COLLECT_YELLOW_FUSE_MOVE_FOWARD, ERROR);
			state6= check_act_status(ACT_QUEUE_Big_bearing_front_right, state6, COLLECT_YELLOW_FUSE_MOVE_FOWARD, ERROR);


if((state1==ERROR)||(state2==ERROR)||(state3==ERROR)||(state4==ERROR)||(state5==ERROR)||(state6==ERROR)){
	state=ERROR;
}
else if((state1==COLLECT_YELLOW_FUSE_VALIDATE)&&(state2==COLLECT_YELLOW_FUSE_VALIDATE)&&(state3==COLLECT_YELLOW_FUSE_VALIDATE)&&(state4==COLLECT_YELLOW_FUSE_VALIDATE)&&(state5==COLLECT_YELLOW_FUSE_VALIDATE)&&(state6==COLLECT_YELLOW_FUSE_VALIDATE)){
	state=GET_OUT_COLLECT_YELLOW_FUSE;
}


		}break;

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

		case ERROR_COLLECT_YELLOW_FUSE_MOVE_FOWARD:
			// il y a quelqu'un dans le cratere
			state = try_going(1390, 2600, state, COLLECT_YELLOW_FUSE_MOVE_FOWARD,  COLLECT_YELLOW_FUSE_MOVE_FOWARD, SLOW, FORWARD, DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case ERROR_COLLECT_YELLOW_FUSE_POSITION_LEFT:
			// il y a quelqu'un qui bloque la sortie du cratere
			state = try_going(1690, 2600, state, COLLECT_YELLOW_FUSE_POSITION_LEFT,  COLLECT_YELLOW_FUSE_POSITION_LEFT, SLOW, FORWARD, DODGE_AND_WAIT, END_AT_LAST_POINT);
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
				COLLECT_YELLOW_CORNER_ACTIVATE,
				COLLECT_YELLOW_CORNER_MOVE_FOWARD,
				COLLECT_YELLOW_CORNER_ROLLER_UP,
				COLLECT_YELLOW_CORNER_POSITION_LEFT,
				COLLECT_YELLOW_CORNER_VALIDATE,
				GET_OUT_COLLECT_YELLOW_CORNER,
				ERROR_COLLECT_YELLOW_CORNER_MOVE_FOWARD,
				ERROR_COLLECT_YELLOW_CORNER_POSITION_LEFT,
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
			//rotate
			state = try_going(1780, 2200, state, COLLECT_YELLOW_CORNER_ACTIVATE,  ERROR, SLOW, FORWARD, DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;


		case COLLECT_YELLOW_CORNER_ACTIVATE:{
			Uint8 state1=COLLECT_YELLOW_CORNER_ACTIVATE;
			Uint8 state2=COLLECT_YELLOW_CORNER_ACTIVATE;
			Uint8 state3=COLLECT_YELLOW_CORNER_ACTIVATE;
			Uint8 state4=COLLECT_YELLOW_CORNER_ACTIVATE;
			Uint8 state5=COLLECT_YELLOW_CORNER_ACTIVATE;
			Uint8 state6=COLLECT_YELLOW_CORNER_ACTIVATE;


			// descendre le rouleau
			ACT_push_order(ACT_ORE_SHOVEL,  ACT_ORE_SHOVEL_DOWN);
			//remonter les billes porteuses et descendre la roue
			ACT_push_order(ACT_BIG_BALL_FRONT_LEFT,  ACT_BIG_BALL_FRONT_LEFT_UP);
			ACT_push_order(ACT_BIG_BALL_FRONT_RIGHT,  ACT_BIG_BALL_FRONT_RIGHT_UP);
			ACT_push_order(ACT_BIG_BALL_BACK_LEFT,  ACT_BIG_BALL_FRONT_LEFT_UP);
			ACT_push_order(ACT_BIG_BALL_BACK_RIGHT,  ACT_BIG_BALL_BACK_RIGHT_UP);
			ACT_push_order(ACT_BEARING_BALL_WHEEL,  ACT_BEARING_BALL_WHEEL_DOWN);

			state1= check_act_status(ACT_QUEUE_Ore_shovel, state1, COLLECT_YELLOW_CORNER_MOVE_FOWARD, ERROR);
			state2= check_act_status(ACT_QUEUE_Bearing_ball_wheel, state2, COLLECT_YELLOW_CORNER_MOVE_FOWARD, ERROR);
			state3= check_act_status(ACT_QUEUE_Big_bearing_back_left, state3, COLLECT_YELLOW_CORNER_MOVE_FOWARD, ERROR);
			state4= check_act_status(ACT_QUEUE_Big_bearing_back_right, state4, COLLECT_YELLOW_CORNER_MOVE_FOWARD, ERROR);
			state5= check_act_status(ACT_QUEUE_Big_bearing_front_left, state5, COLLECT_YELLOW_CORNER_MOVE_FOWARD, ERROR);
			state6= check_act_status(ACT_QUEUE_Big_bearing_front_right, state6, COLLECT_YELLOW_CORNER_MOVE_FOWARD, ERROR);


if((state1==ERROR)||(state2==ERROR)||(state3==ERROR)||(state4==ERROR)||(state5==ERROR)||(state6==ERROR)){
	state=ERROR;
}
else if((state1==COLLECT_YELLOW_CORNER_ACTIVATE)&&(state2==COLLECT_YELLOW_CORNER_ACTIVATE)&&(state3==COLLECT_YELLOW_CORNER_ACTIVATE)&&(state4==COLLECT_YELLOW_CORNER_ACTIVATE)&&(state5==COLLECT_YELLOW_CORNER_ACTIVATE)&&(state6==COLLECT_YELLOW_CORNER_ACTIVATE)){
	state=COLLECT_YELLOW_CORNER_MOVE_FOWARD;
}
			}break;

		case COLLECT_YELLOW_CORNER_MOVE_FOWARD:
			state = try_going(1780, 2690, state, COLLECT_YELLOW_CORNER_ROLLER_UP,  ERROR, SLOW, FORWARD, DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case COLLECT_YELLOW_CORNER_ROLLER_UP:
			// accelere rouleau ou arr�t ...
			state = COLLECT_YELLOW_CORNER_POSITION_LEFT;
			break;

		case COLLECT_YELLOW_CORNER_POSITION_LEFT:
			state = try_going(1780, 2200, state, COLLECT_YELLOW_CORNER_VALIDATE,  ERROR, SLOW, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);
			//lien avec action suivante
			break;

		case COLLECT_YELLOW_CORNER_VALIDATE:{
			Uint8 state1=COLLECT_YELLOW_CORNER_VALIDATE;
			Uint8 state2=COLLECT_YELLOW_CORNER_VALIDATE;
			Uint8 state3=COLLECT_YELLOW_CORNER_VALIDATE;
			Uint8 state4=COLLECT_YELLOW_CORNER_VALIDATE;
			Uint8 state5=COLLECT_YELLOW_CORNER_VALIDATE;
			Uint8 state6=COLLECT_YELLOW_CORNER_VALIDATE;


			// remonter le rouleau
			ACT_push_order(ACT_ORE_SHOVEL,  ACT_ORE_SHOVEL_UP);
			//remonter les billes porteuses et descendre la roue
			ACT_push_order(ACT_BIG_BALL_FRONT_LEFT,  ACT_BIG_BALL_FRONT_LEFT_DOWN);
			ACT_push_order(ACT_BIG_BALL_FRONT_RIGHT,  ACT_BIG_BALL_FRONT_RIGHT_DOWN);
			ACT_push_order(ACT_BIG_BALL_BACK_LEFT,  ACT_BIG_BALL_FRONT_LEFT_DOWN);
			ACT_push_order(ACT_BIG_BALL_BACK_RIGHT,  ACT_BIG_BALL_BACK_RIGHT_DOWN);
			ACT_push_order(ACT_BEARING_BALL_WHEEL,  ACT_BEARING_BALL_WHEEL_UP);

			state1= check_act_status(ACT_QUEUE_Ore_shovel, state1, COLLECT_YELLOW_CORNER_MOVE_FOWARD, ERROR);
			state2= check_act_status(ACT_QUEUE_Bearing_ball_wheel, state2, COLLECT_YELLOW_CORNER_MOVE_FOWARD, ERROR);
			state3= check_act_status(ACT_QUEUE_Big_bearing_back_left, state3, COLLECT_YELLOW_CORNER_MOVE_FOWARD, ERROR);
			state4= check_act_status(ACT_QUEUE_Big_bearing_back_right, state4, COLLECT_YELLOW_CORNER_MOVE_FOWARD, ERROR);
			state5= check_act_status(ACT_QUEUE_Big_bearing_front_left, state5, COLLECT_YELLOW_CORNER_MOVE_FOWARD, ERROR);
			state6= check_act_status(ACT_QUEUE_Big_bearing_front_right, state6, COLLECT_YELLOW_CORNER_MOVE_FOWARD, ERROR);


if((state1==ERROR)||(state2==ERROR)||(state3==ERROR)||(state4==ERROR)||(state5==ERROR)||(state6==ERROR)){
	state=ERROR;
}
else if((state1==COLLECT_YELLOW_CORNER_VALIDATE)&&(state2==COLLECT_YELLOW_CORNER_VALIDATE)&&(state3==COLLECT_YELLOW_CORNER_VALIDATE)&&(state4==COLLECT_YELLOW_CORNER_VALIDATE)&&(state5==COLLECT_YELLOW_CORNER_VALIDATE)&&(state6==COLLECT_YELLOW_CORNER_VALIDATE)){
	state=GET_OUT_COLLECT_YELLOW_CORNER;
}


		}break;

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

		case ERROR_COLLECT_YELLOW_CORNER_MOVE_FOWARD:
			// il y a quelqu'un dans le cratere
			state = try_going(1390, 2600, state, COLLECT_YELLOW_CORNER_MOVE_FOWARD,  COLLECT_YELLOW_CORNER_MOVE_FOWARD, SLOW, FORWARD, DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case ERROR_COLLECT_YELLOW_CORNER_POSITION_LEFT:
			// il y a quelqu'un qui bloque la sortie du cratere
			state = try_going(1690, 2600, state, COLLECT_YELLOW_CORNER_POSITION_LEFT,  COLLECT_YELLOW_CORNER_POSITION_LEFT, SLOW, FORWARD, DODGE_AND_WAIT, END_AT_LAST_POINT);
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
			COLLECT_BLUE_MIDDLE_ACTIVATE,
			COLLECT_BLUE_MIDDLE_MOVE_FOWARD,
			COLLECT_BLUE_MIDDLE_ROLLER_UP,
			COLLECT_BLUE_MIDDLE_POSITION_LEFT,
			COLLECT_BLUE_MIDDLE_VALIDATE,
			GET_OUT_COLLECT_BLUE_MIDDLE,
			ERROR_COLLECT_BLUE_MIDDLE_MOVE_FOWARD,
			ERROR_COLLECT_BLUE_MIDDLE_POSITION_LEFT,
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
			//rotate
			state = try_going(1390, 400, state, COLLECT_BLUE_MIDDLE_ACTIVATE,  ERROR, SLOW, FORWARD, DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;



		case COLLECT_BLUE_MIDDLE_ACTIVATE:{
			Uint8 state1=COLLECT_BLUE_MIDDLE_ACTIVATE;
			Uint8 state2=COLLECT_BLUE_MIDDLE_ACTIVATE;
			Uint8 state3=COLLECT_BLUE_MIDDLE_ACTIVATE;
			Uint8 state4=COLLECT_BLUE_MIDDLE_ACTIVATE;
			Uint8 state5=COLLECT_BLUE_MIDDLE_ACTIVATE;
			Uint8 state6=COLLECT_BLUE_MIDDLE_ACTIVATE;


			// descendre le rouleau
			ACT_push_order(ACT_ORE_SHOVEL,  ACT_ORE_SHOVEL_DOWN);
			//remonter les billes porteuses et descendre la roue
			ACT_push_order(ACT_BIG_BALL_FRONT_LEFT,  ACT_BIG_BALL_FRONT_LEFT_UP);
			ACT_push_order(ACT_BIG_BALL_FRONT_RIGHT,  ACT_BIG_BALL_FRONT_RIGHT_UP);
			ACT_push_order(ACT_BIG_BALL_BACK_LEFT,  ACT_BIG_BALL_FRONT_LEFT_UP);
			ACT_push_order(ACT_BIG_BALL_BACK_RIGHT,  ACT_BIG_BALL_BACK_RIGHT_UP);
			ACT_push_order(ACT_BEARING_BALL_WHEEL,  ACT_BEARING_BALL_WHEEL_DOWN);

			state1= check_act_status(ACT_QUEUE_Ore_shovel, state1, COLLECT_BLUE_MIDDLE_MOVE_FOWARD, ERROR);
			state2= check_act_status(ACT_QUEUE_Bearing_ball_wheel, state2, COLLECT_BLUE_MIDDLE_MOVE_FOWARD, ERROR);
			state3= check_act_status(ACT_QUEUE_Big_bearing_back_left, state3, COLLECT_BLUE_MIDDLE_MOVE_FOWARD, ERROR);
			state4= check_act_status(ACT_QUEUE_Big_bearing_back_right, state4, COLLECT_BLUE_MIDDLE_MOVE_FOWARD, ERROR);
			state5= check_act_status(ACT_QUEUE_Big_bearing_front_left, state5, COLLECT_BLUE_MIDDLE_MOVE_FOWARD, ERROR);
			state6= check_act_status(ACT_QUEUE_Big_bearing_front_right, state6, COLLECT_BLUE_MIDDLE_MOVE_FOWARD, ERROR);


if((state1==ERROR)||(state2==ERROR)||(state3==ERROR)||(state4==ERROR)||(state5==ERROR)||(state6==ERROR)){
	state=ERROR;
}
else if((state1==COLLECT_BLUE_MIDDLE_ACTIVATE)&&(state2==COLLECT_BLUE_MIDDLE_ACTIVATE)&&(state3==COLLECT_BLUE_MIDDLE_ACTIVATE)&&(state4==COLLECT_BLUE_MIDDLE_ACTIVATE)&&(state5==COLLECT_BLUE_MIDDLE_ACTIVATE)&&(state6==COLLECT_BLUE_MIDDLE_ACTIVATE)){
	state=COLLECT_BLUE_MIDDLE_MOVE_FOWARD;
}
			}break;

		case COLLECT_BLUE_MIDDLE_MOVE_FOWARD:
			state = try_going(1690, 400, state, COLLECT_BLUE_MIDDLE_ROLLER_UP,  ERROR, SLOW, FORWARD, DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case COLLECT_BLUE_MIDDLE_ROLLER_UP:
			// accelere rouleau ou arr�t ...
			state = COLLECT_BLUE_MIDDLE_POSITION_LEFT;
			break;

		case COLLECT_BLUE_MIDDLE_POSITION_LEFT:
			state = try_going(1390, 400, state, COLLECT_BLUE_MIDDLE_VALIDATE,  ERROR, SLOW, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);;
			//lien avec action suivante
			break;

		case COLLECT_BLUE_MIDDLE_VALIDATE:{
			Uint8 state1=COLLECT_BLUE_MIDDLE_VALIDATE;
			Uint8 state2=COLLECT_BLUE_MIDDLE_VALIDATE;
			Uint8 state3=COLLECT_BLUE_MIDDLE_VALIDATE;
			Uint8 state4=COLLECT_BLUE_MIDDLE_VALIDATE;
			Uint8 state5=COLLECT_BLUE_MIDDLE_VALIDATE;
			Uint8 state6=COLLECT_BLUE_MIDDLE_VALIDATE;


			// remonter le rouleau
			ACT_push_order(ACT_ORE_SHOVEL,  ACT_ORE_SHOVEL_UP);
			//remonter les billes porteuses et descendre la roue
			ACT_push_order(ACT_BIG_BALL_FRONT_LEFT,  ACT_BIG_BALL_FRONT_LEFT_DOWN);
			ACT_push_order(ACT_BIG_BALL_FRONT_RIGHT,  ACT_BIG_BALL_FRONT_RIGHT_DOWN);
			ACT_push_order(ACT_BIG_BALL_BACK_LEFT,  ACT_BIG_BALL_FRONT_LEFT_DOWN);
			ACT_push_order(ACT_BIG_BALL_BACK_RIGHT,  ACT_BIG_BALL_BACK_RIGHT_DOWN);
			ACT_push_order(ACT_BEARING_BALL_WHEEL,  ACT_BEARING_BALL_WHEEL_UP);

			state1= check_act_status(ACT_QUEUE_Ore_shovel, state1, COLLECT_BLUE_MIDDLE_MOVE_FOWARD, ERROR);
			state2= check_act_status(ACT_QUEUE_Bearing_ball_wheel, state2, COLLECT_BLUE_MIDDLE_MOVE_FOWARD, ERROR);
			state3= check_act_status(ACT_QUEUE_Big_bearing_back_left, state3, COLLECT_BLUE_MIDDLE_MOVE_FOWARD, ERROR);
			state4= check_act_status(ACT_QUEUE_Big_bearing_back_right, state4, COLLECT_BLUE_MIDDLE_MOVE_FOWARD, ERROR);
			state5= check_act_status(ACT_QUEUE_Big_bearing_front_left, state5, COLLECT_BLUE_MIDDLE_MOVE_FOWARD, ERROR);
			state6= check_act_status(ACT_QUEUE_Big_bearing_front_right, state6, COLLECT_BLUE_MIDDLE_MOVE_FOWARD, ERROR);


if((state1==ERROR)||(state2==ERROR)||(state3==ERROR)||(state4==ERROR)||(state5==ERROR)||(state6==ERROR)){
	state=ERROR;
}
else if((state1==COLLECT_BLUE_MIDDLE_VALIDATE)&&(state2==COLLECT_BLUE_MIDDLE_VALIDATE)&&(state3==COLLECT_BLUE_MIDDLE_VALIDATE)&&(state4==COLLECT_BLUE_MIDDLE_VALIDATE)&&(state5==COLLECT_BLUE_MIDDLE_VALIDATE)&&(state6==COLLECT_BLUE_MIDDLE_VALIDATE)){
	state=GET_OUT_COLLECT_BLUE_MIDDLE;
}


		}break;

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

		case ERROR_COLLECT_BLUE_MIDDLE_MOVE_FOWARD:
			// il y a quelqu'un dans le cratere
			state = try_going(1390, 2600, state, COLLECT_BLUE_MIDDLE_MOVE_FOWARD,  COLLECT_BLUE_MIDDLE_MOVE_FOWARD, SLOW, FORWARD, DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case ERROR_COLLECT_BLUE_MIDDLE_POSITION_LEFT:
			// il y a quelqu'un qui bloque la sortie du cratere
			state = try_going(1690, 2600, state, COLLECT_BLUE_MIDDLE_POSITION_LEFT,  COLLECT_BLUE_MIDDLE_POSITION_LEFT, SLOW, FORWARD, DODGE_AND_WAIT, END_AT_LAST_POINT);
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
			COLLECT_BLUE_FUSE_ACTIVATE,
			COLLECT_BLUE_FUSE_MOVE_FOWARD,
			COLLECT_BLUE_FUSE_ROLLER_UP,
			COLLECT_BLUE_FUSE_POSITION_LEFT,
			COLLECT_BLUE_FUSE_VALIDATE,
			GET_OUT_COLLECT_BLUE_FUSE,
			ERROR_COLLECT_BLUE_FUSE_MOVE_FOWARD,
			ERROR_COLLECT_BLUE_FUSE_POSITION_LEFT,
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
			//rotate
			state = try_going(1600, 700, state, COLLECT_BLUE_FUSE_ACTIVATE,  ERROR, SLOW, FORWARD, DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;


		case COLLECT_BLUE_FUSE_ACTIVATE:{
			Uint8 state1=COLLECT_BLUE_FUSE_ACTIVATE;
			Uint8 state2=COLLECT_BLUE_FUSE_ACTIVATE;
			Uint8 state3=COLLECT_BLUE_FUSE_ACTIVATE;
			Uint8 state4=COLLECT_BLUE_FUSE_ACTIVATE;
			Uint8 state5=COLLECT_BLUE_FUSE_ACTIVATE;
			Uint8 state6=COLLECT_BLUE_FUSE_ACTIVATE;


			// descendre le rouleau
			ACT_push_order(ACT_ORE_SHOVEL,  ACT_ORE_SHOVEL_DOWN);
			//remonter les billes porteuses et descendre la roue
			ACT_push_order(ACT_BIG_BALL_FRONT_LEFT,  ACT_BIG_BALL_FRONT_LEFT_UP);
			ACT_push_order(ACT_BIG_BALL_FRONT_RIGHT,  ACT_BIG_BALL_FRONT_RIGHT_UP);
			ACT_push_order(ACT_BIG_BALL_BACK_LEFT,  ACT_BIG_BALL_FRONT_LEFT_UP);
			ACT_push_order(ACT_BIG_BALL_BACK_RIGHT,  ACT_BIG_BALL_BACK_RIGHT_UP);
			ACT_push_order(ACT_BEARING_BALL_WHEEL,  ACT_BEARING_BALL_WHEEL_DOWN);

			state1= check_act_status(ACT_QUEUE_Ore_shovel, state1, COLLECT_BLUE_FUSE_MOVE_FOWARD, ERROR);
			state2= check_act_status(ACT_QUEUE_Bearing_ball_wheel, state2, COLLECT_BLUE_FUSE_MOVE_FOWARD, ERROR);
			state3= check_act_status(ACT_QUEUE_Big_bearing_back_left, state3, COLLECT_BLUE_FUSE_MOVE_FOWARD, ERROR);
			state4= check_act_status(ACT_QUEUE_Big_bearing_back_right, state4, COLLECT_BLUE_FUSE_MOVE_FOWARD, ERROR);
			state5= check_act_status(ACT_QUEUE_Big_bearing_front_left, state5, COLLECT_BLUE_FUSE_MOVE_FOWARD, ERROR);
			state6= check_act_status(ACT_QUEUE_Big_bearing_front_right, state6, COLLECT_BLUE_FUSE_MOVE_FOWARD, ERROR);


if((state1==ERROR)||(state2==ERROR)||(state3==ERROR)||(state4==ERROR)||(state5==ERROR)||(state6==ERROR)){
	state=ERROR;
}
else if((state1==COLLECT_BLUE_FUSE_ACTIVATE)&&(state2==COLLECT_BLUE_FUSE_ACTIVATE)&&(state3==COLLECT_BLUE_FUSE_ACTIVATE)&&(state4==COLLECT_BLUE_FUSE_ACTIVATE)&&(state5==COLLECT_BLUE_FUSE_ACTIVATE)&&(state6==COLLECT_BLUE_FUSE_ACTIVATE)){
	state=COLLECT_BLUE_FUSE_MOVE_FOWARD;
}
			}break;


		case COLLECT_BLUE_FUSE_MOVE_FOWARD:
			state = try_going(1600, 310, state, COLLECT_BLUE_FUSE_ROLLER_UP,  ERROR, SLOW, FORWARD, DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case COLLECT_BLUE_FUSE_ROLLER_UP:
			// accelere rouleau ou arr�t ...
			state = COLLECT_BLUE_FUSE_POSITION_LEFT;
			break;

		case COLLECT_BLUE_FUSE_POSITION_LEFT:
			state = try_going(1600, 700, state, COLLECT_BLUE_FUSE_VALIDATE,  ERROR, SLOW, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);;
			//lien avec action suivante
			break;

		case COLLECT_BLUE_FUSE_VALIDATE:{
			Uint8 state1=COLLECT_BLUE_FUSE_VALIDATE;
			Uint8 state2=COLLECT_BLUE_FUSE_VALIDATE;
			Uint8 state3=COLLECT_BLUE_FUSE_VALIDATE;
			Uint8 state4=COLLECT_BLUE_FUSE_VALIDATE;
			Uint8 state5=COLLECT_BLUE_FUSE_VALIDATE;
			Uint8 state6=COLLECT_BLUE_FUSE_VALIDATE;


			// remonter le rouleau
			ACT_push_order(ACT_ORE_SHOVEL,  ACT_ORE_SHOVEL_UP);
			//remonter les billes porteuses et descendre la roue
			ACT_push_order(ACT_BIG_BALL_FRONT_LEFT,  ACT_BIG_BALL_FRONT_LEFT_DOWN);
			ACT_push_order(ACT_BIG_BALL_FRONT_RIGHT,  ACT_BIG_BALL_FRONT_RIGHT_DOWN);
			ACT_push_order(ACT_BIG_BALL_BACK_LEFT,  ACT_BIG_BALL_FRONT_LEFT_DOWN);
			ACT_push_order(ACT_BIG_BALL_BACK_RIGHT,  ACT_BIG_BALL_BACK_RIGHT_DOWN);
			ACT_push_order(ACT_BEARING_BALL_WHEEL,  ACT_BEARING_BALL_WHEEL_UP);

			state1= check_act_status(ACT_QUEUE_Ore_shovel, state1, COLLECT_BLUE_FUSE_MOVE_FOWARD, ERROR);
			state2= check_act_status(ACT_QUEUE_Bearing_ball_wheel, state2, COLLECT_BLUE_FUSE_MOVE_FOWARD, ERROR);
			state3= check_act_status(ACT_QUEUE_Big_bearing_back_left, state3, COLLECT_BLUE_FUSE_MOVE_FOWARD, ERROR);
			state4= check_act_status(ACT_QUEUE_Big_bearing_back_right, state4, COLLECT_BLUE_FUSE_MOVE_FOWARD, ERROR);
			state5= check_act_status(ACT_QUEUE_Big_bearing_front_left, state5, COLLECT_BLUE_FUSE_MOVE_FOWARD, ERROR);
			state6= check_act_status(ACT_QUEUE_Big_bearing_front_right, state6, COLLECT_BLUE_FUSE_MOVE_FOWARD, ERROR);


if((state1==ERROR)||(state2==ERROR)||(state3==ERROR)||(state4==ERROR)||(state5==ERROR)||(state6==ERROR)){
	state=ERROR;
}
else if((state1==COLLECT_BLUE_FUSE_VALIDATE)&&(state2==COLLECT_BLUE_FUSE_VALIDATE)&&(state3==COLLECT_BLUE_FUSE_VALIDATE)&&(state4==COLLECT_BLUE_FUSE_VALIDATE)&&(state5==COLLECT_BLUE_FUSE_VALIDATE)&&(state6==COLLECT_BLUE_FUSE_VALIDATE)){
	state=GET_OUT_COLLECT_BLUE_FUSE;
}


		}break;

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

		case ERROR_COLLECT_BLUE_FUSE_MOVE_FOWARD:
			// il y a quelqu'un dans le cratere
			state = try_going(1390, 2600, state, COLLECT_BLUE_FUSE_MOVE_FOWARD,  COLLECT_BLUE_FUSE_MOVE_FOWARD, SLOW, FORWARD, DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case ERROR_COLLECT_BLUE_FUSE_POSITION_LEFT:
			// il y a quelqu'un qui bloque la sortie du cratere
			state = try_going(1690, 2600, state, COLLECT_BLUE_FUSE_POSITION_LEFT,  COLLECT_BLUE_FUSE_POSITION_LEFT, SLOW, FORWARD, DODGE_AND_WAIT, END_AT_LAST_POINT);
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
			COLLECT_BLUE_CORNER_ACTIVATE,
			COLLECT_BLUE_CORNER_MOVE_FOWARD,
			COLLECT_BLUE_CORNER_ROLLER_UP,
			COLLECT_BLUE_CORNER_POSITION_LEFT,
			COLLECT_BLUE_CORNER_VALIDATE,
			GET_OUT_COLLECT_BLUE_CORNER,
			ERROR_COLLECT_BLUE_CORNER_MOVE_FOWARD,
			ERROR_COLLECT_BLUE_CORNER_POSITION_LEFT,
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
			//rotate
			state = try_going(1780, 800, state, COLLECT_BLUE_CORNER_ACTIVATE,  ERROR, SLOW, FORWARD, DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;


		case COLLECT_BLUE_CORNER_ACTIVATE:{
			Uint8 state1=COLLECT_BLUE_CORNER_ACTIVATE;
			Uint8 state2=COLLECT_BLUE_CORNER_ACTIVATE;
			Uint8 state3=COLLECT_BLUE_CORNER_ACTIVATE;
			Uint8 state4=COLLECT_BLUE_CORNER_ACTIVATE;
			Uint8 state5=COLLECT_BLUE_CORNER_ACTIVATE;
			Uint8 state6=COLLECT_BLUE_CORNER_ACTIVATE;


			// descendre le rouleau
			ACT_push_order(ACT_ORE_SHOVEL,  ACT_ORE_SHOVEL_DOWN);
			//remonter les billes porteuses et descendre la roue
			ACT_push_order(ACT_BIG_BALL_FRONT_LEFT,  ACT_BIG_BALL_FRONT_LEFT_UP);
			ACT_push_order(ACT_BIG_BALL_FRONT_RIGHT,  ACT_BIG_BALL_FRONT_RIGHT_UP);
			ACT_push_order(ACT_BIG_BALL_BACK_LEFT,  ACT_BIG_BALL_FRONT_LEFT_UP);
			ACT_push_order(ACT_BIG_BALL_BACK_RIGHT,  ACT_BIG_BALL_BACK_RIGHT_UP);
			ACT_push_order(ACT_BEARING_BALL_WHEEL,  ACT_BEARING_BALL_WHEEL_DOWN);

			state1= check_act_status(ACT_QUEUE_Ore_shovel, state1, COLLECT_BLUE_CORNER_MOVE_FOWARD, ERROR);
			state2= check_act_status(ACT_QUEUE_Bearing_ball_wheel, state2, COLLECT_BLUE_CORNER_MOVE_FOWARD, ERROR);
			state3= check_act_status(ACT_QUEUE_Big_bearing_back_left, state3, COLLECT_BLUE_CORNER_MOVE_FOWARD, ERROR);
			state4= check_act_status(ACT_QUEUE_Big_bearing_back_right, state4, COLLECT_BLUE_CORNER_MOVE_FOWARD, ERROR);
			state5= check_act_status(ACT_QUEUE_Big_bearing_front_left, state5, COLLECT_BLUE_CORNER_MOVE_FOWARD, ERROR);
			state6= check_act_status(ACT_QUEUE_Big_bearing_front_right, state6, COLLECT_BLUE_CORNER_MOVE_FOWARD, ERROR);


if((state1==ERROR)||(state2==ERROR)||(state3==ERROR)||(state4==ERROR)||(state5==ERROR)||(state6==ERROR)){
	state=ERROR;
}
else if((state1==COLLECT_BLUE_CORNER_ACTIVATE)&&(state2==COLLECT_BLUE_CORNER_ACTIVATE)&&(state3==COLLECT_BLUE_CORNER_ACTIVATE)&&(state4==COLLECT_BLUE_CORNER_ACTIVATE)&&(state5==COLLECT_BLUE_CORNER_ACTIVATE)&&(state6==COLLECT_BLUE_CORNER_ACTIVATE)){
	state=COLLECT_BLUE_CORNER_MOVE_FOWARD;
}
			}break;

		case COLLECT_BLUE_CORNER_MOVE_FOWARD:
			state = try_going(1780, 310, state, COLLECT_BLUE_CORNER_ROLLER_UP,  ERROR, SLOW, FORWARD, DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case COLLECT_BLUE_CORNER_ROLLER_UP:
			// accelere rouleau ou arr�t ...
			state = COLLECT_BLUE_CORNER_POSITION_LEFT;
			break;

		case COLLECT_BLUE_CORNER_POSITION_LEFT:
			state = try_going(1780, 800, state, COLLECT_BLUE_CORNER_VALIDATE,  ERROR, SLOW, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);
			//lien avec action suivante 1850
			break;


		case COLLECT_BLUE_CORNER_VALIDATE:{
			Uint8 state1=COLLECT_BLUE_CORNER_VALIDATE;
			Uint8 state2=COLLECT_BLUE_CORNER_VALIDATE;
			Uint8 state3=COLLECT_BLUE_CORNER_VALIDATE;
			Uint8 state4=COLLECT_BLUE_CORNER_VALIDATE;
			Uint8 state5=COLLECT_BLUE_CORNER_VALIDATE;
			Uint8 state6=COLLECT_BLUE_CORNER_VALIDATE;


			// remonter le rouleau
			ACT_push_order(ACT_ORE_SHOVEL,  ACT_ORE_SHOVEL_UP);
			//remonter les billes porteuses et descendre la roue
			ACT_push_order(ACT_BIG_BALL_FRONT_LEFT,  ACT_BIG_BALL_FRONT_LEFT_DOWN);
			ACT_push_order(ACT_BIG_BALL_FRONT_RIGHT,  ACT_BIG_BALL_FRONT_RIGHT_DOWN);
			ACT_push_order(ACT_BIG_BALL_BACK_LEFT,  ACT_BIG_BALL_FRONT_LEFT_DOWN);
			ACT_push_order(ACT_BIG_BALL_BACK_RIGHT,  ACT_BIG_BALL_BACK_RIGHT_DOWN);
			ACT_push_order(ACT_BEARING_BALL_WHEEL,  ACT_BEARING_BALL_WHEEL_UP);

			state1= check_act_status(ACT_QUEUE_Ore_shovel, state1, COLLECT_BLUE_CORNER_MOVE_FOWARD, ERROR);
			state2= check_act_status(ACT_QUEUE_Bearing_ball_wheel, state2, COLLECT_BLUE_CORNER_MOVE_FOWARD, ERROR);
			state3= check_act_status(ACT_QUEUE_Big_bearing_back_left, state3, COLLECT_BLUE_CORNER_MOVE_FOWARD, ERROR);
			state4= check_act_status(ACT_QUEUE_Big_bearing_back_right, state4, COLLECT_BLUE_CORNER_MOVE_FOWARD, ERROR);
			state5= check_act_status(ACT_QUEUE_Big_bearing_front_left, state5, COLLECT_BLUE_CORNER_MOVE_FOWARD, ERROR);
			state6= check_act_status(ACT_QUEUE_Big_bearing_front_right, state6, COLLECT_BLUE_CORNER_MOVE_FOWARD, ERROR);


if((state1==ERROR)||(state2==ERROR)||(state3==ERROR)||(state4==ERROR)||(state5==ERROR)||(state6==ERROR)){
	state=ERROR;
}
else if((state1==COLLECT_BLUE_CORNER_VALIDATE)&&(state2==COLLECT_BLUE_CORNER_VALIDATE)&&(state3==COLLECT_BLUE_CORNER_VALIDATE)&&(state4==COLLECT_BLUE_CORNER_VALIDATE)&&(state5==COLLECT_BLUE_CORNER_VALIDATE)&&(state6==COLLECT_BLUE_CORNER_VALIDATE)){
	state=GET_OUT_COLLECT_BLUE_CORNER;
}


		}break;

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

		case ERROR_COLLECT_BLUE_CORNER_MOVE_FOWARD:
			// il y a quelqu'un dans le cratere
			state = try_going(1390, 2600, state, COLLECT_BLUE_CORNER_MOVE_FOWARD,  COLLECT_BLUE_CORNER_MOVE_FOWARD, SLOW, FORWARD, DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case ERROR_COLLECT_BLUE_CORNER_POSITION_LEFT:
			// il y a quelqu'un qui bloque la sortie du cratere
			state = try_going(1690, 2600, state, COLLECT_BLUE_CORNER_POSITION_LEFT,  COLLECT_BLUE_CORNER_POSITION_LEFT, SLOW, FORWARD, DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;
			//GET_OUT_COLLECT_BLUE_CORNER valide sorti

			// GET_OUT= GET_OUT_COLLECT_BLUE_CORNER
	}

	return IN_PROGRESS;
}


