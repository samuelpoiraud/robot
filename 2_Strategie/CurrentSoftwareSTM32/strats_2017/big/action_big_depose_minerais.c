#include "action_big.h"
#include "../../propulsion/movement.h"
#include "../../propulsion/astar.h"
#include "../../QS/QS_stateMachineHelper.h"
#include "../../utils/actionChecker.h"
#include "../../utils/generic_functions.h"
#include "../../actuator/act_functions.h"



error_e sub_harry_depose_minerais(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_DEPOSE_MINERAIS,
			INIT,
			GET_IN,
			GO_TO_SHOOTING_POS,
			TURN_TO_SHOOTING_POS,
			RUSH_TO_CLEAT,
			MOVE_BACK_SHOOTING_POS,
			SHOOTING,
			GET_OUT,
			GET_OUT_ERROR,
			ERROR,
			DONE
		);

	switch(state){
		case INIT:
			if(ELEMENTS_get_flag(FLAG_ANNE_TAKE_CYLINDER_SOUTH_UNI)||(ELEMENTS_get_flag(FLAG_ANNE_DEPOSE_CYLINDER_OUR_SIDE)||(!ELEMENTS_get_flag(FLAG_STOMACH_IS_FULL)))){
				state=ERROR;
			}else{
				state=GET_IN;
				ELEMENTS_set_flag(FLAG_HARRY_ORE_SHOOTING,TRUE);
			}
			break;

		case GET_IN:
			state=check_sub_action_result(sub_harry_get_in_depose_minerais(),state,GO_TO_SHOOTING_POS,ERROR);
			break;

		case GO_TO_SHOOTING_POS:
			state=try_going(650,COLOR_Y(300),state,TURN_TO_SHOOTING_POS,GET_OUT_ERROR,FAST,BACKWARD,NO_DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
			break;

		case TURN_TO_SHOOTING_POS:
			state=try_go_angle(0,state,RUSH_TO_CLEAT,GET_OUT_ERROR,FAST,ANY_WAY,END_AT_LAST_POINT);
			break;

		case RUSH_TO_CLEAT:
			state=try_rush(0,COLOR_Y(300),state,MOVE_BACK_SHOOTING_POS,GET_OUT_ERROR,BACKWARD,NO_DODGE_AND_WAIT,TRUE);
			break;

		case MOVE_BACK_SHOOTING_POS:
			state=try_advance(NULL,entrance,50,state,SHOOTING,RUSH_TO_CLEAT,FAST,FORWARD,NO_DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
			break;

		case SHOOTING:
			//tu tires !!!
			state=GET_OUT;
			break;

		case GET_OUT:
			state=try_going(800,COLOR_Y(500),state,DONE,RUSH_TO_CLEAT,FAST,FORWARD,NO_DODGE_AND_WAIT,END_AT_BRAKE);
#warning il faudra mettre une condition dans rush to cleat(ou shooting) pour quil ne reparte tirer
			break;

		case GET_OUT_ERROR:
			state=try_going(800,COLOR_Y(500),state,ERROR,RUSH_TO_CLEAT,FAST,FORWARD,NO_DODGE_AND_WAIT,END_AT_BRAKE);
			break;

		case ERROR:
			RESET_MAE();
			ELEMENTS_set_flag(FLAG_HARRY_ORE_SHOOTING,FALSE);
			return NOT_HANDLED;
			break;

		case DONE:
			RESET_MAE();
			ELEMENTS_set_flag(FLAG_HARRY_ORE_SHOOTING,FALSE);
			return END_OK;
			break;
	}

	return IN_PROGRESS;
}

error_e sub_harry_get_in_depose_minerais(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_GET_IN_DEPOSE_MINERAIS,
			INIT,
			GET_IN_FROM_OUR_SQUARE,
			GET_IN_MIDDLE_SQUARE,
			GET_IN_FROM_ADV_SQUARE,
			PATHFIND,
			ERROR,
			DONE
		);

	switch(state){
		case INIT:
			//si j'ai rien à déposer je vais en erreur
			if(i_am_in_square_color(800,1400,300,900))
				state=GET_IN_FROM_OUR_SQUARE;
			else if(i_am_in_square_color(100,1100,900,2100))
				state=GET_IN_MIDDLE_SQUARE;
			else if(i_am_in_square_color(800,1400,2100,2700))
				state=GET_IN_FROM_ADV_SQUARE;
			else
				state=PATHFIND;
			break;

		case GET_IN_FROM_OUR_SQUARE:
			state=try_going(800,COLOR_Y(500),state,DONE,ERROR,FAST,ANY_WAY,NO_DODGE_AND_WAIT,END_AT_BRAKE);
			break;

		case GET_IN_MIDDLE_SQUARE:
			state=try_going(1000,COLOR_Y(1000),state,GET_IN_FROM_OUR_SQUARE,ERROR,FAST,ANY_WAY,NO_DODGE_AND_WAIT,END_AT_BRAKE);
			break;

		case GET_IN_FROM_ADV_SQUARE:
			state=try_going(1000,COLOR_Y(2000),state,GET_IN_MIDDLE_SQUARE,ERROR,FAST,ANY_WAY,NO_DODGE_AND_WAIT,END_AT_BRAKE);
			break;

		case PATHFIND:
			state=ASTAR_try_going(800,COLOR_Y(500),state,DONE,ERROR,FAST,ANY_WAY,NO_DODGE_AND_WAIT,END_AT_BRAKE);
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

error_e sub_harry_shooting_depose_minerais(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_SHOOTING_DEPOSE_MINERAIS,
			INIT,
			DOWN_GUN,
			ROTATION_TRIHOLE,
			OTHERWISE,
			STOP_ROTATION_TRIHOLE,
			GUN_UP,
			ERROR,
			DONE
		);

	switch(state){
		case INIT:
			state=DOWN_GUN;
			break;

		case DOWN_GUN:
			ACT_push_order(ACT_ORE_GUN,ACT_ORE_GUN_DOWN);
			//ACT_push_order(,);Turbine
			state=check_act_status(ACT_QUEUE_Ore_gun,state,ROTATION_TRIHOLE,ERROR);
			break;

		case ROTATION_TRIHOLE:
			//ACT_push_order_with_param();
			state=OTHERWISE;
			break;

		case OTHERWISE:
			//ACT_push_order_with_param();
			state=STOP_ROTATION_TRIHOLE;
			break;

		case STOP_ROTATION_TRIHOLE:
			//ACT_push_order_with_param();
			state=GUN_UP;
			break;

		case GUN_UP:
			ACT_push_order(ACT_ORE_GUN,ACT_ORE_GUN_UP);
			//ACT_push_order(,);Turbine
			state=check_act_status(ACT_QUEUE_Ore_gun,state,DONE,ERROR);
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



error_e sub_harry_depose_minerais_alternative(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_DEPOSE_MINERAIS_ALTERNATIVE,
			INIT,
			GET_IN,
			GO_TO_SHOOTING_POS,
			TURN_TO_SHOOTING_POS,
			SHOOTING,
			GET_OUT,
			GET_OUT2,
			GET_OUT_ERROR,
			ERROR,
			DONE
		);

	switch(state){
		case INIT:
			break;

		case GET_IN:
			state=check_sub_action_result(sub_harry_get_in_depose_minerais_alternative(),state,GO_TO_SHOOTING_POS,ERROR);
			break;

		case GO_TO_SHOOTING_POS:
			state=try_going(500,COLOR_Y(900),state,TURN_TO_SHOOTING_POS,GET_OUT_ERROR,FAST,BACKWARD,NO_DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
			break;

		case TURN_TO_SHOOTING_POS:
			state=try_go_angle(COLOR_ANGLE(PI4096/4),state,SHOOTING,GET_OUT_ERROR,FAST,ANY_WAY,END_AT_LAST_POINT);
			break;

		case SHOOTING:
			//tu tires !!!
			state=GET_OUT;
			break;

		case GET_OUT:
			state=try_going(600,COLOR_Y(1150),state,DONE,GET_OUT2,FAST,ANY_WAY,NO_DODGE_AND_WAIT,END_AT_BRAKE);
			break;

		case GET_OUT2:
			state=try_going(270,COLOR_Y(1000),state,GET_OUT,GET_OUT,FAST,ANY_WAY,NO_DODGE_AND_WAIT,END_AT_BRAKE);
			break;

		case GET_OUT_ERROR:
			state=try_going(600,COLOR_Y(1150),state,ERROR,GET_OUT2,FAST,ANY_WAY,NO_DODGE_AND_WAIT,END_AT_BRAKE);
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

error_e sub_harry_get_in_depose_minerais_alternative(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_GET_IN_DEPOSE_MINERAIS_ALTERNATIVE,
			INIT,
			GET_IN_FROM_OUR_SQUARE,
			GET_IN_MIDDLE_SQUARE,
			GET_IN_FROM_ADV_SQUARE,
			PATHFIND,
			ERROR,
			DONE
		);

	switch(state){
		case INIT:
			//si j'ai rien à déposer je vais en erreur
			if(i_am_in_square_color(800,1400,300,900))
				state=GET_IN_FROM_OUR_SQUARE;
			else if(i_am_in_square_color(100,1100,900,2100))
				state=DONE;//GET_IN_MIDDLE_SQUARE;
			else if(i_am_in_square_color(800,1400,2100,2700))
				state=GET_IN_FROM_ADV_SQUARE;
			else
				state=PATHFIND;
			break;

		case GET_IN_FROM_OUR_SQUARE:
			state=try_going(1000,COLOR_Y(1100),state,DONE,ERROR,FAST,ANY_WAY,NO_DODGE_AND_WAIT,END_AT_BRAKE);
			break;

		case GET_IN_MIDDLE_SQUARE:
			state=try_going(800,COLOR_Y(1000),state,GET_IN_FROM_OUR_SQUARE,ERROR,FAST,ANY_WAY,NO_DODGE_AND_WAIT,END_AT_BRAKE);
			break;

		case GET_IN_FROM_ADV_SQUARE:
			state=try_going(800,COLOR_Y(1850),state,GET_IN_MIDDLE_SQUARE,ERROR,FAST,ANY_WAY,NO_DODGE_AND_WAIT,END_AT_BRAKE);
			break;

		case PATHFIND:
			state=ASTAR_try_going(600,COLOR_Y(1150),state,DONE,ERROR,FAST,ANY_WAY,NO_DODGE_AND_WAIT,END_AT_BRAKE);
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

