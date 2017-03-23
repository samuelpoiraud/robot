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



		case INIT: //+150x +0y
			state = try_going(BIG_CALIBRATION_FORWARD_BORDER_DISTANCE+150, 1100-(BIG_ROBOT_WIDTH/2), state, TEST, ERROR, SLOW, BACKWARD, NO_DODGE_AND_NO_WAIT, END_AT_BRAKE);
			//state = try_go_angle(0, state, TEST, ERROR, FAST, CLOCKWISE, END_AT_LAST_POINT);
			//state = try_going(800, 800, state, TEST, ERROR, SLOW, BACKWARD, NO_DODGE_AND_NO_WAIT, END_AT_BRAKE);
			break;// depend du point de depard

		case ERROR:
			state = ERROR;
			break;

		case DONE:
			state = DONE;
			break;

		case TEST: //clocksise (y)
			state = try_go_angle(PI4096*9/8, state, TEST_2, ERROR, SLOW, CLOCKWISE, END_AT_LAST_POINT);


//#define BIG_ROBOT_WIDTH									292  //310		//Largeur du gros robot [mm]
//#define BIG_CALIBRATION_BACKWARD_BORDER_DISTANCE			96		//Distance entre le 'centre' du robot et l'arrière en calage [mm]
//#define BIG_CALIBRATION_FORWARD_BORDER_DISTANCE			146		//Distance entre le 'centre' du robot et l'avant en calage [mm]

			//state = check_sub_action_result(sub_harry_take_north_little_crater(OUR_ELEMENT), state, TEST_2, ERROR);
			break;

		case TEST_2:
			state = try_going(496, 1054, state, TEST_3, ERROR, SLOW, BACKWARD, NO_DODGE_AND_NO_WAIT, END_AT_BRAKE);
			//state = check_sub_action_result(sub_harry_take_north_little_crater(ADV_ELEMENT), state, TEST_3, ERROR);
			break;

		case TEST_3:
			state = try_go_angle(PI4096*19/16, state, TEST_4, ERROR, SLOW, TRIGOWISE, END_AT_LAST_POINT);
			//state = check_sub_action_result(sub_harry_take_big_crater(ADV_ELEMENT), state, TEST_4, ERROR);
			break;

		case TEST_4:
			state = try_going(BIG_CALIBRATION_FORWARD_BORDER_DISTANCE+20, 1020-(BIG_ROBOT_WIDTH/2), state, DONE, ERROR, SLOW, FORWARD, NO_DODGE_AND_NO_WAIT, END_AT_BRAKE);
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


