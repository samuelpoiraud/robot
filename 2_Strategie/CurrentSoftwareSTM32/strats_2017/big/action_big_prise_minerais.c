#include "action_big.h"
#include "../../propulsion/movement.h"
#include "../../propulsion/astar.h"
#include "../../QS/QS_stateMachineHelper.h"
#include "../../QS/QS_outputlog.h"
#include "../../QS/QS_types.h"
#include "../../QS/QS_IHM.h"
#include  "../../utils/generic_functions.h"
#include "../../actuator/act_functions.h"
#include "../../actuator/queue.h"
#include "../../utils/actionChecker.h"

// flag de  sub l'autre robot, crater south
/*
error_e sub_harry_take_big_crater(ELEMENTS_property_e minerais){ // OUR_ELEMENT / ADV_ELEMENT
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_TAKE_BIG_CRATER,
			INIT,
			SUB_MOVE_POS_YELLOW,
			SUB_TAKE_YELLOW_MIDDLE,
			SUB_TAKE_YELLOW_ROCKET,
			SUB_TAKE_YELLOW_CORNER,
			SUB_MOVE_POS_BLUE,
			SUB_TAKE_BLUE_MIDDLE,
			SUB_TAKE_BLUE_ROCKET,
			SUB_TAKE_BLUE_CORNER,
			GET_OUT_YELLOW,
			GET_OUT_BLUE,
			ERROR,
			ERROR_GET_OUT_YELLOW,
			ERROR_GET_OUT_BLUE,
			DONE
		);

	switch(state){
		case INIT:
			debug_printf("flag_our:%d\tflag_adv:%d", ELEMENTS_get_flag(FLAG_OUR_CORNER_CRATER_IS_TAKEN), ELEMENTS_get_flag(FLAG_ADV_CORNER_CRATER_IS_TAKEN));
			if(IHM_switchs_get(SWITCH_DISABLE_ORE)){
				state = ERROR; // L'actionneur minerais a �t� d�sactiv�
			}else if((ELEMENTS_get_flag(FLAG_OUR_CORNER_CRATER_IS_TAKEN) && minerais == OUR_ELEMENT)
					|| (ELEMENTS_get_flag(FLAG_ADV_CORNER_CRATER_IS_TAKEN) && minerais == ADV_ELEMENT)){
				state = DONE;	// L'action a d�j� �t� faite
			}*/ /*else if(ELEMENTS_get_flag(FLAG_SUB_ANNE_TAKE_ORE)){ 			//! anne peut prendre les minerais
				state=ERROR; // L'autre robot fait la sub
			}*/ /*else{
				if (minerais != OUR_ELEMENT && minerais != ADV_ELEMENT){

					state = ERROR;

				}else if(minerais ==OUR_ELEMENT){
					if(global.color == YELLOW){
						ELEMENTS_set_flag(FLAG_SUB_HARRY_OUR_CORNER_CRATER, TRUE);
						state = SUB_MOVE_POS_YELLOW;
					}else{
						ELEMENTS_set_flag(FLAG_SUB_HARRY_OUR_CORNER_CRATER, TRUE);
						state = SUB_MOVE_POS_BLUE;
					}

				}else{
					if(global.color == YELLOW){
						ELEMENTS_set_flag(FLAG_SUB_HARRY_ADV_CORNER_CRATER, TRUE);
						state = SUB_MOVE_POS_BLUE;
					}else{
						ELEMENTS_set_flag(FLAG_SUB_HARRY_ADV_CORNER_CRATER, TRUE);
						state = SUB_MOVE_POS_YELLOW;
					}
				}



			}
			break;

		case SUB_MOVE_POS_YELLOW:
			state = check_sub_action_result(sub_harry_take_big_crater_move_pos_yellow(), state, SUB_TAKE_YELLOW_MIDDLE, ERROR);
			break;

		case SUB_TAKE_YELLOW_MIDDLE:
			state = check_sub_action_result(sub_harry_take_big_crater_yellow_middle(), state, SUB_TAKE_YELLOW_ROCKET, ERROR);
			break;

		case SUB_TAKE_YELLOW_ROCKET:
			state = check_sub_action_result(sub_harry_take_big_crater_yellow_rocket(), state, SUB_TAKE_YELLOW_CORNER, ERROR);
			break;

		case SUB_TAKE_YELLOW_CORNER:
			state = check_sub_action_result(sub_harry_take_big_crater_yellow_corner(), state, GET_OUT_YELLOW, ERROR);
			break;

		case GET_OUT_YELLOW:
			state = try_going(1310, 2560, GET_OUT_YELLOW, DONE,  ERROR_GET_OUT_YELLOW, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;





		case SUB_MOVE_POS_BLUE:
			state = check_sub_action_result(sub_harry_take_big_crater_move_pos_blue(), state, SUB_TAKE_BLUE_MIDDLE, ERROR);
			break;

		case SUB_TAKE_BLUE_MIDDLE:
			state = check_sub_action_result(sub_harry_take_big_crater_blue_middle(), state, SUB_TAKE_BLUE_ROCKET, ERROR);
			break;

		case SUB_TAKE_BLUE_ROCKET:
			state = check_sub_action_result(sub_harry_take_big_crater_blue_rocket(), state, SUB_TAKE_BLUE_CORNER, ERROR);
			break;

		case SUB_TAKE_BLUE_CORNER:
			state = check_sub_action_result(sub_harry_take_big_crater_blue_corner(), state, GET_OUT_BLUE, ERROR);
			break;


		case GET_OUT_BLUE: //try go
			state = try_going(1310, 440, GET_OUT_BLUE, DONE,  ERROR_GET_OUT_BLUE, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;


		case ERROR_GET_OUT_YELLOW:
			state = try_going(1780, 2200, ERROR_GET_OUT_YELLOW, GET_OUT_YELLOW,  GET_OUT_YELLOW, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

		case ERROR_GET_OUT_BLUE:
			state = try_going(1780, 800, ERROR_GET_OUT_BLUE, GET_OUT_BLUE,  GET_OUT_BLUE, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

		case ERROR:
			RESET_MAE();
			on_turning_point();
			return NOT_HANDLED;
			break;


		case DONE:
			RESET_MAE();
			on_turning_point();
			if(minerais == OUR_ELEMENT){
				ELEMENTS_set_flag(FLAG_SUB_HARRY_OUR_CORNER_CRATER, FALSE);
				ELEMENTS_set_flag(FLAG_OUR_CORNER_CRATER_IS_TAKEN, TRUE);
			}else{
				ELEMENTS_set_flag(FLAG_SUB_HARRY_ADV_CORNER_CRATER, FALSE);
				ELEMENTS_set_flag(FLAG_ADV_CORNER_CRATER_IS_TAKEN, TRUE);
			}
			ELEMENTS_set_flag(FLAG_HARRY_STOMACH_IS_FULL, TRUE);
			return END_OK;
			break;

		default:
			if(entrance)
				debug_printf("default case in sub_harry_take_big_crater\n");
			break;

	}

	return IN_PROGRESS;
}
*/

//sub actionneurs
error_e sub_act_big_take(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_START_TAKE,
			INIT,
			ERROR,
			DONE
		);

	switch(state){
		case INIT:{

			Uint8 state1=INIT;
			Uint8 state2=INIT;
			Uint8 state3=INIT;
			Uint8 state4=INIT;
			Uint8 state5=INIT;

			if(entrance){
				ACT_push_order(ACT_BIG_BALL_FRONT_LEFT,  ACT_BIG_BALL_FRONT_LEFT_UP);
				ACT_push_order(ACT_BIG_BALL_FRONT_RIGHT,  ACT_BIG_BALL_FRONT_RIGHT_UP);
				ACT_push_order(ACT_BIG_BALL_BACK_LEFT,  ACT_BIG_BALL_FRONT_LEFT_UP);
				ACT_push_order(ACT_BIG_BALL_BACK_RIGHT,  ACT_BIG_BALL_BACK_RIGHT_UP);
				ACT_push_order(ACT_BEARING_BALL_WHEEL,  ACT_BEARING_BALL_WHEEL_UP);
			}

			state1= check_act_status(ACT_QUEUE_Bearing_ball_wheel, state1, DONE, ERROR);
			state2= check_act_status(ACT_QUEUE_Big_bearing_back_left, state2, DONE, ERROR);
			state3= check_act_status(ACT_QUEUE_Big_bearing_back_right, state3, DONE, ERROR);
			state4= check_act_status(ACT_QUEUE_Big_bearing_front_left, state4, DONE, ERROR);
			state5= check_act_status(ACT_QUEUE_Big_bearing_front_right, state5, DONE, ERROR);

		if((state1==ERROR)||(state2==ERROR)||(state3==ERROR)||(state4==ERROR)||(state5==ERROR)){
			state=ERROR;
		}
		else if((state1==DONE)&&(state2==DONE)&&(state3==DONE)&&(state4==DONE)&&(state5==DONE)){
			state=DONE;
		}
	}break;


		case ERROR:
			RESET_MAE();
			on_turning_point();
			return NOT_HANDLED;
			break;

		case DONE:
			RESET_MAE();
			on_turning_point();
			return END_OK;
			break;

		default:
			if(entrance)
				debug_printf("default case in sub_act_big_take\n");
			break;
	}
	return IN_PROGRESS;
}


error_e sub_act_big_off(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_END_OF_STRAT,
			INIT,
			ERROR,
			DONE
		);


	switch(state){
		case INIT:{
			Uint8 state1=INIT;
			Uint8 state2=INIT;
			Uint8 state3=INIT;
			Uint8 state4=INIT;
			Uint8 state5=INIT;

			if(entrance){
				ACT_push_order(ACT_BIG_BALL_FRONT_LEFT,  ACT_BIG_BALL_FRONT_LEFT_DOWN);
				ACT_push_order(ACT_BIG_BALL_FRONT_RIGHT,  ACT_BIG_BALL_FRONT_RIGHT_DOWN);
				ACT_push_order(ACT_BIG_BALL_BACK_LEFT,  ACT_BIG_BALL_FRONT_LEFT_DOWN);
				ACT_push_order(ACT_BIG_BALL_BACK_RIGHT,  ACT_BIG_BALL_BACK_RIGHT_DOWN);
				ACT_push_order(ACT_BEARING_BALL_WHEEL,  ACT_BEARING_BALL_WHEEL_UP);
			}

			state1= check_act_status(ACT_QUEUE_Bearing_ball_wheel, state1, DONE, ERROR);
			state2= check_act_status(ACT_QUEUE_Big_bearing_back_left, state2, DONE, ERROR);
			state3= check_act_status(ACT_QUEUE_Big_bearing_back_right, state3, DONE, ERROR);
			state4= check_act_status(ACT_QUEUE_Big_bearing_front_left, state4, DONE, ERROR);
			state5= check_act_status(ACT_QUEUE_Big_bearing_front_right, state5, DONE, ERROR);

		if((state1==ERROR)||(state2==ERROR)||(state3==ERROR)||(state4==ERROR)||(state5==ERROR)){
			state=ERROR;
		}
		else if((state1==DONE)&&(state2==DONE)&&(state3==DONE)&&(state4==ERROR)&&(state5==DONE)){
			state=DONE;
		}
		}break;

		case ERROR:
			RESET_MAE();
			on_turning_point();
			return NOT_HANDLED;
			break;

		case DONE:
			RESET_MAE();
			on_turning_point();
			return END_OK;
			break;

		default:
			if(entrance)
				debug_printf("default case in sub_act_big_off\n");
			break;
	}
	return IN_PROGRESS;
}


//sous sub deplacement
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


/*
//aller cratere jaune
error_e sub_harry_take_big_crater_move_pos_yellow(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_TAKE_BIG_CRATER_MOVE_POS_YELLOW,
			INIT,
			GO_YELLOW_CRATER_FROM_BLUE_SQUARE,
			GO_YELLOW_CRATER_FROM_MIDDLE_SQUARE,
			GO_YELLOW_CRATER_FROM_YELLOW_SQUARE,
			ASTAR_GO_YELLOW_CRATER,
			GET_IN,
			ERROR,
			DONE

		);

	const displacement_t leave_middle_square[2] = { {(GEOMETRY_point_t){900, 2200}, FAST},
										  {(GEOMETRY_point_t){1330, 2600}, FAST},
										  };

	const displacement_t leave_blue_square[3] = { {(GEOMETRY_point_t){900, 800}, FAST},
										  {(GEOMETRY_point_t){900, 2200}, FAST},
										  {(GEOMETRY_point_t){1330, 2600}, FAST}
										  };

	switch(state){
		// deplacement robot
		case INIT:
			state = GET_IN;
			break;

		case GET_IN:
			if(i_am_in_square(700, 1500, 300, 800)){
				state = GO_YELLOW_CRATER_FROM_BLUE_SQUARE;
			}
			else if(i_am_in_square(700, 1500, 2700, 2200)){
				state = GO_YELLOW_CRATER_FROM_YELLOW_SQUARE;
			}
<<<<<<< .mine
			else if(i_am_in_square(350, 1200, 800, 2200)){
=======
			else if(i_am_in_square(300, 1050, 800, 2200)){
>>>>>>> .r5420
				state = GO_YELLOW_CRATER_FROM_MIDDLE_SQUARE;
			}else{
				state = ASTAR_GO_YELLOW_CRATER;
			}
			break;

		case GO_YELLOW_CRATER_FROM_BLUE_SQUARE:
			state = try_going_multipoint( leave_blue_square, 3, state, DONE, ASTAR_GO_YELLOW_CRATER, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

		case GO_YELLOW_CRATER_FROM_MIDDLE_SQUARE:
			state = try_going_multipoint( leave_middle_square, 2, state, DONE, ASTAR_GO_YELLOW_CRATER, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

		case GO_YELLOW_CRATER_FROM_YELLOW_SQUARE:
			state = try_going(1330, 2600, state, DONE, ASTAR_GO_YELLOW_CRATER, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

		case ASTAR_GO_YELLOW_CRATER:
			state = ASTAR_try_going(1330, 2600, state, DONE,  ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

		case ERROR:
			RESET_MAE();
			return NOT_HANDLED;
			break;

		case DONE:
			RESET_MAE();
			return END_OK;
			break;

		default:
			if(entrance)
				debug_printf("default case in sub_harry_take_big_crater_move_pos_yellow\n");
			break;
	}

	return IN_PROGRESS;
}



//aller cratere bleu
error_e sub_harry_take_big_crater_move_pos_blue(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_TAKE_BIG_CRATER_MOVE_POS_BLUE,
			INIT,
			GO_BLUE_CRATER_FROM_YELLOW_SQUARE,
			GO_BLUE_CRATER_FROM_MIDDLE_SQUARE,
			GO_BLUE_CRATER_FROM_BLUE_SQUARE,
			ASTAR_GO_BLUE_CRATER,
			GET_IN,
			ERROR,
			DONE

		);

	const displacement_t leave_middle_square[2] = { {(GEOMETRY_point_t){900, 800}, FAST},
										  {(GEOMETRY_point_t){1330, 400}, FAST},
										  };

	const displacement_t leave_yellow_square[3] = { {(GEOMETRY_point_t){950, 2200}, FAST},
										  {(GEOMETRY_point_t){900, 800}, FAST},
										  {(GEOMETRY_point_t){1330, 400}, FAST}
										  };

	switch(state){
		// deplacement robot
		case INIT:
			state = GET_IN;
			break;

		case GET_IN:
			if(i_am_in_square(700, 1500, 300, 800)){
				state = GO_BLUE_CRATER_FROM_BLUE_SQUARE;
			}
			else if(i_am_in_square(700, 1500, 2700, 2200)){
				state = GO_BLUE_CRATER_FROM_YELLOW_SQUARE;
			}
<<<<<<< .mine
			else if(i_am_in_square(350, 1200, 800, 2200)){
=======
			else if(i_am_in_square(300, 1050, 800, 2200)){
>>>>>>> .r5420
				state = GO_BLUE_CRATER_FROM_MIDDLE_SQUARE;
			}else{
				state = ASTAR_GO_BLUE_CRATER;
			}
			break;


		case GO_BLUE_CRATER_FROM_YELLOW_SQUARE:
			state = try_going_multipoint( leave_yellow_square, 3, state, DONE, ASTAR_GO_BLUE_CRATER, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

		case GO_BLUE_CRATER_FROM_MIDDLE_SQUARE:
			state = try_going_multipoint( leave_middle_square, 2, state, DONE, ASTAR_GO_BLUE_CRATER, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

		case GO_BLUE_CRATER_FROM_BLUE_SQUARE:
			state = try_going(1330, 400, state, DONE, ASTAR_GO_BLUE_CRATER, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

		case ASTAR_GO_BLUE_CRATER:
			state = ASTAR_try_going(1330, 400, state, DONE,  ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

		case ERROR:
			RESET_MAE();
			return NOT_HANDLED;
			break;

		case DONE:
			RESET_MAE();
			return END_OK;
			break;

		default:
			if(entrance)
				debug_printf("default case in sub_harry_take_big_crater_move_pos_blue\n");
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
			COLLECT_YELLOW_MIDDLE_LINE,
			COLLECT_YELLOW_MIDDLE_WALL_DOWN,
			COLLECT_YELLOW_MIDDLE_ACTIVATE,
			COLLECT_YELLOW_MIDDLE_MOVE_FOWARD,
			COLLECT_YELLOW_MIDDLE_ROLLER_UP,
			COLLECT_YELLOW_MIDDLE_POSITION_LEFT,
			COLLECT_YELLOW_MIDDLE_END_ACTIVATE,
			COLLECT_YELLOW_MIDDLE_WALL_UP,
			ERROR_COLLECT_YELLOW_MIDDLE_MOVE_FOWARD,
			ERROR_COLLECT_YELLOW_MIDDLE_POSITION_LEFT,
			DONE,
			ERROR
		);

	switch(state){
		case INIT:
			state = try_going(1330, 2600, state, COLLECT_YELLOW_MIDDLE_LINE,  ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;


		case COLLECT_YELLOW_MIDDLE_LINE:
			// rotation nord avant de reculer dans le crat�re
			state = try_go_angle(PI4096, state, COLLECT_YELLOW_MIDDLE_WALL_DOWN, ERROR, FAST, CLOCKWISE, END_AT_LAST_POINT);
			break;

		case COLLECT_YELLOW_MIDDLE_WALL_DOWN:{
			Uint8 state0=COLLECT_YELLOW_MIDDLE_WALL_DOWN;
			Uint8 state1=COLLECT_YELLOW_MIDDLE_WALL_DOWN;
			Uint8 state2=COLLECT_YELLOW_MIDDLE_WALL_DOWN;
				if(entrance){
					ACT_push_order(ACT_ORE_ROLLER_ARM,  ACT_ORE_ROLLER_ARM_OUT);
					ACT_push_order(ACT_ORE_WALL,  ACT_ORE_WALL_OUT);
				}
				state0= check_act_status(ACT_QUEUE_Ore_roller_arm, state0, DONE, ERROR);
				state1= check_act_status(ACT_QUEUE_Ore_wall, state1, DONE, ERROR);
				state2= check_sub_action_result(sub_act_big_take(), state2, DONE, ERROR);

				if((state0==ERROR)||(state1==ERROR)||(state2==ERROR)){
					state=COLLECT_YELLOW_MIDDLE_MOVE_FOWARD;// juste en test pas ERROR
				}
				else if((state0==DONE)&&(state1==DONE)&&(state2==DONE)){
					state=COLLECT_YELLOW_MIDDLE_ACTIVATE;
				}
			}break;

		case COLLECT_YELLOW_MIDDLE_ACTIVATE:
			if (entrance){
				// descendre le mur du rouleau et allumer le rouleau
				ACT_push_order_with_param(ACT_ORE_ROLLER_FOAM, ACT_ORE_ROLLER_FOAM_RUN, ACT_ROLLER_FOAM_SPEED_RUN);
			}
			state= check_act_status(ACT_QUEUE_Ore_roller_foam, state, COLLECT_YELLOW_MIDDLE_MOVE_FOWARD, ERROR);
			break;


		case COLLECT_YELLOW_MIDDLE_MOVE_FOWARD:
			state = try_going(1690, 2600, state, COLLECT_YELLOW_MIDDLE_ROLLER_UP,  ERROR_COLLECT_YELLOW_MIDDLE_MOVE_FOWARD, SLOW, BACKWARD, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

		case COLLECT_YELLOW_MIDDLE_ROLLER_UP:
			// accelere rouleau ou arr�t ...
			state = COLLECT_YELLOW_MIDDLE_POSITION_LEFT;
			break;

		case COLLECT_YELLOW_MIDDLE_POSITION_LEFT:
			state = try_going(1330, 2600, state, COLLECT_YELLOW_MIDDLE_END_ACTIVATE,  ERROR_COLLECT_YELLOW_MIDDLE_POSITION_LEFT, SLOW, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			//lien avec action suivante
			break;

		case COLLECT_YELLOW_MIDDLE_END_ACTIVATE:
			if (entrance){
				// descendre le mur du rouleau et allumer le rouleau
				ACT_push_order_with_param(ACT_ORE_ROLLER_FOAM, ACT_ORE_ROLLER_FOAM_STOP, ACT_ROLLER_FOAM_SPEED_RUN);
			}
			state= check_act_status(ACT_QUEUE_Ore_roller_foam, state, COLLECT_YELLOW_MIDDLE_WALL_UP, ERROR);
			break;

		case COLLECT_YELLOW_MIDDLE_WALL_UP:{
			Uint8 state0=COLLECT_YELLOW_MIDDLE_WALL_UP;
			Uint8 state1=COLLECT_YELLOW_MIDDLE_WALL_UP;
			Uint8 state2=COLLECT_YELLOW_MIDDLE_WALL_UP;
				if(entrance){
					ACT_push_order(ACT_ORE_ROLLER_ARM,  ACT_ORE_ROLLER_ARM_IDLE);
					ACT_push_order(ACT_ORE_WALL,  ACT_ORE_WALL_IDLE);
				}
				state0= check_act_status(ACT_QUEUE_Ore_roller_arm, state0, DONE, ERROR);
				state1= check_act_status(ACT_QUEUE_Ore_wall, state1, DONE, ERROR);
				state2= check_sub_action_result(sub_act_big_off(), state2, DONE, ERROR);

				if((state0==ERROR)||(state1==ERROR)||(state2==ERROR)){
					state=DONE;
				}
				else if((state0==DONE)&&(state1==DONE)&&(state2==DONE)){
					state=DONE;
				}
			}break;


		case ERROR_COLLECT_YELLOW_MIDDLE_MOVE_FOWARD:
			// il y a quelqu'un dans le cratere
			state = try_going(1330, 2600, state, COLLECT_YELLOW_MIDDLE_MOVE_FOWARD,  COLLECT_YELLOW_MIDDLE_MOVE_FOWARD, FAST, FORWARD, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

		case ERROR_COLLECT_YELLOW_MIDDLE_POSITION_LEFT:
			// il y a quelqu'un qui bloque la sortie du cratere
			state = try_going(1690, 2600, state, COLLECT_YELLOW_MIDDLE_POSITION_LEFT,  COLLECT_YELLOW_MIDDLE_POSITION_LEFT, FAST, BACKWARD, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

		case ERROR:
			RESET_MAE();
			return NOT_HANDLED;
			break;

		case DONE:
			RESET_MAE();
			return END_OK;
			break;

		default:
			if(entrance)
				debug_printf("default case in sub_harry_take_big_crater_yellow_middle\n");
			break;

	}
	return IN_PROGRESS;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// 2
// COLLECT_YELLOW_ROCKET
error_e sub_harry_take_big_crater_yellow_rocket(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_TAKE_BIG_CRATER_YELLOW_FUSE,
			INIT,
			COLLECT_YELLOW_ROCKET_LINE,
			COLLECT_YELLOW_ROCKET_WALL_DOWN,
			COLLECT_YELLOW_ROCKET_ACTIVATE,
			COLLECT_YELLOW_ROCKET_MOVE_FOWARD,
			COLLECT_YELLOW_ROCKET_ROLLER_UP,
			COLLECT_YELLOW_ROCKET_POSITION_LEFT,
			COLLECT_YELLOW_ROCKET_END_ACTIVATE,
			COLLECT_YELLOW_ROCKET_WALL_UP,
			ERROR_COLLECT_YELLOW_ROCKET_MOVE_FOWARD,
			ERROR_COLLECT_YELLOW_ROCKET_POSITION_LEFT,
			DONE,
			ERROR
		);

	switch(state){
		case INIT:
			state = try_going(1600, 2350, state, COLLECT_YELLOW_ROCKET_LINE,  ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;


		case COLLECT_YELLOW_ROCKET_LINE:
			// rotation nord avant de reculer dans le crat�re
			state = try_go_angle(-PI4096/2, state, COLLECT_YELLOW_ROCKET_WALL_DOWN, ERROR, FAST, CLOCKWISE, END_AT_LAST_POINT);
			break;

		case COLLECT_YELLOW_ROCKET_WALL_DOWN:{
			Uint8 state0=COLLECT_YELLOW_ROCKET_WALL_DOWN;
			Uint8 state1=COLLECT_YELLOW_ROCKET_WALL_DOWN;
			Uint8 state2=COLLECT_YELLOW_ROCKET_WALL_DOWN;
				if(entrance){
					ACT_push_order(ACT_ORE_ROLLER_ARM,  ACT_ORE_ROLLER_ARM_OUT);
					ACT_push_order(ACT_ORE_WALL,  ACT_ORE_WALL_OUT);
				}
				state0= check_act_status(ACT_QUEUE_Ore_roller_arm, state0, DONE, ERROR);
				state1= check_act_status(ACT_QUEUE_Ore_wall, state1, DONE, ERROR);
				state2= check_sub_action_result(sub_act_big_take(), state2, DONE, ERROR);

				if((state0==ERROR)||(state1==ERROR)||(state2==ERROR)){
					state=COLLECT_YELLOW_ROCKET_MOVE_FOWARD;
				}
				else if((state0==DONE)&&(state1==DONE)&&(state2==DONE)){
					state=COLLECT_YELLOW_ROCKET_ACTIVATE;
				}
			}break;

		case COLLECT_YELLOW_ROCKET_ACTIVATE:
			if (entrance){
				// descendre le mur du rouleau et allumer le rouleau
				ACT_push_order_with_param(ACT_ORE_ROLLER_FOAM, ACT_ORE_ROLLER_FOAM_RUN, ACT_ROLLER_FOAM_SPEED_RUN);
			}
			state= check_act_status(ACT_QUEUE_Ore_roller_foam, state, COLLECT_YELLOW_ROCKET_MOVE_FOWARD, ERROR);
			break;


		case COLLECT_YELLOW_ROCKET_MOVE_FOWARD:
			state = try_going(1600, 2690, state, COLLECT_YELLOW_ROCKET_ROLLER_UP,  ERROR_COLLECT_YELLOW_ROCKET_MOVE_FOWARD, SLOW, BACKWARD, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

		case COLLECT_YELLOW_ROCKET_ROLLER_UP:
			// accelere rouleau ou arr�t ...
			state = COLLECT_YELLOW_ROCKET_POSITION_LEFT;
			break;

		case COLLECT_YELLOW_ROCKET_POSITION_LEFT:
			state = try_going(1600, 2350, state, COLLECT_YELLOW_ROCKET_END_ACTIVATE,  ERROR_COLLECT_YELLOW_ROCKET_POSITION_LEFT, SLOW, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);;
			//lien avec action suivante
			break;

		case COLLECT_YELLOW_ROCKET_END_ACTIVATE:
			if (entrance){
				// descendre le mur du rouleau et allumer le rouleau
				ACT_push_order_with_param(ACT_ORE_ROLLER_FOAM, ACT_ORE_ROLLER_FOAM_STOP, ACT_ROLLER_FOAM_SPEED_RUN);
			}
			state= check_act_status(ACT_QUEUE_Ore_roller_foam, state, COLLECT_YELLOW_ROCKET_WALL_UP, ERROR);
			break;

		case COLLECT_YELLOW_ROCKET_WALL_UP:{
			Uint8 state0=COLLECT_YELLOW_ROCKET_WALL_UP;
			Uint8 state1=COLLECT_YELLOW_ROCKET_WALL_UP;
			Uint8 state2=COLLECT_YELLOW_ROCKET_WALL_UP;
				if(entrance){
					ACT_push_order(ACT_ORE_ROLLER_ARM,  ACT_ORE_ROLLER_ARM_IDLE);
					ACT_push_order(ACT_ORE_WALL,  ACT_ORE_WALL_IDLE);
				}
				state0= check_act_status(ACT_QUEUE_Ore_roller_arm, state0, DONE, ERROR);
				state1= check_act_status(ACT_QUEUE_Ore_wall, state1, DONE, ERROR);
				state2= check_sub_action_result(sub_act_big_off(), state2, DONE, ERROR);

				if((state0==ERROR)||(state1==ERROR)||(state2==ERROR)){
					state=DONE;
				}
				else if((state0==DONE)&&(state1==DONE)&&(state2==DONE)){
					state=DONE;
				}
			}break;

		case ERROR_COLLECT_YELLOW_ROCKET_MOVE_FOWARD:
			// il y a quelqu'un dans le cratere
			state = try_going(1600, 2350, state, COLLECT_YELLOW_ROCKET_MOVE_FOWARD,  COLLECT_YELLOW_ROCKET_MOVE_FOWARD, FAST, FORWARD, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

		case ERROR_COLLECT_YELLOW_ROCKET_POSITION_LEFT:
			// il y a quelqu'un qui bloque la sortie du cratere
			state = try_going(1600, 2690, state, COLLECT_YELLOW_ROCKET_POSITION_LEFT,  COLLECT_YELLOW_ROCKET_POSITION_LEFT, FAST, BACKWARD, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

		case ERROR:
			RESET_MAE();
			return NOT_HANDLED;
			break;

		case DONE:
			RESET_MAE();
			return END_OK;
			break;

		default:
			if(entrance)
				debug_printf("default case in sub_harry_take_big_crater_yellow_rocket\n");
			break;
	}

	return IN_PROGRESS;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// 3
//COLLECT_YELLOW_CORNER
	error_e sub_harry_take_big_crater_yellow_corner(){
		CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_TAKE_BIG_CRATER_YELLOW_CORNER,
				INIT,
				COLLECT_YELLOW_CORNER_POS,
				COLLECT_YELLOW_CORNER_LINE,
				COLLECT_YELLOW_CORNER_WALL_DOWN,
				COLLECT_YELLOW_CORNER_ACTIVATE,
				COLLECT_YELLOW_CORNER_MOVE_FOWARD,
				COLLECT_YELLOW_CORNER_ROLLER_UP,
				COLLECT_YELLOW_CORNER_POSITION_LEFT,
				COLLECT_YELLOW_CORNER_END_ACTIVATE,
				COLLECT_YELLOW_CORNER_WALL_UP,
				ERROR_COLLECT_YELLOW_CORNER_MOVE_FOWARD,
				ERROR_COLLECT_YELLOW_CORNER_POSITION_LEFT,
				DONE,
				ERROR
			);

	switch(state){
		case INIT:
			if((global.color == YELLOW)&&(!ELEMENTS_get_flag(FLAG_OUR_UNICOLOR_SOUTH_IS_TAKEN))){
				state = check_sub_action_result(sub_harry_prise_module_unicolor_south(NO_SIDE),state, COLLECT_YELLOW_CORNER_POS, ERROR);
			}
			else if((global.color == BLUE)&&(!ELEMENTS_get_flag(FLAG_ADV_UNICOLOR_SOUTH_IS_TAKEN))){
				state = ERROR;//pas possible
			}
			else{
				state=COLLECT_YELLOW_CORNER_POS;
			}
			break;

		case COLLECT_YELLOW_CORNER_POS:
			state = try_going(1780, 2200, state, COLLECT_YELLOW_CORNER_LINE,  ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

		case COLLECT_YELLOW_CORNER_LINE:
			// rotation nord avant de reculer dans le crat�re
			state = try_go_angle(-PI4096/2, state, COLLECT_YELLOW_CORNER_WALL_DOWN, ERROR, FAST, CLOCKWISE, END_AT_LAST_POINT);
			break;

		case COLLECT_YELLOW_CORNER_WALL_DOWN:{
			Uint8 state0=COLLECT_YELLOW_CORNER_WALL_DOWN;
			Uint8 state1=COLLECT_YELLOW_CORNER_WALL_DOWN;
			Uint8 state2=COLLECT_YELLOW_CORNER_WALL_DOWN;
				if(entrance){
					ACT_push_order(ACT_ORE_ROLLER_ARM,  ACT_ORE_ROLLER_ARM_OUT);
					ACT_push_order(ACT_ORE_WALL,  ACT_ORE_WALL_OUT);
				}
				state0= check_act_status(ACT_QUEUE_Ore_roller_arm, state0, DONE, ERROR);
				state1= check_act_status(ACT_QUEUE_Ore_wall, state1, DONE, ERROR);
				state2= check_sub_action_result(sub_act_big_take(), state2, DONE, ERROR);

				if((state0==ERROR)||(state1==ERROR)||(state2==ERROR)){
					state=COLLECT_YELLOW_CORNER_MOVE_FOWARD;
				}
				else if((state0==DONE)&&(state1==DONE)&&(state2==DONE)){
					state=COLLECT_YELLOW_CORNER_ACTIVATE;
				}
			}break;

			case COLLECT_YELLOW_CORNER_ACTIVATE:
				if (entrance){
					// descendre le mur du rouleau et allumer le rouleau
					ACT_push_order_with_param(ACT_ORE_ROLLER_FOAM, ACT_ORE_ROLLER_FOAM_RUN, ACT_ROLLER_FOAM_SPEED_RUN);
				}
				state= check_act_status(ACT_QUEUE_Ore_roller_foam, state, COLLECT_YELLOW_CORNER_MOVE_FOWARD, ERROR);
				break;

		case COLLECT_YELLOW_CORNER_MOVE_FOWARD:
			state = try_going(1780, 2690, state, COLLECT_YELLOW_CORNER_ROLLER_UP,  ERROR_COLLECT_YELLOW_CORNER_MOVE_FOWARD, SLOW, BACKWARD, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

		case COLLECT_YELLOW_CORNER_ROLLER_UP:
			// accelere rouleau ou arr�t ...
			state = COLLECT_YELLOW_CORNER_POSITION_LEFT;
			break;

		case COLLECT_YELLOW_CORNER_POSITION_LEFT:
			state = try_going(1780, 2200, state, COLLECT_YELLOW_CORNER_END_ACTIVATE,  ERROR_COLLECT_YELLOW_CORNER_POSITION_LEFT, SLOW, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			//lien avec action suivante
			break;

		case COLLECT_YELLOW_CORNER_END_ACTIVATE:
			if (entrance){
				// descendre le mur du rouleau et allumer le rouleau
				ACT_push_order_with_param(ACT_ORE_ROLLER_FOAM, ACT_ORE_ROLLER_FOAM_STOP, ACT_ROLLER_FOAM_SPEED_RUN);
			}
			state= check_act_status(ACT_QUEUE_Ore_roller_foam, state, COLLECT_YELLOW_CORNER_WALL_UP, ERROR);
			break;

		case COLLECT_YELLOW_CORNER_WALL_UP:{
			Uint8 state0=COLLECT_YELLOW_CORNER_WALL_UP;
			Uint8 state1=COLLECT_YELLOW_CORNER_WALL_UP;
			Uint8 state2=COLLECT_YELLOW_CORNER_WALL_UP;
				if(entrance){
					ACT_push_order(ACT_ORE_ROLLER_ARM,  ACT_ORE_ROLLER_ARM_IDLE);
					ACT_push_order(ACT_ORE_WALL,  ACT_ORE_WALL_IDLE);
				}
				state0= check_act_status(ACT_QUEUE_Ore_roller_arm, state0, DONE, ERROR);
				state1= check_act_status(ACT_QUEUE_Ore_wall, state1, DONE, ERROR);
				state2= check_sub_action_result(sub_act_big_off(), state2, DONE, ERROR);

				if((state0==ERROR)||(state1==ERROR)||(state2==ERROR)){
					state=DONE;
				}
				else if((state0==DONE)&&(state1==DONE)&&(state2==DONE)){
					state=DONE;
				}
			}break;

		case ERROR_COLLECT_YELLOW_CORNER_MOVE_FOWARD:
			// il y a quelqu'un dans le cratere
			state = try_going(1780, 2200, state, COLLECT_YELLOW_CORNER_MOVE_FOWARD,  COLLECT_YELLOW_CORNER_MOVE_FOWARD, FAST, FORWARD, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

		case ERROR_COLLECT_YELLOW_CORNER_POSITION_LEFT:
			// il y a quelqu'un qui bloque la sortie du cratere
			state = try_going(1780, 2690, state, COLLECT_YELLOW_CORNER_POSITION_LEFT,  COLLECT_YELLOW_CORNER_POSITION_LEFT, FAST, BACKWARD, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

		case ERROR:
			RESET_MAE();
			return NOT_HANDLED;
			break;

		case DONE:
			RESET_MAE();
			return END_OK;
			break;

		default:
			if(entrance)
				debug_printf("default case in sub_harry_take_big_crater_yellow_corner\n");
			break;
	}
	return IN_PROGRESS;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// 4
//COLLECT_BLUE_MIDDLE
error_e sub_harry_take_big_crater_blue_middle(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_TAKE_BIG_CRATER_BLUE_MIDDLE,
			INIT,
			COLLECT_BLUE_MIDDLE_LINE,
			COLLECT_BLUE_MIDDLE_WALL_DOWN,
			COLLECT_BLUE_MIDDLE_ACTIVATE,
			COLLECT_BLUE_MIDDLE_MOVE_FOWARD,
			COLLECT_BLUE_MIDDLE_ROLLER_UP,
			COLLECT_BLUE_MIDDLE_POSITION_LEFT,
			COLLECT_BLUE_MIDDLE_END_ACTIVATE,
			COLLECT_BLUE_MIDDLE_WALL_UP,
			ERROR_COLLECT_BLUE_MIDDLE_MOVE_FOWARD,
			ERROR_COLLECT_BLUE_MIDDLE_POSITION_LEFT,
			DONE,
			ERROR
		);

	switch(state){
		case INIT:
			state = try_going(1330, 400, state, COLLECT_BLUE_MIDDLE_LINE,  ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

		case COLLECT_BLUE_MIDDLE_LINE:
			// rotation nord avant de reculer dans le crat�re
			state = try_go_angle(PI4096, state, COLLECT_BLUE_MIDDLE_WALL_DOWN, ERROR, FAST, CLOCKWISE, END_AT_LAST_POINT);
			break;

	case COLLECT_BLUE_MIDDLE_WALL_DOWN:{
		Uint8 state0=COLLECT_BLUE_MIDDLE_WALL_DOWN;
		Uint8 state1=COLLECT_BLUE_MIDDLE_WALL_DOWN;
		Uint8 state2=COLLECT_BLUE_MIDDLE_WALL_DOWN;
			if(entrance){
				ACT_push_order(ACT_ORE_ROLLER_ARM,  ACT_ORE_ROLLER_ARM_OUT);
				ACT_push_order(ACT_ORE_WALL,  ACT_ORE_WALL_OUT);
			}
			state0= check_act_status(ACT_QUEUE_Ore_roller_arm, state0, DONE, ERROR);
			state1= check_act_status(ACT_QUEUE_Ore_wall, state1, DONE, ERROR);
			state2= check_sub_action_result(sub_act_big_take(), state2, DONE, ERROR);

			if((state0==ERROR)||(state1==ERROR)||(state2==ERROR)){
				state=COLLECT_BLUE_MIDDLE_MOVE_FOWARD;
			}
			else if((state0==DONE)&&(state1==DONE)&&(state2==DONE)){
				state=COLLECT_BLUE_MIDDLE_ACTIVATE;
			}
		}break;

		case COLLECT_BLUE_MIDDLE_ACTIVATE:
			if (entrance){
				// descendre le mur du rouleau et allumer le rouleau
				ACT_push_order_with_param(ACT_ORE_ROLLER_FOAM, ACT_ORE_ROLLER_FOAM_RUN, ACT_ROLLER_FOAM_SPEED_RUN);
			}
			state= check_act_status(ACT_QUEUE_Ore_roller_foam, state, COLLECT_BLUE_MIDDLE_MOVE_FOWARD, ERROR);
			break;

		case COLLECT_BLUE_MIDDLE_MOVE_FOWARD:
			state = try_going(1690, 400, state, COLLECT_BLUE_MIDDLE_ROLLER_UP,  ERROR_COLLECT_BLUE_MIDDLE_MOVE_FOWARD, SLOW, BACKWARD, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

		case COLLECT_BLUE_MIDDLE_ROLLER_UP:
			// accelere rouleau ou arr�t ...
			state = COLLECT_BLUE_MIDDLE_POSITION_LEFT;
			break;

		case COLLECT_BLUE_MIDDLE_POSITION_LEFT:
			state = try_going(1330, 400, state, COLLECT_BLUE_MIDDLE_END_ACTIVATE,  ERROR_COLLECT_BLUE_MIDDLE_POSITION_LEFT, SLOW, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);;
			//lien avec action suivante
			break;

		case COLLECT_BLUE_MIDDLE_END_ACTIVATE:
			if (entrance){
				// descendre le mur du rouleau et allumer le rouleau
				ACT_push_order_with_param(ACT_ORE_ROLLER_FOAM, ACT_ORE_ROLLER_FOAM_STOP, ACT_ROLLER_FOAM_SPEED_RUN);
			}
			state= check_act_status(ACT_QUEUE_Ore_roller_foam, state, COLLECT_BLUE_MIDDLE_WALL_UP, ERROR);
			break;

		case COLLECT_BLUE_MIDDLE_WALL_UP:{
			Uint8 state0=COLLECT_BLUE_MIDDLE_WALL_UP;
			Uint8 state1=COLLECT_BLUE_MIDDLE_WALL_UP;
			Uint8 state2=COLLECT_BLUE_MIDDLE_WALL_UP;
				if(entrance){
					ACT_push_order(ACT_ORE_ROLLER_ARM,  ACT_ORE_ROLLER_ARM_IDLE);
					ACT_push_order(ACT_ORE_WALL,  ACT_ORE_WALL_IDLE);
				}
				state0= check_act_status(ACT_QUEUE_Ore_roller_arm, state0, DONE, ERROR);
				state1= check_act_status(ACT_QUEUE_Ore_wall, state1, DONE, ERROR);
				state2= check_sub_action_result(sub_act_big_off(), state2, DONE, ERROR);

				if((state0==ERROR)||(state1==ERROR)||(state2==ERROR)){
					state=DONE;
				}
				else if((state0==DONE)&&(state1==DONE)&&(state2==DONE)){
					state=DONE;
				}
			}break;

		case ERROR_COLLECT_BLUE_MIDDLE_MOVE_FOWARD:
			// il y a quelqu'un dans le cratere
			state = try_going(1330, 400, state, COLLECT_BLUE_MIDDLE_MOVE_FOWARD,  COLLECT_BLUE_MIDDLE_MOVE_FOWARD, FAST, FORWARD, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

		case ERROR_COLLECT_BLUE_MIDDLE_POSITION_LEFT:
			// il y a quelqu'un qui bloque la sortie du cratere
			state = try_going(1690, 400, state, COLLECT_BLUE_MIDDLE_POSITION_LEFT,  COLLECT_BLUE_MIDDLE_POSITION_LEFT, FAST, BACKWARD, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

		case ERROR:
			RESET_MAE();
			return NOT_HANDLED;
			break;

		case DONE:
			RESET_MAE();
			return END_OK;
			break;

		default:
			if(entrance)
				debug_printf("default case in sub_harry_take_big_crater_blue_middle\n");
			break;
	}
	return IN_PROGRESS;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// 5
//COLLECT_BLUE_ROCKET


error_e sub_harry_take_big_crater_blue_rocket(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_TAKE_BIG_CRATER_BLUE_FUSE,
			INIT,
			COLLECT_BLUE_ROCKET_LINE,
			COLLECT_BLUE_ROCKET_WALL_DOWN,
			COLLECT_BLUE_ROCKET_ACTIVATE,
			COLLECT_BLUE_ROCKET_MOVE_FOWARD,
			COLLECT_BLUE_ROCKET_ROLLER_UP,
			COLLECT_BLUE_ROCKET_POSITION_LEFT,
			COLLECT_BLUE_ROCKET_END_ACTIVATE,
			COLLECT_BLUE_ROCKET_WALL_UP,
			ERROR_COLLECT_BLUE_ROCKET_MOVE_FOWARD,
			ERROR_COLLECT_BLUE_ROCKET_POSITION_LEFT,
			DONE,
			ERROR
		);

	switch(state){
		case INIT:
			state = try_going(1600, 700, state, COLLECT_BLUE_ROCKET_LINE,  ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;


		case COLLECT_BLUE_ROCKET_LINE:
			// rotation nord avant de reculer dans le crat�re
			state = try_go_angle(PI4096/2, state, COLLECT_BLUE_ROCKET_WALL_DOWN, ERROR, FAST, CLOCKWISE, END_AT_LAST_POINT);
			break;

		case COLLECT_BLUE_ROCKET_WALL_DOWN:{
			Uint8 state0=COLLECT_BLUE_ROCKET_WALL_DOWN;
			Uint8 state1=COLLECT_BLUE_ROCKET_WALL_DOWN;
			Uint8 state2=COLLECT_BLUE_ROCKET_WALL_DOWN;
				if(entrance){
					ACT_push_order(ACT_ORE_ROLLER_ARM,  ACT_ORE_ROLLER_ARM_OUT);
					ACT_push_order(ACT_ORE_WALL,  ACT_ORE_WALL_OUT);
				}
				state0= check_act_status(ACT_QUEUE_Ore_roller_arm, state0, DONE, ERROR);
				state1= check_act_status(ACT_QUEUE_Ore_wall, state1, DONE, ERROR);
				state2= check_sub_action_result(sub_act_big_take(), state2, DONE, ERROR);

				if((state0==ERROR)||(state1==ERROR)||(state2==ERROR)){
					state=COLLECT_BLUE_ROCKET_MOVE_FOWARD;
				}
				else if((state0==DONE)&&(state1==DONE)&&(state2==DONE)){
					state=COLLECT_BLUE_ROCKET_ACTIVATE;
				}
			}break;

			case COLLECT_BLUE_ROCKET_ACTIVATE:
				if (entrance){
					// descendre le mur du rouleau et allumer le rouleau
					ACT_push_order_with_param(ACT_ORE_ROLLER_FOAM, ACT_ORE_ROLLER_FOAM_RUN, ACT_ROLLER_FOAM_SPEED_RUN);
				}
				state= check_act_status(ACT_QUEUE_Ore_roller_foam, state, COLLECT_BLUE_ROCKET_MOVE_FOWARD, ERROR);
				break;

		case COLLECT_BLUE_ROCKET_MOVE_FOWARD:
			state = try_going(1600, 310, state, COLLECT_BLUE_ROCKET_ROLLER_UP,  ERROR_COLLECT_BLUE_ROCKET_MOVE_FOWARD, SLOW, BACKWARD, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

		case COLLECT_BLUE_ROCKET_ROLLER_UP:
			// accelere rouleau ou arr�t ...
			state = COLLECT_BLUE_ROCKET_POSITION_LEFT;
			break;

		case COLLECT_BLUE_ROCKET_POSITION_LEFT:
			state = try_going(1600, 700, state, COLLECT_BLUE_ROCKET_END_ACTIVATE,  ERROR_COLLECT_BLUE_ROCKET_POSITION_LEFT, SLOW, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);;
			//lien avec action suivante
			break;

		case COLLECT_BLUE_ROCKET_END_ACTIVATE:
			if (entrance){
				// descendre le mur du rouleau et allumer le rouleau
				ACT_push_order_with_param(ACT_ORE_ROLLER_FOAM, ACT_ORE_ROLLER_FOAM_STOP, ACT_ROLLER_FOAM_SPEED_RUN);
			}
			state= check_act_status(ACT_QUEUE_Ore_roller_foam, state, COLLECT_BLUE_ROCKET_WALL_UP, ERROR);
			break;

		case COLLECT_BLUE_ROCKET_WALL_UP:{
			Uint8 state0=COLLECT_BLUE_ROCKET_WALL_UP;
			Uint8 state1=COLLECT_BLUE_ROCKET_WALL_UP;
			Uint8 state2=COLLECT_BLUE_ROCKET_WALL_UP;
				if(entrance){
					ACT_push_order(ACT_ORE_ROLLER_ARM,  ACT_ORE_ROLLER_ARM_IDLE);
					ACT_push_order(ACT_ORE_WALL,  ACT_ORE_WALL_IDLE);
				}
				state0= check_act_status(ACT_QUEUE_Ore_roller_arm, state0, DONE, ERROR);
				state1= check_act_status(ACT_QUEUE_Ore_wall, state1, DONE, ERROR);
				state2= check_sub_action_result(sub_act_big_off(), state2, DONE, ERROR);

				if((state0==ERROR)||(state1==ERROR)||(state2==ERROR)){
					state=DONE;
				}
				else if((state0==DONE)&&(state1==DONE)&&(state2==DONE)){
					state=DONE;
				}
			}break;

		case ERROR_COLLECT_BLUE_ROCKET_MOVE_FOWARD:
			// il y a quelqu'un dans le cratere
			state = try_going(1600, 700, state, COLLECT_BLUE_ROCKET_MOVE_FOWARD,  COLLECT_BLUE_ROCKET_MOVE_FOWARD, FAST, FORWARD, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

		case ERROR_COLLECT_BLUE_ROCKET_POSITION_LEFT:
			// il y a quelqu'un qui bloque la sortie du cratere
			state = try_going(1600, 310, state, COLLECT_BLUE_ROCKET_POSITION_LEFT,  COLLECT_BLUE_ROCKET_POSITION_LEFT, FAST, BACKWARD, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

		case ERROR:
			RESET_MAE();
			return NOT_HANDLED;
			break;

		case DONE:
			RESET_MAE();
			return END_OK;
			break;

		default:
			if(entrance)
				debug_printf("default case in sub_harry_take_big_crater_blue_rocket\n");
			break;
	}
	return IN_PROGRESS;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// 6
//COLLECT_BLUE_CORNER
error_e sub_harry_take_big_crater_blue_corner(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_TAKE_BIG_CRATER_BLUE_CORNER,
			INIT,
			COLLECT_BLUE_CORNER_POS,
			COLLECT_BLUE_CORNER_LINE,
			COLLECT_BLUE_CORNER_WALL_DOWN,
			COLLECT_BLUE_CORNER_ACTIVATE,
			COLLECT_BLUE_CORNER_MOVE_FOWARD,
			COLLECT_BLUE_CORNER_ROLLER_UP,
			COLLECT_BLUE_CORNER_POSITION_LEFT,
			COLLECT_BLUE_CORNER_END_ACTIVATE,
			COLLECT_BLUE_CORNER_WALL_UP,
			ERROR_COLLECT_BLUE_CORNER_MOVE_FOWARD,
			ERROR_COLLECT_BLUE_CORNER_POSITION_LEFT,
			DONE,
			ERROR
		);

	switch(state){
		case INIT:
			if((global.color == BLUE)&&(!ELEMENTS_get_flag(FLAG_OUR_UNICOLOR_SOUTH_IS_TAKEN))){
				state = check_sub_action_result(sub_harry_prise_module_unicolor_south(NO_SIDE),state, COLLECT_BLUE_CORNER_POS, ERROR);
			}
			else if((global.color == YELLOW)&&(!ELEMENTS_get_flag(FLAG_ADV_UNICOLOR_SOUTH_IS_TAKEN))){
				state = ERROR;//pas possible
			}
			else{
				state=COLLECT_BLUE_CORNER_POS;
			}
			break;

		case COLLECT_BLUE_CORNER_POS:
			state = try_going(1780, 800, state, COLLECT_BLUE_CORNER_LINE,  ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;


		case COLLECT_BLUE_CORNER_LINE:
			// rotation nord avant de reculer dans le crat�re
			state = try_go_angle(PI4096/2, state, COLLECT_BLUE_CORNER_WALL_DOWN, ERROR, FAST, CLOCKWISE, END_AT_LAST_POINT);
			break;

		case COLLECT_BLUE_CORNER_WALL_DOWN:{
			Uint8 state0=COLLECT_BLUE_CORNER_WALL_DOWN;
			Uint8 state1=COLLECT_BLUE_CORNER_WALL_DOWN;
			Uint8 state2=COLLECT_BLUE_CORNER_WALL_DOWN;
				if(entrance){
					ACT_push_order(ACT_ORE_ROLLER_ARM,  ACT_ORE_ROLLER_ARM_OUT);
					ACT_push_order(ACT_ORE_WALL,  ACT_ORE_WALL_OUT);
				}
				state0= check_act_status(ACT_QUEUE_Ore_roller_arm, state0, DONE, ERROR);
				state1= check_act_status(ACT_QUEUE_Ore_wall, state1, DONE, ERROR);
				state2= check_sub_action_result(sub_act_big_take(), state2, DONE, ERROR);

				if((state0==ERROR)||(state1==ERROR)||(state2==ERROR)){
					state=COLLECT_BLUE_CORNER_MOVE_FOWARD;
				}
				else if((state0==DONE)&&(state1==DONE)&&(state2==DONE)){
					state=COLLECT_BLUE_CORNER_ACTIVATE;
				}
			}break;

			case COLLECT_BLUE_CORNER_ACTIVATE:
				if (entrance){
					// descendre le mur du rouleau et allumer le rouleau
					ACT_push_order_with_param(ACT_ORE_ROLLER_FOAM, ACT_ORE_ROLLER_FOAM_RUN, ACT_ROLLER_FOAM_SPEED_RUN);
				}
				state= check_act_status(ACT_QUEUE_Ore_roller_foam, state, COLLECT_BLUE_CORNER_MOVE_FOWARD, ERROR);
				break;

		case COLLECT_BLUE_CORNER_MOVE_FOWARD:
			state = try_going(1780, 310, state, COLLECT_BLUE_CORNER_ROLLER_UP,  ERROR_COLLECT_BLUE_CORNER_MOVE_FOWARD, SLOW, BACKWARD, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

		case COLLECT_BLUE_CORNER_ROLLER_UP:
			// accelere rouleau ou arr�t ...
			state = COLLECT_BLUE_CORNER_POSITION_LEFT;
			break;

		case COLLECT_BLUE_CORNER_POSITION_LEFT:
			state = try_going(1780, 800, state, COLLECT_BLUE_CORNER_END_ACTIVATE,  ERROR_COLLECT_BLUE_CORNER_POSITION_LEFT, SLOW, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			//lien avec action suivante 1850
			break;


		case COLLECT_BLUE_CORNER_END_ACTIVATE:
			if (entrance){
				// descendre le mur du rouleau et allumer le rouleau
				ACT_push_order_with_param(ACT_ORE_ROLLER_FOAM, ACT_ORE_ROLLER_FOAM_STOP, ACT_ROLLER_FOAM_SPEED_RUN);
			}
			state= check_act_status(ACT_QUEUE_Ore_roller_foam, state, COLLECT_BLUE_CORNER_WALL_UP, ERROR);
			break;

		case COLLECT_BLUE_CORNER_WALL_UP:{
			Uint8 state0=COLLECT_BLUE_CORNER_WALL_UP;
			Uint8 state1=COLLECT_BLUE_CORNER_WALL_UP;
			Uint8 state2=COLLECT_BLUE_CORNER_WALL_UP;
				if(entrance){
					ACT_push_order(ACT_ORE_ROLLER_ARM,  ACT_ORE_ROLLER_ARM_IDLE);
					ACT_push_order(ACT_ORE_WALL,  ACT_ORE_WALL_IDLE);
				}
				state0= check_act_status(ACT_QUEUE_Ore_roller_arm, state0, DONE, ERROR);
				state1= check_act_status(ACT_QUEUE_Ore_wall, state1, DONE, ERROR);
				state2= check_sub_action_result(sub_act_big_off(), state2, DONE, ERROR);

				if((state0==ERROR)||(state1==ERROR)||(state2==ERROR)){
					state=DONE;
				}
				else if((state0==DONE)&&(state1==DONE)&&(state2==DONE)){
					state=DONE;
				}
			}break;

		case ERROR_COLLECT_BLUE_CORNER_MOVE_FOWARD:
			// il y a quelqu'un dans le cratere
			state = try_going(1780, 800, state, COLLECT_BLUE_CORNER_MOVE_FOWARD,  COLLECT_BLUE_CORNER_MOVE_FOWARD, FAST, FORWARD, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

		case ERROR_COLLECT_BLUE_CORNER_POSITION_LEFT:
			// il y a quelqu'un qui bloque la sortie du cratere
			state = try_going(1780, 310, state, COLLECT_BLUE_CORNER_POSITION_LEFT,  COLLECT_BLUE_CORNER_POSITION_LEFT, FAST, BACKWARD, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

		case ERROR:
			RESET_MAE();
			return NOT_HANDLED;
			break;

		case DONE:
			RESET_MAE();
			return END_OK;
			break;

		default:
			if(entrance)
				debug_printf("default case in sub_harry_take_big_crater_blue_corner\n");
			break;
	}

	return IN_PROGRESS;
}
*/

// sub petit crat�re
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


error_e sub_harry_take_north_little_crater(ELEMENTS_property_e minerais){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_TAKE_NORTH_LITTLE_CRATER,
			INIT,
			GET_IN,
			GO_TO_POSITION,
			TURN_TO_RUSH,
			DOWN_SYSTEM,
			GO_TO_CLEAT,
			MOVE_BACK,
			GET_OUT,
			DONE,
			ERROR
		);

	switch(state){
		case INIT:
			if(IHM_switchs_get(SWITCH_DISABLE_ORE)){
				state = ERROR; // L'actionneur minerais a �t� d�sactiv�
			}else if(ELEMENTS_get_flag(FLAG_OUR_NORTH_CRATER_IS_TAKEN)&&minerais == OUR_ELEMENT){
				state = DONE;
			}else if(ELEMENTS_get_flag(FLAG_ADV_NORTH_CRATER_IS_TAKEN)&&minerais == ADV_ELEMENT){
							state = DONE;
			}else if(ELEMENTS_get_flag(FLAG_SUB_ANNE_TAKE_CYLINDER_OUR_CENTER)){
				state=ERROR;
			}else{
				state=GET_IN;


				// On l�ve le flag de subaction
				if(minerais == OUR_ELEMENT){
					ELEMENTS_set_flag(FLAG_SUB_HARRY_OUR_NORTH_CRATER, TRUE);
				}else{
					ELEMENTS_set_flag(FLAG_SUB_HARRY_ADV_NORTH_CRATER, TRUE);
				}

			}
			break;

		case GET_IN:
			state=check_sub_action_result(sub_harry_get_in_north_little_crater(minerais), state, GO_TO_POSITION, ERROR);
			break;

		case GO_TO_POSITION:
			if((minerais == OUR_ELEMENT && global.color == BLUE) || (minerais == ADV_ELEMENT && global.color == YELLOW)){
				state=try_going(950, 650, state, TURN_TO_RUSH, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			}else{
				state=try_going(950, 2350, state, TURN_TO_RUSH, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			}
			break;

		case TURN_TO_RUSH:
			if((minerais == OUR_ELEMENT && global.color == BLUE) || (minerais == ADV_ELEMENT && global.color == YELLOW)){
				state=try_go_angle(0, state, DOWN_SYSTEM, ERROR, FAST, ANY_WAY, END_AT_LAST_POINT);
			}else{
				state=try_go_angle(180, state, DOWN_SYSTEM, ERROR, FAST, ANY_WAY, END_AT_LAST_POINT);
			}
			break;

		case DOWN_SYSTEM:{
			Uint8 state1=state;
			Uint8 state2=state;
			if (entrance){
				ACT_push_order_with_param(ACT_ORE_ROLLER_FOAM, ACT_ORE_ROLLER_FOAM_RUN, ACT_ROLLER_FOAM_SPEED_RUN);
				ACT_push_order(ACT_ORE_ROLLER_ARM, ACT_ORE_ROLLER_ARM_OUT);
				ACT_push_order(ACT_ORE_WALL, ACT_ORE_WALL_OUT);
			}
			state1=check_act_status(ACT_QUEUE_Ore_roller_arm, DOWN_SYSTEM, GO_TO_CLEAT, ERROR);
			state2=check_act_status(ACT_QUEUE_Ore_wall, DOWN_SYSTEM, GO_TO_CLEAT, ERROR);

			if((state1==ERROR)||(state2==ERROR)){
				state=ERROR;
			}else if ((state1==GO_TO_CLEAT)&&(state2==GO_TO_CLEAT)){
				state=GO_TO_CLEAT;
			}else{ // Add a else just to be secure
				state=GO_TO_CLEAT;
			}
		}
		break;

		case GO_TO_CLEAT:
			if((minerais == OUR_ELEMENT && global.color == BLUE) || (minerais == ADV_ELEMENT && global.color == YELLOW)){
				state=try_going(600, 650, state, MOVE_BACK, MOVE_BACK, SLOW, BACKWARD, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			}else{
				state=try_going(600, 2350, state, MOVE_BACK, MOVE_BACK, SLOW, BACKWARD, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			}
			break;

		case MOVE_BACK:
			if((minerais == OUR_ELEMENT && global.color == BLUE) || (minerais == ADV_ELEMENT && global.color == YELLOW)){
				state=try_going(800, 650, state, GET_OUT, GO_TO_CLEAT, SLOW, FORWARD, NO_DODGE_AND_NO_WAIT, END_AT_BRAKE);
			}else{
				state=try_going(800, 2350, state, GET_OUT, GO_TO_CLEAT, SLOW, FORWARD, NO_DODGE_AND_NO_WAIT, END_AT_BRAKE);
			}
			if(ON_LEAVE()){
				ACT_push_order(ACT_ORE_ROLLER_FOAM, ACT_ORE_ROLLER_FOAM_STOP);
				ACT_push_order(ACT_ORE_ROLLER_ARM, ACT_ORE_ROLLER_ARM_IDLE);
				ACT_push_order(ACT_ORE_WALL, ACT_ORE_WALL_IDLE);
			}
			break;

		case GET_OUT:
			if((minerais == OUR_ELEMENT && global.color == BLUE) || (minerais == ADV_ELEMENT && global.color == YELLOW)){
				state=try_going(950, 650, state, DONE, DONE, FAST, FORWARD, NO_DODGE_AND_NO_WAIT, END_AT_BRAKE);
			}else{
				state=try_going(950, 2350, state, DONE, DONE, FAST, FORWARD, NO_DODGE_AND_NO_WAIT, END_AT_BRAKE);
			}
			break;

		case ERROR:
			RESET_MAE();
			on_turning_point();

			if(minerais == OUR_ELEMENT){
				ELEMENTS_set_flag(FLAG_SUB_HARRY_OUR_NORTH_CRATER, FALSE);
			}else{
				ELEMENTS_set_flag(FLAG_SUB_HARRY_ADV_NORTH_CRATER, FALSE);
			}

			return NOT_HANDLED;
			break;

		case DONE:
			RESET_MAE();
			on_turning_point();

			if(minerais == OUR_ELEMENT){
				ELEMENTS_set_flag(FLAG_SUB_HARRY_OUR_NORTH_CRATER, FALSE);	// flag subaction
				ELEMENTS_set_flag(FLAG_OUR_NORTH_CRATER_IS_TAKEN, TRUE);	// flag element
			}else{
				ELEMENTS_set_flag(FLAG_SUB_HARRY_ADV_NORTH_CRATER, FALSE);	// flag subaction
				ELEMENTS_set_flag(FLAG_ADV_NORTH_CRATER_IS_TAKEN, TRUE);	// flag element
			}
			//ELEMENTS_set_flag(FLAG_HARRY_STOMACH_IS_FULL, TRUE);

			return END_OK;
			break;

		default:
			if(entrance)
				debug_printf("default case in sub_harry_take_north_little_crater\n");
			break;
	}

	return IN_PROGRESS;
}




error_e sub_harry_get_in_north_little_crater(ELEMENTS_property_e minerais){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_GET_IN_NORTH_LITTLE_CRATER,
			INIT,
			GET_IN_OUR_SQUARE,
			GET_IN_MIDDLE_SQUARE,
			GET_IN_ADV_SQUARE,
			PATHFIND,
			DONE,
			ERROR
		);

	switch(state){
		case INIT:
			// Prise crat�re c�t� jaune
			if((minerais == OUR_ELEMENT && global.color == BLUE) || (minerais == ADV_ELEMENT && global.color == YELLOW)){
				if(i_am_in_square(800, 1400, 350, 900)){
					state = DONE;//GET_IN_OUR_SQUARE;
				}else if (i_am_in_square(300, 1100, 900, 2100)){
					state = GET_IN_MIDDLE_SQUARE;
				}else if (i_am_in_square(800, 1400, 2100, 2700)){
					state = GET_IN_ADV_SQUARE;
				}else{
					state = PATHFIND;
				}
			}else{ // Prise crat�re cot� bleu
				if(i_am_in_square(800, 1400, 2100, 2700)){
					state = DONE;//GET_IN_OUR_SQUARE;
				}else if (i_am_in_square(300, 1100, 900, 2100)){
					state = GET_IN_MIDDLE_SQUARE;
				}else if (i_am_in_square(800, 1400, 350, 900)){
					state = GET_IN_ADV_SQUARE;
				}else{
					state = PATHFIND;
				}
			}
			break;

		case GET_IN_OUR_SQUARE:
			if((minerais == OUR_ELEMENT && global.color == BLUE) || (minerais == ADV_ELEMENT && global.color == YELLOW)){
				state = try_going(1000, 650, state, DONE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_BRAKE);
				//pas utiliser pour l'instant je pr�f�re le mettre dans la fonction principale
			}else{
				state = try_going(1000, 2350, state, DONE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_BRAKE);
				//pas utiliser pour l'instant je pr�f�re le mettre dans la fonction principale
			}
			break;

		case GET_IN_MIDDLE_SQUARE:
			if((minerais == OUR_ELEMENT && global.color == BLUE) || (minerais == ADV_ELEMENT && global.color == YELLOW)){
				state = try_going(900, 1000, state, DONE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_BRAKE);
			}else{
				state = try_going(900, 2000, state, DONE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_BRAKE);
			}
			break;

		case GET_IN_ADV_SQUARE:
			if((minerais == OUR_ELEMENT && global.color == BLUE) || (minerais == ADV_ELEMENT && global.color == YELLOW)){
				state = try_going(850, 2000, state, GET_IN_MIDDLE_SQUARE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_BRAKE);
			}else{
				state = try_going(850, 1000, state, GET_IN_MIDDLE_SQUARE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_BRAKE);
			}
			break;

		case PATHFIND:
			if((minerais == OUR_ELEMENT && global.color == BLUE) || (minerais == ADV_ELEMENT && global.color == YELLOW)){
				state = ASTAR_try_going(850, 650, state, DONE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_BRAKE);
			}else{
				state = ASTAR_try_going(850, 2350, state, DONE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_BRAKE);
			}
			break;

		case ERROR:
			RESET_MAE();
			return NOT_HANDLED;
			break;

		case DONE:
			RESET_MAE();
			return END_OK;
			break;

		default:
			if(entrance)
				debug_printf("default case in sub_harry_get_in_north_little_crater\n");
			break;
	}

	return IN_PROGRESS;
}


error_e sub_harry_take_south_little_crater(ELEMENTS_property_e minerais){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_TAKE_SOUTH_LITTLE_CRATER,
			INIT,
			GET_IN,
			GO_TO_POSITION,
			TURN_TO_RUSH,
			DOWN_SYSTEM,
			GO_TO_CENTER,
			MOVE_BACK,
			GET_OUT,
			DONE,
			ERROR
		);

	switch(state){
		case INIT:
			if(IHM_switchs_get(SWITCH_DISABLE_ORE)){
				state = ERROR; // L'actionneur minerais a �t� d�sactiv�
			}else if(ELEMENTS_get_flag(FLAG_OUR_SOUTH_CRATER_IS_TAKEN)&&minerais == OUR_ELEMENT){
				state = DONE;
			}else if(ELEMENTS_get_flag(FLAG_ADV_SOUTH_CRATER_IS_TAKEN)&&minerais == ADV_ELEMENT){
							state = DONE;
			}else if((minerais == OUR_ELEMENT && (ELEMENTS_get_flag(FLAG_SUB_ANNE_TAKE_CYLINDER_SOUTH_UNI) || ELEMENTS_get_flag(FLAG_SUB_ANNE_DEPOSE_CYLINDER_OUR_DIAGONAL)))
					|| (minerais == ADV_ELEMENT && ELEMENTS_get_flag(FLAG_SUB_ANNE_DEPOSE_CYLINDER_ADV_DIAGONAL))){
				state=ERROR;
			}else{
				state=GET_IN;


				// On l�ve le flag de subaction
				if(minerais == OUR_ELEMENT){
					ELEMENTS_set_flag(FLAG_SUB_HARRY_OUR_SOUTH_CRATER, TRUE);
				}else{
					ELEMENTS_set_flag(FLAG_SUB_HARRY_ADV_SOUTH_CRATER, TRUE);
				}

			}
			break;

		case GET_IN:
			state=check_sub_action_result(sub_harry_get_in_south_little_crater(minerais), state, GO_TO_POSITION, ERROR);
			break;

		case GO_TO_POSITION:
			if((minerais == OUR_ELEMENT && global.color == BLUE) || (minerais == ADV_ELEMENT && global.color == YELLOW)){
				state=try_going(1680, 800, state, TURN_TO_RUSH, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			}else{
				state=try_going(1680, 2200, state, TURN_TO_RUSH, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			}
			break;

		case TURN_TO_RUSH:
			if((minerais == OUR_ELEMENT && global.color == BLUE) || (minerais == ADV_ELEMENT && global.color == YELLOW)){
				state=try_go_angle(-2*PI4096/3, state, DOWN_SYSTEM, ERROR, FAST, ANY_WAY, END_AT_LAST_POINT);
			}else{
				state=try_go_angle(2*PI4096/3, state, DOWN_SYSTEM, ERROR, FAST, ANY_WAY, END_AT_LAST_POINT);
			}
			break;

		case DOWN_SYSTEM:{
			Uint8 state1=state;
			Uint8 state2=state;
			if (entrance){
				ACT_push_order_with_param(ACT_ORE_ROLLER_FOAM, ACT_ORE_ROLLER_FOAM_RUN, ACT_ROLLER_FOAM_SPEED_RUN);
				ACT_push_order(ACT_ORE_ROLLER_ARM, ACT_ORE_ROLLER_ARM_OUT);
				ACT_push_order(ACT_ORE_WALL, ACT_ORE_WALL_OUT);
			}

			state1=check_act_status(ACT_QUEUE_Ore_roller_arm, DOWN_SYSTEM, GO_TO_CENTER, ERROR);
			state2=check_act_status(ACT_QUEUE_Ore_wall, DOWN_SYSTEM, GO_TO_CENTER, ERROR);

			if((state1==ERROR)||(state2==ERROR)){
				state=ERROR;
			}else if ((state1==GO_TO_CENTER)&&(state2==GO_TO_CENTER)){
				state=GO_TO_CENTER;
			}else{ // Add a else just to be secure
				state = DONE;
			}
		}
		break;

		case GO_TO_CENTER:
			if((minerais == OUR_ELEMENT && global.color == BLUE) || (minerais == ADV_ELEMENT && global.color == YELLOW)){
				state=try_going(1820, 980, state, MOVE_BACK, MOVE_BACK, SLOW, BACKWARD, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			}else{
				state=try_going(1820, 2020, state, MOVE_BACK, MOVE_BACK, SLOW, BACKWARD, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			}
			break;

		case MOVE_BACK:
			if((minerais == OUR_ELEMENT && global.color == BLUE) || (minerais == ADV_ELEMENT && global.color == YELLOW)){
				state=try_going(1680, 800, state, GET_OUT, GO_TO_CENTER, SLOW, FORWARD, NO_DODGE_AND_NO_WAIT, END_AT_BRAKE);
			}else{
				state=try_going(1680, 2200, state, GET_OUT, GO_TO_CENTER, SLOW, FORWARD, NO_DODGE_AND_NO_WAIT, END_AT_BRAKE);
			}
			if(ON_LEAVE()){
				ACT_push_order(ACT_ORE_ROLLER_FOAM, ACT_ORE_ROLLER_FOAM_STOP);
				ACT_push_order(ACT_ORE_ROLLER_ARM, ACT_ORE_ROLLER_ARM_IDLE);
				ACT_push_order(ACT_ORE_WALL, ACT_ORE_WALL_IDLE);
			}
			break;

		case GET_OUT:
			if((minerais == OUR_ELEMENT && global.color == BLUE) || (minerais == ADV_ELEMENT && global.color == YELLOW)){
				state=try_going(1500, 650, state, DONE, DONE, FAST, FORWARD, NO_DODGE_AND_NO_WAIT, END_AT_BRAKE);
			}else{
				state=try_going(1500, 2350, state, DONE, DONE, FAST, FORWARD, NO_DODGE_AND_NO_WAIT, END_AT_BRAKE);
			}
			break;

		case ERROR:
			RESET_MAE();
			on_turning_point();

			if(minerais == OUR_ELEMENT){
				ELEMENTS_set_flag(FLAG_SUB_HARRY_OUR_SOUTH_CRATER, FALSE);
			}else{
				ELEMENTS_set_flag(FLAG_SUB_HARRY_ADV_SOUTH_CRATER, FALSE);
			}

			return NOT_HANDLED;
			break;

		case DONE:
			RESET_MAE();
			on_turning_point();


			if(minerais == OUR_ELEMENT){
				ELEMENTS_set_flag(FLAG_SUB_HARRY_OUR_SOUTH_CRATER, FALSE); // flag subaction
				ELEMENTS_set_flag(FLAG_OUR_SOUTH_CRATER_IS_TAKEN, TRUE);  // flag element
			}else{
				ELEMENTS_set_flag(FLAG_SUB_HARRY_ADV_SOUTH_CRATER, FALSE);	// flag subaction
				ELEMENTS_set_flag(FLAG_ADV_SOUTH_CRATER_IS_TAKEN, TRUE);	// flag element
			}
			//ELEMENTS_set_flag(FLAG_HARRY_STOMACH_IS_FULL, TRUE);

			return END_OK;
			break;

		default:
			if(entrance)
				debug_printf("default case in sub_harry_take_south_little_crater\n");
			break;
	}

	return IN_PROGRESS;
}



error_e sub_harry_get_in_south_little_crater(ELEMENTS_property_e minerais){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_GET_IN_SOUTH_LITTLE_CRATER,
			INIT,
			GET_IN_OUR_SQUARE,
			GET_IN_MIDDLE_SQUARE,
			GET_IN_ADV_SQUARE,
			PATHFIND,
			DONE,
			ERROR
		);

	switch(state){
		case INIT:
			// Prise crat�re cot� jaune
			if((minerais == OUR_ELEMENT && global.color == BLUE) || (minerais == ADV_ELEMENT && global.color == YELLOW)){
				if(i_am_in_square(800, 1400, 350, 900)){
					state = GET_IN_OUR_SQUARE; // DONE;
				}else if (i_am_in_square(300, 1050, 900, 2100)){
					state = GET_IN_MIDDLE_SQUARE;
				}else if (i_am_in_square(800, 1400, 2100, 2700)){
					state = GET_IN_ADV_SQUARE;
				}else{
					state = PATHFIND;
				}
			}else{	// Prise crat�re cot� bleu
				if(i_am_in_square(800, 1400, 2100, 2700)){
					state = DONE;//GET_IN_OUR_SQUARE;
				}else if (i_am_in_square(300, 1050, 900, 2100)){
					state = GET_IN_MIDDLE_SQUARE;
				}else if (i_am_in_square(800, 1400, 350, 900)){
					state = GET_IN_ADV_SQUARE;
				}else{
					state = PATHFIND;
				}
			}
			break;

		case GET_IN_OUR_SQUARE:
			if((minerais == OUR_ELEMENT && global.color == BLUE) || (minerais == ADV_ELEMENT && global.color == YELLOW)){
				state = try_going(1330, 600, state, DONE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_BRAKE);
			}else{
				state = try_going(1330, 2400, state, DONE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_BRAKE);
			}
			break;

		case GET_IN_MIDDLE_SQUARE:
			if((minerais == OUR_ELEMENT && global.color == BLUE) || (minerais == ADV_ELEMENT && global.color == YELLOW)){
				state = try_going(900, 1000, state, GET_IN_OUR_SQUARE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_BRAKE);
			}else{
				state = try_going(900, 2000, state, GET_IN_OUR_SQUARE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_BRAKE);
			}
			break;

		case GET_IN_ADV_SQUARE:
			if((minerais == OUR_ELEMENT && global.color == BLUE) || (minerais == ADV_ELEMENT && global.color == YELLOW)){
				state = try_going(850, 2000, state, GET_IN_MIDDLE_SQUARE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_BRAKE);
			}else{
				state = try_going(850, 1000, state, GET_IN_MIDDLE_SQUARE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_BRAKE);
			}
			break;

		case PATHFIND:
			if((minerais == OUR_ELEMENT && global.color == BLUE) || (minerais == ADV_ELEMENT && global.color == YELLOW)){
				state = ASTAR_try_going(1630, 700, state, DONE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_BRAKE);
			}else{
				state = ASTAR_try_going(1630, 2350, state, DONE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_BRAKE);
			}
			break;

		case ERROR:
			RESET_MAE();
			return NOT_HANDLED;
			break;

		case DONE:
			RESET_MAE();
			return END_OK;
			break;

		default:
			if(entrance)
				debug_printf("default case in sub_harry_get_in_south_little_crater\n");
			break;
	}

	return IN_PROGRESS;
}



// strat un SEUL passage dans le cratere sans flag
error_e sub_harry_take_big_crater(ELEMENTS_property_e minerais){ // OUR_ELEMENT / ADV_ELEMENT
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_TAKE_BIG_CRATER,
			INIT,
			SUB_MOVE_POS_YELLOW,
			SUB_TAKE_YELLOW,
			SUB_MOVE_POS_BLUE,
			SUB_TAKE_BLUE,
			GET_OUT_YELLOW,
			GET_OUT_BLUE,
			ERROR,
			ERROR_GET_OUT_YELLOW,
			ERROR_GET_OUT_BLUE,
			DONE
		);

	switch(state){
		case INIT:
			debug_printf("flag_our:%d\tflag_adv:%d", ELEMENTS_get_flag(FLAG_OUR_CORNER_CRATER_IS_TAKEN), ELEMENTS_get_flag(FLAG_ADV_CORNER_CRATER_IS_TAKEN));
			if(IHM_switchs_get(SWITCH_DISABLE_ORE)){
				state = ERROR; // L'actionneur minerais a �t� d�sactiv�
			}else if((ELEMENTS_get_flag(FLAG_OUR_CORNER_CRATER_IS_TAKEN) && minerais == OUR_ELEMENT)
					|| (ELEMENTS_get_flag(FLAG_ADV_CORNER_CRATER_IS_TAKEN) && minerais == ADV_ELEMENT)){
				state = DONE;	// L'action a d�j� �t� faite
			}/*else if(ELEMENTS_get_flag(FLAG_SUB_ANNE_TAKE_ORE)){ 			//! anne peut prendre les minerais
				state=ERROR; // L'autre robot fait la sub
			}*/
			else{
				if (minerais != OUR_ELEMENT && minerais != ADV_ELEMENT){
					state = ERROR;

				}else if(minerais ==OUR_ELEMENT){
					if(global.color == YELLOW){
						ELEMENTS_set_flag(FLAG_SUB_HARRY_OUR_CORNER_CRATER, TRUE);
						state = SUB_MOVE_POS_YELLOW;

					}else{
						ELEMENTS_set_flag(FLAG_SUB_HARRY_OUR_CORNER_CRATER, TRUE);
						state = SUB_MOVE_POS_BLUE;

					}
				}else{
					if(global.color == YELLOW){
						ELEMENTS_set_flag(FLAG_SUB_HARRY_ADV_CORNER_CRATER, TRUE);
						state = SUB_MOVE_POS_BLUE;
					}else{
						ELEMENTS_set_flag(FLAG_SUB_HARRY_ADV_CORNER_CRATER, TRUE);
						state = SUB_MOVE_POS_YELLOW;
					}
				}
			}
			break;

		case SUB_MOVE_POS_YELLOW:
			state = check_sub_action_result(sub_harry_take_big_crater_move_pos_yellow(), state, SUB_TAKE_YELLOW, ERROR);
			break;

		case SUB_TAKE_YELLOW:
			state = check_sub_action_result(sub_harry_take_big_crater_yellow(), state, GET_OUT_YELLOW, ERROR);
			break;

		case SUB_MOVE_POS_BLUE:
			state = check_sub_action_result(sub_harry_take_big_crater_move_pos_blue(), state, SUB_TAKE_BLUE, ERROR);
			break;

		case SUB_TAKE_BLUE:
			state = check_sub_action_result(sub_harry_take_big_crater_blue(), state, GET_OUT_BLUE, ERROR);
			break;

// revoir la sortie pos
		case GET_OUT_BLUE: //try go
			state = try_going(1310, 440, GET_OUT_BLUE, DONE,  ERROR_GET_OUT_BLUE, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

		case GET_OUT_YELLOW: //try go
			state = try_going(1310, 2560, GET_OUT_YELLOW, DONE,  ERROR_GET_OUT_YELLOW, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

		case ERROR_GET_OUT_YELLOW:
			state = try_going(1780, 2200, ERROR_GET_OUT_YELLOW, GET_OUT_YELLOW,  GET_OUT_YELLOW, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

		case ERROR_GET_OUT_BLUE:
			state = try_going(1780, 800, ERROR_GET_OUT_BLUE, GET_OUT_BLUE,  GET_OUT_BLUE, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

		case ERROR:
			RESET_MAE();
			on_turning_point();
			return NOT_HANDLED;
			break;

		case DONE:
			RESET_MAE();
			on_turning_point();
			if(minerais == OUR_ELEMENT){
				ELEMENTS_set_flag(FLAG_SUB_HARRY_OUR_CORNER_CRATER, FALSE);
				ELEMENTS_set_flag(FLAG_OUR_CORNER_CRATER_IS_TAKEN, TRUE);
			}else{
				ELEMENTS_set_flag(FLAG_SUB_HARRY_ADV_CORNER_CRATER, FALSE);
				ELEMENTS_set_flag(FLAG_ADV_CORNER_CRATER_IS_TAKEN, TRUE);
			}
			ELEMENTS_set_flag(FLAG_HARRY_STOMACH_IS_FULL, TRUE);
			return END_OK;
			break;

		default:
			if(entrance)
				debug_printf("default case in sub_harry_take_big_crater\n");
			break;
	}
	return IN_PROGRESS;
}


// 1
// COLLECT_YELLOW
error_e sub_harry_take_big_crater_yellow(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_TAKE_BIG_CRATER_YELLOW,
			INIT,
			COLLECT_YELLOW_LINE,
			COLLECT_YELLOW_BIG_BALL,
			COLLECT_YELLOW_WALL_DOWN,
			COLLECT_YELLOW_ACTIVATE,
			COLLECT_YELLOW_MOVE_FOWARD,
			COLLECT_YELLOW_ROLLER_UP,
			COLLECT_YELLOW_POSITION_LEFT,
			COLLECT_YELLOW_END_ACTIVATE,
			COLLECT_YELLOW_BIG_BALL_OUT,
			COLLECT_YELLOW_WALL_UP,
			ERROR_COLLECT_YELLOW_MOVE_FOWARD,
			ERROR_COLLECT_YELLOW_POSITION_LEFT,


			COLLECT_YELLOW_IN,
			COLLECT_YELLOW_PUSHER_LEFT,
			COLLECT_YELLOW_ROTATE_LEFT,
			COLLECT_YELLOW_ROTATE_LEFT_END,
			COLLECT_YELLOW_PUSHER_LEFT_UP,

			COLLECT_YELLOW_PUSHER_RIGHT,
			COLLECT_YELLOW_ROTATE_RIGHT,
			COLLECT_YELLOW_ROTATE_RIGHT_END,
			COLLECT_YELLOW_PUSHER_RIGHT_UP,
			COLLECT_YELLOW_OUT,

			DONE,
			ERROR
		);

	switch(state){
		case INIT:
			state = try_going(1440, 2440, state, COLLECT_YELLOW_LINE,  ERROR, FAST, BACKWARD, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

		case COLLECT_YELLOW_LINE:
			// rotation nord avant de reculer dans le crat�re
			state = try_go_angle(-PI4096/4, state, COLLECT_YELLOW_BIG_BALL, ERROR, FAST, CLOCKWISE, END_AT_LAST_POINT);
			break;

		case COLLECT_YELLOW_BIG_BALL:{
			Uint8 state0=COLLECT_YELLOW_BIG_BALL;
			Uint8 state1=COLLECT_YELLOW_BIG_BALL;
			Uint8 state2=COLLECT_YELLOW_BIG_BALL;
			Uint8 state3=COLLECT_YELLOW_BIG_BALL;
			Uint8 state4=COLLECT_YELLOW_BIG_BALL;

				if(entrance){
					ACT_push_order(ACT_BIG_BALL_FRONT_LEFT,  ACT_BIG_BALL_FRONT_LEFT_UP);
					ACT_push_order(ACT_BIG_BALL_FRONT_RIGHT,  ACT_BIG_BALL_FRONT_RIGHT_UP);
					ACT_push_order(ACT_BIG_BALL_BACK_LEFT,  ACT_BIG_BALL_BACK_LEFT_UP);
					ACT_push_order(ACT_BIG_BALL_BACK_RIGHT,  ACT_BIG_BALL_BACK_RIGHT_UP);
					ACT_push_order(ACT_BEARING_BALL_WHEEL,  ACT_BEARING_BALL_WHEEL_UP);
				}
				state0= check_act_status(ACT_QUEUE_Big_bearing_front_left, state0, DONE, ERROR);
				state1= check_act_status(ACT_QUEUE_Big_bearing_front_right, state1, DONE, ERROR);
				state2= check_act_status(ACT_QUEUE_Big_bearing_back_left, state2, DONE, ERROR);
				state3= check_act_status(ACT_QUEUE_Big_bearing_back_right, state3, DONE, ERROR);
				state4= check_act_status(ACT_QUEUE_Bearing_ball_wheel, state4, DONE, ERROR);

				if((state0==ERROR)||(state1==ERROR)||(state2==ERROR)||(state3==ERROR)||(state4==ERROR)){
					state=ERROR;
				}
				else if((state0==DONE)&&(state1==DONE)&&(state2==DONE)&&(state3==DONE)&&(state4==DONE)){
					state=COLLECT_YELLOW_WALL_DOWN;
				}
			}break;

	case COLLECT_YELLOW_WALL_DOWN:{
		Uint8 state0=COLLECT_YELLOW_WALL_DOWN;
		Uint8 state1=COLLECT_YELLOW_WALL_DOWN;
		Uint8 state2=COLLECT_YELLOW_WALL_DOWN;
			if(entrance){
				ACT_push_order(ACT_ORE_ROLLER_ARM,  ACT_ORE_ROLLER_ARM_OUT);
				ACT_push_order(ACT_ORE_WALL,  ACT_ORE_WALL_OUT);
			}
			state0= check_act_status(ACT_QUEUE_Ore_roller_arm, state0, DONE, ERROR);
			state1= check_act_status(ACT_QUEUE_Ore_wall, state1, DONE, ERROR);
			state2= check_sub_action_result(sub_act_big_take(), state2, DONE, ERROR);

			if((state0==ERROR)||(state1==ERROR)||(state2==ERROR)){
				state=COLLECT_YELLOW_ACTIVATE;
			}
			else if((state0==DONE)&&(state1==DONE)&&(state2==DONE)){
				state=COLLECT_YELLOW_ACTIVATE;
			}
		}break;

		case COLLECT_YELLOW_ACTIVATE:
			if (entrance){
				// descendre le mur du rouleau et allumer le rouleau
				ACT_push_order_with_param(ACT_ORE_ROLLER_FOAM, ACT_ORE_ROLLER_FOAM_RUN, ACT_ROLLER_FOAM_SPEED_RUN);
			}
			state= check_act_status(ACT_QUEUE_Ore_roller_foam, state, COLLECT_YELLOW_IN, ERROR);
			break;

		case COLLECT_YELLOW_IN:
			// entree dans le cratere
			state = try_going(1680, 2650, state, DONE,  ERROR, FAST, BACKWARD, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;// stop secu

		case COLLECT_YELLOW_PUSHER_LEFT:
			//remonter le rouleau si perimetre trop large
			if (entrance){
				ACT_push_order(ACT_CYLINDER_PUSHER_LEFT, ACT_CYLINDER_PUSHER_LEFT_OUT);
			}
			state= check_act_status(ACT_QUEUE_Cylinder_pusher_left, state, COLLECT_YELLOW_ROTATE_LEFT, ERROR);
			break;

		case COLLECT_YELLOW_ROTATE_LEFT:
			state = try_go_angle(-PI4096*7/12, state, COLLECT_YELLOW_ROTATE_LEFT_END, ERROR, FAST, CLOCKWISE, END_AT_LAST_POINT);
			break;

		case COLLECT_YELLOW_ROTATE_LEFT_END:
			state = try_go_angle(-PI4096/4, state, COLLECT_YELLOW_PUSHER_LEFT_UP, ERROR, FAST, TRIGOWISE, END_AT_LAST_POINT);
			break;

		case COLLECT_YELLOW_PUSHER_LEFT_UP:
			if (entrance){
				ACT_push_order(ACT_CYLINDER_PUSHER_LEFT, ACT_CYLINDER_PUSHER_LEFT_IN);
			}
			state= check_act_status(ACT_QUEUE_Cylinder_pusher_left, state, COLLECT_YELLOW_PUSHER_RIGHT, ERROR);
			break;



		case COLLECT_YELLOW_PUSHER_RIGHT:
			//remonter le rouleau si perimetre trop large
			if (entrance){
				ACT_push_order(ACT_CYLINDER_PUSHER_RIGHT, ACT_CYLINDER_PUSHER_RIGHT_OUT);
			}
			state= check_act_status(ACT_QUEUE_Cylinder_pusher_right, state, COLLECT_YELLOW_ROTATE_RIGHT, ERROR);
			break;

		case COLLECT_YELLOW_ROTATE_RIGHT:
			state = try_go_angle(PI4096/12, state, COLLECT_YELLOW_ROTATE_RIGHT_END, ERROR, FAST, TRIGOWISE, END_AT_LAST_POINT);
			break;

		case COLLECT_YELLOW_ROTATE_RIGHT_END:
			state = try_go_angle(-PI4096/4, state, COLLECT_YELLOW_PUSHER_RIGHT_UP, ERROR, FAST, CLOCKWISE, END_AT_LAST_POINT);
			break;

		case COLLECT_YELLOW_PUSHER_RIGHT_UP:
			if (entrance){
				ACT_push_order(ACT_CYLINDER_PUSHER_RIGHT, ACT_CYLINDER_PUSHER_RIGHT_IN);
			}
			state= check_act_status(ACT_QUEUE_Cylinder_pusher_right, state, COLLECT_YELLOW_OUT, ERROR);
			break;

		case COLLECT_YELLOW_OUT:
			state = try_going(1390, 2390, state, COLLECT_YELLOW_END_ACTIVATE, COLLECT_YELLOW_END_ACTIVATE, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			//lien avec action suivante 1850
			break;


		case COLLECT_YELLOW_END_ACTIVATE:
			if (entrance){
				// descendre le mur du rouleau et allumer le rouleau
				ACT_push_order_with_param(ACT_ORE_ROLLER_FOAM, ACT_ORE_ROLLER_FOAM_STOP, ACT_ROLLER_FOAM_SPEED_RUN);
			}
			state= check_act_status(ACT_QUEUE_Ore_roller_foam, state, COLLECT_YELLOW_BIG_BALL_OUT, ERROR);
			break;

		case COLLECT_YELLOW_BIG_BALL_OUT:{
			Uint8 state0=COLLECT_YELLOW_BIG_BALL_OUT;
			Uint8 state1=COLLECT_YELLOW_BIG_BALL_OUT;
			Uint8 state2=COLLECT_YELLOW_BIG_BALL_OUT;
			Uint8 state3=COLLECT_YELLOW_BIG_BALL_OUT;
			Uint8 state4=COLLECT_YELLOW_BIG_BALL_OUT;

				if(entrance){
					ACT_push_order(ACT_BIG_BALL_FRONT_LEFT,  ACT_BIG_BALL_FRONT_LEFT_DOWN);
					ACT_push_order(ACT_BIG_BALL_FRONT_RIGHT,  ACT_BIG_BALL_FRONT_RIGHT_DOWN);
					ACT_push_order(ACT_BIG_BALL_BACK_LEFT,  ACT_BIG_BALL_BACK_LEFT_DOWN);
					ACT_push_order(ACT_BIG_BALL_BACK_RIGHT,  ACT_BIG_BALL_BACK_RIGHT_DOWN);
					ACT_push_order(ACT_BEARING_BALL_WHEEL,  ACT_BEARING_BALL_WHEEL_DOWN);
				}
				state0= check_act_status(ACT_QUEUE_Big_bearing_front_left, state0, DONE, ERROR);
				state1= check_act_status(ACT_QUEUE_Big_bearing_front_right, state1, DONE, ERROR);
				state2= check_act_status(ACT_QUEUE_Big_bearing_back_left, state2, DONE, ERROR);
				state3= check_act_status(ACT_QUEUE_Big_bearing_back_right, state3, DONE, ERROR);
				state4= check_act_status(ACT_QUEUE_Bearing_ball_wheel, state4, DONE, ERROR);

				if((state0==ERROR)||(state1==ERROR)||(state2==ERROR)||(state3==ERROR)||(state4==ERROR)){
					state=ERROR;
				}
				else if((state0==DONE)&&(state1==DONE)&&(state2==DONE)&&(state3==DONE)&&(state4==DONE)){
					state=COLLECT_YELLOW_WALL_UP;
				}
			}break;

		case COLLECT_YELLOW_WALL_UP:{
			Uint8 state0=COLLECT_YELLOW_WALL_UP;
			Uint8 state1=COLLECT_YELLOW_WALL_UP;
			Uint8 state2=COLLECT_YELLOW_WALL_UP;
				if(entrance){
					ACT_push_order(ACT_ORE_ROLLER_ARM,  ACT_ORE_ROLLER_ARM_IDLE);
					ACT_push_order(ACT_ORE_WALL,  ACT_ORE_WALL_IDLE);
				}
				state0= check_act_status(ACT_QUEUE_Ore_roller_arm, state0, DONE, ERROR);
				state1= check_act_status(ACT_QUEUE_Ore_wall, state1, DONE, ERROR);
				state2= check_sub_action_result(sub_act_big_off(), state2, DONE, ERROR);

				if((state0==ERROR)||(state1==ERROR)||(state2==ERROR)){
					state=DONE;
				}
				else if((state0==DONE)&&(state1==DONE)&&(state2==DONE)){
					if(global.color == BLUE){
						ELEMENTS_set_flag(FLAG_OUR_CORNER_CRATER_IS_TAKEN,TRUE); // crater vide
					}
					else{
						ELEMENTS_set_flag(FLAG_ADV_CORNER_CRATER_IS_TAKEN,TRUE); // crater vide
					}
					state=DONE;
				}
			}break;



		case ERROR_COLLECT_YELLOW_MOVE_FOWARD:
			// il y a quelqu'un dans le cratere
			state = try_going(1390, 2390, state, COLLECT_YELLOW_MOVE_FOWARD,  COLLECT_YELLOW_MOVE_FOWARD, FAST, FORWARD, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

		case ERROR_COLLECT_YELLOW_POSITION_LEFT:
			// il y a quelqu'un qui bloque la sortie du cratere
			state = try_going(1680, 2650, state, COLLECT_YELLOW_POSITION_LEFT,  COLLECT_YELLOW_POSITION_LEFT, FAST, BACKWARD, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

		case ERROR:
			RESET_MAE();
			return NOT_HANDLED;
			break;

		case DONE:
			RESET_MAE();
			return END_OK;
			break;

		default:
			if(entrance)
				debug_printf("default case in sub_harry_take_big_crater_yellow_middle\n");
			break;
	}
	return IN_PROGRESS;
}


// 2
//COLLECT_BLUE
error_e sub_harry_take_big_crater_blue(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_TAKE_BIG_CRATER_BLUE,
			INIT,
			COLLECT_BLUE_LINE,
			COLLECT_BLUE_BIG_BALL,
			COLLECT_BLUE_WALL_DOWN,
			COLLECT_BLUE_ACTIVATE,
			COLLECT_BLUE_MOVE_FOWARD,
			COLLECT_BLUE_ROLLER_UP,
			COLLECT_BLUE_POSITION_LEFT,
			COLLECT_BLUE_END_ACTIVATE,
			COLLECT_BLUE_BIG_BALL_OUT,
			COLLECT_BLUE_WALL_UP,
			ERROR_COLLECT_BLUE_MOVE_FOWARD,
			ERROR_COLLECT_BLUE_POSITION_LEFT,



			COLLECT_BLUE_IN,
			COLLECT_BLUE_PUSHER_LEFT,
			COLLECT_BLUE_ROTATE_LEFT,
			COLLECT_BLUE_ROTATE_LEFT_END,
			COLLECT_BLUE_PUSHER_LEFT_UP,

			COLLECT_BLUE_PUSHER_RIGHT,
			COLLECT_BLUE_ROTATE_RIGHT,
			COLLECT_BLUE_ROTATE_RIGHT_END,
			COLLECT_BLUE_PUSHER_RIGHT_UP,
			COLLECT_BLUE_OUT,

			DONE,
			ERROR
		);

	switch(state){
		case INIT:
			state = try_going(1440, 560, state, COLLECT_BLUE_LINE,  ERROR, FAST, BACKWARD, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

		case COLLECT_BLUE_LINE:
			// rotation nord avant de reculer dans le crat�re
			//state = try_go_angle(-PI4096/4, state, COLLECT_BLUE_BIG_BALL, ERROR, FAST, CLOCKWISE, END_AT_LAST_POINT);
			state = COLLECT_BLUE_BIG_BALL;
			break;

		case COLLECT_BLUE_BIG_BALL:{
			Uint8 state0=COLLECT_BLUE_BIG_BALL;
			Uint8 state1=COLLECT_BLUE_BIG_BALL;
			Uint8 state2=COLLECT_BLUE_BIG_BALL;
			Uint8 state3=COLLECT_BLUE_BIG_BALL;
			Uint8 state4=COLLECT_BLUE_BIG_BALL;

				if(entrance){
					ACT_push_order(ACT_BIG_BALL_FRONT_LEFT,  ACT_BIG_BALL_FRONT_LEFT_UP);
					ACT_push_order(ACT_BIG_BALL_FRONT_RIGHT,  ACT_BIG_BALL_FRONT_RIGHT_UP);
					ACT_push_order(ACT_BIG_BALL_BACK_LEFT,  ACT_BIG_BALL_BACK_LEFT_UP);
					ACT_push_order(ACT_BIG_BALL_BACK_RIGHT,  ACT_BIG_BALL_BACK_RIGHT_UP);
					ACT_push_order(ACT_BEARING_BALL_WHEEL,  ACT_BEARING_BALL_WHEEL_UP);
				}
				state0= check_act_status(ACT_QUEUE_Big_bearing_front_left, state0, DONE, ERROR);
				state1= check_act_status(ACT_QUEUE_Big_bearing_front_right, state1, DONE, ERROR);
				state2= check_act_status(ACT_QUEUE_Big_bearing_back_left, state2, DONE, ERROR);
				state3= check_act_status(ACT_QUEUE_Big_bearing_back_right, state3, DONE, ERROR);
				state4= check_act_status(ACT_QUEUE_Bearing_ball_wheel, state4, DONE, ERROR);

				if((state0==ERROR)||(state1==ERROR)||(state2==ERROR)||(state3==ERROR)||(state4==ERROR)){
					state=ERROR;
				}
				else if((state0==DONE)&&(state1==DONE)&&(state2==DONE)&&(state3==DONE)&&(state4==DONE)){
					state=COLLECT_BLUE_WALL_DOWN;
				}
			}break;

	case COLLECT_BLUE_WALL_DOWN:{
		Uint8 state0=COLLECT_BLUE_WALL_DOWN;
		Uint8 state1=COLLECT_BLUE_WALL_DOWN;
		Uint8 state2=COLLECT_BLUE_WALL_DOWN;
			if(entrance){
				ACT_push_order(ACT_ORE_ROLLER_ARM,  ACT_ORE_ROLLER_ARM_OUT);
				ACT_push_order(ACT_ORE_WALL,  ACT_ORE_WALL_OUT);
			}
			state0= check_act_status(ACT_QUEUE_Ore_roller_arm, state0, DONE, ERROR);
			state1= check_act_status(ACT_QUEUE_Ore_wall, state1, DONE, ERROR);
			state2= check_sub_action_result(sub_act_big_take(), state2, DONE, ERROR);

			if((state0==ERROR)||(state1==ERROR)||(state2==ERROR)){
				state=COLLECT_BLUE_ACTIVATE;
			}
			else if((state0==DONE)&&(state1==DONE)&&(state2==DONE)){
				state=COLLECT_BLUE_ACTIVATE;
			}
		}break;

		case COLLECT_BLUE_ACTIVATE:
			if (entrance){
				// descendre le mur du rouleau et allumer le rouleau
				ACT_push_order_with_param(ACT_ORE_ROLLER_FOAM, ACT_ORE_ROLLER_FOAM_RUN, ACT_ROLLER_FOAM_SPEED_RUN);
			}
			state= check_act_status(ACT_QUEUE_Ore_roller_foam, state, COLLECT_BLUE_IN, ERROR);
			break;

		case COLLECT_BLUE_IN:
			// entree dans le cratere
			state = try_going(1680, 350, state, DONE,  ERROR, SLOW, BACKWARD, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;// stop secu

		case COLLECT_BLUE_PUSHER_LEFT:
			//remonter le rouleau si perimetre trop large
			state = COLLECT_BLUE_OUT;

			//state = COLLECT_BLUE_ROTATE_LEFT;
			/*
			if (entrance){
				ACT_push_order(ACT_CYLINDER_PUSHER_LEFT, ACT_CYLINDER_PUSHER_LEFT_OUT);
			}
			state= check_act_status(ACT_QUEUE_Cylinder_pusher_left, state, COLLECT_BLUE_ROTATE_LEFT, ERROR);
			*/
			break;

		case COLLECT_BLUE_ROTATE_LEFT:
			state = try_go_angle(-PI4096*7/12, state, COLLECT_BLUE_ROTATE_LEFT_END, ERROR, FAST, CLOCKWISE, END_AT_LAST_POINT);
			break;

		case COLLECT_BLUE_ROTATE_LEFT_END:
			state = try_go_angle(-PI4096/4, state, COLLECT_BLUE_PUSHER_LEFT_UP, ERROR, FAST, TRIGOWISE, END_AT_LAST_POINT);
			break;

		case COLLECT_BLUE_PUSHER_LEFT_UP:
			state = COLLECT_BLUE_PUSHER_RIGHT;
			/*
			if (entrance){
				ACT_push_order(ACT_CYLINDER_PUSHER_LEFT, ACT_CYLINDER_PUSHER_LEFT_IN);
			}
			state= check_act_status(ACT_QUEUE_Cylinder_pusher_left, state, COLLECT_BLUE_PUSHER_RIGHT, ERROR);
			*/
			break;



		case COLLECT_BLUE_PUSHER_RIGHT:
			//remonter le rouleau si perimetre trop large
			state = COLLECT_BLUE_ROTATE_RIGHT;
			/*
			if (entrance){
				ACT_push_order(ACT_CYLINDER_PUSHER_RIGHT, ACT_CYLINDER_PUSHER_RIGHT_OUT);
			}
			state= check_act_status(ACT_QUEUE_Cylinder_pusher_right, state, COLLECT_BLUE_ROTATE_RIGHT, ERROR);
			*/
			break;

		case COLLECT_BLUE_ROTATE_RIGHT:
			state = try_go_angle(PI4096/12, state, COLLECT_BLUE_ROTATE_RIGHT_END, ERROR, FAST, TRIGOWISE, END_AT_LAST_POINT);
			break;

		case COLLECT_BLUE_ROTATE_RIGHT_END:
			state = try_go_angle(-PI4096/4, state, COLLECT_BLUE_PUSHER_RIGHT_UP, ERROR, FAST, CLOCKWISE, END_AT_LAST_POINT);
			break;

		case COLLECT_BLUE_PUSHER_RIGHT_UP:
			state = COLLECT_BLUE_OUT;
			/*
			if (entrance){
				ACT_push_order(ACT_CYLINDER_PUSHER_RIGHT, ACT_CYLINDER_PUSHER_RIGHT_IN);
			}
			state= check_act_status(ACT_QUEUE_Cylinder_pusher_right, state, COLLECT_BLUE_OUT, ERROR);
			*/
			break;

		case COLLECT_BLUE_OUT: // name
			state = try_going(1390, 610, state, COLLECT_BLUE_END_ACTIVATE, COLLECT_BLUE_END_ACTIVATE, SLOW, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			//lien avec action suivante 1850
			break;


		case COLLECT_BLUE_END_ACTIVATE:
			if (entrance){
				// descendre le mur du rouleau et allumer le rouleau
				ACT_push_order_with_param(ACT_ORE_ROLLER_FOAM, ACT_ORE_ROLLER_FOAM_STOP, ACT_ROLLER_FOAM_SPEED_RUN);
			}
			state= check_act_status(ACT_QUEUE_Ore_roller_foam, state, COLLECT_BLUE_BIG_BALL_OUT, ERROR);
			break;

		case COLLECT_BLUE_BIG_BALL_OUT:{
			Uint8 state0=COLLECT_BLUE_BIG_BALL_OUT;
			Uint8 state1=COLLECT_BLUE_BIG_BALL_OUT;
			Uint8 state2=COLLECT_BLUE_BIG_BALL_OUT;
			Uint8 state3=COLLECT_BLUE_BIG_BALL_OUT;
			Uint8 state4=COLLECT_BLUE_BIG_BALL_OUT;

				if(entrance){
					ACT_push_order(ACT_BIG_BALL_FRONT_LEFT,  ACT_BIG_BALL_FRONT_LEFT_DOWN);
					ACT_push_order(ACT_BIG_BALL_FRONT_RIGHT,  ACT_BIG_BALL_FRONT_RIGHT_DOWN);
					ACT_push_order(ACT_BIG_BALL_BACK_LEFT,  ACT_BIG_BALL_BACK_LEFT_DOWN);
					ACT_push_order(ACT_BIG_BALL_BACK_RIGHT,  ACT_BIG_BALL_BACK_RIGHT_DOWN);
					ACT_push_order(ACT_BEARING_BALL_WHEEL,  ACT_BEARING_BALL_WHEEL_DOWN);
				}
				state0= check_act_status(ACT_QUEUE_Big_bearing_front_left, state0, DONE, ERROR);
				state1= check_act_status(ACT_QUEUE_Big_bearing_front_right, state1, DONE, ERROR);
				state2= check_act_status(ACT_QUEUE_Big_bearing_back_left, state2, DONE, ERROR);
				state3= check_act_status(ACT_QUEUE_Big_bearing_back_right, state3, DONE, ERROR);
				state4= check_act_status(ACT_QUEUE_Bearing_ball_wheel, state4, DONE, ERROR);

				if((state0==ERROR)||(state1==ERROR)||(state2==ERROR)||(state3==ERROR)||(state4==ERROR)){
					state=ERROR;
				}
				else if((state0==DONE)&&(state1==DONE)&&(state2==DONE)&&(state3==DONE)&&(state4==DONE)){
					state=COLLECT_BLUE_WALL_UP;
				}
			}break;

		case COLLECT_BLUE_WALL_UP:{
			Uint8 state0=COLLECT_BLUE_WALL_UP;
			Uint8 state1=COLLECT_BLUE_WALL_UP;
			Uint8 state2=COLLECT_BLUE_WALL_UP;
				if(entrance){
					ACT_push_order(ACT_ORE_ROLLER_ARM,  ACT_ORE_ROLLER_ARM_IDLE);
					ACT_push_order(ACT_ORE_WALL,  ACT_ORE_WALL_IDLE);
				}
				state0= check_act_status(ACT_QUEUE_Ore_roller_arm, state0, DONE, ERROR);
				state1= check_act_status(ACT_QUEUE_Ore_wall, state1, DONE, ERROR);
				state2= check_sub_action_result(sub_act_big_off(), state2, DONE, ERROR);

				if((state0==ERROR)||(state1==ERROR)||(state2==ERROR)){
					state=DONE;
				}
				else if((state0==DONE)&&(state1==DONE)&&(state2==DONE)){
					if(global.color == BLUE){
						ELEMENTS_set_flag(FLAG_OUR_CORNER_CRATER_IS_TAKEN,TRUE); // crater vide
					}
					else{
						ELEMENTS_set_flag(FLAG_ADV_CORNER_CRATER_IS_TAKEN,TRUE); // crater vide
					}
					state=DONE;
				}
			}break;

		case ERROR_COLLECT_BLUE_MOVE_FOWARD:
			// il y a quelqu'un dans le cratere
			state = try_going(1390, 610, state, COLLECT_BLUE_MOVE_FOWARD,  COLLECT_BLUE_MOVE_FOWARD, FAST, FORWARD, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

		case ERROR_COLLECT_BLUE_POSITION_LEFT:
			// il y a quelqu'un qui bloque la sortie du cratere
			state = try_going(1680, 350, state, COLLECT_BLUE_POSITION_LEFT,  COLLECT_BLUE_POSITION_LEFT, FAST, BACKWARD, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

		case ERROR:
			RESET_MAE();
			return NOT_HANDLED;
			break;

		case DONE:
			RESET_MAE();
			return END_OK;
			break;

		default:
			if(entrance)
				debug_printf("default case in sub_harry_take_big_crater_blue_middle\n");
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
			GO_YELLOW_CRATER_FROM_BLUE_SQUARE,
			GO_YELLOW_CRATER_FROM_MIDDLE_SQUARE,
			GO_YELLOW_CRATER_FROM_YELLOW_SQUARE,
			ASTAR_GO_YELLOW_CRATER,
			GET_IN,
			ERROR,
			DONE

		);

	const displacement_t leave_middle_square[2] = { {(GEOMETRY_point_t){900, 2200}, FAST},
										  {(GEOMETRY_point_t){1390, 2290}, FAST},
										  };

	const displacement_t leave_blue_square[3] = { {(GEOMETRY_point_t){900, 800}, FAST},
										  {(GEOMETRY_point_t){900, 2200}, FAST},
										  {(GEOMETRY_point_t){1390, 2290}, FAST}
										  };

	switch(state){
		// deplacement robot
		case INIT:
			state = GET_IN;
			break;

		case GET_IN:
			if(i_am_in_square(400, 1500, 200, 800)){
				state = GO_YELLOW_CRATER_FROM_BLUE_SQUARE;
			}
			else if(i_am_in_square(400, 1500, 2700, 2200)){
				state = GO_YELLOW_CRATER_FROM_YELLOW_SQUARE;
			}
			else if(i_am_in_square(350, 1200, 800, 2200)){
				state = GO_YELLOW_CRATER_FROM_MIDDLE_SQUARE;
			}else{
				state = ASTAR_GO_YELLOW_CRATER;
			}
			break;

		case GO_YELLOW_CRATER_FROM_BLUE_SQUARE:
			state = try_going_multipoint( leave_blue_square, 3, state, DONE, ASTAR_GO_YELLOW_CRATER, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

		case GO_YELLOW_CRATER_FROM_MIDDLE_SQUARE:
			state = try_going_multipoint( leave_middle_square, 2, state, DONE, ASTAR_GO_YELLOW_CRATER, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

		case GO_YELLOW_CRATER_FROM_YELLOW_SQUARE:
			state = try_going(1390, 2290, state, DONE, ASTAR_GO_YELLOW_CRATER, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

		case ASTAR_GO_YELLOW_CRATER:
			state = ASTAR_try_going(1390, 2290, state, DONE,  ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

		case ERROR:
			RESET_MAE();
			return NOT_HANDLED;
			break;

		case DONE:
			RESET_MAE();
			return END_OK;
			break;

		default:
			if(entrance)
				debug_printf("default case in sub_harry_take_big_crater_move_pos_yellow\n");
			break;
	}

	return IN_PROGRESS;
}



//aller cratere bleu
error_e sub_harry_take_big_crater_move_pos_blue(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_TAKE_BIG_CRATER_MOVE_POS_BLUE,
			INIT,
			GO_BLUE_CRATER_FROM_YELLOW_SQUARE,
			GO_BLUE_CRATER_FROM_MIDDLE_SQUARE,
			GO_BLUE_CRATER_FROM_BLUE_SQUARE,
			ASTAR_GO_BLUE_CRATER,
			GET_IN,
			ERROR,
			DONE

		);

	const displacement_t leave_middle_square[2] = { {(GEOMETRY_point_t){900, 800}, FAST},
										  {(GEOMETRY_point_t){1390, 610}, FAST},
										  };

	const displacement_t leave_yellow_square[3] = { {(GEOMETRY_point_t){900, 2200}, FAST},
										  {(GEOMETRY_point_t){900, 800}, FAST},
										  {(GEOMETRY_point_t){1390, 610}, FAST}
										  };

	switch(state){
		// deplacement robot
		case INIT:
			state = GET_IN;
			break;

		case GET_IN:
			if(i_am_in_square(400, 1500, 200, 800)){
				state = GO_BLUE_CRATER_FROM_BLUE_SQUARE;
			}
			else if(i_am_in_square(400, 1500, 2700, 2200)){
				state = GO_BLUE_CRATER_FROM_YELLOW_SQUARE;
			}
			else if(i_am_in_square(350, 1200, 800, 2200)){
				state = GO_BLUE_CRATER_FROM_MIDDLE_SQUARE;
			}else{
				state = ASTAR_GO_BLUE_CRATER;
			}
			break;


		case GO_BLUE_CRATER_FROM_YELLOW_SQUARE:
			state = try_going_multipoint( leave_yellow_square, 3, state, DONE, ASTAR_GO_BLUE_CRATER, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

		case GO_BLUE_CRATER_FROM_MIDDLE_SQUARE:
			state = try_going_multipoint( leave_middle_square, 2, state, DONE, ASTAR_GO_BLUE_CRATER, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

		case GO_BLUE_CRATER_FROM_BLUE_SQUARE:
			state = try_going(1390, 610, state, DONE, ASTAR_GO_BLUE_CRATER, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

		case ASTAR_GO_BLUE_CRATER:
			state = ASTAR_try_going(1390, 610, state, DONE,  ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

		case ERROR:
			RESET_MAE();
			return NOT_HANDLED;
			break;

		case DONE:
			RESET_MAE();
			return END_OK;
			break;

		default:
			if(entrance)
				debug_printf("default case in sub_harry_take_big_crater_move_pos_blue\n");
			break;
	}
	return IN_PROGRESS;
}






/// sale

/*
// strat un SEUL passage dans le cratere
error_e sub_harry_take_big_crater(ELEMENTS_property_e minerais){ // OUR_ELEMENT / ADV_ELEMENT
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_TAKE_BIG_CRATER,
			INIT,
			SUB_MOVE_POS_YELLOW,
			SUB_TAKE_YELLOW_MIDDLE,
			SUB_TAKE_YELLOW_ROCKET,
			SUB_TAKE_YELLOW_CORNER,
			SUB_MOVE_POS_BLUE,
			SUB_TAKE_BLUE_MIDDLE,
			SUB_TAKE_BLUE_ROCKET,
			SUB_TAKE_BLUE_CORNER,
			GET_OUT_YELLOW,
			GET_OUT_BLUE,
			ERROR,
			ERROR_GET_OUT_YELLOW,
			ERROR_GET_OUT_BLUE,
			DONE
		);

	switch(state){
		case INIT:
			debug_printf("flag_our:%d\tflag_adv:%d", ELEMENTS_get_flag(FLAG_OUR_CORNER_CRATER_IS_TAKEN), ELEMENTS_get_flag(FLAG_ADV_CORNER_CRATER_IS_TAKEN));
			if(IHM_switchs_get(SWITCH_DISABLE_ORE)){
				state = ERROR; // L'actionneur minerais a �t� d�sactiv�
			}else if((ELEMENTS_get_flag(FLAG_OUR_CORNER_CRATER_IS_TAKEN) && minerais == OUR_ELEMENT)
					|| (ELEMENTS_get_flag(FLAG_ADV_CORNER_CRATER_IS_TAKEN) && minerais == ADV_ELEMENT)){
				state = DONE;	// L'action a d�j� �t� faite
			}




			/*else if(ELEMENTS_get_flag(FLAG_SUB_ANNE_TAKE_ORE)){

//! anne peut prendre les minerais
				state=ERROR; // L'autre robot fait la sub
			}*/
/*


	switch(state){
		case INIT:
			if((global.color == BLUE)&&(!ELEMENTS_get_flag(FLAG_OUR_UNICOLOR_SOUTH_IS_TAKEN))){
				state = check_sub_action_result(sub_harry_prise_module_unicolor_south(NO_SIDE),state, COLLECT_BLUE_POS, ERROR);
			}
			else if((global.color == YELLOW)&&(!ELEMENTS_get_flag(FLAG_ADV_UNICOLOR_SOUTH_IS_TAKEN))){
				state = ERROR;//pas possible
			}
			else{
				state=COLLECT_BLUE_POS;
			}
			break;

			///////////////////////

		case COLLECT_BLUE_IN:
			// entree dans le cratere
			state = try_going(1680, 350, state, COLLECT_BLUE_ANGLE,  ERROR, FAST, BACKWARD, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

		case COLLECT_BLUE_PUSHER_LEFT:
			//remonter le rouleau si perimetre trop large
			if (entrance){
				ACT_push_order(ACT_CYLINDER_PUSHER_LEFT, ACT_CYLINDER_PUSHER_LEFT_OUT);
			}
			state= check_act_status(ACT_QUEUE_Cylinder_pusher_left, state, COLLECT_BLUE_ROTATE_LEFT, ERROR);
			break;

		case COLLECT_BLUE_ROTATE_LEFT:
			state = try_go_angle(-PI4096*7/12, state, COLLECT_BLUE_ROTATE_LEFT_END, ERROR, FAST, CLOCKWISE, END_AT_LAST_POINT);
			break;

		case COLLECT_BLUE_ROTATE_LEFT_END:
			state = try_go_angle(-PI4096/4, state, COLLECT_BLUE_PUSHER_LEFT_UP, ERROR, FAST, TRIGOWISE, END_AT_LAST_POINT);
			break;

		case COLLECT_BLUE_PUSHER_LEFT_UP:
			if (entrance){
				ACT_push_order(ACT_CYLINDER_PUSHER_LEFT, ACT_CYLINDER_PUSHER_LEFT_IN);
			}
			state= check_act_status(ACT_QUEUE_Cylinder_pusher_left, state, COLLECT_BLUE_ROTATE_LEFT, ERROR);
			break;

		case COLLECT_BLUE_PUSHER_RIGHT:
			//remonter le rouleau si perimetre trop large
			if (entrance){
				ACT_push_order(ACT_CYLINDER_PUSHER_RIGHT, ACT_CYLINDER_PUSHER_RIGHT_OUT);
			}
			state= check_act_status(ACT_QUEUE_Cylinder_pusher_right, state, COLLECT_BLUE_ROTATE_RIGHT, ERROR);
			break;

		case COLLECT_BLUE_ROTATE_RIGHT:
			state = try_go_angle(PI4096/12, state, COLLECT_BLUE_ROTATE_RIGHT_END, ERROR, FAST, TRIGOWISE, END_AT_LAST_POINT);
			break;

		case COLLECT_BLUE_ROTATE_RIGHT_END:
			state = try_go_angle(-PI4096/4, state, COLLECT_BLUE_PUSHER_RIGHT_UP, ERROR, FAST, CLOCKWISE, END_AT_LAST_POINT);
			break;

		case COLLECT_BLUE_PUSHER_RIGHT_UP:
			if (entrance){
				ACT_push_order(ACT_CYLINDER_PUSHER_RIGHT, ACT_CYLINDER_PUSHER_RIGHT_IN);
			}
			state= check_act_status(ACT_QUEUE_Cylinder_pusher_right, state, COLLECT_BLUE_ROTATE_RIGHT, ERROR);
			break;

		case COLLECT_BLUE_GET_OUT:
			state = try_going(1390, 610, state, COLLECT_BLUE_VALIDATE, ERROR_COLLECT_BLUE_GET_OUT, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			//lien avec action suivante 1850
			break;

/////////////////////////////
		case COLLECT_BLUE_VALIDATE:{
			Uint8 state0=COLLECT_BLUE_VALIDATE;
			Uint8 state1=COLLECT_BLUE_VALIDATE;
			Uint8 state2=COLLECT_BLUE_VALIDATE;
			Uint8 state3=COLLECT_BLUE_VALIDATE;

			if(entrance){
				// remonter le rouleau et le mur  ! eteindre le rouleau avant!
				ACT_push_order(ACT_ORE_ROLLER_FOAM, ACT_ORE_ROLLER_FOAM_STOP);
				ACT_push_order(ACT_ORE_ROLLER_ARM,  ACT_ORE_ROLLER_ARM_IDLE);
				ACT_push_order(ACT_ORE_WALL,  ACT_ORE_WALL_IDLE);
			}

			state0= check_act_status(ACT_QUEUE_Ore_roller_arm, state0, DONE,ERROR);
			state1= check_act_status(ACT_QUEUE_Ore_wall, state1, DONE, ERROR);
			state2= check_act_status(ACT_QUEUE_Ore_roller_foam, state2, DONE,ERROR);
			state3= check_sub_action_result(sub_act_big_off(), state, DONE,ERROR);


			if((state0==ERROR)||(state1==ERROR)||(state2==ERROR)||(state3==ERROR)){
				state=ERROR;
			}
			else if((state0==DONE)&&(state1==DONE)&&(state2==DONE)&&(state3==DONE)){
				if(global.color == BLUE){
					ELEMENTS_set_flag(FLAG_OUR_CORNER_CRATER_IS_TAKEN,TRUE); // crater vide
				}
				else{
					ELEMENTS_set_flag(FLAG_ADV_CORNER_CRATER_IS_TAKEN,TRUE); // crater vide
				}
				state=DONE;
			}
			else{	// Add a else just to be secure
				if(global.color == BLUE){
					ELEMENTS_set_flag(FLAG_OUR_CORNER_CRATER_IS_TAKEN,TRUE); // crater vide
				}
				else{
					ELEMENTS_set_flag(FLAG_ADV_CORNER_CRATER_IS_TAKEN,TRUE); // crater vide
				}
				state=DONE;
			}
		}break;


		case ERROR_COLLECT_BLUE_IN:
			// il y a quelqu'un dans le cratere
			state = try_going(1390, 610, state, COLLECT_BLUE_IN,  ERROR, FAST, FORWARD, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

		case ERROR_COLLECT_BLUE_GET_OUT:
			// il y a quelqu'un qui bloque la sortie du cratere
			state = try_going(1680, 350, state, COLLECT_BLUE_GET_OUT, COLLECT_BLUE_GET_OUT, FAST, BACKWARD, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

		case ERROR:
			RESET_MAE();
			return NOT_HANDLED;
			break;

		case DONE:
			RESET_MAE();
			return END_OK;
			break;

		default:
			if(entrance)
				debug_printf("default case in sub_harry_take_big_crater_blue\n");
			break;
	}

	return IN_PROGRESS;
}
*/

