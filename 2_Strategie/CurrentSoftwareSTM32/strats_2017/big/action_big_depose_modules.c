#include "action_big.h"
#include "../../propulsion/movement.h"
#include "../../propulsion/astar.h"
#include "../../QS/QS_stateMachineHelper.h"
#include "../../QS/QS_CapteurCouleurCW.h"
#include "../../QS/QS_outputlog.h"
#include "../../QS/QS_IHM.h"
#include "../../utils/generic_functions.h"
#include "../../utils/actionChecker.h"
#include "../../elements.h"
#include "../../actuator/act_functions.h"
#include "../../QS/QS_types.h"
#include "../../actuator/queue.h"


//permet de régler la distance entre la base côté et le robot(largeurBase+distance):
#define DISTANCE_BASE_SIDE_ET_ROBOT	((Uint16) 120+200)


typedef enum{
	POS_1,
	POS_2,
	POS_3,
	POS_4,
	POS_5,
	POS_6,
	NO_POS,
}dropPlace_e;

Uint8 nbCylindresSurBase = 0;
bool_e depose_side;



error_e sub_harry_depose_centre_manager(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_DEPOSE_CENTRE_MANAGER,
				INIT,
				CHOOSE_ROBOT_SIDE,
				DEPOSE_POSITION,
				MODULE_CHOOSE_POSITION,
				ACTION_DISPOSE,
				COMPUTE,
				SUCCESS_DEPOSE,
				ERROR_DEPOSE,
				ERROR,
				DONE
			);

	//Emplacement de dépose
	static Uint8 dispose_place = NO_POS;

	//Côté de dépose pour le robot
	static bool_e depose_left = FALSE;
	static bool_e depose_right = FALSE;


	static ELEMENTS_side_match_e basis_side = OUR_SIDE;
	static moduleStockLocation_e robot_side = MODULE_STOCK_LEFT;
	static moduleTypeDominating_e module_type = MODULE_MONO_DOMINATING;

	//Nombre d'essais de dépose
	static Uint8 nb_try_left = 0;
	static Uint8 nb_try_right = 0;

	switch(state){
		case INIT:
			if(i_am_in_square_color(0,2000,0,1500)){
				basis_side = OUR_SIDE;
			}
			else{
				basis_side = ADV_SIDE;
			}
			state = CHOOSE_ROBOT_SIDE;
			break;

		//On choisit le côté avec lequel le robot effectue sa dépose
		case CHOOSE_ROBOT_SIDE:
			if(STOCKS_getNbModules(MODULE_STOCK_LEFT) > 0 && depose_left == FALSE){

				if(STOCKS_getDominatingModulesType(MODULE_STOCK_LEFT) == MODULE_MONO_DOMINATING){
					module_type = MODULE_MONO_DOMINATING;
				}
				else if(STOCKS_getDominatingModulesType(MODULE_STOCK_LEFT) == MODULE_POLY_DOMINATING){
					module_type = MODULE_POLY_DOMINATING;
				}
				else{//A choisir si l'on veut déposer plutôt comme des mono ou poly sur le terrain
					module_type = NO_DOMINATING;
				}
				robot_side = MODULE_STOCK_LEFT;
				state = MODULE_CHOOSE_POSITION;
			}
			else if(STOCKS_getNbModules(MODULE_STOCK_RIGHT) > 0 && depose_right == FALSE){
				if(STOCKS_getDominatingModulesType(MODULE_STOCK_RIGHT) == MODULE_MONO_DOMINATING){
					module_type = MODULE_MONO_DOMINATING;
				}
				else if(STOCKS_getDominatingModulesType(MODULE_STOCK_RIGHT) == MODULE_POLY_DOMINATING){
					module_type = MODULE_POLY_DOMINATING;
				}
				else{//A choisir si l'on veut déposer plutôt comme des mono ou poly sur le terrain
					module_type = NO_DOMINATING;
				}
				robot_side = MODULE_STOCK_RIGHT;
				state = MODULE_CHOOSE_POSITION;
			}
			else{
				state = DONE;
			}
			if(module_type == NO_DOMINATING){
				module_type = MODULE_POLY_DOMINATING;
			}
			break;

		//Choix de l'emplacement exact de la dépose
		case MODULE_CHOOSE_POSITION:
			if((module_type == MODULE_POLY_DOMINATING) && (robot_side == MODULE_STOCK_LEFT) && (basis_side == OUR_SIDE)){
				if(MOONBASES_getNbModules(MODULE_MOONBASE_OUR_CENTER)<6 && nb_try_left == 0){
					dispose_place = POS_1;
				}else if(MOONBASES_getNbModules(MODULE_MOONBASE_MIDDLE)<6 && nb_try_left == 1){
					dispose_place = POS_3;
				}else if(MOONBASES_getNbModules(MODULE_MOONBASE_ADV_CENTER)<6){
					dispose_place = POS_5;
				}else{dispose_place = NO_POS;}

			}else if(((module_type == MODULE_POLY_DOMINATING) && (robot_side == MODULE_STOCK_RIGHT) && (basis_side == OUR_SIDE))\
					|| ((module_type == MODULE_MONO_DOMINATING) && (robot_side == MODULE_STOCK_RIGHT) && (basis_side == OUR_SIDE))){
				if(MOONBASES_getNbModules(MODULE_MOONBASE_OUR_CENTER)<6  && nb_try_left == 0){
					dispose_place = POS_2;
				}else if(MOONBASES_getNbModules(MODULE_MOONBASE_MIDDLE)<6 && nb_try_left == 1){
					dispose_place = POS_4;
				}else{dispose_place = NO_POS;}

			}else if((module_type == MODULE_MONO_DOMINATING) && (robot_side == MODULE_STOCK_LEFT) && (basis_side == OUR_SIDE)){
				if(MOONBASES_getNbModules(MODULE_MOONBASE_MIDDLE)<6 && nb_try_left == 0){
					dispose_place = POS_3;
				}else if(MOONBASES_getNbModules(MODULE_MOONBASE_OUR_CENTER)<6 && nb_try_left == 1){
					dispose_place = POS_1;
				}else if(MOONBASES_getNbModules(MODULE_MOONBASE_ADV_CENTER)<6){
					dispose_place = POS_5;
				}else{dispose_place = NO_POS;}

			}else if(((module_type == MODULE_MONO_DOMINATING) && (robot_side == MODULE_STOCK_RIGHT) && (basis_side == ADV_SIDE))\
					|| ((module_type == MODULE_POLY_DOMINATING) && (robot_side == MODULE_STOCK_RIGHT) && (basis_side == ADV_SIDE))){
				if(MOONBASES_getNbModules(MODULE_MOONBASE_MIDDLE)<6 && nb_try_left == 0){
					dispose_place = POS_4;
				}else if(MOONBASES_getNbModules(MODULE_MOONBASE_OUR_CENTER)<6 && nb_try_left == 1){
					dispose_place = POS_2;
				}else{dispose_place = NO_POS;}

			}else if(((module_type == MODULE_MONO_DOMINATING) && (robot_side == MODULE_STOCK_LEFT) && (basis_side == ADV_SIDE))\
					||((module_type == MODULE_POLY_DOMINATING) && (robot_side == MODULE_STOCK_LEFT) && (basis_side == ADV_SIDE))){
				if(MOONBASES_getNbModules(MODULE_MOONBASE_ADV_CENTER)<6 && nb_try_left == 0){
					dispose_place = POS_5;
				}else if(MOONBASES_getNbModules(MODULE_MOONBASE_MIDDLE)<6 && nb_try_left == 1){
					dispose_place = POS_3;
				}else if(MOONBASES_getNbModules(MODULE_MOONBASE_OUR_CENTER)<6){
					dispose_place = POS_1;
				}else{dispose_place = NO_POS;}
			}else{
				dispose_place = NO_POS;
			}

			/*}else if((module_type == ADV_ELEMENT) && (basis_side == ADV_SIDE)){
				state = GET_IN_POS_6; //centre depose du cote adv SIM 1
*/
			state = ACTION_DISPOSE;
		break;

		case ACTION_DISPOSE:
			if(dispose_place != NO_POS){
				state = check_sub_action_result(sub_harry_depose_modules_centre(dispose_place, robot_side), state, SUCCESS_DEPOSE, ERROR_DEPOSE);
			}else{
				state = DONE;
			}
			break;

		// Vérifie que tout a été déposé, cependant si l'on reste bloqué trop longtemps on sort
		case COMPUTE:
			if( (depose_left == FALSE && nb_try_left >= 3 && depose_right == FALSE && nb_try_right >= 3) ||\
				(depose_left == TRUE && (depose_right == FALSE && nb_try_right >= 3)) ||\
				((depose_left == FALSE && nb_try_left >= 3)&& depose_right == TRUE)){
				state = DONE;
			}
			else if(STOCKS_isEmpty(MODULE_STOCK_LEFT) == FALSE || STOCKS_isEmpty(MODULE_STOCK_RIGHT) == FALSE){
				state = CHOOSE_ROBOT_SIDE;
			}
			else{
				state = DONE;
			}
			break;

		case SUCCESS_DEPOSE:
			if (dispose_place == POS_1 || dispose_place == POS_3 || dispose_place == POS_5){
				depose_left = TRUE;
			}else{
				depose_right = TRUE;
			}
			state = COMPUTE;
			break;

		case ERROR_DEPOSE:
			if(robot_side == MODULE_STOCK_LEFT){
				nb_try_left++;
			}else{
				nb_try_right++;
			}
			state = COMPUTE;
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
				debug_printf("default case in sub_harry_depose_centre_manager\n");
			break;
	}
	return IN_PROGRESS;
}


error_e sub_harry_depose_modules_centre(Uint8 drop_place, moduleStockLocation_e robot_side){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_DEPOSE_MODULES_CENTRE,
			INIT,
			GET_IN,
			GO_TO_DEPOSE_MODULE_POS,
			GO_TO_DEPOSE_MODULE,
			DEPOSE_MODULE,

			DOWN_PUSHER,
			ERROR_DOWN_PUSHER,
			UP_PUSHER_RIGHT,
			UP_PUSHER_LEFT,

			PUSH_MODULE,
			PUSH_MODULE_RETURN,
			NEXT_DEPOSE_MODULE_LEFT,
			NEXT_DEPOSE_MODULE_RIGHT,
			GET_OUT,
			FINISH_GET_OUT_POS_1,
			GET_OUT_ERROR,
			ERROR,
			DONE
		);

	switch(state){
		case INIT:
			state=GET_IN;
			break;

		case GET_IN:
			state=check_sub_action_result(sub_harry_get_in_depose_modules_centre(drop_place), state, DOWN_PUSHER, ERROR);
			break;

		case GO_TO_DEPOSE_MODULE:
			if((robot_side == MODULE_STOCK_LEFT && !STOCKS_isEmpty(MODULE_STOCK_LEFT))||(robot_side == MODULE_STOCK_RIGHT && !STOCKS_isEmpty(MODULE_STOCK_RIGHT))){
				state = DEPOSE_MODULE;
			}
			else{
				state = DONE;
			}
			break;

		case DEPOSE_MODULE:
			if(entrance){
				if(robot_side == MODULE_STOCK_LEFT){
					ACT_push_order(ACT_CYLINDER_PUSHER_LEFT, ACT_CYLINDER_PUSHER_LEFT_PREVENT_DEPOSE);
				}else{
					ACT_push_order(ACT_CYLINDER_PUSHER_RIGHT, ACT_CYLINDER_PUSHER_RIGHT_PREVENT_DEPOSE);
				}
			}
			if(robot_side == MODULE_STOCK_LEFT){
				state = check_sub_action_result(sub_act_harry_mae_dispose_modules(MODULE_STOCK_LEFT, ARG_DISPOSE_ONE_CYLINDER_FOLLOW_BY_ANOTHER), state, DOWN_PUSHER, ERROR);
			}
			else if(robot_side == MODULE_STOCK_RIGHT){
				state = check_sub_action_result(sub_act_harry_mae_dispose_modules(MODULE_STOCK_RIGHT, ARG_DISPOSE_ONE_CYLINDER_FOLLOW_BY_ANOTHER), state, DOWN_PUSHER, ERROR);
			}
			else{
				state = DONE;
			}
			if(ON_LEAVE()){
				if(state == DOWN_PUSHER){
					//J'ai mis polychrome parce que je vois pas trop comment le gérer sinon.
					if(drop_place == POS_1 || drop_place == POS_2){MOONBASES_addModule(MODULE_POLY, MODULE_MOONBASE_OUR_CENTER);}
					else if(drop_place == POS_3 || drop_place == POS_4){MOONBASES_addModule(MODULE_POLY, MODULE_MOONBASE_MIDDLE);}
					else if(drop_place == POS_5 || drop_place == POS_6){MOONBASES_addModule(MODULE_POLY, MODULE_MOONBASE_ADV_CENTER);}
				}
			}
			break;

		case DOWN_PUSHER: // on sort le pusher
			if (entrance){
				if(robot_side == MODULE_STOCK_RIGHT){
					ACT_push_order(ACT_CYLINDER_PUSHER_RIGHT,  ACT_CYLINDER_PUSHER_RIGHT_OUT);
				}else{
					ACT_push_order(ACT_CYLINDER_PUSHER_LEFT,  ACT_CYLINDER_PUSHER_LEFT_OUT);
				}
			}
			if(robot_side == MODULE_STOCK_RIGHT){
				state= check_act_status(ACT_QUEUE_Cylinder_pusher_right, state, PUSH_MODULE, ERROR_DOWN_PUSHER);
			}else{
				state= check_act_status(ACT_QUEUE_Cylinder_pusher_left, state, PUSH_MODULE, ERROR_DOWN_PUSHER);
			}
			break;

		case ERROR_DOWN_PUSHER:
			if (entrance){
				if(robot_side == MODULE_STOCK_RIGHT){
					ACT_push_order(ACT_CYLINDER_PUSHER_RIGHT,  ACT_CYLINDER_PUSHER_RIGHT_IN);
				}else{
					ACT_push_order(ACT_CYLINDER_PUSHER_LEFT,  ACT_CYLINDER_PUSHER_LEFT_IN);
				}
			}
			state = try_advance(NULL, entrance, 150, state, DOWN_PUSHER, ERROR, FAST, BACKWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;


		case PUSH_MODULE:
			if(drop_place == POS_1){
				// pos 1
				if(global.color == BLUE){
					state = try_rush(1730, COLOR_Y(880), state, UP_PUSHER_LEFT, ERROR, FORWARD, DODGE_AND_WAIT, FALSE);
				}else{
					state = try_rush(1730, COLOR_Y(880), state, UP_PUSHER_RIGHT, ERROR, FORWARD, DODGE_AND_WAIT, FALSE);
				}
			}else if(drop_place == POS_2){

				// pos 2
				if(global.color == BLUE){
					state = try_rush(1390, COLOR_Y(1220), state, UP_PUSHER_RIGHT,ERROR, FORWARD, DODGE_AND_WAIT, FALSE);
				}else{
					state = try_rush(1390, COLOR_Y(1220), state, UP_PUSHER_LEFT, ERROR, FORWARD, DODGE_AND_WAIT, FALSE);
				}
			}else if(drop_place == POS_3){

				 // pos 3
				if(global.color == BLUE){
					state = try_rush(1380, COLOR_Y(1250), state, UP_PUSHER_LEFT,ERROR, FORWARD, DODGE_AND_WAIT, FALSE);
				}else{
					state = try_rush(1380, COLOR_Y(1250), state, UP_PUSHER_RIGHT,ERROR, FORWARD, DODGE_AND_WAIT, FALSE);
				}
			}else if(drop_place == POS_4){
				// pos 4
				if(global.color == BLUE){
					state = try_rush(1320, COLOR_Y(1700), state, UP_PUSHER_RIGHT, ERROR, FORWARD, DODGE_AND_WAIT, FALSE);
				}else{
					state = try_rush(1320, COLOR_Y(1700), state, UP_PUSHER_LEFT, ERROR, FORWARD, DODGE_AND_WAIT, FALSE);
				}
			}else if(drop_place == POS_5){
				// pos 5
				if(global.color == BLUE){

					state = try_rush(1390, COLOR_Y(1780), state, UP_PUSHER_LEFT, ERROR, FORWARD, DODGE_AND_WAIT, FALSE);
				}else{
					state = try_rush(1390, COLOR_Y(1780), state, UP_PUSHER_RIGHT, ERROR, FORWARD, DODGE_AND_WAIT, FALSE);
				}
			}/*else if((modules == ADV_ELEMENT) && (basis_side == ADV_SIDE)){
				// pos 6
				if(global.color == BLUE){
					state = try_going(1730, COLOR_Y(2120), state, UP_PUSHER_RIGHT, ERROR, FAST, FORWARD, DODGE_AND_WAIT, END_AT_LAST_POINT);
				}else{
					state = try_going(1730, COLOR_Y(2120), state, UP_PUSHER_LEFT, ERROR, FAST, FORWARD, DODGE_AND_WAIT, END_AT_LAST_POINT);
				}
			}*/else
				state = ERROR;
			break;

		case UP_PUSHER_RIGHT: // on rentre le pusher
			if (entrance){
				ACT_push_order(ACT_CYLINDER_PUSHER_RIGHT,  ACT_CYLINDER_PUSHER_RIGHT_IN);
			}
			state = check_act_status(ACT_QUEUE_Cylinder_pusher_right, state, PUSH_MODULE_RETURN, ERROR);
			break;

		case UP_PUSHER_LEFT:
			if (entrance){
				ACT_push_order(ACT_CYLINDER_PUSHER_LEFT,  ACT_CYLINDER_PUSHER_LEFT_IN);
			}
			state = check_act_status(ACT_QUEUE_Cylinder_pusher_left, state, PUSH_MODULE_RETURN, ERROR);
			break;


		case PUSH_MODULE_RETURN:
			if(drop_place == POS_1){
				// pos 1
				if(global.color == BLUE){
					state = try_going(1610, COLOR_Y(760), state, NEXT_DEPOSE_MODULE_LEFT, ERROR, FAST, BACKWARD, DODGE_AND_WAIT, END_AT_LAST_POINT);
				}else{
					state = try_going(1610, COLOR_Y(760), state, NEXT_DEPOSE_MODULE_RIGHT, ERROR, FAST, BACKWARD, DODGE_AND_WAIT, END_AT_LAST_POINT);
				}
			}else if(drop_place == POS_2){

				// pos 2
				if(global.color == BLUE){
					state = try_going(1270, COLOR_Y(1100), state, NEXT_DEPOSE_MODULE_RIGHT, ERROR, FAST, BACKWARD, DODGE_AND_WAIT, END_AT_LAST_POINT);
				}else{
					state = try_going(1270, COLOR_Y(1100), state, NEXT_DEPOSE_MODULE_LEFT, ERROR, FAST, BACKWARD, DODGE_AND_WAIT, END_AT_LAST_POINT);
				}
			}else if(drop_place == POS_3){

				 // pos 3
				if(global.color == BLUE){
					state = try_going(1200, COLOR_Y(1250), state, NEXT_DEPOSE_MODULE_LEFT, ERROR, FAST, BACKWARD, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
				}else{
					state = try_going(1200, COLOR_Y(1250), state, NEXT_DEPOSE_MODULE_RIGHT, ERROR, FAST, BACKWARD, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
				}
			}else if(drop_place == POS_4){
				// pos 4
				if(global.color == BLUE){
					state = try_going(1200, COLOR_Y(1700), state, NEXT_DEPOSE_MODULE_RIGHT, ERROR, FAST, BACKWARD, DODGE_AND_WAIT, END_AT_LAST_POINT);
				}else{
					state = try_going(1200, COLOR_Y(1700), state, NEXT_DEPOSE_MODULE_LEFT, ERROR, FAST, BACKWARD, DODGE_AND_WAIT, END_AT_LAST_POINT);
				}
			}else if(drop_place == POS_5){
				// pos 5
				if(global.color == BLUE){
					state = try_going(1270, COLOR_Y(1900), state, NEXT_DEPOSE_MODULE_LEFT, ERROR, FAST, BACKWARD, DODGE_AND_WAIT, END_AT_LAST_POINT);
				}else{
					state = try_going(1270, COLOR_Y(1900), state, NEXT_DEPOSE_MODULE_RIGHT, ERROR, FAST, BACKWARD, DODGE_AND_WAIT, END_AT_LAST_POINT);
				}
			}else{
				state = ERROR;
			}
			break;

		case NEXT_DEPOSE_MODULE_LEFT:
			if(STOCKS_isEmpty(MODULE_STOCK_LEFT)){
				state = GET_OUT;
			}else if (drop_place == POS_1 && MOONBASES_getNbModules(MODULE_MOONBASE_OUR_CENTER) >= 6){
				state=GET_OUT;
			}else if (drop_place == POS_3 && MOONBASES_getNbModules(MODULE_MOONBASE_MIDDLE) >= 6){
				state=GET_OUT;
			}else if (drop_place == POS_5 && MOONBASES_getNbModules(MODULE_MOONBASE_ADV_CENTER) >= 6){
				state=GET_OUT;
			}else{
				state = GO_TO_DEPOSE_MODULE;
			}
			break;

		case NEXT_DEPOSE_MODULE_RIGHT:

			if(STOCKS_isEmpty(MODULE_STOCK_RIGHT)){
				state = GET_OUT;
			}else if (drop_place == POS_2 && MOONBASES_getNbModules(MODULE_MOONBASE_OUR_CENTER) >= 6){
				state=GET_OUT;
			}else if (drop_place == POS_4 && MOONBASES_getNbModules(MODULE_MOONBASE_MIDDLE) >= 6){
				state=GET_OUT;
			}else{
				state = GO_TO_DEPOSE_MODULE;
			}
			break;

		case GET_OUT:
			if(drop_place == POS_1){
				state = try_going(1460, COLOR_Y(610), state, FINISH_GET_OUT_POS_1, GET_OUT_ERROR, FAST, BACKWARD, NO_DODGE_AND_WAIT, END_AT_BRAKE);
			}else{
				state = try_advance(NULL, entrance, 250, state, DONE, GET_OUT_ERROR, FAST, BACKWARD, NO_DODGE_AND_WAIT, END_AT_BRAKE);
			}
			break;

		case FINISH_GET_OUT_POS_1:
			state = try_going(1130, COLOR_Y(680), state, DONE, DONE, FAST, BACKWARD, NO_DODGE_AND_WAIT, END_AT_BRAKE);
			break;

		case GET_OUT_ERROR:
			if(drop_place == POS_1){
				state = try_rush(1610, COLOR_Y(760), state, GET_OUT, GET_OUT, FORWARD, DODGE_AND_WAIT, FALSE);
			}else if(drop_place == POS_2){

				state = try_rush(1270, COLOR_Y(1100), state, GET_OUT, GET_OUT, FORWARD, DODGE_AND_WAIT, FALSE);

			}else if(drop_place == POS_3){

				state = try_rush(1200, COLOR_Y(1250), state, GET_OUT, GET_OUT, FORWARD, DODGE_AND_WAIT, FALSE);

			}else if(drop_place == POS_4){
				state = try_rush(1200, COLOR_Y(1700), state, GET_OUT, GET_OUT, FORWARD, DODGE_AND_WAIT, FALSE);

			}else if(drop_place == POS_5){
				state = try_rush(1270, COLOR_Y(1900), state, GET_OUT, GET_OUT, FORWARD, DODGE_AND_WAIT, FALSE);
			}else{
				state = ERROR;
			}
			break;

		case ERROR:
			if(entrance){
				ACT_push_order(ACT_CYLINDER_PUSHER_LEFT,ACT_CYLINDER_PUSHER_LEFT_IDLE);
				ACT_push_order(ACT_CYLINDER_PUSHER_RIGHT,ACT_CYLINDER_PUSHER_RIGHT_IDLE);
			}
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
				debug_printf("default case in sub_harry_depose_modules_centre\n");
			break;
	}
	return IN_PROGRESS;
}


error_e sub_harry_get_in_depose_modules_centre(Uint8 drop_place){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_GET_IN_DEPOSE_MODULES_CENTRE,
			INIT,
			GET_IN_POS_1,
			GET_IN_POS_2,
			GET_IN_POS_3,
			GET_IN_POS_4,
			GET_IN_POS_5,
			GET_IN_POS_6,
			ERROR,
			DONE
		);
//color Y
	switch(state){
		case INIT:
			if(drop_place == POS_1){
				state = GET_IN_POS_1;
			}else if(drop_place == POS_2){
				state = GET_IN_POS_2;
			}else if(drop_place == POS_3){
				state = GET_IN_POS_3;
			}else if(drop_place == POS_4){
				state = GET_IN_POS_4;
			}else if(drop_place == POS_5){
				state = GET_IN_POS_5;
			}else if(drop_place == POS_6){
				state = GET_IN_POS_6;
			}else{
				state = DONE;
			}
			break;

		case GET_IN_POS_1:
			state = check_sub_action_result(sub_harry_get_in_pos_1_depose_module_centre(), state, DONE, ERROR);
			break;

		case GET_IN_POS_2:
			state = check_sub_action_result(sub_harry_get_in_pos_2_depose_module_centre(), state, DONE, ERROR);
			break;

		case GET_IN_POS_3:
			state = check_sub_action_result(sub_harry_get_in_pos_3_depose_module_centre(), state, DONE, ERROR);
			break;

		case GET_IN_POS_4:
			state = check_sub_action_result(sub_harry_get_in_pos_4_depose_module_centre(), state, DONE, ERROR);
			break;

		case GET_IN_POS_5:
			state = check_sub_action_result(sub_harry_get_in_pos_5_depose_module_centre(), state, DONE, ERROR);
			break;

		case GET_IN_POS_6:
			state = check_sub_action_result(sub_harry_get_in_pos_6_depose_module_centre(), state, DONE, ERROR);
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


error_e sub_harry_get_in_pos_1_depose_module_centre(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_GET_IN_POS_1_DEPOSE_MODULES_CENTRE,
			INIT,
			GET_IN_FROM_OUR_SQUARE,
			GET_IN_FROM_MIDDLE_SQUARE,
			GET_IN_FROM_ADV_SQUARE,
			AVANCE,
			ROTATE,
			PATHFIND,
			ERROR,
			DONE
		);

	switch(state){
		case INIT:
			if(i_am_in_square_color(800, 1400, 300, 900)){
				state = GET_IN_FROM_OUR_SQUARE;
			}else if(i_am_in_square_color(200, 1100, 900, 2100)){
				state = GET_IN_FROM_MIDDLE_SQUARE;
			}else if(i_am_in_square_color(800, 1400, 2100, 2700)){
				state = GET_IN_FROM_ADV_SQUARE;
			}else{
				state = PATHFIND;
			}
			break;

		case GET_IN_FROM_OUR_SQUARE:
			state = try_going(1450, COLOR_Y(600), state, AVANCE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT,END_AT_BRAKE);
			break;

		case GET_IN_FROM_MIDDLE_SQUARE:
			state = try_going(1000, COLOR_Y(1000), state, GET_IN_FROM_OUR_SQUARE, ERROR, FAST, ANY_WAY, DODGE_AND_WAIT,END_AT_BRAKE);
			break;

		case GET_IN_FROM_ADV_SQUARE:
			state = try_going(900, COLOR_Y(2000), state, GET_IN_FROM_MIDDLE_SQUARE, ERROR, FAST, ANY_WAY, DODGE_AND_WAIT,END_AT_BRAKE);
			break;

		case AVANCE:
			state = try_going(1600,COLOR_Y(750), state, ROTATE, ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT,END_AT_LAST_POINT);
			break;

		case ROTATE:
			if(global.color == YELLOW){
				state = try_go_angle(COLOR_ANGLE(-PI4096/4), state, DONE, ERROR, FAST, CLOCKWISE, END_AT_LAST_POINT);
			}else{
				state = try_go_angle(COLOR_ANGLE(PI4096/4), state, DONE, ERROR, FAST, TRIGOWISE, END_AT_LAST_POINT);
			}
			break;

		case PATHFIND:
			state = ASTAR_try_going(1450, COLOR_Y(600), state, AVANCE, ERROR, FAST, ANY_WAY, DODGE_AND_WAIT, END_AT_BRAKE);
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


error_e sub_harry_get_in_pos_2_depose_module_centre(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_GET_IN_POS_2_DEPOSE_MODULES_CENTRE,
			INIT,
			GET_IN_FROM_OUR_SQUARE,
			GET_IN_FROM_MIDDLE_SQUARE,
			GET_IN_FROM_ADV_SQUARE,
			AVANCE,
			ROTATE,
			PATHFIND,
			ERROR,
			DONE
		);

	switch(state){
		case INIT:
			if(i_am_in_square_color(800, 1400, 300, 900)){
				state = GET_IN_FROM_OUR_SQUARE;
			}else if(i_am_in_square_color(200, 1100, 900, 2100)){
				state = GET_IN_FROM_MIDDLE_SQUARE;
			}else if(i_am_in_square_color(800, 1400, 2100, 2700)){
				state = GET_IN_FROM_ADV_SQUARE;
			}else{
				state = PATHFIND;
			}
			break;

		case GET_IN_FROM_OUR_SQUARE: // Le delta = 30 sert à ne pas être trop proche de la bordure au début
			state = try_going(1100 - 30, COLOR_Y(935 + 30), state, AVANCE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT,END_AT_BRAKE);
			break;

		case GET_IN_FROM_MIDDLE_SQUARE:
			state = try_going(1000, COLOR_Y(1000), state, GET_IN_FROM_OUR_SQUARE, ERROR, FAST, ANY_WAY, DODGE_AND_WAIT,END_AT_BRAKE);
			break;

		case GET_IN_FROM_ADV_SQUARE:
			state = try_going(900, COLOR_Y(2000), state, GET_IN_FROM_MIDDLE_SQUARE, ERROR, FAST, ANY_WAY, DODGE_AND_WAIT,END_AT_BRAKE);
			break;

		case AVANCE:
			state = try_going(1300,COLOR_Y(1135), state, ROTATE, ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT,END_AT_LAST_POINT);
			break;

		case ROTATE:
			if(global.color == YELLOW){
				state = try_go_angle(COLOR_ANGLE(-PI4096/4), state, DONE, ERROR, FAST, CLOCKWISE, END_AT_LAST_POINT);
			}else{
				state = try_go_angle(COLOR_ANGLE(PI4096/4), state, DONE, ERROR, FAST, TRIGOWISE, END_AT_LAST_POINT);
			}
			break;

		case PATHFIND:
			state = ASTAR_try_going(1100, COLOR_Y(925), state, AVANCE, ERROR, FAST, ANY_WAY, DODGE_AND_NO_WAIT, END_AT_BRAKE);
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

error_e sub_harry_get_in_pos_3_depose_module_centre(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_GET_IN_POS_3_DEPOSE_MODULES_CENTRE,
			INIT,
			GET_IN_FROM_OUR_SQUARE,
			//GET_IN_FROM_MIDDLE_SQUARE,
			GET_IN_FROM_ADV_SQUARE,
			AVANCE,
			ROTATE,
			PATHFIND,
			ERROR,
			DONE
		);

	switch(state){
		case INIT:
			if(i_am_in_square_color(800,1400, 1000,1400)){
				state = AVANCE;
			}
			else if((i_am_in_square_color(800, 1400, 300, 900))||(i_am_in_square_color(200, 1100, 900, 2100))){
				state = GET_IN_FROM_OUR_SQUARE;
			//}else if(i_am_in_square(200, 1100, 900, 2100)){
			//	state = GET_IN_FROM_MIDDLE_SQUARE;
			}else if(i_am_in_square_color(800, 1400, 2100, 2700)){
				state = GET_IN_FROM_ADV_SQUARE;
			}
			else{
				state = PATHFIND;
			}
			break;

		case GET_IN_FROM_OUR_SQUARE:
			state = try_going(800, COLOR_Y(1245), state, AVANCE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT,END_AT_BRAKE);
			break;

		//case GET_IN_FROM_MIDDLE_SQUARE:
		//	state = try_going(1000, COLOR_Y(1250), state, GET_IN_FROM_OUR_SQUARE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT,END_AT_BRAKE);
		//	break;

		case GET_IN_FROM_ADV_SQUARE:
			state = try_going(900, COLOR_Y(2000), state, GET_IN_FROM_OUR_SQUARE, ERROR, FAST, ANY_WAY, DODGE_AND_WAIT,END_AT_BRAKE);
			break;

		case AVANCE://1125
			state = try_going(1200, COLOR_Y(1245), state, ROTATE, ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case ROTATE:
			if(global.color == YELLOW){
				state = try_go_angle(0, state, DONE, ERROR, FAST, CLOCKWISE, END_AT_LAST_POINT);
			}else{
				state = try_go_angle(0, state, DONE, ERROR, FAST, TRIGOWISE, END_AT_LAST_POINT);
			}
			break;

		case PATHFIND:
			state = ASTAR_try_going(800, COLOR_Y(1245), state, AVANCE, ERROR, FAST, FORWARD, DODGE_AND_NO_WAIT, END_AT_BRAKE);
			break;

		case ERROR:
			if(entrance){
				ACT_push_order(ACT_CYLINDER_PUSHER_RIGHT, ACT_CYLINDER_PUSHER_RIGHT_IDLE);
				ACT_push_order(ACT_CYLINDER_PUSHER_LEFT, ACT_CYLINDER_PUSHER_LEFT_IDLE);
			}
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

error_e sub_harry_get_in_pos_4_depose_module_centre(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_GET_IN_POS_4_DEPOSE_MODULES_CENTRE,
			INIT,
			GET_IN_FROM_OUR_SQUARE,
			//GET_IN_FROM_MIDDLE_SQUARE,
			GET_IN_FROM_ADV_SQUARE,
			AVANCE,
			ROTATE,
			PATHFIND,
			ERROR,
			DONE
		);

	switch(state){
		case INIT:
			if((i_am_in_square_color(800, 1400, 300, 900))||(i_am_in_square(200, 1100, 900, 2100))){
				state = GET_IN_FROM_OUR_SQUARE;
			//}else if(i_am_in_square(200, 1100, 900, 2100)){
			//	state = GET_IN_FROM_MIDDLE_SQUARE;
			}else if(i_am_in_square_color(800, 1400, 2100, 2700)){
				state = GET_IN_FROM_ADV_SQUARE;
			}else{
				state = PATHFIND;
			}
			break;

		case GET_IN_FROM_OUR_SQUARE:
			state = try_going(800, COLOR_Y(1755), state, AVANCE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT,END_AT_BRAKE);
			break;

		//case GET_IN_FROM_MIDDLE_SQUARE:
		//	state = try_going(1000, COLOR_Y(1750), state, GET_IN_FROM_OUR_SQUARE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT,END_AT_BRAKE);
		//	break;

		case GET_IN_FROM_ADV_SQUARE:
			state = try_going(900, COLOR_Y(1000), state, GET_IN_FROM_OUR_SQUARE, ERROR, FAST, ANY_WAY, DODGE_AND_WAIT,END_AT_BRAKE);
			break;

		case AVANCE:
			state = try_going(1200,COLOR_Y(1755), state, ROTATE, ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT,END_AT_LAST_POINT);
			break;

		case ROTATE:
			if(global.color == YELLOW){
				state = try_go_angle(0, state, DONE, ERROR, FAST, CLOCKWISE, END_AT_LAST_POINT);
			}else{
				state = try_go_angle(0, state, DONE, ERROR, FAST, TRIGOWISE, END_AT_LAST_POINT);
			}
			break;

		case PATHFIND:
			state = ASTAR_try_going(800, COLOR_Y(1755), state, AVANCE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_BRAKE);
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

error_e sub_harry_get_in_pos_5_depose_module_centre(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_GET_IN_POS_5_DEPOSE_MODULES_CENTRE,
			INIT,
			GET_IN_FROM_OUR_SQUARE,
			GET_IN_FROM_MIDDLE_SQUARE,
			GET_IN_FROM_ADV_SQUARE,
			AVANCE,
			ROTATE,
			PATHFIND,
			ERROR,
			DONE
		);

	switch(state){
		case INIT:
			if(i_am_in_square_color(800, 1400, 300, 900)){
				state = GET_IN_FROM_OUR_SQUARE;
			}else if(i_am_in_square_color(200, 1100, 900, 2100)){
				state = GET_IN_FROM_MIDDLE_SQUARE;
			}else if(i_am_in_square_color(800, 1400, 2100, 2700)){
				state = GET_IN_FROM_ADV_SQUARE;
			}else{
				state = PATHFIND;
			}
			break;

		case GET_IN_FROM_OUR_SQUARE:
			state = try_going(1100, COLOR_Y(2075), state, DONE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT,END_AT_BRAKE);
			break;

		case GET_IN_FROM_MIDDLE_SQUARE:
			state = try_going(1000, COLOR_Y(2000), state, GET_IN_FROM_OUR_SQUARE, ERROR, FAST, ANY_WAY, DODGE_AND_WAIT,END_AT_BRAKE);
			break;

		case GET_IN_FROM_ADV_SQUARE:
			state = try_going(900, COLOR_Y(1000), state, GET_IN_FROM_MIDDLE_SQUARE, ERROR, FAST, ANY_WAY, DODGE_AND_WAIT,END_AT_BRAKE);
			break;

		case AVANCE:
			state = try_going(1250,COLOR_Y(1925), state, ROTATE, ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT,END_AT_LAST_POINT);
			break;

		case ROTATE:
			if(global.color == YELLOW){
				state = try_go_angle(COLOR_ANGLE(PI4096/4), state, DONE, ERROR, FAST, TRIGOWISE, END_AT_LAST_POINT);
			}else{
				state = try_go_angle(COLOR_ANGLE(-PI4096/4), state, DONE, ERROR, FAST, CLOCKWISE, END_AT_LAST_POINT);
			}
			break;

		case PATHFIND:
			state = ASTAR_try_going(1100, COLOR_Y(2075), state, AVANCE, ERROR, FAST, ANY_WAY, DODGE_AND_NO_WAIT, END_AT_BRAKE);
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

error_e sub_harry_get_in_pos_6_depose_module_centre(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_GET_IN_POS_6_DEPOSE_MODULES_CENTRE,
			INIT,
			GET_IN_FROM_OUR_SQUARE,
			GET_IN_FROM_MIDDLE_SQUARE,
			GET_IN_FROM_ADV_SQUARE,
			PATHFIND,
			ERROR,
			DONE
		);

	switch(state){
		case INIT:
			if(i_am_in_square_color(800, 1400, 300, 900)){
				state = GET_IN_FROM_OUR_SQUARE;
			}else if(i_am_in_square_color(200, 1100, 900, 2100)){
				state = GET_IN_FROM_MIDDLE_SQUARE;
			}else if(i_am_in_square_color(800, 1400, 2100, 2700)){
				state = GET_IN_FROM_ADV_SQUARE;
			}else{
				state = PATHFIND;
			}
			break;

		case GET_IN_FROM_OUR_SQUARE:
			state = try_going(1450, COLOR_Y(2400), state, DONE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT,END_AT_BRAKE);
			break;

		case GET_IN_FROM_MIDDLE_SQUARE:
			state = try_going(1000, COLOR_Y(2000), state, GET_IN_FROM_OUR_SQUARE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT,END_AT_BRAKE);
			break;

		case GET_IN_FROM_ADV_SQUARE:
			state = try_going(900, COLOR_Y(1000), state, GET_IN_FROM_MIDDLE_SQUARE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT,END_AT_BRAKE);
			break;

		case PATHFIND:
			state = ASTAR_try_going(1450, COLOR_Y(2400), state, DONE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_BRAKE);
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




error_e sub_harry_pos_2_depose_module_centre(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_POS_2_DEPOSE_MODULES_CENTRE,
			INIT,
			ROTATE,
			ADVANCE,
			ERROR,
			DONE
		);

	switch(state){
		case INIT:
			state = ROTATE;
			break;

		case ROTATE:
			state = try_go_angle(PI4096/8, state, ADVANCE, ERROR, FAST, FORWARD, END_AT_BRAKE);
			break;

		case ADVANCE:
			state = try_going(1270, COLOR_Y(1100), state, DONE, ERROR, FAST, FORWARD, NO_DODGE_AND_NO_WAIT,END_AT_BRAKE);
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

error_e sub_harry_pos_3_depose_module_centre(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_POS_3_DEPOSE_MODULES_CENTRE,
			INIT,
			ROTATE,
			ADVANCE,
			ERROR,
			DONE
		);

	switch(state){
		case INIT:
			state = ROTATE;
			break;

		case ROTATE:
			state = try_go_angle(0, state, ADVANCE, ERROR, FAST, FORWARD, END_AT_BRAKE);
			break;

		case ADVANCE:
			state = try_going(1200, COLOR_Y(1250), state, DONE, ERROR, FAST, FORWARD, NO_DODGE_AND_NO_WAIT,END_AT_BRAKE);
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

error_e sub_harry_pos_4_depose_module_centre(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_POS_4_DEPOSE_MODULES_CENTRE,
			INIT,
			ROTATE,
			ADVANCE,
			ERROR,
			DONE
		);

	switch(state){
		case INIT:
			state = ROTATE;
			break;

		case ROTATE:
			state = try_go_angle(0, state, ADVANCE, ERROR, FAST, FORWARD, END_AT_BRAKE);
			break;

		case ADVANCE:
			state = try_going(1200, COLOR_Y(1700), state, DONE, ERROR, FAST, FORWARD, NO_DODGE_AND_NO_WAIT,END_AT_BRAKE);
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

error_e sub_harry_pos_5_depose_module_centre(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_POS_5_DEPOSE_MODULES_CENTRE,
			INIT,
			ROTATE,
			ADVANCE,
			ERROR,
			DONE
		);

	switch(state){
		case INIT:
			state = ROTATE;
			break;

		case ROTATE:
			state = try_go_angle(-PI4096/8, state, ADVANCE, ERROR, FAST, FORWARD, END_AT_BRAKE);
			break;

		case ADVANCE:
			state = try_going(1270, COLOR_Y(1900), state, DONE, ERROR, FAST, FORWARD, NO_DODGE_AND_NO_WAIT,END_AT_BRAKE);
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

error_e sub_harry_pos_6_depose_module_centre(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_POS_6_DEPOSE_MODULES_CENTRE,
			INIT,
			ROTATE,
			ADVANCE,
			ERROR,
			DONE
		);

	switch(state){
		case INIT:
			state = ROTATE;
			break;

		case ROTATE:
			state = try_go_angle(-PI4096/8, state, ADVANCE, ERROR, FAST, FORWARD, END_AT_BRAKE);
			break;

		case ADVANCE:
			state = try_going(1610, COLOR_Y(2240), state, DONE, ERROR, FAST, FORWARD, NO_DODGE_AND_NO_WAIT,END_AT_BRAKE);
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

#warning 'sub depose sur les côtés, non testé'
/*
 * sub qui depose 3 modules
 * argument côté table et côté robot
 * logique de sub calquée sur anne
 */

error_e sub_harry_depose_modules_side(ELEMENTS_side_e robot_side, ELEMENTS_side_match_e side){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_DEPOSE_MODULES_SIDE_INTRO,
			INIT,
			ERROR,
			GET_IN,
			GET_IN_ASTAR,
			CALAGE,
			POSITION_ANGLE,
			GO_TO_PUSH,
			PREPARE_TO_PUSH,
			PUSH,
			PUSHER_UP,
			PREPARE_TO_DISPOSE,
			POSITION_DISPOSE_1,
			DISPOSE_1,
			POSITION_DISPOSE_2,
			DISPOSE_2,
			POSITION_DISPOSE_3,
			DISPOSE_3,
			GET_OUT,

			DONE
		);

	static color_e color_side;
	static ELEMENTS_side_e sens_robot, sens_robot_inv;

	switch(state){
		case INIT:
			if(side == OUR_SIDE)
				color_side = global.color;
			else
				color_side = (global.color==BLUE)?YELLOW:BLUE;


			if(color_side == BLUE){
				sens_robot = BACKWARD;
				sens_robot_inv = FORWARD;
				if(i_am_in_square(750, 1350, 200, 1400)){
					state = GET_IN;
				}
				else{
					state = GET_IN_ASTAR;
				}
			}
			else{ // color yellow
				sens_robot = FORWARD;
				sens_robot_inv = BACKWARD;
				if(i_am_in_square(750, 1350, 1600, 2800)){
					state = GET_IN;
				}
				else{
					state = GET_IN_ASTAR;
				}
			}
			break;

		case GET_IN:
			state = try_going(1000, COLOR_Y(400), state, CALAGE, GET_IN_ASTAR, FAST, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT); //1000 258
			break;

		case GET_IN_ASTAR:
			state = ASTAR_try_going(1000, COLOR_Y(400), state, CALAGE, ERROR, FAST, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT); //1000 258
			break;

		case CALAGE:
			state = try_rush(1000, COLOR_Y(0), state, POSITION_ANGLE, ERROR, sens_robot, NO_DODGE_AND_WAIT, TRUE); //1000 0
			break;

		case POSITION_ANGLE:
			// avance et tourne pour longer (400)
			state = try_going(1000, COLOR_Y(400), state, GO_TO_PUSH, ERROR, FAST, sens_robot_inv, DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case GO_TO_PUSH:
			//va au sud avant de remonter /// FORWARK/BACKWARD robot side
			if(robot_side == RIGHT){
				state = try_going(1300, COLOR_Y(400), state, PREPARE_TO_PUSH, ERROR, FAST, sens_robot_inv, DODGE_AND_WAIT, END_AT_LAST_POINT);
			}
			else{
				state = try_going(1300, COLOR_Y(400), state, PREPARE_TO_PUSH, ERROR, FAST, sens_robot, DODGE_AND_WAIT, END_AT_LAST_POINT);
			}
			break;

			/*
			 * pousser les modules avant la dépose
			 */
		case PREPARE_TO_PUSH:
			//descendre l'actionneur
			if(robot_side == RIGHT){
				if(entrance){
					ACT_push_order(ACT_CYLINDER_PUSHER_RIGHT,  ACT_CYLINDER_PUSHER_RIGHT_OUT);
				}

				state= check_act_status(ACT_QUEUE_Cylinder_pusher_right, state, PUSH, PUSHER_UP);
			}
			else{
				if(entrance){
					ACT_push_order(ACT_CYLINDER_PUSHER_LEFT,  ACT_CYLINDER_PUSHER_LEFT_OUT);
				}

				state= check_act_status(ACT_QUEUE_Cylinder_pusher_left, state, PUSH, PUSHER_UP);
			}
			break;

		case PUSH:
			if(robot_side == RIGHT){
				state = try_going(600, COLOR_Y(400), state, PUSHER_UP, PUSHER_UP, FAST, sens_robot, DODGE_AND_WAIT, END_AT_LAST_POINT);
			}
			else{
				state = try_going(600, COLOR_Y(400), state, PUSHER_UP, PUSHER_UP, FAST, sens_robot_inv, DODGE_AND_WAIT, END_AT_LAST_POINT);
			}
			break;

			/*
			 * remonte le pusher avant de se rapprocher du côté de terrain
			 */
		case PUSHER_UP:{
			Uint8 state1=PUSHER_UP;
			Uint8 state2=PUSHER_UP;
			if(entrance){
				ACT_push_order(ACT_CYLINDER_PUSHER_RIGHT,  ACT_CYLINDER_PUSHER_RIGHT_IN);
				ACT_push_order(ACT_CYLINDER_PUSHER_LEFT,  ACT_CYLINDER_PUSHER_LEFT_IN);
			}

			state1 = check_act_status(ACT_QUEUE_Cylinder_pusher_right, state1, DONE, ERROR);
			state2 = check_act_status(ACT_QUEUE_Cylinder_pusher_left, state2, DONE, ERROR);
			 // abandon après avoir pousser
			if((state1==ERROR)||(state2==ERROR)){
				state=ERROR; //bug ?
			}
			else if((state1==DONE)&&(state2==DONE)){
				state=PREPARE_TO_DISPOSE;
			}
		}break;

		case PREPARE_TO_DISPOSE:
			if(robot_side == RIGHT){
				state = try_going(1200, COLOR_Y(350), state, POSITION_DISPOSE_1, ERROR, FAST, sens_robot, DODGE_AND_WAIT, END_AT_LAST_POINT);
			}
			else{
				state = try_going(1200, COLOR_Y(350), state, POSITION_DISPOSE_1, ERROR, FAST, sens_robot_inv, DODGE_AND_WAIT, END_AT_LAST_POINT);
			}
			break;

			/*
			 * se met en position pour poser le premier module
			 */
		case POSITION_DISPOSE_1:
			if(robot_side == RIGHT){
				try_going(1000, COLOR_Y(350), state, DISPOSE_1, ERROR, FAST, sens_robot, DODGE_AND_WAIT, END_AT_LAST_POINT);
			}
			else{
				try_going(1000, COLOR_Y(350), state, DISPOSE_1, ERROR, FAST, sens_robot_inv, DODGE_AND_WAIT, END_AT_LAST_POINT);
			}
			break;

		case DISPOSE_1:
			if(robot_side == RIGHT){
				state = check_sub_action_result(sub_act_harry_mae_dispose_modules(MODULE_STOCK_RIGHT, ARG_DISPOSE_ONE_CYLINDER_FOLLOW_BY_ANOTHER), state, POSITION_DISPOSE_2, ERROR);
			}
			else{
				state = check_sub_action_result(sub_act_harry_mae_dispose_modules(MODULE_STOCK_LEFT, ARG_DISPOSE_ONE_CYLINDER_FOLLOW_BY_ANOTHER), state, POSITION_DISPOSE_2, ERROR);
			}
			break;

			/*
			 * se met en position pour le seconde module
			 */
		case POSITION_DISPOSE_2:
			if(robot_side == RIGHT){
				state = try_going(925, COLOR_Y(350), state, DISPOSE_2, ERROR, FAST, sens_robot, DODGE_AND_WAIT, END_AT_LAST_POINT);
			}
			else{
				state = try_going(925, COLOR_Y(350), state, DISPOSE_2, ERROR, FAST, sens_robot_inv, DODGE_AND_WAIT, END_AT_LAST_POINT);
			}
			break;

		case DISPOSE_2:
			if(robot_side == RIGHT){
				state = check_sub_action_result(sub_act_harry_mae_dispose_modules(MODULE_STOCK_RIGHT, ARG_DISPOSE_ONE_CYLINDER_FOLLOW_BY_ANOTHER), state, POSITION_DISPOSE_3, ERROR);
			}
			else{
				state = check_sub_action_result(sub_act_harry_mae_dispose_modules(MODULE_STOCK_LEFT, ARG_DISPOSE_ONE_CYLINDER_FOLLOW_BY_ANOTHER), state, POSITION_DISPOSE_3, ERROR);
			}
			break;

			/*
			 * se met en position pour le troisieme module
			 */
		case POSITION_DISPOSE_3:
			if(robot_side == RIGHT){
				state = try_going(850, COLOR_Y(350), state, DISPOSE_3, ERROR, FAST, sens_robot, DODGE_AND_WAIT, END_AT_LAST_POINT);
			}
			else{
				state = try_going(850, COLOR_Y(350), state, DISPOSE_3, ERROR, FAST, sens_robot_inv, DODGE_AND_WAIT, END_AT_LAST_POINT);
			}
			break;

		case DISPOSE_3:
			if(robot_side == RIGHT){
				state = check_sub_action_result(sub_act_harry_mae_dispose_modules(MODULE_STOCK_RIGHT, ARG_DISPOSE_ONE_CYLINDER_AND_FINISH), state, GET_OUT, ERROR);
			}
			else{
				state = check_sub_action_result(sub_act_harry_mae_dispose_modules(MODULE_STOCK_LEFT, ARG_DISPOSE_ONE_CYLINDER_AND_FINISH), state, GET_OUT, ERROR);
			}
			break;

		case GET_OUT:
			//pos de depard ou plus loin
			state = try_going(1000, COLOR_Y(500), state, DONE, ERROR, FAST, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;


//côté robot après calage /ok
			// test des positions i_am_in square /non


		case ERROR:
			RESET_MAE();
			on_turning_point();
			return NOT_HANDLED;


		case DONE:
			RESET_MAE();
			on_turning_point();
			return END_OK;
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

	moduleMoonbaseLocation_e first_zone = MODULE_MOONBASE_MIDDLE;
	moduleMoonbaseLocation_e second_zone = MODULE_MOONBASE_OUR_CENTER;
	moduleMoonbaseLocation_e third_zone = MODULE_MOONBASE_OUR_SIDE;
	moduleMoonbaseLocation_e fourth_zone = MODULE_MOONBASE_ADV_CENTER;
	moduleMoonbaseLocation_e fifth_zone = MODULE_MOONBASE_ADV_SIDE;
	moduleMoonbaseLocation_e prefered_zone = first_zone;

	ELEMENTS_side_match_e depose_side;

	switch(state){
		case INIT:
			if(IHM_switchs_get(SWITCH_DISABLE_MODULE_RIGHT) && IHM_switchs_get(SWITCH_DISABLE_MODULE_LEFT)){
				state = ERROR; // Actionneur désactivé, on a rien pu prendre
			}else if(STOCKS_isEmpty(MODULE_STOCK_LEFT) && STOCKS_isEmpty(MODULE_STOCK_RIGHT)){
				state = ERROR; // Il n'y a rien à déposer
			}else{
#ifdef SCAN_ELEMENTS
			//scan les bases de constructions pour compter et évaluer le remplissage
#else
			state=COMPUTE;
#endif
			}
			break;

		case COMPUTE:
			if((MOONBASES_getNbModules(first_zone) < module_zone[first_zone].nb_cylinder_max)&&((module_zone[first_zone].enable_zone!=TRUE)||(i_am_in_square_color(module_zone[first_zone].xmin, module_zone[first_zone].xmax, module_zone[first_zone].ymin, module_zone[first_zone].ymax)))){
				prefered_zone=first_zone;
				state=MANAGE;
			}else if((MOONBASES_getNbModules(second_zone) < module_zone[second_zone].nb_cylinder_max)&&((module_zone[second_zone].enable_zone!=TRUE)||(i_am_in_square_color(module_zone[second_zone].xmin, module_zone[second_zone].xmax, module_zone[second_zone].ymin, module_zone[second_zone].ymax)))){
				prefered_zone=second_zone;
				state=MANAGE;
			}else if((MOONBASES_getNbModules(third_zone) < module_zone[third_zone].nb_cylinder_max)&&((module_zone[third_zone].enable_zone!=TRUE)||(i_am_in_square_color(module_zone[third_zone].xmin, module_zone[third_zone].xmax, module_zone[third_zone].ymin, module_zone[third_zone].ymax)))){
				prefered_zone=third_zone;
				state=MANAGE;
			}else if((MOONBASES_getNbModules(fourth_zone) < module_zone[fourth_zone].nb_cylinder_max)&&((module_zone[fourth_zone].enable_zone!=TRUE)||(i_am_in_square_color(module_zone[fourth_zone].xmin, module_zone[fourth_zone].xmax, module_zone[fourth_zone].ymin, module_zone[fourth_zone].ymax)))){
				prefered_zone=fourth_zone;
				state=MANAGE;
			}else if((MOONBASES_getNbModules(fifth_zone) < module_zone[fifth_zone].nb_cylinder_max)&&((module_zone[fifth_zone].enable_zone!=TRUE)||(i_am_in_square_color(module_zone[fifth_zone].xmin, module_zone[fifth_zone].xmax, module_zone[fifth_zone].ymin, module_zone[fifth_zone].ymax)))){
				prefered_zone=fifth_zone;
				state=MANAGE;
			}else{
				state=ERROR;
			}
			break;

		case MANAGE:
			if(prefered_zone==MODULE_MOONBASE_MIDDLE){
				state=GO_TO_MIDDLE;
			}else if(prefered_zone==MODULE_MOONBASE_OUR_CENTER){
				state=GO_TO_OUR_CENTER;
			}else if(prefered_zone==MODULE_MOONBASE_OUR_SIDE){
				state=GO_TO_OUR_SIDE;
			}else if(prefered_zone==MODULE_MOONBASE_ADV_CENTER){
				state=GO_TO_ADV_CENTER;
			}else if(prefered_zone==MODULE_MOONBASE_ADV_SIDE){
				state=GO_TO_ADV_SIDE;
			}else{
				state = ERROR;
			}
			break;

		case GO_TO_MIDDLE:
#ifdef ADV_SIDE_ON_CENTRAL_MOONBASE
			depose_side = ADV_SIDE;
#else
			depose_side = OUR_SIDE;
#endif
			//state=check_sub_action_result(sub_harry_depose_modules_centre(NEUTRAL_ELEMENT, , depose_side),state,DONE,ERROR);
			state = DONE;
			break;

		case GO_TO_OUR_CENTER:
#ifdef ADV_SIDE_ON_OUR_MOONBASE
			depose_side = ADV_SIDE;
#else
			depose_side = OUR_SIDE;
#endif
			//state=check_sub_action_result(sub_harry_depose_modules_centre(OUR_ELEMENT, , depose_side),state,DONE,ERROR);
			state = DONE;
			break;

		case GO_TO_OUR_SIDE:
			//state=check_sub_action_result(sub_harry_depose_modules_side_intro(OUR_ELEMENT,RIGHT, OUR_SIDE,SUD),state,DONE,ERROR);
			break;

		case GO_TO_ADV_CENTER:
#ifdef ADV_SIDE_ON_ADV_MOONBASE
			depose_side = ADV_SIDE;
#else
			depose_side = OUR_SIDE;
#endif
			//state=check_sub_action_result(sub_harry_depose_modules_centre(ADV_ELEMENT, , depose_side),state,DONE,ERROR);
			state = DONE;
			break;

		case GO_TO_ADV_SIDE:
			//state=check_sub_action_result(sub_harry_depose_modules_side_intro(ADV_ELEMENT,RIGHT, OUR_SIDE,SUD),state,DONE,ERROR);
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
				debug_printf("default case in sub_harry_depose_manager\n");
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
				if((global.color==BLUE)&&(CW_is_color_detected(CW_SENSOR_LEFT, CW_Channel_Yellow, FALSE))){ //jaune à gauche
					state=WAIT_WHITE;
				}else if((global.color==YELLOW)&&(CW_is_color_detected(CW_SENSOR_LEFT, CW_Channel_Blue, FALSE))){ //bleu à gauche
					state=WAIT_WHITE;
				}
			}else{
				if((global.color==BLUE)&&(CW_is_color_detected(CW_SENSOR_RIGHT, CW_Channel_Yellow, FALSE))){ //jaune à droite
					state=WAIT_WHITE;
				}else if((global.color==YELLOW)&&(CW_is_color_detected(CW_SENSOR_RIGHT, CW_Channel_Blue, FALSE))){ //bleu à droite
					state=WAIT_WHITE;
				}
			}
			break;

		case WAIT_WHITE:
			if(side==LEFT){
				if(CW_is_color_detected(CW_SENSOR_LEFT, CW_Channel_White, FALSE)){ //blanc à gauche
					state=STOP;
				}
			}else{
				if(CW_is_color_detected(CW_SENSOR_RIGHT, CW_Channel_White, FALSE)){ //blanc à droite
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
		}
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
				debug_printf("default case in sub_harry_return_modules\n");
			break;
	}

	return IN_PROGRESS;
}


// la place des modules est comptée en partant du centre "Atos"
/*error_e manager_return_modules(){ => Fonction à ne pas utiliser
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_DEPOSE_MODULES_CENTRE,
			INIT,
			RETURN_MODULE_DROP_MIDDLE,
			RETURN_MODULE_DROP_OUR_CENTER,
			RETURN_MODULE_DROP_ADV_CENTER,
			RETURN_MODULE_DROP_OUR_SIDE,
			RETURN_MODULE_DROP_ADV_SIDE,
			ERROR,
			DONE
		);

	switch(state){
		case INIT:

// ATTENTION : Il n y a pas de GET_in, comment fait on pour se rendre au point demandé ?
			//module au centre
			if(MOONBASES_getModuleType(1, MODULE_MOONBASE_MIDDLE) == MODULE_POLY){ 				//apelle de fonction pour aller à coté du module
				state = check_sub_action_result(deplacement1, state, RETURN_MODULE_DROP_MIDDLE, ERROR);	//state = try_going + try_go_angle
			}
			if(MOONBASES_getModuleType(2, MODULE_MOONBASE_MIDDLE) == MODULE_POLY){
				state = check_sub_action_result(deplacemen2t, state, RETURN_MODULE_DROP_MIDDLE, ERROR);
			}
			if(MOONBASES_getModuleType(3, MODULE_MOONBASE_MIDDLE) == MODULE_POLY){
				state = check_sub_action_result(deplacement3, state, RETURN_MODULE_DROP_MIDDLE, ERROR);
			}
			if(MOONBASES_getModuleType(4, MODULE_MOONBASE_MIDDLE) == MODULE_POLY){
				state = check_sub_action_result(deplacement, state, RETURN_MODULE_DROP_MIDDLE, ERROR);
			}
			if(MOONBASES_getModuleType(5, MODULE_MOONBASE_MIDDLE) == MODULE_POLY){
				state = check_sub_action_result(deplacement, state, RETURN_MODULE_DROP_MIDDLE, ERROR);
			}
			if(MOONBASES_getModuleType(6, MODULE_MOONBASE_MIDDLE) == MODULE_POLY){ // problème de position
				state = check_sub_action_result(deplacement, state, RETURN_MODULE_DROP_MIDDLE, ERROR);
			}

			//module a notre centre
			if(MOONBASES_getModuleType(1, MODULE_MOONBASE_OUR_CENTER) == MODULE_POLY){
				state = check_sub_action_result(deplacement, state, RETURN_MODULE_DROP_OUR_CENTER, ERROR);
			}
			if(MOONBASES_getModuleType(2, MODULE_MOONBASE_OUR_CENTER) == MODULE_POLY){
				state = check_sub_action_result(deplacement, state, RETURN_MODULE_DROP_OUR_CENTER, ERROR);
			}
			if(MOONBASES_getModuleType(3, MODULE_MOONBASE_OUR_CENTER) == MODULE_POLY){
				state = check_sub_action_result(deplacement, state, RETURN_MODULE_DROP_OUR_CENTER, ERROR);
			}
			if(MOONBASES_getModuleType(4, MODULE_MOONBASE_OUR_CENTER) == MODULE_POLY){
				state = check_sub_action_result(deplacement, state, RETURN_MODULE_DROP_OUR_CENTER, ERROR);
			}
			if(MOONBASES_getModuleType(5, MODULE_MOONBASE_OUR_CENTER) == MODULE_POLY){
				state = check_sub_action_result(deplacement, state, RETURN_MODULE_DROP_OUR_CENTER, ERROR);
			}
			if(MOONBASES_getModuleType(6, MODULE_MOONBASE_OUR_CENTER) == MODULE_POLY){
				state = check_sub_action_result(deplacement, state, RETURN_MODULE_DROP_OUR_CENTER, ERROR);
			}

			//module centre adverse
			if(MOONBASES_getModuleType(1, MODULE_MOONBASE_ADV_CENTER) == MODULE_POLY){
				state = check_sub_action_result(deplacement, state, RETURN_MODULE_DROP_ADV_CENTER, ERROR);
			}
			if(MOONBASES_getModuleType(2, MODULE_MOONBASE_ADV_CENTER) == MODULE_POLY){
				state = check_sub_action_result(deplacement, state, RETURN_MODULE_DROP_ADV_CENTER, ERROR);
			}
			if(MOONBASES_getModuleType(3, MODULE_MOONBASE_ADV_CENTER) == MODULE_POLY){
				state = check_sub_action_result(deplacement, state, RETURN_MODULE_DROP_ADV_CENTER, ERROR);
			}
			if(MOONBASES_getModuleType(4, MODULE_MOONBASE_ADV_CENTER) == MODULE_POLY){
				state = check_sub_action_result(deplacement, state, RETURN_MODULE_DROP_ADV_CENTER, ERROR);
			}
			if(MOONBASES_getModuleType(5, MODULE_MOONBASE_ADV_CENTER) == MODULE_POLY){
				state = check_sub_action_result(deplacement, state, RETURN_MODULE_DROP_ADV_CENTER, ERROR);
			}
			if(MOONBASES_getModuleType(6, MODULE_MOONBASE_ADV_CENTER) == MODULE_POLY){
				state = check_sub_action_result(deplacement, state, RETURN_MODULE_DROP_ADV_CENTER, ERROR);
			}

			//module notre cote
			if(MOONBASES_getModuleType(1, MODULE_MOONBASE_OUR_SIDE) == MODULE_POLY){
				state = check_sub_action_result(deplacement, state, RETURN_MODULE_DROP_OUR_SIDE, ERROR);
			}
			if(MOONBASES_getModuleType(2, MODULE_MOONBASE_OUR_SIDE) == MODULE_POLY){
				state = check_sub_action_result(deplacement, state, RETURN_MODULE_DROP_OUR_SIDE, ERROR);
			}
			if(MOONBASES_getModuleType(3, MODULE_MOONBASE_OUR_SIDE) == MODULE_POLY){
				state = check_sub_action_result(deplacement, state, RETURN_MODULE_DROP_OUR_SIDE, ERROR);
			}
			if(MOONBASES_getModuleType(4, MODULE_MOONBASE_OUR_SIDE) == MODULE_POLY){
				state = check_sub_action_result(deplacement, state, RETURN_MODULE_DROP_OUR_SIDE, ERROR);
			}

			//module cote adverse
			if(MOONBASES_getModuleType(1, MODULE_MOONBASE_ADV_SIDE) == MODULE_POLY){
				state = check_sub_action_result(deplacement, state, RETURN_MODULE_DROP_ADV_SIDE, ERROR);
			}
			if(MOONBASES_getModuleType(2, MODULE_MOONBASE_ADV_SIDE) == MODULE_POLY){
				state = check_sub_action_result(deplacement, state, RETURN_MODULE_DROP_ADV_SIDE, ERROR);
			}
			if(MOONBASES_getModuleType(3, MODULE_MOONBASE_ADV_SIDE) == MODULE_POLY){
				state = check_sub_action_result(deplacement, state, RETURN_MODULE_DROP_ADV_SIDE, ERROR);
			}
			if(MOONBASES_getModuleType(4, MODULE_MOONBASE_ADV_SIDE) == MODULE_POLY){
				state = check_sub_action_result(deplacement, state, RETURN_MODULE_DROP_ADV_SIDE, ERROR);
			}
			else{

			}
			break;

			//state = check_sub_action_result(sub_harry_return_modules(RIGHT,LEFT), state, DONE, ERROR);

		case RETURN_MODULE_DROP_MIDDLE:
			state = check_sub_action_result(sub_harry_return_modules(RIGHT), state, DONE, ERROR);   //choix gauche ou droite
			break;

		case RETURN_MODULE_DROP_OUR_CENTER:
			state = check_sub_action_result(sub_harry_return_modules(RIGHT), state, DONE, ERROR);
			break;

		case RETURN_MODULE_DROP_ADV_CENTER:
			state = check_sub_action_result(sub_harry_return_modules(RIGHT), state, DONE, ERROR);
			break;

		case RETURN_MODULE_DROP_OUR_SIDE:
			state = check_sub_action_result(sub_harry_return_modules(RIGHT), state, DONE, ERROR);
			break;

		case RETURN_MODULE_DROP_ADV_SIDE:
			state = check_sub_action_result(sub_harry_return_modules(RIGHT), state, DONE, ERROR);
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
				debug_printf("default case in manager_return_modules\n");
			break;
	}

	return IN_PROGRESS;
}*/
