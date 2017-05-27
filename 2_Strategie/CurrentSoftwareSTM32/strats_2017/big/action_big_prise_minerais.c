#warning 'x = 1700 pour cratere sud ! '

#include "action_big.h"
#include "../../propulsion/movement.h"
#include "../../propulsion/astar.h"
#include "../../QS/QS_stateMachineHelper.h"
#include "../../QS/QS_outputlog.h"
#include "../../QS/QS_types.h"
#include "../../QS/QS_IHM.h"
#include  "../../utils/generic_functions.h"
#include "../../actuator/act_functions.h"
#include "../../actuator/queue.h"
#include "../../utils/actionChecker.h"
#include "../../high_level_strat.h"

//sub actionneurs
error_e sub_act_big_take(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_START_TAKE,
			INIT,
			ERROR,
			DONE
		);

	switch(state){
		case INIT:{

			Uint8 state1=INIT;
			Uint8 state2=INIT;
			Uint8 state3=INIT;
			Uint8 state4=INIT;
			Uint8 state5=INIT;

			if(entrance){
				ACT_push_order(ACT_BIG_BALL_FRONT_LEFT,  ACT_BIG_BALL_FRONT_LEFT_UP);
				ACT_push_order(ACT_BIG_BALL_FRONT_RIGHT,  ACT_BIG_BALL_FRONT_RIGHT_UP);
				ACT_push_order(ACT_BIG_BALL_BACK_LEFT,  ACT_BIG_BALL_FRONT_LEFT_UP);
				ACT_push_order(ACT_BIG_BALL_BACK_RIGHT,  ACT_BIG_BALL_BACK_RIGHT_UP);
				ACT_push_order(ACT_BEARING_BALL_WHEEL,  ACT_BEARING_BALL_WHEEL_UP);
			}

			state1= check_act_status(ACT_QUEUE_Bearing_ball_wheel, state1, DONE, ERROR);
			state2= check_act_status(ACT_QUEUE_Big_bearing_back_left, state2, DONE, ERROR);
			state3= check_act_status(ACT_QUEUE_Big_bearing_back_right, state3, DONE, ERROR);
			state4= check_act_status(ACT_QUEUE_Big_bearing_front_left, state4, DONE, ERROR);
			state5= check_act_status(ACT_QUEUE_Big_bearing_front_right, state5, DONE, ERROR);

		if((state1==ERROR)||(state2==ERROR)||(state3==ERROR)||(state4==ERROR)||(state5==ERROR)){
			state=ERROR;
		}
		else if((state1==DONE)&&(state2==DONE)&&(state3==DONE)&&(state4==DONE)&&(state5==DONE)){
			state=DONE;
		}
	}break;


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
				debug_printf("default case in sub_act_big_take\n");
			break;
	}
	return IN_PROGRESS;
}


error_e sub_act_big_off(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_END_OF_STRAT,
			INIT,
			ERROR,
			DONE
		);


	switch(state){
		case INIT:{
			Uint8 state1=INIT;
			Uint8 state2=INIT;
			Uint8 state3=INIT;
			Uint8 state4=INIT;
			Uint8 state5=INIT;

			if(entrance){
				ACT_push_order(ACT_BIG_BALL_FRONT_LEFT,  ACT_BIG_BALL_FRONT_LEFT_DOWN);
				ACT_push_order(ACT_BIG_BALL_FRONT_RIGHT,  ACT_BIG_BALL_FRONT_RIGHT_DOWN);
				ACT_push_order(ACT_BIG_BALL_BACK_LEFT,  ACT_BIG_BALL_FRONT_LEFT_DOWN);
				ACT_push_order(ACT_BIG_BALL_BACK_RIGHT,  ACT_BIG_BALL_BACK_RIGHT_DOWN);
				ACT_push_order(ACT_BEARING_BALL_WHEEL,  ACT_BEARING_BALL_WHEEL_UP);
			}

			state1= check_act_status(ACT_QUEUE_Bearing_ball_wheel, state1, DONE, ERROR);
			state2= check_act_status(ACT_QUEUE_Big_bearing_back_left, state2, DONE, ERROR);
			state3= check_act_status(ACT_QUEUE_Big_bearing_back_right, state3, DONE, ERROR);
			state4= check_act_status(ACT_QUEUE_Big_bearing_front_left, state4, DONE, ERROR);
			state5= check_act_status(ACT_QUEUE_Big_bearing_front_right, state5, DONE, ERROR);

		if((state1==ERROR)||(state2==ERROR)||(state3==ERROR)||(state4==ERROR)||(state5==ERROR)){
			state=ERROR;
		}
		else if((state1==DONE)&&(state2==DONE)&&(state3==DONE)&&(state4==DONE)&&(state5==DONE)){
			state=DONE;
		}
		}break;

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
				debug_printf("default case in sub_act_big_off\n");
			break;
	}
	return IN_PROGRESS;
}


// sub petit cratère
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#warning 'debut de cratere'

error_e sub_harry_take_north_little_crater(ELEMENTS_property_e minerais){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_TAKE_NORTH_LITTLE_CRATER,
			INIT,
			GET_IN,
			GO_TO_POSITION,
			TURN_TO_RUSH,
			DOWN_SYSTEM,
			GO_TO_CLEAT,
			GO_TO_CLEAT_ERROR,
			WAIT,
			MOVE_BACK,
			GET_OUT,
			DONE,
			ERROR
		);
	time32_t time = 4000;
	time32_t start;

	switch(state){
		case INIT:
			if(IHM_switchs_get(SWITCH_DISABLE_ORE)){
				state = ERROR; // L'actionneur minerais a été désactivé
			}else if(ELEMENTS_get_flag(FLAG_OUR_NORTH_CRATER_IS_TAKEN)&&minerais == OUR_ELEMENT){
				state = DONE;
			}else if(ELEMENTS_get_flag(FLAG_ADV_NORTH_CRATER_IS_TAKEN)&&minerais == ADV_ELEMENT){
							state = DONE;
			}else if(ELEMENTS_get_flag(FLAG_SUB_ANNE_TAKE_CYLINDER_OUR_CENTER)){
				state=ERROR;
			}else{
				state=GET_IN;


				// On lève le flag de subaction
				if(minerais == OUR_ELEMENT){
					ELEMENTS_set_flag(FLAG_SUB_HARRY_OUR_NORTH_CRATER, TRUE);
				}else{
					ELEMENTS_set_flag(FLAG_SUB_HARRY_ADV_NORTH_CRATER, TRUE);
				}

			}
			break;

		case GET_IN:
			state=check_sub_action_result(sub_harry_get_in_north_little_crater(minerais), state, GO_TO_POSITION, ERROR);
			break;

		case GO_TO_POSITION:
			if((minerais == OUR_ELEMENT && global.color == BLUE) || (minerais == ADV_ELEMENT && global.color == YELLOW)){
				state=try_going(950, 650, state, TURN_TO_RUSH, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			}else{
				state=try_going(950, 2350, state, TURN_TO_RUSH, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			}
			break;

		case TURN_TO_RUSH:
			if((minerais == OUR_ELEMENT && global.color == BLUE) || (minerais == ADV_ELEMENT && global.color == YELLOW)){
				state=try_go_angle(0, state, DOWN_SYSTEM, ERROR, FAST, ANY_WAY, END_AT_LAST_POINT);
			}else{
				state=try_go_angle(180, state, DOWN_SYSTEM, ERROR, FAST, ANY_WAY, END_AT_LAST_POINT);
			}
			break;

		case DOWN_SYSTEM:{
			Uint8 state1=state;
			Uint8 state2=state;
			if (entrance){
				ACT_push_order_with_param(ACT_ORE_ROLLER_FOAM, ACT_ORE_ROLLER_FOAM_RUN, ACT_ROLLER_FOAM_SPEED_RUN);
				ACT_push_order(ACT_ORE_ROLLER_ARM, ACT_ORE_ROLLER_ARM_OUT);
				ACT_push_order(ACT_ORE_WALL, ACT_ORE_WALL_OUT);
			}
			state1=check_act_status(ACT_QUEUE_Ore_roller_arm, DOWN_SYSTEM, GO_TO_CLEAT, ERROR);
			state2=check_act_status(ACT_QUEUE_Ore_wall, DOWN_SYSTEM, GO_TO_CLEAT, ERROR);

			if((state1==ERROR)||(state2==ERROR)){
				state=ERROR;
			}else if ((state1==GO_TO_CLEAT)&&(state2==GO_TO_CLEAT)){
				state=GO_TO_CLEAT;
			}else{ // Add a else just to be secure
				state=GO_TO_CLEAT;
			}
		}
		break;

		case GO_TO_CLEAT:
			if((minerais == OUR_ELEMENT && global.color == BLUE) || (minerais == ADV_ELEMENT && global.color == YELLOW)){
				state=try_going(745, 650, state, WAIT, MOVE_BACK, SLOW, BACKWARD, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			}else{
				state=try_going(745, 2350, state, WAIT, MOVE_BACK, SLOW, BACKWARD, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			}
			break;
			/*
			 * attente pour rammasser les balles
			 */
		case WAIT:
			if(entrance){
				start = global.match_time;
			}
			if(global.match_time == start + time){
				state = MOVE_BACK;
			}
			break;

			/*
			 * sortir du cratere
			 */
		case MOVE_BACK:
			if((minerais == OUR_ELEMENT && global.color == BLUE) || (minerais == ADV_ELEMENT && global.color == YELLOW)){
				state=try_going(800, 650, state, GET_OUT, GO_TO_CLEAT_ERROR, SLOW, FORWARD, NO_DODGE_AND_NO_WAIT, END_AT_BRAKE);
			}else{
				state=try_going(800, 2350, state, GET_OUT, GO_TO_CLEAT_ERROR, SLOW, FORWARD, NO_DODGE_AND_NO_WAIT, END_AT_BRAKE);
			}
			if(ON_LEAVE()){
				ACT_push_order(ACT_ORE_ROLLER_FOAM, ACT_ORE_ROLLER_FOAM_STOP);
				ACT_push_order(ACT_ORE_ROLLER_ARM, ACT_ORE_ROLLER_ARM_IDLE);
				ACT_push_order(ACT_ORE_WALL, ACT_ORE_WALL_IDLE);
			}
			break;

			/*
			 * sortie de la sub
			 */
		case GET_OUT:
			if((minerais == OUR_ELEMENT && global.color == BLUE) || (minerais == ADV_ELEMENT && global.color == YELLOW)){
				state=try_going(950, 650, state, DONE, DONE, FAST, FORWARD, NO_DODGE_AND_NO_WAIT, END_AT_BRAKE);
			}else{
				state=try_going(950, 2350, state, DONE, DONE, FAST, FORWARD, NO_DODGE_AND_NO_WAIT, END_AT_BRAKE);
			}
			break;

			/*
			 * si erreur, evite de wait
			 */
		case GO_TO_CLEAT_ERROR:
			if((minerais == OUR_ELEMENT && global.color == BLUE) || (minerais == ADV_ELEMENT && global.color == YELLOW)){
				state=try_going(745, 650, state, MOVE_BACK, MOVE_BACK, SLOW, BACKWARD, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			}else{
				state=try_going(745, 2350, state, MOVE_BACK, MOVE_BACK, SLOW, BACKWARD, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			}
			break;

		case ERROR:
			RESET_MAE();
			on_turning_point();

			if(minerais == OUR_ELEMENT){
				ELEMENTS_set_flag(FLAG_SUB_HARRY_OUR_NORTH_CRATER, FALSE);
			}else{
				ELEMENTS_set_flag(FLAG_SUB_HARRY_ADV_NORTH_CRATER, FALSE);
			}

			return NOT_HANDLED;
			break;

		case DONE:
			RESET_MAE();
			on_turning_point();

			if(minerais == OUR_ELEMENT){
				ELEMENTS_set_flag(FLAG_SUB_HARRY_OUR_NORTH_CRATER, FALSE);	// flag subaction
				ELEMENTS_set_flag(FLAG_OUR_NORTH_CRATER_IS_TAKEN, TRUE);	// flag element
			}else{
				ELEMENTS_set_flag(FLAG_SUB_HARRY_ADV_NORTH_CRATER, FALSE);	// flag subaction
				ELEMENTS_set_flag(FLAG_ADV_NORTH_CRATER_IS_TAKEN, TRUE);	// flag element
			}
			//ELEMENTS_set_flag(FLAG_HARRY_STOMACH_IS_FULL, TRUE);

			return END_OK;
			break;

		default:
			if(entrance)
				debug_printf("default case in sub_harry_take_north_little_crater\n");
			break;
	}

	return IN_PROGRESS;
}


#warning 'debut de cratere 2 '

error_e sub_harry_get_in_north_little_crater(ELEMENTS_property_e minerais){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_GET_IN_NORTH_LITTLE_CRATER,
			INIT,
			GET_IN_OUR_SQUARE,
			GET_IN_MIDDLE_SQUARE,
			GET_IN_ADV_SQUARE,
			PATHFIND,
			DONE,
			ERROR
		);

	switch(state){
		case INIT:
			// Prise cratère côté jaune
			if((minerais == OUR_ELEMENT && global.color == BLUE) || (minerais == ADV_ELEMENT && global.color == YELLOW)){
				if(i_am_in_square(800, 1400, 350, 900)){
					state = DONE;//GET_IN_OUR_SQUARE;
				}else if (i_am_in_square(350, 1100, 900, 2100)){
					state = GET_IN_MIDDLE_SQUARE;
				}else if (i_am_in_square(800, 1400, 2100, 2700)){
					state = GET_IN_ADV_SQUARE;
				}else{
					state = PATHFIND;
				}
			}else{ // Prise cratère coté bleu
				if(i_am_in_square(800, 1400, 2100, 2700)){
					state = DONE;//GET_IN_OUR_SQUARE;
				}else if (i_am_in_square(350, 1100, 900, 2100)){
					state = GET_IN_MIDDLE_SQUARE;
				}else if (i_am_in_square(800, 1400, 350, 900)){
					state = GET_IN_ADV_SQUARE;
				}else{
					state = PATHFIND;
				}
			}
			break;

		case GET_IN_OUR_SQUARE:
			if((minerais == OUR_ELEMENT && global.color == BLUE) || (minerais == ADV_ELEMENT && global.color == YELLOW)){
				state = try_going(1000, 650, state, DONE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_BRAKE);
				//pas utiliser pour l'instant je préfère le mettre dans la fonction principale
			}else{
				state = try_going(1000, 2350, state, DONE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_BRAKE);
				//pas utiliser pour l'instant je préfère le mettre dans la fonction principale
			}
			break;

		case GET_IN_MIDDLE_SQUARE:
			if((minerais == OUR_ELEMENT && global.color == BLUE) || (minerais == ADV_ELEMENT && global.color == YELLOW)){
				state = try_going(900, 1000, state, DONE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_BRAKE);
			}else{
				state = try_going(900, 2000, state, DONE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_BRAKE);
			}
			break;

		case GET_IN_ADV_SQUARE:
			if((minerais == OUR_ELEMENT && global.color == BLUE) || (minerais == ADV_ELEMENT && global.color == YELLOW)){
				state = try_going(850, 2000, state, GET_IN_MIDDLE_SQUARE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_BRAKE);
			}else{
				state = try_going(850, 1000, state, GET_IN_MIDDLE_SQUARE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_BRAKE);
			}
			break;

		case PATHFIND:
			if((minerais == OUR_ELEMENT && global.color == BLUE) || (minerais == ADV_ELEMENT && global.color == YELLOW)){
				state = ASTAR_try_going(850, 650, state, DONE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_BRAKE);
			}else{
				state = ASTAR_try_going(850, 2350, state, DONE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_BRAKE);
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

		default:
			if(entrance)
				debug_printf("default case in sub_harry_get_in_north_little_crater\n");
			break;
	}

	return IN_PROGRESS;
}

#warning 'debut de cratere 3 '
error_e sub_harry_take_south_little_crater(ELEMENTS_property_e minerais){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_TAKE_SOUTH_LITTLE_CRATER,
			INIT,
			GET_IN,
			GO_TO_POSITION,
			TURN_TO_RUSH,
			DOWN_SYSTEM,
			GO_TO_CENTER,
			MOVE_BACK,
			GET_OUT,
			DONE,
			ERROR
		);

	switch(state){
		case INIT:
			if(IHM_switchs_get(SWITCH_DISABLE_ORE)){
				state = ERROR; // L'actionneur minerais a été désactivé
			}else if(ELEMENTS_get_flag(FLAG_OUR_SOUTH_CRATER_IS_TAKEN)&&minerais == OUR_ELEMENT){
				state = DONE;
			}else if(ELEMENTS_get_flag(FLAG_ADV_SOUTH_CRATER_IS_TAKEN)&&minerais == ADV_ELEMENT){
							state = DONE;
			}else if((minerais == OUR_ELEMENT && (ELEMENTS_get_flag(FLAG_SUB_ANNE_TAKE_CYLINDER_SOUTH_UNI) || ELEMENTS_get_flag(FLAG_SUB_ANNE_DEPOSE_CYLINDER_OUR_DIAGONAL)))
					|| (minerais == ADV_ELEMENT && ELEMENTS_get_flag(FLAG_SUB_ANNE_DEPOSE_CYLINDER_ADV_DIAGONAL))){
				state=ERROR;
			}else{
				state=GET_IN;


				// On lève le flag de subaction
				if(minerais == OUR_ELEMENT){
					ELEMENTS_set_flag(FLAG_SUB_HARRY_OUR_SOUTH_CRATER, TRUE);
				}else{
					ELEMENTS_set_flag(FLAG_SUB_HARRY_ADV_SOUTH_CRATER, TRUE);
				}

			}
			break;

		case GET_IN:
			state=check_sub_action_result(sub_harry_get_in_south_little_crater(minerais), state, GO_TO_POSITION, ERROR);
			break;

		case GO_TO_POSITION:
			if((minerais == OUR_ELEMENT && global.color == BLUE) || (minerais == ADV_ELEMENT && global.color == YELLOW)){
				state=try_going(1680, 800, state, TURN_TO_RUSH, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			}else{
				state=try_going(1680, 2200, state, TURN_TO_RUSH, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			}
			break;

		case TURN_TO_RUSH:
			if((minerais == OUR_ELEMENT && global.color == BLUE) || (minerais == ADV_ELEMENT && global.color == YELLOW)){
				state=try_go_angle(-2*PI4096/3, state, DOWN_SYSTEM, ERROR, FAST, ANY_WAY, END_AT_LAST_POINT);
			}else{
				state=try_go_angle(2*PI4096/3, state, DOWN_SYSTEM, ERROR, FAST, ANY_WAY, END_AT_LAST_POINT);
			}
			break;

		case DOWN_SYSTEM:{
			Uint8 state1=state;
			Uint8 state2=state;
			if (entrance){
				ACT_push_order_with_param(ACT_ORE_ROLLER_FOAM, ACT_ORE_ROLLER_FOAM_RUN, ACT_ROLLER_FOAM_SPEED_RUN);
				ACT_push_order(ACT_ORE_ROLLER_ARM, ACT_ORE_ROLLER_ARM_OUT);
				ACT_push_order(ACT_ORE_WALL, ACT_ORE_WALL_OUT);
			}

			state1=check_act_status(ACT_QUEUE_Ore_roller_arm, DOWN_SYSTEM, END_OK, ERROR);
			state2=check_act_status(ACT_QUEUE_Ore_wall, DOWN_SYSTEM, END_OK, ERROR);

			if((state1==ERROR)||(state2==ERROR)){
				state=ERROR;
			}else if ((state1==END_OK)&&(state2==END_OK)){
				state=DONE;
			}else{ // Add a else just to be secure
				state = DONE;
			}
		}
		break;

		case GO_TO_CENTER:
			if((minerais == OUR_ELEMENT && global.color == BLUE) || (minerais == ADV_ELEMENT && global.color == YELLOW)){
				state=try_going(1820, 980, state, MOVE_BACK, MOVE_BACK, SLOW, BACKWARD, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			}else{
				state=try_going(1820, 2020, state, MOVE_BACK, MOVE_BACK, SLOW, BACKWARD, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			}
			break;

		case MOVE_BACK:
			if((minerais == OUR_ELEMENT && global.color == BLUE) || (minerais == ADV_ELEMENT && global.color == YELLOW)){
				state=try_going(1680, 800, state, GET_OUT, GO_TO_CENTER, SLOW, FORWARD, NO_DODGE_AND_NO_WAIT, END_AT_BRAKE);
			}else{
				state=try_going(1680, 2200, state, GET_OUT, GO_TO_CENTER, SLOW, FORWARD, NO_DODGE_AND_NO_WAIT, END_AT_BRAKE);
			}
			if(ON_LEAVE()){
				ACT_push_order(ACT_ORE_ROLLER_FOAM, ACT_ORE_ROLLER_FOAM_STOP);
				ACT_push_order(ACT_ORE_ROLLER_ARM, ACT_ORE_ROLLER_ARM_IDLE);
				ACT_push_order(ACT_ORE_WALL, ACT_ORE_WALL_IDLE);
			}
			break;

		case GET_OUT:
			if((minerais == OUR_ELEMENT && global.color == BLUE) || (minerais == ADV_ELEMENT && global.color == YELLOW)){
				state=try_going(1500, 650, state, DONE, DONE, FAST, FORWARD, NO_DODGE_AND_NO_WAIT, END_AT_BRAKE);
			}else{
				state=try_going(1500, 2350, state, DONE, DONE, FAST, FORWARD, NO_DODGE_AND_NO_WAIT, END_AT_BRAKE);
			}
			break;

		case ERROR:
			RESET_MAE();
			on_turning_point();

			if(minerais == OUR_ELEMENT){
				ELEMENTS_set_flag(FLAG_SUB_HARRY_OUR_SOUTH_CRATER, FALSE);
			}else{
				ELEMENTS_set_flag(FLAG_SUB_HARRY_ADV_SOUTH_CRATER, FALSE);
			}

			return NOT_HANDLED;
			break;

		case DONE:
			RESET_MAE();
			on_turning_point();


			if(minerais == OUR_ELEMENT){
				ELEMENTS_set_flag(FLAG_SUB_HARRY_OUR_SOUTH_CRATER, FALSE); // flag subaction
				ELEMENTS_set_flag(FLAG_OUR_SOUTH_CRATER_IS_TAKEN, TRUE);  // flag element
			}else{
				ELEMENTS_set_flag(FLAG_SUB_HARRY_ADV_SOUTH_CRATER, FALSE);	// flag subaction
				ELEMENTS_set_flag(FLAG_ADV_SOUTH_CRATER_IS_TAKEN, TRUE);	// flag element
			}
			//ELEMENTS_set_flag(FLAG_HARRY_STOMACH_IS_FULL, TRUE);

			return END_OK;
			break;

		default:
			if(entrance)
				debug_printf("default case in sub_harry_take_south_little_crater\n");
			break;
	}

	return IN_PROGRESS;
}

#warning 'debut de cratere 4 '

error_e sub_harry_get_in_south_little_crater(ELEMENTS_property_e minerais){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_GET_IN_SOUTH_LITTLE_CRATER,
			INIT,
			GET_IN_OUR_SQUARE,
			GET_IN_MIDDLE_SQUARE,
			GET_IN_ADV_SQUARE,
			PATHFIND,
			DONE,
			ERROR
		);

	switch(state){
		case INIT:
			// Prise cratère coté jaune
			if((minerais == OUR_ELEMENT && global.color == BLUE) || (minerais == ADV_ELEMENT && global.color == YELLOW)){
				if(i_am_in_square(800, 1400, 350, 900)){
					state = GET_IN_OUR_SQUARE; // DONE;
				}else if (i_am_in_square(350, 1050, 900, 2100)){
					state = GET_IN_MIDDLE_SQUARE;
				}else if (i_am_in_square(800, 1400, 2100, 2700)){
					state = GET_IN_ADV_SQUARE;
				}else{
					state = PATHFIND;
				}
			}else{	// Prise cratère coté bleu
				if(i_am_in_square(800, 1400, 2100, 2700)){
					state = DONE;//GET_IN_OUR_SQUARE;
				}else if (i_am_in_square(350, 1050, 900, 2100)){
					state = GET_IN_MIDDLE_SQUARE;
				}else if (i_am_in_square(800, 1400, 350, 900)){
					state = GET_IN_ADV_SQUARE;
				}else{
					state = PATHFIND;
				}
			}
			break;

		case GET_IN_OUR_SQUARE:
			if((minerais == OUR_ELEMENT && global.color == BLUE) || (minerais == ADV_ELEMENT && global.color == YELLOW)){
				state = try_going(1590, 745, state, DONE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_BRAKE);
			}else{
				state = try_going(1590, 2400, state, DONE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_BRAKE);
			}
			break;

		case GET_IN_MIDDLE_SQUARE:
			if((minerais == OUR_ELEMENT && global.color == BLUE) || (minerais == ADV_ELEMENT && global.color == YELLOW)){
				state = try_going(900, 1000, state, GET_IN_OUR_SQUARE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_BRAKE);
			}else{
				state = try_going(900, 2000, state, GET_IN_OUR_SQUARE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_BRAKE);
			}
			break;

		case GET_IN_ADV_SQUARE:
			if((minerais == OUR_ELEMENT && global.color == BLUE) || (minerais == ADV_ELEMENT && global.color == YELLOW)){
				state = try_going(850, 2000, state, GET_IN_MIDDLE_SQUARE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_BRAKE);
			}else{
				state = try_going(850, 1000, state, GET_IN_MIDDLE_SQUARE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_BRAKE);
			}
			break;

		case PATHFIND:
			if((minerais == OUR_ELEMENT && global.color == BLUE) || (minerais == ADV_ELEMENT && global.color == YELLOW)){
				state = ASTAR_try_going(1590, 745, state, DONE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_BRAKE);
			}else{
				state = ASTAR_try_going(1590, 2400, state, DONE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_BRAKE);
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

		default:
			if(entrance)
				debug_printf("default case in sub_harry_get_in_south_little_crater\n");
			break;
	}

	return IN_PROGRESS;
}




void mae_harry_take_ore_during_dispose_module(mae_harry_take_ore_during_dispose_module_e event)
{
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_TAKE_ORE_DURING_DISPOSE,
			INIT,
			WAIT_FOR_TAKING_ORDER,
			MOVING_ACTUATORS,
			TAKING,
			STORE_ACTUATORS,
			FAIL);
	static error_e state1=IN_PROGRESS;
	static error_e state2=IN_PROGRESS;
	switch(state)
	{
		case INIT:
			if(!IHM_switchs_get(SWITCH_OUR_ORES))
				state = FAIL;
			else
				state = WAIT_FOR_TAKING_ORDER;
			break;
		case WAIT_FOR_TAKING_ORDER:
			if(event == ORE_DURING_DISPOSE_YOU_CAN_TAKE_NOW)
				state = MOVING_ACTUATORS;
			break;
		case MOVING_ACTUATORS:
			if (entrance){
				state1=IN_PROGRESS;
				state2=IN_PROGRESS;
				ACT_push_order_with_param(ACT_ORE_ROLLER_FOAM, ACT_ORE_ROLLER_FOAM_RUN, ACT_ROLLER_FOAM_SPEED_RUN);
				ACT_push_order(ACT_ORE_ROLLER_ARM, ACT_ORE_ROLLER_ARM_OUT);
				ACT_push_order(ACT_ORE_WALL, ACT_ORE_WALL_OUT);
			}

			if(state1 == IN_PROGRESS)
				state1=check_act_status(ACT_QUEUE_Ore_roller_arm, IN_PROGRESS, END_OK, NOT_HANDLED);
			if(state2 == IN_PROGRESS)
				state2=check_act_status(ACT_QUEUE_Ore_wall, IN_PROGRESS, END_OK, NOT_HANDLED);

			if(state1 != IN_PROGRESS && state2 != IN_PROGRESS)
			{
				if ((state1==END_OK)&&(state2==END_OK))
					state=TAKING;
				else
					state=STORE_ACTUATORS;
			}
			break;
		case TAKING:
			if(entrance)
				ELEMENTS_set_flag(FLAG_OUR_SOUTH_CRATER_IS_TAKEN, TRUE);
			if(event == ORE_DURING_DISPOSE_YOU_MUST_STOP_NOW)
				state = STORE_ACTUATORS;
			break;
		case STORE_ACTUATORS:
			if(entrance)
			{
				state1=IN_PROGRESS;
				state2=IN_PROGRESS;
				ACT_push_order_with_param(ACT_ORE_ROLLER_FOAM, ACT_ORE_ROLLER_FOAM_STOP, ACT_ROLLER_FOAM_SPEED_RUN);
				ACT_push_order(ACT_ORE_ROLLER_ARM, ACT_ORE_ROLLER_ARM_IN);
				ACT_push_order(ACT_ORE_WALL, ACT_ORE_WALL_IN);
			}
			if(state1 == IN_PROGRESS)
				state1=check_act_status(ACT_QUEUE_Ore_roller_arm, IN_PROGRESS, END_OK, NOT_HANDLED);
			if(state2 == IN_PROGRESS)
				state2=check_act_status(ACT_QUEUE_Ore_wall, IN_PROGRESS, END_OK, NOT_HANDLED);

			if(state1 != IN_PROGRESS && state2 != IN_PROGRESS)
			{
				if ((state1==END_OK)&&(state2==END_OK))
					state = WAIT_FOR_TAKING_ORDER;
				else
					state = FAIL;
			}
			break;
		case FAIL:
			//etat puits :(
			break;
	}

}



// sub_harry_rammassage
error_e sub_harry_take_minerais(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_TAKE_MINERAIS,
			INIT,
			OPEN_AND_RUN,
			RAMASSAGE,
			FIN_RAMASSAGE,
			ERROR,
			DONE
		);
	static time32_t time;
	static error_e state1=IN_PROGRESS;
	static error_e state2=IN_PROGRESS;
	error_e ret;
	ret = IN_PROGRESS;
	switch(state){
	/*
	 * active puis stop, les actionneurs de prise de minerais
	 * 26/05/2017
	 */
		case INIT:{
			if(IHM_switchs_get(SWITCH_DISABLE_ORE))
				state = ERROR;
			else
				state = OPEN_AND_RUN;
		}break;

		case OPEN_AND_RUN:
			if (entrance){
				state1=IN_PROGRESS;
				state2=IN_PROGRESS;
				ACT_push_order_with_param(ACT_ORE_ROLLER_FOAM, ACT_ORE_ROLLER_FOAM_RUN, ACT_ROLLER_FOAM_SPEED_RUN);
				ACT_push_order(ACT_ORE_ROLLER_ARM, ACT_ORE_ROLLER_ARM_OUT);
				ACT_push_order(ACT_ORE_WALL, ACT_ORE_WALL_OUT);
			}

			if(state1 == IN_PROGRESS)
				state1=check_act_status(ACT_QUEUE_Ore_roller_arm, IN_PROGRESS, END_OK, NOT_HANDLED);
			if(state2 == IN_PROGRESS)
				state2=check_act_status(ACT_QUEUE_Ore_wall, IN_PROGRESS, END_OK, NOT_HANDLED);

			if(state1 != IN_PROGRESS && state2 != IN_PROGRESS)
			{
				if((state1==NOT_HANDLED)||(state2==NOT_HANDLED)){
					state=FIN_RAMASSAGE;
				}else if ((state1==END_OK)&&(state2==END_OK)){
					state=RAMASSAGE;
				}else{ // Add a else just to be secure
					state = DONE;
				}
			}
			break;
		case RAMASSAGE:
			if(entrance){
				time = global.match_time;






			}
			if((global.match_time - time)>=5000){
				{
					ELEMENTS_set_flag(FLAG_OUR_SOUTH_CRATER_IS_TAKEN, TRUE);
					state = FIN_RAMASSAGE;
				}

			}
			break;

		case FIN_RAMASSAGE:{
			if(entrance)
			{
				state1=IN_PROGRESS;
				state2=IN_PROGRESS;
				ACT_push_order_with_param(ACT_ORE_ROLLER_FOAM, ACT_ORE_ROLLER_FOAM_STOP, ACT_ROLLER_FOAM_SPEED_RUN);
				ACT_push_order(ACT_ORE_ROLLER_ARM, ACT_ORE_ROLLER_ARM_IN);
				ACT_push_order(ACT_ORE_WALL, ACT_ORE_WALL_IN);
			}
			if(state1 == IN_PROGRESS)
				state1=check_act_status(ACT_QUEUE_Ore_roller_arm, IN_PROGRESS, END_OK, NOT_HANDLED);
			if(state2 == IN_PROGRESS)
				state2=check_act_status(ACT_QUEUE_Ore_wall, IN_PROGRESS, END_OK, NOT_HANDLED);


			if((state1==NOT_HANDLED)||(state2==NOT_HANDLED)){
				state = ERROR;
			}else if ((state1==END_OK)&&(state2==END_OK)){
				state = DONE;
			}else{ // Add a else just to be secure
				state = DONE;
			}


		}
			break;

		case ERROR:

			RESET_MAE();
			ret = NOT_HANDLED;
			break;

		case DONE:
			RESET_MAE();
			ret = END_OK;
			break;

		default:
			if(entrance)
			{
				error_printf("default case in sub_harry_depose_minerais_zone\n");
			}
			break;
	}


	return ret;
}

/*
sub_harry_take_north_little_crater
sub_harry_get_in_north_little_crater
sub_harry_take_south_little_crater
sub_harry_get_in_south_little_crater



liste des sub pour france :
sub_harry_take_minerais()
sub_harry_get_in_north_little_crater(OUR_ELEMENT)
sub_harry_depose_minerais_zone()
*/

error_e sub_harry_test_minerai(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_CRATER_TEST,
			INIT,
			TEST,
			TEST2,
			DONE,
			ERROR
		);

	switch(state){
		case INIT:
			state = check_sub_action_result(sub_harry_take_south_little_crater(OUR_ELEMENT), state, TEST, ERROR);
			break;

		case TEST:
			state = check_sub_action_result(sub_harry_take_north_little_crater(OUR_ELEMENT), state, TEST2, ERROR);
			break;

		case TEST2:
			state = check_sub_action_result(sub_harry_depose_minerais_zone(), state, DONE, ERROR);
			break;

		case ERROR:
			RESET_MAE();
			return NOT_HANDLED;
			break;

		case DONE:
			RESET_MAE();
			return END_OK;
			break;

		default:
			if(entrance)
				debug_printf("default case in sub_harry_get_in_south_little_crater\n");
			break;
	}

	return IN_PROGRESS;
}











error_e sub_harry_take_big_crater(ELEMENTS_property_e minerais){ // OUR_ELEMENT / ADV_ELEMENT
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_TAKE_BIG_CRATER,
			INIT,
			SUB_CRATERE_MOVE_POS_YELLOW,
			SUB_CRATERE_TAKE_YELLOW_MIDDLE,
			SUB_CRATERE_TAKE_YELLOW_ROCKET,
			SUB_CRATERE_TAKE_YELLOW_CORNER,
			SUB_CRATERE_MOVE_POS_BLUE,
			SUB_CRATERE_TAKE_BLUE_MIDDLE,
			SUB_CRATERE_TAKE_BLUE_ROCKET,
			SUB_CRATERE_TAKE_BLUE_CORNER,
			GET_OUT_YELLOW,
			GET_OUT_BLUE,
			ERROR,
			ERROR_GET_OUT_YELLOW,
			ERROR_GET_OUT_BLUE,
			DONE
		);

	switch(state){
		case INIT:
			debug_printf("flag_our:%d\tflag_adv:%d", ELEMENTS_get_flag(FLAG_OUR_CORNER_CRATER_IS_TAKEN), ELEMENTS_get_flag(FLAG_ADV_CORNER_CRATER_IS_TAKEN));
			if(IHM_switchs_get(SWITCH_DISABLE_ORE)){
				state = ERROR; // L'actionneur minerais a été désactivé
			}else if((ELEMENTS_get_flag(FLAG_OUR_CORNER_CRATER_IS_TAKEN) && minerais == OUR_ELEMENT)
					|| (ELEMENTS_get_flag(FLAG_ADV_CORNER_CRATER_IS_TAKEN) && minerais == ADV_ELEMENT)){
				state = DONE;	// L'action a déjà été faite
			}/*else if(ELEMENTS_get_flag(FLAG_SUB_ANNE_TAKE_ORE)){ 			//! anne peut prendre les minerais
				state=ERROR; // L'autre robot fait la sub
			}*/else{
				if (minerais != OUR_ELEMENT && minerais != ADV_ELEMENT){

					state = ERROR;

				}else if(minerais ==OUR_ELEMENT){
					if(global.color == YELLOW){
						ELEMENTS_set_flag(FLAG_SUB_HARRY_OUR_CORNER_CRATER, TRUE);
						state = SUB_CRATERE_MOVE_POS_YELLOW;
					}else{
						ELEMENTS_set_flag(FLAG_SUB_HARRY_OUR_CORNER_CRATER, TRUE);
						state = SUB_CRATERE_MOVE_POS_BLUE;
					}

				}else{
					if(global.color == YELLOW){
						ELEMENTS_set_flag(FLAG_SUB_HARRY_ADV_CORNER_CRATER, TRUE);
						state = SUB_CRATERE_MOVE_POS_BLUE;
					}else{
						ELEMENTS_set_flag(FLAG_SUB_HARRY_ADV_CORNER_CRATER, TRUE);
						state = SUB_CRATERE_MOVE_POS_YELLOW;
					}
				}



			}
			break;

		case SUB_CRATERE_MOVE_POS_YELLOW:
			state = check_sub_action_result(sub_harry_take_big_crater_move_pos_yellow(), state, SUB_CRATERE_TAKE_YELLOW_MIDDLE, ERROR);
			break;

		case SUB_CRATERE_TAKE_YELLOW_MIDDLE:
			state = check_sub_action_result(sub_harry_take_big_crater_yellow_middle(), state, SUB_CRATERE_TAKE_YELLOW_ROCKET, ERROR);
			break;

		case SUB_CRATERE_TAKE_YELLOW_ROCKET:
			state = check_sub_action_result(sub_harry_take_big_crater_yellow_rocket(), state, GET_OUT_YELLOW, ERROR); //
			break;

		case GET_OUT_YELLOW:
			state = try_going(1310, 2560, GET_OUT_YELLOW, DONE,  ERROR_GET_OUT_YELLOW, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;





		case SUB_CRATERE_MOVE_POS_BLUE:
			state = check_sub_action_result(sub_harry_take_big_crater_move_pos_blue(), state, SUB_CRATERE_TAKE_BLUE_MIDDLE, ERROR);
			break;

		case SUB_CRATERE_TAKE_BLUE_MIDDLE:
			state = check_sub_action_result(sub_harry_take_big_crater_blue_middle(), state, SUB_CRATERE_TAKE_BLUE_ROCKET, ERROR);
			break;

		case SUB_CRATERE_TAKE_BLUE_ROCKET:
			state = check_sub_action_result(sub_harry_take_big_crater_blue_rocket(), state, GET_OUT_BLUE, ERROR);//
			break;

		case GET_OUT_BLUE: //try go
			state = try_going(1310, 440, GET_OUT_BLUE, DONE,  ERROR_GET_OUT_BLUE, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;


		case ERROR_GET_OUT_YELLOW:
			state = try_going(1780, 2200, ERROR_GET_OUT_YELLOW, GET_OUT_YELLOW,  GET_OUT_YELLOW, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

		case ERROR_GET_OUT_BLUE:
			state = try_going(1780, 800, ERROR_GET_OUT_BLUE, GET_OUT_BLUE,  GET_OUT_BLUE, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

		case ERROR:
			RESET_MAE();
			on_turning_point();
			return NOT_HANDLED;
			break;


		case DONE:
			RESET_MAE();
			on_turning_point();
			if(minerais == OUR_ELEMENT){
				ELEMENTS_set_flag(FLAG_SUB_HARRY_OUR_CORNER_CRATER, FALSE);
				ELEMENTS_set_flag(FLAG_OUR_CORNER_CRATER_IS_TAKEN, TRUE);
			}else{
				ELEMENTS_set_flag(FLAG_SUB_HARRY_ADV_CORNER_CRATER, FALSE);
				ELEMENTS_set_flag(FLAG_ADV_CORNER_CRATER_IS_TAKEN, TRUE);
			}
			ELEMENTS_set_flag(FLAG_HARRY_STOMACH_IS_FULL, TRUE);
			return END_OK;
			break;

		default:
			if(entrance)
				debug_printf("default case in sub_harry_take_big_crater\n");
			break;

	}

	return IN_PROGRESS;
}



//sous sub deplacement
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



//aller cratere jaune
error_e sub_harry_take_big_crater_move_pos_yellow(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_TAKE_BIG_CRATER_MOVE_POS_YELLOW,
			INIT,
			GO_YELLOW_CRATER_FROM_BLUE_SQUARE,
			GO_YELLOW_CRATER_FROM_MIDDLE_SQUARE,
			GO_YELLOW_CRATER_FROM_YELLOW_SQUARE,
			ASTAR_GO_YELLOW_CRATER,
			GET_IN,
			ERROR,
			DONE

		);

	const displacement_t leave_middle_square[2] = { {(GEOMETRY_point_t){900, 2200}, FAST},
										  {(GEOMETRY_point_t){1390, 2600}, FAST},
										  };

	const displacement_t leave_blue_square[3] = { {(GEOMETRY_point_t){900, 800}, FAST},
										  {(GEOMETRY_point_t){900, 2200}, FAST},
										  {(GEOMETRY_point_t){1390, 2600}, FAST}
										  };

	switch(state){
		// deplacement robot
		case INIT:
			state = GET_IN;
			break;

		case GET_IN:
			if(i_am_in_square(400, 1500, 200, 800)){
				state = GO_YELLOW_CRATER_FROM_BLUE_SQUARE;
			}
			else if(i_am_in_square(400, 1500, 2700, 2200)){
				state = GO_YELLOW_CRATER_FROM_YELLOW_SQUARE;
			}
			else if(i_am_in_square(300, 1200, 800, 2200)){
				state = GO_YELLOW_CRATER_FROM_MIDDLE_SQUARE;
			}else{
				state = ASTAR_GO_YELLOW_CRATER;
			}
			break;

		case GO_YELLOW_CRATER_FROM_BLUE_SQUARE:
			state = try_going_multipoint( leave_blue_square, 3, state, DONE, ASTAR_GO_YELLOW_CRATER, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

		case GO_YELLOW_CRATER_FROM_MIDDLE_SQUARE:
			state = try_going_multipoint( leave_middle_square, 2, state, DONE, ASTAR_GO_YELLOW_CRATER, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

		case GO_YELLOW_CRATER_FROM_YELLOW_SQUARE:
			state = try_going(1390, 2600, state, DONE, ASTAR_GO_YELLOW_CRATER, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

		case ASTAR_GO_YELLOW_CRATER:
			state = ASTAR_try_going(1390, 2600, state, DONE,  ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

		case ERROR:
			RESET_MAE();
			return NOT_HANDLED;
			break;

		case DONE:
			RESET_MAE();
			return END_OK;
			break;

		default:
			if(entrance)
				debug_printf("default case in sub_harry_take_big_crater_move_pos_yellow\n");
			break;
	}

	return IN_PROGRESS;
}



//aller cratere bleu
error_e sub_harry_take_big_crater_move_pos_blue(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_TAKE_BIG_CRATER_MOVE_POS_BLUE,
			INIT,
			GO_BLUE_CRATER_FROM_YELLOW_SQUARE,
			GO_BLUE_CRATER_FROM_MIDDLE_SQUARE,
			GO_BLUE_CRATER_FROM_BLUE_SQUARE,
			ASTAR_GO_BLUE_CRATER,
			GET_IN,
			ERROR,
			DONE

		);

	const displacement_t leave_middle_square[2] = { {(GEOMETRY_point_t){900, 800}, FAST},
										  {(GEOMETRY_point_t){1390, 400}, FAST},
										  };

	const displacement_t leave_yellow_square[3] = { {(GEOMETRY_point_t){900, 2200}, FAST},
										  {(GEOMETRY_point_t){900, 800}, FAST},
										  {(GEOMETRY_point_t){1390, 400}, FAST}
										  };

	switch(state){
		// deplacement robot
		case INIT:
			state = GET_IN;
			break;

		case GET_IN:
			if(i_am_in_square(400, 1500, 200, 800)){
				state = GO_BLUE_CRATER_FROM_BLUE_SQUARE;
			}
			else if(i_am_in_square(400, 1500, 2700, 2200)){
				state = GO_BLUE_CRATER_FROM_YELLOW_SQUARE;
			}
			else if(i_am_in_square(300, 1200, 800, 2200)){
				state = GO_BLUE_CRATER_FROM_MIDDLE_SQUARE;
			}else{
				state = ASTAR_GO_BLUE_CRATER;
			}
			break;


		case GO_BLUE_CRATER_FROM_YELLOW_SQUARE:
			state = try_going_multipoint( leave_yellow_square, 3, state, DONE, ASTAR_GO_BLUE_CRATER, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

		case GO_BLUE_CRATER_FROM_MIDDLE_SQUARE:
			state = try_going_multipoint( leave_middle_square, 2, state, DONE, ASTAR_GO_BLUE_CRATER, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

		case GO_BLUE_CRATER_FROM_BLUE_SQUARE:
			state = try_going(1390, 400, state, DONE, ASTAR_GO_BLUE_CRATER, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

		case ASTAR_GO_BLUE_CRATER:
			state = ASTAR_try_going(1390, 400, state, DONE,  ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

		case ERROR:
			RESET_MAE();
			return NOT_HANDLED;
			break;

		case DONE:
			RESET_MAE();
			return END_OK;
			break;

		default:
			if(entrance)
				debug_printf("default case in sub_harry_take_big_crater_move_pos_blue\n");
			break;
	}
	return IN_PROGRESS;
}






//sous sub ramassage
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//1
//COLLECT_YELLOW_MIDDLE
error_e sub_harry_take_big_crater_yellow_middle(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_TAKE_BIG_CRATER_BLUE_MIDDLE,
			INIT,
			COLLECT_YELLOW_MIDDLE_LINE,
			COLLECT_YELLOW_MIDDLE_BEARING,
			COLLECT_YELLOW_MIDDLE_WALL_DOWN,
			COLLECT_YELLOW_MIDDLE_ACTIVATE,
			COLLECT_YELLOW_MIDDLE_MOVE_FOWARD,
			COLLECT_YELLOW_MIDDLE_ROLLER_UP,
			COLLECT_YELLOW_MIDDLE_POSITION_LEFT,
			COLLECT_YELLOW_MIDDLE_END_ACTIVATE,
			COLLECT_YELLOW_MIDDLE_WALL_UP,
			ERROR_COLLECT_YELLOW_MIDDLE_MOVE_FOWARD,
			ERROR_COLLECT_YELLOW_MIDDLE_POSITION_LEFT,

			TEST,

			DONE,
			ERROR
		);

	switch(state){
		case INIT:
			state = try_going(1300, 2600, state, COLLECT_YELLOW_MIDDLE_LINE,  ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

			/*
			 * rotation vers le cratere
			 */
		case COLLECT_YELLOW_MIDDLE_LINE:
			state = try_go_angle(-PI4096/4, state, COLLECT_YELLOW_MIDDLE_WALL_DOWN, ERROR, FAST, CLOCKWISE, END_AT_LAST_POINT);
			break;

			/*
			 * descente du mur et du rouleau
			 */
	case COLLECT_YELLOW_MIDDLE_WALL_DOWN:{
		Uint8 state0=COLLECT_YELLOW_MIDDLE_WALL_DOWN;
		Uint8 state1=COLLECT_YELLOW_MIDDLE_WALL_DOWN;
			if(entrance){
				ACT_push_order(ACT_ORE_ROLLER_ARM,  ACT_ORE_ROLLER_ARM_OUT);
				ACT_push_order(ACT_ORE_WALL,  ACT_ORE_WALL_OUT);
			}
			state0= check_act_status(ACT_QUEUE_Ore_roller_arm, state0, DONE, ERROR);
			state1= check_act_status(ACT_QUEUE_Ore_wall, state1, DONE, ERROR);

			if((state0==ERROR)||(state1==ERROR)){
				state=ERROR;
			}
			else if((state0==DONE)&&(state1==DONE)){
				state=COLLECT_YELLOW_MIDDLE_BEARING;
			}
		}break;

			/*
			 * monter des billes porteuse
			 */
		case COLLECT_YELLOW_MIDDLE_BEARING:
			// rotation nord avant de reculer dans le cratère
			//sub_act_big_take
			state = check_sub_action_result(sub_act_big_take(), state, COLLECT_YELLOW_MIDDLE_ACTIVATE, DONE);// lance lu sub suivante pour remonter les actionneurs
			break;

		/*
		 * allume le rouleau
		 */
		case COLLECT_YELLOW_MIDDLE_ACTIVATE:
			if (entrance){
				// descendre le mur du rouleau et allumer le rouleau
				ACT_push_order_with_param(ACT_ORE_ROLLER_FOAM, ACT_ORE_ROLLER_FOAM_RUN, ACT_ROLLER_FOAM_SPEED_RUN);
			}
			state= check_act_status(ACT_QUEUE_Ore_roller_foam, state, COLLECT_YELLOW_MIDDLE_MOVE_FOWARD, DONE);
			break;

			/*
			 * avance vers le cratere
			 */
		case COLLECT_YELLOW_MIDDLE_MOVE_FOWARD:
			state = try_going(1480, 2710, state, COLLECT_YELLOW_MIDDLE_ROLLER_UP,  ERROR_COLLECT_YELLOW_MIDDLE_MOVE_FOWARD, 30, BACKWARD, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

			/*
			 * pas utilise
			 */
		case COLLECT_YELLOW_MIDDLE_ROLLER_UP:
			// accelere rouleau ou arrêt ...
			state = COLLECT_YELLOW_MIDDLE_POSITION_LEFT;
			break;

			/*
			 * sort du cratere
			 */
		case COLLECT_YELLOW_MIDDLE_POSITION_LEFT:
			state = try_going(1300, 2600, state, TEST,  ERROR_COLLECT_YELLOW_MIDDLE_POSITION_LEFT, 30, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);;
			//lien avec action suivante
			break;

			/*
			 * erreur d'entrer dans le cratere
			 */
		case ERROR_COLLECT_YELLOW_MIDDLE_MOVE_FOWARD:
			// il y a quelqu'un dans le cratere
			state = try_going(1300, 2600, state, COLLECT_YELLOW_MIDDLE_MOVE_FOWARD,  COLLECT_YELLOW_MIDDLE_MOVE_FOWARD, FAST, FORWARD, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

			/*
			 * erreur de sortie du cratere
			 */
		case ERROR_COLLECT_YELLOW_MIDDLE_POSITION_LEFT:
			// il y a quelqu'un qui bloque la sortie du cratere
			state = try_going(1470, 2700, state, COLLECT_YELLOW_MIDDLE_POSITION_LEFT,  COLLECT_YELLOW_MIDDLE_POSITION_LEFT, FAST, BACKWARD, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

		case ERROR:
			RESET_MAE();
			return NOT_HANDLED;
			break;

		case DONE:
			RESET_MAE();
			return END_OK;
			break;

		default:
			if(entrance)
				debug_printf("default case in sub_harry_take_big_crater_blue_middle\n");
			break;
	}
	return IN_PROGRESS;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// 2
//COLLECT_YELLOW_ROCKET


error_e sub_harry_take_big_crater_yellow_rocket(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_TAKE_BIG_CRATER_YELLOW_FUSE,
			INIT,
			COLLECT_YELLOW_ROCKET_LINE,
			COLLECT_YELLOW_ROCKET_WALL_DOWN,
			COLLECT_YELLOW_ROCKET_ACTIVATE,
			COLLECT_YELLOW_ROCKET_MOVE_FOWARD,
			COLLECT_YELLOW_ROCKET_ROLLER_UP,
			COLLECT_YELLOW_ROCKET_POSITION_LEFT,
			COLLECT_YELLOW_ROCKET_END_ACTIVATE,
			COLLECT_YELLOW_ROCKET_BEARING,
			COLLECT_YELLOW_ROCKET_WALL_UP,
			ERROR_COLLECT_YELLOW_ROCKET_MOVE_FOWARD,
			ERROR_COLLECT_YELLOW_ROCKET_POSITION_LEFT,
			DONE,

			TEST,
			ERROR
		);

	switch(state){
		/*
	 	 * deplacement pour le deuxieme passage
	 	 */
		case INIT:
			state = try_going(1600, 2300, state, COLLECT_YELLOW_ROCKET_LINE,  COLLECT_YELLOW_ROCKET_END_ACTIVATE, 80, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

			/*
			 * rotation vers le cratere
			 */
		case COLLECT_YELLOW_ROCKET_LINE:
			// rotation nord avant de reculer dans le cratère
			state = try_go_angle(-PI4096/4, state, COLLECT_YELLOW_ROCKET_MOVE_FOWARD, COLLECT_YELLOW_ROCKET_END_ACTIVATE, FAST, CLOCKWISE, END_AT_LAST_POINT);
			break;

			/*
			 * avance vers le cratere
			 */
		case COLLECT_YELLOW_ROCKET_MOVE_FOWARD:
			state = try_going(1730, 2495, state, COLLECT_YELLOW_ROCKET_ROLLER_UP,  ERROR_COLLECT_YELLOW_ROCKET_MOVE_FOWARD, 30, BACKWARD, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

			/*
			 * pas utilisé
			 */
		case COLLECT_YELLOW_ROCKET_ROLLER_UP:
			// accelere rouleau ou arrêt ...
			state = COLLECT_YELLOW_ROCKET_POSITION_LEFT;
			break;

			/*
			 * sort du cratere
			 */
		case COLLECT_YELLOW_ROCKET_POSITION_LEFT:
			state = try_going(1600, 2300, state, COLLECT_YELLOW_ROCKET_END_ACTIVATE,  ERROR_COLLECT_YELLOW_ROCKET_POSITION_LEFT, 30, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);;
			//lien avec action suivante
			break;

				/*
				 * arret du rouleau
				 */
		case COLLECT_YELLOW_ROCKET_END_ACTIVATE:
			if (entrance){
				// descendre le mur du rouleau et allumer le rouleau
				ACT_push_order_with_param(ACT_ORE_ROLLER_FOAM, ACT_ORE_ROLLER_FOAM_STOP, ACT_ROLLER_FOAM_SPEED_RUN);
			}
			state= check_act_status(ACT_QUEUE_Ore_roller_foam, state, COLLECT_YELLOW_ROCKET_WALL_UP, COLLECT_YELLOW_ROCKET_END_ACTIVATE);
			break;

			/*
			 * remonte le mur et le rouleau
			 */
		case COLLECT_YELLOW_ROCKET_WALL_UP:{
			Uint8 state0=COLLECT_YELLOW_ROCKET_WALL_UP;
			Uint8 state1=COLLECT_YELLOW_ROCKET_WALL_UP;
				if(entrance){
					ACT_push_order(ACT_ORE_ROLLER_ARM,  ACT_ORE_ROLLER_ARM_IDLE);
					ACT_push_order(ACT_ORE_WALL,  ACT_ORE_WALL_IDLE);
				}
				state0= check_act_status(ACT_QUEUE_Ore_roller_arm, state0, DONE, ERROR);
				state1= check_act_status(ACT_QUEUE_Ore_wall, state1, DONE, ERROR);

				if((state0==ERROR)||(state1==ERROR)){
					state=ERROR;
				}
				else if((state0==DONE)&&(state1==DONE)){
					state=COLLECT_YELLOW_ROCKET_BEARING;
				}
			}break;

			/*
			 * descente des billes
			 */
		case COLLECT_YELLOW_ROCKET_BEARING:
			state = check_sub_action_result(sub_act_big_off(), state, DONE, ERROR);
			break;

			/*
			 * erreur d'entree dans le cratere
			 */
		case ERROR_COLLECT_YELLOW_ROCKET_MOVE_FOWARD:
			// il y a quelqu'un dans le cratere
			state = try_going(1600, 2300, state, COLLECT_YELLOW_ROCKET_MOVE_FOWARD,  COLLECT_YELLOW_ROCKET_MOVE_FOWARD, FAST, FORWARD, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

			/*
			 * erreur de sortie du cratere
			 */
		case ERROR_COLLECT_YELLOW_ROCKET_POSITION_LEFT:
			// il y a quelqu'un qui bloque la sortie du cratere
			state = try_going(1700, 2465, state, COLLECT_YELLOW_ROCKET_POSITION_LEFT,  COLLECT_YELLOW_ROCKET_POSITION_LEFT, FAST, BACKWARD, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

		case ERROR:
			RESET_MAE();
			return NOT_HANDLED;
			break;

		case DONE:
			RESET_MAE();
			return END_OK;
			break;

		default:
			if(entrance)
				debug_printf("default case in sub_harry_take_big_crater_yellow_rocket\n");
			break;
	}
	return IN_PROGRESS;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//3
//COLLECT_BLUE_MIDDLE
error_e sub_harry_take_big_crater_blue_middle(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_TAKE_BIG_CRATER_BLUE_MIDDLE,
			INIT,
			COLLECT_BLUE_MIDDLE_LINE,
			COLLECT_BLUE_MIDDLE_BEARING,
			COLLECT_BLUE_MIDDLE_WALL_DOWN,
			COLLECT_BLUE_MIDDLE_ACTIVATE,
			COLLECT_BLUE_MIDDLE_MOVE_FOWARD,
			COLLECT_BLUE_MIDDLE_ROLLER_UP,
			COLLECT_BLUE_MIDDLE_POSITION_LEFT,
			COLLECT_BLUE_MIDDLE_END_ACTIVATE,
			COLLECT_BLUE_MIDDLE_WALL_UP,
			ERROR_COLLECT_BLUE_MIDDLE_MOVE_FOWARD,
			ERROR_COLLECT_BLUE_MIDDLE_POSITION_LEFT,

			DONE,
			ERROR
		);

	switch(state){
		case INIT:
			state = try_going(1300, 400, state, COLLECT_BLUE_MIDDLE_LINE,  ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

			/*
			 * rotation vers le cratere
			 */
		case COLLECT_BLUE_MIDDLE_LINE:
			state = try_go_angle(PI4096*3/4, state, COLLECT_BLUE_MIDDLE_WALL_DOWN, ERROR, FAST, CLOCKWISE, END_AT_LAST_POINT);
			break;

			/*
			 * descente du mur et du rouleau
			 */
	case COLLECT_BLUE_MIDDLE_WALL_DOWN:{
		Uint8 state0=COLLECT_BLUE_MIDDLE_WALL_DOWN;
		Uint8 state1=COLLECT_BLUE_MIDDLE_WALL_DOWN;
			if(entrance){
				ACT_push_order(ACT_ORE_ROLLER_ARM,  ACT_ORE_ROLLER_ARM_OUT);
				ACT_push_order(ACT_ORE_WALL,  ACT_ORE_WALL_OUT);
			}
			state0= check_act_status(ACT_QUEUE_Ore_roller_arm, state0, DONE, ERROR);
			state1= check_act_status(ACT_QUEUE_Ore_wall, state1, DONE, ERROR);

			if((state0==ERROR)||(state1==ERROR)){
				state=ERROR;
			}
			else if((state0==DONE)&&(state1==DONE)){
				state=COLLECT_BLUE_MIDDLE_BEARING;
			}
		}break;

			/*
			 * monter des billes porteuse
			 */
		case COLLECT_BLUE_MIDDLE_BEARING:
			// rotation nord avant de reculer dans le cratère
			//sub_act_big_take
			state = check_sub_action_result(sub_act_big_take(), state, COLLECT_BLUE_MIDDLE_ACTIVATE, DONE);// lance lu sub suivante pour remonter les actionneurs
			break;

		/*
		 * allume le rouleau
		 */
		case COLLECT_BLUE_MIDDLE_ACTIVATE:
			if (entrance){
				// descendre le mur du rouleau et allumer le rouleau
				ACT_push_order_with_param(ACT_ORE_ROLLER_FOAM, ACT_ORE_ROLLER_FOAM_RUN, ACT_ROLLER_FOAM_SPEED_RUN);
			}
			state= check_act_status(ACT_QUEUE_Ore_roller_foam, state, COLLECT_BLUE_MIDDLE_MOVE_FOWARD, DONE);
			break;

			/*
			 * avance vers le cratere
			 */
		case COLLECT_BLUE_MIDDLE_MOVE_FOWARD:
			state = try_going(1480, 290, state, COLLECT_BLUE_MIDDLE_ROLLER_UP,  ERROR_COLLECT_BLUE_MIDDLE_MOVE_FOWARD, 30, BACKWARD, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

			/*
			 * pas utilise
			 */
		case COLLECT_BLUE_MIDDLE_ROLLER_UP:
			// accelere rouleau ou arrêt ...
			state = COLLECT_BLUE_MIDDLE_POSITION_LEFT;
			break;

			/*
			 * sort du cratere
			 */
		case COLLECT_BLUE_MIDDLE_POSITION_LEFT:
			state = try_going(1300, 400, state, DONE,  ERROR_COLLECT_BLUE_MIDDLE_POSITION_LEFT, 30, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);;
			//lien avec action suivante
			break;

			/*
			 * erreur d'entrer dans le cratere
			 */
		case ERROR_COLLECT_BLUE_MIDDLE_MOVE_FOWARD:
			// il y a quelqu'un dans le cratere
			state = try_going(1300, 400, state, COLLECT_BLUE_MIDDLE_MOVE_FOWARD,  COLLECT_BLUE_MIDDLE_MOVE_FOWARD, FAST, FORWARD, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

			/*
			 * erreur de sortie du cratere
			 */
		case ERROR_COLLECT_BLUE_MIDDLE_POSITION_LEFT:
			// il y a quelqu'un qui bloque la sortie du cratere
			state = try_going(1470, 300, state, COLLECT_BLUE_MIDDLE_POSITION_LEFT,  COLLECT_BLUE_MIDDLE_POSITION_LEFT, FAST, BACKWARD, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

		case ERROR:
			RESET_MAE();
			return NOT_HANDLED;
			break;

		case DONE:
			RESET_MAE();
			return END_OK;
			break;

		default:
			if(entrance)
				debug_printf("default case in sub_harry_take_big_crater_blue_middle\n");
			break;
	}
	return IN_PROGRESS;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// 4
//COLLECT_BLUE_ROCKET


error_e sub_harry_take_big_crater_blue_rocket(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_TAKE_BIG_CRATER_BLUE_FUSE,
			INIT,
			COLLECT_BLUE_ROCKET_LINE,
			COLLECT_BLUE_ROCKET_WALL_DOWN,
			COLLECT_BLUE_ROCKET_ACTIVATE,
			COLLECT_BLUE_ROCKET_MOVE_FOWARD,
			COLLECT_BLUE_ROCKET_ROLLER_UP,
			COLLECT_BLUE_ROCKET_POSITION_LEFT,
			COLLECT_BLUE_ROCKET_END_ACTIVATE,
			COLLECT_BLUE_ROCKET_BEARING,
			COLLECT_BLUE_ROCKET_WALL_UP,
			ERROR_COLLECT_BLUE_ROCKET_MOVE_FOWARD,
			ERROR_COLLECT_BLUE_ROCKET_POSITION_LEFT,
			DONE,

			TEST,
			ERROR
		);

	switch(state){
		/*
	 	 * deplacement pour le deuxieme passage
	 	 */
		case INIT:
			state = try_going(1600, 700, state, COLLECT_BLUE_ROCKET_LINE,  COLLECT_BLUE_ROCKET_END_ACTIVATE, 80, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

			/*
			 * rotation vers le cratere
			 */
		case COLLECT_BLUE_ROCKET_LINE:
			// rotation nord avant de reculer dans le cratère
			state = try_go_angle(PI4096*3/4, state, COLLECT_BLUE_ROCKET_MOVE_FOWARD, COLLECT_BLUE_ROCKET_END_ACTIVATE, FAST, CLOCKWISE, END_AT_LAST_POINT);
			break;

			/*
			 * avance vers le cratere
			 */
		case COLLECT_BLUE_ROCKET_MOVE_FOWARD:
			state = try_going(1730, 505, state, COLLECT_BLUE_ROCKET_ROLLER_UP,  ERROR_COLLECT_BLUE_ROCKET_MOVE_FOWARD, 30, BACKWARD, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

			/*
			 * pas utilisé
			 */
		case COLLECT_BLUE_ROCKET_ROLLER_UP:
			// accelere rouleau ou arrêt ...
			state = COLLECT_BLUE_ROCKET_POSITION_LEFT;
			break;

			/*
			 * sort du cratere
			 */
		case COLLECT_BLUE_ROCKET_POSITION_LEFT:
			state = try_going(1600, 700, state, COLLECT_BLUE_ROCKET_END_ACTIVATE,  ERROR_COLLECT_BLUE_ROCKET_POSITION_LEFT, 30, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);;
			//lien avec action suivante
			break;

				/*
				 * arret du rouleau
				 */
		case COLLECT_BLUE_ROCKET_END_ACTIVATE:
			if (entrance){
				// descendre le mur du rouleau et allumer le rouleau
				ACT_push_order_with_param(ACT_ORE_ROLLER_FOAM, ACT_ORE_ROLLER_FOAM_STOP, ACT_ROLLER_FOAM_SPEED_RUN);
			}
			state= check_act_status(ACT_QUEUE_Ore_roller_foam, state, COLLECT_BLUE_ROCKET_WALL_UP, COLLECT_BLUE_ROCKET_END_ACTIVATE);
			break;

			/*
			 * remonte le mur et le rouleau
			 */
		case COLLECT_BLUE_ROCKET_WALL_UP:{
			Uint8 state0=COLLECT_BLUE_ROCKET_WALL_UP;
			Uint8 state1=COLLECT_BLUE_ROCKET_WALL_UP;
				if(entrance){
					ACT_push_order(ACT_ORE_ROLLER_ARM,  ACT_ORE_ROLLER_ARM_IDLE);
					ACT_push_order(ACT_ORE_WALL,  ACT_ORE_WALL_IDLE);
				}
				state0= check_act_status(ACT_QUEUE_Ore_roller_arm, state0, DONE, ERROR);
				state1= check_act_status(ACT_QUEUE_Ore_wall, state1, DONE, ERROR);

				if((state0==ERROR)||(state1==ERROR)){
					state=ERROR;
				}
				else if((state0==DONE)&&(state1==DONE)){
					state=COLLECT_BLUE_ROCKET_BEARING;
				}
			}break;

			/*
			 * descente des billes
			 */
		case COLLECT_BLUE_ROCKET_BEARING:
			state = check_sub_action_result(sub_act_big_off(), state, TEST, ERROR);
			break;


// lance le petit cratere pour test;
		case TEST:
			state = check_sub_action_result(sub_harry_take_north_little_crater(OUR_ELEMENT), state, DONE, ERROR);
		break;

			/*
			 * erreur d'entree dans le cratere
			 */
		case ERROR_COLLECT_BLUE_ROCKET_MOVE_FOWARD:
			// il y a quelqu'un dans le cratere
			state = try_going(1600, 700, state, COLLECT_BLUE_ROCKET_MOVE_FOWARD,  COLLECT_BLUE_ROCKET_MOVE_FOWARD, FAST, FORWARD, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

			/*
			 * erreur de sortie du cratere
			 */
		case ERROR_COLLECT_BLUE_ROCKET_POSITION_LEFT:
			// il y a quelqu'un qui bloque la sortie du cratere
			state = try_going(1700, 535, state, COLLECT_BLUE_ROCKET_POSITION_LEFT,  COLLECT_BLUE_ROCKET_POSITION_LEFT, FAST, BACKWARD, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

		case ERROR:
			RESET_MAE();
			return NOT_HANDLED;
			break;

		case DONE:
			RESET_MAE();
			return END_OK;
			break;

		default:
			if(entrance)
				debug_printf("default case in sub_harry_take_big_crater_blue_rocket\n");
			break;
	}
	return IN_PROGRESS;
}
