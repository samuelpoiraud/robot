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
			GO_POINT_1,
			GO_POINT_2,
			ERROR,
			DONE
		);

	sub_act_harry_mae_prepare_modules_for_dispose(MODULE_STOCK_RIGHT, FALSE);
	static time32_t local_time = 0;
	static bool_e trigger_is_ok = FALSE;

	switch(state){

		case INIT:
			state = ACTION;
			//state = check_sub_action_result(strat_test_avoidance(), state, DONE, ERROR);
			break;

		case ACTION:
			if(entrance){
				local_time = global.absolute_time + 3000;
			}
			if(global.absolute_time > local_time){
				sub_act_harry_mae_prepare_modules_for_dispose(MODULE_STOCK_RIGHT, TRUE);
			}

			//state = check_sub_action_result(sub_act_harry_take_rocket_down_to_top(MODULE_ROCKET_MONO_OUR_SIDE, RIGHT, LEFT, RIGHT, LEFT), state, DONE, ERROR);
			//state = check_sub_action_result(sub_act_harry_mae_prepare_modules_for_dispose(MODULE_STOCK_RIGHT, FALSE), state, DONE, ERROR);
			state = check_sub_action_result(sub_act_harry_mae_dispose_modules(MODULE_STOCK_RIGHT, ARG_DISPOSE_ONE_CYLINDER_FOLLOW_BY_ANOTHER), state, DONE, ERROR);
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
