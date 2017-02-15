#include "action_big.h"
#include "../../propulsion/movement.h"
#include "../../propulsion/astar.h"
#include "../../QS/QS_stateMachineHelper.h"
#include "../../QS/QS_types.h"
#include "../../QS/QS_outputlog.h"

#include  "../../propulsion/astar.h"
#include  "../../utils/generic_functions.h"
#include "../../actuator/act_functions.h"
#include "../../actuator/queue.h"
#include "../../utils/actionChecker.h"
#include "../../elements.h"
#include "../../high_level_strat.h"


error_e sub_harry_prise_modules_centre(ELEMENTS_property_e modules, bool_e onlyTwoModules){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_MODULE_CENTER,
			INIT,
			GO_TO_START_POINT_UP,
			GO_TO_START_POINT_SIDE,
			GET_IN_CLOSE_ADV_ZONE,
			GET_IN_FAR_ADV_ZONE,
			GET_IN_ASTAR,
			TAKE_FIRST_MODULE_UP,
			TAKE_FIRST_MODULE_SIDE,
			CHOICE_SECOND_MODULE,
			GO_TO_THIRD_MODULE_DIRECTLY,
			TAKE_THIRD_MODULE_DIRECTLY,
			GO_TO_SECOND_MODULE,
			TAKE_SECOND_MODULE,
			GET_OUT_SECOND_MODULE,
			GO_TO_THIRD_MODULE,
			TAKE_THIRD_MODULE,
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
			if( (onlyTwoModules && !ELEMENTS_get_flag(FLAG_OUR_MULTICOLOR_START_IS_TAKEN) && !ELEMENTS_get_flag(FLAG_OUR_MULTICOLOR_SIDE_IS_TAKEN))
			|| (!onlyTwoModules && !ELEMENTS_get_flag(FLAG_OUR_MULTICOLOR_START_IS_TAKEN) && !ELEMENTS_get_flag(FLAG_OUR_MULTICOLOR_SIDE_IS_TAKEN) && !ELEMENTS_get_flag(FLAG_OUR_MULTICOLOR_NEAR_DEPOSE_IS_TAKEN))){
				state = DONE; // Il n'y a plus rien à faire
			}
			else if(ELEMENTS_get_flag(FLAG_SUB_ANNE_TAKE_CYLINDER_OUR_ROCKET_UNI)) // Si Anne est déjà en train de faire cette action
			{
				state = ERROR;
			}
			else
			{
				// Si on prend les modules du coté BLUE		(utiliser i_am_in_square et pas i_am_in_square_color)
				if((global.color == BLUE && modules == OUR_ELEMENT) || (global.color == YELLOW && modules == ADV_ELEMENT)){
					if(i_am_in_square(0, 550, 800, 1200)){
						state = GO_TO_START_POINT_UP;
					}else if(i_am_in_square(0, 1000, 1200, 2100)){
						state = GO_TO_START_POINT_SIDE;
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
						state = GO_TO_START_POINT_UP;
					}else if(i_am_in_square(0, 1000, 900, 1800)){
						state = GO_TO_START_POINT_SIDE;
					}else if(i_am_in_square(700, 1400, 400, 1100)){
						state = GET_IN_CLOSE_ADV_ZONE;
					}else if(i_am_in_square(700, 1400, 200, 400)){
						state = GET_IN_FAR_ADV_ZONE;
					}else{
						state = GET_IN_ASTAR;
					}
				}

				// On lève le flag de subaction
				ELEMENTS_set_flag(FLAG_SUB_HARRY_TAKE_CYLINDER_OUR_ROCKET_UNI, TRUE);
			}
			break;

		case GET_IN_CLOSE_ADV_ZONE:
			if((global.color == BLUE && modules == OUR_ELEMENT) || (global.color == YELLOW && modules == ADV_ELEMENT)){
				state = try_going(900, 2000, state, GO_TO_START_POINT_SIDE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_BRAKE);
			}else{
				state = try_going(900, 1000, state, GO_TO_START_POINT_SIDE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_BRAKE);
			}
			break;

		case GET_IN_FAR_ADV_ZONE:
			if((global.color == BLUE && modules == OUR_ELEMENT) || (global.color == YELLOW && modules == ADV_ELEMENT)){
				state = try_going_multipoint(curve_far_zone_yellow, 2, state, GO_TO_START_POINT_SIDE, ERROR, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_BRAKE);
			}else{
				state = try_going_multipoint(curve_far_zone_blue, 2, state, GO_TO_START_POINT_SIDE, ERROR, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_BRAKE);
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
				state = try_going(350, 950, state, TAKE_FIRST_MODULE_UP, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			}else{
				state = try_going(350, 2050, state, TAKE_FIRST_MODULE_UP, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			}
			break;

		case GO_TO_START_POINT_SIDE:
			if((global.color == BLUE && modules == OUR_ELEMENT) || (global.color == YELLOW && modules == ADV_ELEMENT)){
				state = try_going(650, 1250, state, TAKE_FIRST_MODULE_SIDE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			}else{
				state = try_going(650, 1750, state, TAKE_FIRST_MODULE_SIDE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			}
			break;

		case TAKE_FIRST_MODULE_UP:
			if((global.color == BLUE && modules == OUR_ELEMENT) || (global.color == YELLOW && modules == ADV_ELEMENT)){
				state = try_going(650, 950, state, CHOICE_SECOND_MODULE, ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			}else{
				state = try_going(650, 2050, state, CHOICE_SECOND_MODULE, ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			}
			if(ON_LEAVING(state)){
				ELEMENTS_set_flag(FLAG_OUR_MULTICOLOR_START_IS_TAKEN, TRUE);	// Flag element
				ELEMENTS_set_flag(FLAG_SUB_HARRY_TAKE_CYLINDER_OUR_ROCKET_UNI, FALSE);	// Flag subaction
			}
			break;

		case TAKE_FIRST_MODULE_SIDE:
			if((global.color == BLUE && modules == OUR_ELEMENT) || (global.color == YELLOW && modules == ADV_ELEMENT)){
				state = try_going(650, 1100, state, CHOICE_SECOND_MODULE, ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_BRAKE);
			}else{
				state = try_going(650, 1900, state, CHOICE_SECOND_MODULE, ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_BRAKE);
			}
			if(ON_LEAVING(state)){
				ELEMENTS_set_flag(FLAG_OUR_MULTICOLOR_START_IS_TAKEN, TRUE);	// Flag element
				ELEMENTS_set_flag(FLAG_SUB_HARRY_TAKE_CYLINDER_OUR_ROCKET_UNI, FALSE);	// Flag subaction
				set_sub_act_enable(SUB_HARRY_DEPOSE_MODULES, TRUE);   // Activation de la dépose
			}
			break;

		case CHOICE_SECOND_MODULE:
			if(onlyTwoModules){
				state = GO_TO_THIRD_MODULE_DIRECTLY;
			}else{
				state = GO_TO_SECOND_MODULE;
			}
			break;

// Prend seulement le 3ième module
		case GO_TO_THIRD_MODULE_DIRECTLY:
			if(entrance){
				ELEMENTS_set_flag(FLAG_SUB_HARRY_TAKE_CYLINDER_OUR_ROCKET_MULTI, TRUE);
			}
			if((global.color == BLUE && modules == OUR_ELEMENT) || (global.color == YELLOW && modules == ADV_ELEMENT)){
				state = try_going(1090, 750, state, TAKE_THIRD_MODULE_DIRECTLY, ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_BRAKE);
			}else{
				state = try_going(1090, 2250, state, TAKE_THIRD_MODULE_DIRECTLY, ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_BRAKE);
			}
			break;

		case TAKE_THIRD_MODULE_DIRECTLY:
			if((global.color == BLUE && modules == OUR_ELEMENT) || (global.color == YELLOW && modules == ADV_ELEMENT)){
				state = try_going(1150, 580, state, DONE, ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			}else{
				state = try_going(1150, 2420, state, DONE, ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			}
			if(ON_LEAVING(state)){
				ELEMENTS_set_flag(FLAG_OUR_MULTICOLOR_SIDE_IS_TAKEN, TRUE);	// Flag element
				ELEMENTS_set_flag(FLAG_SUB_HARRY_TAKE_CYLINDER_OUR_ROCKET_MULTI, FALSE);	// Flag subaction
				set_sub_act_enable(SUB_HARRY_DEPOSE_MODULES, TRUE);   // Activation de la dépose
			}
			break;

// Prend le 2ième et le 3ième module
		case GO_TO_SECOND_MODULE:
			if(entrance){
				ELEMENTS_set_flag(FLAG_SUB_HARRY_TAKE_CYLINDER_OUR_CENTER, TRUE);
			}
			if((global.color == BLUE && modules == OUR_ELEMENT) || (global.color == YELLOW && modules == ADV_ELEMENT)){
				state = try_going(1150, 850, state, TAKE_SECOND_MODULE, GET_OUT_SECOND_MODULE, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_BRAKE);
			}else{
				state = try_going(1150, 2150, state, TAKE_SECOND_MODULE, GET_OUT_SECOND_MODULE, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_BRAKE);
			}
			break;

		case TAKE_SECOND_MODULE:
			if((global.color == BLUE && modules == OUR_ELEMENT) || (global.color == YELLOW && modules == ADV_ELEMENT)){
				state = try_going(1345, 810, state, GET_OUT_SECOND_MODULE, ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			}else{
				state = try_going(1345, 1190, state, GET_OUT_SECOND_MODULE, ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			}
			if(ON_LEAVING(state)){
				ELEMENTS_set_flag(FLAG_OUR_MULTICOLOR_NEAR_DEPOSE_IS_TAKEN, TRUE);	// Flag element
				ELEMENTS_set_flag(FLAG_SUB_HARRY_TAKE_CYLINDER_OUR_CENTER, FALSE); // Flag subaction
				set_sub_act_enable(SUB_HARRY_DEPOSE_MODULES, TRUE);   // Activation de la dépose
			}
			break;

		case GET_OUT_SECOND_MODULE:
			if((global.color == BLUE && modules == OUR_ELEMENT) || (global.color == YELLOW && modules == ADV_ELEMENT)){
				state = try_going(1150, 850, state, GO_TO_THIRD_MODULE, TAKE_SECOND_MODULE, FAST, BACKWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			}else{
				state = try_going(1150, 2150, state, GO_TO_THIRD_MODULE, TAKE_SECOND_MODULE, FAST, BACKWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			}
			break;

		case GO_TO_THIRD_MODULE:
			if(entrance){
				ELEMENTS_set_flag(FLAG_SUB_HARRY_TAKE_CYLINDER_OUR_ROCKET_MULTI, TRUE);
			}
			if((global.color == BLUE && modules == OUR_ELEMENT) || (global.color == YELLOW && modules == ADV_ELEMENT)){
				state = try_going(1160, 680, state, TAKE_THIRD_MODULE, ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_BRAKE);
			}else{
				state = try_going(1200, 2320, state, TAKE_THIRD_MODULE, ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_BRAKE);
			}
			break;

		case TAKE_THIRD_MODULE:
			if((global.color == BLUE && modules == OUR_ELEMENT) || (global.color == YELLOW && modules == ADV_ELEMENT)){
				state = try_going(1165, 580, state, DONE, ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			}else{
				state = try_going(1165, 2420, state, DONE, ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			}
			if(ON_LEAVING(state)){
				ELEMENTS_set_flag(FLAG_OUR_MULTICOLOR_SIDE_IS_TAKEN, TRUE);	// Flag element
				ELEMENTS_set_flag(FLAG_SUB_HARRY_TAKE_CYLINDER_OUR_ROCKET_MULTI, FALSE);	// Flag subaction
				set_sub_act_enable(SUB_HARRY_DEPOSE_MODULES, TRUE);   // Activation de la dépose
			}
			break;

		case ERROR:
			RESET_MAE();
			ELEMENTS_set_flag(FLAG_SUB_HARRY_TAKE_CYLINDER_OUR_ROCKET_UNI, FALSE);
			ELEMENTS_set_flag(FLAG_SUB_HARRY_TAKE_CYLINDER_OUR_ROCKET_MULTI, FALSE);
			ELEMENTS_set_flag(FLAG_SUB_HARRY_TAKE_CYLINDER_OUR_CENTER, FALSE);
			on_turning_point();
			return NOT_HANDLED;
			break;

		case DONE:
			RESET_MAE();
			ELEMENTS_set_flag(FLAG_SUB_HARRY_TAKE_CYLINDER_OUR_ROCKET_UNI, FALSE);
			ELEMENTS_set_flag(FLAG_SUB_HARRY_TAKE_CYLINDER_OUR_ROCKET_MULTI, FALSE);
			ELEMENTS_set_flag(FLAG_SUB_HARRY_TAKE_CYLINDER_OUR_CENTER, FALSE);
			on_turning_point();
			return END_OK;
			break;

		default:
			if(entrance)
				debug_printf("default case in sub_harry_prise_modules_centre\n");
			break;
	}

	return IN_PROGRESS;
}




error_e sub_harry_rocket_monocolor(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_ROCKET_MONOCOLOR,
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
			if(ROCKETS_isEmpty(MODULE_ROCKET_MONO_OUR_SIDE)){
				state = DONE; // La fusée a déjà été prise
			}else if(ELEMENTS_get_flag(FLAG_SUB_ANNE_TAKE_CYLINDER_OUR_ROCKET_UNI)){
				state = ERROR;
			}else{
				if (i_am_in_square_color(200, 1100, 900, 2100)){
					state = GET_IN_DIRECT;
				}else if(i_am_in_square_color(800, 1400, 300, 900)){
					state = GET_IN_OUR_SQUARE;
				}else if (i_am_in_square_color(800, 1400, 2100, 2700)){
					state = GET_IN_ADV_SQUARE;
				}else{
					state = PATHFIND;
				}

				// On lève le flag de subaction
				ELEMENTS_set_flag(FLAG_SUB_HARRY_TAKE_CYLINDER_OUR_ROCKET_UNI, TRUE);
			}
			break;

		case GET_IN_OUR_SQUARE:
			state = try_going(800, COLOR_Y(1000), state, GET_IN_DIRECT, ERROR, FAST, ANY_WAY, DODGE_AND_WAIT, END_AT_BRAKE);
			break;

		case GET_IN_ADV_SQUARE:
			state = try_going(800, COLOR_Y(2000), state, GET_IN_DIRECT, ERROR, FAST, ANY_WAY, DODGE_AND_WAIT, END_AT_BRAKE);
			break;

		case GET_IN_DIRECT:
			state = try_going(400, COLOR_Y(1150), state, GO_TO_START_POINT, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case PATHFIND:
			state = ASTAR_try_going(400, COLOR_Y(1150), state, GO_TO_START_POINT, ERROR, FAST, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case GO_TO_START_POINT:
			state = try_going(275, COLOR_Y(1150), state, TAKE_ROCKET, GET_OUT_ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case TAKE_ROCKET: // Execution des ordres actionneurs
			//state=check_sub_action_result(sub_harry_take_rocket(),state,GET_OUT,GET_OUT_ERROR);

			// Juste pour les tests
			ROCKETS_removeModule(MODULE_ROCKET_MONO_OUR_SIDE);
			ROCKETS_removeModule(MODULE_ROCKET_MONO_OUR_SIDE);
			ROCKETS_removeModule(MODULE_ROCKET_MONO_OUR_SIDE);
			ROCKETS_removeModule(MODULE_ROCKET_MONO_OUR_SIDE);

			state = GET_OUT;
			break;

		case GET_OUT:
			if(entrance){
				set_sub_act_enable(SUB_HARRY_DEPOSE_MODULES, TRUE);
			}
			state=try_going(500, global.pos.y, state, DONE, AVANCE, FAST, BACKWARD, NO_DODGE_AND_NO_WAIT, END_AT_BRAKE);
			break;

		case GET_OUT_ERROR:
			state=try_going(500, global.pos.y, state, ERROR, AVANCE_ERROR, FAST, BACKWARD, NO_DODGE_AND_NO_WAIT, END_AT_BRAKE);
			break;

		case AVANCE:
			state=try_going(220, global.pos.y, state, GET_OUT, GET_OUT, FAST, FORWARD, NO_DODGE_AND_NO_WAIT, END_AT_BRAKE);
			break;

		case AVANCE_ERROR:
			state=try_going(220, global.pos.y, state, GET_OUT_ERROR, GET_OUT_ERROR, FAST, FORWARD, NO_DODGE_AND_NO_WAIT, END_AT_BRAKE);
			break;

		case ERROR:
			RESET_MAE();
			on_turning_point();
			ELEMENTS_set_flag(FLAG_SUB_HARRY_TAKE_CYLINDER_OUR_ROCKET_UNI, FALSE);
			return NOT_HANDLED;
			break;

		case DONE:
			RESET_MAE();
			on_turning_point();
			ELEMENTS_set_flag(FLAG_SUB_HARRY_TAKE_CYLINDER_OUR_ROCKET_UNI, FALSE);
			return END_OK;
			break;

		default:
			if(entrance)
				debug_printf("default case in sub_harry_rocket_monocolor\n");
			break;
	}

	return IN_PROGRESS;
}



//#if 0
error_e sub_harry_rocket_multicolor(ELEMENTS_property_e rocket, bool_e right_side){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_PRISE_ROCKET_MULTICOLOR,
				INIT,

				ALL_THE_GET_IN,
				GET_IN_DIRECT,
				GET_IN_AVOID_CRATERE,
				GET_IN_AVOID_START_ZONE,
				GET_IN_FROM_CLOSE_ADV_ZONE,
				GET_IN_FROM_FAR_ADV_ZONE,
				GET_IN_ASTAR,

				ACTION,
				ACTION_RETRACT_ALL,
				FAILED_INIT_ACTION,

				ERROR,
				DONE
			);

	//provisoire, il faut rendre ces valeurs globale !!!
	moduleStockLocation_e nb_cylinder_big_left  = MODULE_STOCK_LEFT;
	moduleStockLocation_e nb_cylinder_big_right = MODULE_STOCK_RIGHT;
	moduleRocketLocation_e   nb_cylinder_fusee  = MODULE_ROCKET_MULTI_OUR_SIDE;

	//rendre les valeurs globale !

	switch(state){

		case INIT:{
			if((rocket == OUR_ELEMENT && ROCKETS_isEmpty(MODULE_ROCKET_MULTI_OUR_SIDE))
			|| (rocket == OUR_ELEMENT && ROCKETS_isEmpty(MODULE_ROCKET_MULTI_OUR_SIDE))){
				state = DONE;	 // On a déjà vidé cette fusée
			}else if((rocket == OUR_ELEMENT && ELEMENTS_get_flag(FLAG_SUB_ANNE_TAKE_CYLINDER_OUR_ROCKET_MULTI))
			|| (rocket == ADV_ELEMENT && ELEMENTS_get_flag(FLAG_SUB_ANNE_TAKE_CYLINDER_ADV_ROCKET_MULTI)) ){
				state = ERROR; // Anne est déjà en train de vider cette fusée
			}else{
				// on appelle une fonction qui verifie la position initiale de tout nos actionneurs
				error_e result = init_all_actionneur(nb_cylinder_big_right, nb_cylinder_big_left);
				if(result == END_OK){
					state=ALL_THE_GET_IN;
				}else{
					state=FAILED_INIT_ACTION;
				}

				// On lève le flag de subaction
				if(rocket == OUR_ELEMENT){
					ELEMENTS_set_flag(FLAG_SUB_HARRY_TAKE_CYLINDER_OUR_ROCKET_MULTI, TRUE);
				}else{
					ELEMENTS_set_flag(FLAG_SUB_HARRY_TAKE_CYLINDER_ADV_ROCKET_MULTI, TRUE);
				}
			}
			}break;

		case ALL_THE_GET_IN:

			if( i_am_in_square_color(350, 600, 800, 1300)){ //zone bloquee par cratere traitee en propriete:
				state = GET_IN_AVOID_CRATERE;
			}else if( i_am_in_square_color(400,1950,100,880) || i_am_in_square_color(350,1250,880,1600)){
				state = GET_IN_DIRECT;
			}else if( i_am_in_square_color(0, 350, 1100, 1900)){
				state = GET_IN_AVOID_START_ZONE;
			}else if( i_am_in_square_color(400, 1500, 1700, 2400)){
				state = GET_IN_FROM_CLOSE_ADV_ZONE;
			}else if( i_am_in_square_color(400, 1950, 2500, 3000)){
				state = GET_IN_FROM_FAR_ADV_ZONE;
			}else{
				state = GET_IN_ASTAR;
			}
			break;

		case GET_IN_DIRECT:
			state = try_going(1200, 300, GET_IN_DIRECT, ACTION,
					ERROR, FAST, ANY_WAY, NO_AVOIDANCE, END_AT_BRAKE);
			break;

		case GET_IN_AVOID_CRATERE:{
			const displacement_t curve_by_our_zone_crat[2] = {
					{ { 750, 790 }, FAST }, { { 1200, 300 }, FAST } };
			state = try_going_multipoint(curve_by_our_zone_crat, 2,
					GET_IN_AVOID_CRATERE, ACTION, ERROR, ANY_WAY,
					DODGE_AND_WAIT, END_AT_BRAKE);
			}break;

		case GET_IN_AVOID_START_ZONE:{
			const displacement_t curve_by_our_zone_start[2] = { { { 500, 1250 },FAST }, { { 1200, 300 }, FAST } };
			state = try_going_multipoint(curve_by_our_zone_start, 2,
					GET_IN_AVOID_START_ZONE, ACTION, ERROR, ANY_WAY,
					DODGE_AND_WAIT, END_AT_BRAKE);
			}break;

		case GET_IN_FROM_CLOSE_ADV_ZONE:{
			const displacement_t curve_by_our_zone_advclo[2] = { { { 1000, 1725 },FAST }, { { 1200, 300 }, FAST } };
			state = try_going_multipoint(curve_by_our_zone_advclo, 2,
					GET_IN_FROM_CLOSE_ADV_ZONE, ACTION, ERROR,
					ANY_WAY, DODGE_AND_WAIT, END_AT_BRAKE);
			}break;

		case GET_IN_FROM_FAR_ADV_ZONE:{
			const displacement_t curve_by_our_zone_advfa[2] = { { { 900, 2400 },FAST }, { { 1200, 300 }, FAST } };
			state = try_going_multipoint(curve_by_our_zone_advfa, 2,
					GET_IN_FROM_FAR_ADV_ZONE, ACTION, ERROR,
					ANY_WAY, DODGE_AND_WAIT, END_AT_BRAKE);
			}break;

		case GET_IN_ASTAR:
			state = ASTAR_try_going(1200, 300, GET_IN_ASTAR, ACTION,
					ERROR, FAST, ANY_WAY, NO_AVOIDANCE, END_AT_BRAKE);
			break;

		case ACTION:{
			error_e resultAction = boucle_charge_module( nb_cylinder_big_right, nb_cylinder_big_left, nb_cylinder_fusee, MODULE_POLY, right_side );
			if(resultAction == END_OK){
				state=ACTION_RETRACT_ALL;
				if(rocket == OUR_ELEMENT){
					ROCKETS_removeModule(MODULE_ROCKET_MULTI_OUR_SIDE);
					ROCKETS_removeModule(MODULE_ROCKET_MULTI_OUR_SIDE);
					ROCKETS_removeModule(MODULE_ROCKET_MULTI_OUR_SIDE);
					ROCKETS_removeModule(MODULE_ROCKET_MULTI_OUR_SIDE);
				}else{
					ROCKETS_removeModule(MODULE_ROCKET_MULTI_ADV_SIDE);
					ROCKETS_removeModule(MODULE_ROCKET_MULTI_ADV_SIDE);
					ROCKETS_removeModule(MODULE_ROCKET_MULTI_ADV_SIDE);
					ROCKETS_removeModule(MODULE_ROCKET_MULTI_ADV_SIDE);
				}
			}else{
				state=ERROR;
			}
			}break;

		case ACTION_RETRACT_ALL:{
			error_e result = init_all_actionneur(nb_cylinder_big_right, nb_cylinder_big_left);
			if(result == END_OK){
				state=DONE;
			}else{
				state=FAILED_INIT_ACTION;
			}
			}break;

		case FAILED_INIT_ACTION:
			//stop?
			state = ERROR;
			break;

		case DONE:
			RESET_MAE();
			if(rocket == OUR_ELEMENT){
				ELEMENTS_set_flag(FLAG_SUB_HARRY_TAKE_CYLINDER_OUR_ROCKET_MULTI, FALSE);
			}else{
				ELEMENTS_set_flag(FLAG_SUB_HARRY_TAKE_CYLINDER_ADV_ROCKET_MULTI, FALSE);
			}
			return END_OK;
			break;

		case ERROR:
			RESET_MAE();
			if(rocket == OUR_ELEMENT){
				ELEMENTS_set_flag(FLAG_SUB_HARRY_TAKE_CYLINDER_OUR_ROCKET_MULTI, FALSE);
			}else{
				ELEMENTS_set_flag(FLAG_SUB_HARRY_TAKE_CYLINDER_ADV_ROCKET_MULTI, FALSE);
			}
			return NOT_HANDLED;
			break;
		}

		return IN_PROGRESS;

}


error_e boucle_charge_module(moduleStockLocation_e nb_cylinder_big_right,moduleStockLocation_e nb_cylinder_big_left,moduleRocketLocation_e nb_cylinder_fusee, moduleType_e myModule,  bool_e right_side ){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_INIT_CYLINDER,
			INIT,
			ACTION_GO_TAKE_CYLINDER,
			CHECK_STATUS_OTHER_SLINDER,
			PROTECT_NEXT_FALL,
			ACTION_BRING_BACK_CYLINDER,
			CHECK_STATUS_OTHER_SLINDER_2,
			CHECK_STATUS_ELEVATOR,
			ACTION_START_BRING_UP_CYLINDER,
			CHECK_STATUS_SLOPE,
			ACTION_END_BRING_UP_CYLINDER,
			CHECK_STATUS_SLIDER,
			ACTION_STOCK_UP_CYLINDER,
			CHECK_STATUS_OTHER_SLINDER_3,
			RESTART_CONDITION,
			ACTION_RETRACT_ALL,
			ACTION_FINAL_1_ON_3,
			ACTION_FINAL_2_ON_3,
			ACTION_FINAL_3_ON_3,
			CHECK_STATUS_OTHER_CYLINDER_FINAL,
			ERROR,
			DONE
		);

	switch(state){
		case INIT:
			state = ACTION_GO_TAKE_CYLINDER;
			break;

		case ACTION_GO_TAKE_CYLINDER:

			if (right_side){
				if (entrance) {
				//On active la pompe avant d'avancer
				ACT_push_order( ACT_POMPE_SLIDER_RIGHT , ACT_POMPE_NORMAL );
				ACT_push_order( ACT_CYLINDER_SLIDER_RIGHT , ACT_CYLINDER_SLIDER_RIGHT_OUT );

				// On avance l'autre bras pour bloquer la chute du cylindre d'apres
				ACT_push_order( ACT_CYLINDER_SLIDER_LEFT , ACT_CYLINDER_SLIDER_LEFT_ALMOST_OUT );}

				state = check_act_status(ACT_QUEUE_Cylinder_slider_right, ACTION_GO_TAKE_CYLINDER,CHECK_STATUS_OTHER_SLINDER, ERROR);
			}else{
				if (entrance) {
				ACT_push_order( ACT_POMPE_SLIDER_LEFT , ACT_POMPE_NORMAL);
				ACT_push_order( ACT_CYLINDER_SLIDER_LEFT , ACT_CYLINDER_SLIDER_LEFT_OUT );

				ACT_push_order( ACT_CYLINDER_SLIDER_RIGHT , ACT_CYLINDER_SLIDER_RIGHT_ALMOST_OUT );}

				state = check_act_status(ACT_QUEUE_Cylinder_slider_left, ACTION_GO_TAKE_CYLINDER,CHECK_STATUS_OTHER_SLINDER, ERROR);
			}
			break;

		case CHECK_STATUS_OTHER_SLINDER:
			if (right_side){
				state = check_act_status(ACT_QUEUE_Cylinder_slider_left, CHECK_STATUS_OTHER_SLINDER,PROTECT_NEXT_FALL, ERROR);
			}else{
				state = check_act_status(ACT_QUEUE_Cylinder_slider_right, CHECK_STATUS_OTHER_SLINDER,PROTECT_NEXT_FALL, ERROR);
			}
			break;

		case PROTECT_NEXT_FALL:
			if (right_side){
				if(entrance){
				ACT_push_order( ACT_CYLINDER_SLIDER_RIGHT , ACT_CYLINDER_SLIDER_RIGHT_ALMOST_OUT_WITH_CYLINDER);}
				state = check_act_status(ACT_QUEUE_Cylinder_slider_right, PROTECT_NEXT_FALL,ACTION_BRING_BACK_CYLINDER, ERROR);
			}else{
				if(entrance){
				ACT_push_order( ACT_CYLINDER_SLIDER_LEFT , ACT_CYLINDER_SLIDER_LEFT_ALMOST_OUT_WITH_CYLINDER);}
				state = check_act_status(ACT_QUEUE_Cylinder_slider_left, PROTECT_NEXT_FALL,ACTION_BRING_BACK_CYLINDER, ERROR);
			}
			break;

		case ACTION_BRING_BACK_CYLINDER:
			if (right_side){
				if (entrance) {
				ACT_push_order( ACT_CYLINDER_SLIDER_RIGHT , ACT_CYLINDER_SLIDER_RIGHT_IN );

				//On avance en profite pour bouger de l'autre cote
				ACT_push_order( ACT_POMPE_SLIDER_LEFT , ACT_POMPE_NORMAL );
				//vacuose ici?
				ACT_push_order( ACT_CYLINDER_SLIDER_LEFT , ACT_CYLINDER_SLIDER_LEFT_OUT );

				//On redescend le bras et desactive la pompe du cycle precedent si pas utilise par le stockage
				// attention a 4 on est quand meme en haut, il faut descendre !
				if ((STOCKS_getNbModules(nb_cylinder_big_left)<5)){
					ACT_push_order( ACT_POMPE_ELEVATOR_LEFT , ACT_POMPE_STOP );
					//vacuose ici?!
					ACT_push_order( ACT_CYLINDER_ELEVATOR_LEFT , ACT_CYLINDER_ELEVATOR_RIGHT_BOTTOM );}}

				//capteur inuctif fin de course a la place du check at status ?
				state = check_act_status(ACT_QUEUE_Cylinder_slider_right, ACTION_BRING_BACK_CYLINDER,CHECK_STATUS_OTHER_SLINDER_2, ERROR);
			}else{
				if (entrance) {
				ACT_push_order( ACT_CYLINDER_SLIDER_LEFT , ACT_CYLINDER_SLIDER_LEFT_IN );

				ACT_push_order( ACT_POMPE_SLIDER_RIGHT , ACT_POMPE_NORMAL );
				//vacuose ici?
				ACT_push_order( ACT_CYLINDER_SLIDER_RIGHT , ACT_CYLINDER_SLIDER_RIGHT_OUT );

				if ((STOCKS_getNbModules(nb_cylinder_big_right)<5)){
				ACT_push_order( ACT_POMPE_ELEVATOR_RIGHT , ACT_POMPE_STOP );
				//vacuose ici?
				ACT_push_order( ACT_CYLINDER_ELEVATOR_RIGHT , ACT_CYLINDER_ELEVATOR_RIGHT_BOTTOM );}}

				//capteur inuctif fin de course a la place du check at status ?
				state = check_act_status(ACT_QUEUE_Cylinder_slider_left, ACTION_BRING_BACK_CYLINDER,CHECK_STATUS_OTHER_SLINDER_2, ERROR);
			}break;

		case CHECK_STATUS_OTHER_SLINDER_2:
			if (right_side){
				//pas besoin de verifier l'elevator si il a pas ete actionne
				if ((STOCKS_getNbModules(nb_cylinder_big_left)<5)){
					state = check_act_status(ACT_QUEUE_Cylinder_slider_left, CHECK_STATUS_OTHER_SLINDER_2,CHECK_STATUS_ELEVATOR, ERROR);
				}else{
					state = check_act_status(ACT_QUEUE_Cylinder_slider_left, CHECK_STATUS_OTHER_SLINDER_2,ACTION_START_BRING_UP_CYLINDER, ERROR);}
			}else{
				if ((STOCKS_getNbModules(nb_cylinder_big_right)<5)){
					state = check_act_status(ACT_QUEUE_Cylinder_slider_right, CHECK_STATUS_OTHER_SLINDER_2,CHECK_STATUS_ELEVATOR, ERROR);
				}else{
					state = check_act_status(ACT_QUEUE_Cylinder_slider_right, CHECK_STATUS_OTHER_SLINDER_2,ACTION_START_BRING_UP_CYLINDER, ERROR);}
			}
			break;

		case CHECK_STATUS_ELEVATOR:
			if (right_side){
				state = check_act_status(ACT_QUEUE_Cylinder_elevator_left, CHECK_STATUS_ELEVATOR,ACTION_START_BRING_UP_CYLINDER, ERROR);
			}else{
				state = check_act_status(ACT_QUEUE_Cylinder_elevator_right, CHECK_STATUS_ELEVATOR,ACTION_START_BRING_UP_CYLINDER, ERROR);
			}
			break;

		case ACTION_START_BRING_UP_CYLINDER:

			if (right_side){
				if (entrance) {
				ACT_push_order( ACT_POMPE_ELEVATOR_RIGHT , ACT_POMPE_NORMAL );
				ACT_push_order( ACT_POMPE_SLIDER_RIGHT , ACT_POMPE_STOP );
				// ici rajouter le check de la pompe avec vacuose pour les deux
				ACT_push_order( ACT_CYLINDER_ELEVATOR_RIGHT , ACT_CYLINDER_ELEVATOR_RIGHT_LOCK_WITH_CYLINDER );

				// on redescend en meme temps le clapet du cylindre precedent si il n'est pas bloque
				if ((STOCKS_getNbModules(nb_cylinder_big_left)<4)){
					ACT_push_order( ACT_CYLINDER_SLOPE_LEFT , ACT_CYLINDER_SLOPE_LEFT_UNLOCK );}}

				//la on doit actionner le slider du cote de l'elevator pour qu'il parte en ALMOST_OUT
				//le plus efficace serait-il d'utiliser Wait() plutot que de creer un nouveau case pour separer la monter en deux ? !!!
				//Attention le wait peut etre dangereux, mechaniquement, si le slider avance trop tot
				state = check_act_status(ACT_QUEUE_Cylinder_elevator_right, ACTION_START_BRING_UP_CYLINDER,CHECK_STATUS_SLOPE, ERROR);
			}else{
				if (entrance) {
				ACT_push_order( ACT_POMPE_ELEVATOR_LEFT , ACT_POMPE_NORMAL );
				ACT_push_order( ACT_POMPE_SLIDER_LEFT , ACT_POMPE_STOP );
				// vacuose
				ACT_push_order( ACT_CYLINDER_ELEVATOR_LEFT , ACT_CYLINDER_ELEVATOR_LEFT_LOCK_WITH_CYLINDER );
				if ((STOCKS_getNbModules(nb_cylinder_big_right)<4)){ACT_push_order( ACT_CYLINDER_SLOPE_RIGHT , ACT_CYLINDER_SLOPE_RIGHT_UNLOCK );}}
				state = check_act_status(ACT_QUEUE_Cylinder_elevator_left, ACTION_START_BRING_UP_CYLINDER,CHECK_STATUS_SLOPE, ERROR);
			}break;

		case CHECK_STATUS_SLOPE:
			//necessaire de le passer si non utilise?
			if (right_side){
				state = check_act_status(ACT_QUEUE_Cylinder_slope_left, CHECK_STATUS_SLOPE,ACTION_END_BRING_UP_CYLINDER, ERROR);
			}else{
				state = check_act_status(ACT_QUEUE_Cylinder_slope_right, CHECK_STATUS_SLOPE,ACTION_END_BRING_UP_CYLINDER, ERROR);
			}
			break;

		case ACTION_END_BRING_UP_CYLINDER:

			if (right_side){
				if (entrance) {
				//on finit le mouvement en simultanee avec la sortie du slider
				ACT_push_order( ACT_CYLINDER_SLIDER_RIGHT , ACT_CYLINDER_SLIDER_RIGHT_ALMOST_OUT );
				if ((STOCKS_getNbModules(nb_cylinder_big_right)<4)){
				ACT_push_order( ACT_CYLINDER_ELEVATOR_RIGHT , ACT_CYLINDER_ELEVATOR_RIGHT_TOP );}}
				state = check_act_status(ACT_QUEUE_Cylinder_elevator_right, ACTION_END_BRING_UP_CYLINDER,CHECK_STATUS_SLIDER, ERROR);
			}else{
				if (entrance) {
				ACT_push_order( ACT_CYLINDER_SLIDER_LEFT , ACT_CYLINDER_SLIDER_LEFT_ALMOST_OUT );
				if ((STOCKS_getNbModules(nb_cylinder_big_left)<4)){ACT_push_order( ACT_CYLINDER_ELEVATOR_LEFT , ACT_CYLINDER_ELEVATOR_LEFT_TOP );}}
				state = check_act_status(ACT_QUEUE_Cylinder_elevator_left, ACTION_END_BRING_UP_CYLINDER,CHECK_STATUS_SLIDER, ERROR);
			}break;

		case CHECK_STATUS_SLIDER:
			if (right_side){
				state = check_act_status(ACT_QUEUE_Cylinder_slider_right, CHECK_STATUS_SLIDER,ACTION_STOCK_UP_CYLINDER, ERROR);
			}else{
				state = check_act_status(ACT_QUEUE_Cylinder_slider_left, CHECK_STATUS_SLIDER,ACTION_STOCK_UP_CYLINDER, ERROR);
			}
			break;

		case ACTION_STOCK_UP_CYLINDER:

			if (right_side){
				if (entrance) {
				// on verouille le slope
				ACT_push_order( ACT_CYLINDER_SLOPE_RIGHT , ACT_CYLINDER_SLOPE_RIGHT_LOCK );

				//et en meme temps on prepare la collecte d'apres
				ACT_push_order( ACT_CYLINDER_SLIDER_LEFT , ACT_CYLINDER_SLIDER_LEFT_ALMOST_OUT_WITH_CYLINDER );}

				state = check_act_status(ACT_QUEUE_Cylinder_slope_right, ACTION_STOCK_UP_CYLINDER, CHECK_STATUS_OTHER_SLINDER_3, ERROR);

			}else{
				if (entrance) {
				ACT_push_order( ACT_CYLINDER_SLOPE_LEFT , ACT_CYLINDER_SLOPE_LEFT_LOCK );
				ACT_push_order( ACT_CYLINDER_SLIDER_RIGHT , ACT_CYLINDER_SLIDER_RIGHT_ALMOST_OUT_WITH_CYLINDER );}
				state = check_act_status(ACT_QUEUE_Cylinder_slope_right, ACTION_STOCK_UP_CYLINDER, CHECK_STATUS_OTHER_SLINDER_3, ERROR);}
			break;

		case CHECK_STATUS_OTHER_SLINDER_3:
			if (right_side){
				state = check_act_status(ACT_QUEUE_Cylinder_slider_left, CHECK_STATUS_OTHER_SLINDER_3,RESTART_CONDITION, ERROR);
			}else{
				state = check_act_status(ACT_QUEUE_Cylinder_slider_right, CHECK_STATUS_OTHER_SLINDER_3,RESTART_CONDITION, ERROR);
			}
			break;

		case RESTART_CONDITION:
			ROCKETS_removeModule(nb_cylinder_fusee);
			if ((ROCKETS_getNbModules(nb_cylinder_fusee)>0)){
				if (right_side){
					//on actualise le nombre de cylindre stocke dans le robot
					STOCKS_addModule(MODULE_POLY,nb_cylinder_big_right);
					//On change de cote
					right_side = FALSE;
					if ((STOCKS_getNbModules(nb_cylinder_big_right)<5)){
						//et on recommence
						state = ACTION_BRING_BACK_CYLINDER;
					}else{
						state = ACTION_FINAL_1_ON_3;
					}
				}else{
					STOCKS_addModule(MODULE_POLY,nb_cylinder_big_left);
					right_side = TRUE;
					if ((STOCKS_getNbModules(nb_cylinder_big_left)<5)){
					state = ACTION_BRING_BACK_CYLINDER;
					}else{state = ACTION_FINAL_1_ON_3;}}
			}else{
				state=DONE;}
				break;

		case ACTION_FINAL_1_ON_3:
			if (right_side){
				if (entrance) {
				ACT_push_order( ACT_POMPE_SLIDER_RIGHT , ACT_POMPE_NORMAL );
				ACT_push_order( ACT_CYLINDER_SLIDER_RIGHT , ACT_CYLINDER_SLIDER_RIGHT_OUT );}
				state = check_act_status(ACT_QUEUE_Cylinder_slider_right, ACTION_FINAL_1_ON_3, ACTION_FINAL_2_ON_3, ERROR);

			}else{
				if (entrance) {
				ACT_push_order( ACT_POMPE_SLIDER_LEFT , ACT_POMPE_NORMAL );
				ACT_push_order( ACT_CYLINDER_SLIDER_LEFT , ACT_CYLINDER_SLIDER_LEFT_OUT );}
				state = check_act_status(ACT_QUEUE_Cylinder_slider_left, ACTION_FINAL_1_ON_3, ACTION_FINAL_2_ON_3, ERROR);}
			break;

		case ACTION_FINAL_2_ON_3:
			//si on arrive ici car notre robot est plein mais il reste des cylindres dans la fusee
			//on doit quand meme prevoir la chute d'un cylindre
			if (right_side){
				if (entrance) {
				ACT_push_order( ACT_CYLINDER_SLIDER_RIGHT , ACT_CYLINDER_SLIDER_RIGHT_ALMOST_OUT_WITH_CYLINDER );}
				state = check_act_status(ACT_QUEUE_Cylinder_slope_right, ACTION_FINAL_2_ON_3, ACTION_FINAL_3_ON_3, ERROR);

			}else{
				if (entrance) {
				ACT_push_order( ACT_CYLINDER_SLIDER_LEFT , ACT_CYLINDER_SLIDER_LEFT_ALMOST_OUT_WITH_CYLINDER );}
				state = check_act_status(ACT_QUEUE_Cylinder_slope_left, ACTION_FINAL_2_ON_3, ACTION_FINAL_3_ON_3, ERROR);}
			break;

		case ACTION_FINAL_3_ON_3:
			if (entrance) {
			ACT_push_order( ACT_CYLINDER_SLIDER_LEFT , ACT_CYLINDER_SLIDER_LEFT_IN );
			ACT_push_order( ACT_CYLINDER_SLIDER_RIGHT , ACT_CYLINDER_SLIDER_RIGHT_IN );}
			state = check_act_status(ACT_QUEUE_Cylinder_slope_right, ACTION_FINAL_3_ON_3, CHECK_STATUS_OTHER_CYLINDER_FINAL, ERROR);
			break;

		case CHECK_STATUS_OTHER_CYLINDER_FINAL :
			state = check_act_status(ACT_QUEUE_Cylinder_slope_left, CHECK_STATUS_OTHER_CYLINDER_FINAL, DONE, ERROR);
			break;

		case DONE:
			if(entrance){
				set_sub_act_enable(SUB_HARRY_DEPOSE_MODULES, TRUE);
			}
			RESET_MAE();
			on_turning_point();
			return END_OK;
			break;

		case ERROR:
			RESET_MAE();
			on_turning_point();
			return NOT_HANDLED;
			break;

		default:
			if(entrance)
				debug_printf("default case in sub_harry_rocket_multicolor\n");
			break;
	}

	return IN_PROGRESS;
}


error_e init_all_actionneur(moduleStockLocation_e nb_cylinder_big_right,moduleStockLocation_e nb_cylinder_big_left){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_INIT_CYLINDER,
			INIT,
			INIT_ACTION_SLIDER_LEFT,
			INIT_ACTION_SLIDER_RIGHT,
			INIT_ACTION_ELEVATOR_LEFT,
			INIT_ACTION_ELEVATOR_RIGHT,
			INIT_ACTION_SLOPE_LEFT,
			INIT_ACTION_SLOPE_RIGHT,
			ERROR,
			DONE
		);

	//l'initalisation des pompes est-elle necesaire ?

	switch(state){
		case INIT:
			state = INIT_ACTION_SLIDER_RIGHT;
			break;

		case INIT_ACTION_SLIDER_RIGHT:
			if(entrance){
				if ((STOCKS_getNbModules(nb_cylinder_big_right)<6)){
					ACT_push_order( ACT_POMPE_SLIDER_RIGHT , ACT_POMPE_STOP );
				}
				ACT_push_order(ACT_CYLINDER_SLIDER_RIGHT, ACT_CYLINDER_SLIDER_RIGHT_IN);
			}
			if ((STOCKS_getNbModules(nb_cylinder_big_right)<5)){
				state= check_act_status(ACT_QUEUE_Cylinder_slider_right, INIT_ACTION_SLIDER_RIGHT, INIT_ACTION_ELEVATOR_RIGHT, ERROR);
			}else{
				state= check_act_status(ACT_QUEUE_Cylinder_slider_right, INIT_ACTION_SLIDER_RIGHT, INIT_ACTION_SLIDER_LEFT, ERROR);
			}
			break;

		case INIT_ACTION_ELEVATOR_RIGHT:
			if(entrance){
				if ((STOCKS_getNbModules(nb_cylinder_big_right)<5)){
					ACT_push_order( ACT_POMPE_ELEVATOR_RIGHT , ACT_POMPE_STOP );
				}
				ACT_push_order(ACT_CYLINDER_ELEVATOR_RIGHT, ACT_CYLINDER_ELEVATOR_RIGHT_BOTTOM);
			}
			if ((STOCKS_getNbModules(nb_cylinder_big_right)<4)){
				state= check_act_status(ACT_QUEUE_Cylinder_elevator_right, INIT_ACTION_ELEVATOR_RIGHT, INIT_ACTION_SLOPE_RIGHT, ERROR);
			}else{
				state= check_act_status(ACT_QUEUE_Cylinder_elevator_right, INIT_ACTION_ELEVATOR_RIGHT, INIT_ACTION_SLIDER_LEFT, ERROR);
			}
			break;

		case INIT_ACTION_SLOPE_RIGHT:
			if(entrance){
				ACT_push_order(ACT_CYLINDER_SLOPE_RIGHT, ACT_CYLINDER_SLOPE_RIGHT_UNLOCK);
			}
			state= check_act_status(ACT_QUEUE_Cylinder_slope_right, INIT_ACTION_SLOPE_RIGHT, INIT_ACTION_SLIDER_LEFT, ERROR);
			break;

		case INIT_ACTION_SLIDER_LEFT:
			if(entrance){
				if ((STOCKS_getNbModules(nb_cylinder_big_left)<6)){
					ACT_push_order( ACT_POMPE_SLIDER_LEFT , ACT_POMPE_STOP );
				}
				ACT_push_order(ACT_CYLINDER_SLIDER_LEFT, ACT_CYLINDER_SLIDER_LEFT_IN);}
			if ((STOCKS_getNbModules(nb_cylinder_big_left)<5)){
				state= check_act_status(ACT_QUEUE_Cylinder_slider_left, INIT_ACTION_SLIDER_LEFT, INIT_ACTION_ELEVATOR_LEFT, ERROR);
			}else{
				state= check_act_status(ACT_QUEUE_Cylinder_slider_left, INIT_ACTION_SLIDER_LEFT, DONE, ERROR);
			}
			break;

		case INIT_ACTION_ELEVATOR_LEFT:
			if(entrance){
				if ((STOCKS_getNbModules(nb_cylinder_big_right) < 5)){
					ACT_push_order( ACT_POMPE_ELEVATOR_LEFT, ACT_POMPE_STOP );
				}
				ACT_push_order(ACT_CYLINDER_ELEVATOR_LEFT, ACT_CYLINDER_ELEVATOR_LEFT_BOTTOM);
			}
			if ((STOCKS_getNbModules(nb_cylinder_big_left) < 4)){
				state= check_act_status(ACT_QUEUE_Cylinder_elevator_left, INIT_ACTION_ELEVATOR_LEFT, INIT_ACTION_SLOPE_LEFT, ERROR);
			}else{
				state= check_act_status(ACT_QUEUE_Cylinder_elevator_left, INIT_ACTION_ELEVATOR_LEFT, DONE, ERROR);
			}
			break;

		case INIT_ACTION_SLOPE_LEFT :
			if(entrance){
				ACT_push_order(ACT_CYLINDER_SLOPE_LEFT, ACT_CYLINDER_SLOPE_LEFT_UNLOCK);
			}
			state = check_act_status(ACT_QUEUE_Cylinder_slope_left, INIT_ACTION_SLOPE_LEFT, DONE, ERROR);
			break;

		case DONE:
			RESET_MAE();
			return END_OK;
			break;

		case ERROR:
			RESET_MAE();
			return NOT_HANDLED;
			break;

		default:
			if(entrance)
				debug_printf("default case in init_all_actionneur\n");
			break;

	}

	return IN_PROGRESS;
}
