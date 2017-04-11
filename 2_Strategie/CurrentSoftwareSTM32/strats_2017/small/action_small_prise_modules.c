#include "action_small.h"
#include "../../propulsion/movement.h"
#include "../../propulsion/astar.h"
#include "../../QS/QS_stateMachineHelper.h"
#include "../../QS/QS_outputlog.h"
#include "../../utils/actionChecker.h"
#include "../../utils/generic_functions.h"
#include "../../actuator/act_functions.h"
#include "../../avoidance.h"





error_e sub_anne_prise_modules_centre(ELEMENTS_property_e modules){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_ANNE_PRISE_MODULES_CENTRE,
			INIT,
			ERROR,
			DONE
		);

	switch(state){
		case INIT:
			state = INIT;
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
				debug_printf("default case in sub_anne_prise_modules_centre\n");
			break;
	}

	return IN_PROGRESS;
}

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
			state = try_going(400, COLOR_Y(1150), state, GO_TO_START_POINT, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case PATHFIND:
			state = ASTAR_try_going(400, COLOR_Y(1150), state, GO_TO_START_POINT, ERROR, FAST, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case GO_TO_START_POINT: //ajuster la distance fusée
			state = try_going(265, COLOR_Y(1150), state, TAKE_ROCKET, GET_OUT_ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case TAKE_ROCKET: // Execution des ordres actionneurs
			//state=check_sub_action_result(sub_anne_take_rocket(),state,GET_OUT,GET_OUT_ERROR);

			ROCKETS_removeModule(MODULE_ROCKET_MONO_OUR_SIDE);
			ROCKETS_removeModule(MODULE_ROCKET_MONO_OUR_SIDE);
			ROCKETS_removeModule(MODULE_ROCKET_MONO_OUR_SIDE);
			ROCKETS_removeModule(MODULE_ROCKET_MONO_OUR_SIDE);
			state = DONE;
			break;

		case GET_OUT:
			if(entrance){
				//set_sub_act_enable(SUB__DEPOSE_MODULES, TRUE);
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

error_e sub_anne_fusee_multicolor(ELEMENTS_property_e rocket){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_ANNE_FUSEE_MULTICOLOR,
			INIT,
			ALL_THE_GET_IN,
			GET_IN_MIDDLE_SQUARE,
			GET_IN_OUR_SQUARE,
			GET_IN_ADV_SQUARE,
			GET_IN_PATHFIND,

			GET_IN_FRONT_OF_ONE_ON_TWO,
			TURN_TO_POS,
			GET_IN_FRONT_OF_TWO_ON_TWO,
			ACTION,

			GET_OUT,
			GET_OUT_ERROR,
			AVANCE,
			AVANCE_ERROR,

			ERROR,
			DONE
						);

	switch(state){

		case INIT:{
			/*if(IHM_switchs_get(SWITCH_DISABLE_MODULE_RIGHT) && IHM_switchs_get(SWITCH_DISABLE_MODULE_LEFT)){
				state = ERROR; // Actionneurs désactivés
			}else*/ if((rocket == OUR_ELEMENT && ROCKETS_isEmpty(MODULE_ROCKET_MULTI_OUR_SIDE))
			|| (rocket == ADV_ELEMENT && ROCKETS_isEmpty(MODULE_ROCKET_MULTI_ADV_SIDE))){
				state = DONE;	 // On a déjà vidé cette fusée
			}else if((rocket == OUR_ELEMENT && ELEMENTS_get_flag(FLAG_SUB_HARRY_TAKE_CYLINDER_OUR_ROCKET_MULTI))
			|| (rocket == ADV_ELEMENT && ELEMENTS_get_flag(FLAG_SUB_HARRY_TAKE_CYLINDER_ADV_ROCKET_MULTI)) ){
				state = ERROR; //  est déjà en train de vider cette fusée
			}else{
				state = ALL_THE_GET_IN;
				//state = INIT_ALL_ACTIONNEUR;
				// On lève le flag de subaction
				if(rocket == OUR_ELEMENT){
					ELEMENTS_set_flag(FLAG_SUB_ANNE_TAKE_CYLINDER_OUR_ROCKET_MULTI, TRUE);
				}else{
					ELEMENTS_set_flag(FLAG_SUB_ANNE_TAKE_CYLINDER_ADV_ROCKET_MULTI, TRUE);
				}
						}
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
				if(i_am_in_square(700, 1450, 250, 1000)){
					state = GET_IN_OUR_SQUARE;
				}else if (i_am_in_square(350, 1150, 1000, 2000)){
					state = GET_IN_MIDDLE_SQUARE;
				}else if (i_am_in_square(700, 1450, 2000, 2750)){
					state = GET_IN_ADV_SQUARE;
				}else{
					state = GET_IN_PATHFIND;
				}
			}else{ // Prise fusee cote bleu
				if(i_am_in_square(700, 1450, 2000, 2750)){
					state = GET_IN_OUR_SQUARE;
				}else if (i_am_in_square(350, 1150, 1000, 2000)){
					state = GET_IN_MIDDLE_SQUARE;
				}else if (i_am_in_square(700, 1450, 250, 1000)){
					state = GET_IN_ADV_SQUARE;
				}else{
					state = GET_IN_PATHFIND;
				}
			}
			break;


		case GET_IN_OUR_SQUARE:
			if((rocket == OUR_ELEMENT && global.color == BLUE) || (rocket == ADV_ELEMENT && global.color == YELLOW)){
				state = try_going(1250, 500, state, GET_IN_FRONT_OF_ONE_ON_TWO, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_BRAKE);
			}else{
				state = try_going(1250, 2500, state, GET_IN_FRONT_OF_ONE_ON_TWO, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_BRAKE);
			}
			break;

		case GET_IN_MIDDLE_SQUARE:
			if((rocket == OUR_ELEMENT && global.color == BLUE) || (rocket == ADV_ELEMENT && global.color == YELLOW)){
				state = try_going(900, 950, state, GET_IN_OUR_SQUARE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_BRAKE);
			}else{
				state = try_going(900, 2050, state, GET_IN_OUR_SQUARE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_BRAKE);
			}
			break;

		case GET_IN_ADV_SQUARE:
			if((rocket == OUR_ELEMENT && global.color == BLUE) || (rocket == ADV_ELEMENT && global.color == YELLOW)){
				state = try_going(850, 2000, state, GET_IN_MIDDLE_SQUARE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_BRAKE);
			}else{
				state = try_going(850, 1000, state, GET_IN_MIDDLE_SQUARE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_BRAKE);
			}
			break;


		case GET_IN_PATHFIND:
			if((rocket == OUR_ELEMENT && global.color == BLUE) || (rocket == ADV_ELEMENT && global.color == YELLOW)){
				state = ASTAR_try_going(1250, 500, GET_IN_PATHFIND, GET_IN_FRONT_OF_ONE_ON_TWO, ERROR, FAST, ANY_WAY, NO_AVOIDANCE, END_AT_BRAKE);
			}else{
				state = ASTAR_try_going(1250, 2500, GET_IN_PATHFIND, GET_IN_FRONT_OF_ONE_ON_TWO, ERROR, FAST, ANY_WAY, NO_AVOIDANCE, END_AT_BRAKE);
			}
			break;


		case GET_IN_FRONT_OF_ONE_ON_TWO:
			if((rocket == OUR_ELEMENT && global.color == BLUE) || (rocket == ADV_ELEMENT && global.color == YELLOW)){
				state = try_going(1350, 360, state, TURN_TO_POS, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			}else{
				state = try_going(1350, 2640, state, TURN_TO_POS, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			}
			break;

		case TURN_TO_POS:
			if((rocket == OUR_ELEMENT && global.color == BLUE) || (rocket == ADV_ELEMENT && global.color == YELLOW)){
				state = try_go_angle(-PI4096/2, state, GET_IN_FRONT_OF_TWO_ON_TWO, ERROR, FAST, ANY_WAY, END_AT_LAST_POINT);
			}else{
				state = try_go_angle(PI4096/2, state, GET_IN_FRONT_OF_TWO_ON_TWO, ERROR, FAST, ANY_WAY, END_AT_LAST_POINT);
			}
			break;

		case GET_IN_FRONT_OF_TWO_ON_TWO:
			if(entrance){
				if((rocket == OUR_ELEMENT && global.color == BLUE) || (rocket == ADV_ELEMENT && global.color == YELLOW)){
					ACT_push_order(ACT_BIG_BALL_FRONT_LEFT, ACT_BIG_BALL_FRONT_LEFT_UP);
				}else{
					ACT_push_order(ACT_BIG_BALL_FRONT_RIGHT, ACT_BIG_BALL_FRONT_RIGHT_UP);
					}
				}
				if((rocket == OUR_ELEMENT && global.color == BLUE) || (rocket == ADV_ELEMENT && global.color == YELLOW)){
					state = try_going(1350,160, state, ACTION, ERROR, FAST, FORWARD, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
				}else{
					state = try_going(1350, 2840, state, ACTION, ERROR, FAST, FORWARD, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
				}
				break;



		case ACTION:{
			error_e resultAction = sub_act_anne_take_rocket_down_to_top(rocket, LEFT, RIGHT, LEFT, RIGHT);
			//error_e resultInit = init_all_actionneur(); // On init ou pas ? Si on le fail on part en failed_init // A ne pas faire ici
			resultAction = END_OK;
			if(resultAction == END_OK){
				state=GET_OUT;
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
				state=GET_OUT_ERROR;
			}
			}break;

		case GET_OUT:
			if((rocket == OUR_ELEMENT && global.color == BLUE) || (rocket == ADV_ELEMENT && global.color == YELLOW)){
				state=try_going(global.pos.x, 500, state, DONE, AVANCE, FAST, BACKWARD, NO_DODGE_AND_NO_WAIT, END_AT_BRAKE);
			}else{
				state=try_going(global.pos.x, 2500, state, DONE, AVANCE, FAST, BACKWARD, NO_DODGE_AND_NO_WAIT, END_AT_BRAKE);
			}
			if(ON_LEAVE()){
				if((rocket == OUR_ELEMENT && global.color == BLUE) || (rocket == ADV_ELEMENT && global.color == YELLOW)){
					ACT_push_order(ACT_BIG_BALL_FRONT_LEFT, ACT_BIG_BALL_FRONT_LEFT_DOWN);
				}else{
					ACT_push_order(ACT_BIG_BALL_FRONT_RIGHT, ACT_BIG_BALL_FRONT_RIGHT_DOWN);
				}
			}
			break;

		case GET_OUT_ERROR:
			if((rocket == OUR_ELEMENT && global.color == BLUE) || (rocket == ADV_ELEMENT && global.color == YELLOW)){
				state=try_going(global.pos.x, 500, state, ERROR, AVANCE, FAST, BACKWARD, NO_DODGE_AND_NO_WAIT, END_AT_BRAKE);
			}else{
				state=try_going(global.pos.x, 2500, state, ERROR, AVANCE, FAST, BACKWARD, NO_DODGE_AND_NO_WAIT, END_AT_BRAKE);
			}
			if(ON_LEAVE()){
				if((rocket == OUR_ELEMENT && global.color == BLUE) || (rocket == ADV_ELEMENT && global.color == YELLOW)){
					ACT_push_order(ACT_BIG_BALL_FRONT_LEFT, ACT_BIG_BALL_FRONT_LEFT_DOWN);
				}else{
					ACT_push_order(ACT_BIG_BALL_FRONT_RIGHT, ACT_BIG_BALL_FRONT_RIGHT_DOWN);
				}
			}
			break;

		case AVANCE:
			if((rocket == OUR_ELEMENT && global.color == BLUE) || (rocket == ADV_ELEMENT && global.color == YELLOW)){
				state=try_going(global.pos.x, 2500, state, GET_OUT, GET_OUT, FAST, BACKWARD, NO_DODGE_AND_NO_WAIT, END_AT_BRAKE);
			}else{
				state=try_going(global.pos.x, 2780, state, GET_OUT, GET_OUT, FAST, BACKWARD, NO_DODGE_AND_NO_WAIT, END_AT_BRAKE);
			}
			break;

		case AVANCE_ERROR:
			if((rocket == OUR_ELEMENT && global.color == BLUE) || (rocket == ADV_ELEMENT && global.color == YELLOW)){
				state=try_going(global.pos.x, 220, state, GET_OUT, GET_OUT, FAST, BACKWARD, NO_DODGE_AND_NO_WAIT, END_AT_BRAKE);
			}else{
				state=try_going(global.pos.x, 2780, state, GET_OUT, GET_OUT, FAST, BACKWARD, NO_DODGE_AND_NO_WAIT, END_AT_BRAKE);
			}
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
				debug_printf("default case in sub_anne_fusee_multicolor\n");
			break;
	}

	return IN_PROGRESS;
}
