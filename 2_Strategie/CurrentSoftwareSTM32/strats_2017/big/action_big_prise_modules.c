#include "action_big.h"
#include "../../propulsion/movement.h"
#include "../../propulsion/astar.h"
#include "../../QS/QS_stateMachineHelper.h"
#include "../../QS/QS_types.h"
#include "../../QS/QS_outputlog.h"
#include "../../QS/QS_IHM.h"
#include  "../../utils/generic_functions.h"
#include "../../actuator/act_functions.h"
#include "../../actuator/queue.h"
#include "../../utils/actionChecker.h"
#include "../../elements.h"
#include "../../high_level_strat.h"




error_e sub_harry_prise_modules_manager(const get_this_module_s list_modules[], Uint8 modules_nb){ //Passer un tableau avec les modules choisis et leur nombre
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_MODULES_MANAGER,
			INIT,
			CHOOSE_MODULE,
			CHECK_SIDE_ACT,
			CHOOSE_ACTION,
			TAKE_MODULE,
			GET_MODULE_START_ZONE,
			GET_MODULE_MID_ZONE,
			GET_MODULE_SIDE_ZONE,
			GET_MODULE_SOUTH_ZONE,
			GET_MODULE_NORTH_ZONE,
			OUR_MODULES,
			ADV_MODULES,
			GET_OUR_MODULES,
			GET_ADV_MODULES,
			ERROR,
			DONE
		);

	static ELEMENTS_property_e module = OUR_ELEMENT;
	static ELEMENTS_side_e side = NO_SIDE;

	static Uint8 actions_done = 0;


	switch(state){
		case INIT:
			actions_done = 0;
			if(IHM_switchs_get(SWITCH_DISABLE_MODULE_RIGHT) && IHM_switchs_get(SWITCH_DISABLE_MODULE_LEFT)){
				state = ERROR; //Prise des modules impossible
			}
			else{
				state = CHOOSE_MODULE;
			}
			break;

		case CHOOSE_MODULE:		// Choix du module à prendre
			while(actions_done < modules_nb && ELEMENTS_get_flag_module(list_modules[actions_done].numero)){
				actions_done++;
			}

			debug_printf("\nflag FLAG_OUR_MULTICOLOR_START_IS_TAKEN = %d\n", ELEMENTS_get_flag(FLAG_OUR_MULTICOLOR_START_IS_TAKEN));
			debug_printf("flag FLAG_OUR_MULTICOLOR_SIDE_IS_TAKEN = %d\n", ELEMENTS_get_flag(FLAG_OUR_MULTICOLOR_SIDE_IS_TAKEN));
			debug_printf("flag FLAG_OUR_MULTICOLOR_NEAR_DEPOSE_IS_TAKEN = %d\n", ELEMENTS_get_flag(FLAG_OUR_MULTICOLOR_NEAR_DEPOSE_IS_TAKEN));
			debug_printf("flag FLAG_OUR_UNICOLOR_NORTH_IS_TAKEN = %d\n", ELEMENTS_get_flag(FLAG_OUR_UNICOLOR_NORTH_IS_TAKEN));
			debug_printf("flag FLAG_OUR_UNICOLOR_SOUTH_IS_TAKEN = %d\n", ELEMENTS_get_flag(FLAG_OUR_UNICOLOR_SOUTH_IS_TAKEN));
			debug_printf("actions_done = %d\n", actions_done);

			if(actions_done >= modules_nb){
				state = DONE; // On a récupéré tous les modules demandés
			}else{
				state = CHECK_SIDE_ACT;
			}
			break;

		case CHECK_SIDE_ACT:	// Choix du côté de stockage
			if(list_modules[actions_done].side == RIGHT && !IHM_switchs_get(SWITCH_DISABLE_MODULE_RIGHT) && !STOCKS_isFull(RIGHT)){
				side = RIGHT;  // On demande RIGHT et RIGHT est possible
				state = TAKE_MODULE;
			}else if(list_modules[actions_done].side == LEFT && !IHM_switchs_get(SWITCH_DISABLE_MODULE_LEFT) && !STOCKS_isFull(LEFT)){
				side = LEFT;  // On demande LEFT et LEFT est possible
				state = TAKE_MODULE;
			}else if(list_modules[actions_done].side == RIGHT && !IHM_switchs_get(SWITCH_DISABLE_MODULE_LEFT) && !STOCKS_isFull(LEFT)){
				side = LEFT;  // On demande RIGHT, RIGHT n'est pas possible mais LEFT est possible
				state = TAKE_MODULE;
			}else if(list_modules[actions_done].side == LEFT && !IHM_switchs_get(SWITCH_DISABLE_MODULE_RIGHT) && !STOCKS_isFull(RIGHT)){
				side = RIGHT;  // On demande LEFT, LEFT n'est pas possible mais RIGHT est possible
				state = TAKE_MODULE;
			}else{
				state = ERROR; // Les stocks sont pleins, on devra repasser plus tard
			}
			break;

		case TAKE_MODULE:	// Si on arrive ici, c'est qu'on peut prendre le module demandé
			switch(list_modules[actions_done].numero){
				case MODULE_OUR_START:
					module = OUR_ELEMENT;
					state = GET_MODULE_START_ZONE;
					break;

				case MODULE_OUR_SIDE:
					module = OUR_ELEMENT;
					state = GET_MODULE_SIDE_ZONE;
					break;

				case MODULE_OUR_MID:
					module = OUR_ELEMENT;
					state = GET_MODULE_MID_ZONE;
					break;

				case MODULE_ADV_START:
					module = ADV_ELEMENT;
					state = GET_MODULE_START_ZONE;
					break;

				case MODULE_ADV_SIDE:
					module = ADV_ELEMENT;
					state = GET_MODULE_SIDE_ZONE;
					break;

				case MODULE_ADV_MID:
					module = ADV_ELEMENT;
					state = GET_MODULE_MID_ZONE;
					break;

				case MODULE_OUR_ORE_UNI:
					state = GET_MODULE_SOUTH_ZONE;
					break;

				case MODULE_OUR_START_ZONE_UNI:
					state = GET_MODULE_NORTH_ZONE;
					break;

				default:
					state = ERROR;
					break;
			}
			break;


		//à voir si on essaie pas d'attrapper les modules suivants même si on a eu une erreur au préalable.
		case GET_MODULE_START_ZONE:
			state = check_sub_action_result(sub_harry_prise_module_start_centre(module, side), state, CHOOSE_MODULE, CHOOSE_MODULE);
			if(ON_LEAVE()){
				actions_done++;
			}
			break;

		case GET_MODULE_MID_ZONE:
			state = check_sub_action_result(sub_harry_prise_module_base_centre(module, side), state, CHOOSE_MODULE, CHOOSE_MODULE);
			if(ON_LEAVE()){
				actions_done++;
			}
			break;

		case GET_MODULE_SIDE_ZONE:
			state = check_sub_action_result(sub_harry_prise_module_side_centre(module, side), state, CHOOSE_MODULE, CHOOSE_MODULE);
			if(ON_LEAVE()){
				actions_done++;
			}
			break;

		case GET_MODULE_NORTH_ZONE:
			state = check_sub_action_result(sub_harry_prise_module_unicolor_north(side), state, CHOOSE_MODULE, CHOOSE_MODULE);
			if(ON_LEAVE()){
				actions_done++;
			}
			break;

		case GET_MODULE_SOUTH_ZONE:
			state = check_sub_action_result(sub_harry_prise_module_unicolor_south(side), state, CHOOSE_MODULE, CHOOSE_MODULE);
			if(ON_LEAVE()){
				actions_done++;
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
				debug_printf("default case in sub_harry_prise_modules_manager\n");
			break;
	}

	return IN_PROGRESS;

}



error_e sub_harry_prise_module_start_centre(ELEMENTS_property_e modules, ELEMENTS_side_e side){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_MODULE_START_CENTER,
			INIT,
			CHECK_FIRST_ELEMENT,
			GO_TO_START_POINT_UP,
			GO_TO_START_POINT_SIDE,
			GO_TO_START_POINT_DOWN,
			GET_IN_CLOSE_ADV_ZONE,
			GET_IN_FAR_ADV_ZONE,
			GET_IN_ASTAR,
			TAKE_FIRST_MODULE_UP,
			TAKE_FIRST_MODULE_SIDE,
			TAKE_FIRST_MODULE_DOWN,

			STORAGE_LEFT,
			STORAGE_RIGHT,
			STOCK_LEFT,
			STOCK_RIGHT,

			ERROR,
			DONE
		);

		const displacement_t curve_far_zone_yellow[2] = {(displacement_t){(GEOMETRY_point_t){1000, 2400}, FAST},
													     (displacement_t){(GEOMETRY_point_t){950, 2000}, FAST},
													    };

		const displacement_t curve_far_zone_blue[2] = {(displacement_t){(GEOMETRY_point_t){1000, 600}, FAST},
													   (displacement_t){(GEOMETRY_point_t){950, 1000}, FAST},
													  };

		switch(state){
			case INIT:
				if (ELEMENTS_get_flag(FLAG_OUR_MULTICOLOR_START_IS_TAKEN)){
					state = DONE; // Il n'y a plus rien à faire
				}
				else if(ELEMENTS_get_flag(FLAG_SUB_ANNE_TAKE_CYLINDER_OUR_ROCKET_UNI)) // Si Anne est déjà en train de faire cette action
				{
					state = ERROR;
				}
				else
				{
					// Si on prend les modules du coté BLUE		(utiliser i_am_in_square et pas i_am_in_square_color)
					if((global.color == BLUE && modules == OUR_ELEMENT) || (global.color == YELLOW && modules == ADV_ELEMENT)){
						if(i_am_in_square(0, 550, 800, 1200)){
							state = CHECK_FIRST_ELEMENT;
						}else if(i_am_in_square(0, 1000, 1200, 2100)){
							state = GO_TO_START_POINT_SIDE;
						}else if(i_am_in_square(700,1400,0,800)){
							state = GO_TO_START_POINT_DOWN;
						}else if(i_am_in_square(700, 1400, 2100, 2600)){
							state = GET_IN_CLOSE_ADV_ZONE;
						}else if(i_am_in_square(700, 1400, 2600, 2800)){
							state = GET_IN_FAR_ADV_ZONE;
						}else{
							state = GET_IN_ASTAR;
						}
					}
					else	// Si on prend les modules du coté YELLOW	(utiliser i_am_in_square et pas i_am_in_square_color)
					{
						if(i_am_in_square(0, 550, 1800, 2200)){
							state = CHECK_FIRST_ELEMENT;
						}else if(i_am_in_square(0, 1000, 900, 1800)){
							state = GO_TO_START_POINT_SIDE;
						}else if(i_am_in_square(700,1400,2200,3000)){
							state = GO_TO_START_POINT_DOWN;
						}else if(i_am_in_square(700, 1400, 400, 1100)){
							state = GET_IN_CLOSE_ADV_ZONE;
						}else if(i_am_in_square(700, 1400, 200, 400)){
							state = GET_IN_FAR_ADV_ZONE;
						}else{
							state = GET_IN_ASTAR;
						}
					}

					// On lève le flag de subaction
					ELEMENTS_set_flag(FLAG_SUB_HARRY_TAKE_CYLINDER_OUR_ROCKET_UNI, TRUE);
				}
			break;


			case CHECK_FIRST_ELEMENT:
				if(modules == ADV_ELEMENT){
					state = try_going(350,COLOR_Y(1400), CHECK_FIRST_ELEMENT, GO_TO_START_POINT_UP, ERROR, FAST, ANY_WAY, DODGE_AND_NO_WAIT, END_AT_BRAKE);
				}
				else{
					state = try_going(global.pos.x+100, global.pos.y, state, GO_TO_START_POINT_UP, ERROR, FAST,ANY_WAY, DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
				}
				break;

			case GET_IN_CLOSE_ADV_ZONE:
				if((global.color == BLUE && modules == OUR_ELEMENT) || (global.color == YELLOW && modules == ADV_ELEMENT)){
					state = try_going(900, 2000, state, GO_TO_START_POINT_SIDE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_BRAKE);
				}else{
					state = try_going(900, 1000, state, GO_TO_START_POINT_SIDE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_BRAKE);
				}
				break;

			case GET_IN_FAR_ADV_ZONE:
				if((global.color == BLUE && modules == OUR_ELEMENT) || (global.color == YELLOW && modules == ADV_ELEMENT)){
					state = try_going_multipoint(curve_far_zone_yellow, 2, state, GO_TO_START_POINT_SIDE, ERROR, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_BRAKE);
				}else{
					state = try_going_multipoint(curve_far_zone_blue, 2, state, GO_TO_START_POINT_SIDE, ERROR, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_BRAKE);
				}
				break;

			case GET_IN_ASTAR:
				if((global.color == BLUE && modules == OUR_ELEMENT) || (global.color == YELLOW && modules == ADV_ELEMENT)){
					state = ASTAR_try_going(600, 1250, state, TAKE_FIRST_MODULE_SIDE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
				}else{
					state = ASTAR_try_going(600, 1750, state, TAKE_FIRST_MODULE_SIDE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
				}
				break;

			case GO_TO_START_POINT_UP:
				if((global.color == BLUE && modules == OUR_ELEMENT) || (global.color == YELLOW && modules == ADV_ELEMENT)){
					state = try_going(350, 950, state, TAKE_FIRST_MODULE_UP, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_BRAKE);
				}else{
					state = try_going(350, 2050, state, TAKE_FIRST_MODULE_UP, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_BRAKE);
				}
				if(ON_LEAVE()){
					if(side == LEFT){
						ACT_push_order( ACT_POMPE_ELEVATOR_LEFT , ACT_POMPE_NORMAL );
						ACT_push_order( ACT_POMPE_SLIDER_LEFT , ACT_POMPE_NORMAL );
					}
					else{
						ACT_push_order( ACT_POMPE_ELEVATOR_RIGHT, ACT_POMPE_NORMAL );
						ACT_push_order( ACT_POMPE_SLIDER_RIGHT , ACT_POMPE_NORMAL );
					}
				}
				break;

			case GO_TO_START_POINT_SIDE:
				if((global.color == BLUE && modules == OUR_ELEMENT) || (global.color == YELLOW && modules == ADV_ELEMENT)){
					state = try_going(650, 1250, state, TAKE_FIRST_MODULE_SIDE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_BRAKE);
				}else{
					state = try_going(650, 1750, state, TAKE_FIRST_MODULE_SIDE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_BRAKE);
				}
				if(ON_LEAVE()){
					if(side == LEFT){
						ACT_push_order( ACT_POMPE_ELEVATOR_LEFT , ACT_POMPE_NORMAL );
						ACT_push_order( ACT_POMPE_SLIDER_LEFT , ACT_POMPE_NORMAL );
					}
					else{
						ACT_push_order( ACT_POMPE_ELEVATOR_RIGHT, ACT_POMPE_NORMAL );
						ACT_push_order( ACT_POMPE_SLIDER_RIGHT , ACT_POMPE_NORMAL );
					}
				}
				break;

			case GO_TO_START_POINT_DOWN:
				if((global.color == BLUE && modules == OUR_ELEMENT) || (global.color == YELLOW && modules == ADV_ELEMENT)){
					state = try_going(1000, 860, state, TAKE_FIRST_MODULE_SIDE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_BRAKE);
				}else{
					state = try_going(1000, 2140, state, TAKE_FIRST_MODULE_SIDE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_BRAKE);
				}
				if(ON_LEAVE()){
					if(side == LEFT){
						ACT_push_order( ACT_POMPE_ELEVATOR_LEFT , ACT_POMPE_NORMAL );
						ACT_push_order( ACT_POMPE_SLIDER_LEFT , ACT_POMPE_NORMAL );
					}
					else{
						ACT_push_order( ACT_POMPE_ELEVATOR_RIGHT, ACT_POMPE_NORMAL );
						ACT_push_order( ACT_POMPE_SLIDER_RIGHT , ACT_POMPE_NORMAL );
					}
				}
				break;

			case TAKE_FIRST_MODULE_UP:
				if(side == LEFT || side == NO_SIDE){
					if((global.color == BLUE && modules == OUR_ELEMENT) || (global.color == YELLOW && modules == ADV_ELEMENT)){
						state = try_going(500, 950, state, STORAGE_LEFT, ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
					}else{
						state = try_going(500, 2050, state, STORAGE_LEFT, ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
					}
				}
				else{
					if((global.color == BLUE && modules == OUR_ELEMENT) || (global.color == YELLOW && modules == ADV_ELEMENT)){
						state = try_going(500, 1025, state, STORAGE_RIGHT, ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
					}else{
						state = try_going(500, 1975, state, STORAGE_RIGHT, ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
					}
				}
				break;

			case TAKE_FIRST_MODULE_SIDE:
				if(side == LEFT || side == NO_SIDE){
					if((global.color == BLUE && modules == OUR_ELEMENT) || (global.color == YELLOW && modules == ADV_ELEMENT)){
						state = try_going(660, 1030, state, STORAGE_LEFT, ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_BRAKE);
					}else{
						state = try_going(660, 1970, state, STORAGE_LEFT, ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_BRAKE);
					}
				}
				else{
					if((global.color == BLUE && modules == OUR_ELEMENT) || (global.color == YELLOW && modules == ADV_ELEMENT)){
						state = try_going(650, 940, state, STORAGE_RIGHT, ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_BRAKE);
					}else{
						state = try_going(650, 2060, state, STORAGE_RIGHT, ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_BRAKE);
					}
				}
				break;

			case TAKE_FIRST_MODULE_DOWN:
				if(side == LEFT || side == NO_SIDE){
					if((global.color == BLUE && modules == OUR_ELEMENT) || (global.color == YELLOW && modules == ADV_ELEMENT)){
						state = try_going(650, 1100, state, STORAGE_LEFT, ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_BRAKE);
					}else{
						state = try_going(650, 1900, state, STORAGE_LEFT, ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_BRAKE);
					}
				}
				else{
					if((global.color == BLUE && modules == OUR_ELEMENT) || (global.color == YELLOW && modules == ADV_ELEMENT)){
						state = try_going(500, 920, state, STORAGE_RIGHT, ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_BRAKE);
					}else{
						state = try_going(500, 2080, state, STORAGE_RIGHT, ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_BRAKE);
					}
				}
				break;


			case STORAGE_LEFT:
				if(entrance){
					if(global.color == BLUE){
						STOCKS_addModule(MODULE_BLUE, STOCK_POS_ELEVATOR,MODULE_STOCK_LEFT);
					}else{
						STOCKS_addModule(MODULE_YELLOW, STOCK_POS_ELEVATOR, MODULE_STOCK_LEFT);
					}
					ACT_push_order( ACT_POMPE_SLIDER_LEFT , ACT_POMPE_STOP );

				}

				state = check_sub_action_result(sub_act_harry_mae_store_modules(MODULE_STOCK_LEFT, TRUE), state, DONE, ERROR);

				if(state == DONE){
					if(modules == OUR_ELEMENT)
						ELEMENTS_set_flag(FLAG_OUR_MULTICOLOR_START_IS_TAKEN, TRUE);
					else
						ELEMENTS_set_flag(FLAG_ADV_MULTICOLOR_START_IS_TAKEN, TRUE);
				}
				if(ON_LEAVE()){
					ELEMENTS_set_flag(FLAG_SUB_HARRY_TAKE_CYLINDER_OUR_ROCKET_UNI, FALSE);	// Flag subaction
					//set_sub_act_enable(SUB_HARRY_DEPOSE_MODULES, TRUE);   // Activation de la dépose
				}
				break;

			case STORAGE_RIGHT:
				if(entrance){
					if(global.color == BLUE){
						STOCKS_addModule(MODULE_BLUE, STOCK_POS_ELEVATOR, MODULE_STOCK_RIGHT);
					}else{
						STOCKS_addModule(MODULE_YELLOW, STOCK_POS_ELEVATOR, MODULE_STOCK_RIGHT);
					}
					ACT_push_order( ACT_POMPE_SLIDER_RIGHT , ACT_POMPE_STOP );
				}

				state=check_sub_action_result(sub_act_harry_mae_store_modules(MODULE_STOCK_RIGHT, TRUE),state,DONE,ERROR);

				if(state==DONE){
					if(modules == OUR_ELEMENT)
						ELEMENTS_set_flag(FLAG_OUR_MULTICOLOR_START_IS_TAKEN, TRUE);
					else
						ELEMENTS_set_flag(FLAG_ADV_MULTICOLOR_START_IS_TAKEN, TRUE);	// Flag element
				}
				if(ON_LEAVE()){
					ELEMENTS_set_flag(FLAG_SUB_HARRY_TAKE_CYLINDER_OUR_ROCKET_UNI, FALSE);	// Flag subaction
					//set_sub_act_enable(SUB_HARRY_DEPOSE_MODULES, TRUE);   // Activation de la dépose
				}
				break;

			case ERROR:
				RESET_MAE();
				ELEMENTS_set_flag(FLAG_SUB_HARRY_TAKE_CYLINDER_OUR_ROCKET_UNI, FALSE);
				ELEMENTS_set_flag(FLAG_SUB_HARRY_TAKE_CYLINDER_OUR_ROCKET_MULTI, FALSE);
				ELEMENTS_set_flag(FLAG_SUB_HARRY_TAKE_CYLINDER_OUR_CENTER, FALSE);
				on_turning_point();
				return NOT_HANDLED;
				break;

			case DONE:
				RESET_MAE();
				ELEMENTS_set_flag(FLAG_SUB_HARRY_TAKE_CYLINDER_OUR_ROCKET_UNI, FALSE);
				ELEMENTS_set_flag(FLAG_SUB_HARRY_TAKE_CYLINDER_OUR_ROCKET_MULTI, FALSE);
				ELEMENTS_set_flag(FLAG_SUB_HARRY_TAKE_CYLINDER_OUR_CENTER, FALSE);
				on_turning_point();
				return END_OK;
				break;

			default:
				if(entrance)
					debug_printf("default case in sub_harry_prise_modules_centre\n");
				break;
		}
	return IN_PROGRESS;
}

error_e sub_harry_prise_module_side_centre(ELEMENTS_property_e modules, ELEMENTS_side_e side){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_MODULE_SIDE_CENTER,
			INIT,

			GET_IN_CLOSE_ADV_ZONE,
			GET_IN_FAR_ADV_ZONE,
			GET_IN_ASTAR,
			GET_IN_CLOSE_START_ZONE,
			AVOID_MODULE_START,

			GO_TO_SIDE_POINT_UP,
			GO_TO_SIDE_POINT_FROM_DEPOSE_SIDE,
			GO_TO_SIDE_POINT_FROM_LEFT_SIDE,
			TAKE_SIDE_MODULE_FROM_START_ZONE,
			TAKE_SIDE_MODULE_FROM_DEPOSE_SIDE,
			TAKE_SIDE_MODULE_FROM_LEFT_SIDE,

			STORAGE_LEFT,
			STORAGE_RIGHT,

			ERROR,
			DONE
		);

	const displacement_t curve_far_zone_yellow[2] = {(displacement_t){(GEOMETRY_point_t){1000, 2400}, FAST},
												     (displacement_t){(GEOMETRY_point_t){950, 2000}, FAST},
												    };

	const displacement_t curve_far_zone_blue[2] = {(displacement_t){(GEOMETRY_point_t){1000, 600}, FAST},
												   (displacement_t){(GEOMETRY_point_t){950, 1000}, FAST},
												  };

	const displacement_t curve_avoid_module_start_yellow[2] = {(displacement_t){(GEOMETRY_point_t){600, 1700}, FAST},
																 (displacement_t){(GEOMETRY_point_t){900, 2000}, FAST},
																};

	const displacement_t curve_avoid_module_start_blue[2] = {(displacement_t){(GEOMETRY_point_t){600, 1300}, FAST},
															 (displacement_t){(GEOMETRY_point_t){900, 1000}, FAST},
															};


	switch(state){
		case INIT:
			if(IHM_switchs_get(SWITCH_DISABLE_MODULE_RIGHT) && IHM_switchs_get(SWITCH_DISABLE_MODULE_LEFT)){
				state = ERROR; // Actionneurs désactivés
			}else if (ELEMENTS_get_flag(FLAG_OUR_MULTICOLOR_SIDE_IS_TAKEN)){
					state = DONE; // Il n'y a plus rien à faire
			}
			else
			{
				// Si on prend les modules du coté BLUE		(utiliser i_am_in_square et pas i_am_in_square_color)
				if((global.color == BLUE && modules == OUR_ELEMENT) || (global.color == YELLOW && modules == ADV_ELEMENT)){
					if(i_am_in_square(0, 550, 800, 1200) && !ELEMENTS_get_flag(FLAG_OUR_MULTICOLOR_START_IS_TAKEN)){
						state = AVOID_MODULE_START;
					}else if(i_am_in_square(0, 550, 800, 1200) && ELEMENTS_get_flag(FLAG_OUR_MULTICOLOR_START_IS_TAKEN)){
						state = GET_IN_CLOSE_START_ZONE;
					}else if(i_am_in_square(350,1100,0,700)){
						state = GO_TO_SIDE_POINT_FROM_LEFT_SIDE;
					}else if(i_am_in_square(350, 1100, 700, 2100)){
						state = GO_TO_SIDE_POINT_UP;
					}else if(i_am_in_square(1100, 2000, 0, 800)){
						state =	GO_TO_SIDE_POINT_FROM_DEPOSE_SIDE;
					}else if(i_am_in_square(700, 1400, 2100, 2600)){
						state = GET_IN_CLOSE_ADV_ZONE;
					}else if(i_am_in_square(700, 1400, 2600, 2800)){
						state = GET_IN_FAR_ADV_ZONE;
					}else{
						state = GET_IN_ASTAR;
					}
				}
				else	// Si on prend les modules du coté YELLOW	(utiliser i_am_in_square et pas i_am_in_square_color)
				{
					if(i_am_in_square(0, 550, 1800, 2200) && !ELEMENTS_get_flag(FLAG_OUR_MULTICOLOR_START_IS_TAKEN)){
						state = AVOID_MODULE_START;
					}else if(i_am_in_square(0, 550, 1800, 2200) && ELEMENTS_get_flag(FLAG_OUR_MULTICOLOR_START_IS_TAKEN)){
						state = GET_IN_CLOSE_START_ZONE;
					}else if(i_am_in_square(350,1100,2300,3000)){
						state = GO_TO_SIDE_POINT_FROM_LEFT_SIDE;
					}else if(i_am_in_square(350, 1100, 900, 2300)){
						state = GO_TO_SIDE_POINT_UP;
					}else if(i_am_in_square(1100, 2000, 2200, 3000)){
						state =	GO_TO_SIDE_POINT_FROM_DEPOSE_SIDE;
					}else if(i_am_in_square(700, 1400, 400, 1100)){
						state = GET_IN_CLOSE_ADV_ZONE;
					}else if(i_am_in_square(700, 1400, 200, 400)){
						state = GET_IN_FAR_ADV_ZONE;
					}else{
						state = GET_IN_ASTAR;
					}
				}

				// On lève le flag de subaction
				ELEMENTS_set_flag(FLAG_SUB_HARRY_TAKE_CYLINDER_OUR_CENTER, TRUE);
			}
			break;

		case AVOID_MODULE_START:
			if((global.color == BLUE && modules == OUR_ELEMENT) || (global.color == YELLOW && modules == ADV_ELEMENT)){
				state = try_going_multipoint(curve_avoid_module_start_blue, 2, state, GO_TO_SIDE_POINT_UP, ERROR, ANY_WAY, NO_DODGE_AND_WAIT,END_AT_BRAKE);
			}else{
				state = try_going_multipoint(curve_avoid_module_start_yellow, 2, state, GO_TO_SIDE_POINT_UP, ERROR, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_BRAKE);
			}
			break;

		case GET_IN_CLOSE_ADV_ZONE:
			if((global.color == BLUE && modules == OUR_ELEMENT) || (global.color == YELLOW && modules == ADV_ELEMENT)){
				state = try_going(900, 2000, state, GO_TO_SIDE_POINT_UP, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_BRAKE);
			}else{
				state = try_going(900, 1000, state, GO_TO_SIDE_POINT_UP, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_BRAKE);
			}
			break;

		case GET_IN_FAR_ADV_ZONE:
			if((global.color == BLUE && modules == OUR_ELEMENT) || (global.color == YELLOW && modules == ADV_ELEMENT)){
				state = try_going_multipoint(curve_far_zone_yellow, 2, state, GO_TO_SIDE_POINT_UP, ERROR, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_BRAKE);
			}else{
				state = try_going_multipoint(curve_far_zone_blue, 2, state, GO_TO_SIDE_POINT_UP, ERROR, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_BRAKE);
			}
			break;

		case GET_IN_ASTAR:
			if((global.color == BLUE && modules == OUR_ELEMENT) || (global.color == YELLOW && modules == ADV_ELEMENT)){
				state = ASTAR_try_going(870, 700, state, GO_TO_SIDE_POINT_UP, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_BRAKE);
			}else{
				state = ASTAR_try_going(870, 2400, state, GO_TO_SIDE_POINT_UP, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_BRAKE);
			}
			break;


		case GET_IN_CLOSE_START_ZONE:
			if((global.color == BLUE && modules == OUR_ELEMENT) || (global.color == YELLOW && modules == ADV_ELEMENT)){
				state = try_going(700, 1000, state, GO_TO_SIDE_POINT_UP, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_BRAKE);
			}else{
				state = try_going(700, 2000, state, GO_TO_SIDE_POINT_UP, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_BRAKE);
			}
			break;


		case GO_TO_SIDE_POINT_UP:
			if(entrance){
				ELEMENTS_set_flag(FLAG_SUB_HARRY_TAKE_CYLINDER_OUR_ROCKET_MULTI, TRUE);
			}
			if((global.color == BLUE && modules == OUR_ELEMENT) || (global.color == YELLOW && modules == ADV_ELEMENT)){
				state = try_going(880, 750, state, TAKE_SIDE_MODULE_FROM_START_ZONE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_BRAKE);
			}
			else{
				state = try_going(880, 2250, state, TAKE_SIDE_MODULE_FROM_START_ZONE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_BRAKE);
			}
			if(ON_LEAVE()){
				if(side == LEFT)
					ACT_push_order( ACT_POMPE_ELEVATOR_LEFT , ACT_POMPE_NORMAL );
				else
					ACT_push_order( ACT_POMPE_ELEVATOR_RIGHT, ACT_POMPE_NORMAL );
			}
			break;

		case TAKE_SIDE_MODULE_FROM_START_ZONE:
			if(side == LEFT){
				if((global.color == BLUE && modules == OUR_ELEMENT) || (global.color == YELLOW && modules == ADV_ELEMENT)){
					state = try_going(980, 550, state, STORAGE_LEFT, ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
				}else{
					state = try_going(980, 2450, state, STORAGE_LEFT, ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
				}
			}
			else{
				if((global.color == BLUE && modules == OUR_ELEMENT) || (global.color == YELLOW && modules == ADV_ELEMENT)){
					state = try_going(1060, 590, state, STORAGE_RIGHT, ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
				}else{
					state = try_going(1060, 2410, state, STORAGE_RIGHT, ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
				}
			}
			break;


		case GO_TO_SIDE_POINT_FROM_DEPOSE_SIDE:
			if(entrance){
				ELEMENTS_set_flag(FLAG_SUB_HARRY_TAKE_CYLINDER_OUR_ROCKET_MULTI, TRUE);
			}
			if((global.color == BLUE && modules == OUR_ELEMENT) || (global.color == YELLOW && modules == ADV_ELEMENT)){
				state = try_going(1300, 700, state, TAKE_SIDE_MODULE_FROM_DEPOSE_SIDE, ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_BRAKE);
			}else{
				state = try_going(1300, 2300, state, TAKE_SIDE_MODULE_FROM_DEPOSE_SIDE, ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_BRAKE);
			}
			if(ON_LEAVE()){
				if(side == LEFT)
					ACT_push_order( ACT_POMPE_ELEVATOR_LEFT , ACT_POMPE_NORMAL );
				else
					ACT_push_order( ACT_POMPE_ELEVATOR_RIGHT, ACT_POMPE_NORMAL );
			}
			break;



		case TAKE_SIDE_MODULE_FROM_DEPOSE_SIDE:
			if(side == LEFT){
				if((global.color == BLUE && modules == OUR_ELEMENT) || (global.color == YELLOW && modules == ADV_ELEMENT)){
					state = try_going(1100, 620, state, STORAGE_LEFT, ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
				}else{
					state = try_going(1100, 1380, state, STORAGE_LEFT, ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
				}
			}
			else{
				if((global.color == BLUE && modules == OUR_ELEMENT) || (global.color == YELLOW && modules == ADV_ELEMENT)){
					state = try_going(1200, 520, state, STORAGE_RIGHT, ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
				}else{
					state = try_going(1200, 2480, state, STORAGE_RIGHT, ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
				}
			}
			break;


		case GO_TO_SIDE_POINT_FROM_LEFT_SIDE:
			if(entrance){
				ELEMENTS_set_flag(FLAG_SUB_HARRY_TAKE_CYLINDER_OUR_ROCKET_MULTI, TRUE);
			}
			if((global.color == BLUE && modules == OUR_ELEMENT) || (global.color == YELLOW && modules == ADV_ELEMENT)){
				state = try_going(800, 300, state, TAKE_SIDE_MODULE_FROM_LEFT_SIDE, ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_BRAKE);
			}else{
				state = try_going(950, 2650, state, TAKE_SIDE_MODULE_FROM_LEFT_SIDE, ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_BRAKE);
			}
			if(ON_LEAVE()){
				if(side == LEFT)
					ACT_push_order( ACT_POMPE_ELEVATOR_LEFT , ACT_POMPE_NORMAL );
				else
					ACT_push_order( ACT_POMPE_ELEVATOR_RIGHT, ACT_POMPE_NORMAL );
			}
			break;

		case TAKE_SIDE_MODULE_FROM_LEFT_SIDE:
			if(side == LEFT){
				if((global.color == BLUE && modules == OUR_ELEMENT) || (global.color == YELLOW && modules == ADV_ELEMENT)){
					state = try_going(1050, 380, state, STORAGE_LEFT, ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
				}else{
					state = try_going(1050, 1620, state, STORAGE_LEFT, ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
				}
			}
			else{
				if((global.color == BLUE && modules == OUR_ELEMENT) || (global.color == YELLOW && modules == ADV_ELEMENT)){
					state = try_going(1030, 500, state, STORAGE_RIGHT, ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
				}else{
					state = try_going(1030, 2500, state, STORAGE_RIGHT, ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
				}
			}
			break;

		case STORAGE_LEFT:
			if(entrance){
				if(global.color == BLUE){
					STOCKS_addModule(MODULE_BLUE, STOCK_POS_ELEVATOR, MODULE_STOCK_LEFT);
				}else{
					STOCKS_addModule(MODULE_YELLOW, STOCK_POS_ELEVATOR, MODULE_STOCK_LEFT);
				}
			}

			state=check_sub_action_result(sub_act_harry_mae_store_modules(MODULE_STOCK_LEFT, TRUE),state,DONE,ERROR);

			if(state==DONE){
				if(modules == OUR_ELEMENT)
					ELEMENTS_set_flag(FLAG_OUR_MULTICOLOR_SIDE_IS_TAKEN, TRUE);	// Flag element
				else
					ELEMENTS_set_flag(FLAG_ADV_MULTICOLOR_SIDE_IS_TAKEN, TRUE);	// Flag element
			}
			if(ON_LEAVE()){
				ELEMENTS_set_flag(FLAG_SUB_HARRY_TAKE_CYLINDER_OUR_CENTER, FALSE);	// Flag subaction
				//set_sub_act_enable(SUB_HARRY_DEPOSE_MODULES, TRUE);   // Activation de la dépose
			}
			break;

		case STORAGE_RIGHT:
			if(entrance){
				if(global.color == BLUE){
					STOCKS_addModule(MODULE_BLUE, STOCK_POS_ELEVATOR, MODULE_STOCK_RIGHT);
				}else{
					STOCKS_addModule(MODULE_YELLOW, STOCK_POS_ELEVATOR, MODULE_STOCK_RIGHT);
				}
			}
			state=check_sub_action_result(sub_act_harry_mae_store_modules(MODULE_STOCK_RIGHT, TRUE),state,DONE,ERROR);

			if(state==DONE){
				if(modules == OUR_ELEMENT)
					ELEMENTS_set_flag(FLAG_OUR_MULTICOLOR_SIDE_IS_TAKEN, TRUE);	// Flag element
				else
					ELEMENTS_set_flag(FLAG_ADV_MULTICOLOR_SIDE_IS_TAKEN, TRUE);	// Flag element
			}
			if(ON_LEAVE()){
				ELEMENTS_set_flag(FLAG_SUB_HARRY_TAKE_CYLINDER_OUR_CENTER, FALSE);	// Flag subaction
				//set_sub_act_enable(SUB_HARRY_DEPOSE_MODULES, TRUE);   // Activation de la dépose
			}
			break;

		case ERROR:
			RESET_MAE();
			ELEMENTS_set_flag(FLAG_SUB_HARRY_TAKE_CYLINDER_OUR_ROCKET_UNI, FALSE);
			ELEMENTS_set_flag(FLAG_SUB_HARRY_TAKE_CYLINDER_OUR_ROCKET_MULTI, FALSE);
			ELEMENTS_set_flag(FLAG_SUB_HARRY_TAKE_CYLINDER_OUR_CENTER, FALSE);
			on_turning_point();
			return NOT_HANDLED;
			break;

		case DONE:
			RESET_MAE();
			ELEMENTS_set_flag(FLAG_SUB_HARRY_TAKE_CYLINDER_OUR_ROCKET_UNI, FALSE);
			ELEMENTS_set_flag(FLAG_SUB_HARRY_TAKE_CYLINDER_OUR_ROCKET_MULTI, FALSE);
			ELEMENTS_set_flag(FLAG_SUB_HARRY_TAKE_CYLINDER_OUR_CENTER, FALSE);
			on_turning_point();
			return END_OK;
			break;

		default:
			if(entrance)
				debug_printf("default case in sub_harry_prise_modules_centre\n");
			break;
	}

	return IN_PROGRESS;
}


error_e sub_harry_prise_module_base_centre(ELEMENTS_property_e modules, ELEMENTS_side_e side){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_MODULE_BASE_CENTER,
			INIT,

			GET_IN_CLOSE_ADV_ZONE,
			GET_IN_FAR_ADV_ZONE,
			GET_IN_ASTAR,
			GET_IN_FROM_START_ZONE,
			AVOID_MODULE_START,

			GO_TO_CLOSE_POINT,
			GO_TO_MID_POINT,

			GO_TO_MODULE_FROM_MIDDLE,
			GO_TO_MODULE_FROM_DOWN,
			TAKE_BASE_MODULE,

			STORAGE_LEFT,
			STORAGE_RIGHT,
			GET_OUT,
			GET_OUT_ERROR,

			ERROR,
			DONE
		);

	const displacement_t curve_far_zone_yellow[2] = {(displacement_t){(GEOMETRY_point_t){1000, 2400}, FAST},
												     (displacement_t){(GEOMETRY_point_t){950, 2000}, FAST},
												    };

	const displacement_t curve_far_zone_blue[2] = {(displacement_t){(GEOMETRY_point_t){1000, 600}, FAST},
												   (displacement_t){(GEOMETRY_point_t){950, 1000}, FAST},
												  };

	const displacement_t curve_avoid_module_start_yellow[2] = {(displacement_t){(GEOMETRY_point_t){600, 1700}, FAST},
																 (displacement_t){(GEOMETRY_point_t){900, 2000}, FAST},
																};

	const displacement_t curve_avoid_module_start_blue[2] = {(displacement_t){(GEOMETRY_point_t){600, 1300}, FAST},
															 (displacement_t){(GEOMETRY_point_t){900, 1000}, FAST},
															};


	switch(state){
		case INIT:
			if(IHM_switchs_get(SWITCH_DISABLE_MODULE_RIGHT) && IHM_switchs_get(SWITCH_DISABLE_MODULE_LEFT)){
				state = ERROR; // Actionneurs désactivés
			}else if (ELEMENTS_get_flag(FLAG_OUR_MULTICOLOR_NEAR_DEPOSE_IS_TAKEN)){
					state = DONE; // Il n'y a plus rien à faire
			}
			else
			{
				// Si on prend les modules du coté BLUE		(utiliser i_am_in_square et pas i_am_in_square_color)
				if((global.color == BLUE && modules == OUR_ELEMENT) || (global.color == YELLOW && modules == ADV_ELEMENT)){
					if(i_am_in_square(0, 550, 800, 1200) && !ELEMENTS_get_flag(FLAG_OUR_MULTICOLOR_START_IS_TAKEN)){
						state = AVOID_MODULE_START;
					}else if(i_am_in_square(0, 550, 800, 1200) && ELEMENTS_get_flag(FLAG_OUR_MULTICOLOR_START_IS_TAKEN)){
						state = GET_IN_FROM_START_ZONE;
					}else if(i_am_in_square(450, 2000, 0, 900)){
						state = GO_TO_CLOSE_POINT;
					}else if(i_am_in_square(0, 1000, 1200, 2100)){
						state = GO_TO_MID_POINT;
					}else if(i_am_in_square(700, 1400, 2100, 2600)){ // 2100 -> 1900 sysmétrie bleu ou jaune
						state = GET_IN_CLOSE_ADV_ZONE;
					}else if(i_am_in_square(700, 1400, 2600, 2800)){
						state = GET_IN_FAR_ADV_ZONE;
					}else{
						state = GET_IN_ASTAR;
					}
				}
				else	// Si on prend les modules du coté YELLOW	(utiliser i_am_in_square et pas i_am_in_square_color)
				{
					if(i_am_in_square(0, 550, 1800, 2200) && ELEMENTS_get_flag(FLAG_OUR_MULTICOLOR_START_IS_TAKEN)){
						state = AVOID_MODULE_START;
					}else if(i_am_in_square(0, 550, 1800, 2200) && !ELEMENTS_get_flag(FLAG_OUR_MULTICOLOR_START_IS_TAKEN)){
						state = GET_IN_FROM_START_ZONE;
					}else if(i_am_in_square(450, 2000, 2100, 3000)){
						state = GO_TO_CLOSE_POINT;
					}else if(i_am_in_square(0, 1000, 900, 1800)){
						state = GO_TO_MID_POINT;
					}else if(i_am_in_square(700, 1400, 400, 1100)){  // 1100 -> 900 sysmétrie bleu ou jaune
						state = GET_IN_CLOSE_ADV_ZONE;
					}else if(i_am_in_square(700, 1400, 200, 400)){
						state = GET_IN_FAR_ADV_ZONE;
					}else{
						state = GET_IN_ASTAR;
					}
				}

				// On lève le flag de subaction
				ELEMENTS_set_flag(FLAG_SUB_HARRY_TAKE_CYLINDER_OUR_CENTER, TRUE);
			}
			break;

		case AVOID_MODULE_START:
			if((global.color == BLUE && modules == OUR_ELEMENT) || (global.color == YELLOW && modules == ADV_ELEMENT)){
				state = try_going_multipoint(curve_avoid_module_start_blue, 2, state, GO_TO_CLOSE_POINT, ERROR, ANY_WAY, NO_DODGE_AND_WAIT,END_AT_BRAKE);
			}else{
				state = try_going_multipoint(curve_avoid_module_start_yellow, 2, state, GO_TO_CLOSE_POINT, ERROR, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_BRAKE);
			}
			break;

		case GET_IN_FROM_START_ZONE:
			if((global.color == BLUE && modules == OUR_ELEMENT) || (global.color == YELLOW && modules == ADV_ELEMENT)){
				state = try_going(700, 1000, state, GO_TO_MID_POINT, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_BRAKE);
			}else{
				state = try_going(700, 2000, state, GO_TO_MID_POINT, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_BRAKE);
			}
			break;

		case GET_IN_CLOSE_ADV_ZONE:
			if((global.color == BLUE && modules == OUR_ELEMENT) || (global.color == YELLOW && modules == ADV_ELEMENT)){
				state = try_going(900, 2000, state, GO_TO_MID_POINT, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_BRAKE);
			}else{
				state = try_going(900, 1000, state, GO_TO_MID_POINT, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_BRAKE);
			}
			break;

		case GET_IN_FAR_ADV_ZONE:
			if((global.color == BLUE && modules == OUR_ELEMENT) || (global.color == YELLOW && modules == ADV_ELEMENT)){
				state = try_going_multipoint(curve_far_zone_yellow, 2, state, GO_TO_MID_POINT, ERROR, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_BRAKE);
			}else{
				state = try_going_multipoint(curve_far_zone_blue, 2, state, GO_TO_MID_POINT, ERROR, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_BRAKE);
			}
			break;

		case GET_IN_ASTAR:
			if((global.color == BLUE && modules == OUR_ELEMENT) || (global.color == YELLOW && modules == ADV_ELEMENT)){
				state = ASTAR_try_going(1000, 1135, state, GO_TO_MID_POINT, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_BRAKE);
			}else{
				state = ASTAR_try_going(1000, 2865, state, GO_TO_MID_POINT, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_BRAKE);
			}
			break;

		case GO_TO_MID_POINT:
			if((global.color == BLUE && modules == OUR_ELEMENT) || (global.color == YELLOW && modules == ADV_ELEMENT)){
				state = try_going(1000, 950, state, GO_TO_CLOSE_POINT, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_BRAKE);
			}
			else{
				state = try_going(1000, 2050, state, GO_TO_CLOSE_POINT, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_BRAKE);
			}
			break;

		case GO_TO_CLOSE_POINT:
			if((global.color == BLUE && modules == OUR_ELEMENT) || (global.color == YELLOW && modules == ADV_ELEMENT)){
				state = try_going(1275, 675, state, TAKE_BASE_MODULE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			}
			else{
				state = try_going(1275, 2325, state, TAKE_BASE_MODULE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			}
			if(ON_LEAVE()){
				if(side == LEFT)
					ACT_push_order( ACT_POMPE_ELEVATOR_LEFT , ACT_POMPE_NORMAL );
				else
					ACT_push_order( ACT_POMPE_ELEVATOR_RIGHT, ACT_POMPE_NORMAL );
			}
			break;

		case TAKE_BASE_MODULE:
			if(side == LEFT || side == NO_SIDE){
				if((global.color == BLUE && modules == OUR_ELEMENT) || (global.color == YELLOW && modules == ADV_ELEMENT)){
					state = try_going(1380, 785, state, STORAGE_LEFT, GET_OUT_ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
				}else{
					state = try_going(1380, 2215, state, STORAGE_LEFT, GET_OUT_ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
				}
			}
			else{
				if((global.color == BLUE && modules == OUR_ELEMENT) || (global.color == YELLOW && modules == ADV_ELEMENT)){
					state = try_going(1310, 810, state, STORAGE_RIGHT, GET_OUT_ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
				}else{
					state = try_going(1310, 2190, state, STORAGE_RIGHT, GET_OUT_ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
				}
			}
			break;

		case STORAGE_LEFT:
			if(entrance){
				if(global.color == BLUE){
					STOCKS_addModule(MODULE_BLUE, STOCK_POS_ELEVATOR, MODULE_STOCK_LEFT);
				}else{
					STOCKS_addModule(MODULE_YELLOW, STOCK_POS_ELEVATOR, MODULE_STOCK_LEFT);
				}
			}

			state=check_sub_action_result(sub_act_harry_mae_store_modules(MODULE_STOCK_LEFT, TRUE), state, GET_OUT, ERROR);

			if(state==GET_OUT){
				if(modules == OUR_ELEMENT)
					ELEMENTS_set_flag(FLAG_OUR_MULTICOLOR_NEAR_DEPOSE_IS_TAKEN, TRUE);	// Flag element
				else{
					ELEMENTS_set_flag(FLAG_ADV_MULTICOLOR_NEAR_DEPOSE_IS_TAKEN, TRUE);	// Flag element
				}
			}
			if(ON_LEAVE()){
				ELEMENTS_set_flag(FLAG_SUB_HARRY_TAKE_CYLINDER_OUR_CENTER, FALSE);	// Flag subaction
				//set_sub_act_enable(SUB_HARRY_DEPOSE_MODULES, TRUE);   // Activation de la dépose
			}
		break;

		case STORAGE_RIGHT:
			if(entrance){
				if(global.color == BLUE){
					STOCKS_addModule(MODULE_BLUE, STOCK_POS_ELEVATOR, MODULE_STOCK_RIGHT);
				}else{
					STOCKS_addModule(MODULE_YELLOW, STOCK_POS_ELEVATOR, MODULE_STOCK_RIGHT);
				}
			}

			state=check_sub_action_result(sub_act_harry_mae_store_modules(MODULE_STOCK_RIGHT, TRUE), state, GET_OUT, ERROR);

			if(state==GET_OUT){
				if(modules == OUR_ELEMENT)
					ELEMENTS_set_flag(FLAG_OUR_MULTICOLOR_NEAR_DEPOSE_IS_TAKEN, TRUE);	// Flag element
				else
					ELEMENTS_set_flag(FLAG_ADV_MULTICOLOR_NEAR_DEPOSE_IS_TAKEN, TRUE);	// Flag element
			}
			if(ON_LEAVE()){
				ELEMENTS_set_flag(FLAG_SUB_HARRY_TAKE_CYLINDER_OUR_CENTER, FALSE);	// Flag subaction
				//set_sub_act_enable(SUB_HARRY_DEPOSE_MODULES, TRUE);   // Activation de la dépose
			}
			break;

		case GET_OUT:
			if((global.color == BLUE && modules == OUR_ELEMENT) || (global.color == YELLOW && modules == ADV_ELEMENT)){
				state = try_going(1260, 700, state, DONE, GET_OUT_ERROR, FAST, BACKWARD, NO_DODGE_AND_WAIT, END_AT_BRAKE);
			}else{
				state = try_going(1260, 2300, state, DONE, GET_OUT_ERROR, FAST, BACKWARD, NO_DODGE_AND_WAIT, END_AT_BRAKE);
			}
			if(ON_LEAVE()){
				ELEMENTS_set_flag(FLAG_SUB_HARRY_TAKE_CYLINDER_OUR_CENTER, FALSE);	// Flag subaction
				//set_sub_act_enable(SUB_HARRY_DEPOSE_MODULES, TRUE); // Activation de la dépose
			}
			break;

		case GET_OUT_ERROR:
			if(side == LEFT || side == NO_SIDE){
				if((global.color == BLUE && modules == OUR_ELEMENT) || (global.color == YELLOW && modules == ADV_ELEMENT)){
					state = try_going(1380, 785, state, GET_OUT, GET_OUT, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
				}else{
					state = try_going(1380, 2215, state, GET_OUT, GET_OUT, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
				}
			}
			else{
				if((global.color == BLUE && modules == OUR_ELEMENT) || (global.color == YELLOW && modules == ADV_ELEMENT)){
					state = try_going(1310, 810, state, GET_OUT, GET_OUT, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
				}else{
					state = try_going(1310, 2190, state, GET_OUT, GET_OUT, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
				}
			}
			break;

		case ERROR:
			RESET_MAE();
			ELEMENTS_set_flag(FLAG_SUB_HARRY_TAKE_CYLINDER_OUR_ROCKET_UNI, FALSE);
			ELEMENTS_set_flag(FLAG_SUB_HARRY_TAKE_CYLINDER_OUR_ROCKET_MULTI, FALSE);
			ELEMENTS_set_flag(FLAG_SUB_HARRY_TAKE_CYLINDER_OUR_CENTER, FALSE);
			on_turning_point();
			return NOT_HANDLED;
			break;

		case DONE:
			RESET_MAE();
			ELEMENTS_set_flag(FLAG_SUB_HARRY_TAKE_CYLINDER_OUR_ROCKET_UNI, FALSE);
			ELEMENTS_set_flag(FLAG_SUB_HARRY_TAKE_CYLINDER_OUR_ROCKET_MULTI, FALSE);
			ELEMENTS_set_flag(FLAG_SUB_HARRY_TAKE_CYLINDER_OUR_CENTER, FALSE);
			on_turning_point();
			return END_OK;
			break;

		default:
			if(entrance)
				debug_printf("default case in sub_harry_prise_modules_centre\n");
			break;
	}

	return IN_PROGRESS;
}


error_e sub_harry_prise_module_unicolor_north(ELEMENTS_side_e side){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_MODULE_UNICOLOR_NORTH,
			INIT,

			GET_IN_DIRECT_LEFT,
			GET_IN_DIRECT_RIGHT,
			GET_IN_MIDDLE,
			GET_IN_CLOSE_ADV_ZONE,
			GET_IN_FAR_ADV_ZONE,
			GET_IN_ASTAR_LEFT,
			GET_IN_ASTAR_RIGHT,

			MOVE_BACK,

			TAKE_MODULE_LEFT,
			TAKE_MODULE_RIGHT,
			STORAGE_LEFT,
			STORAGE_RIGHT,

			GET_OUT_RIGHT,
			MOVE_BACK_RIGHT,

			ERROR,
			DONE
		);

	switch(state){
		case INIT:
			if(IHM_switchs_get(SWITCH_DISABLE_MODULE_RIGHT) && IHM_switchs_get(SWITCH_DISABLE_MODULE_LEFT)){
				state = ERROR; // Actionneurs désactivés
			}else if (ELEMENTS_get_flag(FLAG_OUR_UNICOLOR_NORTH_IS_TAKEN)){
				state = DONE; // Il n'y a plus rien à faire
			}else if (ELEMENTS_get_flag(FLAG_SUB_ANNE_TAKE_CYLINDER_NORTH_UNI)){
				state = ERROR; // Anne est en train de le prendre
			}
			else
			{

				if(i_am_in_square_color(600, 1400, 100, 600) || i_am_in_square_color(800, 1400, 600, 1000)){
					if(side == LEFT){
						state = GET_IN_DIRECT_LEFT;
					}else{
						state = GET_IN_DIRECT_RIGHT;
					}
				}else if(i_am_in_square_color(200, 1000, 1000, 2000)){
					state = GET_IN_MIDDLE;
				}else if(i_am_in_square_color(600, 1400, 2400, 2900)){
					state = GET_IN_FAR_ADV_ZONE;
				}else if(i_am_in_square_color(800, 1400, 2000, 2400)){
					state = GET_IN_CLOSE_ADV_ZONE;
				}else{
					if(side == LEFT){
						state = GET_IN_ASTAR_LEFT;
					}else{
						state = GET_IN_ASTAR_RIGHT;
					}
				}

				// On lève le flag de subaction
				ELEMENTS_set_flag(FLAG_SUB_HARRY_TAKE_CYLINDER_NORTH_UNI, TRUE);
			}
			break;

		case GET_IN_DIRECT_LEFT:	// Point d'accès pour stockage à gauche
			state = try_going(850, COLOR_Y(400), state, TAKE_MODULE_LEFT, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case GET_IN_DIRECT_RIGHT:	// Point d'accès pour stockage à droite
			state = try_going(800, COLOR_Y(450), state, TAKE_MODULE_RIGHT, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case GET_IN_MIDDLE:
			if(side == LEFT){
				state = try_going(1050, COLOR_Y(900), state, GET_IN_DIRECT_LEFT, ERROR, FAST, ANY_WAY, DODGE_AND_WAIT, END_AT_BRAKE);
			}else{
				state = try_going(1050, COLOR_Y(900), state, GET_IN_DIRECT_RIGHT, ERROR, FAST, ANY_WAY, DODGE_AND_WAIT, END_AT_BRAKE);
			}
			break;

		case GET_IN_CLOSE_ADV_ZONE:
			state = try_going(800, COLOR_Y(1800), state, GET_IN_MIDDLE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_BRAKE);
			break;

		case GET_IN_FAR_ADV_ZONE:
			state = try_going(1000, COLOR_Y(2300), state, GET_IN_CLOSE_ADV_ZONE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_BRAKE);
			break;

		case GET_IN_ASTAR_LEFT:	// Point d'accès pour stockage à gauche
			state = ASTAR_try_going(850, COLOR_Y(400), state, TAKE_MODULE_LEFT, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case GET_IN_ASTAR_RIGHT:	// Point d'accès pour stockage à droite
			state = ASTAR_try_going(800, COLOR_Y(450), state, TAKE_MODULE_RIGHT, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case TAKE_MODULE_LEFT:
			if(entrance){
				ACT_push_order(ACT_POMPE_SLIDER_LEFT, ACT_POMPE_NORMAL);
			}
			//state = try_going(650, COLOR_Y(300), state, STORAGE_LEFT, ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			state = try_rush(450, COLOR_Y(300), state , MOVE_BACK, ERROR, FORWARD, NO_DODGE_AND_NO_WAIT, TRUE);
			break;

		case MOVE_BACK:
			state = try_advance(NULL, entrance, 150, MOVE_BACK, STORAGE_LEFT, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case STORAGE_LEFT:
			if(entrance){
				if(global.color == BLUE){
					STOCKS_addModule(MODULE_BLUE, STOCK_POS_ELEVATOR, MODULE_STOCK_LEFT);
				}else{
					STOCKS_addModule(MODULE_YELLOW, STOCK_POS_ELEVATOR, MODULE_STOCK_LEFT);
				}
				ELEMENTS_set_flag(FLAG_OUR_UNICOLOR_NORTH_IS_TAKEN, TRUE);	// Flag element
				//set_sub_act_enable(SUB_HARRY_DEPOSE_MODULES, TRUE);   // Activation de la dépose
			}
			state = check_sub_action_result(sub_act_harry_mae_store_modules(MODULE_STOCK_LEFT, TRUE), state, DONE, ERROR);
			break;


		case TAKE_MODULE_RIGHT:
			if(entrance){
				ACT_push_order(ACT_POMPE_SLIDER_RIGHT, ACT_POMPE_NORMAL);
			}
			state = try_going(730, COLOR_Y(280), state, STORAGE_RIGHT, ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;


		case STORAGE_RIGHT:
			if(entrance){
				if(global.color == BLUE){
					STOCKS_addModule(MODULE_BLUE, STOCK_POS_ELEVATOR, MODULE_STOCK_RIGHT);
				}else{
					STOCKS_addModule(MODULE_YELLOW, STOCK_POS_ELEVATOR, MODULE_STOCK_RIGHT);
				}
				ELEMENTS_set_flag(FLAG_OUR_UNICOLOR_NORTH_IS_TAKEN, TRUE);	// Flag element
				//set_sub_act_enable(SUB_HARRY_DEPOSE_MODULES, TRUE);   // Activation de la dépose
			}
			state = check_sub_action_result(sub_act_harry_mae_store_modules(MODULE_STOCK_RIGHT, TRUE), state, GET_OUT_RIGHT, ERROR);
			break;

		case GET_OUT_RIGHT:
			state = try_going(800, COLOR_Y(450), state, DONE, MOVE_BACK_RIGHT, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_BRAKE);
			break;

		case MOVE_BACK_RIGHT:
			state = try_going(730, COLOR_Y(280), state, GET_OUT_RIGHT, GET_OUT_RIGHT, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case ERROR:
			RESET_MAE();
			ELEMENTS_set_flag(FLAG_SUB_HARRY_TAKE_CYLINDER_NORTH_UNI, FALSE); // Flag subaction
			on_turning_point();
			return NOT_HANDLED;
			break;

		case DONE: // Pas de GET_OUT dans le cas LEFT, ca doit pouvoir passer
			RESET_MAE();
			ELEMENTS_set_flag(FLAG_SUB_HARRY_TAKE_CYLINDER_NORTH_UNI, FALSE); // Flag subaction
			on_turning_point();
			return END_OK;
			break;

		default:
			if(entrance)
				debug_printf("default case in sub_harry_prise_module_unicolor_north\n");
			break;
	}

	return IN_PROGRESS;
}



error_e sub_harry_prise_module_unicolor_south(ELEMENTS_side_e side){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_MODULE_UNICOLOR_SOUTH,
			INIT,

			GET_IN_DIRECT_LEFT,
			GET_IN_DIRECT_RIGHT,
			GET_IN_MIDDLE,
			GET_IN_CLOSE_ADV_ZONE,
			GET_IN_FAR_ADV_ZONE,
			GET_IN_ASTAR_LEFT,
			GET_IN_ASTAR_RIGHT,

			TAKE_MODULE_LEFT,
			TAKE_MODULE_RIGHT,
			STORAGE_LEFT,
			STORAGE_RIGHT,

			GET_OUT_RIGHT,
			MOVE_BACK_RIGHT,

			ERROR,
			DONE
		);

	switch(state){
		case INIT:
			if(IHM_switchs_get(SWITCH_DISABLE_MODULE_RIGHT) && IHM_switchs_get(SWITCH_DISABLE_MODULE_LEFT)){
				state = ERROR; // Actionneurs désactivés
			}else if (ELEMENTS_get_flag(FLAG_OUR_UNICOLOR_SOUTH_IS_TAKEN)){
				state = DONE; // Il n'y a plus rien à faire
			}else if (ELEMENTS_get_flag(FLAG_SUB_ANNE_TAKE_CYLINDER_SOUTH_UNI)){
				state = ERROR; // Anne est en train de le prendre
			}
			else
			{

				if(i_am_in_square_color(700, 1600, 200, 900)){
					if(side == LEFT){
						state = GET_IN_DIRECT_LEFT;
					}else{
						state = GET_IN_DIRECT_RIGHT;
					}
				}else if(i_am_in_square_color(200, 1000, 900, 2100)){
					state = GET_IN_MIDDLE;
				}else if(i_am_in_square_color(600, 1400, 2400, 2900)){
					state = GET_IN_FAR_ADV_ZONE;
				}else if(i_am_in_square_color(800, 1400, 2000, 2400)){
					state = GET_IN_CLOSE_ADV_ZONE;
				}else{
					if(side == LEFT){
						state = GET_IN_ASTAR_LEFT;
					}else{
						state = GET_IN_ASTAR_RIGHT;
					}
				}

				// On lève le flag de subaction
				ELEMENTS_set_flag(FLAG_SUB_HARRY_TAKE_CYLINDER_SOUTH_UNI, TRUE);
			}
			break;

		case GET_IN_DIRECT_LEFT:	// Point d'accès pour stockage à gauche
			state = try_going(1600, COLOR_Y(700), state, TAKE_MODULE_LEFT, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_BRAKE);
			break;

		case GET_IN_DIRECT_RIGHT:	// Point d'accès pour stockage à droite
			state = try_going(1600, COLOR_Y(680), state, TAKE_MODULE_RIGHT, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_BRAKE);
			break;

		case GET_IN_MIDDLE:{
			const displacement_t curve_middle[2] = {(displacement_t){(GEOMETRY_point_t){1000, COLOR_Y(800)}, FAST},
													(displacement_t){(GEOMETRY_point_t){1320, COLOR_Y(630)}, SLOW},
													};
			if(side == LEFT){
				state = try_going_multipoint(curve_middle, 2, state, GET_IN_DIRECT_LEFT, ERROR, ANY_WAY, DODGE_AND_WAIT, END_AT_BRAKE);
			}else{
				state = try_going_multipoint(curve_middle, 2, state, GET_IN_DIRECT_RIGHT, ERROR, ANY_WAY, DODGE_AND_WAIT, END_AT_BRAKE);
			}
		}break;

		case GET_IN_CLOSE_ADV_ZONE:
			state = try_going(800, COLOR_Y(1800), state, GET_IN_MIDDLE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_BRAKE);
			break;

		case GET_IN_FAR_ADV_ZONE:
			state = try_going(1000, COLOR_Y(2300), state, GET_IN_CLOSE_ADV_ZONE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_BRAKE);
			break;

		case GET_IN_ASTAR_LEFT:	// Point d'accès pour stockage à gauche
			state = ASTAR_try_going(1600, COLOR_Y(700), state, TAKE_MODULE_LEFT, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case GET_IN_ASTAR_RIGHT:	// Point d'accès pour stockage à droite
			state = ASTAR_try_going(1600, COLOR_Y(680), state, TAKE_MODULE_RIGHT, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case TAKE_MODULE_LEFT:
			if(entrance){
				ACT_push_order(ACT_POMPE_SLIDER_LEFT, ACT_POMPE_NORMAL);
			}
			state = try_going(1800, COLOR_Y(730), state, STORAGE_LEFT, ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case STORAGE_LEFT:
			if(entrance){
				if(global.color == BLUE){
					STOCKS_addModule(MODULE_BLUE, STOCK_POS_ELEVATOR, MODULE_STOCK_LEFT);
				}else{
					STOCKS_addModule(MODULE_YELLOW, STOCK_POS_ELEVATOR, MODULE_STOCK_LEFT);
				}
				ELEMENTS_set_flag(FLAG_OUR_UNICOLOR_SOUTH_IS_TAKEN, TRUE);	// Flag element
				//set_sub_act_enable(SUB_HARRY_DEPOSE_MODULES, TRUE);   // Activation de la dépose
			}
			state = check_sub_action_result(sub_act_harry_mae_store_modules(MODULE_STOCK_LEFT, TRUE), state, DONE, ERROR);
			break;


		case TAKE_MODULE_RIGHT:
			if(entrance){
				ACT_push_order(ACT_POMPE_SLIDER_RIGHT, ACT_POMPE_NORMAL);
			}
			state = try_going(1775, COLOR_Y(800), state, STORAGE_RIGHT, ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;


		case STORAGE_RIGHT:
			if(entrance){
				if(global.color == BLUE){
					STOCKS_addModule(MODULE_BLUE, STOCK_POS_ELEVATOR, MODULE_STOCK_RIGHT);
				}else{
					STOCKS_addModule(MODULE_YELLOW, STOCK_POS_ELEVATOR, MODULE_STOCK_RIGHT);
				}
				ELEMENTS_set_flag(FLAG_OUR_UNICOLOR_SOUTH_IS_TAKEN, TRUE);	// Flag element
				//set_sub_act_enable(SUB_HARRY_DEPOSE_MODULES, TRUE);   // Activation de la dépose
			}
			state = check_sub_action_result(sub_act_harry_mae_store_modules(MODULE_STOCK_RIGHT, TRUE), state, DONE, ERROR);
			break;

		case ERROR:
			RESET_MAE();
			ELEMENTS_set_flag(FLAG_SUB_HARRY_TAKE_CYLINDER_SOUTH_UNI, FALSE); // Flag subaction
			on_turning_point();
			return NOT_HANDLED;
			break;

		case DONE: // Pas de GET_OUT dans le cas LEFT, ca doit pouvoir passer
			RESET_MAE();
			ELEMENTS_set_flag(FLAG_SUB_HARRY_TAKE_CYLINDER_SOUTH_UNI, FALSE); // Flag subaction
			on_turning_point();
			return END_OK;
			break;

		default:
			if(entrance)
				debug_printf("default case in sub_harry_prise_module_unicolor_south\n");
			break;
	}

	return IN_PROGRESS;
}


error_e sub_harry_rocket_monocolor(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_ROCKET_MONOCOLOR,
			INIT,
			GET_IN_DIRECT,
			GET_IN_ADV_SQUARE,
			GET_IN_OUR_SQUARE,
			PATHFIND,
			GO_TO_START_POINT,
			ROTATE,
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
			if(IHM_switchs_get(SWITCH_DISABLE_MODULE_RIGHT) && IHM_switchs_get(SWITCH_DISABLE_MODULE_LEFT)){
				state = ERROR; // Actionneurs désactivés
			}else if(ROCKETS_isEmpty(MODULE_ROCKET_MONO_OUR_SIDE)){
				state = DONE; // La fusée a déjà été prise
			}else if(ELEMENTS_get_flag(FLAG_SUB_ANNE_TAKE_CYLINDER_OUR_ROCKET_UNI)){
				state = ERROR;
			}else{
				if (i_am_in_square_color(200, 1100, 900, 2100)){
					state = GET_IN_DIRECT;
				}else if(i_am_in_square_color(850, 1400, 300, 900)){
					state = GET_IN_OUR_SQUARE;
				}else if (i_am_in_square_color(850, 1400, 2100, 2700)){
					state = GET_IN_ADV_SQUARE;
				}else{
					state = PATHFIND;
				}

				// On lève le flag de subaction
				ELEMENTS_set_flag(FLAG_SUB_HARRY_TAKE_CYLINDER_OUR_ROCKET_UNI, TRUE);
			}
			break;

		case GET_IN_OUR_SQUARE:
			state = try_going(850, COLOR_Y(1000), state, GET_IN_DIRECT, ERROR, FAST, ANY_WAY, DODGE_AND_WAIT, END_AT_BRAKE);
			break;

		case GET_IN_ADV_SQUARE:
			state = try_going(850, COLOR_Y(2000), state, GET_IN_DIRECT, ERROR, FAST, ANY_WAY, DODGE_AND_WAIT, END_AT_BRAKE);
			break;

		case GET_IN_DIRECT:
			state = try_going(400, COLOR_Y(1150), state, GO_TO_START_POINT, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case PATHFIND:
			state = ASTAR_try_going(400, COLOR_Y(1150), state, GO_TO_START_POINT, ERROR, FAST, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case GO_TO_START_POINT:
			state = try_going(265, COLOR_Y(1150), state, ROTATE, GET_OUT_ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case ROTATE:
			state = try_go_angle(PI4096,state,TAKE_ROCKET, ERROR, FAST, FORWARD,END_AT_LAST_POINT);
		break;

		case TAKE_ROCKET: // Execution des ordres actionneurs
			state=check_sub_action_result(sub_act_harry_take_rocket_down_to_top(MODULE_ROCKET_MONO_OUR_SIDE,LEFT,RIGHT,LEFT,RIGHT),state,GET_OUT,GET_OUT_ERROR);

			// Juste pour les tests
			/*ROCKETS_removeModule(MODULE_ROCKET_MONO_OUR_SIDE);
			ROCKETS_removeModule(MODULE_ROCKET_MONO_OUR_SIDE);
			ROCKETS_removeModule(MODULE_ROCKET_MONO_OUR_SIDE);
			ROCKETS_removeModule(MODULE_ROCKET_MONO_OUR_SIDE);*/

			//state = DONE;
			break;

		case GET_OUT:
			if(entrance){
				//set_sub_act_enable(SUB_HARRY_DEPOSE_MODULES, TRUE);
			}
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
			on_turning_point();
			ELEMENTS_set_flag(FLAG_SUB_HARRY_TAKE_CYLINDER_OUR_ROCKET_UNI, FALSE);
			return NOT_HANDLED;
			break;

		case DONE:
			RESET_MAE();
			on_turning_point();
			ELEMENTS_set_flag(FLAG_SUB_HARRY_TAKE_CYLINDER_OUR_ROCKET_UNI, FALSE);
			return END_OK;
			break;

		default:
			if(entrance)
				debug_printf("default case in sub_harry_rocket_monocolor\n");
			break;
	}

	return IN_PROGRESS;
}



//#if 0
error_e sub_harry_rocket_multicolor(ELEMENTS_property_e rocket){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_PRISE_ROCKET_MULTICOLOR,
				INIT,
				//INIT_ALL_ACTIONNEUR,
				ALL_THE_GET_IN,
				GET_IN_MIDDLE_SQUARE,
				GET_IN_OUR_SQUARE,
				GET_IN_ADV_SQUARE,
				GET_IN_PATHFIND,

				GET_IN_FRONT_OF_ONE_ON_TWO,
				TURN_TO_POS,
				GET_IN_FRONT_OF_TWO_ON_TWO,
				ACTION,

				GET_OUT,
				GET_OUT_ERROR,
				AVANCE,
				AVANCE_ERROR,

				ERROR,
				DONE
			);

	switch(state){

		case INIT:{
			if(IHM_switchs_get(SWITCH_DISABLE_MODULE_RIGHT) && IHM_switchs_get(SWITCH_DISABLE_MODULE_LEFT)){
				state = ERROR; // Actionneurs désactivés
			}else if((rocket == OUR_ELEMENT && ROCKETS_isEmpty(MODULE_ROCKET_MULTI_OUR_SIDE))
			|| (rocket == ADV_ELEMENT && ROCKETS_isEmpty(MODULE_ROCKET_MULTI_ADV_SIDE))){
				state = DONE;	 // On a déjà vidé cette fusée
			}else if((rocket == OUR_ELEMENT && ELEMENTS_get_flag(FLAG_SUB_ANNE_TAKE_CYLINDER_OUR_ROCKET_MULTI))
			|| (rocket == ADV_ELEMENT && ELEMENTS_get_flag(FLAG_SUB_ANNE_TAKE_CYLINDER_ADV_ROCKET_MULTI)) ){
				state = ERROR; // Anne est déjà en train de vider cette fusée
			}else{
				state = ALL_THE_GET_IN;
				//state = INIT_ALL_ACTIONNEUR;
				// On lève le flag de subaction
				if(rocket == OUR_ELEMENT){
					ELEMENTS_set_flag(FLAG_SUB_HARRY_TAKE_CYLINDER_OUR_ROCKET_MULTI, TRUE);
				}else{
					ELEMENTS_set_flag(FLAG_SUB_HARRY_TAKE_CYLINDER_ADV_ROCKET_MULTI, TRUE);
				}
			}
			}break;
		/*
		case INIT_ALL_ACTIONNEUR:
			// on appelle une fonction qui verifie la position initiale de tout nos actionneurs
			static error_e result;
			if(entrance){
				result = init_all_actionneur();
			}
			if(result == END_OK){
				state=ALL_THE_GET_IN;
			}else{
				state=ERROR;
			}
			break;
			*/

		case ALL_THE_GET_IN:

			if((rocket == OUR_ELEMENT && global.color == BLUE) || (rocket == ADV_ELEMENT && global.color == YELLOW)){
				if(i_am_in_square(700, 1450, 250, 1000)){
					state = GET_IN_OUR_SQUARE;
				}else if (i_am_in_square(350, 1050, 1000, 2000)){
					state = GET_IN_MIDDLE_SQUARE;
				}else if (i_am_in_square(700, 1450, 2000, 2750)){
					state = GET_IN_ADV_SQUARE;
				}else{
					state = GET_IN_PATHFIND;
				}
			}else{ // Prise fusee cote bleu
				if(i_am_in_square(700, 1450, 2000, 2750)){
					state = GET_IN_OUR_SQUARE;
				}else if (i_am_in_square(350, 1050, 1000, 2000)){
					state = GET_IN_MIDDLE_SQUARE;
				}else if (i_am_in_square(700, 1450, 250, 1000)){
					state = GET_IN_ADV_SQUARE;
				}else{
					state = GET_IN_PATHFIND;
				}
			}
			break;


		case GET_IN_OUR_SQUARE:
			if((rocket == OUR_ELEMENT && global.color == BLUE) || (rocket == ADV_ELEMENT && global.color == YELLOW)){
				state = try_going(1250, 500, state, GET_IN_FRONT_OF_ONE_ON_TWO, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_BRAKE);
			}else{
				state = try_going(1250, 2500, state, GET_IN_FRONT_OF_ONE_ON_TWO, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_BRAKE);
			}
			break;

		case GET_IN_MIDDLE_SQUARE:
			if((rocket == OUR_ELEMENT && global.color == BLUE) || (rocket == ADV_ELEMENT && global.color == YELLOW)){
				state = try_going(900, 950, state, GET_IN_OUR_SQUARE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_BRAKE);
			}else{
				state = try_going(900, 2050, state, GET_IN_OUR_SQUARE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_BRAKE);
			}
			break;

		case GET_IN_ADV_SQUARE:
			if((rocket == OUR_ELEMENT && global.color == BLUE) || (rocket == ADV_ELEMENT && global.color == YELLOW)){
				state = try_going(850, 2000, state, GET_IN_MIDDLE_SQUARE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_BRAKE);
			}else{
				state = try_going(850, 1000, state, GET_IN_MIDDLE_SQUARE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_BRAKE);
			}
			break;


		case GET_IN_PATHFIND:
			if((rocket == OUR_ELEMENT && global.color == BLUE) || (rocket == ADV_ELEMENT && global.color == YELLOW)){
				state = ASTAR_try_going(1250, 500, GET_IN_PATHFIND, GET_IN_FRONT_OF_ONE_ON_TWO, ERROR, FAST, ANY_WAY, NO_AVOIDANCE, END_AT_BRAKE);
			}else{
				state = ASTAR_try_going(1250, 2500, GET_IN_PATHFIND, GET_IN_FRONT_OF_ONE_ON_TWO, ERROR, FAST, ANY_WAY, NO_AVOIDANCE, END_AT_BRAKE);
			}
			break;


		case GET_IN_FRONT_OF_ONE_ON_TWO:
			if((rocket == OUR_ELEMENT && global.color == BLUE) || (rocket == ADV_ELEMENT && global.color == YELLOW)){
				state = try_going(1350, 360, state, TURN_TO_POS, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			}else{
				state = try_going(1350, 2640, state, TURN_TO_POS, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			}
			break;

		case TURN_TO_POS:
			if((rocket == OUR_ELEMENT && global.color == BLUE) || (rocket == ADV_ELEMENT && global.color == YELLOW)){
				state = try_go_angle(-PI4096/2, state, GET_IN_FRONT_OF_TWO_ON_TWO, ERROR, FAST, ANY_WAY, END_AT_LAST_POINT);
			}else{
				state = try_go_angle(PI4096/2, state, GET_IN_FRONT_OF_TWO_ON_TWO, ERROR, FAST, ANY_WAY, END_AT_LAST_POINT);
			}
			break;

		case GET_IN_FRONT_OF_TWO_ON_TWO:
			if(entrance){
				if((rocket == OUR_ELEMENT && global.color == BLUE) || (rocket == ADV_ELEMENT && global.color == YELLOW)){
					ACT_push_order(ACT_BIG_BALL_FRONT_LEFT, ACT_BIG_BALL_FRONT_LEFT_UP);
				}else{
					ACT_push_order(ACT_BIG_BALL_FRONT_RIGHT, ACT_BIG_BALL_FRONT_RIGHT_UP);
				}
			}
			if((rocket == OUR_ELEMENT && global.color == BLUE) || (rocket == ADV_ELEMENT && global.color == YELLOW)){
				state = try_going(1345, 265, state, ACTION, ERROR, FAST, FORWARD, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			}else{
				state = try_going(1345, 2735, state, ACTION, ERROR, FAST, FORWARD, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			}
			break;

		case ACTION:
			state = check_sub_action_result(sub_act_harry_take_rocket_down_to_top(rocket, LEFT, RIGHT, LEFT, RIGHT),state, GET_OUT, GET_OUT_ERROR);
			break;

		case GET_OUT:
			if((rocket == OUR_ELEMENT && global.color == BLUE) || (rocket == ADV_ELEMENT && global.color == YELLOW)){
				state=try_going(global.pos.x, 500, state, DONE, AVANCE, FAST, BACKWARD, NO_DODGE_AND_NO_WAIT, END_AT_BRAKE);
			}else{
				state=try_going(global.pos.x, 2500, state, DONE, AVANCE, FAST, BACKWARD, NO_DODGE_AND_NO_WAIT, END_AT_BRAKE);
			}
			if(ON_LEAVE()){
				if((rocket == OUR_ELEMENT && global.color == BLUE) || (rocket == ADV_ELEMENT && global.color == YELLOW)){
					ACT_push_order(ACT_BIG_BALL_FRONT_LEFT, ACT_BIG_BALL_FRONT_LEFT_DOWN);
				}else{
					ACT_push_order(ACT_BIG_BALL_FRONT_RIGHT, ACT_BIG_BALL_FRONT_RIGHT_DOWN);
				}
			}
			break;

		case GET_OUT_ERROR:
			if((rocket == OUR_ELEMENT && global.color == BLUE) || (rocket == ADV_ELEMENT && global.color == YELLOW)){
				state=try_going(global.pos.x, 500, state, ERROR, AVANCE_ERROR, FAST, BACKWARD, NO_DODGE_AND_NO_WAIT, END_AT_BRAKE);
			}else{
				state=try_going(global.pos.x, 2500, state, ERROR, AVANCE_ERROR, FAST, BACKWARD, NO_DODGE_AND_NO_WAIT, END_AT_BRAKE);
			}
			if(ON_LEAVE()){
				if((rocket == OUR_ELEMENT && global.color == BLUE) || (rocket == ADV_ELEMENT && global.color == YELLOW)){
					ACT_push_order(ACT_BIG_BALL_FRONT_LEFT, ACT_BIG_BALL_FRONT_LEFT_DOWN);
				}else{
					ACT_push_order(ACT_BIG_BALL_FRONT_RIGHT, ACT_BIG_BALL_FRONT_RIGHT_DOWN);
				}
			}
			break;

		case AVANCE:
			if((rocket == OUR_ELEMENT && global.color == BLUE) || (rocket == ADV_ELEMENT && global.color == YELLOW)){
				state=try_going(global.pos.x, 200, state, GET_OUT, GET_OUT, FAST, BACKWARD, NO_DODGE_AND_NO_WAIT, END_AT_BRAKE);
			}else{
				state=try_going(global.pos.x, 2780, state, GET_OUT, GET_OUT, FAST, BACKWARD, NO_DODGE_AND_NO_WAIT, END_AT_BRAKE);
			}
			break;

		case AVANCE_ERROR:
			if((rocket == OUR_ELEMENT && global.color == BLUE) || (rocket == ADV_ELEMENT && global.color == YELLOW)){
				state=try_going(global.pos.x, 220, state, GET_OUT_ERROR, GET_OUT_ERROR, FAST, BACKWARD, NO_DODGE_AND_NO_WAIT, END_AT_BRAKE);
			}else{
				state=try_going(global.pos.x, 2780, state, GET_OUT_ERROR, GET_OUT_ERROR, FAST, BACKWARD, NO_DODGE_AND_NO_WAIT, END_AT_BRAKE);
			}
			break;


		case DONE:
			RESET_MAE();
			if(rocket == OUR_ELEMENT){
				ELEMENTS_set_flag(FLAG_SUB_HARRY_TAKE_CYLINDER_OUR_ROCKET_MULTI, FALSE);
			}else{
				ELEMENTS_set_flag(FLAG_SUB_HARRY_TAKE_CYLINDER_ADV_ROCKET_MULTI, FALSE);
			}
			return END_OK;
			break;

		case ERROR:
			RESET_MAE();
			if(rocket == OUR_ELEMENT){
				ELEMENTS_set_flag(FLAG_SUB_HARRY_TAKE_CYLINDER_OUR_ROCKET_MULTI, FALSE);
			}else{
				ELEMENTS_set_flag(FLAG_SUB_HARRY_TAKE_CYLINDER_ADV_ROCKET_MULTI, FALSE);
			}
			return NOT_HANDLED;
			break;

		default:
			if(entrance)
				debug_printf("default case in sub_harry_fusee_multicolor\n");
			break;
		}

		return IN_PROGRESS;
}

/*error_e sub_harry_prise_modules_centre(ELEMENTS_property_e modules, bool_e onlyTwoModules){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_MODULE_CENTER,
			INIT,
			GO_TO_START_POINT_UP,
			GO_TO_START_POINT_SIDE,
			GET_IN_CLOSE_ADV_ZONE,
			GET_IN_FAR_ADV_ZONE,
			GET_IN_ASTAR,
			TAKE_FIRST_MODULE_UP,
			TAKE_FIRST_MODULE_SIDE,
			CHOICE_SECOND_MODULE,
			GO_TO_THIRD_MODULE_DIRECTLY,
			TAKE_THIRD_MODULE_DIRECTLY,
			GO_TO_SECOND_MODULE,
			TAKE_SECOND_MODULE,
			GET_OUT_SECOND_MODULE,
			GO_TO_THIRD_MODULE,
			TAKE_THIRD_MODULE,
			ERROR,
			DONE
		);

	const displacement_t curve_far_zone_yellow[2] = {(displacement_t){(GEOMETRY_point_t){1000, 2400}, FAST},
												     (displacement_t){(GEOMETRY_point_t){950, 2000}, FAST},
												    };

	const displacement_t curve_far_zone_blue[2] = {(displacement_t){(GEOMETRY_point_t){1000, 600}, FAST},
												   (displacement_t){(GEOMETRY_point_t){950, 1000}, FAST},
												  };

	switch(state){
		case INIT:
			if(IHM_switchs_get(SWITCH_DISABLE_MODULE_RIGHT) && IHM_switchs_get(SWITCH_DISABLE_MODULE_LEFT)){
				state = ERROR; // Actionneurs désactivés
			}else if( (onlyTwoModules && ELEMENTS_get_flag(FLAG_OUR_MULTICOLOR_START_IS_TAKEN) && ELEMENTS_get_flag(FLAG_OUR_MULTICOLOR_SIDE_IS_TAKEN))
			|| (!onlyTwoModules && ELEMENTS_get_flag(FLAG_OUR_MULTICOLOR_START_IS_TAKEN) && ELEMENTS_get_flag(FLAG_OUR_MULTICOLOR_SIDE_IS_TAKEN) && ELEMENTS_get_flag(FLAG_OUR_MULTICOLOR_NEAR_DEPOSE_IS_TAKEN))){
				state = DONE; // Il n'y a plus rien à faire
			}
			else if(ELEMENTS_get_flag(FLAG_SUB_ANNE_TAKE_CYLINDER_OUR_ROCKET_UNI)) // Si Anne est déjà en train de faire cette action
			{
				state = ERROR;
			}
			else
			{
				// Si on prend les modules du coté BLUE		(utiliser i_am_in_square et pas i_am_in_square_color)
				if((global.color == BLUE && modules == OUR_ELEMENT) || (global.color == YELLOW && modules == ADV_ELEMENT)){
					if(i_am_in_square(0, 550, 800, 1200)){
						state = GO_TO_START_POINT_UP;
					}else if(i_am_in_square(0, 1000, 1200, 2100)){
						state = GO_TO_START_POINT_SIDE;
					}else if(i_am_in_square(700, 1400, 2100, 2600)){
						state = GET_IN_CLOSE_ADV_ZONE;
					}else if(i_am_in_square(700, 1400, 2600, 2800)){
						state = GET_IN_FAR_ADV_ZONE;
					}else{
						state = GET_IN_ASTAR;
					}
				}
				else	// Si on prend les modules du coté YELLOW	(utiliser i_am_in_square et pas i_am_in_square_color)
				{
					if(i_am_in_square(0, 550, 1800, 2200)){
						state = GO_TO_START_POINT_UP;
					}else if(i_am_in_square(0, 1000, 900, 1800)){
						state = GO_TO_START_POINT_SIDE;
					}else if(i_am_in_square(700, 1400, 400, 1100)){
						state = GET_IN_CLOSE_ADV_ZONE;
					}else if(i_am_in_square(700, 1400, 200, 400)){
						state = GET_IN_FAR_ADV_ZONE;
					}else{
						state = GET_IN_ASTAR;
					}
				}

				// On lève le flag de subaction
				ELEMENTS_set_flag(FLAG_SUB_HARRY_TAKE_CYLINDER_OUR_ROCKET_UNI, TRUE);
			}
			break;

		case GET_IN_CLOSE_ADV_ZONE:
			if((global.color == BLUE && modules == OUR_ELEMENT) || (global.color == YELLOW && modules == ADV_ELEMENT)){
				state = try_going(900, 2000, state, GO_TO_START_POINT_SIDE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_BRAKE);
			}else{
				state = try_going(900, 1000, state, GO_TO_START_POINT_SIDE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_BRAKE);
			}
			break;

		case GET_IN_FAR_ADV_ZONE:
			if((global.color == BLUE && modules == OUR_ELEMENT) || (global.color == YELLOW && modules == ADV_ELEMENT)){
				state = try_going_multipoint(curve_far_zone_yellow, 2, state, GO_TO_START_POINT_SIDE, ERROR, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_BRAKE);
			}else{
				state = try_going_multipoint(curve_far_zone_blue, 2, state, GO_TO_START_POINT_SIDE, ERROR, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_BRAKE);
			}
			break;

		case GET_IN_ASTAR:
			if((global.color == BLUE && modules == OUR_ELEMENT) || (global.color == YELLOW && modules == ADV_ELEMENT)){
				state = ASTAR_try_going(600, 1250, state, TAKE_FIRST_MODULE_SIDE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			}else{
				state = ASTAR_try_going(600, 1750, state, TAKE_FIRST_MODULE_SIDE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			}
			break;

		case GO_TO_START_POINT_UP:
			if((global.color == BLUE && modules == OUR_ELEMENT) || (global.color == YELLOW && modules == ADV_ELEMENT)){
				state = try_going(350, 950, state, TAKE_FIRST_MODULE_UP, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			}else{
				state = try_going(350, 2050, state, TAKE_FIRST_MODULE_UP, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			}
			break;

		case GO_TO_START_POINT_SIDE:
			if((global.color == BLUE && modules == OUR_ELEMENT) || (global.color == YELLOW && modules == ADV_ELEMENT)){
				state = try_going(650, 1250, state, TAKE_FIRST_MODULE_SIDE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			}else{
				state = try_going(650, 1750, state, TAKE_FIRST_MODULE_SIDE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			}
			break;

		case TAKE_FIRST_MODULE_UP:
			if((global.color == BLUE && modules == OUR_ELEMENT) || (global.color == YELLOW && modules == ADV_ELEMENT)){
				state = try_going(650, 950, state, CHOICE_SECOND_MODULE, ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			}else{
				state = try_going(650, 2050, state, CHOICE_SECOND_MODULE, ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			}
			if(ON_LEAVE()){
				ELEMENTS_set_flag(FLAG_OUR_MULTICOLOR_START_IS_TAKEN, TRUE);	// Flag element
				ELEMENTS_set_flag(FLAG_SUB_HARRY_TAKE_CYLINDER_OUR_ROCKET_UNI, FALSE);	// Flag subaction
			}
			break;

		case TAKE_FIRST_MODULE_SIDE:
			if((global.color == BLUE && modules == OUR_ELEMENT) || (global.color == YELLOW && modules == ADV_ELEMENT)){
				state = try_going(650, 1100, state, CHOICE_SECOND_MODULE, ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_BRAKE);
			}else{
				state = try_going(650, 1900, state, CHOICE_SECOND_MODULE, ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_BRAKE);
			}
			if(ON_LEAVE()){
				ELEMENTS_set_flag(FLAG_OUR_MULTICOLOR_START_IS_TAKEN, TRUE);	// Flag element
				ELEMENTS_set_flag(FLAG_SUB_HARRY_TAKE_CYLINDER_OUR_ROCKET_UNI, FALSE);	// Flag subaction
				set_sub_act_enable(SUB_HARRY_DEPOSE_MODULES, TRUE);   // Activation de la dépose
			}
			break;

		case CHOICE_SECOND_MODULE:
			if(onlyTwoModules){
				state = GO_TO_THIRD_MODULE_DIRECTLY;
			}else{
				state = GO_TO_SECOND_MODULE;
			}
			break;

// Prend seulement le 3ième module
		case GO_TO_THIRD_MODULE_DIRECTLY:
			if(entrance){
				ELEMENTS_set_flag(FLAG_SUB_HARRY_TAKE_CYLINDER_OUR_ROCKET_MULTI, TRUE);
			}
			if((global.color == BLUE && modules == OUR_ELEMENT) || (global.color == YELLOW && modules == ADV_ELEMENT)){
				state = try_going(1090, 750, state, TAKE_THIRD_MODULE_DIRECTLY, ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_BRAKE);
			}else{
				state = try_going(1090, 2250, state, TAKE_THIRD_MODULE_DIRECTLY, ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_BRAKE);
			}
			break;

		case TAKE_THIRD_MODULE_DIRECTLY:
			if((global.color == BLUE && modules == OUR_ELEMENT) || (global.color == YELLOW && modules == ADV_ELEMENT)){
				state = try_going(1150, 580, state, DONE, ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			}else{
				state = try_going(1150, 2420, state, DONE, ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			}
			if(ON_LEAVE()){
				ELEMENTS_set_flag(FLAG_OUR_MULTICOLOR_SIDE_IS_TAKEN, TRUE);	// Flag element
				ELEMENTS_set_flag(FLAG_SUB_HARRY_TAKE_CYLINDER_OUR_ROCKET_MULTI, FALSE);	// Flag subaction
				set_sub_act_enable(SUB_HARRY_DEPOSE_MODULES, TRUE);   // Activation de la dépose
			}
			break;

// Prend le 2ième et le 3ième module
		case GO_TO_SECOND_MODULE:
			if(entrance){
				ELEMENTS_set_flag(FLAG_SUB_HARRY_TAKE_CYLINDER_OUR_CENTER, TRUE);
			}
			if((global.color == BLUE && modules == OUR_ELEMENT) || (global.color == YELLOW && modules == ADV_ELEMENT)){
				state = try_going(1150, 850, state, TAKE_SECOND_MODULE, GET_OUT_SECOND_MODULE, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_BRAKE);
			}else{
				state = try_going(1150, 2150, state, TAKE_SECOND_MODULE, GET_OUT_SECOND_MODULE, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_BRAKE);
			}
			break;

		case TAKE_SECOND_MODULE:
			if((global.color == BLUE && modules == OUR_ELEMENT) || (global.color == YELLOW && modules == ADV_ELEMENT)){
				state = try_going(1345, 810, state, GET_OUT_SECOND_MODULE, ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			}else{
				state = try_going(1345, 1190, state, GET_OUT_SECOND_MODULE, ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			}
			if(ON_LEAVE()){
				ELEMENTS_set_flag(FLAG_OUR_MULTICOLOR_NEAR_DEPOSE_IS_TAKEN, TRUE);	// Flag element
				ELEMENTS_set_flag(FLAG_SUB_HARRY_TAKE_CYLINDER_OUR_CENTER, FALSE); // Flag subaction
				set_sub_act_enable(SUB_HARRY_DEPOSE_MODULES, TRUE);   // Activation de la dépose
			}
			break;

		case GET_OUT_SECOND_MODULE:
			if((global.color == BLUE && modules == OUR_ELEMENT) || (global.color == YELLOW && modules == ADV_ELEMENT)){
				state = try_going(1150, 850, state, GO_TO_THIRD_MODULE, TAKE_SECOND_MODULE, FAST, BACKWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			}else{
				state = try_going(1150, 2150, state, GO_TO_THIRD_MODULE, TAKE_SECOND_MODULE, FAST, BACKWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			}
			break;

		case GO_TO_THIRD_MODULE:
			if(entrance){
				ELEMENTS_set_flag(FLAG_SUB_HARRY_TAKE_CYLINDER_OUR_ROCKET_MULTI, TRUE);
			}
			if((global.color == BLUE && modules == OUR_ELEMENT) || (global.color == YELLOW && modules == ADV_ELEMENT)){
				state = try_going(1160, 680, state, TAKE_THIRD_MODULE, ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_BRAKE);
			}else{
				state = try_going(1200, 2320, state, TAKE_THIRD_MODULE, ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_BRAKE);
			}
			break;

		case TAKE_THIRD_MODULE:
			if((global.color == BLUE && modules == OUR_ELEMENT) || (global.color == YELLOW && modules == ADV_ELEMENT)){
				state = try_going(1165, 580, state, DONE, ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			}else{
				state = try_going(1165, 2420, state, DONE, ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			}
			if(ON_LEAVE()){
				ELEMENTS_set_flag(FLAG_OUR_MULTICOLOR_SIDE_IS_TAKEN, TRUE);	// Flag element
				ELEMENTS_set_flag(FLAG_SUB_HARRY_TAKE_CYLINDER_OUR_ROCKET_MULTI, FALSE);	// Flag subaction
				set_sub_act_enable(SUB_HARRY_DEPOSE_MODULES, TRUE);   // Activation de la dépose
			}
			break;

		case ERROR:
			RESET_MAE();
			ELEMENTS_set_flag(FLAG_SUB_HARRY_TAKE_CYLINDER_OUR_ROCKET_UNI, FALSE);
			ELEMENTS_set_flag(FLAG_SUB_HARRY_TAKE_CYLINDER_OUR_ROCKET_MULTI, FALSE);
			ELEMENTS_set_flag(FLAG_SUB_HARRY_TAKE_CYLINDER_OUR_CENTER, FALSE);
			on_turning_point();
			return NOT_HANDLED;
			break;

		case DONE:
			RESET_MAE();
			ELEMENTS_set_flag(FLAG_SUB_HARRY_TAKE_CYLINDER_OUR_ROCKET_UNI, FALSE);
			ELEMENTS_set_flag(FLAG_SUB_HARRY_TAKE_CYLINDER_OUR_ROCKET_MULTI, FALSE);
			ELEMENTS_set_flag(FLAG_SUB_HARRY_TAKE_CYLINDER_OUR_CENTER, FALSE);
			on_turning_point();
			return END_OK;
			break;

		default:
			if(entrance)
				debug_printf("default case in sub_harry_prise_modules_centre\n");
			break;
	}

	return IN_PROGRESS;
}*/

//1285 2265

/*
		case GO_PUNCH:
			state = try_going(1285, COLOR_Y(2265), state, PUNCH, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case PUNCH:

			if(entrance){
				ACT_push_order( ACT_CYLINDER_SLIDER_RIGHT , ACT_CYLINDER_SLIDER_RIGHT_OUT);
			}
			state = check_act_status(ACT_QUEUE_Cylinder_slider_right, state, PUNCH_OUT, ERROR);
			break;

		case PUNCH_OUT:

			if(entrance){
				ACT_push_order( ACT_CYLINDER_SLIDER_RIGHT , ACT_CYLINDER_SLIDER_RIGHT_IN);
			}
			state = check_act_status(ACT_QUEUE_Cylinder_slider_right, state, NEXT, ERROR);
			break;
*/


error_e sub_push_modules_bretagne(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_PUSH_MODULES_BRETAGNE,
			INIT,
			GET_OUT_START_ZONE,
			GO_TO_START_POINT,
			PUSH_MODULES_START_ZONE,
			MOVE_BACK,

			RISE_ELEVATOR_RIGHT,
			RISE_ELEVATOR_LEFT,

			GET_IN_DIRECT,
			GO_TO_ROCKET,
			TAKE_ROCKET_LEFT_OUT,
			TAKE_ROCKET_LEFT_IN,
			GET_BACK,
			DEPOSE,
			STOP_POMPE_LEFT,

			GET_IN_DIRECT_2,
			GO_TO_ROCKET_2,
			TAKE_ROCKET_LEFT_OUT_2,
			TAKE_ROCKET_LEFT_IN_2,
			GET_BACK_2,
			DEPOSE_2,
			STOP_POMPE_LEFT_2,

			GET_IN_DIRECT_3,
			GO_TO_ROCKET_3,
			PROTECT_FALL_OUT_3,
			PROTECT_FALL_IN_3,
			TAKE_ROCKET_LEFT_OUT_3,
			TAKE_ROCKET_LEFT_IN_3,
			GET_BACK_3,
			DEPOSE_3,
			STOP_POMPE_LEFT_3,

			GET_IN_DIRECT_4,
			GO_TO_ROCKET_4,
			PROTECT_FALL_OUT_4,
			PROTECT_FALL_IN_4,
			TAKE_ROCKET_LEFT_OUT_4,
			TAKE_ROCKET_LEFT_IN_4,
			GET_BACK_4,
			DEPOSE_4,
			STOP_POMPE_LEFT_4,

			GET_OUT,
			TAKE_MODULE,
			DEPOSE_MODULE,
			ERROR_GET_OUT,
			ERROR_GET_BACK,

			ERROR,
			DONE
		);

	const displacement_t curve_to_start_point_yellow[5] = {(displacement_t){(GEOMETRY_point_t){450, COLOR_Y(1250)}, FAST},
													 (displacement_t){(GEOMETRY_point_t){950, COLOR_Y(1200)}, FAST},
													 (displacement_t){(GEOMETRY_point_t){1300, COLOR_Y(700)}, FAST},
													 (displacement_t){(GEOMETRY_point_t){1400, COLOR_Y(650)}, SLOW},
													 (displacement_t){(GEOMETRY_point_t){1250, COLOR_Y(375)}, SLOW}
													 };

	const displacement_t curve_to_start_point_blue[5] = {(displacement_t){(GEOMETRY_point_t){450, COLOR_Y(1250)}, FAST},
													 (displacement_t){(GEOMETRY_point_t){950, COLOR_Y(1200)}, FAST},
													 (displacement_t){(GEOMETRY_point_t){1300, COLOR_Y(700)}, FAST},
													 (displacement_t){(GEOMETRY_point_t){1400, COLOR_Y(600)}, SLOW},
													 (displacement_t){(GEOMETRY_point_t){1250, COLOR_Y(350)}, SLOW}
													 };

	const displacement_t curve_push[4] = {(displacement_t){(GEOMETRY_point_t){1100, COLOR_Y(500)}, FAST},
										 (displacement_t){(GEOMETRY_point_t){1000, COLOR_Y(800)}, FAST},
										 (displacement_t){(GEOMETRY_point_t){600, COLOR_Y(1040)}, FAST},
										 (displacement_t){(GEOMETRY_point_t){400, COLOR_Y(980)}, FAST}
										 };

	switch(state){
		case INIT:
			state = GET_OUT_START_ZONE;
			break;

		case GET_OUT_START_ZONE:
			state = try_going(global.pos.x + 100, global.pos.y, state, RISE_ELEVATOR_RIGHT, GO_TO_START_POINT, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case RISE_ELEVATOR_RIGHT:

			if(entrance){
				ACT_push_order( ACT_CYLINDER_ELEVATOR_RIGHT , ACT_CYLINDER_ELEVATOR_RIGHT_LOCK_WITH_CYLINDER);
			}
			state = check_act_status(ACT_QUEUE_Cylinder_elevator_right, state, RISE_ELEVATOR_LEFT, RISE_ELEVATOR_LEFT);
			break;

		case RISE_ELEVATOR_LEFT:

			if(entrance){
				ACT_push_order( ACT_CYLINDER_ELEVATOR_LEFT , ACT_CYLINDER_ELEVATOR_LEFT_LOCK_WITH_CYLINDER);
			}
			state = check_act_status(ACT_QUEUE_Cylinder_elevator_left, state, GET_IN_DIRECT, ERROR);
			break;

		case GET_IN_DIRECT:
			state = try_going(400, COLOR_Y(1150), state, GO_TO_ROCKET, MOVE_BACK, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case GO_TO_ROCKET:
			if(global.color == YELLOW){
				state = try_going(260, COLOR_Y(1100), state, TAKE_ROCKET_LEFT_OUT, TAKE_ROCKET_LEFT_OUT, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			}else{
				state = try_going(260, COLOR_Y(1150), state, TAKE_ROCKET_LEFT_OUT, TAKE_ROCKET_LEFT_OUT, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			}

			break;

		case TAKE_ROCKET_LEFT_OUT:

			if(entrance){
					ACT_push_order( ACT_POMPE_SLIDER_LEFT , ACT_POMPE_NORMAL );
					ACT_push_order( ACT_CYLINDER_SLIDER_LEFT , ACT_CYLINDER_SLIDER_LEFT_OUT);
			}
			state = check_act_status(ACT_QUEUE_Cylinder_slider_left, state, TAKE_ROCKET_LEFT_IN, ERROR);
			break;

		case TAKE_ROCKET_LEFT_IN:

			if(entrance){
					ACT_push_order( ACT_CYLINDER_SLIDER_LEFT , ACT_CYLINDER_SLIDER_LEFT_IN);
			}
			state = check_act_status(ACT_QUEUE_Cylinder_slider_left, state, GET_BACK, ERROR);
			state = check_act_status(ACT_QUEUE_Cylinder_slider_right, state, TAKE_ROCKET_LEFT_OUT, GET_BACK);
			break;

		case GET_BACK:
			state = try_going(400, COLOR_Y(1150), state, DEPOSE, DEPOSE, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case DEPOSE:
			state = try_going(300, COLOR_Y(1000), state, STOP_POMPE_LEFT, STOP_POMPE_LEFT, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case STOP_POMPE_LEFT:

			if(entrance){
					ACT_push_order( ACT_POMPE_SLIDER_LEFT , ACT_POMPE_STOP );
			}
			state = check_act_status(ACT_QUEUE_Pompe_act_slider_left, state, GET_IN_DIRECT_2, GET_IN_DIRECT_2);
			break;

		case GET_IN_DIRECT_2:
			state = try_going(400, COLOR_Y(1150), state, GO_TO_ROCKET_2, MOVE_BACK, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case GO_TO_ROCKET_2:
			if(global.color == YELLOW){
				state = try_going(260, COLOR_Y(1150), state, TAKE_ROCKET_LEFT_OUT_2, TAKE_ROCKET_LEFT_OUT_2, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			}else{
				state = try_going(260, COLOR_Y(1150), state, TAKE_ROCKET_LEFT_OUT_2, TAKE_ROCKET_LEFT_OUT_2, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			}

			break;

		case TAKE_ROCKET_LEFT_OUT_2:

			if(entrance){
					ACT_push_order( ACT_POMPE_SLIDER_LEFT , ACT_POMPE_NORMAL );
					ACT_push_order( ACT_CYLINDER_SLIDER_LEFT , ACT_CYLINDER_SLIDER_LEFT_OUT);
			}
			state = check_act_status(ACT_QUEUE_Cylinder_slider_left, state, TAKE_ROCKET_LEFT_IN_2, ERROR);
			state = check_act_status(ACT_QUEUE_Cylinder_slider_right, state, TAKE_ROCKET_LEFT_IN, GET_BACK);
			break;

		case TAKE_ROCKET_LEFT_IN_2:

			if(entrance){
					ACT_push_order( ACT_CYLINDER_SLIDER_LEFT , ACT_CYLINDER_SLIDER_LEFT_IN);
			}
			state = check_act_status(ACT_QUEUE_Cylinder_slider_left, state, GET_BACK_2, ERROR);
			state = check_act_status(ACT_QUEUE_Cylinder_slider_right, state, GET_BACK, GET_BACK);
			break;

		case GET_BACK_2:
			state = try_going(400, COLOR_Y(1150), state, DEPOSE_2, DEPOSE_2, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case DEPOSE_2:
			state = try_going(300, COLOR_Y(1000), state, STOP_POMPE_LEFT_2, STOP_POMPE_LEFT_2, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case STOP_POMPE_LEFT_2:

			if(entrance){
					ACT_push_order( ACT_POMPE_SLIDER_LEFT , ACT_POMPE_STOP );
			}
			state = check_act_status(ACT_QUEUE_Pompe_act_slider_left, state, GET_OUT, GET_OUT);
			state = check_act_status(ACT_QUEUE_Pompe_act_slider_right, state, STOP_POMPE_LEFT, GET_OUT);
			break;


		case GET_IN_DIRECT_3:
			state = try_going(400, COLOR_Y(1150), state, GO_TO_ROCKET_3, MOVE_BACK, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case GO_TO_ROCKET_3:
			if(global.color == YELLOW){
				state = try_going(260, COLOR_Y(1150), state, PROTECT_FALL_OUT_3, PROTECT_FALL_OUT_3, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			}else{
				state = try_going(260, COLOR_Y(1150), state, PROTECT_FALL_OUT_3, PROTECT_FALL_OUT_3, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			}
			break;

		case PROTECT_FALL_OUT_3:
			if(entrance){
					ACT_push_order( ACT_CYLINDER_SLIDER_RIGHT , ACT_CYLINDER_SLIDER_RIGHT_ALMOST_OUT);
			}
			state = check_act_status(ACT_QUEUE_Cylinder_slider_right, state, PROTECT_FALL_IN_3, ERROR);
			break;

		case PROTECT_FALL_IN_3:
			if(entrance){
					ACT_push_order( ACT_CYLINDER_SLIDER_RIGHT , ACT_CYLINDER_SLIDER_RIGHT_IN);
			}
			state = check_act_status(ACT_QUEUE_Cylinder_slider_right, state, TAKE_ROCKET_LEFT_OUT_3, ERROR);
			state = check_act_status(ACT_QUEUE_Cylinder_elevator_right, state, GO_TO_START_POINT, TAKE_ROCKET_LEFT_OUT_3);
			break;

		case TAKE_ROCKET_LEFT_OUT_3:

			if(entrance){
					ACT_push_order( ACT_POMPE_SLIDER_LEFT , ACT_POMPE_NORMAL );
					ACT_push_order( ACT_CYLINDER_SLIDER_LEFT , ACT_CYLINDER_SLIDER_LEFT_OUT);
			}
			state = check_act_status(ACT_QUEUE_Cylinder_slider_left, state, TAKE_ROCKET_LEFT_IN_3, ERROR);
			break;

		case TAKE_ROCKET_LEFT_IN_3:

			if(entrance){
					ACT_push_order( ACT_CYLINDER_SLIDER_LEFT , ACT_CYLINDER_SLIDER_LEFT_IN);
			}
			state = check_act_status(ACT_QUEUE_Cylinder_slider_left, state, GET_BACK_3, ERROR);
			break;

		case GET_BACK_3:
			state = try_going(400, COLOR_Y(1150), state, DEPOSE_3, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case DEPOSE_3:
			state = try_going(300, COLOR_Y(1000), state, STOP_POMPE_LEFT_3, ERROR, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case STOP_POMPE_LEFT_3:

			if(entrance){
					ACT_push_order( ACT_POMPE_SLIDER_LEFT , ACT_POMPE_STOP );
			}
			state = check_act_status(ACT_QUEUE_Pompe_act_slider_left, state, GET_OUT, GET_OUT);
			break;

		case DEPOSE_MODULE:
			state = check_sub_action_result(sub_harry_cylinder_depose_manager(), state, DONE, ERROR); //
			break;

		case ERROR_GET_OUT:
			state = try_go_angle((PI4096*3/4), state, DEPOSE_MODULE, GET_OUT, SLOW, ANY_WAY, END_AT_LAST_POINT);
			break;

		case ERROR_GET_BACK:
			state = try_go_angle((PI4096*3/4), state, DEPOSE_MODULE, GET_BACK, SLOW, ANY_WAY, END_AT_LAST_POINT);
			break;


			// l'autre fusée

		case GET_IN_DIRECT_4:
			state = try_going(400, COLOR_Y(1150), state, GO_TO_ROCKET_4, MOVE_BACK, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case GO_TO_ROCKET_4:
			if(global.color == YELLOW){
				state = try_going(260, COLOR_Y(1150), state, PROTECT_FALL_OUT_4, PROTECT_FALL_OUT_4, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			}else{
				state = try_going(260, COLOR_Y(1150), state, PROTECT_FALL_OUT_4, PROTECT_FALL_OUT_4, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			}

			break;

		case PROTECT_FALL_OUT_4:
			if(entrance){
					ACT_push_order( ACT_CYLINDER_SLIDER_RIGHT , ACT_CYLINDER_SLIDER_RIGHT_ALMOST_OUT);
			}
			state = check_act_status(ACT_QUEUE_Cylinder_slider_right, state, PROTECT_FALL_IN_4, ERROR);
			break;

		case PROTECT_FALL_IN_4:
			if(entrance){
					ACT_push_order( ACT_CYLINDER_SLIDER_RIGHT , ACT_CYLINDER_SLIDER_RIGHT_IN);
			}
			state = check_act_status(ACT_QUEUE_Cylinder_slider_right, state, TAKE_ROCKET_LEFT_OUT_4, ERROR);
			break;

		case TAKE_ROCKET_LEFT_OUT_4:

			if(entrance){
					ACT_push_order( ACT_POMPE_SLIDER_LEFT , ACT_POMPE_NORMAL );
					ACT_push_order( ACT_CYLINDER_SLIDER_LEFT , ACT_CYLINDER_SLIDER_LEFT_OUT);
			}
			state = check_act_status(ACT_QUEUE_Cylinder_slider_left, state, TAKE_ROCKET_LEFT_IN_4, ERROR);
			break;

		case TAKE_ROCKET_LEFT_IN_4:

			if(entrance){
					ACT_push_order( ACT_CYLINDER_SLIDER_LEFT , ACT_CYLINDER_SLIDER_LEFT_IN);
			}
			state = check_act_status(ACT_QUEUE_Cylinder_slider_left, state, GET_BACK_4, ERROR);
			break;

		case GET_BACK_4:
			state = try_going(400, COLOR_Y(1150), state, DEPOSE_4, DEPOSE_4, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case DEPOSE_4:
			state = try_going(300, COLOR_Y(1000), state, STOP_POMPE_LEFT_4, STOP_POMPE_LEFT_4, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case STOP_POMPE_LEFT_4:

			if(entrance){
					ACT_push_order( ACT_POMPE_SLIDER_LEFT , ACT_POMPE_STOP );
			}
			state = check_act_status(ACT_QUEUE_Pompe_act_slider_left, state, GET_OUT, GET_OUT);
			break;

		case GET_OUT:
			state = try_going(400, COLOR_Y(1150), state, GO_TO_START_POINT, GO_TO_START_POINT, FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case GO_TO_START_POINT:
			if(global.color == YELLOW){
				state = try_going_multipoint(curve_to_start_point_yellow, 5, state, PUSH_MODULES_START_ZONE, PUSH_MODULES_START_ZONE, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);
			}else{
				state = try_going_multipoint(curve_to_start_point_blue, 5, state, PUSH_MODULES_START_ZONE, PUSH_MODULES_START_ZONE, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);
			}
			break;

		case PUSH_MODULES_START_ZONE:
			state = try_going_multipoint(curve_push, 4, state, MOVE_BACK, GO_TO_START_POINT, FORWARD, DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case MOVE_BACK:
			state = try_going(600, COLOR_Y(1000), state, GET_IN_DIRECT, GET_IN_DIRECT, FAST, BACKWARD, NO_DODGE_AND_WAIT, END_AT_BRAKE);
			break;

		case ERROR:
			//RESET_MAE();

			on_turning_point();
			return NOT_HANDLED;
			break;

		case DONE:
			//RESET_MAE();
			on_turning_point();
			return END_OK;
			break;

		default:
			if(entrance)
				debug_printf("default case in sub_push_modules_bretagne\n");
			break;
	}

	return IN_PROGRESS;
}


error_e sub_harry_launch_prise_module_start_centre(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_LAUNCH_PRISE_MODULE,
				INIT,
				GET_OUT_START_ZONE,
				PUSH_MODULES,
				ERROR,
				DONE
			);

	#define SIZE_OUR_MODULES_WITH_ROCKET  (2)   // Prise des modules suivi de la fusée multicouleur
	/*const get_this_module_s our_modules_with_rocket[SIZE_OUR_MODULES_WITH_ROCKET] = {
			{.numero = MODULE_OUR_START,		.side = COLOR_EXP(LEFT, RIGHT)},	// Utiliser COLOR_EXP pour changer le côté de stockage suivant la couleur
			{.numero = MODULE_OUR_SIDE, 		.side = COLOR_EXP(LEFT, RIGHT)}
	};*/

	switch(state){
		case INIT:
			state = try_going(global.pos.x + 100, global.pos.y, state, GET_OUT_START_ZONE, ERROR, FAST,ANY_WAY, DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			//state = GET_OUT_START_ZONE;
			break;

		case GET_OUT_START_ZONE:
			//state = check_sub_action_result(sub_harry_prise_modules_manager(our_modules_with_rocket, SIZE_OUR_MODULES_WITH_ROCKET), state, DONE, ERROR);
			state = check_sub_action_result(sub_harry_rocket_monocolor(), state, DONE, ERROR);
			break;

		/*case PUSH_MODULES:
			state = check_sub_action_result(sub_harry_push_module_north(), state, DONE, ERROR);
			break;*/

		case ERROR:
			on_turning_point();
			return NOT_HANDLED;
			break;

		case DONE:
			on_turning_point();
			return END_OK;
			break;

		default:
			if(entrance)
				debug_printf("default case in sub_push_modules_bretagne\n");
			break;
	}

	return IN_PROGRESS;
}


error_e sub_harry_push_module_north(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_PUSH_MODULE_NORTH,
				INIT,
				GO_TO_MODULE,
				PUSH_MODULES,
				ERROR,
				DONE
			);

	switch(state){
		case INIT:
			//state = try_going(global.pos.x + 100, global.pos.y, state, GO_TO_MODULE, ERROR, FAST,ANY_WAY, DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			state = GO_TO_MODULE;
			break;

		case GO_TO_MODULE:
			state = check_sub_action_result(sub_harry_prise_module_unicolor_north(LEFT), state, DONE, ERROR);
			break;


		case ERROR:
			on_turning_point();
			return NOT_HANDLED;
			break;

		case DONE:
			on_turning_point();
			return END_OK;
			break;

		default:
			if(entrance)
				debug_printf("default case in sub_push_modules_bretagne\n");
			break;
	}

	return IN_PROGRESS;
}

