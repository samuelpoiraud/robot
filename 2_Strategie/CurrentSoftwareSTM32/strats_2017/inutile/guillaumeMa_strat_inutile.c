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

void guillaumeMa_strat_inutile_big(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_INUTILE,

			INIT,
			ERROR,
			TEST,
			TEST_2,
			TEST_3,
			TEST_4,
			DONE
	);
	switch(state){



		case INIT:
			state = TEST;
			break;// depend du point de depard

		case ERROR:
			state = ERROR;
			break;

		case DONE:
			state = DONE;
			break;

		case TEST:
			state = check_sub_action_result(sub_harry_take_big_crater(OUR_ELEMENT), state, DONE, ERROR);
			break;

		case TEST_2:
			state = check_sub_action_result(sub_harry_take_big_crater(ADV_ELEMENT), state, DONE, ERROR);
			break;

		case TEST_3:
			state = check_sub_action_result(sub_harry_take_big_crater(NO_ELEMENT), state, DONE, ERROR); // error
			break;

		case TEST_4:
			state = check_sub_action_result(sub_harry_take_big_crater(OUR_ELEMENT), state, DONE, ERROR);
			break;
	}
}



/*
void guillaumeMa_strat_inutile_big(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_INUTILE,

}


*/
int NBERROR = 0;


void guillaumeMa_strat_inutile_small(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_ANNE_INUTILE,
			INIT,
			ERROR,
			DONE,
			POINT1,
			POINT2,
			POINT3,
			POINT4,
			POINT5,
			POINT6,
			POINT7,
			POINT8,
			POINT9,
			ERROR1,
			ERROR2,
			ERROR3
		);

	switch(state){ //sub sortir bascule anne
		case INIT:
			state = try_going(180, COLOR_Y(200), state, POINT1, ERROR, SLOW, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case POINT1:
			state = try_going(180, COLOR_Y(700), state, POINT2, ERROR, SLOW, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case POINT2:
			state = try_going(200, COLOR_Y(1150), state, POINT3, ERROR, SLOW, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case POINT3:// ramassage balle fusée dépard
			state = POINT4;
			break;

		case POINT4:
			state = try_going(1000, COLOR_Y(1000), state, POINT5, ERROR1, SLOW, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case POINT5:
			state = try_going(1350, COLOR_Y(100), state, POINT6, ERROR2, SLOW, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case POINT6:
			state = try_going(1350, COLOR_Y(300), state, POINT7, ERROR3, SLOW, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case POINT7:
			state = try_going(1000, COLOR_Y(1000), state, DONE, ERROR, SLOW, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

		case POINT8:
			state = ASTAR_try_going(200, COLOR_Y(1000), state, POINT9, ERROR, SLOW, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case POINT9:
			state = ASTAR_try_going(800, COLOR_Y(200), state, DONE, ERROR, SLOW, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case ERROR:
			break;

		case DONE:
			break;

		case ERROR1:
			if(ERROR <=3){
			state = POINT5;
			NBERROR = NBERROR +1;
			break;
			}else{//pb
			state = POINT2;
			NBERROR = 0;
			break;
			}

		case ERROR2:
			state = POINT4;
			break;

		case ERROR3:
			state = POINT5;
			break;
	}
}


