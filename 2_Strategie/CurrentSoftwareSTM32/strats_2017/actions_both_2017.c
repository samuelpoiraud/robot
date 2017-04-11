/*
 *	Club Robot ESEO 2015 - 2016
 *	Black & Pearl
 *
 *	Fichier : actions_both_2016.c
 *	Package : Carte S�/strats_2016
 *	Description : Tests des actions r�alisables par le robot
 *	Auteur : Arnaud
 *	Version 2016
 */


#include "actions_both_2017.h"
#include "../QS/QS_stateMachineHelper.h"
#include "../QS/QS_outputlog.h"
#include "../propulsion/prop_functions.h"
#include "../propulsion/movement.h"
#include "../utils/generic_functions.h"
#include "../actuator/queue.h"
#include "../actuator/act_functions.h"

error_e sub_cross_rocker(void){
	CREATE_MAE_WITH_VERBOSE(SM_ID_SUB_CROSS_ROCKER,
			INIT,
			CROSS,
			CORRECT_ODOMETRY_MATH,
			CORRECT_ODOMETRY_MEASURE,
			GET_OUT,
			GO_BACK,
			ERROR,
			DONE
		);

	switch (state) {
		case INIT:
			if(i_am_in_square_color(0, 350, 0, 400))
				state = CROSS;
			else
				state = ERROR;
			break;

		case CROSS:
			state = try_going(175, COLOR_Y(900), state, CORRECT_ODOMETRY_MATH, ERROR, SLOW, FORWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case CORRECT_ODOMETRY_MATH:{
			if(I_AM_SMALL()){
				ACT_push_order(ACT_QUEUE_Small_bearing_back, ACT_SMALL_BALL_BACK_DOWN);
				ACT_push_order(ACT_QUEUE_Small_bearing_front_left, ACT_SMALL_BALL_FRONT_LEFT_DOWN);
				ACT_push_order(ACT_QUEUE_Small_bearing_front_right, ACT_SMALL_BALL_FRONT_RIGHT_DOWN);
			}else{
				ACT_push_order(ACT_QUEUE_Big_bearing_back_left, ACT_BIG_BALL_BACK_LEFT_DOWN);
				ACT_push_order(ACT_QUEUE_Big_bearing_back_right, ACT_BIG_BALL_BACK_RIGHT_DOWN);
				ACT_push_order(ACT_QUEUE_Big_bearing_front_left, ACT_BIG_BALL_FRONT_LEFT_DOWN);
				ACT_push_order(ACT_QUEUE_Big_bearing_front_right, ACT_BIG_BALL_FRONT_RIGHT_DOWN);
			}
			Sint16 diffY = COLOR_EXP(-10, 10);
#warning 'c\'est plus que bancal cette histoire'
			PROP_set_position(global.pos.x, global.pos.y + diffY, global.pos.angle);

			state = CORRECT_ODOMETRY_MEASURE;
			}break;

		case CORRECT_ODOMETRY_MEASURE:

			// COCO ON T'ATTEND !

			state = GET_OUT;
			break;

		case GET_OUT:
			state = try_going(250, COLOR_Y(1000), state, DONE, GO_BACK, SLOW, FORWARD, NO_DODGE_AND_WAIT, END_AT_BRAKE);
			break;

		case GO_BACK:
			state = try_going(175, COLOR_Y(900), state, GET_OUT, GET_OUT, FAST, BACKWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case DONE:
			RESET_MAE();
			on_turning_point();
			return END_OK;

		case ERROR:
			RESET_MAE();
			on_turning_point();
			return NOT_HANDLED;

		default:
			if(entrance)
				debug_printf("default case in sub_cross_rocker\n");
			break;
	}

	return IN_PROGRESS;
}



error_e sub_wait_1_sec(void){
	CREATE_MAE_WITH_VERBOSE(SM_ID_SUB_WAIT_ONE_SEC,
							INIT,
							DONE,
							ERROR
							);


	switch (state) {
		case INIT:
			if(entrance){
				on_turning_point();
			}
			state = wait_time(1000, INIT, DONE);
			break;

		case DONE:
			RESET_MAE();
			on_turning_point();
			return END_OK;
			break;

		case ERROR:
			RESET_MAE();
			on_turning_point();
			return NOT_HANDLED;
			break;

		default:
			if(entrance)
				debug_printf("default case in sub_wait_1_sec\n");
			break;
	}
	return IN_PROGRESS;
}

// Subaction de test de l'�vitement (A ne pas jouer en match)
error_e strat_test_avoidance(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_TEST_AVOIDANCE,
			INIT,
			ACTION,
			GO_POINT_1,
			GO_POINT_2,
			ERROR,
			DONE
		);

	switch(state){

		case INIT:
			state = try_going(900, COLOR_Y(500), state, GO_POINT_2, GO_POINT_2, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case GO_POINT_1:
			state = try_going(900, COLOR_Y(500), state, GO_POINT_2, GO_POINT_2, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case GO_POINT_2:
			state = try_going(900, COLOR_Y(2500), state, GO_POINT_1, GO_POINT_1, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		default:
			if(entrance)
				debug_printf("default case in strat_test_avoidance\n");
			break;
	}

	return IN_PROGRESS;
}
