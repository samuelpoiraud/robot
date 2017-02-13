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

void valentin_strat_inutile_big(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_INUTILE,
			INIT,
			ACTION,
			ERROR,
			DONE
		);

	switch(state){

		case INIT:
			state = try_going(900, COLOR_Y(2500), state , ACTION, DONE, FAST, ANY_WAY, DODGE_AND_WAIT, END_AT_BRAKE);
			break;

		case ACTION:
			state = check_sub_action_result(sub_harry_prise_modules_centre(OUR_ELEMENT, TRUE), state, DONE, ERROR);
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
