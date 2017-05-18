#include "action_small.h"
#include "../actions_both_generic.h"
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
#include "../../propulsion/pathfind.h"
#include "../../propulsion/prop_functions.h"

error_e sub_anne_manager_return_modules(ELEMENTS_property_e modules){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_ANNE_MANAGER_RETURN_MODULES,
			INIT,
			COMPUTE,
			MANAGE,
			GO_TO_MIDDLE,
			GO_TO_ADV_CENTER,
			GO_TO_ADV_SIDE,
			GO_TO_OUR_CENTER,
			GO_TO_OUR_SIDE,

			ERROR,
			DONE
			);



	switch(state){
		case INIT:
			if(IHM_switchs_get(SWITCH_ANNE_DISABLE_SMALL_MAGIC_ARM)){
				state = ERROR; // Actionneur désactivé, on a rien pu prendre
			}else{
		#ifdef SCAN_ELEMENTS
					//scan les bases de constructions pour compter et évaluer le remplissage
		#else
				state=COMPUTE;
		#endif
			}
			break;

		case COMPUTE:
#warning 'pour définir les priorités des bases c\'est ici'
			if(ELEMENTS_get_flag(FLAG_RETURN_CENTRAL_MOONBASIS)){
				state = GO_TO_MIDDLE;
			}else if(ELEMENTS_get_flag(FLAG_RETURN_ADV_CENTRAL_MOONBASIS)){
				state = GO_TO_ADV_CENTER;
			}else if(ELEMENTS_get_flag(FLAG_RETURN_ADV_SIDE_MOONBASIS)){
				state = GO_TO_ADV_SIDE;
			}else if(ELEMENTS_get_flag(FLAG_RETURN_OUR_CENTRAL_MOONBASIS)){
				state = GO_TO_OUR_CENTER;
			}else if(ELEMENTS_get_flag(FLAG_RETURN_OUR_SIDE_MOONBASIS)){
				state = GO_TO_OUR_SIDE;
			}else{
				state=ERROR;
			}
			break;


		case GO_TO_MIDDLE:
			state=check_sub_action_result(sub_anne_return_modules_centre(NEUTRAL_ELEMENT),state,DONE,ERROR);
			break;

		case GO_TO_OUR_CENTER:
			state=check_sub_action_result(sub_anne_return_modules_centre(OUR_ELEMENT),state,DONE,ERROR);
			break;

		case GO_TO_OUR_SIDE:
			//state=check_sub_action_result(sub_anne_return_modules_centre(OUR_ELEMENT),state,DONE,ERROR);
			state = DONE;
#warning 'faire la sub de retournement sur les bases latérales'
			break;

		case GO_TO_ADV_CENTER:
			state=check_sub_action_result(sub_anne_return_modules_centre(ADV_ELEMENT),state,DONE,ERROR);
			break;

		case GO_TO_ADV_SIDE:
			//state=check_sub_action_result(sub_anne_return_modules_centre(OUR_ELEMENT),state,DONE,ERROR);
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
				debug_printf("default case in sub_anne_manager_return_module\n");
			break;
	}

	return IN_PROGRESS;
}

error_e sub_anne_return_modules_centre(ELEMENTS_property_e modules){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_ANNE_RETURN_MODULES_CENTRE,
			INIT,
			GET_IN,
			COMPUTE,

			RUSH_BLUE,
			CLOSE_PUSHER_BLUE,
			AVANCE_BLUE,
			ENABLE_MAGIC_BLUE,
			COMPUTE_LITTLE_MOVE_BACK_BLUE,
			LITTLE_MOVE_BACK_BLUE,
			LITTLE_MOVE_BACK_ERROR_BLUE,
			MOVE_BACK_BLUE,

			RUSH_CENTRAL,
			CLOSE_PUSHER_CENTRAL,
			AVANCE_CENTRAL,
			ENABLE_MAGIC_CENTRAL,
			COMPUTE_LITTLE_MOVE_BACK_CENTRAL,
			LITTLE_MOVE_BACK_CENTRAL,
			LITTLE_MOVE_BACK_ERROR_CENTRAL,
			MOVE_BACK_CENTRAL,

			UP_BEARING_BALL,
			RUSH_YELLOW,
			CLOSE_PUSHER_YELLOW,
			AVANCE_YELLOW,
			ENABLE_MAGIC_YELLOW,
			COMPUTE_LITTLE_MOVE_BACK_YELLOW,
			LITTLE_MOVE_BACK_YELLOW,
			LITTLE_MOVE_BACK_ERROR_YELLOW,
			MOVE_BACK_YELLOW,

			ERROR,
			DONE
		);

#define TIMEOUT_COLOR	(4000)  // Temps au dela duquel on arrête de tourner le module, on a échoué a détecté la couleur
	static time32_t time_timeout = 0;
	Uint8 pos_get_in;
	Uint8 passage;

	static Uint8 nb_cylinder_in_basis = 0;
	static Sint16 tryx, tryy;

	switch(state){
		case INIT:
			state = GET_IN;
			break;

		case GET_IN:
			state = check_sub_action_result(sub_anne_return_modules_centre_get_in(modules), state, COMPUTE, ERROR);
			break;

		case COMPUTE:
			if(((modules == OUR_ELEMENT)&&(global.color == BLUE)) || ((modules == ADV_ELEMENT)&&(global.color == YELLOW))){
				state = RUSH_BLUE;
			}else if(modules == NEUTRAL_ELEMENT){
				state = RUSH_CENTRAL;
			}else if(((modules == ADV_ELEMENT)&&(global.color == BLUE)) || ((modules == OUR_ELEMENT)&&(global.color == YELLOW))){
				state = UP_BEARING_BALL;
			}else
				state = ERROR;
			break;

		case RUSH_BLUE:
			if(entrance){
				ACT_push_order(ACT_SMALL_CYLINDER_MULTIFONCTION, ACT_SMALL_CYLINDER_MULTIFONCTION_PUSH);
			}
			state = try_rush(1450, 1250, state, CLOSE_PUSHER_BLUE, MOVE_BACK_BLUE, FORWARD, NO_DODGE_AND_WAIT, TRUE);
			if(ON_LEAVE()){
				nb_cylinder_in_basis = (1800 - global.pos.x)/70;
			}
			break;

		case CLOSE_PUSHER_BLUE:
			if(entrance){
				ACT_push_order(ACT_SMALL_CYLINDER_MULTIFONCTION, ACT_SMALL_CYLINDER_MULTIFONCTION_IN);
			}
			state = check_act_status(ACT_QUEUE_Small_cylinder_multifonction, state, AVANCE_BLUE, MOVE_BACK_BLUE);
			break;

		case AVANCE_BLUE:
			state = try_going(1450, 1250, state, ENABLE_MAGIC_BLUE, MOVE_BACK_BLUE, FAST, FORWARD, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
#warning 'il faut mettre autre chose que move back pour retourner ce qui est déjà accessible'
			break;

		case ENABLE_MAGIC_BLUE:
			state = check_sub_action_result(sub_act_anne_return_module(), state, COMPUTE_LITTLE_MOVE_BACK_BLUE, MOVE_BACK_BLUE);
#warning 'quelles sont les raisons possibles d\'une erreur ?'
			break;

		case COMPUTE_LITTLE_MOVE_BACK_BLUE:
			if(global.pos.y>(1800+nb_cylinder_in_basis*70))
				state = LITTLE_MOVE_BACK_BLUE;
			else
				state = MOVE_BACK_BLUE;
			break;

		case LITTLE_MOVE_BACK_BLUE:
			state = try_advance(NULL, entrance, 100, state, ENABLE_MAGIC_BLUE, LITTLE_MOVE_BACK_ERROR_BLUE, FAST, BACKWARD, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

		case LITTLE_MOVE_BACK_ERROR_BLUE:{
			if(entrance){
				Uint8 nb_module_returned = 0;
				nb_module_returned = (1800 - global.pos.x)/70;
				tryx = 1800 - 70*nb_module_returned;
				tryy = 1400 - 70*nb_module_returned;
			}
			state = try_going(tryx, tryy, state, LITTLE_MOVE_BACK_BLUE, LITTLE_MOVE_BACK_BLUE, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
		}
		break;

		case MOVE_BACK_BLUE:
			state = try_going(1000, 1300, state, DONE, ERROR, FAST, BACKWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case RUSH_CENTRAL:
			if(entrance){
				ACT_push_order(ACT_SMALL_CYLINDER_MULTIFONCTION, ACT_SMALL_CYLINDER_MULTIFONCTION_PUSH);
			}
			state = try_rush(1650, 1700, state, CLOSE_PUSHER_CENTRAL, MOVE_BACK_CENTRAL, FORWARD, NO_DODGE_AND_WAIT, TRUE);
			if(ON_LEAVE()){
				nb_cylinder_in_basis = (1800 - global.pos.x)/100;
			}
			break;

		case CLOSE_PUSHER_CENTRAL:
			if(entrance){
				ACT_push_order(ACT_SMALL_CYLINDER_MULTIFONCTION, ACT_SMALL_CYLINDER_MULTIFONCTION_IN);
			}
			state = check_act_status(ACT_QUEUE_Small_cylinder_multifonction, state, AVANCE_CENTRAL, MOVE_BACK_CENTRAL);
			break;

		case AVANCE_CENTRAL:
			state = try_going(1650, 1700, state, ENABLE_MAGIC_CENTRAL, MOVE_BACK_CENTRAL, FAST, FORWARD, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
#warning 'il faut mettre autre chose que move back pour retourner ce qui est déjà accessible'
			break;

		case ENABLE_MAGIC_CENTRAL:
			state = check_sub_action_result(sub_act_anne_return_module(), state, COMPUTE_LITTLE_MOVE_BACK_CENTRAL, MOVE_BACK_CENTRAL);
#warning 'quelles sont les raisons possibles d\'une erreur ?'
			break;

		case COMPUTE_LITTLE_MOVE_BACK_CENTRAL:
			if(global.pos.y>(1800+nb_cylinder_in_basis*100))
				state = LITTLE_MOVE_BACK_CENTRAL;
			else
				state = MOVE_BACK_CENTRAL;
			break;

		case LITTLE_MOVE_BACK_CENTRAL:
			state = try_advance(NULL, entrance, 100, state, ENABLE_MAGIC_CENTRAL, LITTLE_MOVE_BACK_ERROR_CENTRAL, FAST, BACKWARD, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

		case LITTLE_MOVE_BACK_ERROR_CENTRAL:{
			if(entrance){
				Uint8 nb_module_returned = 0;
				nb_module_returned = (1800 - global.pos.x)/100;
				tryx = 1800 - 100*nb_module_returned;
			}
			state = try_going(tryx, 1700, state, LITTLE_MOVE_BACK_CENTRAL, LITTLE_MOVE_BACK_CENTRAL, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
		}
		break;

		case MOVE_BACK_CENTRAL:
			state = try_going(1000, 1700, state, DONE, ERROR, FAST, BACKWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

			state = ERROR;
			break;

		case UP_BEARING_BALL:{
			Uint8 state1, state2, state3;
			if(entrance){
				ACT_push_order(ACT_SMALL_BALL_BACK, ACT_SMALL_BALL_BACK_UP);
				ACT_push_order(ACT_SMALL_BALL_FRONT_LEFT, ACT_SMALL_BALL_FRONT_LEFT_UP);
				ACT_push_order(ACT_SMALL_BALL_FRONT_RIGHT, ACT_SMALL_BALL_FRONT_RIGHT_UP);
			}
			state1 = check_act_status(ACT_QUEUE_Small_bearing_back, state, RUSH_YELLOW, ERROR);
			state2 = check_act_status(ACT_QUEUE_Small_bearing_front_right, state, RUSH_YELLOW, ERROR);
			state3 = check_act_status(ACT_QUEUE_Small_bearing_front_left, state, RUSH_YELLOW, ERROR);

			if((state1 == RUSH_YELLOW) && (state2 == RUSH_YELLOW) && (state3 == RUSH_YELLOW)){
				state = RUSH_YELLOW;
			}else if((state1 == ERROR) && (state1 == ERROR) && (state1 == ERROR)){
				state = ERROR;
			}else{
				state = UP_BEARING_BALL;
			}
		}
			break;

		case RUSH_YELLOW:
			if(entrance){
				ACT_push_order(ACT_SMALL_CYLINDER_MULTIFONCTION, ACT_SMALL_CYLINDER_MULTIFONCTION_PUSH);
			}
			state = try_rush(1450, 2150, state, CLOSE_PUSHER_YELLOW, MOVE_BACK_YELLOW, FORWARD, NO_DODGE_AND_WAIT, TRUE);
			if(ON_LEAVE()){
				nb_cylinder_in_basis = (1800 - global.pos.x)/70;
			}
			break;

		case CLOSE_PUSHER_YELLOW:
			if(entrance){
				ACT_push_order(ACT_SMALL_CYLINDER_MULTIFONCTION, ACT_SMALL_CYLINDER_MULTIFONCTION_IN);
			}
			state = check_act_status(ACT_QUEUE_Small_cylinder_multifonction, state, AVANCE_YELLOW, MOVE_BACK_YELLOW);
			break;

		case AVANCE_YELLOW:
			state = try_going(1450, 2150, state, ENABLE_MAGIC_YELLOW, MOVE_BACK_YELLOW, FAST, FORWARD, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
#warning 'il faut mettre autre chose que move back pour retourner ce qui est déjà accessible'
			break;

		case ENABLE_MAGIC_YELLOW:
			state = check_sub_action_result(sub_act_anne_return_module(), state, COMPUTE_LITTLE_MOVE_BACK_YELLOW, MOVE_BACK_YELLOW);
#warning 'quelles sont les raisons possibles d\'une erreur ?'
			break;

		case COMPUTE_LITTLE_MOVE_BACK_YELLOW:
			if(global.pos.y>(1800+nb_cylinder_in_basis*70))
				state = LITTLE_MOVE_BACK_YELLOW;
			else
				state = MOVE_BACK_YELLOW;
			break;

		case LITTLE_MOVE_BACK_YELLOW:
			state = try_advance(NULL, entrance, 100, state, ENABLE_MAGIC_YELLOW, LITTLE_MOVE_BACK_ERROR_YELLOW, FAST, BACKWARD, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

		case LITTLE_MOVE_BACK_ERROR_YELLOW:{
			if(entrance){
				Uint8 nb_module_returned = 0;
				nb_module_returned = (1800 - global.pos.x)/70;
				tryx = 1800 - 70*nb_module_returned;
				tryy = 2000 - 70*nb_module_returned;
			}
			state = try_going(tryx, tryy, state, LITTLE_MOVE_BACK_YELLOW, LITTLE_MOVE_BACK_YELLOW, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
		}
		break;

		case MOVE_BACK_YELLOW:
			state = try_going(1000, 2200, state, DONE, ERROR, FAST, BACKWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
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
				debug_printf("default case in sub_anne_return_modules_centre\n");
			break;
	}

	return IN_PROGRESS;
}

error_e sub_anne_return_modules_centre_get_in(ELEMENTS_property_e modules){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_ANNE_RETURN_MODULES_CENTRE_GET_IN,
			INIT,
			GET_IN_BLUE,
			GET_IN_CENTRAL,
			GET_IN_YELLOW,

			GET_IN_POS_BLUE,
			GET_IN_POS_BLUE_FROM_YELLOW,
			GET_IN_POS_BLUE_FROM_CENTRAL,
			ASTAR_BLUE,

			GET_IN_POS_CENTRAL,
			GET_IN_POS_CENTRAL_FROM_BLUE,
			GET_IN_POS_CENTRAL_FROM_YELLOW,
			ASTAR_CENTRAL,

			GET_IN_POS_YELLOW,
			GET_IN_POS_YELLOW_FROM_BLUE,
			GET_IN_POS_YELLOW_FROM_CENTRAL,
			ASTAR_YELLOW,

			ERROR,
			DONE
		);
//color Y
	switch(state){
		case INIT:
			if(((modules == OUR_ELEMENT)&&(global.color == BLUE)) || ((modules == ADV_ELEMENT)&&(global.color == YELLOW))){
				state = GET_IN_BLUE;
			}else if(modules == NEUTRAL_ELEMENT){
				state = GET_IN_CENTRAL;
			}else if(((modules == ADV_ELEMENT)&&(global.color == BLUE)) || ((modules == OUR_ELEMENT)&&(global.color == YELLOW))){
				state = GET_IN_YELLOW;
			}else
				state = ERROR;
			break;

		case GET_IN_BLUE:
			if(i_am_in_square(800, 1400, 300, 900)){
				state = GET_IN_POS_BLUE;
			}else if(i_am_in_square_color(800, 1400, 2700, 2100)){
				state = GET_IN_POS_BLUE_FROM_YELLOW;
			}else if(i_am_in_square_color(200, 1100, 900, 2100)){
				state = GET_IN_POS_BLUE_FROM_CENTRAL;
			}else{
				state = ASTAR_BLUE;
			}
			break;

		case GET_IN_POS_BLUE:
			state = try_going(1100, 925, state, DONE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

		case GET_IN_POS_BLUE_FROM_CENTRAL:
			state = try_going(1000, 1000, state, GET_IN_POS_BLUE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_BRAKE);
			break;

		case GET_IN_POS_BLUE_FROM_YELLOW:
			state = try_going(900, 2000, state, GET_IN_POS_BLUE_FROM_YELLOW, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT,END_AT_BRAKE);
			break;

		case ASTAR_BLUE:
			state = ASTAR_try_going(1100, 925, state, DONE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

		case GET_IN_CENTRAL:
			if(i_am_in_square(800, 1400, 300, 900)){
				state = GET_IN_POS_CENTRAL_FROM_BLUE;
			}else if(i_am_in_square_color(800, 1400, 2700, 2100)){
				state = GET_IN_POS_CENTRAL_FROM_YELLOW;
			}else if(i_am_in_square_color(200, 1100, 900, 2100)){
				state = GET_IN_POS_CENTRAL;
			}else{
				state = ASTAR_CENTRAL;
			}
			break;

		case GET_IN_POS_CENTRAL:
			state = try_going(1000, 1750, state, DONE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT,END_AT_BRAKE);
			break;

		case GET_IN_POS_CENTRAL_FROM_BLUE:
			state = try_going(975, 1000, state, GET_IN_POS_CENTRAL, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT,END_AT_BRAKE);
			break;

		case GET_IN_POS_CENTRAL_FROM_YELLOW:
			state = try_going(900, 1750, state, GET_IN_POS_CENTRAL, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT,END_AT_BRAKE);
			break;

		case ASTAR_CENTRAL:
			state = ASTAR_try_going(1000, 1750, state, DONE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT,END_AT_BRAKE);
			break;

		case GET_IN_YELLOW:
			if(i_am_in_square(800, 1400, 300, 900)){
				state = GET_IN_POS_YELLOW_FROM_BLUE;
			}else if(i_am_in_square_color(800, 1400, 2700, 2100)){
				state = GET_IN_POS_YELLOW;
			}else if(i_am_in_square_color(200, 1100, 900, 2100)){
				state = GET_IN_POS_YELLOW_FROM_CENTRAL;
			}else{
				state = ASTAR_YELLOW;
			}
			break;

		case GET_IN_POS_YELLOW:
			state = try_going(1450, 2400, state, DONE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT,END_AT_BRAKE);
			break;

		case GET_IN_POS_YELLOW_FROM_CENTRAL:
			state = try_going(1000, 2000, state, GET_IN_POS_YELLOW, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT,END_AT_BRAKE);
			break;

		case GET_IN_POS_YELLOW_FROM_BLUE:
			state = try_going(900, 1000, state, GET_IN_POS_YELLOW_FROM_CENTRAL, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT,END_AT_BRAKE);
			break;

		case ASTAR_YELLOW:
			state = ASTAR_try_going(1450, 2400, state, DONE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_BRAKE);
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
				debug_printf("default case in sub_anne_return_modules_centre_get_in\n");
			break;
	}

	return IN_PROGRESS;
}



error_e sub_anne_return_modules_side(ELEMENTS_property_e modules){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_ANNE_RETURN_MODULES_SIDE,
			INIT,
			GET_IN,
			COMPUTE,

			GO_TO_BLUE,
			OPEN_MULTIFONCTION_BLUE,
			RUSH_BLUE,
			CLOSE_PUSHER_BLUE,
			AVANCE_BLUE,
			ENABLE_MAGIC_BLUE,
			COMPUTE_LITTLE_MOVE_BACK_BLUE,
			LITTLE_MOVE_BACK_BLUE,
			LITTLE_MOVE_BACK_ERROR_BLUE,
			MOVE_BACK_BLUE,
			GET_OUT_BLUE,

			GO_TO_YELLOW,
			OPEN_MULTIFONCTION_YELLOW,
			RUSH_YELLOW,
			CLOSE_PUSHER_YELLOW,
			AVANCE_YELLOW,
			ENABLE_MAGIC_YELLOW,
			COMPUTE_LITTLE_MOVE_BACK_YELLOW,
			LITTLE_MOVE_BACK_YELLOW,
			LITTLE_MOVE_BACK_ERROR_YELLOW,
			MOVE_BACK_YELLOW,
			GET_OUT_YELLOW,

			ERROR,
			DONE
		);

#define TIMEOUT_COLOR	(4000)  // Temps au dela duquel on arrête de tourner le module, on a échoué a détecté la couleur
	static time32_t time_timeout = 0;
	Uint8 pos_get_in;
	Uint8 passage;

	static Uint8 nb_cylinder_in_basis = 0;
	static Sint16 tryx, tryy;

	switch(state){
		case INIT:
			state = GET_IN;
			break;

		case GET_IN:
			state = check_sub_action_result(sub_anne_return_modules_side_get_in(modules), state, COMPUTE, ERROR);
			break;

		case COMPUTE:
			if(((modules == OUR_ELEMENT)&&(global.color == BLUE)) || ((modules == ADV_ELEMENT)&&(global.color == YELLOW))){
				state = GO_TO_BLUE;
			}else if(((modules == ADV_ELEMENT)&&(global.color == BLUE)) || ((modules == OUR_ELEMENT)&&(global.color == YELLOW))){
				state = GO_TO_YELLOW;
			}else
				state = ERROR;
			break;

		case GO_TO_BLUE:
			state = try_going(750, 300, state, OPEN_MULTIFONCTION_BLUE, GET_OUT_BLUE, FAST, FORWARD, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

		case OPEN_MULTIFONCTION_BLUE:
			if(entrance){
				ACT_push_order(ACT_SMALL_CYLINDER_MULTIFONCTION, ACT_SMALL_CYLINDER_MULTIFONCTION_PUSH);
			}
			state = check_act_status(ACT_QUEUE_Small_cylinder_multifonction, state, RUSH_BLUE, RUSH_BLUE);
#warning 'je vois pas quoi mettre en cas d\'erreur'
			break;

		case RUSH_BLUE:
			/*if(entrance){
				ACT_push_order(ACT_SMALL_CYLINDER_MULTIFONCTION, ACT_SMALL_CYLINDER_MULTIFONCTION_PUSH);
			}*/
			state = try_rush(1450, 300, state, CLOSE_PUSHER_BLUE, MOVE_BACK_BLUE, BACKWARD, NO_DODGE_AND_WAIT, TRUE);
			if(ON_LEAVE()){
				nb_cylinder_in_basis = (1200 - global.pos.x)/100;
			}
			break;

		case CLOSE_PUSHER_BLUE:
			if(entrance){
				ACT_push_order(ACT_SMALL_CYLINDER_MULTIFONCTION, ACT_SMALL_CYLINDER_MULTIFONCTION_IN);
			}
			state = check_act_status(ACT_QUEUE_Small_cylinder_multifonction, state, AVANCE_BLUE, MOVE_BACK_BLUE);
			break;

		case AVANCE_BLUE:
			state = try_going(1450, 300, state, ENABLE_MAGIC_BLUE, MOVE_BACK_BLUE, FAST, BACKWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
#warning 'il faut mettre autre chose que move back pour retourner ce qui est déjà accessible'
			break;

		case ENABLE_MAGIC_BLUE:
			state = check_sub_action_result(sub_act_anne_return_module(), state, COMPUTE_LITTLE_MOVE_BACK_BLUE, MOVE_BACK_BLUE);
#warning 'quelles sont les raisons possibles d\'une erreur ?'
			break;

		case COMPUTE_LITTLE_MOVE_BACK_BLUE:
			if(global.pos.y>(1800 - nb_cylinder_in_basis*100))
				state = LITTLE_MOVE_BACK_BLUE;
			else
				state = MOVE_BACK_BLUE;
			break;

		case LITTLE_MOVE_BACK_BLUE:
			state = try_advance(NULL, entrance, 100, state, ENABLE_MAGIC_BLUE, LITTLE_MOVE_BACK_ERROR_BLUE, FAST, BACKWARD, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

		case LITTLE_MOVE_BACK_ERROR_BLUE:{
			if(entrance){
				Uint8 nb_module_returned = 0;
				nb_module_returned = (1800 - global.pos.x)/70;
				tryy = 1200 - 100*nb_module_returned;
			}
			state = try_going(tryx, 300, state, LITTLE_MOVE_BACK_BLUE, LITTLE_MOVE_BACK_BLUE, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
		}
		break;

		case MOVE_BACK_BLUE:
			state = try_going(700, 300, state, DONE, ERROR, FAST, BACKWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

#warning 'OUPS! j\'ai très mal négocié les get out, faut revoir la fin'

		case GO_TO_YELLOW:
			state = try_going(750, 2700, state, OPEN_MULTIFONCTION_YELLOW, GET_OUT_YELLOW, FAST, BACKWARD, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

		case OPEN_MULTIFONCTION_YELLOW:
			if(entrance){
				ACT_push_order(ACT_SMALL_CYLINDER_MULTIFONCTION, ACT_SMALL_CYLINDER_MULTIFONCTION_PUSH);
			}
			state = check_act_status(ACT_QUEUE_Small_cylinder_multifonction, state, RUSH_BLUE, RUSH_BLUE);
#warning 'je vois pas quoi mettre en cas d\'erreur'
			break;

		case RUSH_YELLOW:
/*			if(entrance){
				ACT_push_order(ACT_SMALL_CYLINDER_MULTIFONCTION, ACT_SMALL_CYLINDER_MULTIFONCTION_PUSH);
			}*/
			state = try_rush(1100, 2700, state, CLOSE_PUSHER_YELLOW, MOVE_BACK_YELLOW, FORWARD, NO_DODGE_AND_WAIT, TRUE);
			if(ON_LEAVE()){
				nb_cylinder_in_basis = (1200 - global.pos.x)/100;
			}
			break;

		case CLOSE_PUSHER_YELLOW:
			if(entrance){
				ACT_push_order(ACT_SMALL_CYLINDER_MULTIFONCTION, ACT_SMALL_CYLINDER_MULTIFONCTION_IN);
			}
			state = check_act_status(ACT_QUEUE_Small_cylinder_multifonction, state, AVANCE_YELLOW, MOVE_BACK_YELLOW);
			break;

		case AVANCE_YELLOW:
			state = try_going(1200, 2700, state, ENABLE_MAGIC_YELLOW, MOVE_BACK_YELLOW, FAST, FORWARD, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
#warning 'il faut mettre autre chose que move back pour retourner ce qui est déjà accessible'
			break;

		case ENABLE_MAGIC_YELLOW:
			state = check_sub_action_result(sub_act_anne_return_module(), state, COMPUTE_LITTLE_MOVE_BACK_YELLOW, MOVE_BACK_YELLOW);
#warning 'quelles sont les raisons possibles d\'une erreur ?'
			break;

		case COMPUTE_LITTLE_MOVE_BACK_YELLOW:
			if(global.pos.y>(1200+nb_cylinder_in_basis*100))
				state = LITTLE_MOVE_BACK_YELLOW;
			else
				state = MOVE_BACK_YELLOW;
			break;

		case LITTLE_MOVE_BACK_YELLOW:
			state = try_advance(NULL, entrance, 100, state, ENABLE_MAGIC_YELLOW, LITTLE_MOVE_BACK_ERROR_YELLOW, FAST, BACKWARD, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

		case LITTLE_MOVE_BACK_ERROR_YELLOW:{
			if(entrance){
				Uint8 nb_module_returned = 0;
				nb_module_returned = (1800 - global.pos.x)/70;
				tryx = 1800 - 70*nb_module_returned;
			}
			state = try_going(tryx, 2700, state, LITTLE_MOVE_BACK_YELLOW, LITTLE_MOVE_BACK_YELLOW, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
		}
		break;

		case MOVE_BACK_YELLOW:
			state = try_going(1000, 2700, state, DONE, ERROR, FAST, BACKWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
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
				debug_printf("default case in sub_anne_return_modules_centre\n");
			break;
	}

	return IN_PROGRESS;
}


error_e sub_anne_return_modules_side_get_in(ELEMENTS_property_e modules){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_ANNE_RETURN_MODULES_SIDE_GET_IN,
			INIT,
			GET_IN_OUR_SIDE,
			GET_IN_ADV_SIDE,

			GET_IN_POS_OUR_SIDE,
			GET_IN_POS_OUR_SIDE_FROM_ADV_SIDE,
			GET_IN_POS_OUR_SIDE_FROM_CENTRAL,
			ASTAR_OUR_SIDE,

			GET_IN_POS_ADV_SIDE,
			GET_IN_POS_ADV_SIDE_FROM_OUR_SIDE,
			GET_IN_POS_ADV_SIDE_FROM_CENTRAL,
			ASTAR_ADV_SIDE,

			ERROR,
			DONE
		);
//color Y
	switch(state){
		case INIT:
			if(modules == OUR_ELEMENT){
				state = GET_IN_OUR_SIDE;
			}else if(modules == ADV_ELEMENT){
				state = GET_IN_ADV_SIDE;
			}else
				state = ERROR;
			break;

		case GET_IN_OUR_SIDE:
			if(i_am_in_square_color(800, 1400, 300, 900)){
				state = GET_IN_POS_OUR_SIDE;
			}else if(i_am_in_square_color(800, 1400, 2700, 2100)){
				state = GET_IN_POS_OUR_SIDE_FROM_ADV_SIDE;
			}else if(i_am_in_square_color(200, 1100, 900, 2100)){
				state = GET_IN_POS_OUR_SIDE_FROM_CENTRAL;
			}else{
				state = ASTAR_OUR_SIDE;
			}
			break;

		case GET_IN_POS_OUR_SIDE:
			state = try_going(820, COLOR_Y(400), state, DONE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_BRAKE);
			break;

		case GET_IN_POS_OUR_SIDE_FROM_CENTRAL:
			state = try_going(1000, COLOR_Y(1000), state, GET_IN_POS_OUR_SIDE_FROM_CENTRAL, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_BRAKE);
			break;

		case GET_IN_POS_OUR_SIDE_FROM_ADV_SIDE:
			state = try_going(900, COLOR_Y(2000), state, GET_IN_POS_OUR_SIDE_FROM_CENTRAL, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT,END_AT_BRAKE);
			break;

		case ASTAR_OUR_SIDE:
			state = ASTAR_try_going(820, COLOR_Y(400), state, DONE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_BRAKE);
			break;

		case GET_IN_ADV_SIDE:
			if(i_am_in_square_color(800, 1400, 300, 900)){
				state = GET_IN_POS_ADV_SIDE_FROM_OUR_SIDE;
			}else if(i_am_in_square_color(800, 1400, 2700, 2100)){
				state = GET_IN_POS_ADV_SIDE;
			}else if(i_am_in_square_color(200, 1100, 900, 2100)){
				state = GET_IN_POS_ADV_SIDE_FROM_CENTRAL;
			}else{
				state = ASTAR_ADV_SIDE;
			}
			break;

		case GET_IN_POS_ADV_SIDE:
			state = try_going(820, COLOR_Y(2600), state, DONE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_BRAKE);
			break;

		case GET_IN_POS_ADV_SIDE_FROM_CENTRAL:
			state = try_going(1000, COLOR_Y(2000), state, GET_IN_POS_ADV_SIDE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT,END_AT_BRAKE);
			break;

		case GET_IN_POS_ADV_SIDE_FROM_OUR_SIDE:
			state = try_going(900, COLOR_Y(1000), state, GET_IN_POS_ADV_SIDE_FROM_CENTRAL, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT,END_AT_BRAKE);
			break;

		case ASTAR_ADV_SIDE:
			state = ASTAR_try_going(820, COLOR_Y(2600), state, DONE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_BRAKE);
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
				debug_printf("default case in sub_anne_return_modules_side_get_in\n");
			break;
	}
	return IN_PROGRESS;
}

#if 0
error_e sub_anne_get_in_pos_1_depose_module_centre(){
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


error_e sub_anne_get_in_pos_2_depose_module_centre(){
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

error_e sub_anne_get_in_pos_3_depose_module_centre(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_ANNE_GET_IN_POS_3_DEPOSE_MODULES_CENTRE,
			INIT,
			GET_IN_FROM_BLUE_SQUARE,
			GET_IN_FROM_MIDDLE_SQUARE,
			GET_IN_FROM_YELLOW_SQUARE,
			PATHFIND,
			GO_TO_RECALAGE,
			RECALAGE_RELATIF,
			GET_OUT_RECALAGE,
			GO_TO_RUSH,
			OPEN_MULTIFONCTION,
			RUSH,
			CLOSE_PUSHER,
			HALF_TURN,
			MOVE_BACK_TO_PUSH,
			AVANCE,
			DISPOSE,
			COMPUTE_LITTLE_MOVE_BACK,
			LITTLE_MOVE_BACK,
			LITTLE_MOVE_BACK_ERROR,
			MOVE_BACK,
			GET_OUT,
			ERROR,
			DONE
		);
	static Uint8 nb_cylinder_in_basis = 0;
	static Sint16 tryx = 0;
	static Sint16 diffy = 0;


	switch(state){
		case INIT:
			if((i_am_in_square(800, 1400, 300, 900))||(i_am_in_square(200, 1100, 900, 2100))){
				state = GET_IN_FROM_BLUE_SQUARE;
			}else if(i_am_in_square(200, 1100, 900, 2100)){
				state = GET_IN_FROM_MIDDLE_SQUARE;
			}else if(i_am_in_square(800, 1400, 2100, 2700)){
				state = GET_IN_FROM_YELLOW_SQUARE;
			}else{
				state = PATHFIND;
			}
			break;

		case GET_IN_FROM_BLUE_SQUARE:
			state = try_going(975, 1300, state, GO_TO_RECALAGE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT,END_AT_BRAKE);
			break;

		case GET_IN_FROM_MIDDLE_SQUARE:
			state = try_going(1000, COLOR_Y(1250), state, GET_IN_FROM_BLUE_SQUARE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT,END_AT_BRAKE);
			break;

		case GET_IN_FROM_YELLOW_SQUARE:
			state = try_going(900, 2000, state, GET_IN_FROM_BLUE_SQUARE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT,END_AT_BRAKE);
			break;

		case PATHFIND:
			state = ASTAR_try_going(975, 1300, state, GO_TO_RECALAGE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_BRAKE);
			break;

		case GO_TO_RECALAGE:
			state = try_going(1270, 1290, state, RECALAGE_RELATIF, GET_OUT, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

		case RECALAGE_RELATIF:
			state = check_sub_action_result(action_recalage_y(FORWARD, PI4096/2, 1432 - SMALL_CALIBRATION_FORWARD_BORDER_DISTANCE, FALSE, &diffy, TRUE), state, GET_OUT_RECALAGE, GET_OUT_RECALAGE);
			break;

		case GET_OUT_RECALAGE:
			state = try_advance(NULL, entrance, 60, state, GO_TO_RUSH, RECALAGE_RELATIF, FAST, BACKWARD, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

		case GO_TO_RUSH:
			state = try_going(950, 1320, state, OPEN_MULTIFONCTION, OPEN_MULTIFONCTION, FAST, BACKWARD, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

		case OPEN_MULTIFONCTION:
			if(entrance){
				ACT_push_order(ACT_SMALL_CYLINDER_MULTIFONCTION, ACT_SMALL_CYLINDER_MULTIFONCTION_PUSH);
			}
			state = check_act_status(ACT_QUEUE_Small_cylinder_multifonction, state, RUSH, ERROR);
			break;

		case RUSH:
			state = try_rush(1520, 1310, state, MOVE_BACK, MOVE_BACK, FORWARD, NO_DODGE_AND_WAIT, TRUE);
			if(ON_LEAVE()){
				nb_cylinder_in_basis = (1580 - global.pos.x)/100;
			}
			break;

	/*	case CLOSE_PUSHER:
			if(entrance){
				ACT_push_order(ACT_SMALL_CYLINDER_MULTIFONCTION, ACT_SMALL_CYLINDER_MULTIFONCTION_IN);
			}
//			state = check_act_status(ACT_QUEUE_Small_cylinder_multifonction, state, MOVE_BACK, MOVE_BACK);
			state = MOVE_BACK;
			//
			break;*/

		case MOVE_BACK:
			if(entrance){
				PROP_WARNER_arm_x(global.pos.x-50);
			}
			state = try_going(1000, 1310, state, HALF_TURN, ERROR, FAST, BACKWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			if((global.prop.reach_x)||ON_LEAVE()){
				ACT_push_order(ACT_SMALL_CYLINDER_MULTIFONCTION, ACT_SMALL_CYLINDER_MULTIFONCTION_IN);
			}
			break;

		case HALF_TURN:
			state = try_go_angle(-PI4096, state, AVANCE, GET_OUT, FAST, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case AVANCE:
			state = try_going(1520-100*nb_cylinder_in_basis, 1310, state, DISPOSE, GET_OUT, FAST, BACKWARD, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

		case DISPOSE:
			state = check_sub_action_result(sub_act_anne_mae_dispose_modules(ARG_DISPOSE_ONE_CYLINDER_FOLLOW_BY_ANOTHER), state, COMPUTE_LITTLE_MOVE_BACK, GET_OUT);
#warning 'quelles sont les raisons possibles d\'une erreur ?'
			break;

		case COMPUTE_LITTLE_MOVE_BACK:
			if(STOCKS_isEmpty(MODULE_STOCK_SMALL)){
				state = GET_OUT;
			}else if(global.pos.x>1370){
				state = LITTLE_MOVE_BACK;
			}else{// si il te reste des modules tu sors pour pousser encore puis poser
				state = MOVE_BACK_TO_PUSH;
			}
			break;

		case MOVE_BACK_TO_PUSH:
			state = try_going(1000, 1310, state, RUSH, LITTLE_MOVE_BACK_ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case LITTLE_MOVE_BACK:
			state = try_advance(NULL, entrance, 110, state, DISPOSE, LITTLE_MOVE_BACK_ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case LITTLE_MOVE_BACK_ERROR:{
			if(entrance){
				Uint8 nb_module_returned = 0;
				nb_module_returned = (1520 - global.pos.x)/110;
				tryx = 1520 - 110*nb_module_returned;
			}
			state = try_going(tryx, 1310, state, LITTLE_MOVE_BACK, LITTLE_MOVE_BACK, FAST, BACKWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
		}
		break;

		case GET_OUT:
			state = try_going(800, 1310, state, DONE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_BRAKE);
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

error_e sub_anne_get_in_pos_4_depose_module_centre(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_GET_IN_POS_4_DEPOSE_MODULES_CENTRE,
			INIT,
			GET_IN_FROM_OUR_SQUARE,
			GET_IN_FROM_MIDDLE_SQUARE,
			GET_IN_FROM_ADV_SQUARE,
			PATHFIND,
			GO_TO_RECALAGE,
			RECALAGE_RELATIF,
			GET_OUT_RECALAGE,
			GO_TO_RUSH,
			OPEN_MULTIFONCTION,
			RUSH,
			CLOSE_PUSHER,
			HALF_TURN,
			MOVE_BACK_TO_PUSH,
			AVANCE,
			DISPOSE,
			COMPUTE_LITTLE_MOVE_BACK,
			LITTLE_MOVE_BACK,
			LITTLE_MOVE_BACK_ERROR,
			MOVE_BACK,
			GET_OUT,

			ERROR,
			DONE
		);

	static Sint16 diffy = 0;
	static Uint8 nb_cylinder_in_basis = 0;
	static Sint16 tryx = 0;

	switch(state){
		case INIT:
			if((i_am_in_square(800, 1400, 300, 900))||(i_am_in_square(200, 1100, 900, 2100))){
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
			state = try_going(975, 1700, state, GO_TO_RECALAGE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT,END_AT_BRAKE);
			break;

		case GET_IN_FROM_MIDDLE_SQUARE:
			state = try_going(1000, 1750, state, GET_IN_FROM_OUR_SQUARE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT,END_AT_BRAKE);
			break;

		case GET_IN_FROM_ADV_SQUARE:
			state = try_going(900, 1000, state, GET_IN_FROM_OUR_SQUARE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT,END_AT_BRAKE);
			break;

		case PATHFIND:
			state = ASTAR_try_going(975, 1700, state, GO_TO_RECALAGE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_BRAKE);
			break;

		case GO_TO_RECALAGE:
			state = try_going(1270, 1680, state, RECALAGE_RELATIF, GET_OUT, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

		case RECALAGE_RELATIF:
			state = check_sub_action_result(action_recalage_y(FORWARD, -PI4096/2, 1568 + SMALL_CALIBRATION_FORWARD_BORDER_DISTANCE, FALSE, &diffy, TRUE), state, GET_OUT_RECALAGE, GET_OUT_RECALAGE);
			break;

		case GET_OUT_RECALAGE:
			state = try_advance(NULL, entrance, 60, state, GO_TO_RUSH, RECALAGE_RELATIF, FAST, BACKWARD, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

		case GO_TO_RUSH:
			state = try_going(950, 1660, state, OPEN_MULTIFONCTION, OPEN_MULTIFONCTION, FAST, FORWARD, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

		case OPEN_MULTIFONCTION:
			if(entrance){
				ACT_push_order(ACT_SMALL_CYLINDER_MULTIFONCTION, ACT_SMALL_CYLINDER_MULTIFONCTION_PUSH);
			}
			state = check_act_status(ACT_QUEUE_Small_cylinder_multifonction, state, RUSH, ERROR);
			break;

		case RUSH:
			state = try_rush(1520, 1660, state, MOVE_BACK, MOVE_BACK, BACKWARD, NO_DODGE_AND_WAIT, TRUE);
			if(ON_LEAVE()){
				nb_cylinder_in_basis = (1580 - global.pos.x)/100;
			}
			break;

	/*	case CLOSE_PUSHER:
			if(entrance){
				ACT_push_order(ACT_SMALL_CYLINDER_MULTIFONCTION, ACT_SMALL_CYLINDER_MULTIFONCTION_IN);
			}
//			state = check_act_status(ACT_QUEUE_Small_cylinder_multifonction, state, MOVE_BACK, MOVE_BACK);
			state = MOVE_BACK;
			//
			break;*/

		case MOVE_BACK:
			if(entrance){
				PROP_WARNER_arm_x(global.pos.x-50);
			}
			state = try_going(1000, 1660, state, HALF_TURN, ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			if((global.prop.reach_x)||ON_LEAVE()){
				ACT_push_order(ACT_SMALL_CYLINDER_MULTIFONCTION, ACT_SMALL_CYLINDER_MULTIFONCTION_IN);
			}
			break;

		case HALF_TURN:
			state = try_go_angle(0, state, AVANCE, GET_OUT, FAST, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case AVANCE:
			state = try_going(1520-100*nb_cylinder_in_basis, 1660, state, DISPOSE, GET_OUT, FAST, FORWARD, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

		case DISPOSE:
			state = check_sub_action_result(sub_act_anne_mae_dispose_modules(ARG_DISPOSE_ONE_CYLINDER_FOLLOW_BY_ANOTHER), state, COMPUTE_LITTLE_MOVE_BACK, GET_OUT);
#warning 'quelles sont les raisons possibles d\'une erreur ?'
			break;

		case COMPUTE_LITTLE_MOVE_BACK:
			if(STOCKS_isEmpty(MODULE_STOCK_SMALL)){
				state = GET_OUT;
			}else if(global.pos.x>1370){
				state = LITTLE_MOVE_BACK;
			}else{// si il te reste des modules tu sors pour pousser encore puis poser
				state = MOVE_BACK_TO_PUSH;
			}
			break;

		case MOVE_BACK_TO_PUSH:
			state = try_going(1000, 1660, state, RUSH, LITTLE_MOVE_BACK_ERROR, FAST, BACKWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case LITTLE_MOVE_BACK:
			state = try_advance(NULL, entrance, 110, state, DISPOSE, LITTLE_MOVE_BACK_ERROR, FAST, BACKWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case LITTLE_MOVE_BACK_ERROR:{
			if(entrance){
				Uint8 nb_module_returned = 0;
				nb_module_returned = (1520 - global.pos.x)/110;
				tryx = 1520 - 110*nb_module_returned;
			}
			state = try_going(tryx, 1660, state, LITTLE_MOVE_BACK, LITTLE_MOVE_BACK, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
		}
		break;

		case GET_OUT:
			state = try_going(800, 1660, state, DONE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_BRAKE);
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

error_e sub_anne_get_in_pos_5_depose_module_centre(){
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

error_e sub_anne_get_in_pos_6_depose_module_centre(){
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


typedef enum{
	POS_1=0,  //position de notre côté
	POS_2,
	POS_3,
	POS_4,
	POS_5,
	POS_6,
	NO_POS
}dropPlace_e;

dropPlace_e drop_zone_bis =NO_POS;

//Uint8 nbCylindresSurBase = 0;
bool_e depose_side;

static Uint8 nb_try = 0;


error_e sub_anne_depose_centre_manager(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_ANNE_DEPOSE_CENTRE_MANAGER,
				INIT,
				DEPOSE,
				COMPUTE,
				SUCCESS_DEPOSE,
				ERROR_DEPOSE,
				ERROR,
				DONE
			);

	static bool_e depose = FALSE;
	static ELEMENTS_side_match_e side = OUR_SIDE;
	static moduleTypeDominating_e module_type = MODULE_MONO_DOMINATING;

	switch(state){

		case INIT:
			if(STOCKS_getNbModules(MODULE_STOCK_SMALL) > 0 && depose == FALSE){

				if(STOCKS_getDominatingModulesType(MODULE_STOCK_SMALL) == MODULE_MONO_DOMINATING){
					module_type = MODULE_MONO_DOMINATING;
				}
				else if(STOCKS_getDominatingModulesType(MODULE_STOCK_SMALL) == MODULE_POLY_DOMINATING){
					module_type = MODULE_POLY_DOMINATING;
				}else{//A choisir si l'on veut déposer plutôt comme des mono ou poly sur le terrain
					module_type = NO_DOMINATING;
				}
				state = DEPOSE;
			}else{
				state = DONE;
			}
			break;

		case DEPOSE:
			//TODO choix de la zone..
			state = check_sub_action_result(sub_anne_depose_modules_centre(MODULE_MOONBASE_MIDDLE, side), state, SUCCESS_DEPOSE, ERROR_DEPOSE);
			break;

		case COMPUTE:
			if((depose == FALSE && nb_try >= 3) ||(depose == TRUE)){
				state = DONE;
			}
			if(depose == FALSE){
				state = DEPOSE;
			}
			else{
				state = DONE;
			}
			break;

		case SUCCESS_DEPOSE:
			if (last_state == DEPOSE) {depose = TRUE;}
			state = COMPUTE;
			break;

		case ERROR_DEPOSE:
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
				debug_printf("default case in sub_anne_depose_centre_manager\n");
			break;
	}
	return IN_PROGRESS;
}
#endif

#define PUSH_BEFORE_DISPOSE_DISABLE			0		//Si 1 : on désactive le poussage avant dépose.
#define ANGLE_ACCEPT_CORRECTION_RUSH_MOONBASE		(PI4096/18) 	//10 degrés //angle d'erreur en dessous duquel on accèpte la correction !
#define DISTANCE_ACCEPT_CORRECTION_RUSH_MOONBASE	100
#define R2				1.41				//Racine carré de 2, à 3 queues de vaches près
#define R_TO_CENTER		650					//Rush goal to center (2000;1500)
#define dHB				250					//Point H vers point B
#define	MOONBASE_TO_R	68					//Distance entre le centre de la moonbase et le bord (80/2+28)
#define dRA				(MOONBASE_TO_R+150)	//Distance entre le centre de la moonbase et l'axe de turning
#define dRH				(MOONBASE_TO_R+105)	//Distance entre le centre de la moonbase et l'axe de dépose / poussage
#define FN_FN1			120					//Distance entre deux déposes de modules

#define dHC_OUTDOOR		80			//distance entre le Rush Goal et le premier module posable, côté cratère
#define dHC_INDOOR		200			//distance entre le Rush Goal et le premier module posable, côté intérieur

#define	F0D_LONG		100			//L'extraction du dispose axis se fait en deux temps (D puis E)
#define F0D_LARGE		5
#define F0E_LONG		200
#define F0E_LARGE		(dRA - dRH)

//Cette fonction a pour but d'effectuer la dépose de module sur l'une des 3 zones centrales.
//On doit lui préciser sur quelle zone on veut déposer, et via quel côté...
error_e sub_anne_depose_modules_centre(moduleMoonbaseLocation_e moonbase, ELEMENTS_side_match_e basis_side)
{
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_ANNE_DEPOSE_MODULES_CENTRE,
			INIT,
			COMPUTE_POINTS,
			GET_IN,
			PATHFIND,
			RUSH_AND_COMPUTE_O_H_A_B_C,
			EXTRACT_FROM_RUSH_TO_A,
			GOTO_B,
			PUSH_TO_C,
			COMPUTE_F_E_D,
			GOTO_D_AND_E,
			BACK_TO_FX,
			GOTO_FX,
			COMPUTE_DISPOSE_MODULE,
			DISPOSE_MODULE,
			DISPOSE_LAST_MODULE,
			GOTO_NEXT_F,
			BACK_TO_PREVIOUS_F,
			PUSH_DISPOSED_MODULES,
			GET_OUT_TO_B,
			GET_OUT_ERROR,
			ERROR,
			DONE
		);

	typedef enum
	{
		DZONE0_BLUE_OUTDOOR,
		DZONE1_BLUE_INDOOR,
		DZONE2_MIDDLE_BLUE,
		DZONE3_MIDDLE_YELLOW,
		DZONE4_YELLOW_INDOOR,
		DZONE5_YELLOW_OUTDOOR
	}dispose_zone_t;

	static bool_e pusher_is_up = FALSE;
	static dispose_zone_t dzone;
	static color_e color_side;		//Défini si par rapport à une zone on va poser plutôt du côté bleu ou jaune
	static bool_e outdoor;			//Défini si la dépose est au sud (c'est à dire en étant placé près d'un cratère) sinon au nord (entre 2 zones !)


	static GEOMETRY_point_t GetIn;	//Point d'entrée pour continuer 				-> Défini en dur
	static GEOMETRY_point_t G;		//Goal (but du rush... point non atteignable) 	-> Défini en dur
	static GEOMETRY_point_t O;		//Origine du robot à la fin du rush				-> Mesuré lorsqu'on s'y trouve
	static GEOMETRY_vector_t OR;	//vecteur OR (R=centre de la zone de dépose)	->
	static GEOMETRY_vector_t RA;	//vecteur RA (A=projeté de R sur l'axe de dispose ou de push)
	static GEOMETRY_vector_t RH;	//vecteur RH (H=projeté de R sur l'axe de turning)
	static GEOMETRY_point_t A;
	static GEOMETRY_point_t H;

	static GEOMETRY_vector_t HB;	//vecteur HB (B=extraction de bout de ligne pour tourner sur l'axe de dépose)
	static GEOMETRY_vector_t HC;	//vecteur HC (C=point le plus profond du push) -> dépend de la zone choisie (cratère !)
	static GEOMETRY_point_t B;
	static GEOMETRY_point_t C;

	//Le point C pourra être adapté si on push des modules ou si on tombe en erreur en avancant !
	//Le point F0 est calculé selon la réussite à atteindre le point C... et le côté choisi (notamment s'il y a déjà des modules)
	//On en déduit alors les points D et E...
	static GEOMETRY_point_t FX;		//Résultat d'une position atteinte (mesure si échec, théorie si réussite)
	static GEOMETRY_point_t D;
	static GEOMETRY_point_t E;

	static GEOMETRY_vector_t F0D;	//vecteur F0D (D=point pour s'extraire doucement de l'axe de push)
	static GEOMETRY_vector_t F0E;	//vecteur F0E (E=point pour s'extraire carrément de l'axe de push)

	static GEOMETRY_vector_t Fn_to_next;	//Déplacement entre Fn et Fn+1 !


	//Le Getout est le point B !

	//Trajet nominal :

	// GetIn -> G(non atteint) -> A -> B -> C (calcul F0) -> D -> E -> F0 -> F1 -> ... -> F3 -> B



	switch(state){
		case INIT:
			//Color_side
			if(basis_side == OUR_SIDE)
				color_side = global.color;
			else
				color_side = (global.color==BLUE)?YELLOW:BLUE;

			//South (ou north !)
			if((moonbase == MODULE_MOONBASE_OUR_CENTER && basis_side == OUR_SIDE)
					||  (moonbase == MODULE_MOONBASE_ADV_CENTER && basis_side == ADV_SIDE))
				outdoor = TRUE;
			else
				outdoor = FALSE;

			//Dispose Zone !
			switch(moonbase)
			{
				case MODULE_MOONBASE_OUR_CENTER:
					if(global.color == BLUE)
						dzone = (basis_side == OUR_SIDE)?DZONE0_BLUE_OUTDOOR:DZONE1_BLUE_INDOOR;
					else
						dzone = (basis_side == OUR_SIDE)?DZONE5_YELLOW_OUTDOOR:DZONE4_YELLOW_INDOOR;
					break;
				case MODULE_MOONBASE_MIDDLE:
					if(basis_side == OUR_SIDE)
						dzone = (global.color == BLUE)?DZONE2_MIDDLE_BLUE:DZONE3_MIDDLE_YELLOW;
					else
						dzone = (global.color == BLUE)?DZONE3_MIDDLE_YELLOW:DZONE2_MIDDLE_BLUE;

					break;
				case MODULE_MOONBASE_ADV_CENTER:
					if(global.color == BLUE)
						dzone = (basis_side == OUR_SIDE)?DZONE4_YELLOW_INDOOR:DZONE5_YELLOW_OUTDOOR;
					else
						dzone = (basis_side == OUR_SIDE)?DZONE1_BLUE_INDOOR:DZONE0_BLUE_OUTDOOR;
					break;
				default:
					break;
			}

			if(moonbase == MODULE_MOONBASE_OUR_SIDE || moonbase == MODULE_MOONBASE_ADV_SIDE)
				state = ERROR;	//C'est pas cette fonction qui gère la dépose latérale !
			else
				state = COMPUTE_POINTS;
			break;
		case COMPUTE_POINTS:
			//On calcule les points qui seront utilisés dans les trajectoires.
			if(dzone == DZONE0_BLUE_OUTDOOR)
			{
				GetIn = (GEOMETRY_point_t){1725,860};
				if(i_am_in_square(1000, 1800, 200, 650))
					state = GET_IN;
				else
					state = PATHFIND;
			}
			else if(dzone == DZONE5_YELLOW_OUTDOOR)
			{
				GetIn = (GEOMETRY_point_t){1725,2140};
				if(i_am_in_square(1000, 1800, 2350, 2800))
					state = GET_IN;
				else
					state = PATHFIND;
			}
			else if(dzone == DZONE1_BLUE_INDOOR || dzone == DZONE2_MIDDLE_BLUE)
			{
				GetIn = (GEOMETRY_point_t){1350,1240};
				if(i_am_in_square(0, 1300, 1000, 1300))
					state = GET_IN;
				else
					state = PATHFIND;
			}
			else	//dzone 3 ou 4
			{
				GetIn = (GEOMETRY_point_t){1350,1760};
				if(i_am_in_square(0, 1300, 1700, 2000))
					state = GET_IN;
				else
					state = PATHFIND;
			}

			//Goal (but du rush... point non atteignable) 	-> Défini en dur
			if(dzone == DZONE0_BLUE_OUTDOOR || dzone == DZONE1_BLUE_INDOOR)
				G = (GEOMETRY_point_t){2000-R_TO_CENTER/R2,1500-R_TO_CENTER/R2};
			else if(dzone == DZONE2_MIDDLE_BLUE || dzone == DZONE3_MIDDLE_YELLOW)
				G = (GEOMETRY_point_t){2000-R_TO_CENTER,1500};
			else
				G = (GEOMETRY_point_t){2000-R_TO_CENTER/R2,1500+R_TO_CENTER/R2};

			switch(dzone)
			{
				case DZONE0_BLUE_OUTDOOR:
					OR = (GEOMETRY_vector_t){-(MOONBASE_TO_R+SMALL_CALIBRATION_FORWARD_BORDER_DISTANCE)/R2,(MOONBASE_TO_R+SMALL_CALIBRATION_FORWARD_BORDER_DISTANCE)/R2};
					RA = (GEOMETRY_vector_t){dRA/R2, -dRA/R2};
					RH = (GEOMETRY_vector_t){dRH/R2, -dRH/R2};
					HB = (GEOMETRY_vector_t){-dHB/R2, -dHB/R2};
					HC = (GEOMETRY_vector_t){dHC_OUTDOOR/R2, dHC_OUTDOOR/R2};
					Fn_to_next = (GEOMETRY_vector_t){-FN_FN1/R2, -FN_FN1/R2};
					F0D = (GEOMETRY_vector_t){(-F0D_LONG+F0D_LARGE)/R2, (-F0D_LONG-F0D_LARGE)/R2};
					F0E = (GEOMETRY_vector_t){(-F0E_LONG+F0E_LARGE)/R2, (-F0E_LONG-F0E_LARGE)/R2};
					break;
				case DZONE1_BLUE_INDOOR:
					OR = (GEOMETRY_vector_t){(MOONBASE_TO_R+SMALL_CALIBRATION_FORWARD_BORDER_DISTANCE)/R2,-(MOONBASE_TO_R+SMALL_CALIBRATION_FORWARD_BORDER_DISTANCE)/R2};
					RA = (GEOMETRY_vector_t){-dRA/R2, dRA/R2};
					RH = (GEOMETRY_vector_t){-dRH/R2, dRH/R2};
					HB = (GEOMETRY_vector_t){-dHB/R2, -dHB/R2};
					HC = (GEOMETRY_vector_t){dHC_INDOOR/R2, dHC_INDOOR/R2};
					Fn_to_next = (GEOMETRY_vector_t){-FN_FN1/R2, -FN_FN1/R2};
					F0D = (GEOMETRY_vector_t){(-F0D_LONG-F0D_LARGE)/R2, (-F0D_LONG+F0D_LARGE)/R2};
					F0E = (GEOMETRY_vector_t){(-F0E_LONG-F0E_LARGE)/R2, (-F0E_LONG+F0E_LARGE)/R2};
					break;
				case DZONE2_MIDDLE_BLUE:
					OR = (GEOMETRY_vector_t){0,MOONBASE_TO_R+SMALL_CALIBRATION_FORWARD_BORDER_DISTANCE};
					RA = (GEOMETRY_vector_t){0,-dRA};
					RH = (GEOMETRY_vector_t){0,-dRH};
					HB = (GEOMETRY_vector_t){-dHB, 0};
					HC = (GEOMETRY_vector_t){dHC_INDOOR, 0};
					Fn_to_next = (GEOMETRY_vector_t){-FN_FN1, 0};
					F0D = (GEOMETRY_vector_t){(-F0D_LONG), -F0D_LARGE};
					F0E = (GEOMETRY_vector_t){(-F0E_LONG), -F0E_LARGE};
					break;
				case DZONE3_MIDDLE_YELLOW:
					OR = (GEOMETRY_vector_t){0,-MOONBASE_TO_R-SMALL_CALIBRATION_FORWARD_BORDER_DISTANCE};
					RA = (GEOMETRY_vector_t){0,dRA};
					RH = (GEOMETRY_vector_t){0,dRH};
					HB = (GEOMETRY_vector_t){-dHB, 0};
					HC = (GEOMETRY_vector_t){dHC_INDOOR, 0};
					Fn_to_next = (GEOMETRY_vector_t){-FN_FN1, 0};
					F0D = (GEOMETRY_vector_t){(-F0D_LONG), F0D_LARGE};
					F0E = (GEOMETRY_vector_t){(-F0E_LONG), F0E_LARGE};
					break;
				case DZONE4_YELLOW_INDOOR:
					OR = (GEOMETRY_vector_t){(MOONBASE_TO_R+SMALL_CALIBRATION_FORWARD_BORDER_DISTANCE)/R2,(MOONBASE_TO_R+SMALL_CALIBRATION_FORWARD_BORDER_DISTANCE)/R2};
					RA = (GEOMETRY_vector_t){-dRA/R2, -dRA/R2};
					RH = (GEOMETRY_vector_t){-dRH/R2, -dRH/R2};
					HB = (GEOMETRY_vector_t){-dHB/R2, dHB/R2};
					HC = (GEOMETRY_vector_t){dHC_INDOOR/R2, -dHC_INDOOR/R2};
					Fn_to_next = (GEOMETRY_vector_t){-FN_FN1/R2, FN_FN1/R2};
					F0D = (GEOMETRY_vector_t){(-F0D_LONG-F0D_LARGE)/R2, (F0D_LONG-F0D_LARGE)/R2};
					F0E = (GEOMETRY_vector_t){(-F0E_LONG-F0E_LARGE)/R2, (F0E_LONG-F0E_LARGE)/R2};
					break;
				case DZONE5_YELLOW_OUTDOOR:
					OR = (GEOMETRY_vector_t){-(MOONBASE_TO_R+SMALL_CALIBRATION_FORWARD_BORDER_DISTANCE)/R2,-(MOONBASE_TO_R+SMALL_CALIBRATION_FORWARD_BORDER_DISTANCE)/R2};
					RA = (GEOMETRY_vector_t){dRA/R2, dRA/R2};
					RH = (GEOMETRY_vector_t){dRH/R2, dRH/R2};
					HB = (GEOMETRY_vector_t){-dHB/R2, dHB/R2};
					HC = (GEOMETRY_vector_t){dHC_OUTDOOR/R2, -dHC_OUTDOOR/R2};
					Fn_to_next = (GEOMETRY_vector_t){-FN_FN1/R2, FN_FN1/R2};
					F0D = (GEOMETRY_vector_t){(-F0D_LONG+F0D_LARGE)/R2, (F0D_LONG+F0D_LARGE)/R2};
					F0E = (GEOMETRY_vector_t){(-F0E_LONG+F0E_LARGE)/R2, (F0E_LONG+F0E_LARGE)/R2};
					break;
			}


			//Seront calculables à partir du rush :
			//O : Origine du robot à la fin du rush				-> Mesuré lorsqu'on s'y trouve
			//OR;	//vecteur OR (R=centre de la zone de dépose)
			//	A = O + OR + RA;
			//	H = O + OR + RH;
			//	B = H + HB;
			//	C = H + HC;

			//Seront calculables à partir du push :
			//Le point F0 est calculé selon la réussite à atteindre le point C... et le côté choisi (notamment s'il y a déjà des modules)
			//On en déduit alors les points D et E...
			//F0;		//Résultat d'une position atteinte (mesure si échec, théorie si réussite)
			//  D = F0 + F0D;
			//  E = F0 + F0E;
			//  F1 = F0 + Fn_to_next;	//Si existe
			//  F2 = F1 + Fn_to_next;	//Si existe
			//  F3 = F2 + Fn_to_next;	//Si existe

			break;

		case GET_IN:	//On file direct vers le point de get_in
			state = try_going(GetIn.x, GetIn.y, state, RUSH_AND_COMPUTE_O_H_A_B_C, ERROR, FAST, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;
		case PATHFIND:
			state = ASTAR_try_going(GetIn.x, GetIn.y, state, RUSH_AND_COMPUTE_O_H_A_B_C, ERROR, FAST, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;
		case RUSH_AND_COMPUTE_O_H_A_B_C:
			state = try_rush(G.x, G.y, state, EXTRACT_FROM_RUSH_TO_A, EXTRACT_FROM_RUSH_TO_A, FORWARD, NO_DODGE_AND_WAIT, TRUE);
			if(ON_LEAVE())
			{
				if(moonbase == MODULE_MOONBASE_MIDDLE)
				{

					if(color_side==BLUE)
					{
						O = (GEOMETRY_point_t){global.pos.x,1500-MOONBASE_TO_R-SMALL_CALIBRATION_FORWARD_BORDER_DISTANCE};
						PROP_set_position(global.pos.x, O.y,PI4096/2);
					}
					else
					{
						O = (GEOMETRY_point_t){global.pos.x,1500+MOONBASE_TO_R+SMALL_CALIBRATION_FORWARD_BORDER_DISTANCE};
						PROP_set_position(global.pos.x, O.y,-PI4096/2);
					}
				}
				else
				{
					O = (GEOMETRY_point_t){global.pos.x, global.pos.y};
					if(absolute(global.pos.angle - PI4096/4) < ANGLE_ACCEPT_CORRECTION_RUSH_MOONBASE)
						PROP_set_position(global.pos.x, global.pos.y,PI4096/4);
					else if(absolute(global.pos.angle + PI4096/4) < ANGLE_ACCEPT_CORRECTION_RUSH_MOONBASE)
						PROP_set_position(global.pos.x, global.pos.y,-PI4096/4);
					else if(absolute(global.pos.angle - 3*PI4096/4) < ANGLE_ACCEPT_CORRECTION_RUSH_MOONBASE)
						PROP_set_position(global.pos.x, global.pos.y,3*PI4096/4);
					else if(absolute(global.pos.angle + 3*PI4096/4) < ANGLE_ACCEPT_CORRECTION_RUSH_MOONBASE)
						PROP_set_position(global.pos.x, global.pos.y,-3*PI4096/4);
				}

				H = (GEOMETRY_point_t){O.x + OR.x + RH.x, O.y + OR.y + RH.y};
				A = (GEOMETRY_point_t){O.x + OR.x + RA.x, O.y + OR.y + RA.y};
				B = (GEOMETRY_point_t){H.x + HB.x, H.y + HB.y};
				C = (GEOMETRY_point_t){H.x + HC.x, H.y + HC.y};
				FX = (GEOMETRY_point_t){C.x, C.y};	//on fait l'hypothèse que C est atteignable, c'est donc notre F0
					//ce point sera susceptible de bouger si on montre que C n'est pas atteignable !
				debug_printf("Points mesurés et calculés :\n");
				debug_printf("O :%4d ; %4d\n", O.x, O.y);
				debug_printf("H :%4d ; %4d\n", H.x, H.y);
				debug_printf("A :%4d ; %4d\n", A.x, A.y);
				debug_printf("B :%4d ; %4d\n", B.x, B.y);
				debug_printf("C :%4d ; %4d\n", C.x, C.y);

			}
			break;

		case EXTRACT_FROM_RUSH_TO_A:
			state = try_going(A.x, A.y, state, GOTO_B, RUSH_AND_COMPUTE_O_H_A_B_C, FAST, BACKWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			if(ON_LEAVE())
			{
				if(PUSH_BEFORE_DISPOSE_DISABLE)
					state = COMPUTE_F_E_D;	//On saute les points B et C qui permettent le poussage.
			}
			break;
		case GOTO_B:
			//inverser le sens si un servo de poussage est ajouté côté dépose.
			state = try_going(B.x, B.y, state, PUSH_TO_C, COMPUTE_F_E_D, FAST, (color_side==BLUE)?BACKWARD:FORWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;
		case PUSH_TO_C:
			if (entrance)
			{
				ACT_push_order(ACT_CYLINDER_PUSHER_LEFT,  ACT_CYLINDER_PUSHER_LEFT_OUT);
				pusher_is_up = TRUE;
			}
			//La flemme d'attendre que le pusher soit descendu pour avancer...
			//Si ca pose problème, on pourra toujours ajouter un état intermédiaire avant ce PUSH_TO_C

			//inverser le sens si un servo de poussage est ajouté côté dépose.
			//il peut être nominal d'échouer lors de cette trajectoire (si on bloque contre des modules...)
			state = try_going(C.x, C.y, state, COMPUTE_F_E_D, ERROR, FAST, (color_side==BLUE)?FORWARD:BACKWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			if(state == ERROR)
			{
				state = COMPUTE_F_E_D;
				FX = (GEOMETRY_point_t){global.pos.x , global.pos.y};	//le point C n'est pas atteignable, notre point actuel servira donc de F0.
			}
			break;
		case COMPUTE_F_E_D:
			//TODO dégager cet état lorsqu'on aura un servo côté dépose !
			D = (GEOMETRY_point_t){FX.x + F0D.x, FX.y + F0D.y};
			E = (GEOMETRY_point_t){FX.x + F0E.x, FX.y + F0E.y};

			debug_printf("FX (au début) :%4d ; %4d\n", FX.x, FX.y);
			debug_printf("D :%4d ; %4d\n", D.x, D.y);
			debug_printf("E :%4d ; %4d\n", E.x, E.y);
			state = GOTO_D_AND_E;
			break;
		case GOTO_D_AND_E:
			if(entrance && pusher_is_up)
			{
				ACT_push_order(ACT_CYLINDER_PUSHER_LEFT,  ACT_CYLINDER_PUSHER_LEFT_IN);
				pusher_is_up = FALSE;
			}
			state = try_going_multipoint(	(displacement_t []){
											(displacement_t){ (GEOMETRY_point_t) {D.x, D.y}, FAST},
											(displacement_t){ (GEOMETRY_point_t) {E.x, E.y}, FAST}
											}, 2, state, GOTO_FX, BACK_TO_FX, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;
		case BACK_TO_FX:	//On retourne a C si échec d'extraction
			state = try_going(FX.x, FX.y, state, GOTO_D_AND_E, GOTO_D_AND_E, SLOW, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);

			break;
		case GOTO_FX:
			//Le sens est important... il faut maintenant qu'on se place pour la pose !
			//state = try_going(FX.x, FX.y, state, COMPUTE_DISPOSE_MODULE, GOTO_D_AND_E, SLOW, (color_side==BLUE)?BACKWARD:FORWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			state = try_going_multipoint(	(displacement_t []){
											(displacement_t){ (GEOMETRY_point_t) {D.x, D.y}, FAST},
											(displacement_t){ (GEOMETRY_point_t) {FX.x, FX.y}, FAST}
											}, 2, state, COMPUTE_DISPOSE_MODULE, GOTO_D_AND_E, (color_side==BLUE)?BACKWARD:FORWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);

			break;
		case COMPUTE_DISPOSE_MODULE:

			//TODO détection de fin de stock ou de fin de zone ou savoir si c'est le dernier module à poser
			//TODO gestion du besoin de poussage après dépose..
			if(STOCKS_getNbModules(MODULE_STOCK_SMALL) == 0)
				state = GET_OUT_TO_B;			//fini !
			else if(STOCKS_getNbModules(MODULE_STOCK_SMALL) == 1)
				state = DISPOSE_LAST_MODULE;	//Presque fini !
			else
				state = DISPOSE_MODULE;	//Encore du taff !
			break;
		case DISPOSE_MODULE:
			state = check_sub_action_result(sub_act_anne_mae_dispose_modules(ARG_DISPOSE_ONE_CYLINDER_FOLLOW_BY_ANOTHER),state,GOTO_NEXT_F,GOTO_NEXT_F);

			//On considère qu'on a posé des modules unicolores de notre couleur... il n'est pas question de venir les retourner plus tard...
			//c'est sans doute perfectible en prenant en compte ce qu'on a réellement déposé.
			if(ON_LEAVE())
				MOONBASES_addModule((global.color==BLUE)?MODULE_BLUE:MODULE_YELLOW, moonbase);
			break;
		case DISPOSE_LAST_MODULE:
			state = check_sub_action_result(sub_act_anne_mae_dispose_modules(ARG_DISPOSE_ONE_CYLINDER_AND_FINISH), state, PUSH_DISPOSED_MODULES, PUSH_DISPOSED_MODULES);

			if(ON_LEAVE())
				MOONBASES_addModule((global.color==BLUE)?MODULE_BLUE:MODULE_YELLOW, moonbase);
			break;
		case GOTO_NEXT_F:
			if(entrance)
				FX = (GEOMETRY_point_t){FX.x + Fn_to_next.x, FX.y + Fn_to_next.y};	//Next point...
			state = try_going(FX.x, FX.y, state, COMPUTE_DISPOSE_MODULE, BACK_TO_PREVIOUS_F, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_BRAKE);
			//Le end at break permettra sans doute de démarrer l'actionneur avant la fin du mouvement, c'est l'éclate !
			break;
		case BACK_TO_PREVIOUS_F:
			if(entrance)
				FX = (GEOMETRY_point_t){FX.x - Fn_to_next.x, FX.y - Fn_to_next.y};	//Previous point...
			state = try_going(FX.x, FX.y, state, GOTO_NEXT_F, GOTO_NEXT_F, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;
		case PUSH_DISPOSED_MODULES:
			//TODO
			state = GET_OUT_TO_B;
			break;
		case GET_OUT_TO_B:
			state = try_going(B.x, B.y, state, DONE, BACK_TO_PREVIOUS_F, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
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
				debug_printf("default case in sub_anne_depose_modules_centre\n");
			break;
	}
	return IN_PROGRESS;
}


#if 0
error_e sub_anne_get_in_depose_modules_centre(moduleTypeDominating_e module_type, moduleStockLocation_e robot_side, ELEMENTS_side_match_e basis_side){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_ANNE_GET_IN_DEPOSE_MODULES_CENTRE,
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
			if((module_type == MODULE_POLY_DOMINATING) && (basis_side == OUR_SIDE)){
				if(MOONBASES_getNbModules(MODULE_MOONBASE_MIDDLE)<6){
					state = GET_IN_POS_3;  //notre depose de notre cote  SIM 6
				}else if(MOONBASES_getNbModules(MODULE_MOONBASE_OUR_SIDE)<6){
					state = GET_IN_POS_1;
				}else if(MOONBASES_getNbModules(MODULE_MOONBASE_ADV_SIDE)<6){
					state = GET_IN_POS_5;
				}else{
					state = ERROR;
				}
			}else if(basis_side == OUR_SIDE){
				if(MOONBASES_getNbModules(MODULE_MOONBASE_OUR_CENTER)<6){
					state = GET_IN_POS_2;  //notre depose de notre cote  SIM 6
				}else if(MOONBASES_getNbModules(MODULE_MOONBASE_MIDDLE)<6){
					state = GET_IN_POS_4;
				}else{
					state = ERROR;
				}
			}else if((module_type == MODULE_MONO_DOMINATING) && (robot_side == MODULE_STOCK_LEFT) && (basis_side == OUR_SIDE)){
				if(MOONBASES_getNbModules(MODULE_MOONBASE_MIDDLE)<6){
					state = GET_IN_POS_3;  //notre depose de notre cote  SIM 6
				}else if(MOONBASES_getNbModules(MODULE_MOONBASE_OUR_CENTER)<6){
					state = GET_IN_POS_1;
				}else if(MOONBASES_getNbModules(MODULE_MOONBASE_ADV_CENTER)<6){
					state = GET_IN_POS_5;
				}else{
					state = ERROR;
				}

			}else if(((module_type == MODULE_MONO_DOMINATING) && (robot_side == MODULE_STOCK_RIGHT) && (basis_side == ADV_SIDE))\
					|| ((module_type == MODULE_POLY_DOMINATING) && (robot_side == MODULE_STOCK_RIGHT) && (basis_side == ADV_SIDE))){
				if(MOONBASES_getNbModules(MODULE_MOONBASE_MIDDLE)<6){
					state = GET_IN_POS_4;  //notre depose de notre cote  SIM 6
				}else if(MOONBASES_getNbModules(MODULE_MOONBASE_OUR_CENTER)<6){
					state = GET_IN_POS_2;
				}else{
					state = ERROR;
				}

			}else if(((module_type == MODULE_MONO_DOMINATING) && (robot_side == MODULE_STOCK_LEFT) && (basis_side == ADV_SIDE))\
					||((module_type == MODULE_POLY_DOMINATING) && (robot_side == MODULE_STOCK_LEFT) && (basis_side == ADV_SIDE))){
				if(MOONBASES_getNbModules(MODULE_MOONBASE_ADV_SIDE)<6){
					state = GET_IN_POS_5;  //notre depose de notre cote  SIM 6
				}else if(MOONBASES_getNbModules(MODULE_MOONBASE_MIDDLE)<6){
					state = GET_IN_POS_3;
				}else{
					state = ERROR;
				}

			/*}else if((module_type == ADV_ELEMENT) && (basis_side == ADV_SIDE)){
				state = GET_IN_POS_6; //centre depose du cote adv SIM 1
*/

			}else{
				state = ERROR;
			}
			//ce manager me gonfle je met ça pour mon test on verra à après pour le manager.
			state = GET_IN_POS_3;
			break;

		case GET_IN_POS_1:
			if(entrance){
				drop_zone_bis = POS_1;
			}
			state = check_sub_action_result(sub_anne_get_in_pos_1_depose_module_centre(), state, DONE, ERROR);
			break;

		case GET_IN_POS_2:
			if(entrance){
				drop_zone_bis = POS_2;
			}
			state = check_sub_action_result(sub_anne_get_in_pos_2_depose_module_centre(), state, DONE, ERROR);
			break;

		case GET_IN_POS_3:
			if(entrance){
				drop_zone_bis = POS_3;
			}
			state = check_sub_action_result(sub_anne_get_in_pos_3_depose_module_centre(), state, DONE, ERROR);
			break;

		case GET_IN_POS_4:
			if(entrance){
				drop_zone_bis = POS_4;
			}
			state = check_sub_action_result(sub_anne_get_in_pos_4_depose_module_centre(), state, DONE, ERROR);
			break;

		case GET_IN_POS_5:
			if(entrance){
				drop_zone_bis = POS_5;
			}
			state = check_sub_action_result(sub_anne_get_in_pos_5_depose_module_centre(), state, DONE, ERROR);
			break;

		case GET_IN_POS_6:
			if(entrance){
				drop_zone_bis = POS_6;
			}
			state = check_sub_action_result(sub_anne_get_in_pos_6_depose_module_centre(), state, DONE, ERROR);
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
#endif



/*
 *
	get in
	calage contre la zone
	calcul des points
	get_out_to_turning_axe
	courbe pour approche de la zone (scan faisable en même temps)
	dépose 1
	dépose 2
	dépose 3
	poussage
	dépose 4 ?

	Si on vient de la prise fusée adverse et que le précédent calage contre cette zone est récent
	-> on peut embrayer direct sur le calcul des points et l'entrée en zone...

	Ce cas peut-être géré en faisant la dépose directement dans la sub de prise fusée adverse ?
		en appelant la sub de dépose side à la première position de dépose... !!!
 */
#define OFFSET_M_R		25	//distance dans le sens avant-arrière entre le module stocké dans le robot et le centre du robot.

error_e sub_anne_dispose_modules_side(ELEMENTS_side_match_e side)
{
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_ANNE_DISPOSE_MODULES_SIDE,
			INIT,
			GET_IN,
			ASTAR_GET_IN,
			RUSH_TO_GOAL,
			EXTRACT_FROM_RUSH_TO_A,
			GOTO_F0_VIA_E0,
			GOTO_F1,
			GOTO_F2,
			GOTO_P0,
			GOTO_P1,
			GOTO_F3,
			DISPOSE,
			EXTRACT_BEYOND_F0,
			EXTRACT_BEYOND_F3,
			ERROR,
			DONE
		);
	error_e ret;
	ret = IN_PROGRESS;
	static GEOMETRY_point_t G, O, A, E0, F0, F1, F2, F3, P0, P1;
	static color_e color_side;
	static displacement_t output_curve[2];

	switch(state)
	{
		case INIT:
			if(side == OUR_SIDE)
				color_side = global.color;
			else
				color_side = (global.color==BLUE)?YELLOW:BLUE;
			if(color_side == BLUE)
			{
				G = (GEOMETRY_point_t){1000-OFFSET_M_R,0};
				O = (GEOMETRY_point_t){1000-OFFSET_M_R,197};
				A = (GEOMETRY_point_t){1000-OFFSET_M_R,258};
				E0 = (GEOMETRY_point_t){900-OFFSET_M_R,220};
				F0 = (GEOMETRY_point_t){800-OFFSET_M_R,208};
				F1 = (GEOMETRY_point_t){925-OFFSET_M_R,208};
				F2 = (GEOMETRY_point_t){1050-OFFSET_M_R,208};
				F3 = (GEOMETRY_point_t){1075-OFFSET_M_R,208};
				P0 = (GEOMETRY_point_t){1100-OFFSET_M_R,208};
				P1 = (GEOMETRY_point_t){950-OFFSET_M_R,208};

				//TODO ajouter les cas spécifiques où l'on est déjà sur place (après prise fusée adverse notamment !)
				if(i_am_in_square(750,1350,200,1400))
					state = GET_IN;
				else
					state = ASTAR_GET_IN;
			}
			else
			{
				G = (GEOMETRY_point_t){850+OFFSET_M_R,3000};
				O = (GEOMETRY_point_t){850+OFFSET_M_R,2903};
				A = (GEOMETRY_point_t){850+OFFSET_M_R,2742};
				E0 = (GEOMETRY_point_t){950+OFFSET_M_R,2780};
				F0 = (GEOMETRY_point_t){1050+OFFSET_M_R,2792};
				F1 = (GEOMETRY_point_t){925+OFFSET_M_R,2792};
				F2 = (GEOMETRY_point_t){800+OFFSET_M_R,2792};
				F3 = (GEOMETRY_point_t){825+OFFSET_M_R,2792};
				P0 = (GEOMETRY_point_t){750+OFFSET_M_R,2792};
				P1 = (GEOMETRY_point_t){900+OFFSET_M_R,2792};

				if(i_am_in_square(750,1350,1600,2800))
					state = GET_IN;
				else
					state = ASTAR_GET_IN;
			}
			break;
		case GET_IN:
			state = try_going(A.x, A.y, state, RUSH_TO_GOAL, ERROR, FAST, FORWARD, DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;
		case ASTAR_GET_IN:
			state = ASTAR_try_going(A.x, A.y, state, RUSH_TO_GOAL, ERROR, FAST, FORWARD, DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;
		case RUSH_TO_GOAL:
			state = try_rush(G.x, G.y, state, EXTRACT_FROM_RUSH_TO_A, EXTRACT_FROM_RUSH_TO_A, FORWARD, NO_DODGE_AND_WAIT, TRUE);
			if(ON_LEAVE())
			{
				if(color_side == BLUE)
				{
						//Si moins de 10cm d'erreur... (dans le bon sens... pas de limite si on est hors terrain)
					if(		(absolute(global.pos.angle - PI4096/2) < ANGLE_ACCEPT_CORRECTION_RUSH_MOONBASE)
						&&	((global.pos.y - O.y) < DISTANCE_ACCEPT_CORRECTION_RUSH_MOONBASE) )
						PROP_set_position(global.pos.x, O.y,-PI4096/2);
				}
				else
				{
					if(		(absolute(global.pos.angle + PI4096/2) < ANGLE_ACCEPT_CORRECTION_RUSH_MOONBASE)
						&&	((O.y - global.pos.y) < DISTANCE_ACCEPT_CORRECTION_RUSH_MOONBASE) )
						PROP_set_position(global.pos.x, O.y,PI4096/2);
				}
			}

			//TODO enrichir avec un scan pour permettre le remplissage d'une zone non vide... en rejoignant après un poussage la bonne case directement.
			break;
		case EXTRACT_FROM_RUSH_TO_A:
			state = try_going(A.x, A.y, state, GOTO_F0_VIA_E0, RUSH_TO_GOAL, FAST, BACKWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;
		case GOTO_F0_VIA_E0:
			//BACKWARD pour se mettre du bon côté !!!
			state = try_going_multipoint(	(displacement_t []){	(GEOMETRY_point_t){E0.x, E0.y}, FAST,(GEOMETRY_point_t){F0.x, F0.y}, FAST}, 2, state, DISPOSE, EXTRACT_BEYOND_F3, BACKWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;
		case DISPOSE:{
			static enum state_e success_state;
			static arg_dipose_mae_e last_module;
			if(entrance)
			{
				if(STOCKS_getNbModules(MODULE_STOCK_SMALL) > 1)//Il reste plus d'un module à déposer. Il faudra donc une autre dépose.
				{
					if(last_state == GOTO_F0_VIA_E0)
						success_state = GOTO_F1;
					else if(last_state == GOTO_F1)
						success_state = GOTO_F2;
					else if(last_state == GOTO_F2)
						success_state = GOTO_P0;	//Il faut pousser !
					else
						success_state = EXTRACT_BEYOND_F3;

					last_module = ARG_DISPOSE_ONE_CYLINDER_FOLLOW_BY_ANOTHER;
				}
				else if(STOCKS_getNbModules(MODULE_STOCK_SMALL)==1)//Dernière dépose
				{
					success_state = EXTRACT_BEYOND_F3;
					last_module = ARG_DISPOSE_ONE_CYLINDER_AND_FINISH;
				}
				else	//On a plus rien à déposer
				{
					state = EXTRACT_BEYOND_F3;
				}
			}
			else	//le else est important en cas de sortie effectuée dans l'entrance !
			{
				state = check_sub_action_result(sub_act_anne_mae_dispose_modules(last_module),state,success_state,EXTRACT_BEYOND_F3);
				if(ON_LEAVE())
				{
					if(STOCKS_getNbModules(MODULE_STOCK_SMALL) == 0)
						nop(); //TODO : Désactiver la sub de dépose, on est vide !
				}
			}

			break;}
		case GOTO_F1:
			state = try_going(F1.x, F1.y, state, DISPOSE, EXTRACT_BEYOND_F0, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;
		case GOTO_F2:
			state = try_going(F2.x, F2.y, state, DISPOSE, EXTRACT_BEYOND_F0, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case GOTO_P0:
			state = try_going(P0.x, P0.y, state, DISPOSE, EXTRACT_BEYOND_F0, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			if(ON_LEAVE())
			{
				//TODO sortir bras de push
			}
			break;
		case GOTO_P1:
			state = try_going(P1.x, P1.y, state, DISPOSE, EXTRACT_BEYOND_F3, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			if(ON_LEAVE())
			{
				//TODO rentrer bras de push
			}
			break;
		case GOTO_F3:
			state = try_going(F3.x, F3.y, state, DISPOSE, EXTRACT_BEYOND_F0, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case EXTRACT_BEYOND_F0:

			if(entrance)
			{
				//On veut sortir vers le côté où se trouve F0
				//On calcule une courbe de sortie...
				if(color_side == BLUE)
				{
					output_curve[0].point.x = global.pos.x - 100;
					output_curve[0].point.y = MIN(global.pos.y + 15, A.y);	//L'écretage est utile si on vient d'un échec d'extraction précédent
					output_curve[0].speed = FAST;
					output_curve[1].point.x = global.pos.x - 200;
					output_curve[1].point.y = MIN(global.pos.y + 50, A.y);
					output_curve[1].speed = FAST;
				}
				else
				{
					output_curve[0].point.x = global.pos.x + 100;
					output_curve[0].point.y = MAX(global.pos.y - 15, A.y);
					output_curve[0].speed = FAST;
					output_curve[1].point.x = global.pos.x + 200;
					output_curve[1].point.y = MAX(global.pos.y - 50, A.y);
					output_curve[1].speed = FAST;
				}
				//TODO borner pour ne pas génèrer de point innaccessible (cratère / obstacle.... !)
			}
			state = try_going_multipoint(	output_curve, 2, state, DONE, EXTRACT_BEYOND_F3, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);

			break;

		case EXTRACT_BEYOND_F3:
			if(entrance)
			{
				//On veut sortir vers le côté où se trouve F0
				//On calcule une courbe de sortie...
				if(color_side == BLUE)
				{
					output_curve[0].point.x = global.pos.x + 100;
					output_curve[0].point.y = MIN(global.pos.y + 15, A.y);
					output_curve[0].speed = FAST;
					output_curve[1].point.x = global.pos.x + 200;
					output_curve[1].point.y = MIN(global.pos.y + 50, A.y);
					output_curve[1].speed = FAST;
				}
				else
				{
					output_curve[0].point.x = global.pos.x - 100;
					output_curve[0].point.y = MAX(global.pos.y - 15, A.y);
					output_curve[0].speed = FAST;
					output_curve[1].point.x = global.pos.x - 200;
					output_curve[1].point.y = MAX(global.pos.y - 50, A.y);
					output_curve[1].speed = FAST;
				}

				//TODO borner pour ne pas génèrer de point innaccessible (cratère / obstacle.... !)

			}
			state = try_going_multipoint(	output_curve, 2, state, DONE, EXTRACT_BEYOND_F0, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;
		case ERROR:
			RESET_MAE();
			on_turning_point();
			ret = NOT_HANDLED;
			break;

		case DONE:
			RESET_MAE();
			on_turning_point();
			ret = END_OK;
			break;

		default:
			if(entrance)
				debug_printf("default case in sub_anne_return_side\n");
			break;
	}


	return ret;
}

#if 0
error_e sub_anne_dispose_modules_side(ELEMENTS_property_e modules){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_ANNE_DISPOSE_MODULES_SIDE,
			INIT,
			GET_IN,
			COMPUTE,

			GO_TO_BLUE,
			OPEN_MULTIFONCTION_BLUE,
			RUSH_BLUE,
			CLOSE_PUSHER_BLUE,
			AVANCE_BLUE,
			ENABLE_MAGIC_BLUE,
			COMPUTE_LITTLE_MOVE_BACK_BLUE,
			LITTLE_MOVE_BACK_BLUE,
			LITTLE_MOVE_BACK_ERROR_BLUE,
			MOVE_BACK_BLUE,
			GET_OUT_BLUE,

			GO_TO_YELLOW,
			OPEN_MULTIFONCTION_YELLOW,
			RUSH_YELLOW,
			CLOSE_PUSHER_YELLOW,
			AVANCE_YELLOW,
			ENABLE_MAGIC_YELLOW,
			COMPUTE_LITTLE_MOVE_BACK_YELLOW,
			LITTLE_MOVE_BACK_YELLOW,
			LITTLE_MOVE_BACK_ERROR_YELLOW,
			MOVE_BACK_YELLOW,
			GET_OUT_YELLOW,

			ERROR,
			DONE
		);

#define TIMEOUT_COLOR	(4000)  // Temps au dela duquel on arrête de tourner le module, on a échoué a détecté la couleur
	static time32_t time_timeout = 0;
	Uint8 pos_get_in;
	Uint8 passage;

	static Uint8 nb_cylinder_in_basis = 0;
	static Sint16 tryx, tryy;

	switch(state){
		case INIT:
			state = GET_IN;
			break;

		case GET_IN:
			state = check_sub_action_result(sub_anne_return_modules_side_get_in(modules), state, COMPUTE, ERROR);
			break;

		case COMPUTE:
			if(((modules == OUR_ELEMENT)&&(global.color == BLUE)) || ((modules == ADV_ELEMENT)&&(global.color == YELLOW))){
				state = GO_TO_BLUE;
			}else if(((modules == ADV_ELEMENT)&&(global.color == BLUE)) || ((modules == OUR_ELEMENT)&&(global.color == YELLOW))){
				state = GO_TO_YELLOW;
			}else
				state = ERROR;
			break;

		case GO_TO_BLUE:
			state = try_going(750, 300, state, OPEN_MULTIFONCTION_BLUE, GET_OUT_BLUE, FAST, FORWARD, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

		case OPEN_MULTIFONCTION_BLUE:
			if(entrance){
				ACT_push_order(ACT_SMALL_CYLINDER_MULTIFONCTION, ACT_SMALL_CYLINDER_MULTIFONCTION_PUSH);
			}
			state = check_act_status(ACT_QUEUE_Small_cylinder_multifonction, state, RUSH_BLUE, RUSH_BLUE);
#warning 'je vois pas quoi mettre en cas d\'erreur'
			break;

		case RUSH_BLUE:
			/*if(entrance){
				ACT_push_order(ACT_SMALL_CYLINDER_MULTIFONCTION, ACT_SMALL_CYLINDER_MULTIFONCTION_PUSH);
			}*/
			state = try_rush(1450, 300, state, CLOSE_PUSHER_BLUE, MOVE_BACK_BLUE, BACKWARD, NO_DODGE_AND_WAIT, TRUE);
			if(ON_LEAVE()){
				nb_cylinder_in_basis = (1200 - global.pos.x)/100;
			}
			break;

		case CLOSE_PUSHER_BLUE:
			if(entrance){
				ACT_push_order(ACT_SMALL_CYLINDER_MULTIFONCTION, ACT_SMALL_CYLINDER_MULTIFONCTION_IN);
			}
			state = check_act_status(ACT_QUEUE_Small_cylinder_multifonction, state, AVANCE_BLUE, MOVE_BACK_BLUE);
			break;

		case AVANCE_BLUE:
			state = try_going(1450, 300, state, ENABLE_MAGIC_BLUE, MOVE_BACK_BLUE, FAST, BACKWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
#warning 'il faut mettre autre chose que move back pour retourner ce qui est déjà accessible'
			break;

		case ENABLE_MAGIC_BLUE:
			state = check_sub_action_result(sub_act_anne_return_module(), state, COMPUTE_LITTLE_MOVE_BACK_BLUE, MOVE_BACK_BLUE);
#warning 'quelles sont les raisons possibles d\'une erreur ?'
			break;

		case COMPUTE_LITTLE_MOVE_BACK_BLUE:
			if(global.pos.y>(1800 - nb_cylinder_in_basis*100))
				state = LITTLE_MOVE_BACK_BLUE;
			else
				state = MOVE_BACK_BLUE;
			break;

		case LITTLE_MOVE_BACK_BLUE:
			state = try_advance(NULL, entrance, 100, state, ENABLE_MAGIC_BLUE, LITTLE_MOVE_BACK_ERROR_BLUE, FAST, BACKWARD, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

		case LITTLE_MOVE_BACK_ERROR_BLUE:{
			if(entrance){
				Uint8 nb_module_returned = 0;
				nb_module_returned = (1800 - global.pos.x)/70;
				tryy = 1200 - 100*nb_module_returned;
			}
			state = try_going(tryx, 300, state, LITTLE_MOVE_BACK_BLUE, LITTLE_MOVE_BACK_BLUE, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
		}
		break;

		case MOVE_BACK_BLUE:
			state = try_going(700, 300, state, DONE, ERROR, FAST, BACKWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

#warning 'OUPS! j\'ai très mal négocié les get out, faut revoir la fin'

		case GO_TO_YELLOW:
			state = try_going(750, 2700, state, OPEN_MULTIFONCTION_YELLOW, GET_OUT_YELLOW, FAST, BACKWARD, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

		case OPEN_MULTIFONCTION_YELLOW:
			if(entrance){
				ACT_push_order(ACT_SMALL_CYLINDER_MULTIFONCTION, ACT_SMALL_CYLINDER_MULTIFONCTION_PUSH);
			}
			state = check_act_status(ACT_QUEUE_Small_cylinder_multifonction, state, RUSH_BLUE, RUSH_BLUE);
#warning 'je vois pas quoi mettre en cas d\'erreur'
			break;

		case RUSH_YELLOW:
/*			if(entrance){
				ACT_push_order(ACT_SMALL_CYLINDER_MULTIFONCTION, ACT_SMALL_CYLINDER_MULTIFONCTION_PUSH);
			}*/
			state = try_rush(1100, 2700, state, CLOSE_PUSHER_YELLOW, MOVE_BACK_YELLOW, FORWARD, NO_DODGE_AND_WAIT, TRUE);
			if(ON_LEAVE()){
				nb_cylinder_in_basis = (1200 - global.pos.x)/100;
			}
			break;

		case CLOSE_PUSHER_YELLOW:
			if(entrance){
				ACT_push_order(ACT_SMALL_CYLINDER_MULTIFONCTION, ACT_SMALL_CYLINDER_MULTIFONCTION_IN);
			}
			state = check_act_status(ACT_QUEUE_Small_cylinder_multifonction, state, AVANCE_YELLOW, MOVE_BACK_YELLOW);
			break;

		case AVANCE_YELLOW:
			state = try_going(1200, 2700, state, ENABLE_MAGIC_YELLOW, MOVE_BACK_YELLOW, FAST, FORWARD, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
#warning 'il faut mettre autre chose que move back pour retourner ce qui est déjà accessible'
			break;

		case ENABLE_MAGIC_YELLOW:
			state = check_sub_action_result(sub_act_anne_return_module(), state, COMPUTE_LITTLE_MOVE_BACK_YELLOW, MOVE_BACK_YELLOW);
#warning 'quelles sont les raisons possibles d\'une erreur ?'
			break;

		case COMPUTE_LITTLE_MOVE_BACK_YELLOW:
			if(global.pos.y>(1200+nb_cylinder_in_basis*100))
				state = LITTLE_MOVE_BACK_YELLOW;
			else
				state = MOVE_BACK_YELLOW;
			break;

		case LITTLE_MOVE_BACK_YELLOW:
			state = try_advance(NULL, entrance, 100, state, ENABLE_MAGIC_YELLOW, LITTLE_MOVE_BACK_ERROR_YELLOW, FAST, BACKWARD, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

		case LITTLE_MOVE_BACK_ERROR_YELLOW:{
			if(entrance){
				Uint8 nb_module_returned = 0;
				nb_module_returned = (1800 - global.pos.x)/70;
				tryx = 1800 - 70*nb_module_returned;
			}
			state = try_going(tryx, 2700, state, LITTLE_MOVE_BACK_YELLOW, LITTLE_MOVE_BACK_YELLOW, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
		}
		break;

		case MOVE_BACK_YELLOW:
			state = try_going(1000, 2700, state, DONE, ERROR, FAST, BACKWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
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
				debug_printf("default case in sub_anne_return_side\n");
			break;
	}

	return IN_PROGRESS;
}
#endif














//permet de régler la distance entre la base côté et le robot(largeurBase+distance):
//#define DISTANCE_BASE_SIDE_ET_ROBOT	((Uint16) 120+200)

// Fonctions privées
//error_e sub_anne_get_in_our_side_depose_module_centre();
//error_e sub_anne_get_in_middle_adv_side_depose_module_centre();
//error_e sub_anne_get_in_adv_side_depose_module_centre();
//error_e sub_anne_get_in_middle_our_side_depose_module_centre();
/*
Uint8 nbCylindresSurBase = 0;


error_e sub_anne_depose_modules_centre(ELEMENTS_property_e modules, ELEMENTS_side_e robot_side, ELEMENTS_side_match_e basis_side){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_ANNE_DEPOSE_MODULES_CENTRE,
			INIT,
			GET_IN,
			GO_TO_DEPOSE_MODULE_POS,
			GO_TO_DEPOSE_MODULE,

			DOWN_PUSHER,
			UP_PUSHER,

			PUSH_MODULE,
			PUSH_MODULE_RETURN,
			NEXT_DEPOSE_MODULE,
			ERROR,
			DONE
		);

	switch(state){
		case INIT:
			state=GET_IN;
			break;

		case GET_IN:
			state=check_sub_action_result(sub_anne_get_in_depose_modules_centre(modules, basis_side), state, GO_TO_DEPOSE_MODULE_POS, ERROR);
			break;

		case GO_TO_DEPOSE_MODULE_POS:
			if(((modules == OUR_ELEMENT) && (basis_side == OUR_SIDE)) || ((modules == ADV_ELEMENT) && (basis_side == ADV_SIDE))){

			state=check_sub_action_result(sub_anne_get_1_depose_module_centre(), state, GO_TO_DEPOSE_MODULE, ERROR);

			}else if(((modules == OUR_ELEMENT) && (basis_side == ADV_SIDE)) || ((modules == ADV_ELEMENT) && (basis_side == OUR_SIDE))){
				state = check_sub_action_result(sub_anne_get_2_depose_module_centre(), state, GO_TO_DEPOSE_MODULE, ERROR);

			}else if(((modules == NEUTRAL_ELEMENT) && (basis_side == OUR_SIDE)) || ((modules == NEUTRAL_ELEMENT) && (basis_side == ADV_SIDE))){
				state = check_sub_action_result(sub_anne_get_3_depose_module_centre(), state, GO_TO_DEPOSE_MODULE, ERROR);

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
				state = check_sub_action_result(sub_act_anne_mae_dispose_modules(RIGHT, ARG_DISPOSE_ONE_CYLINDER_FOLLOW_BY_ANOTHER), state, DOWN_PUSHER_RIGHT, ERROR);
			}
			else{
				state = check_sub_action_result(sub_act_anne_mae_dispose_modules(LEFT, ARG_DISPOSE_ONE_CYLINDER_FOLLOW_BY_ANOTHER), state, DOWN_PUSHER_LEFT, ERROR);
			}

			// ajouter le module sur le tableau
			//STOCKS_addModule(moduleType_e type, moduleStockLocation_e storage)
			break;

		case DOWN_PUSHER: // on sort le pusher
			if (entrance){
							ACT_push_order(ACT_CYLINDER_PUSHER,  ACT_CYLINDER_PUSHER_OUT);
						}
					state= check_act_status(ACT_QUEUE_Small_Cylinder_pusher_left, state, PUSH_MODULE, ERROR);
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
					state = try_going(1320, COLOR_Y(1300), state, UP_PUSHER_LEFT, ERROR, FAST, FORWARD, DODGE_AND_WAIT, END_AT_LAST_POINT);
				}else{
					state = try_going(1320, COLOR_Y(1300), state, UP_PUSHER_RIGHT, ERROR, FAST, FORWARD, DODGE_AND_WAIT, END_AT_LAST_POINT);
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
					state = try_going(1200, COLOR_Y(1300), state, NEXT_DEPOSE_MODULE_LEFT, ERROR, FAST, BACKWARD, DODGE_AND_WAIT, END_AT_LAST_POINT);
				}else{
					state = try_going(1200, COLOR_Y(1300), state, NEXT_DEPOSE_MODULE_RIGHT, ERROR, FAST, BACKWARD, DODGE_AND_WAIT, END_AT_LAST_POINT);
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
			//regarde combien de module on été posé si + de 6 partir (flag depose pleine)
			//else regarde si le stock à entre des modules; si oui GO_TO_DEPOSE_MODULE et complé +1
			//else if 6 module déposé partir (autre zone/quitter sub)
			//else if plus de module à déposé (flag nb deposé), partir
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
				debug_printf("default case in sub_anne_depose_modules_centre\n");
			break;
	}
	return IN_PROGRESS;
}


error_e sub_anne_get_in_depose_modules_centre(ELEMENTS_property_e modules, ELEMENTS_side_match_e basis_side){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_ANNE_GET_IN_DEPOSE_MODULES_CENTRE,
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
			state = check_sub_action_result(sub_anne_get_in_pos_1_depose_module_centre(), state, DONE, ERROR);
			break;

		case GET_IN_POS_2:
			state = check_sub_action_result(sub_anne_get_in_pos_2_depose_module_centre(), state, DONE, ERROR);
			break;

		case GET_IN_POS_3:
			state = check_sub_action_result(sub_anne_get_in_pos_3_depose_module_centre(), state, DONE, ERROR);
			break;

		case GET_IN_POS_4:
			state = check_sub_action_result(sub_anne_get_in_pos_4_depose_module_centre(), state, DONE, ERROR);
			break;

		case GET_IN_POS_5:
			state = check_sub_action_result(sub_anne_get_in_pos_5_depose_module_centre(), state, DONE, ERROR);
			break;

		case GET_IN_POS_6:
			state = check_sub_action_result(sub_anne_get_in_pos_6_depose_module_centre(), state, DONE, ERROR);
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


error_e sub_anne_get_in_pos_1_depose_module_centre(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_ANNE_GET_IN_POS_1_DEPOSE_MODULES_CENTRE,
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


error_e sub_anne_get_in_pos_2_depose_module_centre(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_ANNE_GET_IN_POS_2_DEPOSE_MODULES_CENTRE,
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

error_e sub_anne_get_in_pos_3_depose_module_centre(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_ANNE_GET_IN_POS_3_DEPOSE_MODULES_CENTRE,
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
			state = try_going(975, COLOR_Y(1300), state, DONE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT,END_AT_BRAKE);
			break;

		//case GET_IN_FROM_MIDDLE_SQUARE:
		//	state = try_going(1000, COLOR_Y(1250), state, GET_IN_FROM_OUR_SQUARE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT,END_AT_BRAKE);
		//	break;

		case GET_IN_FROM_ADV_SQUARE:
			state = try_going(900, COLOR_Y(2000), state, GET_IN_FROM_OUR_SQUARE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT,END_AT_BRAKE);
			break;

		case PATHFIND:
			state = ASTAR_try_going(975, COLOR_Y(1300), state, DONE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_BRAKE);
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

error_e sub_anne_get_in_pos_4_depose_module_centre(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_ANNE_GET_IN_POS_4_DEPOSE_MODULES_CENTRE,
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

error_e sub_anne_get_in_pos_5_depose_module_centre(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_ANNE_GET_IN_POS_5_DEPOSE_MODULES_CENTRE,
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

error_e sub_anne_get_in_pos_6_depose_module_centre(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_ANNE_GET_IN_POS_6_DEPOSE_MODULES_CENTRE,
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

error_e sub_anne_pos_1_depose_module_centre(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_ANNE_POS_1_DEPOSE_MODULES_CENTRE,
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

error_e sub_anne_pos_2_depose_module_centre(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_ANNE_POS_2_DEPOSE_MODULES_CENTRE,
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

error_e sub_anne_pos_3_depose_module_centre(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_ANNE_POS_3_DEPOSE_MODULES_CENTRE,
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
			state = try_going(1200, COLOR_Y(1300), state, DONE, ERROR, FAST, FORWARD, NO_DODGE_AND_NO_WAIT,END_AT_BRAKE);
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

error_e sub_anne_pos_4_depose_module_centre(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_ANNE_POS_4_DEPOSE_MODULES_CENTRE,
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

error_e sub_anne_pos_5_depose_module_centre(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_ANNE_POS_5_DEPOSE_MODULES_CENTRE,
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

error_e sub_anne_pos_6_depose_module_centre(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_ANNE_POS_6_DEPOSE_MODULES_CENTRE,
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


error_e sub_anne_depose_modules_side_manager(ELEMENTS_property_e modules,ELEMENTS_side_e robot_side, ELEMENTS_side_match_e basis_side){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_ANNE_DEPOSE_MODULES_SIDE_INTRO,
			INIT,
			ERROR,
			DONE
			);

	switch(state){

	case INIT:
		//state = check_sub_action_result(sub_anne_depose_modules_side_intro(modules, robot_side, bassis_side), state, DONE, ERROR);
		//sub_anne_depose_modules_side_intro(ELEMENTS_property_e modules,ELEMENTS_side_e robot_side, ELEMENTS_side_match_e basis_side);
		if(nbCylindresSurBase<5){

		}

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

error_e sub_anne_depose_modules_side_intro(ELEMENTS_property_e modules,ELEMENTS_side_e robot_side, ELEMENTS_side_match_e basis_side){
	//liste de ce que je dois faire:
		//gérer les actionneurs pour la dépose
		//DETERMINE_NB_CYLINDRE_SUR_BASE_DESCENDRE_BRAS ACT_push_order...
		//VERIFICATION_ACTIONNEUR a faire!!!!
		//faire Procedure de depose


	 * Tu peux créer une autre sub pour tes get_in/déplacements
	 *
	 * Pour voir si tu as un module, tu utilises la fonction STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_ENTRY, param );
	 * Si ça t'intéresse elle est utilisée dans le fichier action_big_act_modules.c dans la sub sub_act_anne_mae_modules()
	 *
	 * Autre commentaire : line 455


	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_ANNE_DEPOSE_MODULES_SIDE_INTRO,
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
			SI_4_CYLINDRES,//A revoir car on ne peut plus tourner les cylindres extérieurs...
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


//Determiner le nombre de cylindre:
			case DETERMINE_NB_CYLINDRE_SUR_BASE_ROTATION:
				//Rotation vers PI:
				state = try_go_angle(PI4096, DETERMINE_NB_CYLINDRE_SUR_BASE_ROTATION, DETERMINE_NB_CYLINDRE_SUR_BASE_DESCENDRE_BRAS, ERROR, FAST, ANY_WAY, END_AT_LAST_POINT);
				break;


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
						//sub_anne_return_modules(LEFT);
						state = try_going(900,DISTANCE_BASE_SIDE_ET_ROBOT,SI_4_CYLINDRES,SI_3_CYLINDRES,\
								ERROR,FAST,BACKWARD,DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
					}
					else{
						//rotation cylindre Droit
						//sub_anne_return_modules(RIGHT);
						state = try_going(850,DISTANCE_BASE_SIDE_ET_ROBOT,SI_4_CYLINDRES,SI_3_CYLINDRES,\
								ERROR,FAST,BACKWARD,DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
					}
				}
				else{
					if(mon_point_de_rotation==SUD){
						//rotation cylindre Droit
						//sub_anne_return_modules(RIGHT);
						state = try_going(900,3000-DISTANCE_BASE_SIDE_ET_ROBOT,SI_4_CYLINDRES,SI_3_CYLINDRES,\
								ERROR,FAST,BACKWARD,DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
					}
					else{
						//rotation cylindre Gauche
						//sub_anne_return_modules(LEFT);
						state = try_going(850,3000-DISTANCE_BASE_SIDE_ET_ROBOT,SI_4_CYLINDRES,SI_3_CYLINDRES,\
								ERROR,FAST,BACKWARD,DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
					}
				}
				break;

			case SI_3_CYLINDRES:
				if(endroit_depose == OUEST){
					if(mon_point_de_rotation==SUD){
						//rotation cylindre Gauche
						//sub_anne_return_modules(LEFT);
						state = try_going(800,DISTANCE_BASE_SIDE_ET_ROBOT,SI_3_CYLINDRES,SI_2_CYLINDRES,\
								ERROR,FAST,BACKWARD,DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
					}
					else{
						//rotation cylindre Droit
						//sub_anne_return_modules(RIGHT);
						state = try_going(950,DISTANCE_BASE_SIDE_ET_ROBOT,SI_3_CYLINDRES,SI_2_CYLINDRES,\
								ERROR,FAST,BACKWARD,DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
					}
				}
				else{
					if(mon_point_de_rotation==SUD){
						//rotation cylindre Droit
						//sub_anne_return_modules(RIGHT);
						state = try_going(800,3000-DISTANCE_BASE_SIDE_ET_ROBOT,SI_3_CYLINDRES,SI_2_CYLINDRES,\
								ERROR,FAST,BACKWARD,DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
					}
					else{
						//rotation cylindre Gauche
						//sub_anne_return_modules(LEFT);
						state = try_going(950,3000-DISTANCE_BASE_SIDE_ET_ROBOT,SI_3_CYLINDRES,SI_2_CYLINDRES,\
								ERROR,FAST,BACKWARD,DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
					}
				}
				break;

			case SI_2_CYLINDRES:
				if(endroit_depose == OUEST){
					if(mon_point_de_rotation==SUD){
						//rotation cylindre Gauche
						//sub_anne_return_modules(LEFT);
						state = try_going(700,DISTANCE_BASE_SIDE_ET_ROBOT,SI_2_CYLINDRES,SI_1_CYLINDRES,\
								ERROR,FAST,BACKWARD,DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
					}
					else{
						//rotation cylindre Droit
						//sub_anne_return_modules(RIGHT);
						state = try_going(1050,DISTANCE_BASE_SIDE_ET_ROBOT,SI_2_CYLINDRES,SI_1_CYLINDRES,\
								ERROR,FAST,BACKWARD,DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
					}
				}
				else{
					if(mon_point_de_rotation==SUD){
						//rotation cylindre Droit
						//sub_anne_return_modules(RIGHT);
						state = try_going(700,3000-DISTANCE_BASE_SIDE_ET_ROBOT,SI_2_CYLINDRES,SI_1_CYLINDRES,\
								ERROR,FAST,BACKWARD,DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
					}
					else{
						//rotation cylindre Gauche
						//sub_anne_return_modules(LEFT);
						state = try_going(1050,3000-DISTANCE_BASE_SIDE_ET_ROBOT,SI_2_CYLINDRES,SI_1_CYLINDRES,\
								ERROR,FAST,BACKWARD,DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
					}
				}
				break;

			case SI_1_CYLINDRES:
				if(endroit_depose == OUEST){
					if(mon_point_de_rotation==SUD){
						//rotation cylindre Gauche
						//sub_anne_return_modules(LEFT);
					}
					else{
						//rotation cylindre Droit
						//sub_anne_return_modules(RIGHT);
					}
				}
				else{
					if(mon_point_de_rotation==SUD){
						//rotation cylindre Droit
						//sub_anne_return_modules(RIGHT);
					}
					else{
						//rotation cylindre Gauche
						//sub_anne_return_modules(LEFT);
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
				state=check_sub_action_result(sub_anne_depose_modules_side(modules,robot_side,basis_side,\
						mon_point_de_rotation,endroit_depose,cote_depose),DEPOSE,CHOIX_DE_COTE_DEPOSE,ERROR);
				break;

			case FIN_DEPOSE:
				state = check_sub_action_result(sub_anne_depose_modules_side_fin(modules,robot_side,basis_side),\
						FIN_DEPOSE,DONE,ERROR);
				break;
	}
	return IN_PROGRESS;
}

error_e sub_anne_depose_modules_side(ELEMENTS_property_e modules,ELEMENTS_side_e robot_side, ELEMENTS_side_match_e basis_side,POINT_DE_ROTATION_config_e mon_point_de_rotation,endroit_depose_config_e endroit_depose,COTE_DE_DEPOSE_config_e cote_depose){
	//====================================================================================================
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_ANNE_DEPOSE_MODULES_SIDE,
				INIT,
				ERROR,
				DONE,
			//Procedure de depose: partie commune à l'ouset et l'est:
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
				state=check_sub_action_result(sub_act_anne_mae_prepare_modules_for_dispose(MODULE_STOCK_RIGHT,\
						TRUE),PREPARATION_NEXT,TAKE_MODULE,ERROR);
			}
			else{
				state=check_sub_action_result(sub_act_anne_mae_prepare_modules_for_dispose(MODULE_STOCK_LEFT,\
						TRUE),PREPARATION_NEXT,TAKE_MODULE,ERROR);
			}
			break;


		case TAKE_MODULE:
			if(cote_depose == DROITE){
				state=check_sub_action_result(sub_act_anne_mae_dispose_modules(MODULE_STOCK_RIGHT,\
						ARG_DISPOSE_ONE_CYLINDER_AND_FINISH),TAKE_MODULE,DECISION,ERROR);
			}
			else{
				state=check_sub_action_result(sub_act_anne_mae_dispose_modules(MODULE_STOCK_LEFT,\
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

error_e sub_anne_depose_modules_side_fin(ELEMENTS_property_e modules,ELEMENTS_side_e robot_side, ELEMENTS_side_match_e basis_side){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_ANNE_DEPOSE_MODULES_SIDE_FIN,
		//Procédure de fin de sub!!!!
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




error_e sub_anne_cylinder_depose_manager(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_ANNE_CYLINDER_DEPOSE_MANAGER,
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
#warning 'CORENTIN : Que fait t on sinon ? On reste planté là ?'
				state = ERROR;
			}
			break;

		case GO_TO_MIDDLE:
#ifdef ADV_SIDE_ON_CENTRAL_MOONBASE
			depose_side = ADV_SIDE;
#else
			depose_side = OUR_SIDE;
#endif
			//state=check_sub_action_result(sub_anne_depose_modules_centre(NEUTRAL_ELEMENT, , depose_side),state,DONE,ERROR);
			state = DONE;
			break;

		case GO_TO_OUR_CENTER:
#ifdef ADV_SIDE_ON_OUR_MOONBASE
			depose_side = ADV_SIDE;
#else
			depose_side = OUR_SIDE;
#endif
			//state=check_sub_action_result(sub_anne_depose_modules_centre(OUR_ELEMENT, , depose_side),state,DONE,ERROR);
			state = DONE;
			break;

		case GO_TO_OUR_SIDE:
			state=check_sub_action_result(sub_anne_depose_modules_side_intro(OUR_ELEMENT,RIGHT, OUR_SIDE),state,DONE,ERROR);
			break;

		case GO_TO_ADV_CENTER:
#ifdef ADV_SIDE_ON_ADV_MOONBASE
			depose_side = ADV_SIDE;
#else
			depose_side = OUR_SIDE;
#endif
			//state=check_sub_action_result(sub_anne_depose_modules_centre(ADV_ELEMENT, , depose_side),state,DONE,ERROR);
			state = DONE;
			break;

		case GO_TO_ADV_SIDE:
			state=check_sub_action_result(sub_anne_depose_modules_side_intro(ADV_ELEMENT,RIGHT, OUR_SIDE),state,DONE,ERROR);
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
				debug_printf("default case in sub_anne_depose_manager\n");
			break;
	}

	return IN_PROGRESS;
}



error_e sub_anne_return_modules(ELEMENTS_side_e side){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_ANNE_RETURN_MODULES_SIDE,
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
				debug_printf("default case in sub_anne_return_modules\n");
			break;
	}

	return IN_PROGRESS;
}


// la place des modules est comptée en partant du centre "Atos"
/*error_e manager_return_modules(){ => Fonction à ne pas utiliser
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_anne_DEPOSE_MODULES_CENTRE,
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

			//state = check_sub_action_result(sub_anne_return_modules(RIGHT,LEFT), state, DONE, ERROR);

		case RETURN_MODULE_DROP_MIDDLE:
			state = check_sub_action_result(sub_anne_return_modules(RIGHT), state, DONE, ERROR);   //choix gauche ou droite
			break;

		case RETURN_MODULE_DROP_OUR_CENTER:
			state = check_sub_action_result(sub_anne_return_modules(RIGHT), state, DONE, ERROR);
			break;

		case RETURN_MODULE_DROP_ADV_CENTER:
			state = check_sub_action_result(sub_anne_return_modules(RIGHT), state, DONE, ERROR);
			break;

		case RETURN_MODULE_DROP_OUR_SIDE:
			state = check_sub_action_result(sub_anne_return_modules(RIGHT), state, DONE, ERROR);
			break;

		case RETURN_MODULE_DROP_ADV_SIDE:
			state = check_sub_action_result(sub_anne_return_modules(RIGHT), state, DONE, ERROR);
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

