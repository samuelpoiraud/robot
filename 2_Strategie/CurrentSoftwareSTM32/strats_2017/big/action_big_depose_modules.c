#include "action_big.h"
#include "../../propulsion/movement.h"
#include "../../propulsion/astar.h"
#include "../../QS/QS_stateMachineHelper.h"
#include "../../utils/generic_functions.h"

error_e sub_harry_depose_modules_centre(){
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
			FIRST_CHOICE,
			SECOND_CHOICE,
			THIRD_CHOICE,
			FOURTH_CHOICE,
			FIFTH_CHOICE,
			ERROR,
			DONE
		);

	zone_characteristics central;
	central.xmin=0;
	central.xmax=2000;
	central.ymin=0;
	central.ymax=3000;
	central.enable_zone=FALSE;
	central.nb_cylinder_max=6;

	zone_characteristics our_diagonal;
	our_diagonal.xmin=0;
	our_diagonal.xmax=2000;
	our_diagonal.ymin=0;
	our_diagonal.ymax=3000;
	our_diagonal.enable_zone=FALSE;
	our_diagonal.nb_cylinder_max=6;

	zone_characteristics our_side;
	our_side.xmin=0;
	our_side.xmax=2000;
	our_side.ymin=0;
	our_side.ymax=3000;
	our_side.enable_zone=FALSE;
	our_side.nb_cylinder_max=4;

	zone_characteristics adv_diagonal;
	adv_diagonal.xmin=0;
	adv_diagonal.xmax=2000;
	adv_diagonal.ymin=0;
	adv_diagonal.ymax=3000;
	adv_diagonal.enable_zone=FALSE;
	adv_diagonal.nb_cylinder_max=6;

	zone_characteristics adv_side;
	adv_side.xmin=0;
	adv_side.xmax=2000;
	adv_side.ymin=0;
	adv_side.ymax=3000;
	adv_side.enable_zone=FALSE;
	adv_side.nb_cylinder_max=4;

	zone_characteristics first_zone=central;
	zone_characteristics second_zone=our_diagonal;
	zone_characteristics third_zone=our_side;
	zone_characteristics fourth_zone=adv_diagonal;
	zone_characteristics fifth_zone=adv_side;


	switch(state){
		case INIT:
#ifdef SCAN_ELEMENTS
			//scan les bases de constructions pour compter et évaluer le remplissage
#else
			state=COMPUTE;
#endif
			break;

		case COMPUTE:
			if((first_zone.nb_cylinder<first_zone.nb_cylinder_max)&&((first_zone.enable_zone=!TRUE)||(i_am_in_square_color(first_zone.xmin, first_zone.xmax, first_zone.ymin, first_zone.ymax)))){
				state=FIRST_CHOICE;
			}else if((second_zone.nb_cylinder<second_zone.nb_cylinder_max)&&((second_zone.enable_zone=!TRUE)||(i_am_in_square_color(second_zone.xmin, second_zone.xmax, second_zone.ymin, second_zone.ymax)))){
				state=SECOND_CHOICE;
			}else if((third_zone.nb_cylinder<third_zone.nb_cylinder_max)&&((third_zone.enable_zone=!TRUE)||(i_am_in_square_color(third_zone.xmin, third_zone.xmax, third_zone.ymin, third_zone.ymax)))){
				state=THIRD_CHOICE;
			}else if((fourth_zone.nb_cylinder<fourth_zone.nb_cylinder_max)&&((fourth_zone.enable_zone=!TRUE)||(i_am_in_square_color(fourth_zone.xmin, fourth_zone.xmax, fourth_zone.ymin, fourth_zone.ymax)))){
				state=FOURTH_CHOICE;
			}else if((fifth_zone.nb_cylinder<fifth_zone.nb_cylinder_max)&&((fifth_zone.enable_zone=!TRUE)||(i_am_in_square_color(fifth_zone.xmin, fifth_zone.xmax, fifth_zone.ymin, fifth_zone.ymax)))){
				state=FIFTH_CHOICE;
			}
			break;

		case FIRST_CHOICE:
			//va sur la dépose prioritaire 1
			break;

		case SECOND_CHOICE:
			//va sur la dépose prioritaire 2
			break;

		case THIRD_CHOICE:
			//va sur la dépose prioritaire 3
			break;

		case FOURTH_CHOICE:
			//va sur la dépose prioritaire 4
			break;

		case FIFTH_CHOICE:
			//va sur la dépose prioritaire 5
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
