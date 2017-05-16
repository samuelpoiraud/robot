#include "strat_inutile.h"
#include "../../propulsion/movement.h"
#include "../../propulsion/astar.h"
#include "../../actuator/act_functions.h"
#include "../../actuator/queue.h"
#include "../../QS/QS_stateMachineHelper.h"
#include "../../utils/generic_functions.h"
#include "../../utils/actionChecker.h"
#include "../../elements.h"
#include "../../strats_2017/big/action_big.h"
#include "../../strats_2017/small/action_small.h"
#include "../../strats_2017/actions_both_2017.h"


void thomas_strat_inutile_big(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_INUTILE,
				INIT,
				GO_POSITION,
				ACTION,
				ERROR,
				DONE
			);

	switch(state){
		case INIT:
			state = try_going(global.pos.x+200, global.pos.y, state, GO_POSITION, ERROR, FAST, ANY_WAY, DODGE_AND_WAIT, END_AT_BRAKE);
			break;

		case GO_POSITION:
			state = try_going(800, COLOR_Y(1000), state, ACTION, ERROR, FAST, ANY_WAY, DODGE_AND_WAIT, END_AT_BRAKE);
			break;
		case ACTION:
			state=check_sub_action_result(sub_anne_fusee_color(OUR_ELEMENT),state, DONE, ERROR);
			break;
		case ERROR:
			RESET_MAE();
			//return NOT_HANDLED;
			break;

		case DONE:
			RESET_MAE();
			//return END_OK;
			break;

	}

	//return IN_PROGRESS;
}

void thomas_strat_inutile_small(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_INUTILE,
				INIT,
				GO_POSITION,
				GET_IN_DIRECT,
				GO_TO_START_POINT,
				ACTION,
				ERROR,
				DONE
			);

	switch(state){
		case INIT:
			state = try_going(global.pos.x+200, global.pos.y, state, GO_POSITION, ERROR, FAST, ANY_WAY, DODGE_AND_WAIT, END_AT_BRAKE);
			break;

		case GO_POSITION:
			state = try_going(800, COLOR_Y(1000), state, GET_IN_DIRECT, ERROR, FAST, ANY_WAY, DODGE_AND_WAIT, END_AT_BRAKE);
			break;

		case GET_IN_DIRECT:
			state = try_going(400, COLOR_Y(1150), state, GO_TO_START_POINT, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case GO_TO_START_POINT: //ajuster la distance fusée
			state = try_going(265, COLOR_Y(1150), state, ACTION, ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case ACTION:
			state = check_sub_action_result(sub_act_anne_take_rocket_down_to_top( MODULE_ROCKET_MULTI_OUR_SIDE, TRUE, TRUE, TRUE, TRUE)
, state, DONE, ERROR);
			break;

		case ERROR:
			break;

		case DONE:
			break;
	}
}

