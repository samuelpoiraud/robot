#include "action_big.h"
#include "../../propulsion/movement.h"
#include "../../propulsion/astar.h"
#include "../../QS/QS_stateMachineHelper.h"

#include  "../../propulsion/astar.h"
#include  "../../utils/generic_functions.h"
#include "../../actuator/act_functions.h"
#include "../../actuator/queue.h"
#include "../../utils/actionChecker.h"

error_e sub_harry_prise_modules_centre(ELEMENTS_property_e modules){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_MODULE_CENTER,
			INIT,
			ERROR,
			DONE
		);

	switch(state){
		case INIT:
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




error_e sub_harry_rocket_monocolor(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_ROCKET_MONOCOLOR,
			INIT,
			GET_IN,
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
			state=GET_IN;
			break;

		case GET_IN:
			state=check_sub_action_result(sub_harry_get_in_rocket_monocolor(),state,TAKE_ROCKET,ERROR);
			break;

		case TAKE_ROCKET:
			//state=check_sub_action_result(sub_harry_take_rocket(),state,GET_OUT,GET_OUT_ERROR);
			break;

		case GET_OUT:
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
			return NOT_HANDLED;
			break;

		case DONE:
			RESET_MAE();
			return END_OK;
			break;
	}

	return IN_PROGRESS;
}


error_e sub_harry_get_in_rocket_monocolor(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_ROCKET_MONOCOLOR,
			INIT,
			GET_IN_MIDDLE_SQUARE,
			GET_IN_ADV_SQUARE,
			GET_IN_OUR_SQUARE,
			PATHFIND,
			ERROR,
			DONE
		);

	switch(state){
		case INIT:
			if(i_am_in_square_color(800, 1400, 300, 900)){
				state = DONE;//GET_IN_OUR_SQUARE;
			}else if (i_am_in_square_color(200, 1100, 900, 2100)){
				state = GET_IN_MIDDLE_SQUARE;
			}else if (i_am_in_square_color(800, 1400, 2100, 2700)){
				state = GET_IN_ADV_SQUARE;
			}else
				state = PATHFIND;

				break;

		case GET_IN_OUR_SQUARE:
			state = try_going(800, COLOR_Y(1000), state, DONE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_BRAKE);
			break;

		case GET_IN_MIDDLE_SQUARE:
			state = try_going(275, COLOR_Y(1150), state, DONE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_BRAKE);
			break;

		case GET_IN_ADV_SQUARE:
			state = try_going(800, COLOR_Y(2000), state, GET_IN_MIDDLE_SQUARE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_BRAKE);
			break;

		case PATHFIND:
			state = ASTAR_try_going(275, COLOR_Y(1150), state, DONE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_BRAKE);
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

//#if 0
error_e sub_harry_fusee_multicolor(ELEMENTS_property_e fusee, bool_e right_side){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_PRISE_ROCKET_MULTICOLOR,
				INIT,

				ALL_THE_GET_IN,
				GET_IN,
				GET_IN_DIRECT,
				GET_IN_AVOID_CRATERE,
				GET_IN_AVOID_START_ZONE,
				GET_IN_FROM_CLOSE_ADV_ZONE,
				GET_IN_FROM_FAR_ADV_ZONE,
				GET_IN_ASTAR,

				FAILED_INIT_ACTION,
				ACTION_GO_TAKE_CYLINDER,
				CHECK_STATUS_OTHER_SLINDER,
				CHECK_STATUS_ELEVATOR,
				CHECK_STATUS_SLOPE,
				ACTION_BRING_BACK_CYLINDER,
				ACTION_BRING_UP_CYLINDER,
				ACTION_STOCK_UP_CYLINDER,
				ACTION_RETRACT_ALL,

				ERROR,
				DONE
			);

		Uint8 nombre_cylindres_restants = 5;

		/*
		ACT_sid_e Slope_tag;
		queue_id_e Slope_tag_Queue;
		ACT_order_e Slope_unlock;
		ACT_order_e Slope_lock;

		ACT_sid_e Elevator_tag;
		queue_id_e Elevator_tag_Queue;
		ACT_order_e Elevator_bottom;
		ACT_order_e Elevator_top;

		ACT_sid_e Slinder_tag;
		queue_id_e Slinder_tag_Queue;
		ACT_order_e Slinder_in;
		ACT_order_e Slinder_out;

		//on adapte si veut le prendre de la droite ou de la gauche en premier
		if (right_side){
						Slope_tag = ACT_CYLINDER_SLOPE_RIGHT;
						Slope_tag_Queue = ACT_QUEUE_Cylinder_slope_right;
						Slope_unlock = ACT_CYLINDER_SLOPE_RIGHT_UNLOCK;
						Slope_lock = ACT_CYLINDER_SLOPE_RIGHT_LOCK;

						Elevator_tag = ACT_CYLINDER_ELEVATOR_RIGHT;
						Elevator_tag_Queue = ACT_QUEUE_Cylinder_elevator_right;
						Elevator_bottom = ACT_CYLINDER_ELEVATOR_RIGHT_UNLOCK;
						Elevator_top = ACT_CYLINDER_ELEVATOR_RIGHT_LOCK;

						Slinder_tag = ACT_CYLINDER_SLIDER_RIGHT;
						Slinder_tag_Queue = ACT_QUEUE_Cylinder_slider_right;
						Slinder_in = ACT_CYLINDER_SLIDER_RIGHT_LOCK;
						Slinder_out = ACT_CYLINDER_SLIDER_RIGHT_UNLOCK;
					}else{
						Slope_tag = ACT_CYLINDER_SLOPE_LEFT;
						Slope_tag_Queue = ACT_QUEUE_Cylinder_slope_left;
						Slope_unlock = ACT_CYLINDER_SLOPE_LEFT_UNLOCK;
						Slope_lock = ACT_CYLINDER_SLOPE_LEFT_LOCK;

						Elevator_tag = ACT_CYLINDER_ELEVATOR_LEFT;
						Elevator_tag_Queue = ACT_QUEUE_Cylinder_elevator_left;
						Elevator_bottom = ACT_CYLINDER_ELEVATOR_LEFT_BOTTOM;
						Elevator_top = ACT_CYLINDER_ELEVATOR_LEFT_TOP;

						Slinder_tag = ACT_CYLINDER_SLIDER_LEFT;
						Slinder_tag_Queue = ACT_QUEUE_Cylinder_slider_left;
						Slinder_in = ACT_CYLINDER_SLIDER_LEFT_IN;
						Slinder_out = ACT_CYLINDER_SLIDER_LEFT_OUT;
					}*/

	switch(state){

		case INIT:{
				error_e result = init_all_actionneur(); // on appelle une fonction qui verifie la position initiale de tout nos actionneurs
				if(result == END_OK){   state=ALL_THE_GET_IN;
				}else{                  state=FAILED_INIT_ACTION;
				}}break;

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
			state = try_going(1200, 300, GET_IN_DIRECT, ACTION_GO_TAKE_CYLINDER,
					ERROR, FAST, ANY_WAY, NO_AVOIDANCE, END_AT_BRAKE);
			break;

		case GET_IN_AVOID_CRATERE:{
			const displacement_t curve_by_our_zone_crat[2] = {
					{ { 750, 790 }, FAST }, { { 1200, 300 }, FAST } };
			state = try_going_multipoint(curve_by_our_zone_crat, 2,
					GET_IN_AVOID_CRATERE, ACTION_GO_TAKE_CYLINDER, ERROR, ANY_WAY,
					DODGE_AND_WAIT, END_AT_BRAKE);
			}break;

		case GET_IN_AVOID_START_ZONE:{
			const displacement_t curve_by_our_zone_start[2] = { { { 500, 1250 },FAST }, { { 1200, 300 }, FAST } };
			state = try_going_multipoint(curve_by_our_zone_start, 2,
					GET_IN_AVOID_START_ZONE, ACTION_GO_TAKE_CYLINDER, ERROR, ANY_WAY,
					DODGE_AND_WAIT, END_AT_BRAKE);
			}break;

		case GET_IN_FROM_CLOSE_ADV_ZONE:{
			const displacement_t curve_by_our_zone_advclo[2] = { { { 1000, 1725 },FAST }, { { 1200, 300 }, FAST } };
			state = try_going_multipoint(curve_by_our_zone_advclo, 2,
					GET_IN_FROM_CLOSE_ADV_ZONE, ACTION_GO_TAKE_CYLINDER, ERROR,
					ANY_WAY, DODGE_AND_WAIT, END_AT_BRAKE);
			}break;

		case GET_IN_FROM_FAR_ADV_ZONE:{
			const displacement_t curve_by_our_zone_advfa[2] = { { { 900, 2400 },FAST }, { { 1200, 300 }, FAST } };
			state = try_going_multipoint(curve_by_our_zone_advfa, 2,
					GET_IN_FROM_FAR_ADV_ZONE, ACTION_GO_TAKE_CYLINDER, ERROR,
					ANY_WAY, DODGE_AND_WAIT, END_AT_BRAKE);
			}break;

		case GET_IN_ASTAR:
			state = ASTAR_try_going(1200, 300, GET_IN_ASTAR, ACTION_GO_TAKE_CYLINDER,
					ERROR, FAST, ANY_WAY, NO_AVOIDANCE, END_AT_BRAKE);
			break;

		case ACTION_GO_TAKE_CYLINDER:

			if (right_side){
				if (entrance) {
				ACT_push_order( ACT_CYLINDER_SLIDER_RIGHT , ACT_CYLINDER_SLIDER_RIGHT_UNLOCK );
				//ACT_push_order( ACT_CYLINDER_SLIDER_LEFT , ACT_CYLINDER_SLIDER_LEFT_ALMOST_UNLOCK );} On avance l'autre bras pour bloquer la chute générer au mouvement d'apres
				ACT_push_order( ACT_CYLINDER_SLOPE_LEFT , ACT_CYLINDER_SLOPE_LEFT_UNLOCK );// on redescend en meme temps le clapet du cylindre precedent
				ACT_push_order( ACT_CYLINDER_ELEVATOR_LEFT , ACT_CYLINDER_ELEVATOR_LEFT_BOTTOM );}//et le bras du cylindre precedent
				state = check_act_status(ACT_QUEUE_Cylinder_slider_right, ACTION_GO_TAKE_CYLINDER,CHECK_STATUS_OTHER_SLINDER, FAILED_INIT_ACTION);
			}else{
				if (entrance) {
				ACT_push_order( ACT_CYLINDER_SLIDER_LEFT , ACT_CYLINDER_SLIDER_LEFT_OUT );
				//ACT_push_order( ACT_CYLINDER_SLIDER_RIGHT , ACT_CYLINDER_SLIDER_RIGHT_ALMOST_OUT );} On avance l'autre bras pour bloquer la chute générer au mouvement d'apres
				ACT_push_order( ACT_CYLINDER_SLOPE_RIGHT , ACT_CYLINDER_SLOPE_RIGHT_UNLOCK );// on redescend en meme temps le clapet du cylindre precedent
				ACT_push_order( ACT_CYLINDER_ELEVATOR_RIGHT , ACT_CYLINDER_ELEVATOR_RIGHT_LOCK );}//et le bras du cylindre precedent
				state = check_act_status(ACT_QUEUE_Cylinder_slider_left, ACTION_GO_TAKE_CYLINDER,CHECK_STATUS_OTHER_SLINDER, FAILED_INIT_ACTION);
			}
			break;

		case CHECK_STATUS_OTHER_SLINDER:
			if (right_side){
				state = check_act_status(ACT_QUEUE_Cylinder_slider_left, CHECK_STATUS_OTHER_SLINDER,CHECK_STATUS_ELEVATOR, FAILED_INIT_ACTION);
			}else{
				state = check_act_status(ACT_QUEUE_Cylinder_slider_right, CHECK_STATUS_OTHER_SLINDER,CHECK_STATUS_ELEVATOR, FAILED_INIT_ACTION);
			}
			break;

		case CHECK_STATUS_ELEVATOR:
			if (right_side){
				state = check_act_status(ACT_QUEUE_Cylinder_elevator_left, CHECK_STATUS_ELEVATOR,CHECK_STATUS_SLOPE, FAILED_INIT_ACTION);
			}else{
				state = check_act_status(ACT_QUEUE_Cylinder_elevator_right, CHECK_STATUS_ELEVATOR,CHECK_STATUS_SLOPE, FAILED_INIT_ACTION);
			}
			break;

		case CHECK_STATUS_SLOPE:
			if (right_side){
				state = check_act_status(ACT_QUEUE_Cylinder_slope_left, CHECK_STATUS_SLOPE,ACTION_BRING_BACK_CYLINDER, FAILED_INIT_ACTION);
			}else{
				state = check_act_status(ACT_QUEUE_Cylinder_slope_right, CHECK_STATUS_SLOPE,ACTION_BRING_BACK_CYLINDER, FAILED_INIT_ACTION);
			}
			break;

		case ACTION_BRING_BACK_CYLINDER:

			if (right_side){
				if (entrance) {
				ACT_push_order( ACT_CYLINDER_SLIDER_RIGHT , ACT_CYLINDER_SLIDER_RIGHT_LOCK );}
				state = check_act_status(ACT_QUEUE_Cylinder_slider_right, ACTION_BRING_BACK_CYLINDER,ACTION_BRING_UP_CYLINDER, FAILED_INIT_ACTION);
			}else{
				if (entrance) {
				ACT_push_order( ACT_CYLINDER_SLIDER_LEFT , ACT_CYLINDER_SLIDER_LEFT_IN );}
				state = check_act_status(ACT_QUEUE_Cylinder_slider_left, ACTION_BRING_BACK_CYLINDER,ACTION_BRING_UP_CYLINDER, FAILED_INIT_ACTION);
			}break;

		case ACTION_BRING_UP_CYLINDER:

			if (right_side){
				if (entrance) {
				ACT_push_order( ACT_CYLINDER_ELEVATOR_RIGHT , ACT_CYLINDER_ELEVATOR_RIGHT_UNLOCK );}
				state = check_act_status(ACT_QUEUE_Cylinder_elevator_right, ACTION_BRING_UP_CYLINDER,ACTION_STOCK_UP_CYLINDER, FAILED_INIT_ACTION);
			}else{
				if (entrance) {
				ACT_push_order( ACT_CYLINDER_ELEVATOR_LEFT , ACT_CYLINDER_ELEVATOR_LEFT_TOP );}
				state = check_act_status(ACT_QUEUE_Cylinder_elevator_left, ACTION_BRING_UP_CYLINDER,ACTION_STOCK_UP_CYLINDER, FAILED_INIT_ACTION);
			}break;

		case ACTION_STOCK_UP_CYLINDER:

			if (right_side){
				if (entrance) {
				ACT_push_order( ACT_CYLINDER_SLOPE_RIGHT , ACT_CYLINDER_SLOPE_RIGHT_LOCK );}
			}else{
				if (entrance) {
				ACT_push_order( ACT_CYLINDER_SLOPE_LEFT , ACT_CYLINDER_SLOPE_LEFT_LOCK );}}

			if (nombre_cylindres_restants == 0){
				// si il n'y as plus de cylindre on arrete
				if (right_side){
					state = check_act_status(ACT_QUEUE_Cylinder_slope_right, ACTION_STOCK_UP_CYLINDER,ACTION_RETRACT_ALL, FAILED_INIT_ACTION);
				}else{
					state = check_act_status(ACT_QUEUE_Cylinder_slope_left,  ACTION_STOCK_UP_CYLINDER,ACTION_RETRACT_ALL, FAILED_INIT_ACTION);}
			}else{
				// si il reste des cylindres on recommence de l'autre cote
				nombre_cylindres_restants -= 1;
				if (right_side){
					right_side = FALSE;
					state = check_act_status(ACT_QUEUE_Cylinder_elevator_right, ACTION_STOCK_UP_CYLINDER, ACTION_GO_TAKE_CYLINDER, FAILED_INIT_ACTION);
				}else{
					right_side = TRUE;
					state = check_act_status(ACT_QUEUE_Cylinder_elevator_left,  ACTION_STOCK_UP_CYLINDER, ACTION_GO_TAKE_CYLINDER, FAILED_INIT_ACTION);}
			}break;

		case ACTION_RETRACT_ALL:{
			// on reinitialise tout nos actionneurs, car on a un slider, un elevator, et un slope de sortit
			error_e result = init_all_actionneur();
			if(result == END_OK){   state=DONE;
			}else{                  state=FAILED_INIT_ACTION;
			}}break;

		case FAILED_INIT_ACTION:
			//stop?
			break;

		case DONE:
			break;

		case ERROR:
			break;
		}

		return IN_PROGRESS;
}
//#endif

error_e init_all_actionneur(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_INIT_CYLINDER,
			INIT,
			INIT_ACTION_SLOPE_LEFT,
			INIT_ACTION_SLOPE_RIGHT,
			INIT_ACTION_ELEVATOR_LEFT,
			INIT_ACTION_ELEVATOR_RIGHT,
			INIT_ACTION_SLIDER_LEFT,
			INIT_ACTION_SLIDER_RIGHT,
			ERROR,
			DONE
		);

	switch(state){
		case INIT:
			state = INIT_ACTION_SLOPE_LEFT;
			break;


		case INIT_ACTION_SLOPE_LEFT:
			if(entrance){
				ACT_push_order(ACT_CYLINDER_SLOPE_LEFT, ACT_CYLINDER_SLOPE_LEFT_UNLOCK);}
			state= check_act_status(ACT_QUEUE_Cylinder_slope_left, INIT_ACTION_SLOPE_LEFT, INIT_ACTION_SLOPE_RIGHT, ERROR);
			break;

		case INIT_ACTION_SLOPE_RIGHT:
			if(entrance){
				ACT_push_order(ACT_CYLINDER_SLOPE_RIGHT, ACT_CYLINDER_SLOPE_RIGHT_UNLOCK);}
			state= check_act_status(ACT_QUEUE_Cylinder_slope_right, INIT_ACTION_SLOPE_RIGHT, INIT_ACTION_ELEVATOR_LEFT, ERROR);
			break;

		case INIT_ACTION_ELEVATOR_LEFT:
			if(entrance){
			ACT_push_order(ACT_CYLINDER_ELEVATOR_LEFT, ACT_CYLINDER_ELEVATOR_LEFT_BOTTOM);}
			state= check_act_status(ACT_QUEUE_Cylinder_elevator_left, INIT_ACTION_ELEVATOR_LEFT, INIT_ACTION_ELEVATOR_RIGHT, ERROR);
			break;

		case INIT_ACTION_ELEVATOR_RIGHT:
			if(entrance){
			ACT_push_order(ACT_CYLINDER_ELEVATOR_RIGHT, ACT_CYLINDER_ELEVATOR_RIGHT_UNLOCK);}
			state= check_act_status(ACT_QUEUE_Cylinder_elevator_right, INIT_ACTION_ELEVATOR_RIGHT, INIT_ACTION_SLIDER_LEFT, ERROR);
			break;

		case INIT_ACTION_SLIDER_LEFT:
			if(entrance){
			ACT_push_order(ACT_CYLINDER_SLIDER_LEFT, ACT_CYLINDER_SLIDER_LEFT_IN);}
			state= check_act_status(ACT_QUEUE_Cylinder_slider_left, INIT_ACTION_SLIDER_LEFT, INIT_ACTION_SLIDER_RIGHT, ERROR);
			break;

		case INIT_ACTION_SLIDER_RIGHT:
			if(entrance){
			ACT_push_order(ACT_CYLINDER_SLIDER_RIGHT, ACT_CYLINDER_SLIDER_RIGHT_LOCK);}
			state= check_act_status(ACT_QUEUE_Cylinder_slider_right, INIT_ACTION_SLIDER_RIGHT, DONE, ERROR);
			break;

		case DONE:
			RESET_MAE();
			return END_OK;
			break;

		case ERROR:
			RESET_MAE();
			return NOT_HANDLED;
			break;

	}

	return IN_PROGRESS;
}
