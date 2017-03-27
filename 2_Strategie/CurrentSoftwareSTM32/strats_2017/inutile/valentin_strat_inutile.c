#include "strat_inutile.h"
#include "../../propulsion/movement.h"
#include "../../propulsion/astar.h"
#include "../../actuator/act_functions.h"
#include "../../actuator/queue.h"
#include "../../QS/QS_stateMachineHelper.h"
#include "../../QS/QS_outputlog.h"
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
			state = ACTION;
			break;

		case ACTION:
			state = check_sub_action_result(sub_act_harry_take_rocket_down_to_top(MODULE_ROCKET_MONO_OUR_SIDE, RIGHT, LEFT, NO_SIDE, NO_SIDE), state, DONE, ERROR);
			break;

		case ERROR:
			if(entrance){
				debug_printf("ERROR\n");
			}
			break;

		case DONE:
			if(entrance){
				debug_printf("DONE\n");
			}
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
