#include "action_small.h"
#include "../../propulsion/movement.h"
#include "../../propulsion/astar.h"
#include "../../QS/QS_stateMachineHelper.h"
#include "../../QS/QS_outputlog.h"
#include "../../utils/actionChecker.h"
#include "../../utils/generic_functions.h"
#include "../../actuator/act_functions.h"
#include "../../avoidance.h"

error_e sub_anne_depose_minerais(void){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_ANNE_DEPOSE_MINERAIS,
			INIT,
			ERROR,
			DONE
		);

	switch(state){
		case INIT:
			state = DONE;
			break;

		case ERROR:
			RESET_MAE();
			on_turning_point();
			return NOT_HANDLED;
			break;

		case DONE:
			RESET_MAE();
			on_turning_point();
			return END_OK;
			break;

		default:
			if(entrance)
				debug_printf("default case in sub_anne_depose_minerais\n");
			break;
	}

	return IN_PROGRESS;
}
