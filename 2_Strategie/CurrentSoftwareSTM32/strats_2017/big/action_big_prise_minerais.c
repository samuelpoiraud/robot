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

// flag de  sub l'autre robot, crater south
/*
error_e sub_harry_take_big_crater(ELEMENTS_property_e minerais){ // OUR_ELEMENT / ADV_ELEMENT
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_TAKE_BIG_CRATER,
			INIT,
			SUB_MOVE_POS_YELLOW,
			SUB_TAKE_YELLOW_MIDDLE,
			SUB_TAKE_YELLOW_ROCKET,
			SUB_TAKE_YELLOW_CORNER,
			SUB_MOVE_POS_BLUE,
			SUB_TAKE_BLUE_MIDDLE,
			SUB_TAKE_BLUE_ROCKET,
			SUB_TAKE_BLUE_CORNER,
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
				state = ERROR; // L'actionneur minerais a �t� d�sactiv�
			}else if((ELEMENTS_get_flag(FLAG_OUR_CORNER_CRATER_IS_TAKEN) && minerais == OUR_ELEMENT)
					|| (ELEMENTS_get_flag(FLAG_ADV_CORNER_CRATER_IS_TAKEN) && minerais == ADV_ELEMENT)){
				state = DONE;	// L'action a d�j� �t� faite
			}*/ /*else if(ELEMENTS_get_flag(FLAG_SUB_ANNE_TAKE_ORE)){ 			//! anne peut prendre les minerais
				state=ERROR; // L'autre robot fait la sub
			}*/ /*else{
				if (minerais != OUR_ELEMENT && minerais != ADV_ELEMENT){

					state = ERROR;

				}else if(minerais ==OUR_ELEMENT){
					if(global.color == YELLOW){
						ELEMENTS_set_flag(FLAG_SUB_HARRY_OUR_CORNER_CRATER, TRUE);
						state = SUB_MOVE_POS_YELLOW;
					}else{
						ELEMENTS_set_flag(FLAG_SUB_HARRY_OUR_CORNER_CRATER, TRUE);
						state = SUB_MOVE_POS_BLUE;
					}

				}else{
					if(global.color == YELLOW){
						ELEMENTS_set_flag(FLAG_SUB_HARRY_ADV_CORNER_CRATER, TRUE);
						state = SUB_MOVE_POS_BLUE;
					}else{
						ELEMENTS_set_flag(FLAG_SUB_HARRY_ADV_CORNER_CRATER, TRUE);
						state = SUB_MOVE_POS_YELLOW;
					}
				}



			}
			break;

		case SUB_MOVE_POS_YELLOW:
			state = check_sub_action_result(sub_harry_take_big_crater_move_pos_yellow(), state, SUB_TAKE_YELLOW_MIDDLE, ERROR);
			break;

		case SUB_TAKE_YELLOW_MIDDLE:
			state = check_sub_action_result(sub_harry_take_big_crater_yellow_middle(), state, SUB_TAKE_YELLOW_ROCKET, ERROR);
			break;

		case SUB_TAKE_YELLOW_ROCKET:
			state = check_sub_action_result(sub_harry_take_big_crater_yellow_rocket(), state, SUB_TAKE_YELLOW_CORNER, ERROR);
			break;

		case SUB_TAKE_YELLOW_CORNER:
			state = check_sub_action_result(sub_harry_take_big_crater_yellow_corner(), state, GET_OUT_YELLOW, ERROR);
			break;

		case GET_OUT_YELLOW:
			state = try_going(1310, 2560, GET_OUT_YELLOW, DONE,  ERROR_GET_OUT_YELLOW, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;





		case SUB_MOVE_POS_BLUE:
			state = check_sub_action_result(sub_harry_take_big_crater_move_pos_blue(), state, SUB_TAKE_BLUE_MIDDLE, ERROR);
			break;

		case SUB_TAKE_BLUE_MIDDLE:
			state = check_sub_action_result(sub_harry_take_big_crater_blue_middle(), state, SUB_TAKE_BLUE_ROCKET, ERROR);
			break;

		case SUB_TAKE_BLUE_ROCKET:
			state = check_sub_action_result(sub_harry_take_big_crater_blue_rocket(), state, SUB_TAKE_BLUE_CORNER, ERROR);
			break;

		case SUB_TAKE_BLUE_CORNER:
			state = check_sub_action_result(sub_harry_take_big_crater_blue_corner(), state, GET_OUT_BLUE, ERROR);
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
*/

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
		else if((state1==DONE)&&(state2==DONE)&&(state3==DONE)&&(state4==ERROR)&&(state5==DONE)){
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

// sub petit crat�re
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


error_e sub_harry_take_north_little_crater(ELEMENTS_property_e minerais){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_TAKE_NORTH_LITTLE_CRATER,
			INIT,
			GET_IN,
			GO_TO_POSITION,
			TURN_TO_RUSH,
			DOWN_SYSTEM,
			GO_TO_CLEAT,
			MOVE_BACK,
			GET_OUT,
			DONE,
			ERROR
		);

	switch(state){
		case INIT:
			if(IHM_switchs_get(SWITCH_DISABLE_ORE)){
				state = ERROR; // L'actionneur minerais a �t� d�sactiv�
			}else if(ELEMENTS_get_flag(FLAG_OUR_NORTH_CRATER_IS_TAKEN)&&minerais == OUR_ELEMENT){
				state = DONE;
			}else if(ELEMENTS_get_flag(FLAG_ADV_NORTH_CRATER_IS_TAKEN)&&minerais == ADV_ELEMENT){
							state = DONE;
			}else if(ELEMENTS_get_flag(FLAG_SUB_ANNE_TAKE_CYLINDER_OUR_CENTER)){
				state=ERROR;
			}else{
				state=GET_IN;


				// On l�ve le flag de subaction
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
				state=try_going(745, 650, state, MOVE_BACK, MOVE_BACK, SLOW, BACKWARD, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			}else{
				state=try_going(745, 2350, state, MOVE_BACK, MOVE_BACK, SLOW, BACKWARD, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			}
			break;

		case MOVE_BACK:
			if((minerais == OUR_ELEMENT && global.color == BLUE) || (minerais == ADV_ELEMENT && global.color == YELLOW)){
				state=try_going(800, 650, state, GET_OUT, GO_TO_CLEAT, SLOW, FORWARD, NO_DODGE_AND_NO_WAIT, END_AT_BRAKE);
			}else{
				state=try_going(800, 2350, state, GET_OUT, GO_TO_CLEAT, SLOW, FORWARD, NO_DODGE_AND_NO_WAIT, END_AT_BRAKE);
			}
			if(ON_LEAVE()){
				ACT_push_order(ACT_ORE_ROLLER_FOAM, ACT_ORE_ROLLER_FOAM_STOP);
				ACT_push_order(ACT_ORE_ROLLER_ARM, ACT_ORE_ROLLER_ARM_IDLE);
				ACT_push_order(ACT_ORE_WALL, ACT_ORE_WALL_IDLE);
			}
			break;

		case GET_OUT:
			if((minerais == OUR_ELEMENT && global.color == BLUE) || (minerais == ADV_ELEMENT && global.color == YELLOW)){
				state=try_going(950, 650, state, DONE, DONE, FAST, FORWARD, NO_DODGE_AND_NO_WAIT, END_AT_BRAKE);
			}else{
				state=try_going(950, 2350, state, DONE, DONE, FAST, FORWARD, NO_DODGE_AND_NO_WAIT, END_AT_BRAKE);
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
			// Prise crat�re c�t� jaune
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
			}else{ // Prise crat�re cot� bleu
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
				//pas utiliser pour l'instant je pr�f�re le mettre dans la fonction principale
			}else{
				state = try_going(1000, 2350, state, DONE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_BRAKE);
				//pas utiliser pour l'instant je pr�f�re le mettre dans la fonction principale
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
				state = ERROR; // L'actionneur minerais a �t� d�sactiv�
			}else if(ELEMENTS_get_flag(FLAG_OUR_SOUTH_CRATER_IS_TAKEN)&&minerais == OUR_ELEMENT){
				state = DONE;
			}else if(ELEMENTS_get_flag(FLAG_ADV_SOUTH_CRATER_IS_TAKEN)&&minerais == ADV_ELEMENT){
							state = DONE;
			}else if((minerais == OUR_ELEMENT && (ELEMENTS_get_flag(FLAG_SUB_ANNE_TAKE_CYLINDER_SOUTH_UNI) || ELEMENTS_get_flag(FLAG_SUB_ANNE_DEPOSE_CYLINDER_OUR_DIAGONAL)))
					|| (minerais == ADV_ELEMENT && ELEMENTS_get_flag(FLAG_SUB_ANNE_DEPOSE_CYLINDER_ADV_DIAGONAL))){
				state=ERROR;
			}else{
				state=GET_IN;


				// On l�ve le flag de subaction
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

			state1=check_act_status(ACT_QUEUE_Ore_roller_arm, DOWN_SYSTEM, GO_TO_CENTER, ERROR);
			state2=check_act_status(ACT_QUEUE_Ore_wall, DOWN_SYSTEM, GO_TO_CENTER, ERROR);

			if((state1==ERROR)||(state2==ERROR)){
				state=ERROR;
			}else if ((state1==GO_TO_CENTER)&&(state2==GO_TO_CENTER)){
				state=GO_TO_CENTER;
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
			// Prise crat�re cot� jaune
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
			}else{	// Prise crat�re cot� bleu
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



error_e sub_harry_take_big_crater(ELEMENTS_property_e minerais){ // OUR_ELEMENT / ADV_ELEMENT
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_TAKE_BIG_CRATER,
			INIT,
			SUB_MOVE_POS_YELLOW,
			SUB_TAKE_YELLOW,
			SUB_MOVE_POS_BLUE,
			SUB_TAKE_BLUE,
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
				state = ERROR; // L'actionneur minerais a �t� d�sactiv�
			}else if((ELEMENTS_get_flag(FLAG_OUR_CORNER_CRATER_IS_TAKEN) && minerais == OUR_ELEMENT)
					|| (ELEMENTS_get_flag(FLAG_ADV_CORNER_CRATER_IS_TAKEN) && minerais == ADV_ELEMENT)){
				state = DONE;	// L'action a d�j� �t� faite
			}/*else if(ELEMENTS_get_flag(FLAG_SUB_ANNE_TAKE_ORE)){ 			//! anne peut prendre les minerais
				state=ERROR; // L'autre robot fait la sub
			}*/
			else{
				if (minerais != OUR_ELEMENT && minerais != ADV_ELEMENT){
					state = ERROR;

				}else if(minerais ==OUR_ELEMENT){
					if(global.color == YELLOW){
						ELEMENTS_set_flag(FLAG_SUB_HARRY_OUR_CORNER_CRATER, TRUE);
						state = SUB_MOVE_POS_YELLOW;

					}else{
						ELEMENTS_set_flag(FLAG_SUB_HARRY_OUR_CORNER_CRATER, TRUE);
						state = SUB_MOVE_POS_BLUE;

					}
				}else{
					if(global.color == YELLOW){
						ELEMENTS_set_flag(FLAG_SUB_HARRY_ADV_CORNER_CRATER, TRUE);
						state = SUB_MOVE_POS_BLUE;
					}else{
						ELEMENTS_set_flag(FLAG_SUB_HARRY_ADV_CORNER_CRATER, TRUE);
						state = SUB_MOVE_POS_YELLOW;
					}
				}
			}
			break;

		case SUB_MOVE_POS_YELLOW:
			state = check_sub_action_result(sub_harry_take_big_crater_move_pos_yellow(), state, SUB_TAKE_YELLOW, ERROR);
			break;

		case SUB_TAKE_YELLOW:
			state = check_sub_action_result(sub_harry_take_big_crater_yellow(), state, GET_OUT_YELLOW, ERROR);
			break;

		case SUB_MOVE_POS_BLUE:
			state = check_sub_action_result(sub_harry_take_big_crater_move_pos_blue(), state, SUB_TAKE_BLUE, ERROR);
			break;

		case SUB_TAKE_BLUE:
			state = check_sub_action_result(sub_harry_take_big_crater_blue(), state, GET_OUT_BLUE, ERROR);
			break;
//revoir
			// position de passage
			//1650, 650  //1440 500 in
			//1590, 745  //1330 600 out
		case GET_OUT_BLUE:
			state = try_going(1330, 600, GET_OUT_BLUE, DONE,  ERROR_GET_OUT_BLUE, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

		case GET_OUT_YELLOW:
			state = try_going(1330, 2400, GET_OUT_YELLOW, DONE,  ERROR_GET_OUT_YELLOW, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

		case ERROR_GET_OUT_YELLOW:
			state = try_going(1440, 2500, ERROR_GET_OUT_YELLOW, GET_OUT_YELLOW,  GET_OUT_YELLOW, SLOW, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

		case ERROR_GET_OUT_BLUE:
			state = try_going(1440, 500, ERROR_GET_OUT_BLUE, GET_OUT_BLUE,  GET_OUT_BLUE, SLOW, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
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
			set_sub_act_enable(SUB_HARRY_DEPOSE_MINERAIS, TRUE);   // Activation de la d�pose
			return END_OK;
			break;

		default:
			if(entrance)
				debug_printf("default case in sub_harry_take_big_crater\n");
			break;
	}
	return IN_PROGRESS;
}


// 1
// COLLECT_YELLOW
error_e sub_harry_take_big_crater_yellow(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_TAKE_BIG_CRATER_YELLOW,
			INIT,
			COLLECT_YELLOW_LINE,
			COLLECT_YELLOW_BIG_BALL,
			COLLECT_YELLOW_IN,
			COLLECT_YELLOW_WALL_DOWN,
			COLLECT_YELLOW_ACTIVATE,
			COLLECT_YELLOW_WAIT,
			COLLECT_YELLOW_OUT,
			COLLECT_YELLOW_END_ACTIVATE,
			COLLECT_YELLOW_BIG_BALL_OUT,
			COLLECT_YELLOW_WALL_UP,
			ERROR_COLLECT_YELLOW_IN,

			DONE,
			ERROR
		);

	switch(state){
			/*
	 	 	 * prend la position avant d'allez vers le cratere
	 	 	 */
		case INIT:
			state = try_going(1440, 2500, state, COLLECT_YELLOW_LINE,  ERROR, SLOW, BACKWARD, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

			/*
			 * prend l'angle pour allez vers le cratere
			 */
		case COLLECT_YELLOW_LINE:
			// rotation nord avant de reculer dans le crat�re
			state = try_go_angle(PI4096/8, state, COLLECT_YELLOW_BIG_BALL, ERROR, FAST, CLOCKWISE, END_AT_LAST_POINT);
			//state = COLLECT_BLUE_BIG_BALL;
			break;

			/*
			 * remonte les billes porteuses
			 */
		case COLLECT_YELLOW_BIG_BALL:{
			Uint8 state0=COLLECT_YELLOW_BIG_BALL;
			Uint8 state1=COLLECT_YELLOW_BIG_BALL;
			Uint8 state2=COLLECT_YELLOW_BIG_BALL;
			Uint8 state3=COLLECT_YELLOW_BIG_BALL;
			Uint8 state4=COLLECT_YELLOW_BIG_BALL;

				if(entrance){
					ACT_push_order(ACT_BIG_BALL_FRONT_LEFT,  ACT_BIG_BALL_FRONT_LEFT_UP);
					ACT_push_order(ACT_BIG_BALL_FRONT_RIGHT,  ACT_BIG_BALL_FRONT_RIGHT_UP);
					ACT_push_order(ACT_BIG_BALL_BACK_LEFT,  ACT_BIG_BALL_BACK_LEFT_UP);
					ACT_push_order(ACT_BIG_BALL_BACK_RIGHT,  ACT_BIG_BALL_BACK_RIGHT_UP);
					ACT_push_order(ACT_BEARING_BALL_WHEEL,  ACT_BEARING_BALL_WHEEL_UP);
				}
				state0= check_act_status(ACT_QUEUE_Big_bearing_front_left, state0, DONE, ERROR);
				state1= check_act_status(ACT_QUEUE_Big_bearing_front_right, state1, DONE, ERROR);
				state2= check_act_status(ACT_QUEUE_Big_bearing_back_left, state2, DONE, ERROR);
				state3= check_act_status(ACT_QUEUE_Big_bearing_back_right, state3, DONE, ERROR);
				state4= check_act_status(ACT_QUEUE_Bearing_ball_wheel, state4, DONE, ERROR);

				if((state0==ERROR)||(state1==ERROR)||(state2==ERROR)||(state3==ERROR)||(state4==ERROR)){
					state=ERROR;
				}
				else if((state0==DONE)&&(state1==DONE)&&(state2==DONE)&&(state3==DONE)&&(state4==DONE)){
					state=COLLECT_YELLOW_IN;
				}
			}break;

			/*
			 * avance au bord du cratere
			 */
		case COLLECT_YELLOW_IN:
			state = try_going(1770, 2610, state, COLLECT_YELLOW_WALL_DOWN,  ERROR_COLLECT_YELLOW_IN, 30, BACKWARD, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;// stop secu
			/*
			 * descente le mur et le rouleau
			 */
	case COLLECT_YELLOW_WALL_DOWN:{
		Uint8 state0=COLLECT_YELLOW_WALL_DOWN;
		Uint8 state1=COLLECT_YELLOW_WALL_DOWN;
		Uint8 state2=COLLECT_YELLOW_WALL_DOWN;
			if(entrance){
				ACT_push_order(ACT_ORE_ROLLER_ARM,  ACT_ORE_ROLLER_ARM_OUT);
				ACT_push_order(ACT_ORE_WALL,  ACT_ORE_WALL_OUT);
			}
			state0= check_act_status(ACT_QUEUE_Ore_roller_arm, state0, DONE, ERROR);
			if(state0 == DONE){
				state1= check_act_status(ACT_QUEUE_Ore_wall, state1, DONE, ERROR);
			}
			//state1= check_act_status(ACT_QUEUE_Ore_wall, state1, DONE, ERROR);
			state2= DONE; // check_sub_action_result(sub_act_big_take(), state2, DONE, ERROR);

			if((state0==ERROR)||(state1==ERROR)||(state2==ERROR)){
				state=COLLECT_YELLOW_ACTIVATE;
			}
			else if((state0==DONE)&&(state1==DONE)&&(state2==DONE)){
				state=COLLECT_YELLOW_ACTIVATE;
			}
		}break;

		/*
		 * allume le rouleau
		 */
		case COLLECT_YELLOW_ACTIVATE:
			if (entrance){
				ACT_push_order_with_param(ACT_ORE_ROLLER_FOAM, ACT_ORE_ROLLER_FOAM_RUN, ACT_ROLLER_FOAM_SPEED_RUN);
			}
			state= check_act_status(ACT_QUEUE_Ore_roller_foam, state, COLLECT_YELLOW_WAIT, ERROR);
			break;

			/*
			 * attandre un peut dans le cratere
			 */
		case COLLECT_YELLOW_WAIT:
			//sub wait 1 sec
			state = COLLECT_YELLOW_OUT;
			break;

			/*
			 * sort du cratere
			 */
		case COLLECT_YELLOW_OUT:
			state = try_going(1330, 2400, state, COLLECT_YELLOW_END_ACTIVATE, COLLECT_YELLOW_IN, 30, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

			/*
			 * eteindre le rouleau
			 */
		case COLLECT_YELLOW_END_ACTIVATE:
			if (entrance){
				ACT_push_order_with_param(ACT_ORE_ROLLER_FOAM, ACT_ORE_ROLLER_FOAM_STOP, ACT_ROLLER_FOAM_SPEED_RUN);
			}
			state= check_act_status(ACT_QUEUE_Ore_roller_foam, state, COLLECT_YELLOW_BIG_BALL_OUT, ERROR);
			break;

		/*
		 * descente de bille porteuse avant de quitter la sub
		 */
		case COLLECT_YELLOW_BIG_BALL_OUT:{
			Uint8 state0=COLLECT_YELLOW_BIG_BALL_OUT;
			Uint8 state1=COLLECT_YELLOW_BIG_BALL_OUT;
			Uint8 state2=COLLECT_YELLOW_BIG_BALL_OUT;
			Uint8 state3=COLLECT_YELLOW_BIG_BALL_OUT;
			Uint8 state4=COLLECT_YELLOW_BIG_BALL_OUT;

				if(entrance){
					ACT_push_order(ACT_BIG_BALL_FRONT_LEFT,  ACT_BIG_BALL_FRONT_LEFT_DOWN);
					ACT_push_order(ACT_BIG_BALL_FRONT_RIGHT,  ACT_BIG_BALL_FRONT_RIGHT_DOWN);
					ACT_push_order(ACT_BIG_BALL_BACK_LEFT,  ACT_BIG_BALL_BACK_LEFT_DOWN);
					ACT_push_order(ACT_BIG_BALL_BACK_RIGHT,  ACT_BIG_BALL_BACK_RIGHT_DOWN);
					ACT_push_order(ACT_BEARING_BALL_WHEEL,  ACT_BEARING_BALL_WHEEL_DOWN);
				}
				state0= check_act_status(ACT_QUEUE_Big_bearing_front_left, state0, DONE, ERROR);
				state1= check_act_status(ACT_QUEUE_Big_bearing_front_right, state1, DONE, ERROR);
				state2= check_act_status(ACT_QUEUE_Big_bearing_back_left, state2, DONE, ERROR);
				state3= check_act_status(ACT_QUEUE_Big_bearing_back_right, state3, DONE, ERROR);
				state4= check_act_status(ACT_QUEUE_Bearing_ball_wheel, state4, DONE, ERROR);

				if((state0==ERROR)||(state1==ERROR)||(state2==ERROR)||(state3==ERROR)||(state4==ERROR)){
					state=ERROR;
				}
				else if((state0==DONE)&&(state1==DONE)&&(state2==DONE)&&(state3==DONE)&&(state4==DONE)){
					state=COLLECT_YELLOW_WALL_UP;
				}
			}break;

			/*
			 * remonte le mur et le rouleau
			 */
		case COLLECT_YELLOW_WALL_UP:{
			Uint8 state0=COLLECT_YELLOW_WALL_UP;
			Uint8 state1=COLLECT_YELLOW_WALL_UP;
			Uint8 state2=COLLECT_YELLOW_WALL_UP;
				if(entrance){
					ACT_push_order(ACT_ORE_ROLLER_ARM,  ACT_ORE_ROLLER_ARM_IDLE);
					ACT_push_order(ACT_ORE_WALL,  ACT_ORE_WALL_IDLE);
				}
				state0= check_act_status(ACT_QUEUE_Ore_roller_arm, state0, DONE, ERROR);
				state1= check_act_status(ACT_QUEUE_Ore_wall, state1, DONE, ERROR);
				state2= check_sub_action_result(sub_act_big_off(), state2, DONE, ERROR);

				if((state0==ERROR)||(state1==ERROR)||(state2==ERROR)){
					state=DONE;
				}
				else if((state0==DONE)&&(state1==DONE)&&(state2==DONE)){
					if(global.color == BLUE){
						ELEMENTS_set_flag(FLAG_OUR_CORNER_CRATER_IS_TAKEN,TRUE); // crater vide
					}
					else{
						ELEMENTS_set_flag(FLAG_ADV_CORNER_CRATER_IS_TAKEN,TRUE); // crater vide
					}
					state=DONE;
				}
			}break;

			/*
			 * erreur allez cratere
			 */
		case ERROR_COLLECT_YELLOW_IN:
			state = try_going(1330, 2400, state, COLLECT_YELLOW_IN, ERROR, 30, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break; //time out !

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


// 2
//COLLECT_BLUE
error_e sub_harry_take_big_crater_blue(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_TAKE_BIG_CRATER_BLUE,
			INIT,
			COLLECT_BLUE_LINE,
			COLLECT_BLUE_BIG_BALL,
			COLLECT_BLUE_IN,
			COLLECT_BLUE_WALL_DOWN,
			COLLECT_BLUE_ACTIVATE,
			COLLECT_BLUE_WAIT,
			COLLECT_BLUE_OUT,
			COLLECT_BLUE_END_ACTIVATE,
			COLLECT_BLUE_BIG_BALL_OUT,
			COLLECT_BLUE_WALL_UP,
			ERROR_COLLECT_BLUE_IN,

			DONE,
			ERROR
		);

	switch(state){
			/*
			 * prend la position avant d'allez vers le cratere
			 */
		case INIT:
			state = try_going(1440, 500, state, COLLECT_BLUE_LINE,  ERROR, SLOW, BACKWARD, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

			/*
			 * prend l'angle pour allez vers le cratere
			 */
		case COLLECT_BLUE_LINE:
			state = try_go_angle(PI4096*5/8, state, COLLECT_BLUE_BIG_BALL, ERROR, FAST, CLOCKWISE, END_AT_LAST_POINT);
			break;

			/*
			 * remonte les billes porteuses
			 */
		case COLLECT_BLUE_BIG_BALL:{
			Uint8 state0=COLLECT_BLUE_BIG_BALL;
			Uint8 state1=COLLECT_BLUE_BIG_BALL;
			Uint8 state2=COLLECT_BLUE_BIG_BALL;
			Uint8 state3=COLLECT_BLUE_BIG_BALL;
			Uint8 state4=COLLECT_BLUE_BIG_BALL;

				if(entrance){
					ACT_push_order(ACT_BIG_BALL_FRONT_LEFT,  ACT_BIG_BALL_FRONT_LEFT_UP);
					ACT_push_order(ACT_BIG_BALL_FRONT_RIGHT,  ACT_BIG_BALL_FRONT_RIGHT_UP);
					ACT_push_order(ACT_BIG_BALL_BACK_LEFT,  ACT_BIG_BALL_BACK_LEFT_UP);
					ACT_push_order(ACT_BIG_BALL_BACK_RIGHT,  ACT_BIG_BALL_BACK_RIGHT_UP);
					ACT_push_order(ACT_BEARING_BALL_WHEEL,  ACT_BEARING_BALL_WHEEL_UP);
				}
				state0= check_act_status(ACT_QUEUE_Big_bearing_front_left, state0, DONE, ERROR);
				state1= check_act_status(ACT_QUEUE_Big_bearing_front_right, state1, DONE, ERROR);
				state2= check_act_status(ACT_QUEUE_Big_bearing_back_left, state2, DONE, ERROR);
				state3= check_act_status(ACT_QUEUE_Big_bearing_back_right, state3, DONE, ERROR);
				state4= check_act_status(ACT_QUEUE_Bearing_ball_wheel, state4, DONE, ERROR);

				if((state0==ERROR)||(state1==ERROR)||(state2==ERROR)||(state3==ERROR)||(state4==ERROR)){
					state=ERROR;
				}
				else if((state0==DONE)&&(state1==DONE)&&(state2==DONE)&&(state3==DONE)&&(state4==DONE)){
					state=COLLECT_BLUE_IN;
				}
			}break;

			/*
			 * avance au bord du cratere
			 */
		case COLLECT_BLUE_IN:
			state = try_going(1770, 390, state, COLLECT_BLUE_WALL_DOWN,  ERROR_COLLECT_BLUE_IN, 30, BACKWARD, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;// stop secu

			/*
			 * descente le mur et le rouleau
			 */
	case COLLECT_BLUE_WALL_DOWN:{
		Uint8 state0=COLLECT_BLUE_WALL_DOWN;
		Uint8 state1=COLLECT_BLUE_WALL_DOWN;
		Uint8 state2=COLLECT_BLUE_WALL_DOWN;
			if(entrance){
				ACT_push_order(ACT_ORE_ROLLER_ARM,  ACT_ORE_ROLLER_ARM_OUT);
				ACT_push_order(ACT_ORE_WALL,  ACT_ORE_WALL_OUT);
			}
			state0= check_act_status(ACT_QUEUE_Ore_roller_arm, state0, DONE, ERROR);
			if(state0 == DONE){
				state1= check_act_status(ACT_QUEUE_Ore_wall, state1, DONE, ERROR);
			}
			//state1= check_act_status(ACT_QUEUE_Ore_wall, state1, DONE, ERROR);
			state2= DONE; // check_sub_action_result(sub_act_big_take(), state2, DONE, ERROR);

			if((state0==ERROR)||(state1==ERROR)||(state2==ERROR)){
				state=COLLECT_BLUE_ACTIVATE;
			}
			else if((state0==DONE)&&(state1==DONE)&&(state2==DONE)){
				state=COLLECT_BLUE_ACTIVATE;
			}
		}break;

		/*
		 * allume le rouleau
		 */
		case COLLECT_BLUE_ACTIVATE:
			if (entrance){
				// descendre le mur du rouleau et allumer le rouleau
				ACT_push_order_with_param(ACT_ORE_ROLLER_FOAM, ACT_ORE_ROLLER_FOAM_RUN, ACT_ROLLER_FOAM_SPEED_RUN);
			}
			state= check_act_status(ACT_QUEUE_Ore_roller_foam, state, COLLECT_BLUE_WAIT, ERROR);
			break;

			/*
			 * allume le rouleau
			 */
		case COLLECT_BLUE_WAIT:
			//sub wait 1 sec
			state = COLLECT_BLUE_OUT;
			break;

			/*
			 * sort du cratere
			 */
		case COLLECT_BLUE_OUT: // name
			state = try_going(1330, 600, state, COLLECT_BLUE_END_ACTIVATE, COLLECT_BLUE_IN, 30, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

			/*
			 * eteindre le rouleau
			 */
		case COLLECT_BLUE_END_ACTIVATE:
			if (entrance){
				ACT_push_order_with_param(ACT_ORE_ROLLER_FOAM, ACT_ORE_ROLLER_FOAM_STOP, ACT_ROLLER_FOAM_SPEED_RUN);
			}
			state= check_act_status(ACT_QUEUE_Ore_roller_foam, state, COLLECT_BLUE_BIG_BALL_OUT, ERROR);
			break;

		/*
		 * descente de bille porteuse avant de quitter la sub
		 */
		case COLLECT_BLUE_BIG_BALL_OUT:{
			Uint8 state0=COLLECT_BLUE_BIG_BALL_OUT;
			Uint8 state1=COLLECT_BLUE_BIG_BALL_OUT;
			Uint8 state2=COLLECT_BLUE_BIG_BALL_OUT;
			Uint8 state3=COLLECT_BLUE_BIG_BALL_OUT;
			Uint8 state4=COLLECT_BLUE_BIG_BALL_OUT;

				if(entrance){
					ACT_push_order(ACT_BIG_BALL_FRONT_LEFT,  ACT_BIG_BALL_FRONT_LEFT_DOWN);
					ACT_push_order(ACT_BIG_BALL_FRONT_RIGHT,  ACT_BIG_BALL_FRONT_RIGHT_DOWN);
					ACT_push_order(ACT_BIG_BALL_BACK_LEFT,  ACT_BIG_BALL_BACK_LEFT_DOWN);
					ACT_push_order(ACT_BIG_BALL_BACK_RIGHT,  ACT_BIG_BALL_BACK_RIGHT_DOWN);
					ACT_push_order(ACT_BEARING_BALL_WHEEL,  ACT_BEARING_BALL_WHEEL_DOWN);
				}
				state0= check_act_status(ACT_QUEUE_Big_bearing_front_left, state0, DONE, ERROR);
				state1= check_act_status(ACT_QUEUE_Big_bearing_front_right, state1, DONE, ERROR);
				state2= check_act_status(ACT_QUEUE_Big_bearing_back_left, state2, DONE, ERROR);
				state3= check_act_status(ACT_QUEUE_Big_bearing_back_right, state3, DONE, ERROR);
				state4= check_act_status(ACT_QUEUE_Bearing_ball_wheel, state4, DONE, ERROR);

				if((state0==ERROR)||(state1==ERROR)||(state2==ERROR)||(state3==ERROR)||(state4==ERROR)){
					state=ERROR;
				}
				else if((state0==DONE)&&(state1==DONE)&&(state2==DONE)&&(state3==DONE)&&(state4==DONE)){
					state=COLLECT_BLUE_WALL_UP;
				}
			}break;

			/*
			 * remonte le mur et le rouleau
			 */
		case COLLECT_BLUE_WALL_UP:{
			Uint8 state0=COLLECT_BLUE_WALL_UP;
			Uint8 state1=COLLECT_BLUE_WALL_UP;
			Uint8 state2=COLLECT_BLUE_WALL_UP;
				if(entrance){
					ACT_push_order(ACT_ORE_ROLLER_ARM,  ACT_ORE_ROLLER_ARM_IDLE);
					ACT_push_order(ACT_ORE_WALL,  ACT_ORE_WALL_IDLE);
				}
				state0= check_act_status(ACT_QUEUE_Ore_roller_arm, state0, DONE, ERROR);
				state1= check_act_status(ACT_QUEUE_Ore_wall, state1, DONE, ERROR);
				state2= check_sub_action_result(sub_act_big_off(), state2, DONE, ERROR);

				if((state0==ERROR)||(state1==ERROR)||(state2==ERROR)){
					state=DONE;
				}
				else if((state0==DONE)&&(state1==DONE)&&(state2==DONE)){
					if(global.color == BLUE){
						ELEMENTS_set_flag(FLAG_OUR_CORNER_CRATER_IS_TAKEN,TRUE); // crater vide
					}
					else{
						ELEMENTS_set_flag(FLAG_ADV_CORNER_CRATER_IS_TAKEN,TRUE); // crater vide
					}
					state=DONE;
				}
			}break;

			/*
			 * erreur allez cratere
			 */
		case ERROR_COLLECT_BLUE_IN:
			state = try_going(1330, 600, state, COLLECT_BLUE_IN, ERROR, 30, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break; //time out !

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

//sous sub deplacement
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//GET_IN_PRISE_MINERAIS_JAUNE
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
										  {(GEOMETRY_point_t){1330, 2400}, FAST},
										  };

	const displacement_t leave_blue_square[3] = { {(GEOMETRY_point_t){900, 800}, FAST},
										  {(GEOMETRY_point_t){900, 2200}, FAST},
										  {(GEOMETRY_point_t){1330, 2400}, FAST}
										  };

	switch(state){
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
			else if(i_am_in_square(350, 1200, 800, 2200)){
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
			state = try_going(1330, 2400, state, DONE, ASTAR_GO_YELLOW_CRATER, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

		case ASTAR_GO_YELLOW_CRATER:
			state = ASTAR_try_going(1330, 2400, state, DONE,  ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
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



//GET_IN_PRISE_MINERAIS_BLEU
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
										  {(GEOMETRY_point_t){1330, 600}, FAST},
										  };

	const displacement_t leave_yellow_square[3] = { {(GEOMETRY_point_t){900, 2200}, FAST},
										  {(GEOMETRY_point_t){900, 800}, FAST},
										  {(GEOMETRY_point_t){1330, 600}, FAST}
										  };

	switch(state){
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
			else if(i_am_in_square(350, 1200, 800, 2200)){
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
			state = try_going(1330, 600, state, DONE, ASTAR_GO_BLUE_CRATER, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

		case ASTAR_GO_BLUE_CRATER:
			state = ASTAR_try_going(1330, 600, state, DONE,  ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
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


