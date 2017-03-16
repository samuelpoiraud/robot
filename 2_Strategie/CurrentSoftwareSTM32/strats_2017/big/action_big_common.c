#include "action_big.h"
#include "../actions_both_2017.h"
#include "../../propulsion/movement.h"
#include "../../propulsion/astar.h"
#include "../../utils/actionChecker.h"
#include "../../utils/generic_functions.h"
#include "../../QS/QS_IHM.h"
#include "../../QS/QS_outputlog.h"
#include "../../QS/QS_stateMachineHelper.h"


error_e sub_harry_initiale(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_INITIALE,
			INIT,
			ROCKER,

			//ADV
			MODULES_AND_ROCKET_MULTICOLOR_ADV,
			MODULES_ADV,
			ROCKET_MULTICOLOR_ADV,
			MODULES_AND_ORE_ADV,
			ORE_ADV,

			//OUR
			//MODULE_AND_ROCKET_MULTICOLOR_OUR,
			//MODULE_AND_ORE,
			//MODULE_AND_ROCKET_UNICOLOR,

			MODULES_AND_ROCKET_MULTICOLOR_OUR,
			MODULES_AND_ORE_OUR,
			MODULES_AND_ROCKET_UNICOLOR,
			MODULES_AND_PUT_OFF,

			MODULES_OUR,
			ROCKET_UNICOLOR,
			ROCKET_MULTICOLOR_OUR,
			ORE_OUR,
			PUT_OFF,
			ERROR,
			DONE
		);

static get_this_module_s list_modules[8] = {
		{.numero = MODULE_OUR_START,		.side = LEFT,			.flag = FLAG_OUR_MULTICOLOR_START_IS_TAKEN},
		{.numero = MODULE_OUR_SIDE, 		.side = RIGHT,			.flag = FLAG_OUR_MULTICOLOR_SIDE_IS_TAKEN},
		{.numero = MODULE_OUR_MID, 			.side = LEFT,			.flag = FLAG_OUR_MULTICOLOR_NEAR_DEPOSE_IS_TAKEN},
		{.numero = MODULE_ADV_START,		.side = LEFT,			.flag = FLAG_ADV_MULTICOLOR_START_IS_TAKEN},
		{.numero = MODULE_ADV_SIDE, 		.side = RIGHT,			.flag = FLAG_ADV_MULTICOLOR_SIDE_IS_TAKEN},
		{.numero = MODULE_ADV_MID, 			.side = LEFT,			.flag = FLAG_ADV_MULTICOLOR_NEAR_DEPOSE_IS_TAKEN},
		{.numero = MODULE_OUR_ORE_UNI,		.side = NO_SIDE,		.flag = FLAG_OUR_UNICOLOR_NORTH_IS_TAKEN},
		{.numero = MODULE_OUR_START_ZONE_UNI, .side = NO_SIDE,		.flag = FLAG_OUR_UNICOLOR_SOUTH_IS_TAKEN}
};

//depose element
	switch(state){
		case INIT:

			if(i_am_in_square_color(0, 360, 0, 360) && IHM_switchs_get(SWITCH_WITH_BASCULE)){   //le robot est dans la zone avant la bascule
				state = ROCKER; //!boucle infini passage bascule!
			}

			//ADV
			else if(IHM_switchs_get(SWITCH_ADV_MODULES) && IHM_switchs_get(SWITCH_ADV_ROCKET_MULTICOLOR)){
				state = MODULES_AND_ROCKET_MULTICOLOR_ADV;
			}
			else if(IHM_switchs_get(SWITCH_ADV_MODULES) && IHM_switchs_get(SWITCH_ADV_ORES)){
				state = MODULES_AND_ORE_ADV;
			}
			else if(IHM_switchs_get(SWITCH_ADV_ROCKET_MULTICOLOR)){
				state = ROCKET_MULTICOLOR_ADV;
			}
			else if(IHM_switchs_get(SWITCH_ADV_MODULES)){
				state = MODULES_ADV;
			}
			else if(IHM_switchs_get(SWITCH_ADV_MODULES)){
				state = ORE_ADV;
			}

			//OUR
			else if(IHM_switchs_get(SWITCH_OUR_MODULES) && IHM_switchs_get(SWITCH_OUR_ROCKET_MULTICOLOR)){
				state = MODULES_AND_ROCKET_MULTICOLOR_OUR;
			}
			else if(IHM_switchs_get(SWITCH_OUR_MODULES) && IHM_switchs_get(SWITCH_OUR_ORES)){
				state = MODULES_AND_ORE_OUR;
			}
			else if(IHM_switchs_get(SWITCH_OUR_MODULES) && IHM_switchs_get(SWITCH_OUR_ROCKET_UNICOLOR)){
				state = MODULES_AND_ROCKET_UNICOLOR;
			}
			else if(IHM_switchs_get(SWITCH_OUR_MODULES) && IHM_switchs_get(SWITCH_DISPOSE_MODULES)){
				state = MODULES_AND_PUT_OFF;
			}
			else if(IHM_switchs_get(SWITCH_OUR_ROCKET_MULTICOLOR)){
				state = ROCKET_MULTICOLOR_OUR;
			}
			else if(IHM_switchs_get(SWITCH_OUR_ROCKET_UNICOLOR)){
				state = ROCKET_UNICOLOR;
			}
			else if(IHM_switchs_get(SWITCH_OUR_ORES)){
				state = ORE_OUR;
			}
			else if(IHM_switchs_get(SWITCH_OUR_MODULES)){
				state = MODULES_OUR;
			}
			else{
				state = DONE;
			}
			break;

		//bascule
		case ROCKER:
			state=check_sub_action_result(sub_cross_rocker(),state, INIT, ERROR);
			break;
		//ADV
		case MODULES_AND_ROCKET_MULTICOLOR_ADV:
			state=check_sub_action_result(sub_harry_prise_modules_manager(&list_modules[3], 2),state, ROCKET_MULTICOLOR_ADV, ERROR);
			break;

		case MODULES_ADV:
			state=check_sub_action_result(sub_harry_prise_modules_manager(&list_modules[3], 3),state, DONE, ERROR);
			break;

		case ROCKET_MULTICOLOR_ADV:
			state=check_sub_action_result(sub_harry_rocket_multicolor(ADV_ELEMENT),state, DONE, ERROR);
			break;

		case MODULES_AND_ORE_ADV:
			state=check_sub_action_result(sub_harry_prise_modules_manager(&list_modules[3], 3),state, ORE_ADV, ERROR);
			break;

		case ORE_ADV:
			state=check_sub_action_result(sub_harry_take_big_crater(ADV_ELEMENT),state, DONE, ERROR);
			break;


		//OUR 1 action
		case MODULES_OUR:
			state=check_sub_action_result(sub_harry_prise_modules_manager(list_modules, 3),state, DONE, ERROR);
			break;

		case ROCKET_MULTICOLOR_OUR:
			state=check_sub_action_result(sub_harry_rocket_multicolor(OUR_ELEMENT),state, DONE, ERROR);
			break;

		case ROCKET_UNICOLOR:
			state=check_sub_action_result(sub_harry_rocket_monocolor(),state, DONE, ERROR);
			break;

		case ORE_OUR:
			state=check_sub_action_result(sub_harry_take_big_crater(OUR_ELEMENT),state, DONE, ERROR);
			break;

		case PUT_OFF:
			state=check_sub_action_result(sub_harry_cylinder_depose_manager(),state, DONE, ERROR);
			break;

		//OUR 2 actions
		case MODULES_AND_ROCKET_MULTICOLOR_OUR:
			state=check_sub_action_result(sub_harry_prise_modules_manager(list_modules, 2),state, ROCKET_MULTICOLOR_OUR, ERROR);
			break;

		case MODULES_AND_ORE_OUR:
			state=check_sub_action_result(sub_harry_prise_modules_manager(list_modules, 3),state, ORE_OUR, ERROR);
			break;

		case MODULES_AND_ROCKET_UNICOLOR:
			state=check_sub_action_result(sub_harry_prise_modules_manager(list_modules, 3),state, ROCKET_UNICOLOR, ERROR);
			break;

		case  MODULES_AND_PUT_OFF:
			state=check_sub_action_result(sub_harry_prise_modules_manager(list_modules, 3),state, PUT_OFF, ERROR);
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
				debug_printf("default case in sub_harry_initiale\n");
			break;
	}

	return IN_PROGRESS;
}

error_e sub_harry_end_of_match(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_END_OF_MATCH,
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
				debug_printf("default case in sub_harry_end_of_match\n");
			break;
	}

	return IN_PROGRESS;
}





