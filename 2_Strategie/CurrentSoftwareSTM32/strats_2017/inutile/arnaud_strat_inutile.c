#include "strat_inutile.h"
#include "../../propulsion/movement.h"
#include "../../propulsion/astar.h"
#include "../../actuator/act_functions.h"
#include "../../actuator/queue.h"
#include "../../QS/QS_stateMachineHelper.h"
#include "../../utils/generic_functions.h"
#include "../../utils/actionChecker.h"
#include "../../elements.h"
#include "../../avoidance.h"
#include "../../strats_2017/big/action_big.h"
#include "../../strats_2017/small/action_small.h"
#include "../../strats_2017/actions_both_2017.h"

void arnaud_strat_inutile_big(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_INUTILE,
			INIT,
			TRANS1,
			TRANS2,
			ROT1,
			ROT2,
			ROT3,
			ERROR,
			DONE
		);

	switch(state){
		case INIT:
			state = TRANS1;
			break;

		case TRANS1:
			if(entrance){
				AVOIDANCE_activeSmallAvoidance(TRUE);
			}
			state = try_going(global.pos.x, global.pos.y+1000, state , TRANS2, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			if(ON_LEAVE()){
				AVOIDANCE_activeSmallAvoidance(FALSE);
			}
			break;

		case TRANS2:
			state = try_going(1500, 2500, state , ROT1, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_BRAKE);
			break;

		case ROT1:
			state = try_go_angle(0, state, ROT2, ERROR, FAST, ANY_WAY, END_AT_LAST_POINT);
			break;

		case ROT2:
			state = try_go_angle(PI4096/2, state, ROT3, ERROR, FAST, CLOCKWISE, END_AT_LAST_POINT);
			break;

		case ROT3:
			state = try_go_angle(PI4096, state, DONE, ERROR, FAST, ANY_WAY, END_AT_BRAKE);
			break;

		case ERROR:
			break;

		case DONE:
			break;
	}
}

void arnaud_strat_inutile_small(){

}
