/*
 *	Club Robot ESEO 2015 - 2016
 *	Black & Pearl
 *
 *	Fichier : actions_both_2016.c
 *	Package : Carte S²/strats_2016
 *	Description : Tests des actions réalisables par le robot
 *	Auteur : Arnaud
 *	Version 2016
 */


#include "actions_both_2017.h"
#include "../QS/QS_stateMachineHelper.h"
#include "../QS/QS_outputlog.h"
#include "../propulsion/prop_functions.h"
#include "../propulsion/movement.h"
#include "../utils/generic_functions.h"

error_e sub_cross_rocker(void){
	CREATE_MAE_WITH_VERBOSE(SM_ID_SUB_CROSS_ROCKER,
			INIT,
			CROSS,
			CORRECT_ODOMETRY_MATH,
			CORRECT_ODOMETRY_MEASURE,
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
			state = try_going(175, COLOR_Y(950), state, CORRECT_ODOMETRY_MATH, ERROR, SLOW, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case CORRECT_ODOMETRY_MATH:{
			Sint16 diffY = COLOR_EXP(-10, 10);

			PROP_set_position(global.pos.x, global.pos.y + diffY, global.pos.angle);

			state = CORRECT_ODOMETRY_MEASURE;
			}break;

		case CORRECT_ODOMETRY_MEASURE:

			// COCO ON T'ATTEND !

			state = DONE;
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
			on_turning_point();
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

