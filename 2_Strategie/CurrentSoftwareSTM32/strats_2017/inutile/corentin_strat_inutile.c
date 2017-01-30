#include "strat_inutile.h"
#include "../../propulsion/movement.h"
#include "../../QS/QS_stateMachineHelper.h"
#include "../../QS/QS_can.h"


void corentin_strat_inutile_big(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_INUTILE,
			INIT,
			START,
			FIRST_STRAIGHT,
			FIRST_ROT,
			SECOND_STRAIGHT,
			SECOND_ROT,
			THIRD_STRAIGHT,
			GIVE_DATA,
			ERROR,
			DONE
		);

	switch(state){
		case INIT:
			state=START;
			break;

		case START:
			state=try_advance(NULL, entrance,600,state, FIRST_STRAIGHT, FIRST_STRAIGHT,FAST, BACKWARD, NO_DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
			break;

		case FIRST_STRAIGHT:
			state=try_going(1270,COLOR_Y(300),state,FIRST_ROT,FIRST_ROT,FAST,ANY_WAY,NO_DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
			break;

		case FIRST_ROT:
			state=try_go_angle(0,state,SECOND_STRAIGHT,SECOND_STRAIGHT,FAST,ANY_WAY,END_AT_LAST_POINT);
			break;

		case SECOND_STRAIGHT:
			state=try_going(600,COLOR_Y(300),state,SECOND_ROT,SECOND_ROT,FAST,ANY_WAY,NO_DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
			break;

		case SECOND_ROT:
			state=try_go_angle(PI4096,state,THIRD_STRAIGHT,THIRD_STRAIGHT,FAST,ANY_WAY,END_AT_LAST_POINT);
			break;

		case THIRD_STRAIGHT:
			state=try_advance(NULL, entrance,730,state, GIVE_DATA, GIVE_DATA,FAST, BACKWARD, NO_DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
			break;

		case GIVE_DATA:{
			CAN_msg_t msg;
			msg.sid=PROP_DATALASER;
			msg.size=0;
			CAN_send(&msg);
			state=DONE;
		}
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
