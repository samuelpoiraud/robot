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
				ERROR,
				DONE
			);

	switch(state){
		case INIT:
			state = ACTION;
			break;
		case ACTION:
			state=check_sub_action_result(sub_harry_rocket_multicolor(OUR_ELEMENT),state, DONE, ERROR);
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
