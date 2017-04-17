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
#include "../../Supervision/SelftestActionneur.h"

void valentin_strat_inutile_big(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_INUTILE,
			INIT,
			PATHFIND,
			ACTION,
			ACTION_2,
			ROLLER_ARM_GO_DOWN,
			ROLLER_FOAM_TURN,
			ROLLER_FOAM_STOP,
			TRY_SELFTEST,
			ARM_GO_UP,
			TURN_BALANCER,
			TURN_COLOR,
			GUN_GO_DOWN,
			ERROR,
			DONE
		);

	//sub_act_harry_mae_prepare_modules_for_dispose(MODULE_STOCK_LEFT, FALSE);
	//static time32_t local_time = 0;

	switch(state){

		case INIT:
			//state = ACTION;
			//state = ROLLER_ARM_GO_DOWN;
			//state = TRY_SELFTEST;
			//state = TURN_COLOR;
			state = GUN_GO_DOWN;
			//state = try_going(602, 2698, state, PATHFIND, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

		case PATHFIND:
			state = ASTAR_try_going(1390, 400, state, DONE,  ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

		case ACTION:
			//state = check_sub_action_result(sub_act_harry_take_rocket_down_to_top(MODULE_ROCKET_MONO_OUR_SIDE, RIGHT, LEFT, RIGHT, LEFT), state, ACTION_2, ERROR);
			//state = check_sub_action_result(sub_act_harry_mae_prepare_modules_for_dispose(MODULE_STOCK_RIGHT, FALSE), state, DONE, ERROR);
			state = check_sub_action_result(sub_act_harry_mae_dispose_modules(MODULE_STOCK_LEFT, ARG_DISPOSE_ONE_CYLINDER_FOLLOW_BY_ANOTHER), state, ACTION_2, ERROR);
			break;

		case ACTION_2:
			//state = check_sub_action_result(sub_act_harry_take_rocket_down_to_top(MODULE_ROCKET_MULTI_OUR_SIDE, RIGHT, LEFT, RIGHT, LEFT), state, DONE, ERROR);
			//state = check_sub_action_result(sub_act_harry_mae_prepare_modules_for_dispose(MODULE_STOCK_RIGHT, FALSE), state, DONE, ERROR);
			state = check_sub_action_result(sub_act_harry_mae_dispose_modules(MODULE_STOCK_LEFT, ARG_DISPOSE_ONE_CYLINDER_AND_FINISH), state, DONE, ERROR);
			break;

		case ROLLER_ARM_GO_DOWN:
			if(entrance){
				ACT_push_order(ACT_ORE_ROLLER_ARM, ACT_ORE_ROLLER_ARM_OUT);
			}
			state = check_act_status(ACT_QUEUE_Ore_roller_arm, state, ROLLER_FOAM_TURN, ERROR);
			break;

		case ROLLER_FOAM_TURN:
			if(entrance){
				ACT_push_order_with_param(ACT_ORE_ROLLER_FOAM, ACT_ORE_ROLLER_FOAM_RUN, 50);
			}
			state = check_act_status(ACT_QUEUE_Ore_roller_foam, state, ROLLER_FOAM_STOP, ROLLER_FOAM_STOP);

			//ACT_push_order_with_param(ACT_ORE_TRIHOLE, ACT_ORE_TRIHOLE_RUN, 100);
			break;

		case ROLLER_FOAM_STOP:
			if(entrance){
				ACT_push_order_with_param(ACT_ORE_ROLLER_FOAM, ACT_ORE_ROLLER_FOAM_STOP, 0);
			}
			state = check_act_status(ACT_QUEUE_Ore_roller_foam, state, DONE, ERROR);

			//ACT_push_order_with_param(ACT_ORE_TRIHOLE, ACT_ORE_TRIHOLE_RUN, 100);
			break;

		case TRY_SELFTEST:
			state = check_sub_action_result(SELFTESTACT_run(), state, DONE , ERROR);
			break;

		case ARM_GO_UP:
			if(entrance){
				ACT_push_order(ACT_CYLINDER_ARM_RIGHT, ACT_CYLINDER_ARM_RIGHT_TAKE);
			}
			state = check_act_status(ACT_QUEUE_Cylinder_arm_right, state, TURN_BALANCER, ERROR);
			break;

		case TURN_BALANCER:
			if(entrance){
				ACT_push_order(ACT_CYLINDER_BALANCER_RIGHT, ACT_CYLINDER_BALANCER_RIGHT_OUT);
			}
			state = check_act_status(ACT_QUEUE_Cylinder_balancer_right, state, DONE, ERROR);
			break;

		case TURN_COLOR:
			if(entrance){
				ACT_push_order(ACT_CYLINDER_COLOR_RIGHT, ACT_CYLINDER_COLOR_RIGHT_NORMAL_SPEED);
			}
			state = check_act_status(ACT_QUEUE_Cylinder_color_right, state, DONE, ERROR);
			break;


		// Test Gun
		case GUN_GO_DOWN:
			if(entrance){
				ACT_push_order(ACT_ORE_GUN, ACT_ORE_GUN_DOWN);
				ACT_push_order(ACT_TURBINE, ACT_TURBINE_NORMAL);
				ACT_push_order(ACT_ORE_TRIHOLE, ACT_ORE_TRIHOLE_RUN, 20);
			}
			state = check_act_status(ACT_QUEUE_Ore_gun, state, DONE, ERROR);
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
