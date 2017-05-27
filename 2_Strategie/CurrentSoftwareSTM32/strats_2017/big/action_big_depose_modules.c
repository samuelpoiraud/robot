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
#define FIRST_DISPOSE_CENTRE 0
#define SECOND_DISPOSE_CENTRE 1


typedef enum{
	POS_1,
	POS_2,
	POS_3,
	POS_4,
	POS_5,
	POS_6,
	POS_SIDE,
	POS_ORES,
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
				DEPOSE_CENTRE,
				COMPUTE_SIDE,
				DEPOSE_ORES,
				SUCCESS_DEPOSE,
				ERROR_DEPOSE,
				DEPOSE_SIDE,
				SECONDE_DEPOSE_SIDE,
				VERIFY_STOCK_EMPTY,
				ERROR,
				DONE
			);

	//Emplacement de dépose
	static Uint8 dispose_place = NO_POS;



	static moduleStockLocation_e robot_side = MODULE_STOCK_LEFT;


	//V2
	static Uint8 number_of_dispose_middle = 0;
	static way_e way = BACKWARD;

	enum zone_dispose_e{
		DISPOSE_MIDDLE,
		DISPOSE_SIDE,
		DISPOSE_ORES
	};

	static Uint8 zone_dispose= DISPOSE_MIDDLE;

	switch(state){
		case INIT:
			zone_dispose = DISPOSE_MIDDLE;
			state = MODULE_CHOOSE_POSITION;
			break;

		//Choix de l'emplacement exact de la dépose
		case MODULE_CHOOSE_POSITION:

			//Choix de la POS 1 si on fait une dépose au centre
			//On dit de commencer par le cas où il va en BACKWARD pour ramasser les modules
			if(zone_dispose == DISPOSE_MIDDLE){
				if(number_of_dispose_middle == FIRST_DISPOSE_CENTRE){
					debug_printf("FIRST DISPOSE\n");
					way = BACKWARD;
					dispose_place = POS_1;
					if(global.color == YELLOW){
						robot_side = MODULE_STOCK_LEFT;
					}else {
						robot_side = MODULE_STOCK_RIGHT;
					}
					number_of_dispose_middle++;
					state=ACTION_DISPOSE;
				}else if(number_of_dispose_middle == SECOND_DISPOSE_CENTRE){
					debug_printf("SECOND DISPOSE\n");
					way = FORWARD;
					dispose_place = POS_1;
					if(global.color == YELLOW){
						robot_side = MODULE_STOCK_RIGHT;
					}else {
						robot_side = MODULE_STOCK_LEFT;
					}
					number_of_dispose_middle++;
					state = ACTION_DISPOSE;
				}else{
					state=DONE;
				}

			//Choix pour la zone de dépose sur le côté
			}else if(zone_dispose == DISPOSE_SIDE){
				dispose_place = POS_SIDE;
				state = ACTION_DISPOSE;

			}else{
				dispose_place = POS_ORES;
				state = ACTION_DISPOSE;
			}
			break;


		case ACTION_DISPOSE:
			if(dispose_place == POS_1){
				state = check_sub_action_result(sub_harry_depose_modules_centre(dispose_place, robot_side, way), state, SUCCESS_DEPOSE, ERROR_DEPOSE); // On force SUCESS_DEPOSE même en cas d'erreur pour faire une dépose side.
			}else if(dispose_place == POS_2 && ELEMENTS_get_flag(FLAG_ANNE_GIVE_AUTORISATION_TO_HARRY_TO_DISPOSE_MIDDLE)== TRUE){
				ELEMENTS_set_flag(FLAG_HARRY_ACCEPT_AND_EXPLOIT_THE_AUTORISATION_TO_DIPOSE_MIDDLE, TRUE);
				state = DEPOSE_CENTRE;
			}else if(dispose_place == POS_SIDE){
				state = COMPUTE_SIDE;
			}else if(dispose_place == POS_ORES){
				state = DEPOSE_ORES;
			}else{
				state = DONE;
			}
			break;

		case DEPOSE_CENTRE:
			state = check_sub_action_result(sub_harry_depose_modules_centre(dispose_place, robot_side, FORWARD), state, SUCCESS_DEPOSE, ERROR_DEPOSE); // On force SUCESS_DEPOSE même en cas d'erreur pour faire une dépose side.
			break;


		//Permet de vérifier si on peut accéder à la zone de Anne
		case COMPUTE_SIDE:
			if(ELEMENTS_get_flag(FLAG_ANNE_IS_GETTING_TO_OUR_SIDE_TO_DISPOSE) == FALSE){
				ELEMENTS_set_flag(FLAG_HARRY_IS_GETTING_TO_OUR_SIDE_TO_DISPOSE, TRUE);
				state = DEPOSE_SIDE;
			}else{
				if(STOCKS_getNbModules(MODULE_STOCK_LEFT) > 0 || STOCKS_getNbModules(MODULE_STOCK_RIGHT) > 0){
					dispose_place = POS_2;
					state = ACTION_DISPOSE;
				}
				else{
					state = DEPOSE_ORES;
				}
			}
			break;

		case SUCCESS_DEPOSE:
			if(number_of_dispose_middle == 2){
				zone_dispose = DISPOSE_SIDE;
			}
			state = MODULE_CHOOSE_POSITION;
			break;

		case ERROR_DEPOSE:
			zone_dispose = DISPOSE_SIDE;
			state = MODULE_CHOOSE_POSITION;
			break;

		case DEPOSE_SIDE:
			if(entrance){
				if(STOCKS_getNbModules(MODULE_STOCK_RIGHT) <= STOCKS_getNbModules(MODULE_STOCK_LEFT)){
					robot_side = MODULE_STOCK_LEFT;
				}
				else {
					robot_side = MODULE_STOCK_RIGHT;
				}
			}
			state = check_sub_action_result(sub_harry_depose_modules_side(robot_side, OUR_SIDE), state, SECONDE_DEPOSE_SIDE, ERROR);
			if(ON_LEAVE()){
				zone_dispose = DISPOSE_ORES;
			}
			break;

		case SECONDE_DEPOSE_SIDE:
			if(entrance){
				if(STOCKS_getNbModules(MODULE_STOCK_RIGHT) <= STOCKS_getNbModules(MODULE_STOCK_LEFT)){
					robot_side = MODULE_STOCK_LEFT;
				}
				else {
					robot_side = MODULE_STOCK_RIGHT;
				}
			}
			if(robot_side == MODULE_STOCK_LEFT && MOONBASES_getNbModules(MODULE_MOONBASE_OUR_SIDE) < 3){
				state = check_sub_action_result(sub_harry_depose_modules_side(LEFT, OUR_SIDE), state, MODULE_CHOOSE_POSITION, ERROR);
			}else if(robot_side == MODULE_STOCK_RIGHT && MOONBASES_getNbModules(MODULE_MOONBASE_OUR_SIDE) < 3){
				state = check_sub_action_result(sub_harry_depose_modules_side(RIGHT, OUR_SIDE), state, MODULE_CHOOSE_POSITION, ERROR);
			}else{
				state = MODULE_CHOOSE_POSITION;
			}
			break;


		case DEPOSE_ORES:
			state = check_sub_action_result(sub_harry_depose_minerais_zone(), DEPOSE_ORES, DONE, ERROR);
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


error_e sub_harry_depose_modules_centre(Uint8 drop_place, moduleStockLocation_e robot_side, way_e way){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_DEPOSE_MODULES_CENTRE,
			INIT,
			GET_IN,
			GO_TO_DEPOSE_MODULE_POS,
			GO_TO_DEPOSE_MODULE,
			DEPOSE_MODULE,

			TIGHT_BACKWARD,
			DOWN_PUSHER,
			ERROR_DOWN_PUSHER,
			UP_PUSHER_RIGHT,
			UP_PUSHER_LEFT,
			ERROR_UP_PUSHER,

			PUSH_MODULE,
			PUSH_MODULE_RETURN,
			NEXT_DEPOSE_MODULE_LEFT,
			NEXT_DEPOSE_MODULE_RIGHT,
			GET_OUT,
			GET_OUT_WITH_ERROR,
			FINISH_GET_OUT_POS_1,
			GET_OUT_ERROR,
			ERROR,
			DONE
		);

	static bool_e flag_error;
	static Uint8 nb_modules_at_entrance;
	CAN_msg_t can_msg;
	static bool_e filter_activate = FALSE;

	error_e ret;
	ret = IN_PROGRESS;

	if(ELEMENTS_get_flag(FLAG_ACTIVATE_FILTER_ANNE_DEPOSE_SIDE) && !filter_activate){
		can_msg.sid = PROP_TRANSPARENCY;
		can_msg.size = SIZE_PROP_TRANSPARENCY;
		can_msg.data.prop_transparency.enable = TRUE;
		can_msg.data.prop_transparency.number = DETECTION_FILTER_ZONE_BIG_IGNORE_SMALL;
		CAN_send(&can_msg);

		filter_activate = TRUE;
	}


	//Appel de la MAE parralèle cratère pendant la prise
	mae_harry_take_ore_during_dispose_module(ORE_DURING_DISPOSE_NO_EVENT);

	switch(state){
		case INIT:
			flag_error = FALSE;
			filter_activate = FALSE;
			sub_act_harry_mae_prepare_modules_for_dispose(robot_side, TRUE);
			state = GET_IN;
			break;

		case GET_IN:
			state = check_sub_action_result(sub_harry_get_in_depose_modules_centre(drop_place, way), state, DOWN_PUSHER, ERROR);
			break;

		case GO_TO_DEPOSE_MODULE:
			if((robot_side == MODULE_STOCK_LEFT && !STOCKS_isEmpty(MODULE_STOCK_LEFT))||(robot_side == MODULE_STOCK_RIGHT && !STOCKS_isEmpty(MODULE_STOCK_RIGHT))){
				if(way == FORWARD){
					state = DEPOSE_MODULE;
				}
				else {
					state = DOWN_PUSHER;
				}
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

				nb_modules_at_entrance = STOCKS_getNbModules(robot_side);
			}
			if(robot_side == MODULE_STOCK_LEFT){
				state = check_sub_action_result(sub_act_harry_mae_dispose_modules(MODULE_STOCK_LEFT, ((STOCKS_getNbModules(robot_side) > 1 ) ? ARG_DISPOSE_ONE_CYLINDER_FOLLOW_BY_ANOTHER :ARG_DISPOSE_ONE_CYLINDER_AND_FINISH)), state, TIGHT_BACKWARD, GET_OUT_WITH_ERROR);
			}
			else if(robot_side == MODULE_STOCK_RIGHT){
				state = check_sub_action_result(sub_act_harry_mae_dispose_modules(MODULE_STOCK_RIGHT, ((STOCKS_getNbModules(robot_side) > 1 ) ? ARG_DISPOSE_ONE_CYLINDER_FOLLOW_BY_ANOTHER :ARG_DISPOSE_ONE_CYLINDER_AND_FINISH)), state, TIGHT_BACKWARD, GET_OUT_WITH_ERROR);
			}
			else{
				state = DONE;
			}
			if(ON_LEAVE()){
				// En cas d'erreur mais qu'on a quand même enlevé un cylindre du stock du robot, on considère que c'est bon
				// Typiquement, le module du balancer en préparation sort du robot en même temps que le module précédent
				if(state == GET_OUT_WITH_ERROR){
					error_printf("nbModules before = %d, after = %d\n", nb_modules_at_entrance, STOCKS_getNbModules(robot_side));
					if(STOCKS_getNbModules(robot_side) < nb_modules_at_entrance){
						state = TIGHT_BACKWARD;
					}
				}
				if(state == TIGHT_BACKWARD){
					//J'ai mis polychrome parce que je vois pas trop comment le gérer sinon.
					if(drop_place == POS_1 || drop_place == POS_2){MOONBASES_addModule(MODULE_POLY, MODULE_MOONBASE_OUR_CENTER);}
					else if(drop_place == POS_3 || drop_place == POS_4){MOONBASES_addModule(MODULE_POLY, MODULE_MOONBASE_MIDDLE);}
					else if(drop_place == POS_5 || drop_place == POS_6){MOONBASES_addModule(MODULE_POLY, MODULE_MOONBASE_ADV_CENTER);}
				}
				/*if(ELEMENTS_get_flag(FLAG_PUSHER_IS_BLOCKING)){
					state = ERROR_DOWN_PUSHER;
					ELEMENTS_set_flag(FLAG_PUSHER_IS_BLOCKING,FALSE);
				}*/
			}
			break;

		case TIGHT_BACKWARD:
			if(way == FORWARD){
				state = try_advance(NULL, entrance, 15, state, (way == FORWARD)?DOWN_PUSHER:PUSH_MODULE, GET_OUT_WITH_ERROR, FAST, (way==FORWARD)?BACKWARD:FORWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			} else {
				state = PUSH_MODULE_RETURN;
			}
			break;

		case DOWN_PUSHER: // on sort le pusher
			if (entrance){
				if(robot_side == MODULE_STOCK_RIGHT){
					ACT_push_order(ACT_CYLINDER_PUSHER_RIGHT,  ACT_CYLINDER_PUSHER_RIGHT_OUT);
				}else{
					ACT_push_order(ACT_CYLINDER_PUSHER_LEFT,  ACT_CYLINDER_PUSHER_LEFT_OUT);
				}



				if(way == BACKWARD)
					mae_harry_take_ore_during_dispose_module(ORE_DURING_DISPOSE_YOU_CAN_TAKE_NOW);
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
			state = try_advance(NULL, entrance, 150, state, DOWN_PUSHER, GET_OUT_WITH_ERROR, FAST, (way==FORWARD)?BACKWARD:FORWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;


		case PUSH_MODULE:
			if(drop_place == POS_1){
				// pos 1
				if(way == FORWARD){
					if(global.color == BLUE){
						state = try_rush((MOONBASES_getNbModules(MODULE_MOONBASE_OUR_CENTER)>3)?(1715-(MOONBASES_getNbModules()-3)*7):(1715),\
								COLOR_Y((MOONBASES_getNbModules(MODULE_MOONBASE_OUR_CENTER)>3)?(865-(MOONBASES_getNbModules()-3)*7):(865)), state, UP_PUSHER_LEFT, ERROR, FORWARD, DODGE_AND_WAIT, FALSE);
					}else{
						state = try_rush((MOONBASES_getNbModules(MODULE_MOONBASE_OUR_CENTER)>3)?(1715-(MOONBASES_getNbModules()-3)*7):(1715),\
									COLOR_Y((MOONBASES_getNbModules(MODULE_MOONBASE_OUR_CENTER)>3)?(865-(MOONBASES_getNbModules()-3)*7):(865)), state, UP_PUSHER_RIGHT, ERROR, FORWARD, DODGE_AND_WAIT, FALSE);
					}
				}
				else{
					if(global.color == BLUE){
						state = try_rush((MOONBASES_getNbModules(MODULE_MOONBASE_OUR_CENTER)>3)?(1690-(MOONBASES_getNbModules()-3)*7):(1690),\
								COLOR_Y((MOONBASES_getNbModules(MODULE_MOONBASE_OUR_CENTER)>3)?(830-(MOONBASES_getNbModules()-3)*7):(830)), state, UP_PUSHER_LEFT, ERROR, FORWARD, DODGE_AND_WAIT, FALSE);
					}else{
						state = try_rush((MOONBASES_getNbModules(MODULE_MOONBASE_OUR_CENTER)>3)?(1690-(MOONBASES_getNbModules()-3)*7):(1690),\
									COLOR_Y((MOONBASES_getNbModules(MODULE_MOONBASE_OUR_CENTER)>3)?(830-(MOONBASES_getNbModules()-3)*7):(830)), state, UP_PUSHER_RIGHT, ERROR, FORWARD, DODGE_AND_WAIT, FALSE);
					}
				}
			}else if(drop_place == POS_2){

				// pos 2
				if(global.color == BLUE){
					state = try_rush(1379, COLOR_Y(1233), state, UP_PUSHER_RIGHT,ERROR, FORWARD, DODGE_AND_WAIT, FALSE);
				}else{
					state = try_rush(1379, COLOR_Y(1233), state, UP_PUSHER_LEFT, ERROR, FORWARD, DODGE_AND_WAIT, FALSE);
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

					state = try_rush(1379, COLOR_Y(1767), state, UP_PUSHER_LEFT, ERROR, FORWARD, DODGE_AND_WAIT, FALSE);
				}else{
					state = try_rush(1379, COLOR_Y(1767), state, UP_PUSHER_RIGHT, ERROR, FORWARD, DODGE_AND_WAIT, FALSE);
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
			state = check_act_status(ACT_QUEUE_Cylinder_pusher_right, state, PUSH_MODULE_RETURN, ERROR_UP_PUSHER);
			break;

		case UP_PUSHER_LEFT:
			if (entrance){
				ACT_push_order(ACT_CYLINDER_PUSHER_LEFT,  ACT_CYLINDER_PUSHER_LEFT_IN);
			}
			state = check_act_status(ACT_QUEUE_Cylinder_pusher_left, state, (way==FORWARD)?PUSH_MODULE_RETURN:DEPOSE_MODULE, ERROR_UP_PUSHER);
			break;


		case ERROR_UP_PUSHER: // on sort le pusher
		if (entrance){
			if(robot_side == MODULE_STOCK_RIGHT){
				ACT_push_order(ACT_CYLINDER_PUSHER_RIGHT,  ACT_CYLINDER_PUSHER_RIGHT_OUT);
			}else{
				ACT_push_order(ACT_CYLINDER_PUSHER_LEFT,  ACT_CYLINDER_PUSHER_LEFT_OUT);
			}
		}
		if(robot_side == MODULE_STOCK_RIGHT){
			state= check_act_status(ACT_QUEUE_Cylinder_pusher_right, state, UP_PUSHER_RIGHT, UP_PUSHER_RIGHT);
		}else{
			state= check_act_status(ACT_QUEUE_Cylinder_pusher_left, state, UP_PUSHER_LEFT, UP_PUSHER_LEFT);
		}
		break;

		case PUSH_MODULE_RETURN:
			if(drop_place == POS_1){
				// pos 1
				if(way == FORWARD){
					if(global.color == BLUE){
						state = try_going(1595, COLOR_Y(745), state, NEXT_DEPOSE_MODULE_LEFT, GET_OUT_WITH_ERROR, FAST, BACKWARD, DODGE_AND_WAIT, END_AT_LAST_POINT);
					}else{
						state = try_going(1595, COLOR_Y(745), state, NEXT_DEPOSE_MODULE_RIGHT, GET_OUT_WITH_ERROR, FAST, BACKWARD, DODGE_AND_WAIT, END_AT_LAST_POINT);
					}
				}
				else{
					if(global.color == BLUE){
						state = try_going(1540, COLOR_Y(692), state, NEXT_DEPOSE_MODULE_RIGHT, GET_OUT_WITH_ERROR, FAST, FORWARD, DODGE_AND_WAIT, END_AT_LAST_POINT);
					}else{
						state = try_going(1540, COLOR_Y(692), state, NEXT_DEPOSE_MODULE_LEFT, GET_OUT_WITH_ERROR, FAST, FORWARD, DODGE_AND_WAIT, END_AT_LAST_POINT);
					}
				}
			}else if(drop_place == POS_2){

				// pos 2
				if(global.color == BLUE){
					state = try_going(1270, COLOR_Y(1100), state, NEXT_DEPOSE_MODULE_RIGHT, GET_OUT_WITH_ERROR, FAST, BACKWARD, DODGE_AND_WAIT, END_AT_LAST_POINT);
				}else{
					state = try_going(1270, COLOR_Y(1100), state, NEXT_DEPOSE_MODULE_LEFT, GET_OUT_WITH_ERROR, FAST, BACKWARD, DODGE_AND_WAIT, END_AT_LAST_POINT);
				}
			}else if(drop_place == POS_3){

				 // pos 3
				if(global.color == BLUE){
					state = try_going(1200, COLOR_Y(1250), state, NEXT_DEPOSE_MODULE_LEFT, GET_OUT_WITH_ERROR, FAST, BACKWARD, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
				}else{
					state = try_going(1200, COLOR_Y(1250), state, NEXT_DEPOSE_MODULE_RIGHT, GET_OUT_WITH_ERROR, FAST, BACKWARD, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
				}
			}else if(drop_place == POS_4){
				// pos 4
				if(global.color == BLUE){
					state = try_going(1200, COLOR_Y(1700), state, NEXT_DEPOSE_MODULE_RIGHT, GET_OUT_WITH_ERROR, FAST, BACKWARD, DODGE_AND_WAIT, END_AT_LAST_POINT);
				}else{
					state = try_going(1200, COLOR_Y(1700), state, NEXT_DEPOSE_MODULE_LEFT, GET_OUT_WITH_ERROR, FAST, BACKWARD, DODGE_AND_WAIT, END_AT_LAST_POINT);
				}
			}else if(drop_place == POS_5){
				// pos 5
				if(global.color == BLUE){
					state = try_going(1270, COLOR_Y(1900), state, NEXT_DEPOSE_MODULE_LEFT, GET_OUT_WITH_ERROR, FAST, BACKWARD, DODGE_AND_WAIT, END_AT_LAST_POINT);
				}else{
					state = try_going(1270, COLOR_Y(1900), state, NEXT_DEPOSE_MODULE_RIGHT, GET_OUT_WITH_ERROR, FAST, BACKWARD, DODGE_AND_WAIT, END_AT_LAST_POINT);
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
				state = try_going(1460, COLOR_Y(610), state, DONE, GET_OUT_ERROR, FAST, (way==FORWARD)?BACKWARD:FORWARD, NO_DODGE_AND_WAIT, END_AT_BRAKE);
			}else if(drop_place == POS_2){
				state = try_going(1105, COLOR_Y(972), state , DONE, GET_OUT_ERROR, FAST, (way==FORWARD)?BACKWARD:FORWARD, NO_DODGE_AND_WAIT, END_AT_BRAKE);
			}
			else{
				state = try_advance(NULL, entrance, 250, state, DONE, GET_OUT_ERROR, FAST, BACKWARD, NO_DODGE_AND_WAIT, END_AT_BRAKE);
			}
			if(ON_LEAVE())
				ACT_push_order(ACT_CYLINDER_PUSHER_LEFT, ACT_CYLINDER_PUSHER_LEFT_IDLE);
				ACT_push_order(ACT_CYLINDER_PUSHER_RIGHT, ACT_CYLINDER_PUSHER_RIGHT_IDLE);
				ELEMENTS_set_flag(FLAG_HARRY_GIVE_AUTORISATION_TO_ANNE_TO_PROTECT_OUR_MIDDLE, TRUE);
			break;

		case GET_OUT_WITH_ERROR:
			flag_error = TRUE;
			state = GET_OUT;
			break;

		case FINISH_GET_OUT_POS_1:
			state = try_going(1170, COLOR_Y(720), state, DONE, DONE, FAST, (way==FORWARD)?BACKWARD:FORWARD, NO_DODGE_AND_WAIT, END_AT_BRAKE);
			break;

		case GET_OUT_ERROR:
			if(drop_place == POS_1){
				state = try_rush(1610, COLOR_Y(760), state, GET_OUT, GET_OUT, (way==FORWARD)?FORWARD:BACKWARD, DODGE_AND_WAIT, FALSE);
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
			// Envoi de la libération de la zone de transparence
			can_msg.sid = PROP_TRANSPARENCY;
			can_msg.size = SIZE_PROP_TRANSPARENCY;
			can_msg.data.prop_transparency.enable = FALSE;
			can_msg.data.prop_transparency.number = DETECTION_FILTER_ZONE_BIG_IGNORE_SMALL;
			CAN_send(&can_msg);

			RESET_MAE();
			on_turning_point();
			ret = NOT_HANDLED;
			break;

		case DONE:
			if(flag_error){
				state = ERROR;
			}
			else{
				// Envoi de la libération de la zone de transparence
				can_msg.sid = PROP_TRANSPARENCY;
				can_msg.size = SIZE_PROP_TRANSPARENCY;
				can_msg.data.prop_transparency.enable = FALSE;
				can_msg.data.prop_transparency.number = DETECTION_FILTER_ZONE_BIG_IGNORE_SMALL;
				CAN_send(&can_msg);

				RESET_MAE();
				on_turning_point();
				ret = END_OK;
			}
			break;

		default:
			if(entrance)
				debug_printf("default case in sub_harry_depose_modules_centre\n");
			break;
	}

	if(ret != IN_PROGRESS)
		mae_harry_take_ore_during_dispose_module(ORE_DURING_DISPOSE_YOU_MUST_STOP_NOW);
	return ret;
}


error_e sub_harry_get_in_depose_modules_centre(Uint8 drop_place, way_e way){
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
			state = check_sub_action_result(sub_harry_get_in_pos_1_depose_module_centre(way), state, DONE, ERROR);
			break;

		case GET_IN_POS_2:
			state = check_sub_action_result(sub_harry_get_in_pos_2_depose_module_centre(), state, DONE, ERROR);
			break;

		case GET_IN_POS_3:
			if(entrance)
			{
				ELEMENTS_set_flag(FLAG_HARRY_ACCEPT_AND_EXPLOIT_THE_AUTORISATION_TO_DIPOSE_MIDDLE, TRUE);
			}
			state = check_sub_action_result(sub_harry_get_in_pos_3_depose_module_centre(), state, DONE, ERROR);
			break;

		case GET_IN_POS_4:
			if(entrance)
			{
				ELEMENTS_set_flag(FLAG_HARRY_ACCEPT_AND_EXPLOIT_THE_AUTORISATION_TO_DIPOSE_MIDDLE, TRUE);
			}
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


error_e sub_harry_get_in_pos_1_depose_module_centre(way_e way){
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
			if(way == FORWARD)
				state = try_going(1600,COLOR_Y(750), state, ROTATE, ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT,END_AT_LAST_POINT);
			else
				state = try_going(1528, COLOR_Y(680), state, ROTATE, ERROR, FAST, BACKWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case ROTATE:{
			Sint16 angle;
			if(way == FORWARD){
				angle = PI4096/4;
			}else{
				angle = -3*PI4096/4;
			}
			if(global.color == YELLOW){
				state = try_go_angle(COLOR_ANGLE(angle), state, DONE, ERROR, FAST, CLOCKWISE, END_AT_LAST_POINT);
			}else{
				state = try_go_angle(COLOR_ANGLE(angle), state, DONE, ERROR, FAST, TRIGOWISE, END_AT_LAST_POINT);
			}
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
			state = try_going(1250,COLOR_Y(1110), state, ROTATE, ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT,END_AT_LAST_POINT);
			break;

		case ROTATE:
			if(global.color == YELLOW){
				state = try_go_angle(COLOR_ANGLE(PI4096/4), state, DONE, ERROR, FAST, CLOCKWISE, END_AT_LAST_POINT);
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
			START_POINT,
			AVANCE,
			ROTATE,
			PATHFIND,
			ERROR,
			DONE
		);

	const displacement_t curve_middle[2] = {(displacement_t){(GEOMETRY_point_t){950, COLOR_Y(1050)}, FAST},
														(displacement_t){(GEOMETRY_point_t){800, COLOR_Y(1700)}, FAST},
											};


	switch(state){
		case INIT:
			if(i_am_in_square_color(800, 1400, 300, 900)){
				state = GET_IN_FROM_OUR_SQUARE;
			}else if(i_am_in_square_color(200, 1100, 900, 2100)){
				state = GET_IN_FROM_MIDDLE_SQUARE;
			}else if(i_am_in_square_color(800, 1400, 2100, 2700)){
				state = START_POINT;
			}else{
				state = PATHFIND;
			}
			break;

		case GET_IN_FROM_OUR_SQUARE:
			state = try_going_multipoint(curve_middle, 3, state, START_POINT, ERROR, ANY_WAY, DODGE_AND_WAIT, END_AT_BRAKE);
			//state = try_going(1100, COLOR_Y(2075), state, DONE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT,END_AT_BRAKE);
			break;

		case GET_IN_FROM_MIDDLE_SQUARE:
			state = try_going(1000, COLOR_Y(2000), state, START_POINT, ERROR, FAST, ANY_WAY, DODGE_AND_WAIT,END_AT_BRAKE);
			break;

		case START_POINT:
			state = try_going(1100, COLOR_Y(2075), state, DONE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT,END_AT_BRAKE);
			break;

		case AVANCE:
			state = try_going(1250,COLOR_Y(1925), state, ROTATE, ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT,END_AT_LAST_POINT);
			break;

		case ROTATE:
			if(global.color == YELLOW){
				state = try_go_angle(COLOR_ANGLE(-PI4096/4), state, DONE, ERROR, FAST, TRIGOWISE, END_AT_LAST_POINT);
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

#warning 'sub depose sur les côtés'
/*
 * sub qui depose 3 modules
 * argument côté table et côté robot
 * logique de sub calquée sur anne
 */

error_e sub_harry_depose_modules_side(ELEMENTS_side_e robot_side, ELEMENTS_side_match_e side){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_DEPOSE_MODULES_SIDE,
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
			ELOIGNEMENT,
			GET_OUT,
			ELOIGNEMENT_ERROR,
			GET_OUT_ERROR,
			DONE
		);

	// Pousse les modules présent dans la zone en les poussant
	//#define CHECK_ZONE_BY_PUSHING

	static color_e color_side;
	static way_e sens_robot, sens_robot_inv;
	//static int compteur = 0; // abandon si deux echecs de depose car les modules ne sont pas monter dans le robot
	static Uint32 correction_dx; // correction la depose n'est pas au meme niveau entre gauche et droite
	error_e stateAct;
	static moduleMoonbaseLocation_e moonbase;

	switch(state){
		case INIT:
			if(((robot_side == RIGHT)&&(STOCKS_getNbModules(MODULE_STOCK_RIGHT)))||((robot_side == LEFT)&&(STOCKS_getNbModules(MODULE_STOCK_LEFT)))){
				//rien à deposer
				state = ERROR;
			}
			// un cylindre peu bloqué en bas du robot !

			if(side == OUR_SIDE){
				color_side = global.color;
				moonbase = MODULE_MOONBASE_OUR_SIDE;
			}else{
				color_side = (global.color==BLUE)?YELLOW:BLUE;
				moonbase = MODULE_MOONBASE_ADV_SIDE;
			}

			if(color_side == BLUE){
				sens_robot = BACKWARD;
				sens_robot_inv = FORWARD;
				if(i_am_in_square(750, 1300, 200, 1400)){
					state = GET_IN;
				}
				else{
					state = GET_IN_ASTAR;
				}
			}
			else{ // color yellow
				sens_robot = FORWARD;
				sens_robot_inv = BACKWARD;
				if(i_am_in_square(750, 1300, 1600, 2800)){
					state = GET_IN;
				}
				else{
					state = GET_IN_ASTAR;
				}
			}

			if((color_side == BLUE && robot_side == LEFT) || (color_side == YELLOW && robot_side == RIGHT)){
				correction_dx = 100;
			}else{
				correction_dx = 0;
			}
			break;

		case GET_IN:
			state = try_going(1000, COLOR_Y(400), state, CALAGE, GET_IN_ASTAR, FAST, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT); //1000 258
			break;

		case GET_IN_ASTAR:
			state = ASTAR_try_going(1000, COLOR_Y(400), state, CALAGE, ERROR, FAST, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT); //1000 258
			break;

		case CALAGE:
			state = try_rush(1000, COLOR_Y(0), state, POSITION_ANGLE, ERROR, BACKWARD, NO_DODGE_AND_WAIT, TRUE); //1000 0
			break;

		case POSITION_ANGLE:
			// avance et tourne pour longer (350)
#ifdef CHECK_ZONE_BY_PUSHING
			state = try_going(1000, COLOR_Y(350), state, GO_TO_PUSH, ERROR, FAST, FORWARD, DODGE_AND_WAIT, END_AT_LAST_POINT);
#else
			state = try_going(1000, COLOR_Y(350), state, PREPARE_TO_DISPOSE, ERROR, FAST, FORWARD, DODGE_AND_WAIT, END_AT_LAST_POINT);
#endif
			break;

		case GO_TO_PUSH:
			//va au sud avant de remonter /// FORWARK/BACKWARD robot side
			if(robot_side == RIGHT){ // BACKWARD en yellow // FORWARD en blue
				state = try_going(1200 + correction_dx, COLOR_Y(350), state, PREPARE_TO_PUSH, ELOIGNEMENT_ERROR, FAST, sens_robot_inv, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			}
			else{   // FORWARD en yellow // BACKWARD en blue
				state = try_going(1200 + correction_dx, COLOR_Y(350), state, PREPARE_TO_PUSH, ELOIGNEMENT_ERROR, FAST, sens_robot, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			}
			break;

		// pousser les modules avant la dépose
		case PREPARE_TO_PUSH:
			if(entrance){
				if(robot_side == RIGHT){
					ACT_push_order(ACT_CYLINDER_PUSHER_RIGHT,  ACT_CYLINDER_PUSHER_RIGHT_OUT);
				}else{
					ACT_push_order(ACT_CYLINDER_PUSHER_LEFT,  ACT_CYLINDER_PUSHER_LEFT_OUT);
				}
			}
			//descendre l'actionneur
			if(robot_side == RIGHT){
				state= check_act_status(ACT_QUEUE_Cylinder_pusher_right, state, PUSH, PUSHER_UP);
			}else{
				state= check_act_status(ACT_QUEUE_Cylinder_pusher_left, state, PUSH, PUSHER_UP);
			}
			break;

		case PUSH:
			if(robot_side == RIGHT){
				state = try_rush(750 + correction_dx, COLOR_Y(350), state, PUSHER_UP, PUSHER_UP, sens_robot, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			}
			else{
				state = try_rush(750 + correction_dx, COLOR_Y(350), state, PUSHER_UP, PUSHER_UP, sens_robot_inv, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			}
			break;

		// remonte le pusher avant de se rapprocher du côté de terrain
		case PUSHER_UP:
			if(entrance){
				if(robot_side == RIGHT){
					ACT_push_order(ACT_CYLINDER_PUSHER_RIGHT,  ACT_CYLINDER_BALANCER_RIGHT_IN);
				}else{
					ACT_push_order(ACT_CYLINDER_PUSHER_LEFT,  ACT_CYLINDER_BALANCER_LEFT_IN);
				}
			}

			if(robot_side == RIGHT){
				stateAct = check_act_status(ACT_QUEUE_Cylinder_pusher_right, IN_PROGRESS, END_OK, NOT_HANDLED);
			}else{
				stateAct = check_act_status(ACT_QUEUE_Cylinder_pusher_left, IN_PROGRESS, END_OK, NOT_HANDLED);
			}

			 // abandon après avoir pousser
			if(stateAct==NOT_HANDLED){
				state=PUSHER_UP;
			}
			else if(stateAct==END_OK){
				state=PREPARE_TO_DISPOSE;
			}
		break;

		/*
		 *  placement avant la depose
		 */
		case PREPARE_TO_DISPOSE:
			if(robot_side == RIGHT){
				state = try_going(1150, COLOR_Y(310), state, POSITION_DISPOSE_1, ELOIGNEMENT_ERROR, FAST, sens_robot_inv, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			}
			else{
				state = try_going(1150, COLOR_Y(310), state, POSITION_DISPOSE_1, ELOIGNEMENT_ERROR, FAST, sens_robot, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			}
			break;

			/*
			 * se met en position pour poser le premier module
			 */
		case POSITION_DISPOSE_1:
			if(robot_side == RIGHT){ //slow is decalage
				state = try_going(1050 + correction_dx , COLOR_Y(310), state, DISPOSE_1, ELOIGNEMENT, FAST, sens_robot, DODGE_AND_WAIT, END_AT_LAST_POINT);
			}
			else{
				state = try_going(1050 + correction_dx, COLOR_Y(310), state, DISPOSE_1, ELOIGNEMENT, FAST, sens_robot_inv, DODGE_AND_WAIT, END_AT_LAST_POINT);
			}
			break;

		case DISPOSE_1:
			if(robot_side == RIGHT){
				if(STOCKS_getNbModules(MODULE_STOCK_RIGHT) >= 1){
					state = check_sub_action_result(sub_act_harry_mae_dispose_modules(MODULE_STOCK_RIGHT, ARG_DISPOSE_ONE_CYLINDER_FOLLOW_BY_ANOTHER), state, POSITION_DISPOSE_2, POSITION_DISPOSE_2);
				}
				else{
					state = check_sub_action_result(sub_act_harry_mae_dispose_modules(MODULE_STOCK_RIGHT, ARG_DISPOSE_ONE_CYLINDER_FOLLOW_BY_ANOTHER), state, ELOIGNEMENT, ELOIGNEMENT_ERROR);
				}
			}
			else{
				if(STOCKS_getNbModules(MODULE_STOCK_LEFT) >= 1){
					state = check_sub_action_result(sub_act_harry_mae_dispose_modules(MODULE_STOCK_LEFT, ARG_DISPOSE_ONE_CYLINDER_FOLLOW_BY_ANOTHER), state, POSITION_DISPOSE_2, POSITION_DISPOSE_2);
				}
				else{
					state = check_sub_action_result(sub_act_harry_mae_dispose_modules(MODULE_STOCK_LEFT, ARG_DISPOSE_ONE_CYLINDER_FOLLOW_BY_ANOTHER), state, ELOIGNEMENT, ELOIGNEMENT_ERROR);
				}
			}

			if(ON_LEAVE()){
				MOONBASES_addModule(MODULE_POLY, moonbase);
			}
			break;

			/*
			 * se met en position pour le seconde module
			 */
		case POSITION_DISPOSE_2:
			if(robot_side == RIGHT){
				state = try_going(900 + correction_dx, COLOR_Y(310), state, DISPOSE_2, ELOIGNEMENT_ERROR, FAST, sens_robot, DODGE_AND_WAIT, END_AT_LAST_POINT);
			}
			else{
				state = try_going(900 + correction_dx, COLOR_Y(310), state, DISPOSE_2, ELOIGNEMENT_ERROR, FAST, sens_robot_inv, DODGE_AND_WAIT, END_AT_LAST_POINT);
			}
			break;

		case DISPOSE_2:
			if(robot_side == RIGHT){
				if(STOCKS_getNbModules(MODULE_STOCK_RIGHT) >= 1){
					state = check_sub_action_result(sub_act_harry_mae_dispose_modules(MODULE_STOCK_RIGHT, ARG_DISPOSE_ONE_CYLINDER_FOLLOW_BY_ANOTHER), state, POSITION_DISPOSE_3, POSITION_DISPOSE_3);
				}
				else{
					state = check_sub_action_result(sub_act_harry_mae_dispose_modules(MODULE_STOCK_RIGHT, ARG_DISPOSE_ONE_CYLINDER_FOLLOW_BY_ANOTHER), state, ELOIGNEMENT, ELOIGNEMENT_ERROR);
				}
			}
			else{
				if(STOCKS_getNbModules(MODULE_STOCK_LEFT) >= 1){
					state = check_sub_action_result(sub_act_harry_mae_dispose_modules(MODULE_STOCK_LEFT, ARG_DISPOSE_ONE_CYLINDER_FOLLOW_BY_ANOTHER), state, POSITION_DISPOSE_3, POSITION_DISPOSE_3);
				}
				else{
					state = check_sub_action_result(sub_act_harry_mae_dispose_modules(MODULE_STOCK_LEFT, ARG_DISPOSE_ONE_CYLINDER_FOLLOW_BY_ANOTHER), state, ELOIGNEMENT, ELOIGNEMENT_ERROR);
				}
			}
			if(ON_LEAVE()){
				MOONBASES_addModule(MODULE_POLY, moonbase);
			}
			break;

			/*
			 * se met en position pour le troisieme module
			 */
		case POSITION_DISPOSE_3:
			if(robot_side == RIGHT){
				state = try_going(750 + correction_dx, COLOR_Y(310), state, DISPOSE_3, ELOIGNEMENT_ERROR, FAST, sens_robot, DODGE_AND_WAIT, END_AT_LAST_POINT);
			}
			else{
				state = try_going(750 + correction_dx, COLOR_Y(310), state, DISPOSE_3, ELOIGNEMENT_ERROR, FAST, sens_robot_inv, DODGE_AND_WAIT, END_AT_LAST_POINT);
			}
			break;

		case DISPOSE_3:
			if(robot_side == RIGHT){
				state = check_sub_action_result(sub_act_harry_mae_dispose_modules(MODULE_STOCK_RIGHT, ARG_DISPOSE_ONE_CYLINDER_FOLLOW_BY_ANOTHER), state, ELOIGNEMENT, ELOIGNEMENT);
			}
			else{
				state = check_sub_action_result(sub_act_harry_mae_dispose_modules(MODULE_STOCK_LEFT, ARG_DISPOSE_ONE_CYLINDER_FOLLOW_BY_ANOTHER), state, ELOIGNEMENT, ELOIGNEMENT);
			}
			if(ON_LEAVE()){
				MOONBASES_addModule(MODULE_POLY, moonbase);
			}
			break;

		case ELOIGNEMENT:
			//pos de depard ou plus loin
			if(robot_side == RIGHT){
				state = try_going(900 + correction_dx, COLOR_Y(350), state, GET_OUT, GET_OUT, FAST, sens_robot_inv, DODGE_AND_WAIT, END_AT_BRAKE);
			}else{
				state = try_going(900 + correction_dx, COLOR_Y(350), state, GET_OUT, GET_OUT, FAST, sens_robot, DODGE_AND_WAIT, END_AT_BRAKE);
			}
			break;

		case GET_OUT:
			//pos de depard ou plus loin
			if(robot_side == RIGHT){
				state = try_going(1000, COLOR_Y(500), state, DONE, DONE, FAST, sens_robot_inv, DODGE_AND_WAIT, END_AT_LAST_POINT);
			}else{
				state = try_going(1000, COLOR_Y(500), state, DONE, DONE, FAST, sens_robot, DODGE_AND_WAIT, END_AT_LAST_POINT);
			}
			break;

		case ELOIGNEMENT_ERROR:
			//pos de depard ou plus loin
			if(robot_side == RIGHT){
				state = try_going(900 + correction_dx, COLOR_Y(350), state, GET_OUT_ERROR, GET_OUT_ERROR, FAST, sens_robot_inv, DODGE_AND_WAIT, END_AT_BRAKE);
			}else{
				state = try_going(900 + correction_dx, COLOR_Y(350), state, GET_OUT_ERROR, GET_OUT_ERROR, FAST, sens_robot, DODGE_AND_WAIT, END_AT_BRAKE);
			}
			break;

		case GET_OUT_ERROR:
			//pos de depard ou plus loin
			if(robot_side == RIGHT){
				state = try_going(1000, COLOR_Y(500), state, ERROR, ERROR, FAST, sens_robot_inv, DODGE_AND_WAIT, END_AT_LAST_POINT);
			}else{
				state = try_going(1000, COLOR_Y(500), state, ERROR, ERROR, FAST, sens_robot, DODGE_AND_WAIT, END_AT_LAST_POINT);
			}
			break;

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
