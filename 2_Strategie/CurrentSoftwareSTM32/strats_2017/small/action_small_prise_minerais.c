#include "action_small.h"
#include "../../propulsion/movement.h"
#include "../../propulsion/astar.h"
#include "../../QS/QS_stateMachineHelper.h"
#include "../../QS/QS_outputlog.h"
#include "../../utils/actionChecker.h"
#include "../../utils/generic_functions.h"
#include "../../actuator/act_functions.h"
#include "../../avoidance.h"


error_e sub_anne_prise_gros_cratere(ELEMENTS_property_e mimerais){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_ANNE_PRISE_GROS_CRATERE,
			INIT,
			ERROR,
			DONE
		);

	switch(state){
		case INIT:
			state = DONE;
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
				debug_printf("default case in sub_anne_prise_gros_cratere\n");
			break;
	}

	return IN_PROGRESS;
}


error_e sub_anne_prise_petit_cratere(ELEMENTS_property_e minerais){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_ANNE_PRISE_PETIT_CRATERE,
				INIT,
				GET_IN,
				FROM_ADV_SQUARE,
				FROM_MIDDLE_SQUARE,
				FROM_OUR_SQUARE,
				ASTAR,
				POS_TO_TAKE,
				TAKE_BALL,
				GO_TO_START_ZONE,
				PUT_BALL,
				GET_OUT,
				ERROR,
				DONE
			);

		const displacement_t way_to_start_zone[3] = {(displacement_t){(GEOMETRY_point_t){800, COLOR_Y(650)}, FAST},
								 (displacement_t){(GEOMETRY_point_t){650, COLOR_Y(1000)}, FAST},
								 (displacement_t){(GEOMETRY_point_t){350, COLOR_Y(950)}, FAST},
								 };

		switch(state){
			case INIT:
				if(i_am_in_square_color(800, 1400, 300, 900)){
					state = FROM_ADV_SQUARE;
				}else if(i_am_in_square_color(800, 1400, 2700, 2100)){
					state = FROM_OUR_SQUARE;
				}else if(i_am_in_square_color(200, 1100, 900, 2100)){
					state = FROM_MIDDLE_SQUARE;
				}else{
					state = ASTAR;
				}
				break;

			case FROM_ADV_SQUARE:
				state = try_going(1000, COLOR_Y(2100), state, FROM_MIDDLE_SQUARE, ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
				break;

			case FROM_MIDDLE_SQUARE:
				state = try_going(1000, COLOR_Y(900), state, FROM_OUR_SQUARE, ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
				break;

			case FROM_OUR_SQUARE:
				state = try_going(800, COLOR_Y(650), state, POS_TO_TAKE, ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
				break;

			case ASTAR:
				state = try_going(800, COLOR_Y(650), state, POS_TO_TAKE, ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
				break;

			case POS_TO_TAKE:
				state = try_going(600, COLOR_Y(650), state, TAKE_BALL, ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
				break;

			case TAKE_BALL:
				if(entrance){
					ACT_push_order(ACT_SMALL_ORE, ACT_SMALL_ORE_DOWN);
				}
				state= check_act_status(ACT_QUEUE_Small_ore, state, GO_TO_START_ZONE, ERROR);
				break;

			case GO_TO_START_ZONE:
				state = try_going_multipoint(way_to_start_zone, state, PUT_BALL, ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
				break;

			case PUT_BALL:
				if(entrance){
					ACT_push_order(ACT_SMALL_ORE, ACT_SMALL_ORE_UP);
				}
				state= check_act_status(ACT_QUEUE_Small_ore, state, GET_OUT, ERROR);
				break;

			case GET_OUT:
				state = try_going(650, 1000, state, DONE, ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
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
					debug_printf("default case in sub_anne_prise_petit_cratere\n");
				break;
		}

		return IN_PROGRESS;
}
