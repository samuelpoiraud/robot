#include "action_big.h"
#include "../../propulsion/movement.h"
#include "../../propulsion/astar.h"
#include "../../QS/QS_stateMachineHelper.h"
#include "../../utils/generic_functions.h"
#include "../../utils/actionChecker.h"
#include "../../elements.h"


error_e sub_harry_depose_modules_centre(ELEMENTS_property_e modules){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_DEPOSE_MODULES_CENTRE,
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

error_e sub_harry_depose_modules_side(ELEMENTS_property_e modules){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_DEPOSE_MODULES_SIDE,
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



error_e sub_harry_cylinder_depose_manager(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_CYLINDER_DEPOSE_MANAGER,
			INIT,
			COMPUTE,
			MANAGE,
			GO_TO_MIDDLE,
			GO_TO_OUR_CENTER,
			GO_TO_OUR_SIDE,
			GO_TO_ADV_CENTER,
			GO_TO_ADV_SIDE,
			ERROR,
			DONE
		);



	moduleDropLocation_e first_zone=MODULE_DROP_MIDDLE;
	moduleDropLocation_e second_zone=MODULE_DROP_OUR_CENTER;
	moduleDropLocation_e third_zone=MODULE_DROP_OUR_SIDE;
	moduleDropLocation_e fourth_zone=MODULE_DROP_ADV_CENTER;
	moduleDropLocation_e fifth_zone=MODULE_DROP_ADV_SIDE;
	moduleDropLocation_e prefered_zone;

	switch(state){
		case INIT:
#ifdef SCAN_ELEMENTS
			//scan les bases de constructions pour compter et évaluer le remplissage
#else
			state=COMPUTE;
#endif
			break;

		case COMPUTE:
			if((getNbDrop(first_zone)<module_zone[first_zone].nb_cylinder_max)&&((module_zone[first_zone].enable_zone!=TRUE)||(i_am_in_square_color(module_zone[first_zone].xmin, module_zone[first_zone].xmax, module_zone[first_zone].ymin, module_zone[first_zone].ymax)))){
				prefered_zone=first_zone;
				state=MANAGE;
			}else if((getNbDrop(second_zone)<module_zone[second_zone].nb_cylinder_max)&&((module_zone[second_zone].enable_zone!=TRUE)||(i_am_in_square_color(module_zone[second_zone].xmin, module_zone[second_zone].xmax, module_zone[second_zone].ymin, module_zone[second_zone].ymax)))){
				prefered_zone=second_zone;
				state=MANAGE;
			}else if((getNbDrop(third_zone)<module_zone[third_zone].nb_cylinder_max)&&((module_zone[third_zone].enable_zone!=TRUE)||(i_am_in_square_color(module_zone[third_zone].xmin, module_zone[third_zone].xmax, module_zone[third_zone].ymin, module_zone[third_zone].ymax)))){
				prefered_zone=third_zone;
				state=MANAGE;
			}else if((getNbDrop(fourth_zone)<module_zone[fourth_zone].nb_cylinder_max)&&((module_zone[fourth_zone].enable_zone!=TRUE)||(i_am_in_square_color(module_zone[fourth_zone].xmin, module_zone[fourth_zone].xmax, module_zone[fourth_zone].ymin, module_zone[fourth_zone].ymax)))){
				prefered_zone=fourth_zone;
				state=MANAGE;
			}else if((getNbDrop(fifth_zone)<module_zone[fifth_zone].nb_cylinder_max)&&((module_zone[fifth_zone].enable_zone!=TRUE)||(i_am_in_square_color(module_zone[fifth_zone].xmin, module_zone[fifth_zone].xmax, module_zone[fifth_zone].ymin, module_zone[fifth_zone].ymax)))){
				prefered_zone=fifth_zone;
				state=MANAGE;
			}else
				state=ERROR;
			break;

		case MANAGE:
			if(prefered_zone==MODULE_DROP_MIDDLE){
				state=GO_TO_MIDDLE;
			}else if(prefered_zone==MODULE_DROP_OUR_CENTER){
				state=GO_TO_OUR_CENTER;
			}else if(prefered_zone==MODULE_DROP_OUR_SIDE){
				state=GO_TO_OUR_SIDE;
			}else if(prefered_zone==MODULE_DROP_ADV_CENTER){
				state=GO_TO_ADV_CENTER;
			}else if(prefered_zone==MODULE_DROP_ADV_SIDE){
				state=GO_TO_ADV_SIDE;
			}
			break;

		case GO_TO_MIDDLE:
			state=check_sub_action_result(sub_harry_depose_modules_centre(NEUTRAL_ELEMENT),state,DONE,ERROR);
			break;

		case GO_TO_OUR_CENTER:
			state=check_sub_action_result(sub_harry_depose_modules_centre(OUR_ELEMENT),state,DONE,ERROR);
			break;

		case GO_TO_OUR_SIDE:
			state=check_sub_action_result(sub_harry_depose_modules_side(OUR_ELEMENT),state,DONE,ERROR);
			break;

		case GO_TO_ADV_CENTER:
			state=check_sub_action_result(sub_harry_depose_modules_centre(ADV_ELEMENT),state,DONE,ERROR);
			break;

		case GO_TO_ADV_SIDE:
			state=check_sub_action_result(sub_harry_depose_modules_side(ADV_ELEMENT),state,DONE,ERROR);
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



error_e sub_harry_return_modules(ELEMENTS_side_e side){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_RETURN_MODULES_SIDE,
			INIT,
			START,
			WAIT_ADV_COLOR,
			WAIT_WHITE,
			STOP,
			ERROR,
			DONE
		);




	switch(state){
		case INIT:
			state=START;
			break;

		case START:
			if(side==LEFT){
				//ACT_push_order_with_param(ACT_MOTOR_TURN_LEFT,allume-toi!!!,100);
			}else{
				//ACT_push_order_with_param(ACT_MOTOR_TURN_RIGHT,allume-toi!!!,100);
			}
			state=WAIT_ADV_COLOR;
			break;

		case WAIT_ADV_COLOR:
			if(side==LEFT){
				if((global.color==BLUE)&&(CW_is_color_detected(0, 2))){ //jaune à gauche
					state=WAIT_WHITE;
				}else if((global.color==YELLOW)&&(CW_is_color_detected(0, 1))){ //bleu à gauche
					state=WAIT_WHITE;
				}
			}else{
				if((global.color==BLUE)&&(CW_is_color_detected(1, 2))){ //jaune à droite
					state=WAIT_WHITE;
				}else if((global.color==YELLOW)&&(CW_is_color_detected(1, 1))){ //bleu à droite
					state=WAIT_WHITE;
				}
			}
			break;

		case WAIT_WHITE:
			if(side==LEFT){
				if(CW_is_color_detected(0, 0)){ //blanc à gauche
					state=STOP;
				}
			}else{
				if(CW_is_color_detected(1, 0)){ //blanc à droite
					state=STOP;
				}
			}
			break;

		case STOP:{
			time32_t start_time;
			if(entrance)
				start_time=global.match_time;
			if(global.match_time>start_time+500){
				state=DONE;
				if(side==LEFT){
					//ACT_push_order_with(ACT_MOTOR_TURN_LEFT,éteins-toi!!!);
				}else{
					//ACT_push_order_with(ACT_MOTOR_TURN_RIGHT,éteins-toi!!!);
				}
			}
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
