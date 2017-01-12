
#include "strat_inutile.h"
#include "../../propulsion/movement.h"
#include "../../QS/QS_stateMachineHelper.h"
#include "../../actuator/act_functions.h"
#include "../../actuator/queue.h"
#include "../../utils/actionChecker.h"
#include "../../propulsion/astar.h"

void thomas_strat_inutile_big(bool_e right_side){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_INUTILE,
			INIT,
			GET_IN_DIRECT,
			GET_IN_AVOID_CRATERE,
			GET_IN_AVOID_START_ZONE,
			GET_IN_FROM_CLOSE_ADV_ZONE,
			GET_IN_FROM_FAR_ADV_ZONE,
			GET_IN_ASTAR,
			INIT_ACTION_SLOPE_LEFT,
			INIT_ACTION_SLOPE_RIGHT,
			INIT_ACTION_ELEVATOR_LEFT,
			INIT_ACTION_ELEVATOR_RIGHT,
			INIT_ACTION_SLIDER_LEFT,
			INIT_ACTION_SLIDER_RIGHT,
			FAILED_INIT_ACTION,
			ACTION_RIGHT_OR_LEFT,
			ACTION_GO_TAKE_CYLINDER,
			ACTION_BRING_BACK_CYLINDER,
			ACTION_STOCK_UP_CYLINDER,

			DONE
		);

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

	switch(state){
		case INIT:

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
			try_going(1200,300, GET_IN_DIRECT , INIT_ACTION_SLOPE_LEFT, ERROR, FAST, ANY_WAY, NO_AVOIDANCE, END_AT_BRAKE);
			break;

		case GET_IN_AVOID_CRATERE:{
			const displacement_t curve_by_our_zone_crat[2] = {{{750, 790}, FAST},
															{{1200, 300}, FAST}};
			state= try_going_multipoint(curve_by_our_zone_crat, 2, GET_IN_AVOID_CRATERE, INIT_ACTION_SLOPE_LEFT, ERROR, ANY_WAY, DODGE_AND_WAIT, END_AT_BRAKE);
		}break;

		case GET_IN_AVOID_START_ZONE:{
			const displacement_t curve_by_our_zone_start[2] = {{{500, 1250}, FAST},
															{{1200, 300}, FAST}};
			state= try_going_multipoint(curve_by_our_zone_start, 2, GET_IN_AVOID_START_ZONE, INIT_ACTION_SLOPE_LEFT, ERROR, ANY_WAY, DODGE_AND_WAIT, END_AT_BRAKE);
		}break;

		case GET_IN_FROM_CLOSE_ADV_ZONE:{
			const displacement_t curve_by_our_zone_advclo[2] = {{{1000, 1725}, FAST},
															{{1200, 300}, FAST}};
			state= try_going_multipoint(curve_by_our_zone_advclo, 2, GET_IN_FROM_CLOSE_ADV_ZONE, INIT_ACTION_SLOPE_LEFT, ERROR, ANY_WAY, DODGE_AND_WAIT, END_AT_BRAKE);
		}break;

		case GET_IN_FROM_FAR_ADV_ZONE:{
			const displacement_t curve_by_our_zone_advfa[2] = {{{900, 2400}, FAST},
															{{1200, 300}, FAST}};
			state= try_going_multipoint(curve_by_our_zone_advfa, 2, GET_IN_FROM_FAR_ADV_ZONE, INIT_ACTION_SLOPE_LEFT, ERROR, ANY_WAY, DODGE_AND_WAIT, END_AT_BRAKE);
		}break;

		case GET_IN_ASTAR:
			state= ASTAR_try_going(1200,300, GET_IN_ASTAR , INIT_ACTION_SLOPE_LEFT, ERROR, FAST, ANY_WAY, NO_AVOIDANCE, END_AT_BRAKE);
			break;

		case INIT_ACTION_SLOPE_LEFT:
			ACT_push_order(ACT_CYLINDER_SLOPE_LEFT, ACT_CYLINDER_SLOPE_LEFT_UNLOCK);
			state= check_act_status(ACT_QUEUE_Cylinder_slope_left, INIT_ACTION_SLOPE_LEFT, INIT_ACTION_SLOPE_RIGHT, FAILED_INIT_ACTION);
			break;

		case INIT_ACTION_SLOPE_RIGHT:
			ACT_push_order(ACT_CYLINDER_SLOPE_RIGHT, ACT_CYLINDER_SLOPE_RIGHT_UNLOCK);
			state= check_act_status(ACT_QUEUE_Cylinder_slope_right, INIT_ACTION_SLOPE_RIGHT, INIT_ACTION_ELEVATOR_LEFT, FAILED_INIT_ACTION);
			break;

		case INIT_ACTION_ELEVATOR_LEFT:
			ACT_push_order(ACT_CYLINDER_ELEVATOR_LEFT, ACT_CYLINDER_ELEVATOR_LEFT_BOTTOM);
			state= check_act_status(ACT_QUEUE_Cylinder_elevator_left, INIT_ACTION_ELEVATOR_LEFT, INIT_ACTION_ELEVATOR_RIGHT, FAILED_INIT_ACTION);
			break;

		case INIT_ACTION_ELEVATOR_RIGHT:
			ACT_push_order(ACT_CYLINDER_ELEVATOR_RIGHT, ACT_CYLINDER_ELEVATOR_RIGHT_UNLOCK);
			state= check_act_status(ACT_QUEUE_Cylinder_elevator_right, INIT_ACTION_ELEVATOR_RIGHT, INIT_ACTION_SLIDER_LEFT, FAILED_INIT_ACTION);
			break;

		case INIT_ACTION_SLIDER_LEFT:
			ACT_push_order(ACT_CYLINDER_SLIDER_LEFT, ACT_CYLINDER_SLIDER_LEFT_IN);
			state= check_act_status(ACT_QUEUE_Cylinder_slider_left, INIT_ACTION_SLIDER_LEFT, INIT_ACTION_SLIDER_RIGHT, FAILED_INIT_ACTION);
			break;

		case INIT_ACTION_SLIDER_RIGHT:
			ACT_push_order(ACT_CYLINDER_SLIDER_RIGHT, ACT_CYLINDER_SLIDER_RIGHT_LOCK);
			state= check_act_status(ACT_QUEUE_Cylinder_slider_right, INIT_ACTION_SLIDER_RIGHT, ACTION_RIGHT_OR_LEFT, FAILED_INIT_ACTION);
			break;

		case ACTION_RIGHT_OR_LEFT:
			//on definit une variable globale pour les tags et les positions en fonction de si on veut le prendre de la droite ou de la gauche
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
			}

			ACT_push_order(Slinder_tag, Slinder_in);



			break;
		case FAILED_INIT_ACTION:
			//stop?
			break;
		case DONE:
			break;
	}
}

void thomas_strat_inutile_small(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_ANNE_INUTILE,
			INIT,
			ACTION,
			ACTION2,
			ACTION3,
			ERROR,
			DONE
		);

	switch(state){
		case INIT:
			state = ASTAR_try_going(1500, COLOR_Y(2400), state , ACTION, ERROR, FAST, ANY_WAY, NO_AVOIDANCE, END_AT_BRAKE);
			break;

		case ACTION:
			state = ASTAR_try_going(500, COLOR_Y(1500), state , ACTION2, ERROR, FAST, ANY_WAY, NO_AVOIDANCE, END_AT_BRAKE);
			break;

		case ACTION2:
			state = ASTAR_try_going(1500, COLOR_Y(600), state , ACTION3, ERROR, FAST, ANY_WAY, NO_AVOIDANCE, END_AT_BRAKE);
			break;

		case ACTION3:
			state = ASTAR_try_going(500, COLOR_Y(2400), state , DONE, ERROR, FAST, ANY_WAY, NO_AVOIDANCE, END_AT_BRAKE);
			break;

		case ERROR:
			break;

		case DONE:
			break;
	}
}
