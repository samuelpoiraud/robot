#include "action_small.h"
#include "../../propulsion/movement.h"
#include "../../propulsion/astar.h"
#include "../../propulsion/prop_functions.h"
#include "../../QS/QS_stateMachineHelper.h"
#include "../../QS/QS_outputlog.h"
#include "../../utils/actionChecker.h"
#include "../../utils/generic_functions.h"
#include "../../actuator/act_functions.h"
#include "../../avoidance.h"
#include "../../QS/QS_types.h"
#include "../../QS/QS_IHM.h"
#include "../../elements.h"
#include "../../high_level_strat.h"
#include "../../actuator/act_functions.h"
#include "../../actuator/queue.h"
#include "../actions_both_generic.h"
#include "../Supervision/Supervision.h"

error_e sub_anne_homologation_belgique(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_ANNE_HOMOLOGATION_BELGIQUE,
			INIT,
			GO_TO_CENTER,
			OPEN_ACT,
			TAKE_CYLINDER,
			CLOSE_ACT,
			MOVE_BACK,
			GO_TO_HOME,
			DELIVER_CYLINDER,
			ERROR,
			DONE
		);
	switch(state){
		case INIT:
			state = GO_TO_CENTER;
			break;

		case GO_TO_CENTER:
			state = try_going(600, 1500, state, OPEN_ACT, GO_TO_HOME, FAST, FORWARD, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

		case OPEN_ACT:
			if(entrance){
				ACT_push_order(ACT_QUEUE_Small_cylinder_multifonction, ACT_SMALL_CYLINDER_MULTIFONCTION_PUSH);
			}
			state = TAKE_CYLINDER;
			break;

		case TAKE_CYLINDER:
			state = try_going(600, COLOR_Y(2000), state, CLOSE_ACT, GO_TO_HOME, FAST, FORWARD, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

		case CLOSE_ACT:
			ACT_push_order(ACT_QUEUE_Small_cylinder_multifonction, ACT_SMALL_CYLINDER_MULTIFONCTION_LOCK);
			state = MOVE_BACK;
			break;

		case MOVE_BACK:
			state = try_going(600, 1500, state, GO_TO_HOME, GO_TO_HOME, FAST, BACKWARD, NO_DODGE_AND_NO_WAIT, END_AT_BRAKE);
			break;

		case GO_TO_HOME:
			state = try_going(150, COLOR_Y(860), state, DELIVER_CYLINDER, DELIVER_CYLINDER, FAST, BACKWARD, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

		case DELIVER_CYLINDER:
			if(entrance)
				ACT_push_order(ACT_QUEUE_Small_cylinder_multifonction, ACT_SMALL_CYLINDER_MULTIFONCTION_PUSH);
			//et je reste planté là comme un con parce que je veux pas faire de connerie
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
			{
				error_printf("default case in sub_anne_homologation_belgique\n");
			}
			break;
	}

	return IN_PROGRESS;
}


error_e sub_anne_prise_modules_centre(ELEMENTS_property_e modules){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_ANNE_PRISE_MODULES_CENTRE,
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
			{
				error_printf("default case in sub_anne_prise_modules_centre\n");
			}
			break;
	}

	return IN_PROGRESS;
}


error_e sub_anne_prise_module_start_centre(ELEMENTS_property_e modules, ELEMENTS_side_e side){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_ANNE_MODULE_START_CENTER,
			INIT,
			CHECK_FIRST_ELEMENT,
			GO_TO_START_POINT_UP,
			GO_TO_START_POINT_SIDE,
			GO_TO_START_POINT_DOWN,
			GET_IN_CLOSE_ADV_ZONE,
			GET_IN_FAR_ADV_ZONE,
			GET_IN_ASTAR,
			TAKE_FIRST_MODULE_UP,
			TAKE_FIRST_MODULE_SIDE,
			TAKE_FIRST_MODULE_DOWN,

			STORAGE_LEFT,
			STORAGE_RIGHT,

			ERROR,
			DONE
		);

		const displacement_t curve_far_zone_yellow[2] = {(displacement_t){(GEOMETRY_point_t){1000, 2400}, FAST},
													     (displacement_t){(GEOMETRY_point_t){950, 2000}, FAST},
													    };

		const displacement_t curve_far_zone_blue[2] = {(displacement_t){(GEOMETRY_point_t){1000, 600}, FAST},
													   (displacement_t){(GEOMETRY_point_t){950, 1000}, FAST},
													  };

		switch(state){
			case INIT:
				if (ELEMENTS_get_flag(FLAG_OUR_MULTICOLOR_START_IS_TAKEN)){
					state = DONE; // Il n'y a plus rien à faire
				}
				else if(ELEMENTS_get_flag(FLAG_SUB_HARRY_TAKE_CYLINDER_OUR_ROCKET_UNI)) // Si Harry est déjà en train de faire cette action
				{
					state = ERROR;
				}
				else
				{
					// Si on prend les modules du coté BLUE		(utiliser i_am_in_square et pas i_am_in_square_color)
					if((global.color == BLUE && modules == OUR_ELEMENT) || (global.color == YELLOW && modules == ADV_ELEMENT)){
						if(i_am_in_square(0, 550, 800, 1200)){
							state = CHECK_FIRST_ELEMENT;
						}else if(i_am_in_square(0, 1000, 1200, 2100)){
							state = GO_TO_START_POINT_SIDE;
						}else if(i_am_in_square(700,1400,0,800)){
							state = GO_TO_START_POINT_DOWN;
						}else if(i_am_in_square(700, 1400, 2100, 2600)){
							state = GET_IN_CLOSE_ADV_ZONE;
						}else if(i_am_in_square(700, 1400, 2600, 2800)){
							state = GET_IN_FAR_ADV_ZONE;
						}else{
							state = GET_IN_ASTAR;
						}
					}
					else	// Si on prend les modules du coté YELLOW	(utiliser i_am_in_square et pas i_am_in_square_color)
					{
						if(i_am_in_square(0, 550, 1800, 2200)){
							state = CHECK_FIRST_ELEMENT;
						}else if(i_am_in_square(0, 1000, 900, 1800)){
							state = GO_TO_START_POINT_SIDE;
						}else if(i_am_in_square(700,1400,2200,3000)){
							state = GO_TO_START_POINT_DOWN;
						}else if(i_am_in_square(700, 1400, 400, 1100)){
							state = GET_IN_CLOSE_ADV_ZONE;
						}else if(i_am_in_square(700, 1400, 200, 400)){
							state = GET_IN_FAR_ADV_ZONE;
						}else{
							state = GET_IN_ASTAR;
						}
					}

					// On lève le flag de subaction
					ELEMENTS_set_flag(FLAG_SUB_ANNE_TAKE_CYLINDER_OUR_ROCKET_UNI, TRUE);
				}
			break;


			case CHECK_FIRST_ELEMENT:
				if(modules == ADV_ELEMENT){
					state = try_going(350,COLOR_Y(1400), CHECK_FIRST_ELEMENT, GO_TO_START_POINT_UP, ERROR, FAST, ANY_WAY, DODGE_AND_NO_WAIT, END_AT_BRAKE);
				}
				else{
					state = try_going(global.pos.x+100, global.pos.y, state, GO_TO_START_POINT_UP, ERROR, FAST,ANY_WAY, DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
				}
				break;

			case GET_IN_CLOSE_ADV_ZONE:
				if((global.color == BLUE && modules == OUR_ELEMENT) || (global.color == YELLOW && modules == ADV_ELEMENT)){
					state = try_going(900, 2000, state, GO_TO_START_POINT_SIDE, GET_IN_ASTAR, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_BRAKE);
				}else{
					state = try_going(900, 1000, state, GO_TO_START_POINT_SIDE, GET_IN_ASTAR, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_BRAKE);
				}
				break;

			case GET_IN_FAR_ADV_ZONE:
				if((global.color == BLUE && modules == OUR_ELEMENT) || (global.color == YELLOW && modules == ADV_ELEMENT)){
					state = try_going_multipoint(curve_far_zone_yellow, 2, state, GO_TO_START_POINT_SIDE, GET_IN_ASTAR, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_BRAKE);
				}else{
					state = try_going_multipoint(curve_far_zone_blue, 2, state, GO_TO_START_POINT_SIDE, GET_IN_ASTAR, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_BRAKE);
				}
				break;

			case GET_IN_ASTAR:
				if((global.color == BLUE && modules == OUR_ELEMENT) || (global.color == YELLOW && modules == ADV_ELEMENT)){
					state = ASTAR_try_going(600, 1250, state, TAKE_FIRST_MODULE_SIDE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
				}else{
					state = ASTAR_try_going(600, 1750, state, TAKE_FIRST_MODULE_SIDE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
				}
				break;

			case GO_TO_START_POINT_UP:
				if((global.color == BLUE && modules == OUR_ELEMENT) || (global.color == YELLOW && modules == ADV_ELEMENT)){
					state = try_going(350, 950, state, TAKE_FIRST_MODULE_UP, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_BRAKE);
				}else{
					state = try_going(350, 2050, state, TAKE_FIRST_MODULE_UP, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_BRAKE);
				}
// pompe en meme temps ajout du crochet
				if(ON_LEAVE()){
					if(side == LEFT){
						ACT_push_order( ACT_POMPE_ELEVATOR_LEFT , ACT_POMPE_NORMAL );
						ACT_push_order( ACT_POMPE_SLIDER_LEFT , ACT_POMPE_NORMAL );
					}
					else{
						ACT_push_order( ACT_POMPE_ELEVATOR_RIGHT, ACT_POMPE_NORMAL );
						ACT_push_order( ACT_POMPE_SLIDER_RIGHT , ACT_POMPE_NORMAL );
					}
				}
				break;

			case GO_TO_START_POINT_SIDE:
				if((global.color == BLUE && modules == OUR_ELEMENT) || (global.color == YELLOW && modules == ADV_ELEMENT)){
					state = try_going(650, 1250, state, TAKE_FIRST_MODULE_SIDE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_BRAKE);
				}else{
					state = try_going(650, 1750, state, TAKE_FIRST_MODULE_SIDE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_BRAKE);
				}
				if(ON_LEAVE()){
					if(side == LEFT){
						ACT_push_order( ACT_POMPE_ELEVATOR_LEFT , ACT_POMPE_NORMAL );
						ACT_push_order( ACT_POMPE_SLIDER_LEFT , ACT_POMPE_NORMAL );
					}
					else{
						ACT_push_order( ACT_POMPE_ELEVATOR_RIGHT, ACT_POMPE_NORMAL );
						ACT_push_order( ACT_POMPE_SLIDER_RIGHT , ACT_POMPE_NORMAL );
					}
				}
				break;

			case GO_TO_START_POINT_DOWN:
				if((global.color == BLUE && modules == OUR_ELEMENT) || (global.color == YELLOW && modules == ADV_ELEMENT)){
					state = try_going(1000, 860, state, TAKE_FIRST_MODULE_SIDE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_BRAKE);
				}else{
					state = try_going(1000, 2140, state, TAKE_FIRST_MODULE_SIDE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_BRAKE);
				}
				if(ON_LEAVE()){
					if(side == LEFT){
						ACT_push_order( ACT_POMPE_ELEVATOR_LEFT , ACT_POMPE_NORMAL );
						ACT_push_order( ACT_POMPE_SLIDER_LEFT , ACT_POMPE_NORMAL );
					}
					else{
						ACT_push_order( ACT_POMPE_ELEVATOR_RIGHT, ACT_POMPE_NORMAL );
						ACT_push_order( ACT_POMPE_SLIDER_RIGHT , ACT_POMPE_NORMAL );
					}
				}
				break;

			case TAKE_FIRST_MODULE_UP:
				if(side == LEFT || side == NO_SIDE){
					if((global.color == BLUE && modules == OUR_ELEMENT) || (global.color == YELLOW && modules == ADV_ELEMENT)){
						state = try_going(500, 950, state, STORAGE_LEFT, ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
					}else{
						state = try_going(500, 2050, state, STORAGE_LEFT, ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
					}
				}
				else{
					if((global.color == BLUE && modules == OUR_ELEMENT) || (global.color == YELLOW && modules == ADV_ELEMENT)){
						state = try_going(500, 1025, state, STORAGE_RIGHT, ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
					}else{
						state = try_going(500, 1975, state, STORAGE_RIGHT, ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
					}
				}
				break;

			case TAKE_FIRST_MODULE_SIDE:
				if(side == LEFT || side == NO_SIDE){
					if((global.color == BLUE && modules == OUR_ELEMENT) || (global.color == YELLOW && modules == ADV_ELEMENT)){
						state = try_going(660, 1030, state, STORAGE_LEFT, ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_BRAKE);
					}else{
						state = try_going(660, 1970, state, STORAGE_LEFT, ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_BRAKE);
					}
				}
				else{
					if((global.color == BLUE && modules == OUR_ELEMENT) || (global.color == YELLOW && modules == ADV_ELEMENT)){
						state = try_going(650, 940, state, STORAGE_RIGHT, ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_BRAKE);
					}else{
						state = try_going(650, 2060, state, STORAGE_RIGHT, ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_BRAKE);
					}
				}
				break;

			case TAKE_FIRST_MODULE_DOWN:
				if(side == LEFT || side == NO_SIDE){
					if((global.color == BLUE && modules == OUR_ELEMENT) || (global.color == YELLOW && modules == ADV_ELEMENT)){
						state = try_going(650, 1100, state, STORAGE_LEFT, ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_BRAKE);
					}else{
						state = try_going(650, 1900, state, STORAGE_LEFT, ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_BRAKE);
					}
				}
				else{
					if((global.color == BLUE && modules == OUR_ELEMENT) || (global.color == YELLOW && modules == ADV_ELEMENT)){
						state = try_going(500, 920, state, STORAGE_RIGHT, ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_BRAKE);
					}else{
						state = try_going(500, 2080, state, STORAGE_RIGHT, ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_BRAKE);
					}
				}
				break;


			case STORAGE_LEFT:
				if(entrance){
					if(global.color == BLUE){
						STOCKS_addModule(MODULE_BLUE, MODULE_STOCK_LEFT, STOCK_POS_ELEVATOR);
					}else{
						STOCKS_addModule(MODULE_YELLOW, MODULE_STOCK_RIGHT, STOCK_POS_ELEVATOR);
					}
					ACT_push_order( ACT_POMPE_SLIDER_LEFT , ACT_POMPE_STOP );

				}

				state = check_sub_action_result(sub_act_anne_mae_store_modules(TRUE), state, DONE, ERROR);

				if(state == DONE){
					if(modules == OUR_ELEMENT)
						ELEMENTS_set_flag(FLAG_OUR_MULTICOLOR_START_IS_TAKEN, TRUE);
					else
						ELEMENTS_set_flag(FLAG_ADV_MULTICOLOR_START_IS_TAKEN, TRUE);
				}
				if(ON_LEAVE()){
					ELEMENTS_set_flag(FLAG_SUB_ANNE_TAKE_CYLINDER_OUR_ROCKET_UNI, FALSE);	// Flag subaction
					//set_sub_act_enable(SUB_ANNE_DEPOSE_MODULES, TRUE);   // Activation de la dépose
				}
				break;

			case STORAGE_RIGHT:
				if(entrance){
					if(global.color == BLUE){
						STOCKS_addModule(MODULE_BLUE, MODULE_STOCK_RIGHT, STOCK_POS_ELEVATOR);
					}else{
						STOCKS_addModule(MODULE_YELLOW, MODULE_STOCK_RIGHT, STOCK_POS_ELEVATOR);
					}
					ACT_push_order( ACT_POMPE_SLIDER_RIGHT , ACT_POMPE_STOP );
				}

				state=check_sub_action_result(sub_act_anne_mae_store_modules(TRUE),state,DONE,ERROR);

				if(state==DONE){
					if(modules == OUR_ELEMENT)
						ELEMENTS_set_flag(FLAG_OUR_MULTICOLOR_START_IS_TAKEN, TRUE);
					else
						ELEMENTS_set_flag(FLAG_ADV_MULTICOLOR_START_IS_TAKEN, TRUE);	// Flag element
				}
				if(ON_LEAVE()){
					ELEMENTS_set_flag(FLAG_SUB_ANNE_TAKE_CYLINDER_OUR_ROCKET_UNI, FALSE);	// Flag subaction
					//set_sub_act_enable(SUB_ANNE_DEPOSE_MODULES, TRUE);   // Activation de la dépose
				}
				break;

			case ERROR:
				RESET_MAE();
				ELEMENTS_set_flag(FLAG_SUB_ANNE_TAKE_CYLINDER_OUR_ROCKET_UNI, FALSE);
				ELEMENTS_set_flag(FLAG_SUB_ANNE_TAKE_CYLINDER_OUR_ROCKET_MULTI, FALSE);
				ELEMENTS_set_flag(FLAG_SUB_ANNE_TAKE_CYLINDER_OUR_CENTER, FALSE);
				on_turning_point();
				return NOT_HANDLED;
				break;

			case DONE:
				RESET_MAE();
				ELEMENTS_set_flag(FLAG_SUB_ANNE_TAKE_CYLINDER_OUR_ROCKET_UNI, FALSE);
				ELEMENTS_set_flag(FLAG_SUB_ANNE_TAKE_CYLINDER_OUR_ROCKET_MULTI, FALSE);
				ELEMENTS_set_flag(FLAG_SUB_ANNE_TAKE_CYLINDER_OUR_CENTER, FALSE);
				on_turning_point();
				return END_OK;
				break;

			default:
				if(entrance)
				{
					error_printf("default case in sub_anne_prise_modules_centre\n");
				}
				break;
		}
	return IN_PROGRESS;
}

error_e sub_anne_prise_module_side_centre(ELEMENTS_property_e modules, ELEMENTS_side_e side){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_ANNE_MODULE_SIDE_CENTER,
			INIT,

			GET_IN_CLOSE_ADV_ZONE,
			GET_IN_FAR_ADV_ZONE,
			GET_IN_ASTAR,
			GET_IN_CLOSE_START_ZONE,
			AVOID_MODULE_START,

			GO_TO_SIDE_POINT_UP,
			GO_TO_SIDE_POINT_FROM_DEPOSE_SIDE,
			GO_TO_SIDE_POINT_FROM_LEFT_SIDE,
			TAKE_SIDE_MODULE_FROM_START_ZONE,
			TAKE_SIDE_MODULE_FROM_DEPOSE_SIDE,
			TAKE_SIDE_MODULE_FROM_LEFT_SIDE,

			STORAGE_LEFT,
			STORAGE_RIGHT,

			ERROR,
			DONE
		);

	const displacement_t curve_far_zone_yellow[2] = {(displacement_t){(GEOMETRY_point_t){1000, 2400}, FAST},
												     (displacement_t){(GEOMETRY_point_t){950, 2000}, FAST},
												    };

	const displacement_t curve_far_zone_blue[2] = {(displacement_t){(GEOMETRY_point_t){1000, 600}, FAST},
												   (displacement_t){(GEOMETRY_point_t){950, 1000}, FAST},
												  };

	const displacement_t curve_avoid_module_start_yellow[2] = {(displacement_t){(GEOMETRY_point_t){600, 1700}, FAST},
																 (displacement_t){(GEOMETRY_point_t){900, 2000}, FAST},
																};

	const displacement_t curve_avoid_module_start_blue[2] = {(displacement_t){(GEOMETRY_point_t){600, 1300}, FAST},
															 (displacement_t){(GEOMETRY_point_t){900, 1000}, FAST},
															};


	switch(state){
		case INIT:
			/*if(IHM_switchs_get(SWITCH_DISABLE_MODULE_RIGHT) && IHM_switchs_get(SWITCH_DISABLE_MODULE_LEFT)){
				state = ERROR; // Actionneurs désactivés
			}else*/ if (ELEMENTS_get_flag(FLAG_OUR_MULTICOLOR_SIDE_IS_TAKEN)){
					state = DONE; // Il n'y a plus rien à faire
			}
			else
			{
				// Si on prend les modules du coté BLUE		(utiliser i_am_in_square et pas i_am_in_square_color)
				if((global.color == BLUE && modules == OUR_ELEMENT) || (global.color == YELLOW && modules == ADV_ELEMENT)){
					if(i_am_in_square(0, 550, 800, 1200) && !ELEMENTS_get_flag(FLAG_OUR_MULTICOLOR_START_IS_TAKEN)){
						state = AVOID_MODULE_START;
					}else if(i_am_in_square(0, 550, 800, 1200) && ELEMENTS_get_flag(FLAG_OUR_MULTICOLOR_START_IS_TAKEN)){
						state = GET_IN_CLOSE_START_ZONE;
					}else if(i_am_in_square(350,1100,0,700)){
						state = GO_TO_SIDE_POINT_FROM_LEFT_SIDE;
					}else if(i_am_in_square(350, 1100, 700, 2100)){
						state = GO_TO_SIDE_POINT_UP;
					}else if(i_am_in_square(1100, 2000, 0, 800)){
						state =	GO_TO_SIDE_POINT_FROM_DEPOSE_SIDE;
					}else if(i_am_in_square(700, 1400, 2100, 2600)){
						state = GET_IN_CLOSE_ADV_ZONE;
					}else if(i_am_in_square(700, 1400, 2600, 2800)){
						state = GET_IN_FAR_ADV_ZONE;
					}else{
						state = GET_IN_ASTAR;
					}
				}
				else	// Si on prend les modules du coté YELLOW	(utiliser i_am_in_square et pas i_am_in_square_color)
				{
					if(i_am_in_square(0, 550, 1800, 2200) && !ELEMENTS_get_flag(FLAG_OUR_MULTICOLOR_START_IS_TAKEN)){
						state = AVOID_MODULE_START;
					}else if(i_am_in_square(0, 550, 1800, 2200) && ELEMENTS_get_flag(FLAG_OUR_MULTICOLOR_START_IS_TAKEN)){
						state = GET_IN_CLOSE_START_ZONE;
					}else if(i_am_in_square(350,1100,2300,3000)){
						state = GO_TO_SIDE_POINT_FROM_LEFT_SIDE;
					}else if(i_am_in_square(350, 1100, 900, 2300)){
						state = GO_TO_SIDE_POINT_UP;
					}else if(i_am_in_square(1100, 2000, 2200, 3000)){
						state =	GO_TO_SIDE_POINT_FROM_DEPOSE_SIDE;
					}else if(i_am_in_square(700, 1400, 400, 1100)){
						state = GET_IN_CLOSE_ADV_ZONE;
					}else if(i_am_in_square(700, 1400, 200, 400)){
						state = GET_IN_FAR_ADV_ZONE;
					}else{
						state = GET_IN_ASTAR;
					}
				}

				// On lève le flag de subaction
				ELEMENTS_set_flag(FLAG_SUB_ANNE_TAKE_CYLINDER_OUR_CENTER, TRUE);
			}
			break;

		case AVOID_MODULE_START:
			if((global.color == BLUE && modules == OUR_ELEMENT) || (global.color == YELLOW && modules == ADV_ELEMENT)){
				state = try_going_multipoint(curve_avoid_module_start_blue, 2, state, GO_TO_SIDE_POINT_UP, ERROR, ANY_WAY, NO_DODGE_AND_WAIT,END_AT_BRAKE);
			}else{
				state = try_going_multipoint(curve_avoid_module_start_yellow, 2, state, GO_TO_SIDE_POINT_UP, ERROR, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_BRAKE);
			}
			break;

		case GET_IN_CLOSE_ADV_ZONE:
			if((global.color == BLUE && modules == OUR_ELEMENT) || (global.color == YELLOW && modules == ADV_ELEMENT)){
				state = try_going(900, 2000, state, GO_TO_SIDE_POINT_UP, GET_IN_ASTAR, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_BRAKE);
			}else{
				state = try_going(900, 1000, state, GO_TO_SIDE_POINT_UP, GET_IN_ASTAR, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_BRAKE);
			}
			break;

		case GET_IN_FAR_ADV_ZONE:
			if((global.color == BLUE && modules == OUR_ELEMENT) || (global.color == YELLOW && modules == ADV_ELEMENT)){
				state = try_going_multipoint(curve_far_zone_yellow, 2, state, GO_TO_SIDE_POINT_UP, GET_IN_ASTAR, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_BRAKE);
			}else{
				state = try_going_multipoint(curve_far_zone_blue, 2, state, GO_TO_SIDE_POINT_UP, GET_IN_ASTAR, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_BRAKE);
			}
			break;

		case GET_IN_ASTAR:
			if((global.color == BLUE && modules == OUR_ELEMENT) || (global.color == YELLOW && modules == ADV_ELEMENT)){
				state = ASTAR_try_going(870, 700, state, GO_TO_SIDE_POINT_UP, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_BRAKE);
			}else{
				state = ASTAR_try_going(870, 2400, state, GO_TO_SIDE_POINT_UP, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_BRAKE);
			}
			break;


		case GET_IN_CLOSE_START_ZONE:
			if((global.color == BLUE && modules == OUR_ELEMENT) || (global.color == YELLOW && modules == ADV_ELEMENT)){
				state = try_going(700, 1000, state, GO_TO_SIDE_POINT_UP, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_BRAKE);
			}else{
				state = try_going(700, 2000, state, GO_TO_SIDE_POINT_UP, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_BRAKE);
			}
			break;


		case GO_TO_SIDE_POINT_UP:
			if(entrance){
				ELEMENTS_set_flag(FLAG_SUB_ANNE_TAKE_CYLINDER_OUR_ROCKET_MULTI, TRUE);
			}
			if((global.color == BLUE && modules == OUR_ELEMENT) || (global.color == YELLOW && modules == ADV_ELEMENT)){
				state = try_going(880, 750, state, TAKE_SIDE_MODULE_FROM_START_ZONE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_BRAKE);
			}
			else{
				state = try_going(880, 2250, state, TAKE_SIDE_MODULE_FROM_START_ZONE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_BRAKE);
			}
			if(ON_LEAVE()){
				if(side == LEFT)
					ACT_push_order( ACT_POMPE_ELEVATOR_LEFT , ACT_POMPE_NORMAL );
				else
					ACT_push_order( ACT_POMPE_ELEVATOR_RIGHT, ACT_POMPE_NORMAL );
			}
			break;

		case TAKE_SIDE_MODULE_FROM_START_ZONE:
			if(side == LEFT){
				if((global.color == BLUE && modules == OUR_ELEMENT) || (global.color == YELLOW && modules == ADV_ELEMENT)){
					state = try_going(980, 550, state, STORAGE_LEFT, ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
				}else{
					state = try_going(980, 2450, state, STORAGE_LEFT, ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
				}
			}
			else{
				if((global.color == BLUE && modules == OUR_ELEMENT) || (global.color == YELLOW && modules == ADV_ELEMENT)){
					state = try_going(1060, 590, state, STORAGE_RIGHT, ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
				}else{
					state = try_going(1060, 2410, state, STORAGE_RIGHT, ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
				}
			}
			break;


		case GO_TO_SIDE_POINT_FROM_DEPOSE_SIDE:
			if(entrance){
				ELEMENTS_set_flag(FLAG_SUB_ANNE_TAKE_CYLINDER_OUR_ROCKET_MULTI, TRUE);
			}
			if((global.color == BLUE && modules == OUR_ELEMENT) || (global.color == YELLOW && modules == ADV_ELEMENT)){
				state = try_going(1300, 700, state, TAKE_SIDE_MODULE_FROM_DEPOSE_SIDE, ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_BRAKE);
			}else{
				state = try_going(1300, 2300, state, TAKE_SIDE_MODULE_FROM_DEPOSE_SIDE, ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_BRAKE);
			}
			if(ON_LEAVE()){
				if(side == LEFT)
					ACT_push_order( ACT_POMPE_ELEVATOR_LEFT , ACT_POMPE_NORMAL );
				else
					ACT_push_order( ACT_POMPE_ELEVATOR_RIGHT, ACT_POMPE_NORMAL );
			}
			break;



		case TAKE_SIDE_MODULE_FROM_DEPOSE_SIDE:
			if(side == LEFT){
				if((global.color == BLUE && modules == OUR_ELEMENT) || (global.color == YELLOW && modules == ADV_ELEMENT)){
					state = try_going(1100, 620, state, STORAGE_LEFT, ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
				}else{
					state = try_going(1100, 1380, state, STORAGE_LEFT, ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
				}
			}
			else{
				if((global.color == BLUE && modules == OUR_ELEMENT) || (global.color == YELLOW && modules == ADV_ELEMENT)){
					state = try_going(1200, 520, state, STORAGE_RIGHT, ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
				}else{
					state = try_going(1200, 2480, state, STORAGE_RIGHT, ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
				}
			}
			break;


		case GO_TO_SIDE_POINT_FROM_LEFT_SIDE:
			if(entrance){
				ELEMENTS_set_flag(FLAG_SUB_ANNE_TAKE_CYLINDER_OUR_ROCKET_MULTI, TRUE);
			}
			if((global.color == BLUE && modules == OUR_ELEMENT) || (global.color == YELLOW && modules == ADV_ELEMENT)){
				state = try_going(800, 300, state, TAKE_SIDE_MODULE_FROM_LEFT_SIDE, ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_BRAKE);
			}else{
				state = try_going(950, 2650, state, TAKE_SIDE_MODULE_FROM_LEFT_SIDE, ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_BRAKE);
			}
			if(ON_LEAVE()){
				if(side == LEFT)
					ACT_push_order( ACT_POMPE_ELEVATOR_LEFT , ACT_POMPE_NORMAL );
				else
					ACT_push_order( ACT_POMPE_ELEVATOR_RIGHT, ACT_POMPE_NORMAL );
			}
			break;

		case TAKE_SIDE_MODULE_FROM_LEFT_SIDE:
			if(side == LEFT){
				if((global.color == BLUE && modules == OUR_ELEMENT) || (global.color == YELLOW && modules == ADV_ELEMENT)){
					state = try_going(1050, 380, state, STORAGE_LEFT, ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
				}else{
					state = try_going(1050, 1620, state, STORAGE_LEFT, ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
				}
			}
			else{
				if((global.color == BLUE && modules == OUR_ELEMENT) || (global.color == YELLOW && modules == ADV_ELEMENT)){
					state = try_going(1030, 500, state, STORAGE_RIGHT, ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
				}else{
					state = try_going(1030, 2500, state, STORAGE_RIGHT, ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
				}
			}
			break;

		case STORAGE_LEFT:
			if(entrance){
				if(global.color == BLUE){
					STOCKS_addModule(MODULE_BLUE, MODULE_STOCK_LEFT, STOCK_POS_ELEVATOR);
				}else{
					STOCKS_addModule(MODULE_YELLOW, MODULE_STOCK_LEFT, STOCK_POS_ELEVATOR);
				}
			}

			state=check_sub_action_result(sub_act_anne_mae_store_modules(TRUE),state,DONE,ERROR);

			if(state==DONE){
				if(modules == OUR_ELEMENT)
					ELEMENTS_set_flag(FLAG_OUR_MULTICOLOR_SIDE_IS_TAKEN, TRUE);	// Flag element
				else
					ELEMENTS_set_flag(FLAG_ADV_MULTICOLOR_SIDE_IS_TAKEN, TRUE);	// Flag element
			}
			if(ON_LEAVE()){
				ELEMENTS_set_flag(FLAG_SUB_ANNE_TAKE_CYLINDER_OUR_CENTER, FALSE);	// Flag subaction
				//set_sub_act_enable(SUB_ANNE_DEPOSE_MODULES, TRUE);   // Activation de la dépose
			}
			break;

		case STORAGE_RIGHT:
			if(entrance){
				if(global.color == BLUE){
					STOCKS_addModule(MODULE_BLUE, MODULE_STOCK_RIGHT, STOCK_POS_ELEVATOR);
				}else{
					STOCKS_addModule(MODULE_YELLOW, MODULE_STOCK_RIGHT, STOCK_POS_ELEVATOR);
				}
			}
			state=check_sub_action_result(sub_act_anne_mae_store_modules(TRUE),state,DONE,ERROR);

			if(state==DONE){
				if(modules == OUR_ELEMENT)
					ELEMENTS_set_flag(FLAG_OUR_MULTICOLOR_SIDE_IS_TAKEN, TRUE);	// Flag element
				else
					ELEMENTS_set_flag(FLAG_ADV_MULTICOLOR_SIDE_IS_TAKEN, TRUE);	// Flag element
			}
			if(ON_LEAVE()){
				ELEMENTS_set_flag(FLAG_SUB_ANNE_TAKE_CYLINDER_OUR_CENTER, FALSE);	// Flag subaction
				//set_sub_act_enable(SUB_ANNE_DEPOSE_MODULES, TRUE);   // Activation de la dépose
			}
			break;

		case ERROR:
			RESET_MAE();
			ELEMENTS_set_flag(FLAG_SUB_ANNE_TAKE_CYLINDER_OUR_ROCKET_UNI, FALSE);
			ELEMENTS_set_flag(FLAG_SUB_ANNE_TAKE_CYLINDER_OUR_ROCKET_MULTI, FALSE);
			ELEMENTS_set_flag(FLAG_SUB_ANNE_TAKE_CYLINDER_OUR_CENTER, FALSE);
			on_turning_point();
			return NOT_HANDLED;
			break;

		case DONE:
			RESET_MAE();
			ELEMENTS_set_flag(FLAG_SUB_ANNE_TAKE_CYLINDER_OUR_ROCKET_UNI, FALSE);
			ELEMENTS_set_flag(FLAG_SUB_ANNE_TAKE_CYLINDER_OUR_ROCKET_MULTI, FALSE);
			ELEMENTS_set_flag(FLAG_SUB_ANNE_TAKE_CYLINDER_OUR_CENTER, FALSE);
			on_turning_point();
			return END_OK;
			break;

		default:
			if(entrance)
			{
				error_printf("default case in sub_anne_prise_modules_centre\n");
			}
			break;
	}

	return IN_PROGRESS;
}


error_e sub_anne_prise_module_base_centre(ELEMENTS_property_e modules, ELEMENTS_side_e side){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_ANNE_MODULE_BASE_CENTER,
			INIT,

			GET_IN_CLOSE_ADV_ZONE,
			GET_IN_FAR_ADV_ZONE,
			GET_IN_ASTAR,
			GET_IN_FROM_START_ZONE,
			AVOID_MODULE_START,

			GO_TO_CLOSE_POINT,
			GO_TO_MID_POINT,

			GO_TO_MODULE_FROM_MIDDLE,
			GO_TO_MODULE_FROM_DOWN,
			TAKE_BASE_MODULE,

			STORAGE_LEFT,
			STORAGE_RIGHT,
			GET_OUT,
			GET_OUT_ERROR,

			ERROR,
			DONE
		);

	const displacement_t curve_far_zone_yellow[2] = {(displacement_t){(GEOMETRY_point_t){1000, 2400}, FAST},
												     (displacement_t){(GEOMETRY_point_t){950, 2000}, FAST},
												    };

	const displacement_t curve_far_zone_blue[2] = {(displacement_t){(GEOMETRY_point_t){1000, 600}, FAST},
												   (displacement_t){(GEOMETRY_point_t){950, 1000}, FAST},
												  };

	const displacement_t curve_avoid_module_start_yellow[2] = {(displacement_t){(GEOMETRY_point_t){600, 1700}, FAST},
																 (displacement_t){(GEOMETRY_point_t){900, 2000}, FAST},
																};

	const displacement_t curve_avoid_module_start_blue[2] = {(displacement_t){(GEOMETRY_point_t){600, 1300}, FAST},
															 (displacement_t){(GEOMETRY_point_t){900, 1000}, FAST},
															};


	switch(state){
		case INIT:
			/*if(IHM_switchs_get(SWITCH_DISABLE_MODULE_RIGHT) && IHM_switchs_get(SWITCH_DISABLE_MODULE_LEFT)){
				state = ERROR; // Actionneurs désactivés
			}else*/ if (ELEMENTS_get_flag(FLAG_OUR_MULTICOLOR_NEAR_DEPOSE_IS_TAKEN)){
					state = DONE; // Il n'y a plus rien à faire
			}
			else
			{
				// Si on prend les modules du coté BLUE		(utiliser i_am_in_square et pas i_am_in_square_color)
				if((global.color == BLUE && modules == OUR_ELEMENT) || (global.color == YELLOW && modules == ADV_ELEMENT)){
					if(i_am_in_square(0, 550, 800, 1200) && !ELEMENTS_get_flag(FLAG_OUR_MULTICOLOR_START_IS_TAKEN)){
						state = AVOID_MODULE_START;
					}else if(i_am_in_square(0, 550, 800, 1200) && ELEMENTS_get_flag(FLAG_OUR_MULTICOLOR_START_IS_TAKEN)){
						state = GET_IN_FROM_START_ZONE;
					}else if(i_am_in_square(450, 2000, 0, 900)){
						state = GO_TO_CLOSE_POINT;
					}else if(i_am_in_square(0, 1000, 1200, 2100)){
						state = GO_TO_MID_POINT;
					}else if(i_am_in_square(700, 1400, 2100, 2600)){ // 2100 -> 1900 sysmétrie bleu ou jaune
						state = GET_IN_CLOSE_ADV_ZONE;
					}else if(i_am_in_square(700, 1400, 2600, 2800)){
						state = GET_IN_FAR_ADV_ZONE;
					}else{
						state = GET_IN_ASTAR;
					}
				}
				else	// Si on prend les modules du coté YELLOW	(utiliser i_am_in_square et pas i_am_in_square_color)
				{
					if(i_am_in_square(0, 550, 1800, 2200) && ELEMENTS_get_flag(FLAG_OUR_MULTICOLOR_START_IS_TAKEN)){
						state = AVOID_MODULE_START;
					}else if(i_am_in_square(0, 550, 1800, 2200) && !ELEMENTS_get_flag(FLAG_OUR_MULTICOLOR_START_IS_TAKEN)){
						state = GET_IN_FROM_START_ZONE;
					}else if(i_am_in_square(450, 2000, 2100, 3000)){
						state = GO_TO_CLOSE_POINT;
					}else if(i_am_in_square(0, 1000, 900, 1800)){
						state = GO_TO_MID_POINT;
					}else if(i_am_in_square(700, 1400, 400, 1100)){  // 1100 -> 900 sysmétrie bleu ou jaune
						state = GET_IN_CLOSE_ADV_ZONE;
					}else if(i_am_in_square(700, 1400, 200, 400)){
						state = GET_IN_FAR_ADV_ZONE;
					}else{
						state = GET_IN_ASTAR;
					}
				}

				// On lève le flag de subaction
				ELEMENTS_set_flag(FLAG_SUB_ANNE_TAKE_CYLINDER_OUR_CENTER, TRUE);
			}
			break;

		case AVOID_MODULE_START:
			if((global.color == BLUE && modules == OUR_ELEMENT) || (global.color == YELLOW && modules == ADV_ELEMENT)){
				state = try_going_multipoint(curve_avoid_module_start_blue, 2, state, GO_TO_CLOSE_POINT, ERROR, ANY_WAY, NO_DODGE_AND_WAIT,END_AT_BRAKE);
			}else{
				state = try_going_multipoint(curve_avoid_module_start_yellow, 2, state, GO_TO_CLOSE_POINT, ERROR, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_BRAKE);
			}
			break;

		case GET_IN_FROM_START_ZONE:
			if((global.color == BLUE && modules == OUR_ELEMENT) || (global.color == YELLOW && modules == ADV_ELEMENT)){
				state = try_going(700, 1000, state, GO_TO_MID_POINT, GET_IN_ASTAR, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_BRAKE);
			}else{
				state = try_going(700, 2000, state, GO_TO_MID_POINT, GET_IN_ASTAR, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_BRAKE);
			}
			break;

		case GET_IN_CLOSE_ADV_ZONE:
			if((global.color == BLUE && modules == OUR_ELEMENT) || (global.color == YELLOW && modules == ADV_ELEMENT)){
				state = try_going(900, 2000, state, GO_TO_MID_POINT, GET_IN_ASTAR, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_BRAKE);
			}else{
				state = try_going(900, 1000, state, GO_TO_MID_POINT, GET_IN_ASTAR, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_BRAKE);
			}
			break;

		case GET_IN_FAR_ADV_ZONE:
			if((global.color == BLUE && modules == OUR_ELEMENT) || (global.color == YELLOW && modules == ADV_ELEMENT)){
				state = try_going_multipoint(curve_far_zone_yellow, 2, state, GO_TO_MID_POINT, GET_IN_ASTAR, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_BRAKE);
			}else{
				state = try_going_multipoint(curve_far_zone_blue, 2, state, GO_TO_MID_POINT, GET_IN_ASTAR, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_BRAKE);
			}
			break;

		case GET_IN_ASTAR:
			if((global.color == BLUE && modules == OUR_ELEMENT) || (global.color == YELLOW && modules == ADV_ELEMENT)){
				state = ASTAR_try_going(1000, 1135, state, GO_TO_MID_POINT, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_BRAKE);
			}else{
				state = ASTAR_try_going(1000, 2865, state, GO_TO_MID_POINT, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_BRAKE);
			}
			break;

		case GO_TO_MID_POINT:
			if((global.color == BLUE && modules == OUR_ELEMENT) || (global.color == YELLOW && modules == ADV_ELEMENT)){
				state = try_going(1000, 950, state, GO_TO_CLOSE_POINT, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_BRAKE);
			}
			else{
				state = try_going(1000, 2050, state, GO_TO_CLOSE_POINT, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_BRAKE);
			}
			break;

		case GO_TO_CLOSE_POINT:
			if((global.color == BLUE && modules == OUR_ELEMENT) || (global.color == YELLOW && modules == ADV_ELEMENT)){
				state = try_going(1275, 675, state, TAKE_BASE_MODULE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			}
			else{
				state = try_going(1275, 2325, state, TAKE_BASE_MODULE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			}
			if(ON_LEAVE()){
				if(side == LEFT)
					ACT_push_order( ACT_POMPE_ELEVATOR_LEFT , ACT_POMPE_NORMAL );
				else
					ACT_push_order( ACT_POMPE_ELEVATOR_RIGHT, ACT_POMPE_NORMAL );
			}
			break;

		case TAKE_BASE_MODULE:
			if(side == LEFT || side == NO_SIDE){
				if((global.color == BLUE && modules == OUR_ELEMENT) || (global.color == YELLOW && modules == ADV_ELEMENT)){
					state = try_going(1380, 785, state, STORAGE_LEFT, GET_OUT_ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
				}else{
					state = try_going(1380, 2215, state, STORAGE_LEFT, GET_OUT_ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
				}
			}
			else{
				if((global.color == BLUE && modules == OUR_ELEMENT) || (global.color == YELLOW && modules == ADV_ELEMENT)){
					state = try_going(1310, 810, state, STORAGE_RIGHT, GET_OUT_ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
				}else{
					state = try_going(1310, 2190, state, STORAGE_RIGHT, GET_OUT_ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
				}
			}
			break;

		case STORAGE_LEFT:
			if(entrance){
				if(global.color == BLUE){
					STOCKS_addModule(MODULE_BLUE, MODULE_STOCK_LEFT, STOCK_POS_ELEVATOR);
				}else{
					STOCKS_addModule(MODULE_YELLOW, MODULE_STOCK_LEFT, STOCK_POS_ELEVATOR);
				}
			}

			state=check_sub_action_result(sub_act_anne_mae_store_modules(TRUE), state, GET_OUT, ERROR);

			if(state==GET_OUT){
				if(modules == OUR_ELEMENT)
					ELEMENTS_set_flag(FLAG_OUR_MULTICOLOR_NEAR_DEPOSE_IS_TAKEN, TRUE);	// Flag element
				else{
					ELEMENTS_set_flag(FLAG_ADV_MULTICOLOR_NEAR_DEPOSE_IS_TAKEN, TRUE);	// Flag element
				}
			}
			if(ON_LEAVE()){
				ELEMENTS_set_flag(FLAG_SUB_ANNE_TAKE_CYLINDER_OUR_CENTER, FALSE);	// Flag subaction
				//set_sub_act_enable(SUB_ANNE_DEPOSE_MODULES, TRUE);   // Activation de la dépose
			}
		break;

		case STORAGE_RIGHT:
			if(entrance){
				if(global.color == BLUE){
					STOCKS_addModule(MODULE_BLUE, MODULE_STOCK_RIGHT, STOCK_POS_ELEVATOR);
				}else{
					STOCKS_addModule(MODULE_YELLOW, MODULE_STOCK_RIGHT, STOCK_POS_ELEVATOR);
				}
			}

#warning "Cette subaction n\'existe pas !"
			//state=check_sub_action_result(sub_act_annne_mae_store_modules(MODULE_STOCK_RIGHT, TRUE), state, GET_OUT, ERROR);

			if(state==GET_OUT){
				if(modules == OUR_ELEMENT)
					ELEMENTS_set_flag(FLAG_OUR_MULTICOLOR_NEAR_DEPOSE_IS_TAKEN, TRUE);	// Flag element
				else
					ELEMENTS_set_flag(FLAG_ADV_MULTICOLOR_NEAR_DEPOSE_IS_TAKEN, TRUE);	// Flag element
			}
			if(ON_LEAVE()){
				ELEMENTS_set_flag(FLAG_SUB_ANNE_TAKE_CYLINDER_OUR_CENTER, FALSE);	// Flag subaction
				//set_sub_act_enable(SUB_ANNE_DEPOSE_MODULES, TRUE);   // Activation de la dépose
			}
			break;

		case GET_OUT:
			if((global.color == BLUE && modules == OUR_ELEMENT) || (global.color == YELLOW && modules == ADV_ELEMENT)){
				state = try_going(1260, 700, state, DONE, GET_OUT_ERROR, FAST, BACKWARD, NO_DODGE_AND_WAIT, END_AT_BRAKE);
			}else{
				state = try_going(1260, 2300, state, DONE, GET_OUT_ERROR, FAST, BACKWARD, NO_DODGE_AND_WAIT, END_AT_BRAKE);
			}
			if(ON_LEAVE()){
				ELEMENTS_set_flag(FLAG_SUB_ANNE_TAKE_CYLINDER_OUR_CENTER, FALSE);	// Flag subaction
				//set_sub_act_enable(SUB_ANNE_DEPOSE_MODULES, TRUE); // Activation de la dépose
			}
			break;

		case GET_OUT_ERROR:
			if(side == LEFT || side == NO_SIDE){
				if((global.color == BLUE && modules == OUR_ELEMENT) || (global.color == YELLOW && modules == ADV_ELEMENT)){
					state = try_going(1380, 785, state, GET_OUT, GET_OUT, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
				}else{
					state = try_going(1380, 2215, state, GET_OUT, GET_OUT, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
				}
			}
			else{
				if((global.color == BLUE && modules == OUR_ELEMENT) || (global.color == YELLOW && modules == ADV_ELEMENT)){
					state = try_going(1310, 810, state, GET_OUT, GET_OUT, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
				}else{
					state = try_going(1310, 2190, state, GET_OUT, GET_OUT, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
				}
			}
			break;

		case ERROR:
			RESET_MAE();
			ELEMENTS_set_flag(FLAG_SUB_ANNE_TAKE_CYLINDER_OUR_ROCKET_UNI, FALSE);
			ELEMENTS_set_flag(FLAG_SUB_ANNE_TAKE_CYLINDER_OUR_ROCKET_MULTI, FALSE);
			ELEMENTS_set_flag(FLAG_SUB_ANNE_TAKE_CYLINDER_OUR_CENTER, FALSE);
			on_turning_point();
			return NOT_HANDLED;
			break;

		case DONE:
			RESET_MAE();
			ELEMENTS_set_flag(FLAG_SUB_ANNE_TAKE_CYLINDER_OUR_ROCKET_UNI, FALSE);
			ELEMENTS_set_flag(FLAG_SUB_ANNE_TAKE_CYLINDER_OUR_ROCKET_MULTI, FALSE);
			ELEMENTS_set_flag(FLAG_SUB_ANNE_TAKE_CYLINDER_OUR_CENTER, FALSE);
			on_turning_point();
			return END_OK;
			break;

		default:
			if(entrance)
			{
				error_printf("default case in sub_anne_prise_modules_centre\n");
			}
			break;
	}

	return IN_PROGRESS;
}


error_e sub_anne_prise_module_unicolor_north(ELEMENTS_side_e side){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_ANNE_MODULE_UNICOLOR_NORTH,
			INIT,

			GET_IN_DIRECT_LEFT,
			GET_IN_DIRECT_RIGHT,
			GET_IN_MIDDLE,
			GET_IN_CLOSE_ADV_ZONE,
			GET_IN_FAR_ADV_ZONE,
			GET_IN_ASTAR_LEFT,
			GET_IN_ASTAR_RIGHT,

			MOVE_BACK,

			TAKE_MODULE_LEFT,
			TAKE_MODULE_RIGHT,
			STORAGE_LEFT,
			STORAGE_RIGHT,

			GET_OUT_RIGHT,
			MOVE_BACK_RIGHT,

			ERROR,
			DONE
		);

	switch(state){
		case INIT:
			/*if(IHM_switchs_get(SWITCH_DISABLE_MODULE_RIGHT) && IHM_switchs_get(SWITCH_DISABLE_MODULE_LEFT)){
				state = ERROR; // Actionneurs désactivés
			}else*/ if (ELEMENTS_get_flag(FLAG_OUR_UNICOLOR_NORTH_IS_TAKEN)){
				state = DONE; // Il n'y a plus rien à faire
			}else if (ELEMENTS_get_flag(FLAG_SUB_HARRY_TAKE_CYLINDER_NORTH_UNI)){
				state = ERROR; // Harry est en train de le prendre
			}
			else
			{

				if(i_am_in_square_color(600, 1400, 100, 600) || i_am_in_square_color(800, 1400, 600, 1000)){
					if(side == LEFT){
						state = GET_IN_DIRECT_LEFT;
					}else{
						state = GET_IN_DIRECT_RIGHT;
					}
				}else if(i_am_in_square_color(200, 1000, 1000, 2000)){
					state = GET_IN_MIDDLE;
				}else if(i_am_in_square_color(600, 1400, 2400, 2900)){
					state = GET_IN_FAR_ADV_ZONE;
				}else if(i_am_in_square_color(800, 1400, 2000, 2400)){
					state = GET_IN_CLOSE_ADV_ZONE;
				}else{
					if(side == LEFT){
						state = GET_IN_ASTAR_LEFT;
					}else{
						state = GET_IN_ASTAR_RIGHT;
					}
				}

				// On lève le flag de subaction
				ELEMENTS_set_flag(FLAG_SUB_ANNE_TAKE_CYLINDER_NORTH_UNI, TRUE);
			}
			break;

		case GET_IN_DIRECT_LEFT:	// Point d'accès pour stockage à gauche
			state = try_going(850, COLOR_Y(400), state, TAKE_MODULE_LEFT, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case GET_IN_DIRECT_RIGHT:	// Point d'accès pour stockage à droite
			state = try_going(800, COLOR_Y(450), state, TAKE_MODULE_RIGHT, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case GET_IN_MIDDLE:
			if(side == LEFT){
				state = try_going(1050, COLOR_Y(900), state, GET_IN_DIRECT_LEFT, ERROR, FAST, ANY_WAY, DODGE_AND_WAIT, END_AT_BRAKE);
			}else{
				state = try_going(1050, COLOR_Y(900), state, GET_IN_DIRECT_RIGHT, ERROR, FAST, ANY_WAY, DODGE_AND_WAIT, END_AT_BRAKE);
			}
			break;

		case GET_IN_CLOSE_ADV_ZONE:
			state = try_going(800, COLOR_Y(1800), state, GET_IN_MIDDLE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_BRAKE);
			break;

		case GET_IN_FAR_ADV_ZONE:
			state = try_going(1000, COLOR_Y(2300), state, GET_IN_CLOSE_ADV_ZONE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_BRAKE);
			break;

		case GET_IN_ASTAR_LEFT:	// Point d'accès pour stockage à gauche
			state = ASTAR_try_going(850, COLOR_Y(400), state, TAKE_MODULE_LEFT, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case GET_IN_ASTAR_RIGHT:	// Point d'accès pour stockage à droite
			state = ASTAR_try_going(800, COLOR_Y(450), state, TAKE_MODULE_RIGHT, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case TAKE_MODULE_LEFT:
			if(entrance){
				ACT_push_order(ACT_POMPE_SLIDER_LEFT, ACT_POMPE_NORMAL);
			}
			//state = try_going(650, COLOR_Y(300), state, STORAGE_LEFT, ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			state = try_rush(450, COLOR_Y(300), state , MOVE_BACK, ERROR, FORWARD, NO_DODGE_AND_NO_WAIT, TRUE);
			break;

		case MOVE_BACK:
			state = try_advance(NULL, entrance, 150, MOVE_BACK, STORAGE_LEFT, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case STORAGE_LEFT:
			if(entrance){
				if(global.color == BLUE){
					STOCKS_addModule(MODULE_BLUE, MODULE_STOCK_LEFT, STOCK_POS_ELEVATOR);
				}else{
					STOCKS_addModule(MODULE_YELLOW, MODULE_STOCK_LEFT, STOCK_POS_ELEVATOR);
				}
				ELEMENTS_set_flag(FLAG_OUR_UNICOLOR_NORTH_IS_TAKEN, TRUE);	// Flag element
				//set_sub_act_enable(SUB_ANNE_DEPOSE_MODULES, TRUE);   // Activation de la dépose
			}
			state = check_sub_action_result(sub_act_anne_mae_store_modules(TRUE), state, DONE, ERROR);
			break;


		case TAKE_MODULE_RIGHT:
			if(entrance){
				ACT_push_order(ACT_POMPE_SLIDER_RIGHT, ACT_POMPE_NORMAL);
			}
			state = try_going(730, COLOR_Y(280), state, STORAGE_RIGHT, ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;


		case STORAGE_RIGHT:
			if(entrance){
				if(global.color == BLUE){
					STOCKS_addModule(MODULE_BLUE, MODULE_STOCK_RIGHT, STOCK_POS_ELEVATOR);
				}else{
					STOCKS_addModule(MODULE_YELLOW, MODULE_STOCK_RIGHT, STOCK_POS_ELEVATOR);
				}
				ELEMENTS_set_flag(FLAG_OUR_UNICOLOR_NORTH_IS_TAKEN, TRUE);	// Flag element
				//set_sub_act_enable(SUB_ANNE_DEPOSE_MODULES, TRUE);   // Activation de la dépose
			}
			state = check_sub_action_result(sub_act_anne_mae_store_modules(TRUE), state, GET_OUT_RIGHT, ERROR);
			break;

		case GET_OUT_RIGHT:
			state = try_going(800, COLOR_Y(450), state, DONE, MOVE_BACK_RIGHT, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_BRAKE);
			break;

		case MOVE_BACK_RIGHT:
			state = try_going(730, COLOR_Y(280), state, GET_OUT_RIGHT, GET_OUT_RIGHT, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case ERROR:
			RESET_MAE();
			ELEMENTS_set_flag(FLAG_SUB_ANNE_TAKE_CYLINDER_NORTH_UNI, FALSE); // Flag subaction
			on_turning_point();
			return NOT_HANDLED;
			break;

		case DONE: // Pas de GET_OUT dans le cas LEFT, ca doit pouvoir passer
			RESET_MAE();
			ELEMENTS_set_flag(FLAG_SUB_ANNE_TAKE_CYLINDER_NORTH_UNI, FALSE); // Flag subaction
			on_turning_point();
			return END_OK;
			break;

		default:
			if(entrance)
			{
				error_printf("default case in sub_anne_prise_module_unicolor_north\n");
			}
			break;
	}

	return IN_PROGRESS;
}



error_e sub_anne_prise_module_unicolor_south(ELEMENTS_side_e side){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_ANNE_MODULE_UNICOLOR_SOUTH,
			INIT,

			GET_IN_DIRECT_LEFT,
			GET_IN_DIRECT_RIGHT,
			GET_IN_MIDDLE,
			GET_IN_CLOSE_ADV_ZONE,
			GET_IN_FAR_ADV_ZONE,
			GET_IN_ASTAR_LEFT,
			GET_IN_ASTAR_RIGHT,

			TAKE_MODULE_LEFT,
			TAKE_MODULE_RIGHT,
			STORAGE_LEFT,
			STORAGE_RIGHT,

			GET_OUT_RIGHT,
			MOVE_BACK_RIGHT,

			ERROR,
			DONE
		);

	switch(state){
		case INIT:
			/*if(IHM_switchs_get(SWITCH_DISABLE_MODULE_RIGHT) && IHM_switchs_get(SWITCH_DISABLE_MODULE_LEFT)){
				state = ERROR; // Actionneurs désactivés
			}else*/ if (ELEMENTS_get_flag(FLAG_OUR_UNICOLOR_SOUTH_IS_TAKEN)){
				state = DONE; // Il n'y a plus rien à faire
			}else if (ELEMENTS_get_flag(FLAG_SUB_HARRY_TAKE_CYLINDER_SOUTH_UNI)){
				state = ERROR; // Harry est en train de le prendre
			}
			else
			{

				if(i_am_in_square_color(700, 1600, 200, 900)){
					if(side == LEFT){
						state = GET_IN_DIRECT_LEFT;
					}else{
						state = GET_IN_DIRECT_RIGHT;
					}
				}else if(i_am_in_square_color(200, 1000, 900, 2100)){
					state = GET_IN_MIDDLE;
				}else if(i_am_in_square_color(600, 1400, 2400, 2900)){
					state = GET_IN_FAR_ADV_ZONE;
				}else if(i_am_in_square_color(800, 1400, 2000, 2400)){
					state = GET_IN_CLOSE_ADV_ZONE;
				}else{
					if(side == LEFT){
						state = GET_IN_ASTAR_LEFT;
					}else{
						state = GET_IN_ASTAR_RIGHT;
					}
				}

				// On lève le flag de subaction
				ELEMENTS_set_flag(FLAG_SUB_ANNE_TAKE_CYLINDER_SOUTH_UNI, TRUE);
			}
			break;

		case GET_IN_DIRECT_LEFT:	// Point d'accès pour stockage à gauche
			state = try_going(1600, COLOR_Y(700), state, TAKE_MODULE_LEFT, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_BRAKE);
			break;

		case GET_IN_DIRECT_RIGHT:	// Point d'accès pour stockage à droite
			state = try_going(1600, COLOR_Y(680), state, TAKE_MODULE_RIGHT, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_BRAKE);
			break;

		case GET_IN_MIDDLE:{
			const displacement_t curve_middle[2] = {(displacement_t){(GEOMETRY_point_t){1000, COLOR_Y(800)}, FAST},
													(displacement_t){(GEOMETRY_point_t){1320, COLOR_Y(630)}, SLOW},
													};
			if(side == LEFT){
				state = try_going_multipoint(curve_middle, 2, state, GET_IN_DIRECT_LEFT, ERROR, ANY_WAY, DODGE_AND_WAIT, END_AT_BRAKE);
			}else{
				state = try_going_multipoint(curve_middle, 2, state, GET_IN_DIRECT_RIGHT, ERROR, ANY_WAY, DODGE_AND_WAIT, END_AT_BRAKE);
			}
		}break;

		case GET_IN_CLOSE_ADV_ZONE:
			state = try_going(800, COLOR_Y(1800), state, GET_IN_MIDDLE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_BRAKE);
			break;

		case GET_IN_FAR_ADV_ZONE:
			state = try_going(1000, COLOR_Y(2300), state, GET_IN_CLOSE_ADV_ZONE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_BRAKE);
			break;

		case GET_IN_ASTAR_LEFT:	// Point d'accès pour stockage à gauche
			state = ASTAR_try_going(1600, COLOR_Y(700), state, TAKE_MODULE_LEFT, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case GET_IN_ASTAR_RIGHT:	// Point d'accès pour stockage à droite
			state = ASTAR_try_going(1600, COLOR_Y(680), state, TAKE_MODULE_RIGHT, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case TAKE_MODULE_LEFT:
			if(entrance){
				ACT_push_order(ACT_POMPE_SLIDER_LEFT, ACT_POMPE_NORMAL);
			}
			state = try_going(1800, COLOR_Y(730), state, STORAGE_LEFT, ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case STORAGE_LEFT:
			if(entrance){
				if(global.color == BLUE){
					STOCKS_addModule(MODULE_BLUE, MODULE_STOCK_LEFT, STOCK_POS_ELEVATOR);
				}else{
					STOCKS_addModule(MODULE_YELLOW, MODULE_STOCK_LEFT, STOCK_POS_ELEVATOR);
				}
				ELEMENTS_set_flag(FLAG_OUR_UNICOLOR_SOUTH_IS_TAKEN, TRUE);	// Flag element
				//set_sub_act_enable(SUB_ANNE_DEPOSE_MODULES, TRUE);   // Activation de la dépose
			}
			state = check_sub_action_result(sub_act_anne_mae_store_modules(TRUE), state, DONE, ERROR);
			break;


		case TAKE_MODULE_RIGHT:
			if(entrance){
				ACT_push_order(ACT_POMPE_SLIDER_RIGHT, ACT_POMPE_NORMAL);
			}
			state = try_going(1775, COLOR_Y(800), state, STORAGE_RIGHT, ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;


		case STORAGE_RIGHT:
			if(entrance){
				if(global.color == BLUE){
					STOCKS_addModule(MODULE_BLUE, MODULE_STOCK_RIGHT, STOCK_POS_ELEVATOR);
				}else{
					STOCKS_addModule(MODULE_YELLOW, MODULE_STOCK_RIGHT, STOCK_POS_ELEVATOR);
				}
				ELEMENTS_set_flag(FLAG_OUR_UNICOLOR_SOUTH_IS_TAKEN, TRUE);	// Flag element
				//set_sub_act_enable(SUB_ANNE_DEPOSE_MODULES, TRUE);   // Activation de la dépose
			}
			state = check_sub_action_result(sub_act_anne_mae_store_modules(TRUE), state, DONE, ERROR);
			break;

		case ERROR:
			RESET_MAE();
			ELEMENTS_set_flag(FLAG_SUB_ANNE_TAKE_CYLINDER_SOUTH_UNI, FALSE); // Flag subaction
			on_turning_point();
			return NOT_HANDLED;
			break;

		case DONE: // Pas de GET_OUT dans le cas LEFT, ca doit pouvoir passer
			RESET_MAE();
			ELEMENTS_set_flag(FLAG_SUB_ANNE_TAKE_CYLINDER_SOUTH_UNI, FALSE); // Flag subaction
			on_turning_point();
			return END_OK;
			break;

		default:
			if(entrance)
			{
				error_printf("default case in sub_anne_prise_module_unicolor_south\n");
			}
			break;
	}

	return IN_PROGRESS;
}

#define OFFSET_BEST_SLIDER_APPROACH	(-10)	//[mm] décalage souhaitable pour une meilleure prise...

error_e sub_anne_fusee_color(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_ANNE_FUSEE_COLOR,
		INIT,
		GET_IN_DIRECT,
		GET_IN_ADV_SQUARE,
		GET_IN_OUR_SQUARE,
		PATHFIND,
		GO_TO_START_POINT,
		TAKE_ROCKET,
		GET_OUT,
		GET_OUT_ERROR,
		AVANCE,
		AVANCE_ERROR,
		ERROR,
		DONE
		);

	switch(state){
		case INIT:
			/*if(IHM_switchs_get(SWITCH_DISABLE_MODULE_RIGHT) && IHM_switchs_get(SWITCH_DISABLE_MODULE_LEFT)){
				state = ERROR; // Actionneurs désactivés
			}else*/ if(ROCKETS_isEmpty(MODULE_ROCKET_MONO_OUR_SIDE)){
				state = DONE; // La fusée a déjà été prise
			}else if(ELEMENTS_get_flag(FLAG_SUB_HARRY_TAKE_CYLINDER_OUR_ROCKET_UNI)){
				state = ERROR;
			}else{
				if (i_am_in_square_color(100, 1100, 850, 2100)){
					state = GET_IN_DIRECT;
				}else if(i_am_in_square_color(800, 1400, 300, 900)){
					state = GET_IN_OUR_SQUARE;
				}else if (i_am_in_square_color(800, 1400, 2100, 2700)){
					state = GET_IN_ADV_SQUARE;
				}else{
					state = PATHFIND;
				}

				// On lève le flag de subaction
				ELEMENTS_set_flag(FLAG_SUB_ANNE_TAKE_CYLINDER_OUR_ROCKET_UNI, TRUE);
			}
			break;

		case GET_IN_OUR_SQUARE:
			state = try_going(800, COLOR_Y(1000), state, GET_IN_DIRECT, ERROR, FAST, ANY_WAY, DODGE_AND_WAIT, END_AT_BRAKE);
			break;

		case GET_IN_ADV_SQUARE:
			state = try_going(800, COLOR_Y(2000), state, GET_IN_DIRECT, ERROR, FAST, ANY_WAY, DODGE_AND_WAIT, END_AT_BRAKE);
			break;

		case GET_IN_DIRECT:
				state = try_going(400, COLOR_Y(1150)+OFFSET_BEST_SLIDER_APPROACH, state, GO_TO_START_POINT, ERROR, FAST, ANY_WAY, i_am_in_square_color(0,500,800,1300)?NO_AVOIDANCE:NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case PATHFIND:
			state = ASTAR_try_going(400, COLOR_Y(1150)+OFFSET_BEST_SLIDER_APPROACH, state, GO_TO_START_POINT, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case GO_TO_START_POINT: //ajuster la distance fusée
			state = try_going(255, COLOR_Y(1150)+OFFSET_BEST_SLIDER_APPROACH, state, TAKE_ROCKET, GET_OUT_ERROR, FAST, FORWARD, NO_AVOIDANCE, END_AT_LAST_POINT);
			break;

		case TAKE_ROCKET: // Execution des ordres actionneurs
			state=check_sub_action_result(sub_act_anne_take_rocket_down_to_top(MODULE_ROCKET_MONO_OUR_SIDE, 4),state,GET_OUT,GET_OUT_ERROR);
			break;

		case GET_OUT:
			if(entrance){
				//set_sub_act_enable(SUB__DEPOSE_MODULES, TRUE);
			}
			state=try_going(400, global.pos.y, state, DONE, AVANCE, FAST, BACKWARD, NO_AVOIDANCE, END_AT_BRAKE);
			break;

		case GET_OUT_ERROR:
			state=try_going(400, global.pos.y, state, ERROR, AVANCE_ERROR, FAST, BACKWARD, NO_DODGE_AND_NO_WAIT, END_AT_BRAKE);
			break;

		case AVANCE:
			state=try_going(220, global.pos.y, state, GET_OUT, GET_OUT, FAST, FORWARD, NO_AVOIDANCE, END_AT_BRAKE);
			break;

		case AVANCE_ERROR:
			state=try_going(220, global.pos.y, state, GET_OUT_ERROR, GET_OUT_ERROR, FAST, FORWARD, NO_DODGE_AND_NO_WAIT, END_AT_BRAKE);
			break;

		case ERROR:
			RESET_MAE();
			on_turning_point();
			ELEMENTS_set_flag(FLAG_SUB_ANNE_TAKE_CYLINDER_OUR_ROCKET_UNI, FALSE);
			return NOT_HANDLED;
			break;

		case DONE:
			RESET_MAE();
			on_turning_point();
			ELEMENTS_set_flag(FLAG_SUB_ANNE_TAKE_CYLINDER_OUR_ROCKET_UNI, FALSE);
			return END_OK;
			break;

		default:
			if(entrance)
				debug_printf("default case in sub_anne_fusee_color\n");
			break;
	}

	return IN_PROGRESS;
}


#define OFFSET_POS_SENSOR_IN_ROBOT	65

error_e sub_anne_fusee_multicolor(ELEMENTS_property_e rocket){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_ANNE_FUSEE_MULTICOLOR,
			INIT,
			ALL_THE_GET_IN,
			GET_IN_MIDDLE_SQUARE,
			GET_IN_OUR_SQUARE,
			GET_IN_PATHFIND,

			RUSH_TO_MEASURE_Y,
			BACK_AFTER_RUSH,
			BACK_AFTER_FAILING_RUSH,
			SCANNING_TRAJECTORY,
			WAIT_RESULT,

			GET_IN_FRONT_OF_ONE_ON_TWO,
			TURN_TO_POS,
			GET_IN_FRONT_OF_TWO_ON_TWO,
			ACTION,

			GET_OUT,
			ADVANCE,

			ERROR,
			DONE
						);

	static color_e color_side;
	static bool_e we_will_get_out_with_error;
	static Sint16 delta_x_rocket;
	switch(state){

		case INIT:{
			if(IHM_switchs_get(SWITCH_ANNE_DISABLE_MODULE))
			{
				error_printf("switch modules désactivé\n");
				state = ERROR; 	// Actionneurs désactivés
			}
			else if(rocket != OUR_ELEMENT && IHM_switchs_get(SWITCH_ANNE_TAKE_ADV_ROCKET)==FALSE)
			{
				error_printf("Fusée adverse désactivée\n");
				state = ERROR;	//Fusée adverse désactivée
			}
			else if(rocket == OUR_ELEMENT && IHM_switchs_get(SWITCH_ANNE_TAKE_OUR_ROCKET)==FALSE)
			{
				error_printf("Notre fusée désactivée\n");
				state = ERROR;	//Notre fusée désactivée
			}
			else if((rocket == OUR_ELEMENT && ROCKETS_isEmpty(MODULE_ROCKET_MULTI_OUR_SIDE))
			|| (rocket == ADV_ELEMENT && ROCKETS_isEmpty(MODULE_ROCKET_MULTI_ADV_SIDE))){
			{
				error_printf("Fusée déjà vidée\n");
				state = DONE;	 // On a déjà vidé cette fusée
			}
			}else if((rocket == OUR_ELEMENT && ELEMENTS_get_flag(FLAG_SUB_HARRY_TAKE_CYLINDER_OUR_ROCKET_MULTI))
			|| (rocket == ADV_ELEMENT && ELEMENTS_get_flag(FLAG_SUB_HARRY_TAKE_CYLINDER_ADV_ROCKET_MULTI)) ){
			{
				error_printf("Harry vide déjà cette fusée\n");
				state = ERROR; //  Harry déjà en train de vider cette fusée
			}
			}else if(STOCKS_getNbModules(MODULE_STOCK_SMALL)>=4)
			{
				error_printf("Plus de place pour prendre des modules\n");
				state = ERROR;	//Plus de place pour prendre des modules !
			}
			else{
				state = ALL_THE_GET_IN;
				//state = INIT_ALL_ACTIONNEUR;
				// On lève le flag de subaction
				if(rocket == OUR_ELEMENT){
					ELEMENTS_set_flag(FLAG_SUB_ANNE_TAKE_CYLINDER_OUR_ROCKET_MULTI, TRUE);
				}else{
					ELEMENTS_set_flag(FLAG_SUB_ANNE_TAKE_CYLINDER_ADV_ROCKET_MULTI, TRUE);
				}
			}
			delta_x_rocket = 0;
			}break;
		/*
		case INIT_ALL_ACTIONNEUR:
			// on appelle une fonction qui verifie la position initiale de tout nos actionneurs
			static error_e result;
			if(entrance){
				result = init_all_actionneur();
			}
			if(result == END_OK){
				state=ALL_THE_GET_IN;
			}else{
				state=ERROR;
			}
			break;
			*/

		case ALL_THE_GET_IN:

			if((rocket == OUR_ELEMENT && global.color == BLUE) || (rocket == ADV_ELEMENT && global.color == YELLOW)){
				color_side = BLUE;
				if(i_am_in_square(700, 1450, 250, 1000))
					state = GET_IN_OUR_SQUARE;
				else if (i_am_in_square(300, 1150, 1000, 2000))
					state = GET_IN_MIDDLE_SQUARE;
				else
					state = GET_IN_PATHFIND;

			}else{ // Prise fusee cote bleu
				color_side = YELLOW;
				if(i_am_in_square(700, 1450, 2000, 2750))
					state = GET_IN_OUR_SQUARE;
				else if (i_am_in_square(300, 1150, 1000, 2000))
					state = GET_IN_MIDDLE_SQUARE;
				else
					state = GET_IN_PATHFIND;

			}
			break;


		case GET_IN_OUR_SQUARE:
			state = try_going(1000, (color_side==BLUE)?400:2600, state, RUSH_TO_MEASURE_Y, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_BRAKE);
			break;

		case GET_IN_MIDDLE_SQUARE:
			state = try_going(900, (color_side==BLUE)?750:2250, state, GET_IN_OUR_SQUARE, GET_IN_PATHFIND, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_BRAKE);
			break;

		case GET_IN_PATHFIND:
			state = ASTAR_try_going(1000, (color_side==BLUE)?400:2600, state, RUSH_TO_MEASURE_Y, ERROR, FAST, ANY_WAY, DODGE_AND_WAIT, END_AT_BRAKE);
			break;
		case RUSH_TO_MEASURE_Y:{
			Sint16 delta;
			Sint16 theorical_y;
			theorical_y = 80+28+SMALL_CALIBRATION_FORWARD_BORDER_DISTANCE;
			if(color_side==YELLOW)
				theorical_y = 3000-theorical_y;
			state = check_sub_action_result(action_recalage_y(FORWARD,(color_side==BLUE)?(-PI4096/2):(PI4096/2), theorical_y, (color_side==BLUE)?(-PI4096/2):(PI4096/2),  FALSE, &delta, TRUE, TRUE), state, BACK_AFTER_RUSH, BACK_AFTER_FAILING_RUSH);
			break;}
		case BACK_AFTER_RUSH:
			state = try_going(global.pos.x, (color_side==BLUE)?260:2740,state,SCANNING_TRAJECTORY, RUSH_TO_MEASURE_Y, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;
		case BACK_AFTER_FAILING_RUSH:
			state = try_going(global.pos.x, (color_side==BLUE)?260:2740,state,ERROR, RUSH_TO_MEASURE_Y, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;
		case SCANNING_TRAJECTORY:{


			static bool_e scan_is_running;
			//Le scan se fait toujours en marche avant.. on doit utiliser le bon capteur en fonction du color_side !
			if(entrance)
			{
				scan_is_running = FALSE;
				PROP_WARNER_arm_x(1100);
			}
			if(global.prop.reach_x)
			{
				if(scan_is_running == FALSE)
				{
					scan_is_running = TRUE;

					Supervision_send_periodically_pos(5, PI4096/45);	//Broadcast pos à 5mm d'intervalle pour avoir un scan à 5mm d'intervalle

//TODO : Débugger le capteur droit.... ou sinon corriger ici pour faire fonctionner le gauche  !
					/*if(color_side==YELLOW)								//Activer le scan sur la carte actionneur
						ACT_start_scan(SCAN_I2C_RIGHT);
					else
					*/
					ACT_start_scan(SCAN_I2C_LEFT);

					PROP_WARNER_arm_x(1300);
				}
				else	//Le scan est actif
				{
					ACT_stop_scan();									//Fin du scan sur la carte actionneur... récupérer l'info de position !!!
					Supervision_send_periodically_pos(20, PI4096/45);	//Désactiver les broadcast pos à 5mm d'intervalle -> remettre la bonne valeur par défaut...


				}
			}
			//TODO if(minimum_receive_from_actuator.....) delta_x_rocket = ....;
			state = try_going((color_side==BLUE)?1380:1350, (color_side==BLUE)?260:2740, state, WAIT_RESULT, WAIT_RESULT, SLOW, (color_side==BLUE)?BACKWARD:FORWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);

			//state = GET_IN_FRONT_OF_ONE_ON_TWO;
		}
			break;

		case WAIT_RESULT:{
			static time32_t beginState;
			SCAN_I2C_data_s scanData;

			if(entrance){
				beginState = global.absolute_time;
			}

			if(global.absolute_time - beginState > 100){	//Données du scan non accessibles...
				delta_x_rocket = 0;
				#ifdef ROBOT_VIRTUEL_PARFAIT
					delta_x_rocket = 50;	//Juste pour tester !!!
				#endif
					error_printf("pas de scan accessible, on vise la fusée théorique, ca va merder à coup sur !\n");
				state = GET_IN_FRONT_OF_ONE_ON_TWO;
			}


			if(ACT_get_scan_result(&scanData)){ 			// Réception des données du scan
				delta_x_rocket = 0;

				if(color_side == BLUE)
					scanData.xForMin -= OFFSET_POS_SENSOR_IN_ROBOT;
				else
					scanData.xForMin += OFFSET_POS_SENSOR_IN_ROBOT;

				if(absolute(scanData.max - scanData.min) > 45)	//S'il y a plus de 4,5cm, on considère qu'on a bien vu un module
					if(absolute(scanData.xForMin - 1350) < 100)	//On autorise un décalage de la fusée de 10cm !
						delta_x_rocket = scanData.xForMin - 1350;
				error_printf("SCAN :-) résultat : max-min=%d-%d=%d -> xForMin=%d\n", scanData.max, scanData.min, scanData.max - scanData.min, scanData.xForMin);
				error_printf("SCAN :-) delta_x_rocket : %d\n", delta_x_rocket);
				state = GET_IN_FRONT_OF_ONE_ON_TWO;
			}

			}break;

		case GET_IN_FRONT_OF_ONE_ON_TWO:
			state = try_going(1350+delta_x_rocket, (color_side==BLUE)?360:2640, state, TURN_TO_POS, ERROR, FAST, BACKWARD, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

		case TURN_TO_POS:
			state = try_go_angle((color_side==BLUE)?(-PI4096/2):(PI4096/2), state, GET_IN_FRONT_OF_TWO_ON_TWO, ERROR, FAST, ANY_WAY, END_AT_LAST_POINT);
			break;

		case GET_IN_FRONT_OF_TWO_ON_TWO:
			if(entrance){
				if(color_side==BLUE)
					ACT_push_order(ACT_BIG_BALL_FRONT_LEFT, ACT_BIG_BALL_FRONT_LEFT_UP);
				else
					ACT_push_order(ACT_BIG_BALL_FRONT_RIGHT, ACT_BIG_BALL_FRONT_RIGHT_UP);
				}
				state = try_going(1350+delta_x_rocket+((color_side==BLUE)?(-OFFSET_BEST_SLIDER_APPROACH):OFFSET_BEST_SLIDER_APPROACH),(color_side==BLUE)?255:(3000-255), state, ACTION, ERROR, FAST, FORWARD, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
				break;

		case ACTION:{
			error_e resultAction = sub_act_anne_take_rocket_down_to_top((rocket==OUR_ELEMENT)?MODULE_ROCKET_MULTI_OUR_SIDE:MODULE_ROCKET_MULTI_ADV_SIDE, 4);
//			error_e resultInit = init_all_actionneur(); // On init ou pas ? Si on le fail on part en failed_init // A ne pas faire ici
//			#warning "temporaire pour faire comme si la prise fonctionnait"
//			STOCKS_addModule((global.color==BLUE)?MODULE_BLUE:MODULE_YELLOW, STOCK_POS_ENTRY, MODULE_STOCK_SMALL);
//
//			resultAction = END_OK;
			if(resultAction != IN_PROGRESS)
			{
				if(resultAction == END_OK)
					we_will_get_out_with_error = FALSE;
				else
					we_will_get_out_with_error = TRUE;
				state=GET_OUT;
			}


			}break;

		case GET_OUT:
			state=try_going(global.pos.x, (color_side==BLUE)?500:2500, state, (we_will_get_out_with_error)?ERROR:DONE, ADVANCE, FAST, BACKWARD, NO_DODGE_AND_NO_WAIT, END_AT_BRAKE);

			if(ON_LEAVE())
			{
				if(color_side==BLUE)
					ACT_push_order(ACT_BIG_BALL_FRONT_LEFT, ACT_BIG_BALL_FRONT_LEFT_DOWN);
				else
					ACT_push_order(ACT_BIG_BALL_FRONT_RIGHT, ACT_BIG_BALL_FRONT_RIGHT_DOWN);
			}
			break;

		case ADVANCE:
			state=try_going(global.pos.x, (color_side==BLUE)?220:2780, state, GET_OUT, GET_OUT, FAST, BACKWARD, NO_DODGE_AND_NO_WAIT, END_AT_BRAKE);
			break;

		case ERROR:
			RESET_MAE();
			on_turning_point();
			if(rocket == OUR_ELEMENT){
				ELEMENTS_set_flag(FLAG_SUB_ANNE_TAKE_CYLINDER_OUR_ROCKET_MULTI, FALSE);
			}else{
				ELEMENTS_set_flag(FLAG_SUB_ANNE_TAKE_CYLINDER_ADV_ROCKET_MULTI, FALSE);
			}
			return NOT_HANDLED;
			break;

		case DONE:
			RESET_MAE();
			on_turning_point();
			if(rocket == OUR_ELEMENT){
				ELEMENTS_set_flag(FLAG_SUB_ANNE_TAKE_CYLINDER_OUR_ROCKET_MULTI, FALSE);
			}else{
				ELEMENTS_set_flag(FLAG_SUB_ANNE_TAKE_CYLINDER_ADV_ROCKET_MULTI, FALSE);
			}
			return END_OK;
			break;

		default:
			if(entrance)
			{
				error_printf("default case in sub_anne_fusee_multicolor\n");
			}
			break;
	}

	return IN_PROGRESS;
}
