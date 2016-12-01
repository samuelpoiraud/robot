#include "action_big.h"
#include "../../propulsion/movement.h"
#include "../../propulsion/astar.h"
#include "../../QS/QS_stateMachineHelper.h"

error_e sub_harry_prise_gros_cratere(ELEMENTS_property_e mimerais){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_PRISE_GROS_CRATERE,
			INIT,
			ERROR,
			DONE
		);

	switch(state){
		case INIT:
			break;

		case ERROR:
			RESET_MAE();
			return NOT_HANDLED;
			break;

		case DONE:
			RESET_MAE();
			return END_OK;
			break;
	}

	return IN_PROGRESS;
}
