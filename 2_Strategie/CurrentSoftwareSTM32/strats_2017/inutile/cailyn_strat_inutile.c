#include "strat_inutile.h"
#include "../../propulsion/movement.h"
#include "../../QS/QS_stateMachineHelper.h"
#include "../../actuator/act_functions.h"
#include "../../actuator/queue.h"
#include "../../utils/actionChecker.h"

void cailyn_strat_inutile_big(bool_e right_side, bool_e left_side){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_INUTILE,
			INIT,
			GET_IN_Y,
			GET_IN_START,
			GET_IN_X,
			GET_IN_X2,
			GET_IN_TETA,
			GET_TOWARDS_TOP,
			GET_IN_ASTAR,
			MOVE_BACK_START_ERROR,
			REPOSITION_START,
			CHOICE_CYLINDER_GRAB,
			LEFT_SLIDE_OUT,
			LEFT_SLIDE_IN,
			FAIL_LEFT_SLIDE_OUT,
			LEFT_ELEVATOR_UP,
			FAIL_LEFT_SLIDE_IN,
			LEFT_SLOPE_LOCK,
			FAIL_LEFT_ELEVATOR_UP,
			END_LEFT,
			FAIL_LEFT_SLOPE_LOCK,
			RIGHT_GRAB,
			GET_OUT_ERROR,
			ERROR,
			DONE
		);

	bool_e start_error_once = FALSE;

	switch(state){
		case INIT:
			//if(right_side_full && left_side_full){
			if(TRUE){
				state = ERROR;
			}
			else if(i_am_in_square_color(0,400,0,3000)){
				state = GET_IN_START;
			}
			else if(i_am_in_square_color(400,1000,0,3000)){
				state = GET_IN_Y;
			}
			else if(i_am_in_square_color(0,2000,0,700) || i_am_in_square_color(0,2000,2300,3000)){
				state = GET_TOWARDS_TOP;
			}
			else{
				state = GET_IN_ASTAR;
			}
			break;

		case GET_IN_Y:
			state = try_going(global.pos.x, COLOR_Y(1150), state, GET_IN_X, ERROR, FAST, ANY_WAY, DODGE_AND_WAIT, END_AT_BRAKE);
			break;

		case GET_IN_ASTAR:
			state = ASTAR_try_going(800, COLOR_Y(1150), state, GET_IN_X, ERROR, FAST, ANY_WAY, DODGE_AND_WAIT, END_AT_BRAKE);
			break;

		case GET_TOWARDS_TOP:
			state = try_going(800, global.pos.y, state, GET_IN_Y, ERROR, FAST, ANY_WAY, DODGE_AND_WAIT, END_AT_BRAKE);
			break;

		case GET_IN_X:
			state = try_going(300, COLOR_Y(1150), state, GET_IN_X2, ERROR, FAST, FORWARD, DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case GET_IN_X2:
			state = try_going(200, COLOR_Y(1150), state, GET_IN_TETA, MOVE_BACK_START_ERROR, FAST, FORWARD, DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case GET_IN_TETA:
			state = try_go_angle(PI4096,state,CHOICE_CYLINDER_GRAB,MOVE_BACK_START_ERROR,FAST, ANY_WAY, END_AT_LAST_POINT);
			break;

		case MOVE_BACK_START_ERROR:
			if(entrance && start_error_once){
				state = GET_OUT_ERROR;
			}
			else if(entrance){
				start_error_once = TRUE;
			}
			else{
				state = try_advance(NULL, entrance, 200, state, REPOSITION_START, GET_IN_X, FAST, BACKWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			}
			break;

		case REPOSITION_START:
			state = try_going(400, COLOR_Y(1150), state, GET_IN_X, GET_OUT_ERROR, FAST, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case CHOICE_CYLINDER_GRAB:
			//if(right_side_full  || left_side ||  right_side_amout > left_side_amount){
			if(left_side){
				state = LEFT_SLIDE_OUT;
			}
			//else if(left_side_full  || right_side || right_side_amout < left_side_amount){
			else if(right_side){
				state = RIGHT_GRAB;
			}
			else{
				state = RIGHT_GRAB; //si un côté fonctionne mieux, mettez le
			}
			break;

		case LEFT_SLIDE_OUT:
			if(entrance){
				ACT_push_order(ACT_CYLINDER_SLOPE_LEFT, ACT_CYLINDER_SLOPE_LEFT_UNLOCK); //on baisse la plaque de gauche de maintien de cylindres en haut du robot
				//on éteind la pompe pour remonter le cylindre côté gauche
				ACT_push_order(ACT_CYLINDER_ELEVATOR_LEFT, ACT_CYLINDER_ELEVATOR_LEFT_TOP); //on baisse le bras de remonté de cylindre côté gauche
				//on allume la pompe pour le bras de gauche qui va chercher le cylindre dans la fusée
				ACT_push_order(ACT_CYLINDER_SLIDER_LEFT, ACT_CYLINDER_SLIDER_LEFT_OUT); //on sors le bras qui va chercher le cylindre dans la fusée
			}
			check_act_status(ACT_QUEUE_Cylinder_slider_left, LEFT_SLIDE_OUT, LEFT_SLIDE_IN, FAIL_LEFT_SLIDE_OUT);//attente
			break;

		case LEFT_SLIDE_IN:
			if(entrance){
				//on allume la pompe pour remonter le cylindre côté gauche
				ACT_push_order(ACT_CYLINDER_SLIDER_LEFT, ACT_CYLINDER_SLIDER_LEFT_IN); //on rentre le bras qui va chercher le cylindre dans la fusée
			}
			check_act_status(ACT_QUEUE_Cylinder_slider_left, LEFT_SLIDE_IN, LEFT_ELEVATOR_UP, FAIL_LEFT_SLIDE_IN);//attente
			break;

		case LEFT_ELEVATOR_UP:
			if(entrance){
				//on eteint la pompe pour le bras de gauche qui va chercher le cylindre dans la fusée
				ACT_push_order(ACT_CYLINDER_ELEVATOR_LEFT, ACT_CYLINDER_ELEVATOR_LEFT_TOP); //on monte le bras de remonté de cylindre côté gauche
			}
			check_act_status(ACT_QUEUE_Cylinder_elevator_left, LEFT_ELEVATOR_UP, LEFT_SLOPE_LOCK, FAIL_LEFT_ELEVATOR_UP);//attente
			break;

		case LEFT_SLOPE_LOCK:
			if(entrance){
				ACT_push_order(ACT_CYLINDER_SLOPE_LEFT, ACT_CYLINDER_SLOPE_LEFT_LOCK); //on monte le bras de remonté de cylindre côté gauche
			}
			check_act_status(ACT_QUEUE_Cylinder_elevator_left, LEFT_SLOPE_LOCK, END_LEFT, FAIL_LEFT_SLOPE_LOCK);//attente
			break;

		case END_LEFT:
			if(entrance){
				//on éteind la pompe pour remonter le cylindre côté gauche
				ACT_push_order(ACT_CYLINDER_ELEVATOR_LEFT, ACT_CYLINDER_ELEVATOR_LEFT_BOTTOM); //on monte le bras de remonté de cylindre côté gauche
			}
			state = CHOICE_CYLINDER_GRAB;
			break;

		case GET_OUT_ERROR:
			state = try_advance(NULL, entrance, 200, state, ERROR, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			if(state == ERROR){
				if(global.pos.x > 350){
					state = ERROR;
				}
				else{
					state = GET_OUT_ERROR;
				}
			}
			break;

		case ERROR:
			break;

		case DONE:
			break;
	}
}

void cailyn_strat_inutile_small(){
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
