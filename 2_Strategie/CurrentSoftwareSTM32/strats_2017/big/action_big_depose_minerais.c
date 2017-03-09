#include "action_big.h"
#include "../actions_both_generic.h"
#include "../../propulsion/movement.h"
#include "../../propulsion/astar.h"
#include "../../QS/QS_stateMachineHelper.h"
#include "../../QS/QS_outputlog.h"
#include "../../QS/QS_types.h"
#include "../../QS/QS_IHM.h"
#include "../../utils/actionChecker.h"
#include "../../utils/generic_functions.h"
#include "../../actuator/act_functions.h"
#include "../../avoidance.h"


error_e sub_harry_manager_put_off_ore(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_ORE_DEPOSE_MANAGER,
			INIT,
			MODULE_IN_WAY_TO_FIRST_POS,
			MODULE_IN_WAY_TO_SECOND_POS,
			FIRST_POS,
			SECOND_POS,
			ERROR_FIRST_POS,
			ERROR_SECOND_POS,
			ERROR,
			DONE
		);


	switch(state){
		case INIT:
			//regarde ou est le mechant si bloque je fait l'autre
			if(IHM_switchs_get(SWITCH_DISABLE_ORE)){
				state = ERROR; // L'actionneur minerais a été désactivé
			}else{
				if((foe_in_square(TRUE, 0, 0, COLOR_Y(360), COLOR_Y(360), FOE_TYPE_ALL)==TRUE)&&(foe_in_square(TRUE, 400, 850, COLOR_Y(30), COLOR_Y(600), FOE_TYPE_ALL)==FALSE)){		// il y a notre autre robot qui bloque le cassier depuis la zone de départ
					// on ne pourra pas tiré dans le panier !
					state = ERROR; //le robot ne peut pas tirer de balles , il tire quand même ?
				}
				else if(foe_in_square(TRUE, 400, 850, COLOR_Y(30), COLOR_Y(600), FOE_TYPE_ALL)==FALSE){	 // il n'y a pas un adv dans la zone
					state = MODULE_IN_WAY_TO_FIRST_POS;
				}
				else if(foe_in_square(TRUE, 300, 800, COLOR_Y(800), COLOR_Y(1320), FOE_TYPE_ALL)==FALSE){		// rien ne bloque le tire alternatif
					state = MODULE_IN_WAY_TO_SECOND_POS;
				}
				else{
					state = ERROR; //tire impossible pour le moment !
				}
			}
			break;

		case MODULE_IN_WAY_TO_FIRST_POS:
			if(!ELEMENTS_get_flag(FLAG_OUR_UNICOLOR_NORTH_IS_TAKEN)){
				state = check_sub_action_result(sub_harry_prise_module_unicolor_north(NO_SIDE),state, FIRST_POS, ERROR_FIRST_POS);
			}
			else{
				state=FIRST_POS;
			}
			break;

		case FIRST_POS:
				state=check_sub_action_result(sub_harry_depose_minerais(),state, DONE, ERROR_FIRST_POS);
			break;

		case MODULE_IN_WAY_TO_SECOND_POS:
			if(!ELEMENTS_get_flag(FLAG_OUR_MULTICOLOR_START_IS_TAKEN)){
				state = check_sub_action_result(sub_harry_prise_module_unicolor_north(NO_SIDE),state, SECOND_POS, ERROR_SECOND_POS);
			}
			else{
				state=SECOND_POS;
			}
			break;

		case SECOND_POS:
			state=check_sub_action_result(sub_harry_depose_minerais_alternative(),state, DONE, ERROR_SECOND_POS);
			break;

		case ERROR_FIRST_POS:	//si je n'ai pas reussi à tirer en normal
			if(foe_in_square(TRUE, 300, 800, COLOR_Y(800), COLOR_Y(1320), FOE_TYPE_ALL)==FALSE){
				state = SECOND_POS;
			}else{
				state = ERROR;
			}
			break;

		case ERROR_SECOND_POS: //si je n'ai pas reussi à tirer en alternatif
			if(foe_in_square(TRUE, 400, 850, COLOR_Y(30), COLOR_Y(600), FOE_TYPE_ALL)==FALSE){
				state = FIRST_POS;
			}else{
				state = ERROR;
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
				debug_printf("default case in sub_harry_manager_put_off_ore\n");
			break;
	}

	return IN_PROGRESS;
}



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

	Sint16 correction_x = 0;

	switch(state){
		case INIT:
			if(IHM_switchs_get(SWITCH_DISABLE_ORE)){
				state = ERROR; // L'actionneur minerais a été désactivé
#warning CYLINDER_SOUTH_UNI correspond au cylindre du côté opposé à la zone de départ, il n empêche pas la dépose des minerais\
	peut-être vaut il mieux checker le FLAG_SUB_ANNE_TAKE_CYLINDER_NORTH_UNI en cas de dépose alternative
			}else if(ELEMENTS_get_flag(FLAG_SUB_ANNE_TAKE_CYLINDER_SOUTH_UNI)||(ELEMENTS_get_flag(FLAG_SUB_ANNE_DEPOSE_CYLINDER_OUR_SIDE)||(!ELEMENTS_get_flag(FLAG_HARRY_STOMACH_IS_FULL)))){
				state=ERROR;
			}else{
				state=GET_IN;
				ELEMENTS_set_flag(FLAG_SUB_HARRY_ORE_SHOOTING,TRUE);
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
			// Ca vaudrait peut être le coup de faire un action_recalage ?
			state = check_sub_action_result(action_recalage_x(BACKWARD, 0,  382 - BIG_CALIBRATION_BACKWARD_BORDER_DISTANCE, FALSE, &correction_x, TRUE), state, MOVE_BACK_SHOOTING_POS, GET_OUT_ERROR);
			//state=try_rush(0,COLOR_Y(300),state,MOVE_BACK_SHOOTING_POS,GET_OUT_ERROR,BACKWARD,NO_DODGE_AND_WAIT,TRUE);
			break;

		case MOVE_BACK_SHOOTING_POS:
			if(ELEMENTS_get_flag(FLAG_HARRY_STOMACH_IS_FULL)){
				state=try_advance(NULL, entrance, 100, state, SHOOTING, RUSH_TO_CLEAT, FAST, FORWARD, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			}else{ // On a déjà tirer des balles donc on doit juste faire le GET_OUT
				state=try_advance(NULL, entrance, 100, state, GET_OUT, RUSH_TO_CLEAT, FAST, FORWARD, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			}
			break;

		case SHOOTING:
			//tu tires !!!
			//state = check_sub_action_result(sub_harry_shooting_depose_minerais(), GET_OUT, GET_OUT_ERROR);
			state=GET_OUT;
			if(ON_LEAVING(SHOOTING) && state == GET_OUT){ // En cas de succès
				ELEMENTS_set_flag(FLAG_HARRY_STOMACH_IS_FULL, FALSE); // on baisse le flag car on a plus de balles
			}
			break;

		case GET_OUT:
			state=try_going(850,COLOR_Y(400),state,DONE,RUSH_TO_CLEAT,FAST,FORWARD,NO_DODGE_AND_WAIT,END_AT_BRAKE);
			break;

		case GET_OUT_ERROR:
			state=try_going(850,COLOR_Y(400),state,ERROR,RUSH_TO_CLEAT,FAST,ANY_WAY,NO_DODGE_AND_WAIT,END_AT_BRAKE);
			break;

		case ERROR:
			RESET_MAE();
			on_turning_point();
			ELEMENTS_set_flag(FLAG_SUB_HARRY_ORE_SHOOTING,FALSE);
			return NOT_HANDLED;
			break;

		case DONE:
			RESET_MAE();
			on_turning_point();
			ELEMENTS_set_flag(FLAG_SUB_HARRY_ORE_SHOOTING,FALSE);
			return END_OK;
			break;

		default:
			if(entrance)
				debug_printf("default case in sub_harry_depose_minerais\n");
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
			state=try_going(850,COLOR_Y(400),state,DONE,ERROR,FAST,ANY_WAY,NO_DODGE_AND_WAIT,END_AT_BRAKE);
			break;

		case GET_IN_MIDDLE_SQUARE:
			state=try_going(1000,COLOR_Y(1000),state,GET_IN_FROM_OUR_SQUARE,ERROR,FAST,ANY_WAY,NO_DODGE_AND_WAIT,END_AT_BRAKE);
			break;

		case GET_IN_FROM_ADV_SQUARE:
			state=try_going(1000,COLOR_Y(2000),state,GET_IN_MIDDLE_SQUARE,ERROR,FAST,ANY_WAY,NO_DODGE_AND_WAIT,END_AT_BRAKE);
			break;

		case PATHFIND:
			state=ASTAR_try_going(650,COLOR_Y(300),state,DONE,ERROR,FAST,ANY_WAY,NO_DODGE_AND_WAIT,END_AT_BRAKE);
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
				debug_printf("default case in sub_harry_get_in_depose_minerais\n");
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
				debug_printf("default case in sub_harry_shooting_depose_minerais\n");
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
			GET_OUT_ERROR2,
			ERROR,
			DONE
		);

	switch(state){
		case INIT:
			if(IHM_switchs_get(SWITCH_DISABLE_ORE)){
				state = ERROR; // L'actionneur minerais a été désactivé
			}else{
				state=GET_IN;
				ELEMENTS_set_flag(FLAG_SUB_HARRY_ORE_SHOOTING,TRUE);
			}
			break;

		case GET_IN:
			state=check_sub_action_result(sub_harry_get_in_depose_minerais_alternative(),state,GO_TO_SHOOTING_POS,ERROR);
			break;

		case GO_TO_SHOOTING_POS:
			state=try_going(500,COLOR_Y(1000),state,TURN_TO_SHOOTING_POS,GET_OUT_ERROR,FAST,BACKWARD,NO_DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
			break;

		case TURN_TO_SHOOTING_POS:
			state=try_go_angle(COLOR_ANGLE(55*PI4096/180),state,SHOOTING,GET_OUT_ERROR,FAST,ANY_WAY,END_AT_LAST_POINT);
			break;

		case SHOOTING:
			//tu tires !!!
			//state = check_sub_action_result(sub_harry_shooting_depose_minerais(), GET_OUT, GET_OUT_ERROR);
			state=GET_OUT;
			if(ON_LEAVING(SHOOTING) && state == GET_OUT){ // En cas de succès
				ELEMENTS_set_flag(FLAG_HARRY_STOMACH_IS_FULL, FALSE); // on baisse le flag car on a plus de balles
			}
			break;

		case GET_OUT:
			state=try_going(600,COLOR_Y(1150),state,DONE,GET_OUT2,FAST,ANY_WAY,NO_DODGE_AND_WAIT,END_AT_BRAKE);
			break;

		case GET_OUT2:
			state=try_going(270,COLOR_Y(1000),state,GET_OUT,GET_OUT,FAST,ANY_WAY,NO_DODGE_AND_WAIT,END_AT_BRAKE);
			break;

		case GET_OUT_ERROR:
			state=try_going(600,COLOR_Y(1150),state,ERROR,GET_OUT_ERROR2,FAST,ANY_WAY,NO_DODGE_AND_WAIT,END_AT_BRAKE);
			break;

		case GET_OUT_ERROR2:
			state=try_going(270,COLOR_Y(1000),state,GET_OUT_ERROR,GET_OUT_ERROR,FAST,ANY_WAY,NO_DODGE_AND_WAIT,END_AT_BRAKE);
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
				debug_printf("default case in sub_harry_depose_minerais_alternative\n");
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

		default:
			if(entrance)
				debug_printf("default case in sub_harry_get_in_depose_minerais_alternative\n");
			break;
	}

	return IN_PROGRESS;
}

