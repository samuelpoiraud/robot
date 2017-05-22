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
#include "../../QS/QS_CANmsgList.h"


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
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_ANNE_INUTILE,
				INIT,
				GO_POSITION,
				ARM_OUT,
				GET_IN_DIRECT,
				GO_TO_START_POINT,
				ACTION,
				PRISE,
				FLEEEE,
				DEPOSE,
				DEPOSE_2,
				ERROR,
				DONE
			);

	switch(state){
		case INIT:
			state = try_going(global.pos.x+200, global.pos.y, state, GO_POSITION, ERROR, FAST, ANY_WAY, DODGE_AND_WAIT, END_AT_BRAKE);
			break;

		case GO_POSITION:
			state = try_going(800, COLOR_Y(1000), state, PRISE, ERROR, FAST, ANY_WAY, DODGE_AND_WAIT, END_AT_BRAKE);
			break;

		case ARM_OUT:
			if (entrance){
				ACT_push_order(ACT_SMALL_CYLINDER_ARM, ACT_SMALL_CYLINDER_ARM_PREPARE_TO_TAKE);
			}
			state = check_act_status(ACT_QUEUE_Small_cylinder_arm, state, GET_IN_DIRECT, GET_IN_DIRECT);
			break;

		case GET_IN_DIRECT:
			state = try_going(400, COLOR_Y(1150), state, GO_TO_START_POINT, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case GO_TO_START_POINT: //ajuster la distance fusée
			if(global.color == BLUE){
				state = try_going(265, COLOR_Y(1145), state, ACTION, ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			}else{
				state = try_going(265, COLOR_Y(1155), state, ACTION, ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			}
			break;

		case ACTION:
			state = check_sub_action_result(sub_act_anne_take_rocket_down_to_top( MODULE_ROCKET_MULTI_OUR_SIDE, TRUE, TRUE, TRUE, TRUE)
, state, FLEEEE, ERROR);
			break;

		case PRISE:
			state = check_sub_action_result(sub_anne_fusee_color(), state, FLEEEE, ERROR);
			break;

		case FLEEEE:
			state = try_going(600, COLOR_Y(1150), state, DEPOSE, DEPOSE, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case DEPOSE:
			state = check_sub_action_result(sub_act_anne_mae_dispose_modules(ARG_DISPOSE_ONE_CYLINDER_FOLLOW_BY_ANOTHER)
, state, DEPOSE_2, ERROR);
			break;

		case DEPOSE_2:
			state = check_sub_action_result(sub_act_anne_mae_dispose_modules(ARG_DISPOSE_ONE_CYLINDER_AND_FINISH)
, state, DONE, ERROR);
			break;

		case ERROR:
			break;

		case DONE:
			break;
	}
}

