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
			ACTION_3,
			ACTION_4,
			ROLLER_ARM_GO_DOWN,
			ROLLER_FOAM_TURN,
			ROLLER_FOAM_STOP,
			TRY_SELFTEST,
			ARM_GO_UP,
			TURN_BALANCER,
			TURN_COLOR,
			GUN_GO_DOWN,
			WAIT_FOR_MAX_POWER,
			TURN_TRIHOLE,
			SEND_ORDERS,
			ERROR,
			DONE
		);

	//sub_act_harry_mae_prepare_modules_for_dispose(MODULE_STOCK_LEFT, FALSE);
	static time32_t local_time = 0;
	static bool_e action_done = FALSE;

	switch(state){

		case INIT:
			/*if(entrance){
				ACT_push_order(ACT_CYLINDER_COLOR_LEFT, ACT_CYLINDER_COLOR_LEFT_NORMAL_SPEED);
			}*/
			state = ACTION;
			//state = ROLLER_ARM_GO_DOWN;
			//state = TRY_SELFTEST;
			//state = TURN_COLOR;
			//state = GUN_GO_DOWN;
			//state = try_going(1250, COLOR_Y(300), state, PATHFIND, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			//state = try_going(1250, COLOR_Y(350), state, PATHFIND, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			//state = SEND_ORDERS;
			break;

		case PATHFIND:
			state = ASTAR_try_going(1400, COLOR_Y(2500), state, DONE,  ERROR, FAST, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case ACTION:
			/*if(entrance){
				PROP_set_position(1350, 400, -PI4096/2);
				ACT_push_order(ACT_CYLINDER_BALANCER_LEFT, ACT_CYLINDER_BALANCER_LEFT_IN);
				ACT_push_order(ACT_CYLINDER_BALANCER_RIGHT, ACT_CYLINDER_BALANCER_RIGHT_IN);
				ACT_push_order(ACT_CYLINDER_ARM_LEFT, ACT_CYLINDER_ARM_LEFT_PREPARE_TO_TAKE);
				ACT_push_order(ACT_CYLINDER_ARM_RIGHT, ACT_CYLINDER_ARM_RIGHT_PREPARE_TO_TAKE);
				ACT_push_order(ACT_CYLINDER_DISPOSE_LEFT, ACT_CYLINDER_DISPOSE_LEFT_TAKE);
				ACT_push_order(ACT_CYLINDER_DISPOSE_RIGHT, ACT_CYLINDER_DISPOSE_RIGHT_TAKE);
				//STOCKS_setModuleType(STOCK_POS_BALANCER, MODULE_STOCK_LEFT, MODULE_POLY);
				//sub_act_harry_mae_prepare_modules_for_dispose(MODULE_STOCK_LEFT, TRUE);
				//sub_act_harry_mae_store_modules(MODULE_STOCK_LEFT, TRUE);
			}*/
			//state = check_sub_action_result(sub_act_harry_take_rocket_parallel_down_to_top(MODULE_ROCKET_MONO_OUR_SIDE, RIGHT, LEFT, RIGHT, LEFT), state, DONE, ERROR);
			//state = check_sub_action_result(sub_act_harry_mae_prepare_modules_for_dispose(MODULE_STOCK_RIGHT, FALSE), state, DONE, ERROR);
			//state = check_sub_action_result(sub_act_harry_mae_dispose_modules(MODULE_STOCK_RIGHT, ARG_DISPOSE_ONE_CYLINDER_FOLLOW_BY_ANOTHER), state, DONE, ERROR);
			//state = check_sub_action_result(sub_harry_prise_module_unicolor_south(LEFT), state, PATHFIND, ERROR);
			//state = check_sub_action_result(SELFTESTACT_run(), state, ACTION_2, ERROR);
			state = check_sub_action_result(sub_harry_depose_modules_side(RIGHT, OUR_SIDE), state, DONE, ERROR);
			break;

		case ACTION_2:
			//state = check_sub_action_result(SELFTESTACT_run(), state, DONE, ERROR);
			//state = check_sub_action_result(sub_act_harry_take_rocket_parallel_down_to_top(MODULE_ROCKET_MULTI_OUR_SIDE, RIGHT, LEFT, RIGHT, LEFT), state, DONE, ERROR);
			//state = check_sub_action_result(sub_act_harry_mae_prepare_modules_for_dispose(MODULE_STOCK_RIGHT, FALSE), state, DONE, ERROR);
			//state = check_sub_action_result(sub_act_harry_mae_dispose_modules(MODULE_STOCK_RIGHT, ARG_DISPOSE_ONE_CYLINDER_AND_FINISH), state, DONE, ERROR);
			break;

		case ACTION_3:
			state = check_sub_action_result(sub_harry_prise_modules_initiale(), state, ACTION_4, ERROR);
			break;

		case ACTION_4:
			state = check_sub_action_result(sub_harry_prise_module_base_centre(OUR_ELEMENT, RIGHT), state, DONE, ERROR);
			break;

		case ROLLER_ARM_GO_DOWN:
			if(entrance){
				ACT_push_order(ACT_ORE_ROLLER_ARM, ACT_ORE_ROLLER_ARM_OUT);
			}
			state = check_act_status(ACT_QUEUE_Ore_roller_arm, state, ROLLER_FOAM_TURN, ERROR);
			break;

		case ROLLER_FOAM_TURN:
			if(entrance){
				ACT_push_order(ACT_ORE_WALL, ACT_ORE_WALL_OUT);
				ACT_push_order_with_param(ACT_ORE_ROLLER_FOAM, ACT_ORE_ROLLER_FOAM_RUN, 300);
			}
			state = check_act_status(ACT_QUEUE_Ore_wall, state, DONE, DONE);

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
				ACT_push_order_with_param(ACT_ORE_TRIHOLE, ACT_ORE_TRIHOLE_RUN, 20);
			}
			state = check_act_status(ACT_QUEUE_Ore_gun, state, WAIT_FOR_MAX_POWER, ERROR);
			break;

		case WAIT_FOR_MAX_POWER:
			state = wait_time(3000, state, TURN_TRIHOLE);
			break;

		case TURN_TRIHOLE:
			if(entrance){
				ACT_push_order_with_param(ACT_ORE_TRIHOLE, ACT_ORE_TRIHOLE_RUN, 20);
			}
			state = DONE;
			break;

		case SEND_ORDERS:
			/*if(entrance){
				ACT_push_order_now(ACT_CYLINDER_SLIDER_RIGHT, ACT_CYLINDER_SLIDER_RIGHT_OUT);
			}

			state = check_act_status(ACT_QUEUE_Cylinder_slider_right, state, DONE, ERROR);
			*/
			if(entrance){
				ACT_push_order(ACT_CYLINDER_SLIDER_RIGHT, ACT_CYLINDER_SLIDER_RIGHT_OUT);
				ACT_push_order(ACT_CYLINDER_SLIDER_RIGHT, ACT_CYLINDER_SLIDER_RIGHT_IN);
				ACT_push_order(ACT_CYLINDER_SLIDER_RIGHT, ACT_CYLINDER_SLIDER_RIGHT_OUT);
				ACT_push_order(ACT_CYLINDER_SLIDER_RIGHT, ACT_CYLINDER_SLIDER_RIGHT_IN);
				ACT_push_order(ACT_CYLINDER_SLIDER_RIGHT, ACT_CYLINDER_SLIDER_RIGHT_OUT);
				ACT_push_order(ACT_CYLINDER_SLIDER_RIGHT, ACT_CYLINDER_SLIDER_RIGHT_IN);
				local_time= global.absolute_time;
			}

			if(global.absolute_time > local_time + 100 && !action_done){
				ACT_push_order_now(ACT_CYLINDER_SLIDER_RIGHT, ACT_CYLINDER_SLIDER_RIGHT_IN);
				action_done = TRUE;
			}
			if(action_done){
				state = check_act_status(ACT_QUEUE_Cylinder_slider_right, state, DONE, ERROR);
			}
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
			ACTION,
			TAKE_CRATER,
			ERROR,
			DONE
		);

	switch(state){
		case INIT:
			if(entrance){
				ACT_push_order(ACT_SMALL_CYLINDER_ARM, ACT_SMALL_CYLINDER_ARM_OUT);
			}
			state = TAKE_CRATER;
			//state = ACTION;
			//state = try_going(global.pos.x + 800, global.pos.y, state, TAKE_CRATER, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

		case ACTION:
			state = check_sub_action_result(sub_act_anne_return_module(), state, DONE, DONE);
			break;

		case TAKE_CRATER:
			state = check_sub_action_result(sub_act_anne_take_rocket_down_to_top(MODULE_ROCKET_MONO_OUR_SIDE, TRUE, TRUE, TRUE, TRUE), state, DONE, ERROR);
			break;

		case ERROR:
			break;

		case DONE:
			break;
	}
}
