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

void julien_strat_inutile_big(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_INUTILE,
			INIT,
			STATE0,
			STATE1,
			STATE2,
			STATE3,
			STATE4,
			STATE5,
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
		{.numero = MODULE_OUR_SIDE, 		.side = COLOR_EXP(LEFT, RIGHT)}
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

	static Uint8 number_modules = 3;
	static get_this_module_s list_modules[3]={
			{MODULE_OUR_START,LEFT},
			{MODULE_OUR_SIDE, RIGHT},
			{MODULE_OUR_MID, LEFT}
	};

	UNUSED_VAR(our_modules);
	UNUSED_VAR(our_modules_with_rocket);
	UNUSED_VAR(adv_modules);
	UNUSED_VAR(adv_modules_with_rocket);
	UNUSED_VAR(number_modules);
	UNUSED_VAR(list_modules);

	static int destination=0;

	switch(state){
		case INIT:
			if(entrance){
				destination = global.pos.x + 100;
			}
			//state = check_sub_action_result(sub_harry_prise_module_start_centre(OUR_ELEMENT,NO_SIDE),state,STATE2,ERROR);
			//state = check_sub_action_result(sub_harry_prise_modules_centre(OUR_ELEMENT,FALSE),state,STATE1,ERROR);

			//state = try_going(1000, 2000,state, STATE2, ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			state = try_going(destination, global.pos.y, state,STATE0,ERROR,FAST,ANY_WAY,NO_DODGE_AND_WAIT,END_AT_LAST_POINT);
			break;

		case STATE0:
			//state = check_sub_action_result(sub_harry_prise_modules_manager(our_modules_with_rocket,SIZE_OUR_MODULES_WITH_ROCKET),state,DONE,ERROR);
			state = check_sub_action_result(sub_harry_rocket_monocolor(),state, DONE, ERROR);
			break;

		case STATE1:
			state = try_going(1000,350,state, STATE0, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			//state = check_sub_action_result(sub_harry_prise_module_side_centre(OUR_ELEMENT,NO_SIDE),state,STATE2,ERROR);
			break;

		case STATE2:
			state = check_sub_action_result(sub_harry_prise_module_base_centre(OUR_ELEMENT,NO_SIDE),state,STATE0,ERROR);
			break;

		case STATE3:
			state = check_sub_action_result(sub_harry_prise_module_start_centre(ADV_ELEMENT,NO_SIDE),state,STATE5,ERROR);
			break;

		case STATE4:
			state = check_sub_action_result(sub_harry_prise_module_side_centre(ADV_ELEMENT,NO_SIDE),state,STATE3,ERROR);
			break;

		case STATE5:
			state = check_sub_action_result(sub_harry_prise_module_base_centre(ADV_ELEMENT,NO_SIDE),state,STATE1,ERROR);
			break;

		case ERROR:
			break;

		case DONE:
			break;
	}
}

void julien_strat_inutile_small(){
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
