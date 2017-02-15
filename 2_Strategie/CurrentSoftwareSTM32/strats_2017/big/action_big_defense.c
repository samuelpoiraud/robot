#include "action_big.h"
#include "../actions_both_2017.h"
#include "../../propulsion/movement.h"
#include "../../propulsion/astar.h"
#include "../../QS/QS_stateMachineHelper.h"
#include "../../QS/QS_outputlog.h"
#include "../../utils/actionChecker.h"
#include "../../utils/generic_functions.h"

static error_e sub_harry_defence_our_depose_zone();
static error_e sub_harry_defence_middle_depose_zone();

error_e sub_harry_manager_defence() {
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_INUTILE,
			INIT,
			DEFENCE_OUR_DEPOSE_ZONE,
			DEFENCE_MIDDLE_DEPOSE_ZONE,
			ERROR,
			DONE
		);

	static bool_e activate_our_depose_zone = TRUE;
	static bool_e activate_midddle_depose_zone = TRUE;

	switch(state){

		case INIT:
			if(activate_our_depose_zone && !ELEMENTS_get_flag(FLAG_SUB_ANNE_DEPOSE_CYLINDER_OUR_SIDE) ){
				state = DEFENCE_OUR_DEPOSE_ZONE;
			}else if(activate_midddle_depose_zone && !ELEMENTS_get_flag(FLAG_SUB_ANNE_DEPOSE_CYLINDER_CENTER) && !ELEMENTS_get_flag(FLAG_SUB_ANNE_DEPOSE_CYLINDER_OUR_DIAGONAL)){
				state = DEFENCE_MIDDLE_DEPOSE_ZONE;
			}else{
				state = DONE;   // Nothing to defend
			}
			break;

		case DEFENCE_OUR_DEPOSE_ZONE:
			state = check_sub_action_result(sub_harry_defence_our_depose_zone(), state, DONE, ERROR);
			break;

		case DEFENCE_MIDDLE_DEPOSE_ZONE:
			state = check_sub_action_result(sub_harry_defence_middle_depose_zone(), state, DONE, ERROR);
			break;

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
				debug_printf("default case in sub_harry_manager_defence\n");
			break;
	}
	return IN_PROGRESS;
}


static error_e sub_harry_defence_our_depose_zone(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_DEFENCE_OUR_DEPOSE_ZONE,
			INIT,
			GET_IN_DIRECT,
			GET_IN_MIDDLE,
			GET_IN_ASTAR,

			MOVE_NORTH,
			MOVE_SOUTH,
			ERROR_MOVE_SOUTH,

			ERROR,
			DONE
		);

	switch(state){

		case INIT:
			if(ELEMENTS_get_flag(FLAG_SUB_ANNE_DEPOSE_CYLINDER_OUR_SIDE)){ // Anne est déjà en train de défendre à cette position
				state = ERROR;
			}else{
				if(i_am_in_square_color(1200, 1300, 250, 350)){ // Cas où on est sur le point SUD => On va au NORD
					state = MOVE_NORTH;
				}else if(i_am_in_square_color(400, 1400, 0, 600) || i_am_in_square_color(800, 1200, 600, 900)){
					state = GET_IN_DIRECT;
				}else if(i_am_in_square_color(200, 1000, 900, 2100)){
					state = GET_IN_MIDDLE;
				}else{
					state = GET_IN_ASTAR;
				}
				ELEMENTS_set_flag(FLAG_SUB_HARRY_DEPOSE_CYLINDER_OUR_SIDE, TRUE); // On lève le flag de subaction
			}
			break;

		case GET_IN_MIDDLE:
			state = try_going(900, COLOR_Y(900), state , GET_IN_DIRECT, GET_IN_ASTAR, FAST, ANY_WAY, DODGE_AND_WAIT, END_AT_BRAKE);
			break;

		case GET_IN_ASTAR:
			state = ASTAR_try_going(1250, COLOR_Y(300), state, MOVE_NORTH, ERROR, FAST, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case GET_IN_DIRECT:
			state = try_going(1250, COLOR_Y(300), state, MOVE_NORTH, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case MOVE_NORTH:
			// Here we need to move SOUTH to be on a turning point
			state = try_going(600, COLOR_Y(300), state , MOVE_SOUTH, ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case MOVE_SOUTH:
			state = try_going(1250, COLOR_Y(300), state , DONE, ERROR_MOVE_SOUTH, FAST, BACKWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case ERROR_MOVE_SOUTH:
			state = MOVE_NORTH;
			break;

		case ERROR:
			RESET_MAE();
			on_turning_point();
			ELEMENTS_set_flag(FLAG_SUB_HARRY_DEPOSE_CYLINDER_OUR_SIDE, FALSE);
			return NOT_HANDLED;
			break;

		case DONE:
			RESET_MAE();
			on_turning_point();
			ELEMENTS_set_flag(FLAG_SUB_HARRY_DEPOSE_CYLINDER_OUR_SIDE, FALSE);
			return END_OK;
			break;

		default:
			if(entrance)
				debug_printf("default case in sub_harry_defence_our_depose_zone\n");
			break;
	}
	return IN_PROGRESS;
}

static error_e sub_harry_defence_middle_depose_zone(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_DEFENCE_MIDDLE_DEPOSE_ZONE,
			INIT,
			GET_IN,
			MOVE_BACKWARD,
			MOVE_FORWARD,
			RETURN_TO_POS_BACKWARD,
			RETURN_TO_POS_FORWARD,
			TEST_POSITION_MOVE_BACKWARD,
			TEST_POSITION_MOVE_FORWARD,
			START_TIMER_BACKWARD,
			START_TIMER_FORWARD,
			ESCAPE_FROM_BOTTOM,
			ESCAPE_FROM_TOP,
			ERROR,
			DONE
		);


	const displacement_t curve_backward[3] =
			{{(GEOMETRY_point_t){874,COLOR_Y(1206)}, FAST},
			{(GEOMETRY_point_t){1016, COLOR_Y(917)}, FAST},
			{(GEOMETRY_point_t){1263, COLOR_Y(725)}, FAST}};

	const displacement_t curve_forward[3] =
			{{(GEOMETRY_point_t){1016, COLOR_Y(916)}, FAST},
			{(GEOMETRY_point_t){874, COLOR_Y(1206)}, FAST},
			{(GEOMETRY_point_t){850, COLOR_Y(1521)}, FAST}};


	static time32_t tempsDepart;


		switch (state){
		case INIT :
			if(ELEMENTS_get_flag(FLAG_SUB_ANNE_DEPOSE_CYLINDER_CENTER) || ELEMENTS_get_flag(FLAG_SUB_ANNE_DEPOSE_CYLINDER_OUR_DIAGONAL)){
				state = ERROR; // Anne est déjà en train de défendre à cette position
			}else{
				state = GET_IN;
				// Enrichir les GET_IN ici

				// On lève les flags de subaction
				ELEMENTS_set_flag(FLAG_SUB_HARRY_DEPOSE_CYLINDER_CENTER, TRUE);
				ELEMENTS_set_flag(FLAG_SUB_HARRY_DEPOSE_CYLINDER_OUR_DIAGONAL, TRUE);
			}
			break;

		case GET_IN:
			state=try_going(850, COLOR_Y(1521), state, MOVE_BACKWARD, RESET, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

		case MOVE_BACKWARD :
			state=try_going_multipoint(curve_backward, 3, MOVE_BACKWARD, MOVE_FORWARD, START_TIMER_BACKWARD, BACKWARD, NO_DODGE_AND_NO_WAIT, END_AT_BRAKE);
			break;

		case MOVE_FORWARD:
			state=try_going_multipoint(curve_forward, 3, MOVE_FORWARD, MOVE_BACKWARD, START_TIMER_FORWARD, FORWARD, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

		case START_TIMER_BACKWARD :
			tempsDepart = global.match_time;
			state=TEST_POSITION_MOVE_BACKWARD;
			break;

		case TEST_POSITION_MOVE_BACKWARD :
			if (global.match_time >= tempsDepart + 5000){
				if (i_am_in_square_color(761, 1030, 1630, 1390)){
					state=ESCAPE_FROM_TOP;
				}else{
					state=RETURN_TO_POS_BACKWARD;
				}
			}
			break;

		case RETURN_TO_POS_BACKWARD :
			state=try_going(850, COLOR_Y(1521), RETURN_TO_POS_BACKWARD, MOVE_BACKWARD, RETURN_TO_POS_BACKWARD, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

		case START_TIMER_FORWARD :
			tempsDepart = global.match_time;
			state=TEST_POSITION_MOVE_FORWARD;
			break;

		case TEST_POSITION_MOVE_FORWARD :
			if (global.match_time >= tempsDepart + 5000){
				if (i_am_in_square_color(1064, 1460, 792, 594)){
					state=ESCAPE_FROM_BOTTOM;
				}else{
					state=RETURN_TO_POS_FORWARD;
				}
			}
			break;

		case RETURN_TO_POS_FORWARD :
			state=try_going(1263, COLOR_Y(725), RETURN_TO_POS_FORWARD, MOVE_FORWARD, RETURN_TO_POS_FORWARD, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

		case ESCAPE_FROM_BOTTOM :
			state=try_going(1524, COLOR_Y(560), ESCAPE_FROM_BOTTOM, DONE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

		case ESCAPE_FROM_TOP :
			state=try_going(862, COLOR_Y(1862), ESCAPE_FROM_TOP, DONE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

		case ERROR:
			RESET_MAE();
			on_turning_point();
			ELEMENTS_set_flag(FLAG_SUB_HARRY_DEPOSE_CYLINDER_CENTER, FALSE);
			ELEMENTS_set_flag(FLAG_SUB_HARRY_DEPOSE_CYLINDER_OUR_DIAGONAL, FALSE);
			return NOT_HANDLED;
			break;

		case DONE:
			RESET_MAE();
			on_turning_point();
			ELEMENTS_set_flag(FLAG_SUB_HARRY_DEPOSE_CYLINDER_CENTER, FALSE);
			ELEMENTS_set_flag(FLAG_SUB_HARRY_DEPOSE_CYLINDER_OUR_DIAGONAL, FALSE);
			return END_OK;
			break;

		default:
			if(entrance)
				debug_printf("default case in sub_harry_defence_middle_depose_zone\n");
			break;
	}
	return IN_PROGRESS;
}


