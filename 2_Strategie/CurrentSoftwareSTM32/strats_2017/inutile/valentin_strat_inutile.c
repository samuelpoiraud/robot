#include "strat_inutile.h"
#include "../../propulsion/movement.h"
#include "../../propulsion/astar.h"
#include "../../QS/QS_stateMachineHelper.h"

void valentin_strat_inutile_big(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_INUTILE,
			INIT,
			ACTION,
			ACTION2,
			ERROR,
			DONE
		);

	switch(state){
		case INIT:
			state = try_going(1200, COLOR_Y(2000), state , ACTION, ERROR, FAST, ANY_WAY, NO_AVOIDANCE, END_AT_LAST_POINT);
			break;

		case ACTION:
			state = ASTAR_try_going(1400, COLOR_Y(600), state , ACTION2, ERROR, FAST, ANY_WAY, NO_AVOIDANCE, END_AT_LAST_POINT);
			break;

		case ACTION2:
			state = ASTAR_try_going(200, COLOR_Y(300), state , DONE, ERROR, FAST, ANY_WAY, NO_AVOIDANCE, END_AT_LAST_POINT);
			break;

		case ERROR:
			break;

		case DONE:
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
