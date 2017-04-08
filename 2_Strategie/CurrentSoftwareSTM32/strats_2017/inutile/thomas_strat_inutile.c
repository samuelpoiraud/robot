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
				ACTION,
				ACTION1,
				ACTION2,
				ACTION3,
				ACTION4,
				ACTION5,
				ACTION6,
				ACTION7,
				ERROR,
				DONE
			);

	switch(state){
		case INIT:
			state = check_sub_action_result(sub_push_modules_bretagne(), state, DONE, ERROR);
			/*
			state = try_going(global.pos.x+150, global.pos.y, state, DONE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_BRAKE);
			break;

		case ACTION:
			state = try_going(500, 1450, state, ACTION1, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_BRAKE);
			//state = check_sub_action_result(SELFTESTACT_run(), state, DONE, ERROR);
			break;

		case ACTION1:
			state = try_going(900, 1100, state, ACTION2, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_BRAKE);
			break;

		case ACTION2:
			state = try_going(400, 950, state, ACTION3, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_BRAKE);
			break;

		case ACTION3:
			state = try_going(1100, 1000, state, ACTION4, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_BRAKE);
			break;

		case ACTION4:
			state = try_going(1350, 800, state, ACTION5, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_BRAKE);
			break;

		case ACTION5:
			state = try_going(1350, 290, state, ACTION6, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_BRAKE);
			break;

		case ACTION6:
			state = try_going(600, 100, state, ACTION7, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_BRAKE);
			break;

		case ACTION7:
			state = try_going(400, 950, state, DONE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_BRAKE);
			*/
			break;

		case ERROR:
			break;

		case DONE:
			break;

	}
}

void thomas_strat_inutile_small(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_ANNE_INUTILE,
			INIT,
			ACTION,
			ACTION2,
			ACTION3,
			ERROR,
			DONE
		);

	switch(state){
		case INIT:
			state = ASTAR_try_going(1500, COLOR_Y(2400), state , ACTION, ERROR, FAST, ANY_WAY, NO_AVOIDANCE, END_AT_BRAKE);
			break;

		case ACTION:
			state = ASTAR_try_going(500, COLOR_Y(1500), state , ACTION2, ERROR, FAST, ANY_WAY, NO_AVOIDANCE, END_AT_BRAKE);
			break;

		case ACTION2:
			state = ASTAR_try_going(1500, COLOR_Y(600), state , ACTION3, ERROR, FAST, ANY_WAY, NO_AVOIDANCE, END_AT_BRAKE);
			break;

		case ACTION3:
			state = ASTAR_try_going(500, COLOR_Y(2400), state , DONE, ERROR, FAST, ANY_WAY, NO_AVOIDANCE, END_AT_BRAKE);
			break;

		case ERROR:
			break;

		case DONE:
			break;
	}
}
