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
#include "../../propulsion/prop_functions.h"


void corentin_strat_inutile_big(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_INUTILE,
			INIT,
			GO_TO,
			MAKE_SUB,
			START,
			FIRST_STRAIGHT,
			FIRST_ROT,
			SECOND_STRAIGHT,
			SECOND_ROT,
			THIRD_STRAIGHT,
			GIVE_DATA,
			AVANCE,
			GO_TO_POINT,
			ROTATE,
			SECOND_ROTATE,
			COEF_CENTRIFUGAL,
			FIRST_ROTATION,
			MIDDLE_ROTATION,
			LAST_ROTATION,
			FINAL_BRAKE,
			ERROR,
			DONE
		);

	switch(state){
		case INIT:
			state=START;
		//	PROP_set_acceleration(200);
			break;

		case GO_TO:
			state=try_going(1000, COLOR_Y(600), state, MAKE_SUB, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case MAKE_SUB:
			state=check_sub_action_result(sub_harry_take_big_crater(ADV_ELEMENT),state,DONE,state);
			break;

		case START:
			state=try_advance(NULL, entrance,600,state, FIRST_STRAIGHT, FIRST_STRAIGHT,FAST, FORWARD, NO_DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
			break;

		case FIRST_STRAIGHT:
			state=try_going(1270,COLOR_Y(350),state,FIRST_ROT,FIRST_ROT,FAST,ANY_WAY,NO_DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
			break;

		case FIRST_ROT:
			state=try_go_angle(0,state,SECOND_STRAIGHT,SECOND_STRAIGHT,FAST,CLOCKWISE,END_AT_LAST_POINT);
			break;

		case SECOND_STRAIGHT:
			state=try_going(900,COLOR_Y(400),state,SECOND_ROT,SECOND_ROT,FAST,BACKWARD,NO_DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
			break;

		case SECOND_ROT:
			state=try_go_angle(PI4096,state,THIRD_STRAIGHT,THIRD_STRAIGHT,FAST,ANY_WAY,END_AT_LAST_POINT);
			break;

		case THIRD_STRAIGHT:
			state=try_advance(NULL, entrance,300,state, GIVE_DATA, GIVE_DATA,FAST, BACKWARD, NO_DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
			break;

		case GIVE_DATA:{
			CAN_msg_t msg;
			msg.sid=PROP_DATALASER;
			msg.size=0;
			CAN_send(&msg);
			state=DONE;
		}
			break;

		case AVANCE:
			state = try_advance(NULL, entrance, 600, state, GO_TO_POINT, ERROR, FAST, BACKWARD, NO_AVOIDANCE, END_AT_BRAKE);
			break;

		case GO_TO_POINT:
			state = try_going(1400, COLOR_Y(600), state, ROTATE, ERROR, FAST, BACKWARD, NO_AVOIDANCE, END_AT_LAST_POINT);
			break;

		case ROTATE:
			state = try_go_angle(-PI4096, state, SECOND_ROTATE, ERROR, FAST, ANY_WAY, END_AT_LAST_POINT);
			if(ON_LEAVE()){
				CAN_msg_t msg;
				msg.sid = PROP_ASK_CORNER_SCAN;
				msg.size = SIZE_PROP_ASK_CORNER_SCAN;
				msg.data.strat_ask_corner_scan.color = global.color;
				msg.data.strat_ask_corner_scan.isRightSensor = FALSE;
				msg.data.strat_ask_corner_scan.startScan = TRUE;
				CAN_send(&msg);
			}
			break;

		case SECOND_ROTATE:
			state = try_go_angle(PI4096/2, state, DONE, ERROR, SLOW, ANY_WAY, END_AT_LAST_POINT);
			if(ON_LEAVE()){
				CAN_msg_t msg;
				msg.sid = PROP_ASK_CORNER_SCAN;
				msg.size = SIZE_PROP_ASK_CORNER_SCAN;
				msg.data.strat_ask_corner_scan.color = global.color;
				msg.data.strat_ask_corner_scan.isRightSensor = FALSE;
				msg.data.strat_ask_corner_scan.startScan = FALSE;
				CAN_send(&msg);
			}
			break;

		case COEF_CENTRIFUGAL:
			state = try_advance(NULL, entrance, 700, state, FIRST_ROTATION, ERROR, 100, FORWARD, NO_AVOIDANCE, END_AT_BRAKE);
			break;

		case FIRST_ROTATION:
			state = try_going(1146, 1069, state, MIDDLE_ROTATION, ERROR, 100, FORWARD, NO_AVOIDANCE, END_AT_BRAKE);
			break;

		case MIDDLE_ROTATION:
			state = try_going(1146, 1369, state, LAST_ROTATION, ERROR, 100, FORWARD, NO_AVOIDANCE, END_AT_BRAKE);
			break;

		case LAST_ROTATION:
			state = try_going(886, 1519, state, FINAL_BRAKE, ERROR, 100, FORWARD, NO_AVOIDANCE, END_AT_BRAKE);
			break;

		case FINAL_BRAKE:
			state = try_going(386, 1519, state, DONE, ERROR, 100, FORWARD, NO_AVOIDANCE, END_AT_LAST_POINT);
			break;
		case ERROR:
			break;

		case DONE:
			break;
	}
}

void corentin_strat_inutile_small(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_ANNE_INUTILE,
			INIT,
			DEPOSE,
			ERROR,
			DONE
		);

	switch(state){
		case INIT:
			state = check_sub_action_result(sub_cross_rocker(), state, DEPOSE, ERROR);
			break;

		case DEPOSE:
			state = check_sub_action_result(sub_anne_depose_modules_centre(MODULE_MONO_DOMINATING, MODULE_STOCK_SMALL, OUR_SIDE), state, DONE, ERROR);
			break;

		case ERROR:
			break;

		case DONE:
			break;
	}
}
