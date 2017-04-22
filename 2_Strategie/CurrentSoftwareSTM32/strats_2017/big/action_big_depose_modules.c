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


//#include "action_big.h"
//#include "../../propulsion/movement.h"
//#include "../../propulsion/astar.h"
//#include "../../QS/QS_stateMachineHelper.h"
//#include "../../QS/QS_outputlog.h"
#include "../../QS/QS_types.h"
//#include "../../QS/QS_IHM.h"
//#include  "../../utils/generic_functions.h"
//#include "../../actuator/act_functions.h"
#include "../../actuator/queue.h"
//#include "../../utils/actionChecker.h"

//permet de r�gler la distance entre la base c�t� et le robot(largeurBase+distance):
#define DISTANCE_BASE_SIDE_ET_ROBOT	((Uint16) 120+200)

// Fonctions priv�es
//error_e sub_harry_get_in_our_side_depose_module_centre();
//error_e sub_harry_get_in_middle_adv_side_depose_module_centre();
//error_e sub_harry_get_in_adv_side_depose_module_centre();
//error_e sub_harry_get_in_middle_our_side_depose_module_centre();

Uint8 nbCylindresSurBase = 0;


error_e sub_harry_depose_modules_centre(ELEMENTS_property_e modules, ELEMENTS_side_e robot_side, ELEMENTS_side_match_e basis_side){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_DEPOSE_MODULES_CENTRE,
			INIT,
			GET_IN,
			GO_TO_DEPOSE_MODULE_POS,
			GO_TO_DEPOSE_MODULE,

			DOWN_PUSHER_RIGHT,
			DOWN_PUSHER_LEFT,
			UP_PUSHER_RIGHT,
			UP_PUSHER_LEFT,

			PUSH_MODULE,
			PUSH_MODULE_RETURN,
			NEXT_DEPOSE_MODULE_LEFT,
			NEXT_DEPOSE_MODULE_RIGHT,
			ERROR,
			DONE
		);

	switch(state){
		case INIT:
			state=GET_IN;
			break;

		case GET_IN:
			state=check_sub_action_result(sub_harry_get_in_depose_modules_centre(modules, basis_side), state, GO_TO_DEPOSE_MODULE_POS, ERROR);
			break;

		case GO_TO_DEPOSE_MODULE_POS:
			if(((modules == OUR_ELEMENT) && (basis_side == OUR_SIDE)) || ((modules == ADV_ELEMENT) && (basis_side == ADV_SIDE))){

			state=check_sub_action_result(sub_harry_get_in_pos_1_depose_module_centre(), state, GO_TO_DEPOSE_MODULE, ERROR);

			}else if(((modules == OUR_ELEMENT) && (basis_side == ADV_SIDE)) || ((modules == ADV_ELEMENT) && (basis_side == OUR_SIDE))){
				state = check_sub_action_result(sub_harry_get_in_pos_2_depose_module_centre(), state, GO_TO_DEPOSE_MODULE, ERROR);

			}else if(((modules == NEUTRAL_ELEMENT) && (basis_side == OUR_SIDE)) || ((modules == NEUTRAL_ELEMENT) && (basis_side == ADV_SIDE))){
				state = check_sub_action_result(sub_harry_get_in_pos_3_depose_module_centre(), state, GO_TO_DEPOSE_MODULE, ERROR);

			}else
				state = ERROR;
			break;

			// (basis_side == ADV_SIDE) = nous + bleu gauche
			// (basis_side == ADV_SIDE) = adv + bleu droite
			// (basis_side == ADV_SIDE) = nous + jaune droite
			// (basis_side == ADV_SIDE) = adv + jaune droite
//advance

		case GO_TO_DEPOSE_MODULE: // decompte des modules regarde combien

 			if(((basis_side == OUR_SIDE)&&(global.color == YELLOW))||((basis_side == ADV_SIDE)&&(global.color == BLUE)) ){
				state = check_sub_action_result(sub_act_harry_mae_dispose_modules(MODULE_STOCK_RIGHT, ARG_DISPOSE_ONE_CYLINDER_FOLLOW_BY_ANOTHER), state, DOWN_PUSHER_RIGHT, ERROR);
			}
			else{
				state = check_sub_action_result(sub_act_harry_mae_dispose_modules(MODULE_STOCK_LEFT, ARG_DISPOSE_ONE_CYLINDER_FOLLOW_BY_ANOTHER), state, DOWN_PUSHER_LEFT, ERROR);
			}

			// ajouter le module sur le tableau
			//STOCKS_addModule(moduleType_e type, moduleStockLocation_e storage)
			break;

		case DOWN_PUSHER_RIGHT: // on sort le pusher
			if (entrance){
							ACT_push_order(ACT_CYLINDER_PUSHER_RIGHT,  ACT_CYLINDER_PUSHER_RIGHT_OUT);
						}
					state= check_act_status(ACT_QUEUE_Cylinder_pusher_right, state, PUSH_MODULE, ERROR);
			break;

		case DOWN_PUSHER_LEFT:
			if (entrance){
							ACT_push_order(ACT_CYLINDER_PUSHER_LEFT,  ACT_CYLINDER_PUSHER_LEFT_OUT);
						}
					state= check_act_status(ACT_QUEUE_Cylinder_pusher_left, state, PUSH_MODULE, ERROR);
			break;


		case PUSH_MODULE:
			// si coleur blue, notre cote est gauche

			if((modules == OUR_ELEMENT) && (basis_side == OUR_SIDE)){
				// pos 1
				if(global.color == BLUE){
					state = try_going(1730, COLOR_Y(880), state, UP_PUSHER_LEFT, ERROR, FAST, FORWARD, DODGE_AND_WAIT, END_AT_LAST_POINT);
				}else{
					state = try_going(1730, COLOR_Y(880), state, UP_PUSHER_RIGHT, ERROR, FAST, FORWARD, DODGE_AND_WAIT, END_AT_LAST_POINT);
				}
			}else if((modules == OUR_ELEMENT) && (basis_side == ADV_SIDE)){

				// pos 2
				if(global.color == BLUE){
					state = try_going(1390, COLOR_Y(1220), state, UP_PUSHER_RIGHT, ERROR, FAST, FORWARD, DODGE_AND_WAIT, END_AT_LAST_POINT);
				}else{
					state = try_going(1390, COLOR_Y(1220), state, UP_PUSHER_LEFT, ERROR, FAST, FORWARD, DODGE_AND_WAIT, END_AT_LAST_POINT);
				}
			}else if((modules == NEUTRAL_ELEMENT) && (basis_side == OUR_SIDE)){

				 // pos 3
				if(global.color == BLUE){
					state = try_going(1320, COLOR_Y(1250), state, UP_PUSHER_LEFT, ERROR, FAST, FORWARD, DODGE_AND_WAIT, END_AT_LAST_POINT);
				}else{
					state = try_going(1320, COLOR_Y(1250), state, UP_PUSHER_RIGHT, ERROR, FAST, FORWARD, DODGE_AND_WAIT, END_AT_LAST_POINT);
				}
			}else if((modules == NEUTRAL_ELEMENT) && (basis_side == ADV_SIDE)){
				// pos 4
				if(global.color == BLUE){
					state = try_going(1320, COLOR_Y(1700), state, UP_PUSHER_RIGHT, ERROR, FAST, FORWARD, DODGE_AND_WAIT, END_AT_LAST_POINT);
				}else{
					state = try_going(1320, COLOR_Y(1700), state, UP_PUSHER_LEFT, ERROR, FAST, FORWARD, DODGE_AND_WAIT, END_AT_LAST_POINT);
				}
			}else if((modules == ADV_ELEMENT) && (basis_side == OUR_SIDE)){
				// pos 5
				if(global.color == BLUE){
					state = try_going(1390, COLOR_Y(1780), state, UP_PUSHER_LEFT, ERROR, FAST, FORWARD, DODGE_AND_WAIT, END_AT_LAST_POINT);
				}else{
					state = try_going(1390, COLOR_Y(1780), state, UP_PUSHER_RIGHT, ERROR, FAST, FORWARD, DODGE_AND_WAIT, END_AT_LAST_POINT);
				}
			}else if((modules == ADV_ELEMENT) && (basis_side == ADV_SIDE)){
				// pos 6
				if(global.color == BLUE){
					state = try_going(1730, COLOR_Y(2120), state, UP_PUSHER_RIGHT, ERROR, FAST, FORWARD, DODGE_AND_WAIT, END_AT_LAST_POINT);
				}else{
					state = try_going(1730, COLOR_Y(2120), state, UP_PUSHER_LEFT, ERROR, FAST, FORWARD, DODGE_AND_WAIT, END_AT_LAST_POINT);
				}
			}else
				state = ERROR;
			break;

		case UP_PUSHER_RIGHT: // on rentre le pusher
			if (entrance){
							ACT_push_order(ACT_CYLINDER_PUSHER_RIGHT,  ACT_CYLINDER_PUSHER_RIGHT_IN);
						}
					state= check_act_status(ACT_QUEUE_Cylinder_pusher_right, state, PUSH_MODULE_RETURN, ERROR);
			break;

		case UP_PUSHER_LEFT:
			if (entrance){
							ACT_push_order(ACT_CYLINDER_PUSHER_LEFT,  ACT_CYLINDER_PUSHER_LEFT_IN);
						}
					state= check_act_status(ACT_QUEUE_Cylinder_pusher_left, state, PUSH_MODULE_RETURN, ERROR);
			break;


		case PUSH_MODULE_RETURN:
			// si coleur blue, notre cote est gauche

			if((modules == OUR_ELEMENT) && (basis_side == OUR_SIDE)){
				// pos 1
				if(global.color == BLUE){
					state = try_going(1610, COLOR_Y(760), state, NEXT_DEPOSE_MODULE_LEFT, ERROR, FAST, BACKWARD, DODGE_AND_WAIT, END_AT_LAST_POINT);
				}else{
					state = try_going(1610, COLOR_Y(760), state, NEXT_DEPOSE_MODULE_RIGHT, ERROR, FAST, BACKWARD, DODGE_AND_WAIT, END_AT_LAST_POINT);
				}
			}else if((modules == OUR_ELEMENT) && (basis_side == ADV_SIDE)){

				// pos 2
				if(global.color == BLUE){
					state = try_going(1270, COLOR_Y(1100), state, NEXT_DEPOSE_MODULE_RIGHT, ERROR, FAST, BACKWARD, DODGE_AND_WAIT, END_AT_LAST_POINT);
				}else{
					state = try_going(1270, COLOR_Y(1100), state, NEXT_DEPOSE_MODULE_LEFT, ERROR, FAST, BACKWARD, DODGE_AND_WAIT, END_AT_LAST_POINT);
				}
			}else if((modules == NEUTRAL_ELEMENT) && (basis_side == OUR_SIDE)){

				 // pos 3
				if(global.color == BLUE){
					state = try_going(1200, COLOR_Y(1250), state, NEXT_DEPOSE_MODULE_LEFT, ERROR, FAST, BACKWARD, DODGE_AND_WAIT, END_AT_LAST_POINT);
				}else{
					state = try_going(1200, COLOR_Y(1250), state, NEXT_DEPOSE_MODULE_RIGHT, ERROR, FAST, BACKWARD, DODGE_AND_WAIT, END_AT_LAST_POINT);
				}
			}else if((modules == NEUTRAL_ELEMENT) && (basis_side == ADV_SIDE)){
				// pos 4
				if(global.color == BLUE){
					state = try_going(1200, COLOR_Y(1700), state, NEXT_DEPOSE_MODULE_RIGHT, ERROR, FAST, BACKWARD, DODGE_AND_WAIT, END_AT_LAST_POINT);
				}else{
					state = try_going(1200, COLOR_Y(1700), state, NEXT_DEPOSE_MODULE_LEFT, ERROR, FAST, BACKWARD, DODGE_AND_WAIT, END_AT_LAST_POINT);
				}
			}else if((modules == ADV_ELEMENT) && (basis_side == OUR_SIDE)){
				// pos 5
				if(global.color == BLUE){
					state = try_going(1270, COLOR_Y(1900), state, NEXT_DEPOSE_MODULE_LEFT, ERROR, FAST, BACKWARD, DODGE_AND_WAIT, END_AT_LAST_POINT);
				}else{
					state = try_going(1270, COLOR_Y(1900), state, NEXT_DEPOSE_MODULE_RIGHT, ERROR, FAST, BACKWARD, DODGE_AND_WAIT, END_AT_LAST_POINT);
				}
			}else if((modules == ADV_ELEMENT) && (basis_side == ADV_SIDE)){
				// pos 6
				if(global.color == BLUE){
					state = try_going(1610, COLOR_Y(2240), state, NEXT_DEPOSE_MODULE_RIGHT, ERROR, FAST, BACKWARD, DODGE_AND_WAIT, END_AT_LAST_POINT);
				}else{
					state = try_going(1610, COLOR_Y(2240), state, NEXT_DEPOSE_MODULE_LEFT, ERROR, FAST, BACKWARD, DODGE_AND_WAIT, END_AT_LAST_POINT);
				}
			}else
				state = ERROR;
			break;

		case NEXT_DEPOSE_MODULE_LEFT:
			if(STOCKS_isEmpty(MODULE_STOCK_LEFT)){
				state = DONE;
			}
			else{
				state = GO_TO_DEPOSE_MODULE;
			}

			//module suivant
			//regarde combien de module on �t� pos� si + de 6 partir (flag depose pleine)
			//else regarde si le stock � entre des modules; si oui GO_TO_DEPOSE_MODULE et compl� +1
			//else if 6 module d�pos� partir (autre zone/quitter sub)
			//else if plus de module � d�pos� (flag nb depos�), partir
			//else partir
			break;

		case NEXT_DEPOSE_MODULE_RIGHT:
			if(STOCKS_isEmpty(MODULE_STOCK_RIGHT)){
				state = DONE;
			}
			else{
				state = GO_TO_DEPOSE_MODULE;
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
				debug_printf("default case in sub_harry_depose_modules_centre\n");
			break;
	}
	return IN_PROGRESS;
}


error_e sub_harry_get_in_depose_modules_centre(ELEMENTS_property_e modules, ELEMENTS_side_match_e basis_side){
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
			if((modules == OUR_ELEMENT) && (basis_side == OUR_SIDE)){
				state = GET_IN_POS_1;  //notre depose de notre cote  SIM 6

			}else if((modules == OUR_ELEMENT) && (basis_side == ADV_SIDE)){
				state = GET_IN_POS_2; //notre depose du cote adv SIM 5

			}else if((modules == NEUTRAL_ELEMENT) && (basis_side == OUR_SIDE)){
				state = GET_IN_POS_3; //centre depose de notre code SIM 4

			}else if((modules == NEUTRAL_ELEMENT) && (basis_side == ADV_SIDE)){
				state = GET_IN_POS_4;  //adv depose du cote adv SIM 3

			}else if((modules == ADV_ELEMENT) && (basis_side == OUR_SIDE)){
				state = GET_IN_POS_5; //adv depose de notre cote SIM 2

			}else if((modules == ADV_ELEMENT) && (basis_side == ADV_SIDE)){
				state = GET_IN_POS_6; //centre depose du cote adv SIM 1


			}else
				state = ERROR;
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
			PATHFIND,
			ERROR,
			DONE
		);

	switch(state){
		case INIT:
			if(i_am_in_square(800, 1400, 300, 900)){
				state = GET_IN_FROM_OUR_SQUARE;
			}else if(i_am_in_square(200, 1100, 900, 2100)){
				state = GET_IN_FROM_MIDDLE_SQUARE;
			}else if(i_am_in_square(800, 1400, 2100, 2700)){
				state = GET_IN_FROM_ADV_SQUARE;
			}else{
				state = PATHFIND;
			}
			break;

		case GET_IN_FROM_OUR_SQUARE:
			state = try_going(1450, COLOR_Y(600), state, DONE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT,END_AT_BRAKE);
			break;

		case GET_IN_FROM_MIDDLE_SQUARE:
			state = try_going(1000, COLOR_Y(1000), state, GET_IN_FROM_OUR_SQUARE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT,END_AT_BRAKE);
			break;

		case GET_IN_FROM_ADV_SQUARE:
			state = try_going(900, COLOR_Y(2000), state, GET_IN_FROM_MIDDLE_SQUARE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT,END_AT_BRAKE);
			break;

		case PATHFIND:
			state = ASTAR_try_going(1450, COLOR_Y(600), state, DONE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_BRAKE);
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
			PATHFIND,
			ERROR,
			DONE
		);

	switch(state){
		case INIT:
			if(i_am_in_square(800, 1400, 300, 900)){
				state = GET_IN_FROM_OUR_SQUARE;
			}else if(i_am_in_square(200, 1100, 900, 2100)){
				state = GET_IN_FROM_MIDDLE_SQUARE;
			}else if(i_am_in_square(800, 1400, 2100, 2700)){
				state = GET_IN_FROM_ADV_SQUARE;
			}else{
				state = PATHFIND;
			}
			break;

		case GET_IN_FROM_OUR_SQUARE:
			state = try_going(1100, COLOR_Y(925), state, DONE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT,END_AT_BRAKE);
			break;

		case GET_IN_FROM_MIDDLE_SQUARE:
			state = try_going(1000, COLOR_Y(1000), state, GET_IN_FROM_OUR_SQUARE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT,END_AT_BRAKE);
			break;

		case GET_IN_FROM_ADV_SQUARE:
			state = try_going(900, COLOR_Y(2000), state, GET_IN_FROM_MIDDLE_SQUARE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT,END_AT_BRAKE);
			break;

		case PATHFIND:
			state = ASTAR_try_going(1100, COLOR_Y(925), state, DONE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_BRAKE);
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
			state = try_going(975, COLOR_Y(1250), state, AVANCE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT,END_AT_BRAKE);
			break;

		//case GET_IN_FROM_MIDDLE_SQUARE:
		//	state = try_going(1000, COLOR_Y(1250), state, GET_IN_FROM_OUR_SQUARE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT,END_AT_BRAKE);
		//	break;

		case GET_IN_FROM_ADV_SQUARE:
			state = try_going(900, COLOR_Y(2000), state, GET_IN_FROM_OUR_SQUARE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT,END_AT_BRAKE);
			break;

		case AVANCE:
			state = try_going(1125, COLOR_Y(1250), state, DONE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case PATHFIND:
			state = ASTAR_try_going(975, COLOR_Y(1250), state, DONE, ERROR, FAST, ANY_WAY, DODGE_AND_NO_WAIT, END_AT_BRAKE);
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

error_e sub_harry_get_in_pos_4_depose_module_centre(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_GET_IN_POS_4_DEPOSE_MODULES_CENTRE,
			INIT,
			GET_IN_FROM_OUR_SQUARE,
			//GET_IN_FROM_MIDDLE_SQUARE,
			GET_IN_FROM_ADV_SQUARE,
			PATHFIND,
			ERROR,
			DONE
		);

	switch(state){
		case INIT:
			if((i_am_in_square(800, 1400, 300, 900))||(i_am_in_square(200, 1100, 900, 2100))){
				state = GET_IN_FROM_OUR_SQUARE;
			//}else if(i_am_in_square(200, 1100, 900, 2100)){
			//	state = GET_IN_FROM_MIDDLE_SQUARE;
			}else if(i_am_in_square(800, 1400, 2100, 2700)){
				state = GET_IN_FROM_ADV_SQUARE;
			}else{
				state = PATHFIND;
			}
			break;

		case GET_IN_FROM_OUR_SQUARE:
			state = try_going(975, COLOR_Y(1700), state, DONE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT,END_AT_BRAKE);
			break;

		//case GET_IN_FROM_MIDDLE_SQUARE:
		//	state = try_going(1000, COLOR_Y(1750), state, GET_IN_FROM_OUR_SQUARE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT,END_AT_BRAKE);
		//	break;

		case GET_IN_FROM_ADV_SQUARE:
			state = try_going(900, COLOR_Y(1000), state, GET_IN_FROM_OUR_SQUARE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT,END_AT_BRAKE);
			break;

		case PATHFIND:
			state = ASTAR_try_going(975, COLOR_Y(1700), state, DONE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_BRAKE);
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
			PATHFIND,
			ERROR,
			DONE
		);

	switch(state){
		case INIT:
			if(i_am_in_square(800, 1400, 300, 900)){
				state = GET_IN_FROM_OUR_SQUARE;
			}else if(i_am_in_square(200, 1100, 900, 2100)){
				state = GET_IN_FROM_MIDDLE_SQUARE;
			}else if(i_am_in_square(800, 1400, 2100, 2700)){
				state = GET_IN_FROM_ADV_SQUARE;
			}else{
				state = PATHFIND;
			}
			break;

		case GET_IN_FROM_OUR_SQUARE:
			state = try_going(1100, COLOR_Y(2075), state, DONE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT,END_AT_BRAKE);
			break;

		case GET_IN_FROM_MIDDLE_SQUARE:
			state = try_going(1000, COLOR_Y(2000), state, GET_IN_FROM_OUR_SQUARE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT,END_AT_BRAKE);
			break;

		case GET_IN_FROM_ADV_SQUARE:
			state = try_going(900, COLOR_Y(1000), state, GET_IN_FROM_MIDDLE_SQUARE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT,END_AT_BRAKE);
			break;

		case PATHFIND:
			state = ASTAR_try_going(1100, COLOR_Y(2075), state, DONE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_BRAKE);
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
			if(i_am_in_square(800, 1400, 300, 900)){
				state = GET_IN_FROM_OUR_SQUARE;
			}else if(i_am_in_square(200, 1100, 900, 2100)){
				state = GET_IN_FROM_MIDDLE_SQUARE;
			}else if(i_am_in_square(800, 1400, 2100, 2700)){
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

error_e sub_harry_pos_1_depose_module_centre(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_POS_1_DEPOSE_MODULES_CENTRE,
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
			state = try_going(1610, COLOR_Y(760), state, DONE, ERROR, FAST, FORWARD, NO_DODGE_AND_NO_WAIT,END_AT_BRAKE);
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


error_e sub_harry_depose_modules_side_manager(ELEMENTS_property_e modules,ELEMENTS_side_e robot_side, ELEMENTS_side_match_e basis_side){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_DEPOSE_MODULES_SIDE_INTRO,
			INIT,
			ERROR,
			DONE
			);

	switch(state){

	case INIT:
		//state = check_sub_action_result(sub_harry_depose_modules_side_intro(modules, robot_side, bassis_side), state, DONE, ERROR);
		//sub_harry_depose_modules_side_intro(ELEMENTS_property_e modules,ELEMENTS_side_e robot_side, ELEMENTS_side_match_e basis_side);
		/*if(nbCylindresSurBase<5){

		}
		*/
		break;

	case DONE:
		ACT_push_order(ACT_CYLINDER_PUSHER_LEFT, ACT_CYLINDER_PUSHER_LEFT_IDLE);
		on_turning_point();
		RESET_MAE();
		return END_OK;
		break;

	case ERROR:
		ACT_push_order(ACT_CYLINDER_PUSHER_LEFT, ACT_CYLINDER_PUSHER_LEFT_IDLE);
		RESET_MAE();
		return NOT_HANDLED;
		break;
	}

	return IN_PROGRESS;
}

error_e sub_harry_depose_modules_side_intro(ELEMENTS_property_e modules,ELEMENTS_side_e robot_side, ELEMENTS_side_match_e basis_side){
	//liste de ce que je dois faire:
		//g�rer les actionneurs pour la d�pose
		//DETERMINE_NB_CYLINDRE_SUR_BASE_DESCENDRE_BRAS ACT_push_order...
		//VERIFICATION_ACTIONNEUR a faire!!!!
		//faire Procedure de depose

	/*
	 * Tu peux cr�er une autre sub pour tes get_in/d�placements
	 *
	 * Pour voir si tu as un module, tu utilises la fonction STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_ENTRY, param );
	 * Si �a t'int�resse elle est utilis�e dans le fichier action_big_act_modules.c dans la sub sub_act_harry_mae_modules()
	 *
	 * Autre commentaire : line 455
	 */

	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_DEPOSE_MODULES_SIDE_INTRO,
			INIT,
			ERROR,
			ERROR_DETREMIN_NB_CYLINDRES_NORD,
			ERROR_DETREMIN_NB_CYLINDRES_SUD,
			ERROR_DETREMIN_NB_CYLINDRES_1,
			ERROR_DETREMIN_NB_CYLINDRES_2,
			ERROR_DETREMIN_NB_CYLINDRES_3,
			ERROR_DETREMIN_NB_CYLINDRES,
			DONE,

		//sub deplacement et mesure du nombre de cylindres sur la base:
			LES_SQUARE_COLOR,
			NORD_1,
			NORD_EST_1,
			NORD_EST_2,
			SUD_EST_1,
			SUD_EST_2,
			SUD_EST_3,
			SUD_OUEST_1,
			NORD_OUEST_1,
			PATHFIND,
			FIN_DEPLACEMENTS,


			//DETERMINE_NB_CYLINDRE_SUR_BASE,
			//DETERMINE_NB_CYLINDRE_SUR_BASE_ROTATION,
			DETERMINE_NB_CYLINDRE_SUR_BASE_DESCENDRE_BRAS,
			DETERMINE_NB_CYLINDRE_SUR_BASE_AVANCER,
			DETERMINE_NB_CYLINDRE,
			DETERMINE_NB_CYLINDRE_SUR_BASE_REMONTER_BRAS,
			DETERMINE_NB_CYLINDRE_SUR_BASE_REPLACEMENT,


		//VERIFIER_COULEUR_CYLINDRE_DEJA_SUR_BASE:
			SI_4_CYLINDRES,//A revoir car on ne peut plus tourner les cylindres ext�rieurs...
			SI_3_CYLINDRES,
			SI_2_CYLINDRES,
			SI_1_CYLINDRES,
			SI_0_REPLACEMENT,

		//Maintenant on peut ajouter des cylindres:    ENFIN!
			CHOIX_DE_COTE_DEPOSE,
			DEPOSE_MANAGER,
			DEPOSE,
			FIN_DEPOSE
		);

	static bool_e side;

	switch(state){
		case INIT:
			if((robot_side==LEFT &&STOCKS_isEmpty(MODULE_STOCK_LEFT)) ||\
			   (robot_side==RIGHT && STOCKS_isEmpty(MODULE_STOCK_RIGHT))){
				state = DONE;
			}
			else{
				if(global.color==BLUE){
					if(basis_side==OUR_SIDE){
						endroit_depose = OUEST;
					}
					else {
						endroit_depose = EST;
					}
				}
				else {
					if(basis_side==OUR_SIDE){
						endroit_depose = EST;
					}
					else {
						endroit_depose = OUEST;
					}
				}
				state=LES_SQUARE_COLOR;
			}
			break;

		case ERROR:
			RESET_MAE();
			on_turning_point();
			return NOT_HANDLED;
			break;

		case ERROR_DETREMIN_NB_CYLINDRES:

			//remonter les bras poussoires:
			ACT_push_order(ACT_CYLINDER_PUSHER_LEFT,ACT_CYLINDER_PUSHER_LEFT_IN);
			ACT_push_order(ACT_CYLINDER_PUSHER_RIGHT,ACT_CYLINDER_PUSHER_RIGHT_IN);
			break;

		case ERROR_DETREMIN_NB_CYLINDRES_SUD:
			if(endroit_depose == OUEST){
				state = try_going(1250,DISTANCE_BASE_SIDE_ET_ROBOT,state,ERROR_DETREMIN_NB_CYLINDRES_1,\
						ERROR_DETREMIN_NB_CYLINDRES_NORD,FAST,ANY_WAY,NO_DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
			}
			else {
				state = try_going(1250,3000-DISTANCE_BASE_SIDE_ET_ROBOT,state,ERROR_DETREMIN_NB_CYLINDRES_1,\
						ERROR_DETREMIN_NB_CYLINDRES_NORD,FAST,ANY_WAY,NO_DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
			}
			break;

		case ERROR_DETREMIN_NB_CYLINDRES_NORD:
			if(endroit_depose == OUEST){
				state = try_going(650,DISTANCE_BASE_SIDE_ET_ROBOT,state,ERROR_DETREMIN_NB_CYLINDRES_2,\
						ERROR_DETREMIN_NB_CYLINDRES_SUD,FAST,ANY_WAY,NO_DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
			}
			else {
				state = try_going(650,3000-DISTANCE_BASE_SIDE_ET_ROBOT,state,ERROR_DETREMIN_NB_CYLINDRES_2,\
						ERROR_DETREMIN_NB_CYLINDRES_SUD,FAST,ANY_WAY,NO_DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
			}
			break;

		case ERROR_DETREMIN_NB_CYLINDRES_1:
			state = try_going(1200,COLOR_Y(380),state,ERROR,ERROR_DETREMIN_NB_CYLINDRES_3,\
					FAST,ANY_WAY,NO_DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
			break;

		case ERROR_DETREMIN_NB_CYLINDRES_2:
			state = try_going(1000,COLOR_Y(380),state,ERROR,ERROR_DETREMIN_NB_CYLINDRES_3,\
					FAST,ANY_WAY,NO_DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
			break;

		case ERROR_DETREMIN_NB_CYLINDRES_3:
			state = try_going(800,COLOR_Y(380),state,ERROR,ERROR_DETREMIN_NB_CYLINDRES_1,FAST,\
					ANY_WAY,NO_DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
			break;

		case DONE:
			RESET_MAE();
			on_turning_point();
			return END_OK;
			break;


		case LES_SQUARE_COLOR:
			if (i_am_in_square_color(0,1150,750,2250)){
				state = NORD_1;
			}

			else if (i_am_in_square_color(350,1150,2250,3000)){
				state = NORD_EST_1;
			}

			else if (i_am_in_square_color(1150,1400,2000,3000)){
				state = SUD_EST_1;
			}

			else if (i_am_in_square_color(1150,1400,0,1000)){
				state = SUD_OUEST_1;
			}

			else if (i_am_in_square_color(350,1150,0,750)){
				state = NORD_OUEST_1;
			}

			else{
				state=PATHFIND;
			}
			break;



//DEPLACEMENTS:

			case NORD_1:
				state = try_going(1000,COLOR_Y(750),NORD_1,FIN_DEPLACEMENTS,ERROR,FAST,ANY_WAY,NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
				break;

			case NORD_EST_1:
				state = try_going(1000,COLOR_Y(2250),NORD_EST_1,NORD_EST_2,ERROR,FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
				break;
			case NORD_EST_2:
				state = try_going(1000,COLOR_Y(750),NORD_EST_2,FIN_DEPLACEMENTS,ERROR,FAST,ANY_WAY,NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
				break;

			case SUD_EST_1:
				state = try_going(1150,COLOR_Y(2250),SUD_EST_1,SUD_EST_2,ERROR,FAST,ANY_WAY,NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
				break;
			case SUD_EST_2:
				state = try_going(1000,COLOR_Y(1500),SUD_EST_2,SUD_EST_3,ERROR,FAST,ANY_WAY,NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
				break;
			case SUD_EST_3:
				state = try_going(1000,COLOR_Y(750),SUD_EST_3,FIN_DEPLACEMENTS,ERROR,FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
				break;

			case SUD_OUEST_1:
				state = try_going(1150,COLOR_Y(450),SUD_OUEST_1,FIN_DEPLACEMENTS,ERROR,FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
				break;

			case NORD_OUEST_1:
				state = try_going(1150,COLOR_Y(450),NORD_OUEST_1,FIN_DEPLACEMENTS,ERROR,FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
				break;


			case PATHFIND:
				if(endroit_depose==OUEST){
					if(mon_point_de_rotation==SUD){
						state=ASTAR_try_going(1250,DISTANCE_BASE_SIDE_ET_ROBOT,PATHFIND,FIN_DEPLACEMENTS,\
								ERROR,FAST,ANY_WAY,NO_DODGE_AND_WAIT,END_AT_LAST_POINT);
					}
					else {
						state=ASTAR_try_going(650,DISTANCE_BASE_SIDE_ET_ROBOT,PATHFIND,FIN_DEPLACEMENTS,\
								ERROR,FAST,ANY_WAY,NO_DODGE_AND_WAIT,END_AT_LAST_POINT);
					}
				}
				else{
					if(mon_point_de_rotation==SUD){
						state=ASTAR_try_going(1250,3000-DISTANCE_BASE_SIDE_ET_ROBOT,PATHFIND,FIN_DEPLACEMENTS,\
								ERROR,FAST,ANY_WAY,NO_DODGE_AND_WAIT,END_AT_LAST_POINT);
					}
					else {
						state=ASTAR_try_going(650,3000-DISTANCE_BASE_SIDE_ET_ROBOT,PATHFIND,FIN_DEPLACEMENTS,\
								ERROR,FAST,ANY_WAY,NO_DODGE_AND_WAIT,END_AT_LAST_POINT);
					}
				}
				break;

			case FIN_DEPLACEMENTS:
				if(endroit_depose==OUEST){
					if(mon_point_de_rotation==SUD){
						state = try_going(1250,DISTANCE_BASE_SIDE_ET_ROBOT,FIN_DEPLACEMENTS,\
								DETERMINE_NB_CYLINDRE_SUR_BASE_AVANCER,ERROR,FAST, ANY_WAY, NO_DODGE_AND_WAIT,\
								END_AT_LAST_POINT);
					}
					else {
						state = try_going(650,DISTANCE_BASE_SIDE_ET_ROBOT,FIN_DEPLACEMENTS,\
								DETERMINE_NB_CYLINDRE_SUR_BASE_AVANCER,ERROR,FAST, ANY_WAY, NO_DODGE_AND_WAIT,\
								END_AT_LAST_POINT);
					}
				}
				else {
					if(mon_point_de_rotation==SUD){
						state = try_going(1250,3000-DISTANCE_BASE_SIDE_ET_ROBOT,FIN_DEPLACEMENTS,\
								DETERMINE_NB_CYLINDRE_SUR_BASE_AVANCER,ERROR,FAST, ANY_WAY, NO_DODGE_AND_WAIT,\
								END_AT_LAST_POINT);
					}
					else {
						state = try_going(650,3000-DISTANCE_BASE_SIDE_ET_ROBOT,FIN_DEPLACEMENTS,\
								DETERMINE_NB_CYLINDRE_SUR_BASE_AVANCER,ERROR,FAST, ANY_WAY, NO_DODGE_AND_WAIT,\
								END_AT_LAST_POINT);
					}
				}
				break;

/*
//Determiner le nombre de cylindre:
			case DETERMINE_NB_CYLINDRE_SUR_BASE_ROTATION:
				//Rotation vers PI:
				state = try_go_angle(PI4096, DETERMINE_NB_CYLINDRE_SUR_BASE_ROTATION, DETERMINE_NB_CYLINDRE_SUR_BASE_DESCENDRE_BRAS, ERROR, FAST, ANY_WAY, END_AT_LAST_POINT);
				break;
*/

			case DETERMINE_NB_CYLINDRE_SUR_BASE_DESCENDRE_BRAS:
				if(entrance){
					if(endroit_depose==OUEST && mon_point_de_rotation==SUD){
							////Descendre bras poussoir gauche;
							ACT_push_order(ACT_CYLINDER_PUSHER_LEFT,ACT_CYLINDER_PUSHER_LEFT_OUT);
							side = TRUE;
					}
					else if(endroit_depose==OUEST && mon_point_de_rotation==NORD){
							//Descendre bras poussoir droit;
							ACT_push_order(ACT_CYLINDER_PUSHER_RIGHT,ACT_CYLINDER_PUSHER_RIGHT_OUT);
							side = FALSE;
					}
					else if(endroit_depose==EST && mon_point_de_rotation==SUD){
							//Descendre bras poussoir droit;
							ACT_push_order(ACT_CYLINDER_PUSHER_RIGHT,ACT_CYLINDER_PUSHER_RIGHT_OUT);
							side = FALSE;
					}
					else{
						//Descendre bras poussoir gauche;
						ACT_push_order(ACT_CYLINDER_PUSHER_LEFT,ACT_CYLINDER_PUSHER_LEFT_OUT);
						side = TRUE;
					}
				}
				if(side){
					state = check_act_status(ACT_QUEUE_Cylinder_pusher_left, state, DETERMINE_NB_CYLINDRE_SUR_BASE_AVANCER, ERROR);
				}
				else{
					state = check_act_status(ACT_QUEUE_Cylinder_pusher_right, state, DETERMINE_NB_CYLINDRE_SUR_BASE_AVANCER, ERROR);
				}
				break;

			case DETERMINE_NB_CYLINDRE_SUR_BASE_AVANCER:
				//Avancer doucement jusqu'a bloquage:
				if(endroit_depose==OUEST){
					if(mon_point_de_rotation==SUD){
						state = try_rush(600,DISTANCE_BASE_SIDE_ET_ROBOT,\
								DETERMINE_NB_CYLINDRE_SUR_BASE_AVANCER,DETERMINE_NB_CYLINDRE,\
								DETERMINE_NB_CYLINDRE,FORWARD,NO_DODGE_AND_WAIT,TRUE);
					}
					else {
						state = try_rush(1250,DISTANCE_BASE_SIDE_ET_ROBOT,\
								DETERMINE_NB_CYLINDRE_SUR_BASE_AVANCER,DETERMINE_NB_CYLINDRE,\
								DETERMINE_NB_CYLINDRE,FORWARD,NO_DODGE_AND_WAIT,TRUE);
					}
				}
				else {
					if(mon_point_de_rotation==SUD){
						state = try_rush(600,3000-DISTANCE_BASE_SIDE_ET_ROBOT,\
								DETERMINE_NB_CYLINDRE_SUR_BASE_AVANCER,DETERMINE_NB_CYLINDRE,\
								DETERMINE_NB_CYLINDRE,FORWARD,NO_DODGE_AND_WAIT,TRUE);
					}
					else {
						state = try_rush(1250,3000-DISTANCE_BASE_SIDE_ET_ROBOT,\
								DETERMINE_NB_CYLINDRE_SUR_BASE_AVANCER,DETERMINE_NB_CYLINDRE,\
								DETERMINE_NB_CYLINDRE,FORWARD,NO_DODGE_AND_WAIT,TRUE);
					}
				}
				break;

			case DETERMINE_NB_CYLINDRE:
				if(mon_point_de_rotation==SUD){
					if (i_am_in_square_color(550,750,0,3000)){
						nbCylindresSurBase = 0;
						}
					else if (i_am_in_square_color(750,850,0,3000)){
						nbCylindresSurBase = 1;
						}
					else if (i_am_in_square_color(850,950,0,3000)){
						nbCylindresSurBase = 2;
						}
					else if (i_am_in_square_color(950,1050,0,3000)){
						nbCylindresSurBase = 3;
						}
					else if (i_am_in_square_color(1050,1150,0,3000)){
						nbCylindresSurBase = 4;
						}
					else{
						//ERROR
						state = ERROR;
					}
				}
				else {
					if (i_am_in_square_color(550,750,0,3000)){
						nbCylindresSurBase = 4;
						}
					else if (i_am_in_square_color(750,850,0,3000)){
						nbCylindresSurBase = 3;
						}
					else if (i_am_in_square_color(850,950,0,3000)){
						nbCylindresSurBase = 2;
						}
					else if (i_am_in_square_color(950,1050,0,3000)){
						nbCylindresSurBase = 1;
						}
					else if (i_am_in_square_color(1050,1150,0,3000)){
						nbCylindresSurBase = 0;
						}
					else{
						//ERROR
						state = ERROR;
					}
				}
				state = DETERMINE_NB_CYLINDRE_SUR_BASE_REMONTER_BRAS;
				break;

			case DETERMINE_NB_CYLINDRE_SUR_BASE_REMONTER_BRAS:
				if(endroit_depose==OUEST){
					if(mon_point_de_rotation==SUD){
						//MONTER bras poussoir gauche;
						ACT_push_order(ACT_CYLINDER_PUSHER_LEFT,ACT_CYLINDER_PUSHER_LEFT_IN);
					}
					else{
						//MONTER bras poussoir droit;
						ACT_push_order(ACT_CYLINDER_PUSHER_RIGHT,ACT_CYLINDER_PUSHER_RIGHT_IN);
					}
				}
				else{
					if(mon_point_de_rotation==SUD){
						//MONTER bras poussoir droit;
						ACT_push_order(ACT_CYLINDER_PUSHER_RIGHT,ACT_CYLINDER_PUSHER_RIGHT_IN);
					}
					else{
						//MONTER bras poussoir gauche;
						ACT_push_order(ACT_CYLINDER_PUSHER_LEFT,ACT_CYLINDER_PUSHER_LEFT_IN);
					}
				}
				state = DETERMINE_NB_CYLINDRE_SUR_BASE_REPLACEMENT;
				break;


			case DETERMINE_NB_CYLINDRE_SUR_BASE_REPLACEMENT:
				if(endroit_depose==OUEST){
					if(mon_point_de_rotation==SUD){
						if (nbCylindresSurBase == 4){
							state = try_going(1000,DISTANCE_BASE_SIDE_ET_ROBOT,\
									DETERMINE_NB_CYLINDRE_SUR_BASE_REPLACEMENT,SI_4_CYLINDRES,ERROR,FAST,\
									BACKWARD,DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
						}
						else if(nbCylindresSurBase == 3){
							state = try_going(900,DISTANCE_BASE_SIDE_ET_ROBOT,\
									DETERMINE_NB_CYLINDRE_SUR_BASE_REPLACEMENT,SI_3_CYLINDRES,ERROR,FAST,\
									BACKWARD,DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
						}
						else if(nbCylindresSurBase == 2){
							state = try_going(800,DISTANCE_BASE_SIDE_ET_ROBOT,\
									DETERMINE_NB_CYLINDRE_SUR_BASE_REPLACEMENT,SI_2_CYLINDRES,ERROR,FAST,\
									BACKWARD,DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
						}
						else if(nbCylindresSurBase == 1){
							state = try_going(700,DISTANCE_BASE_SIDE_ET_ROBOT,\
									DETERMINE_NB_CYLINDRE_SUR_BASE_REPLACEMENT,SI_1_CYLINDRES,ERROR,FAST,\
									BACKWARD,DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
						}
						else if(nbCylindresSurBase == 0){
							state = SI_0_REPLACEMENT;
						}
					}
					else {
						if (nbCylindresSurBase == 4){
							state = try_going(750,DISTANCE_BASE_SIDE_ET_ROBOT,\
									DETERMINE_NB_CYLINDRE_SUR_BASE_REPLACEMENT,SI_4_CYLINDRES,ERROR,FAST,\
									BACKWARD,DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
						}
						else if(nbCylindresSurBase == 3){
							state = try_going(850,DISTANCE_BASE_SIDE_ET_ROBOT,\
									DETERMINE_NB_CYLINDRE_SUR_BASE_REPLACEMENT,SI_3_CYLINDRES,ERROR,FAST,\
									BACKWARD,DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
						}
						else if(nbCylindresSurBase == 2){
							state = try_going(950,DISTANCE_BASE_SIDE_ET_ROBOT,\
									DETERMINE_NB_CYLINDRE_SUR_BASE_REPLACEMENT,SI_2_CYLINDRES,ERROR,FAST,\
									BACKWARD,DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
						}
						else if(nbCylindresSurBase == 1){
							state = try_going(1050,DISTANCE_BASE_SIDE_ET_ROBOT,\
									DETERMINE_NB_CYLINDRE_SUR_BASE_REPLACEMENT,SI_1_CYLINDRES,ERROR,FAST,\
									BACKWARD,DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
						}
						else if(nbCylindresSurBase == 0){
							state = SI_0_REPLACEMENT;
						}
					}
				}
				else {
					if(mon_point_de_rotation==SUD){
						if (nbCylindresSurBase == 4){
							state = try_going(1000,3000-DISTANCE_BASE_SIDE_ET_ROBOT,\
									DETERMINE_NB_CYLINDRE_SUR_BASE_REPLACEMENT,SI_4_CYLINDRES,ERROR,FAST,\
									BACKWARD,DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
						}
						else if(nbCylindresSurBase == 3){
							state = try_going(900,3000-DISTANCE_BASE_SIDE_ET_ROBOT,\
									DETERMINE_NB_CYLINDRE_SUR_BASE_REPLACEMENT,SI_3_CYLINDRES,ERROR,FAST,\
									BACKWARD,DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
						}
						else if(nbCylindresSurBase == 2){
							state = try_going(800,3000-DISTANCE_BASE_SIDE_ET_ROBOT,\
									DETERMINE_NB_CYLINDRE_SUR_BASE_REPLACEMENT,SI_2_CYLINDRES,ERROR,FAST,\
									BACKWARD,DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
						}
						else if(nbCylindresSurBase == 1){
							state = try_going(700,3000-DISTANCE_BASE_SIDE_ET_ROBOT,\
									DETERMINE_NB_CYLINDRE_SUR_BASE_REPLACEMENT,SI_1_CYLINDRES,ERROR,FAST,\
									BACKWARD,DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
						}
						else if(nbCylindresSurBase == 0){
							state = SI_0_REPLACEMENT;
						}
					}
					else {
						if (nbCylindresSurBase == 4){
							state = try_going(750,3000-DISTANCE_BASE_SIDE_ET_ROBOT,\
									DETERMINE_NB_CYLINDRE_SUR_BASE_REPLACEMENT,SI_4_CYLINDRES,ERROR,FAST,\
									BACKWARD,DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
						}
						else if(nbCylindresSurBase == 3){
							state = try_going(850,3000-DISTANCE_BASE_SIDE_ET_ROBOT,\
									DETERMINE_NB_CYLINDRE_SUR_BASE_REPLACEMENT,SI_3_CYLINDRES,ERROR,FAST,\
									BACKWARD,DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
						}
						else if(nbCylindresSurBase == 2){
							state = try_going(950,3000-DISTANCE_BASE_SIDE_ET_ROBOT,\
									DETERMINE_NB_CYLINDRE_SUR_BASE_REPLACEMENT,SI_2_CYLINDRES,ERROR,FAST,\
									BACKWARD,DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
						}
						else if(nbCylindresSurBase == 1){
							state = try_going(1050,3000-DISTANCE_BASE_SIDE_ET_ROBOT,\
									DETERMINE_NB_CYLINDRE_SUR_BASE_REPLACEMENT,SI_1_CYLINDRES,ERROR,FAST,\
									BACKWARD,DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
						}
						else if(nbCylindresSurBase == 0){
							state = SI_0_REPLACEMENT;
						}
					}
				}

				break;



//VERIFIER_COULEUR_CYLINDRE_DEJA_SUR_BASE:
//ATTENTION : ROTATION d'un cylindre inconus == monocouleur!

			case SI_4_CYLINDRES:
				if(endroit_depose == OUEST){
					if(mon_point_de_rotation==SUD){
						//rotation cylindre Gauche
						//sub_harry_return_modules(LEFT);
						state = try_going(900,DISTANCE_BASE_SIDE_ET_ROBOT,SI_4_CYLINDRES,SI_3_CYLINDRES,\
								ERROR,FAST,BACKWARD,DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
					}
					else{
						//rotation cylindre Droit
						//sub_harry_return_modules(RIGHT);
						state = try_going(850,DISTANCE_BASE_SIDE_ET_ROBOT,SI_4_CYLINDRES,SI_3_CYLINDRES,\
								ERROR,FAST,BACKWARD,DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
					}
				}
				else{
					if(mon_point_de_rotation==SUD){
						//rotation cylindre Droit
						//sub_harry_return_modules(RIGHT);
						state = try_going(900,3000-DISTANCE_BASE_SIDE_ET_ROBOT,SI_4_CYLINDRES,SI_3_CYLINDRES,\
								ERROR,FAST,BACKWARD,DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
					}
					else{
						//rotation cylindre Gauche
						//sub_harry_return_modules(LEFT);
						state = try_going(850,3000-DISTANCE_BASE_SIDE_ET_ROBOT,SI_4_CYLINDRES,SI_3_CYLINDRES,\
								ERROR,FAST,BACKWARD,DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
					}
				}
				break;

			case SI_3_CYLINDRES:
				if(endroit_depose == OUEST){
					if(mon_point_de_rotation==SUD){
						//rotation cylindre Gauche
						//sub_harry_return_modules(LEFT);
						state = try_going(800,DISTANCE_BASE_SIDE_ET_ROBOT,SI_3_CYLINDRES,SI_2_CYLINDRES,\
								ERROR,FAST,BACKWARD,DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
					}
					else{
						//rotation cylindre Droit
						//sub_harry_return_modules(RIGHT);
						state = try_going(950,DISTANCE_BASE_SIDE_ET_ROBOT,SI_3_CYLINDRES,SI_2_CYLINDRES,\
								ERROR,FAST,BACKWARD,DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
					}
				}
				else{
					if(mon_point_de_rotation==SUD){
						//rotation cylindre Droit
						//sub_harry_return_modules(RIGHT);
						state = try_going(800,3000-DISTANCE_BASE_SIDE_ET_ROBOT,SI_3_CYLINDRES,SI_2_CYLINDRES,\
								ERROR,FAST,BACKWARD,DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
					}
					else{
						//rotation cylindre Gauche
						//sub_harry_return_modules(LEFT);
						state = try_going(950,3000-DISTANCE_BASE_SIDE_ET_ROBOT,SI_3_CYLINDRES,SI_2_CYLINDRES,\
								ERROR,FAST,BACKWARD,DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
					}
				}
				break;

			case SI_2_CYLINDRES:
				if(endroit_depose == OUEST){
					if(mon_point_de_rotation==SUD){
						//rotation cylindre Gauche
						//sub_harry_return_modules(LEFT);
						state = try_going(700,DISTANCE_BASE_SIDE_ET_ROBOT,SI_2_CYLINDRES,SI_1_CYLINDRES,\
								ERROR,FAST,BACKWARD,DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
					}
					else{
						//rotation cylindre Droit
						//sub_harry_return_modules(RIGHT);
						state = try_going(1050,DISTANCE_BASE_SIDE_ET_ROBOT,SI_2_CYLINDRES,SI_1_CYLINDRES,\
								ERROR,FAST,BACKWARD,DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
					}
				}
				else{
					if(mon_point_de_rotation==SUD){
						//rotation cylindre Droit
						//sub_harry_return_modules(RIGHT);
						state = try_going(700,3000-DISTANCE_BASE_SIDE_ET_ROBOT,SI_2_CYLINDRES,SI_1_CYLINDRES,\
								ERROR,FAST,BACKWARD,DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
					}
					else{
						//rotation cylindre Gauche
						//sub_harry_return_modules(LEFT);
						state = try_going(1050,3000-DISTANCE_BASE_SIDE_ET_ROBOT,SI_2_CYLINDRES,SI_1_CYLINDRES,\
								ERROR,FAST,BACKWARD,DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
					}
				}
				break;

			case SI_1_CYLINDRES:
				if(endroit_depose == OUEST){
					if(mon_point_de_rotation==SUD){
						//rotation cylindre Gauche
						//sub_harry_return_modules(LEFT);
					}
					else{
						//rotation cylindre Droit
						//sub_harry_return_modules(RIGHT);
					}
				}
				else{
					if(mon_point_de_rotation==SUD){
						//rotation cylindre Droit
						//sub_harry_return_modules(RIGHT);
					}
					else{
						//rotation cylindre Gauche
						//sub_harry_return_modules(LEFT);
					}
				}
				state = SI_0_REPLACEMENT;
				break;

			case SI_0_REPLACEMENT:
				if(endroit_depose == OUEST){
					if(mon_point_de_rotation==SUD){
						state = try_going(1250,DISTANCE_BASE_SIDE_ET_ROBOT,SI_0_REPLACEMENT,\
								CHOIX_DE_COTE_DEPOSE,ERROR,FAST,BACKWARD,DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
					}
					else {
						state = try_going(650,DISTANCE_BASE_SIDE_ET_ROBOT,SI_0_REPLACEMENT,\
								CHOIX_DE_COTE_DEPOSE,ERROR,FAST,BACKWARD,DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
					}
				}
				else {
					if(mon_point_de_rotation==SUD){
						state = try_going(1250,3000-DISTANCE_BASE_SIDE_ET_ROBOT,SI_0_REPLACEMENT,\
								CHOIX_DE_COTE_DEPOSE,ERROR,FAST,BACKWARD,DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
					}
					else {
						state = try_going(650,3000-DISTANCE_BASE_SIDE_ET_ROBOT,SI_0_REPLACEMENT,\
								CHOIX_DE_COTE_DEPOSE,ERROR,FAST,BACKWARD,DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
					}
				}
				break;

			case CHOIX_DE_COTE_DEPOSE:
				//je determine le futur cote de depose:
				if(STOCKS_isEmpty(MODULE_STOCK_LEFT) && STOCKS_isEmpty(MODULE_STOCK_RIGHT)){
					state = DONE;
				}
				else if(robot_side==LEFT){
					if(STOCKS_isEmpty(MODULE_STOCK_LEFT)){
						robot_side=RIGHT;
						state = DEPOSE_MANAGER;
					}
				}else{
					if(STOCKS_isEmpty(MODULE_STOCK_RIGHT)){
						robot_side=LEFT;
						state = DEPOSE_MANAGER;
					}
				}
				break;

			case DEPOSE_MANAGER:
				if(nbCylindresSurBase!=4){
					state = DEPOSE;
					nbCylindresSurBase++;
				}
				else {
					state = FIN_DEPOSE;
				}
				break;


			case DEPOSE:
				state=check_sub_action_result(sub_harry_depose_modules_side(modules,robot_side,basis_side,\
						mon_point_de_rotation,endroit_depose,cote_depose),DEPOSE,CHOIX_DE_COTE_DEPOSE,ERROR);
				break;

			case FIN_DEPOSE:
				state = check_sub_action_result(sub_harry_depose_modules_side_fin(modules,robot_side,basis_side),\
						FIN_DEPOSE,DONE,ERROR);
				break;
	}
	return IN_PROGRESS;
}

error_e sub_harry_depose_modules_side(ELEMENTS_property_e modules,ELEMENTS_side_e robot_side, ELEMENTS_side_match_e basis_side,POINT_DE_ROTATION_config_e mon_point_de_rotation,endroit_depose_config_e endroit_depose,COTE_DE_DEPOSE_config_e cote_depose){
	//====================================================================================================
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_DEPOSE_MODULES_SIDE,
				INIT,
				ERROR,
				DONE,
			//Procedure de depose: partie commune � l'ouset et l'est:
				PREPARATION_NEXT,
				TAKE_MODULE,
				DECISION,

			//Procedure de depose OUEST:

				PREPARATION_PUSHER_OUEST,
				PUSHER_DOWN_OUEST,
				PUSH_MODULES_OUEST,
				PUSHER_UP_OUEST,
				PUSH_MODULES_RETURN_TO_POSITION_OUEST,

				//DROP_PUT_AWAY_OUEST,

			//Procedure de depose EST:

				PREPARATION_PUSHER_EST,
				PUSHER_DOWN_EST,
				PUSH_MODULES_EST,
				PUSHER_UP_EST,
				PUSH_MODULES_RETURN_TO_POSITION_EST

				//DROP_PUT_AWAY_EST,
				);

//DEPOSE:
	switch(state){

	case INIT:
		state = PREPARATION_NEXT;
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

		case PREPARATION_NEXT:
			if(cote_depose == DROITE){
				state=check_sub_action_result(sub_act_harry_mae_prepare_modules_for_dispose(MODULE_STOCK_RIGHT,\
						TRUE),PREPARATION_NEXT,TAKE_MODULE,ERROR);
			}
			else{
				state=check_sub_action_result(sub_act_harry_mae_prepare_modules_for_dispose(MODULE_STOCK_LEFT,\
						TRUE),PREPARATION_NEXT,TAKE_MODULE,ERROR);
			}
			break;


		case TAKE_MODULE:
			if(cote_depose == DROITE){
				state=check_sub_action_result(sub_act_harry_mae_dispose_modules(MODULE_STOCK_RIGHT,\
						ARG_DISPOSE_ONE_CYLINDER_AND_FINISH),TAKE_MODULE,DECISION,ERROR);
			}
			else{
				state=check_sub_action_result(sub_act_harry_mae_dispose_modules(MODULE_STOCK_LEFT,\
						ARG_DISPOSE_ONE_CYLINDER_AND_FINISH),TAKE_MODULE,DECISION,ERROR);
			}
			break;

		case DECISION:
			if(basis_side==OUEST){
				state=PREPARATION_PUSHER_OUEST;
			}
			else{
				state=PREPARATION_PUSHER_EST;
			}
			break;


//DEPOSE OUEST:
		case PREPARATION_PUSHER_OUEST:
			if(mon_point_de_rotation==NORD){
				if(cote_depose==DROITE){
					state = PUSHER_DOWN_OUEST;
				}
				else{
					state = try_going(550,DISTANCE_BASE_SIDE_ET_ROBOT,state,PUSHER_DOWN_OUEST,ERROR,FAST,BACKWARD,DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
				}
			}
			else{
				if(cote_depose==DROITE){
					state = try_going(1200,DISTANCE_BASE_SIDE_ET_ROBOT,state,PUSHER_DOWN_OUEST,ERROR,FAST,BACKWARD,DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
				}
				else{
					state = PUSHER_DOWN_OUEST;
				}
			}
			break;

		case PUSHER_DOWN_OUEST:
			if(cote_depose==DROITE){
				//bras poussoir droit down
				ACT_push_order(ACT_CYLINDER_PUSHER_RIGHT,ACT_CYLINDER_PUSHER_RIGHT_OUT);
			}
			else{
				//bras poussoir gauche down
				ACT_push_order(ACT_CYLINDER_PUSHER_LEFT,ACT_CYLINDER_PUSHER_LEFT_OUT);
			}
			break;
		case PUSH_MODULES_OUEST:
			if(nbCylindresSurBase!=4){
				if(mon_point_de_rotation==NORD){
					state = try_going(850,DISTANCE_BASE_SIDE_ET_ROBOT,state,PUSHER_UP_OUEST,ERROR,FAST,BACKWARD,DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
				}
				else{
					state = try_going(1000,DISTANCE_BASE_SIDE_ET_ROBOT,state,PUSHER_UP_OUEST,ERROR,FAST,BACKWARD,DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
				}
			}
			else{
				state = PUSHER_UP_OUEST;
			}
			break;

		case PUSHER_UP_OUEST:
			if(cote_depose==DROITE){
				//bras poussoir droit up
				ACT_push_order(ACT_CYLINDER_PUSHER_RIGHT,ACT_CYLINDER_PUSHER_RIGHT_IN);
			}
			else{
				//bras poussoir gauche up
				ACT_push_order(ACT_CYLINDER_PUSHER_LEFT,ACT_CYLINDER_PUSHER_LEFT_IN);
			}
			break;

		case PUSH_MODULES_RETURN_TO_POSITION_OUEST:
			if(mon_point_de_rotation==SUD){
				state = try_going(1250,DISTANCE_BASE_SIDE_ET_ROBOT,state,DONE,ERROR,FAST,BACKWARD,DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
			}
			else {
				state = try_going(650,DISTANCE_BASE_SIDE_ET_ROBOT,state,DONE,ERROR,FAST,BACKWARD,DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
			}
			break;



//DEPOSE EST:
		case PREPARATION_PUSHER_EST:
			if(mon_point_de_rotation==NORD){
				if(cote_depose==DROITE){
					state = PUSHER_DOWN_EST;
				}
				else{
					state = try_going(550,DISTANCE_BASE_SIDE_ET_ROBOT,state,PUSHER_DOWN_EST,ERROR,FAST,BACKWARD,DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
				}
			}
			else{
				if(cote_depose==DROITE){
					state = try_going(1200,DISTANCE_BASE_SIDE_ET_ROBOT,state,PUSHER_DOWN_EST,ERROR,FAST,BACKWARD,DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
				}
				else{
					state = PUSHER_DOWN_EST;
				}
			}
			break;

		case PUSHER_DOWN_EST:
			if(cote_depose==DROITE){
				//bras poussoir droit down
				ACT_push_order(ACT_CYLINDER_PUSHER_RIGHT,ACT_CYLINDER_PUSHER_RIGHT_OUT);
			}
			else{
				//bras poussoir gauche down
				ACT_push_order(ACT_CYLINDER_PUSHER_LEFT,ACT_CYLINDER_PUSHER_LEFT_OUT);
			}
			break;

		case PUSH_MODULES_EST:
			if(nbCylindresSurBase!=4){
				if(mon_point_de_rotation==NORD){
					state = try_going(850,DISTANCE_BASE_SIDE_ET_ROBOT,state,PUSHER_UP_EST,ERROR,FAST,BACKWARD,DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
				}
				else{
					state = try_going(1000,DISTANCE_BASE_SIDE_ET_ROBOT,state,PUSHER_UP_EST,ERROR,FAST,BACKWARD,DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
				}
			}
			else{
				state = PUSHER_UP_EST;
			}
			break;

		case PUSHER_UP_EST:
			if(cote_depose==DROITE){
				//bras poussoir droit up
				ACT_push_order(ACT_CYLINDER_PUSHER_RIGHT,ACT_CYLINDER_PUSHER_RIGHT_IN);
			}
			else{
				//bras poussoir gauche up
				ACT_push_order(ACT_CYLINDER_PUSHER_LEFT,ACT_CYLINDER_PUSHER_LEFT_IN);
			}
			break;

		case PUSH_MODULES_RETURN_TO_POSITION_EST:
			if(mon_point_de_rotation==SUD){
				state = try_going(1250,DISTANCE_BASE_SIDE_ET_ROBOT,state,DONE,ERROR,FAST,BACKWARD,DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
			}
			else {
				state = try_going(650,DISTANCE_BASE_SIDE_ET_ROBOT,state,DONE,ERROR,FAST,BACKWARD,DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
			}
			break;
	}
	return IN_PROGRESS;
}

error_e sub_harry_depose_modules_side_fin(ELEMENTS_property_e modules,ELEMENTS_side_e robot_side, ELEMENTS_side_match_e basis_side){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_DEPOSE_MODULES_SIDE_FIN,
		//Proc�dure de fin de sub!!!!
			//VERIFICATION_ACTIONNEUR,
			INIT,
			ERROR,
			DONE,

			FIN_TENTATIVE_SUD,
			FIN_TENTATIVE_NORD,
			POINT_DE_ROTATION1,
			POINT_DE_ROTATION2,
			POINT_DE_ROTATION3
			);

//PROCEDURE DE FIN:
	switch(state){

	case INIT:
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


	case FIN_TENTATIVE_SUD:
		if(endroit_depose == OUEST){
			state = try_going(1250,DISTANCE_BASE_SIDE_ET_ROBOT,state,POINT_DE_ROTATION1,FIN_TENTATIVE_NORD,FAST,ANY_WAY,NO_DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
		}
		else {
			state = try_going(1250,3000-DISTANCE_BASE_SIDE_ET_ROBOT,state,POINT_DE_ROTATION1,FIN_TENTATIVE_NORD,FAST,ANY_WAY,NO_DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
		}
		break;

	case FIN_TENTATIVE_NORD:
		if(endroit_depose == OUEST){
			state = try_going(650,DISTANCE_BASE_SIDE_ET_ROBOT,state,POINT_DE_ROTATION2,FIN_TENTATIVE_SUD,FAST,ANY_WAY,NO_DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
		}
		else {
			state = try_going(650,3000-DISTANCE_BASE_SIDE_ET_ROBOT,state,POINT_DE_ROTATION2,FIN_TENTATIVE_SUD,FAST,ANY_WAY,NO_DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
		}
		break;

	case POINT_DE_ROTATION1:
		state = try_going(1200,COLOR_Y(380),state,DONE,POINT_DE_ROTATION3,FAST,ANY_WAY,NO_DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
		break;

	case POINT_DE_ROTATION2:
		state = try_going(1000,COLOR_Y(380),state,DONE,POINT_DE_ROTATION3,FAST,ANY_WAY,NO_DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
		break;

	case POINT_DE_ROTATION3:
		state = try_going(800,COLOR_Y(380),state,DONE,POINT_DE_ROTATION1,FAST,ANY_WAY,NO_DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
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
				state = ERROR; // Actionneur d�sactiv�, on a rien pu prendre
			}else if(STOCKS_isEmpty(MODULE_STOCK_LEFT) && STOCKS_isEmpty(MODULE_STOCK_RIGHT)){
				state = ERROR; // Il n'y a rien � d�poser
			}else{
#ifdef SCAN_ELEMENTS
			//scan les bases de constructions pour compter et �valuer le remplissage
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
#warning 'CORENTIN : Que fait t on sinon ? On reste plant� l� ?'
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
			state=check_sub_action_result(sub_harry_depose_modules_side_intro(OUR_ELEMENT,RIGHT, OUR_SIDE),state,DONE,ERROR);
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
			state=check_sub_action_result(sub_harry_depose_modules_side_intro(ADV_ELEMENT,RIGHT, OUR_SIDE),state,DONE,ERROR);
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
				if((global.color==BLUE)&&(CW_is_color_detected(CW_SENSOR_LEFT, CW_Channel_Yellow, FALSE))){ //jaune � gauche
					state=WAIT_WHITE;
				}else if((global.color==YELLOW)&&(CW_is_color_detected(CW_SENSOR_LEFT, CW_Channel_Blue, FALSE))){ //bleu � gauche
					state=WAIT_WHITE;
				}
			}else{
				if((global.color==BLUE)&&(CW_is_color_detected(CW_SENSOR_RIGHT, CW_Channel_Yellow, FALSE))){ //jaune � droite
					state=WAIT_WHITE;
				}else if((global.color==YELLOW)&&(CW_is_color_detected(CW_SENSOR_RIGHT, CW_Channel_Blue, FALSE))){ //bleu � droite
					state=WAIT_WHITE;
				}
			}
			break;

		case WAIT_WHITE:
			if(side==LEFT){
				if(CW_is_color_detected(CW_SENSOR_LEFT, CW_Channel_White, FALSE)){ //blanc � gauche
					state=STOP;
				}
			}else{
				if(CW_is_color_detected(CW_SENSOR_RIGHT, CW_Channel_White, FALSE)){ //blanc � droite
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
					//ACT_push_order_with(ACT_MOTOR_TURN_LEFT,�teins-toi!!!);
				}else{
					//ACT_push_order_with(ACT_MOTOR_TURN_RIGHT,�teins-toi!!!);
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


// la place des modules est compt�e en partant du centre "Atos"
/*error_e manager_return_modules(){ => Fonction � ne pas utiliser
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

// ATTENTION : Il n y a pas de GET_in, comment fait on pour se rendre au point demand� ?
			//module au centre
			if(MOONBASES_getModuleType(1, MODULE_MOONBASE_MIDDLE) == MODULE_POLY){ 				//apelle de fonction pour aller � cot� du module
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
			if(MOONBASES_getModuleType(6, MODULE_MOONBASE_MIDDLE) == MODULE_POLY){ // probl�me de position
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
