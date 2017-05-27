#include "action_big.h"
#include "../actions_both_2017.h"
#include "../../propulsion/movement.h"
#include "../../propulsion/astar.h"
#include "../../utils/actionChecker.h"
#include "../../utils/generic_functions.h"
#include "../../QS/QS_IHM.h"
#include "../../QS/QS_outputlog.h"
#include "../../QS/QS_stateMachineHelper.h"
#include "../../actuator/act_functions.h"


error_e sub_harry_initiale(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_INITIALE,
			INIT,
			COMPUTE,
			ROCKER,
			ADVANCE_ON_TURNING_POINT,
			ADVANCE_TO_GET_OUT_START_ZONE,
			GET_OUT_OF_START_ZONE,

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

#define SIZE_OUR_MODULES  (3)
const get_this_module_s our_modules[SIZE_OUR_MODULES] = {
		{.numero = MODULE_OUR_START,		.side = COLOR_EXP(LEFT, RIGHT)},  // Utiliser COLOR_EXP pour changer le côté de stockage suivant la couleur
		{.numero = MODULE_OUR_SIDE, 		.side = COLOR_EXP(LEFT, RIGHT)},
		{.numero = MODULE_OUR_MID, 			.side = COLOR_EXP(LEFT, RIGHT)}
};

#define SIZE_OUR_MODULES_WITH_ROCKET  (2)   // Prise des modules suivi de la fusée multicouleur
const get_this_module_s our_modules_with_rocket[SIZE_OUR_MODULES_WITH_ROCKET] = {
		{.numero = MODULE_OUR_START,		.side = COLOR_EXP(LEFT, RIGHT)},	// Utiliser COLOR_EXP pour changer le côté de stockage suivant la couleur
		{.numero = MODULE_OUR_SIDE, 		.side = COLOR_EXP(RIGHT, LEFT)}
};

#define SIZE_ADV_MODULES  (3)
const get_this_module_s adv_modules[SIZE_ADV_MODULES] = {
		{.numero = MODULE_ADV_START,		.side = COLOR_EXP(LEFT, RIGHT)},	// Utiliser COLOR_EXP pour changer le côté de stockage suivant la couleur
		{.numero = MODULE_ADV_SIDE, 		.side = COLOR_EXP(LEFT, RIGHT)},
		{.numero = MODULE_ADV_MID, 			.side = COLOR_EXP(LEFT, RIGHT)}
};

#define SIZE_ADV_MODULES_WITH_ROCKET  (2)	// Prise des modules suivi de la fusée multicouleur
const get_this_module_s adv_modules_with_rocket[SIZE_ADV_MODULES_WITH_ROCKET] = {
		{.numero = MODULE_ADV_START,		.side = COLOR_EXP(LEFT, RIGHT)},	// Utiliser COLOR_EXP pour changer le côté de stockage suivant la couleur
		{.numero = MODULE_ADV_SIDE, 		.side = COLOR_EXP(LEFT, RIGHT)}
};



//depose element
	switch(state){
		case INIT:
			if(i_am_in_square_color(0, 360, 0, 360) && IHM_switchs_get(SWITCH_WITH_BASCULE)){   //le robot est dans la zone avant la bascule
				state = ROCKER;
			}else if(IHM_switchs_get(SWITCH_OUR_MODULES)){
				state = COMPUTE; // On n'avance pas car la strat des modules le fait pour nous
			}else if(IHM_switchs_get(SWITCH_OUR_ROCKET_UNICOLOR)){
				state = ADVANCE_ON_TURNING_POINT;
			}else{
				state = ADVANCE_TO_GET_OUT_START_ZONE;
			}
			

			
			break;
		case ADVANCE_ON_TURNING_POINT:
			state = try_advance(NULL, entrance, 100, state, COMPUTE, COMPUTE, FAST, FORWARD, NO_DODGE_AND_NO_WAIT, END_AT_BRAKE);
			break;

		case ADVANCE_TO_GET_OUT_START_ZONE:
			state = try_advance(NULL, entrance, 150, state, GET_OUT_OF_START_ZONE, GET_OUT_OF_START_ZONE, FAST, FORWARD, NO_DODGE_AND_NO_WAIT, END_AT_BRAKE);
			break;

		case GET_OUT_OF_START_ZONE:
			state = try_going(700, COLOR_Y(1050), state, COMPUTE, COMPUTE, FAST, FORWARD, NO_DODGE_AND_NO_WAIT, END_AT_BRAKE);
			break;

		case COMPUTE:
			if(entrance){
				ACT_push_order(ACT_CYLINDER_BALANCER_LEFT, ACT_CYLINDER_BALANCER_LEFT_IN);
				ACT_push_order(ACT_CYLINDER_BALANCER_RIGHT, ACT_CYLINDER_BALANCER_RIGHT_IN);
				ACT_push_order(ACT_CYLINDER_ARM_LEFT, ACT_CYLINDER_ARM_LEFT_PREPARE_TO_TAKE);
				ACT_push_order(ACT_CYLINDER_ARM_RIGHT, ACT_CYLINDER_ARM_RIGHT_PREPARE_TO_TAKE);
				ACT_push_order(ACT_CYLINDER_DISPOSE_LEFT, ACT_CYLINDER_DISPOSE_LEFT_TAKE);
				ACT_push_order(ACT_CYLINDER_DISPOSE_RIGHT, ACT_CYLINDER_DISPOSE_RIGHT_TAKE);
			}

			if(i_am_in_square_color(0, 360, 0, 360) && IHM_switchs_get(SWITCH_WITH_BASCULE)){   //le robot est dans la zone avant la bascule
				state = ROCKER;
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
			state=check_sub_action_result(sub_harry_prise_modules_manager(adv_modules_with_rocket, SIZE_ADV_MODULES_WITH_ROCKET),state, ROCKET_MULTICOLOR_ADV, ERROR);
			break;

		case MODULES_ADV:
			state=check_sub_action_result(sub_harry_prise_modules_manager(adv_modules, SIZE_ADV_MODULES),state, DONE, ERROR);
			break;

		case ROCKET_MULTICOLOR_ADV:
			state=check_sub_action_result(sub_harry_rocket_multicolor(ADV_ELEMENT),state, DONE, ERROR);
			break;

		case MODULES_AND_ORE_ADV:
			state=check_sub_action_result(sub_harry_prise_modules_manager(adv_modules, SIZE_ADV_MODULES),state, ORE_ADV, ERROR);
			break;

		case ORE_ADV:
			state=check_sub_action_result(sub_harry_take_big_crater(ADV_ELEMENT),state, DONE, ERROR);
			break;


		//OUR 1 action
		case MODULES_OUR:
			state=check_sub_action_result(sub_harry_prise_modules_initiale(),state, DONE, ERROR);
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
			state=check_sub_action_result(sub_harry_prise_modules_initiale(), state, ROCKET_MULTICOLOR_OUR, ERROR);
			break;

		case MODULES_AND_ORE_OUR:
			state=check_sub_action_result(sub_harry_prise_modules_initiale(),state, ORE_OUR, ERROR);
			break;

		case MODULES_AND_ROCKET_UNICOLOR:
			state=check_sub_action_result(sub_harry_prise_modules_initiale(),state, ROCKET_UNICOLOR, ERROR);
			break;

		case  MODULES_AND_PUT_OFF:
			state=check_sub_action_result(sub_harry_prise_modules_manager(our_modules, SIZE_OUR_MODULES),state, PUT_OFF, ERROR);
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





