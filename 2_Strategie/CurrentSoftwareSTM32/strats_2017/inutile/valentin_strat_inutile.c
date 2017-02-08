#include "strat_inutile.h"
#include "../../propulsion/movement.h"
#include "../../propulsion/astar.h"
#include "../../QS/QS_stateMachineHelper.h"
#include "../../utils/generic_functions.h"

void valentin_strat_inutile_big(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_INUTILE,
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
			if(i_am_in_square_color(1200, 1300, 250, 350)){ // Cas où on est sur le point SUD => On va au NORD
				state = MOVE_NORTH;
			}else if(i_am_in_square_color(400, 1400, 0, 600) || i_am_in_square_color(800, 1200, 600, 900)){
				state = GET_IN_DIRECT;
			}else if(i_am_in_square_color(200, 1000, 900, 2100)){
				state = GET_IN_MIDDLE;
			}else{
				state = GET_IN_ASTAR;
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
			break;

		case DONE:
			break;
	}
}

void valentin_strat_inutile_small(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_ANNE_INUTILE,
			INIT,
			ERROR,
			DONE
		);

	switch(state){
		case INIT:
			break;

		case ERROR:
			break;

		case DONE:
			break;
	}
}
