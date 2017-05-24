#include "action_small.h"
#include "../../propulsion/movement.h"
#include "../../propulsion/astar.h"
#include "../../QS/QS_stateMachineHelper.h"
#include "../../QS/QS_types.h"
#include "../../QS/QS_outputlog.h"
#include "../../QS/QS_IHM.h"
#include "../../QS/QS_CapteurCouleurCW.h"
#include  "../../utils/generic_functions.h"
#include "../../actuator/act_functions.h"
#include "../../actuator/queue.h"
#include "../../utils/actionChecker.h"
#include "../../elements.h"
#include "../../high_level_strat.h"
#include "../actions_both_2017.h"


error_e sub_act_anne_return_module(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_ACT_ANNE_RETURN_MODULE,
			INIT,
			MAGIC_ARM_GO_OUT,
			MAGIC_ARM_TAKE_CURRENT_POS,
			WAIT_ADV_COLOR,
			WAIT_WHITE_COLOR,
			WAIT_TIMER,
			MAGIC_ARM_GO_IN,

			ERROR_DISABLE_ACT,
			ERROR,
			DONE
		);

	static time32_t time_timeout = 0;
	#define TIMEOUT_TURN		(3000)
	#define TIMEOUT_TIMER		(350)

	switch(state){
		case INIT:
			state = MAGIC_ARM_GO_OUT;
			break;

		case MAGIC_ARM_GO_OUT:
			if(entrance){
				ACT_push_order(ACT_SMALL_MAGIC_ARM, ACT_SMALL_MAGIC_ARM_OUT);
			}
			state = check_act_status(ACT_QUEUE_Small_magic_arm, state, MAGIC_ARM_TAKE_CURRENT_POS, MAGIC_ARM_TAKE_CURRENT_POS);
			break;

		case MAGIC_ARM_TAKE_CURRENT_POS:
			if(entrance){
				ACT_push_order(ACT_SMALL_MAGIC_ARM, ACT_SMALL_MAGIC_ARM_CURRENT_POS);
				ACT_push_order(ACT_SMALL_MAGIC_COLOR, ACT_SMALL_MAGIC_COLOR_NORMAL_SPEED);
			}
			state = check_act_status(ACT_QUEUE_Small_magic_arm, state, WAIT_ADV_COLOR, WAIT_ADV_COLOR);
			break;

		case WAIT_ADV_COLOR:
			if(entrance){
				time_timeout = global.absolute_time + TIMEOUT_TURN;
			}
			if(global.absolute_time > time_timeout){
				state = MAGIC_ARM_GO_IN;
			}else if((global.color == BLUE && !CW_is_color_detected(CW_SENSOR_SMALL, CW_Channel_Yellow, TRUE))
				|| (global.color == YELLOW && !CW_is_color_detected(CW_SENSOR_SMALL, CW_Channel_Blue, TRUE))){
				state = WAIT_WHITE_COLOR;
			}
			break;

		case WAIT_WHITE_COLOR:
			if(global.absolute_time > time_timeout){
				state = MAGIC_ARM_GO_IN;
			}else if(!CW_is_color_detected(CW_SENSOR_SMALL, CW_Channel_White, TRUE)){
				state = WAIT_TIMER;
			}
			break;

		case WAIT_TIMER:
			if(entrance){
				time_timeout = global.absolute_time + TIMEOUT_TIMER;
			}
			if(global.absolute_time > time_timeout){
				state = MAGIC_ARM_GO_IN;
			}
			break;

		case MAGIC_ARM_GO_IN:
			if(entrance){
				ACT_push_order(ACT_SMALL_MAGIC_COLOR, ACT_SMALL_MAGIC_COLOR_ZERO_SPEED);
				ACT_push_order(ACT_SMALL_MAGIC_ARM, ACT_SMALL_MAGIC_ARM_IN);
			}
			state = check_act_status(ACT_QUEUE_Small_magic_arm, state, DONE, DONE);
			break;

		case DONE:
			RESET_MAE();
			on_turning_point();
			return END_OK;
			break;

		case ERROR:
			RESET_MAE();
			on_turning_point();
			return NOT_HANDLED;
			break;

		default:
			if(entrance)
				debug_printf("default case in sub_act_anne_return_module\n");
			break;
	}

	return IN_PROGRESS;
}


#define DISABLE_TAKE_ROCKET_SPEED

#if defined(DISABLE_TAKE_ROCKET_SPEED)

// Subaction actionneur de prise fusée v2
error_e sub_act_anne_take_rocket_down_to_top(moduleRocketLocation_e rocket, Uint8 max_modules_to_take){

	//ATTENTION ! Verfifier de quel cote de la pompe est branche !!!

	CREATE_MAE_WITH_VERBOSE(SM_ID_ACT_ANNE_TAKE_ROCKET,
			INIT,
			COMPUTE_NEXT_CYLINDER,
			COMPUTE_ACTION,

			ACTION_PREPARE_ARM_AND_BALANCER,
			ACTION_MOVE_AWAY_MULTIFONCTION,
			AVANCE,
			AVANCE_ERROR,
			ACTION_GO_TAKE_CYLINDER,

			ACTION_BRING_BACK_CYLINDER,
			RECULE,
			RECULE_ERROR,
			ACTION_LOCK_MULTIFUNCTION,
			STOP_POMPE_SLIDER,
			ACTION_UNLOCK_MULTIFUNCTION,
			ACTION_BRING_UP_CYLINDER_WAIT_FOR_SLOPE,
			ACTION_RISE_SLOPE,
			ACTION_BRING_CYLINDER_TO_TOP,
			ACTION_STOCK_UP_CYLINDER,
			ACTION_PUT_CYLINDER_IN_CONTAINER,
			ACTION_PUT_SLOPE_VERY_UP,

			ERROR,
			DONE
		);

	// Positions du robot
	static GEOMETRY_point_t take_pos;  // position du robot lors de la prise
	static GEOMETRY_point_t store_pos; // position du robot lors du stockage
	static Uint8 modules_taken;

	static bool_e moduleToTake = FALSE;		// Module en cours de prise
	static error_e state1 = IN_PROGRESS, state2 = IN_PROGRESS, state3 = IN_PROGRESS;
	static moduleType_e moduleType = MODULE_EMPTY;
	static time32_t time_timeout;
	static Uint8 nbEssais;

#ifdef DISPLAY_STOCKS
	if(entrance){
		debug_printf("---------- MODULE_STOCK_SMALL ---------\n");
		STOCKS_print(MODULE_STOCK_SMALL);
	}
#endif

	switch(state){

		case INIT:

			if(rocket == MODULE_ROCKET_MONO_OUR_SIDE){
				if(global.color == BLUE){
					moduleType = MODULE_BLUE;
				}else{
					moduleType = MODULE_YELLOW;
				}
			}else{
				moduleType = MODULE_POLY;
			}
			moduleToTake = NO_SIDE;

			// Calcul des positions
			store_pos.x = global.pos.x;
			store_pos.y = global.pos.y;
			compute_take_point_rocket(&take_pos, NULL, store_pos, global.pos.angle, 70);//derniere valeur a modifier pour changer la distance d'avancement et de recule lors de la prise
			debug_printf("Take pos computed is (%d;%d)\n", take_pos.x, take_pos.y );

			modules_taken = 0;
			state = COMPUTE_NEXT_CYLINDER;
			break;

		case COMPUTE_NEXT_CYLINDER:
			if(ELEMENTS_get_flag(FLAG_ANNE_DISABLE_MODULE)){
				moduleToTake = FALSE;
			}else if(ROCKETS_isEmpty(rocket)){
				debug_printf("LA FUSEE EST VIDE");
				moduleToTake = FALSE;		// La fusée est vide, nous avons fini.
			}else if(STOCKS_isFull(MODULE_STOCK_SMALL)){
				debug_printf("ON REFUSE LA PRISE, le stock est plein");
				moduleToTake = FALSE;
			}else if(modules_taken == max_modules_to_take){
				debug_printf("ON REFUSE LA PRISE, on nous a demandé d'en prendre seulement %d\n", max_modules_to_take); // On ne doit pas prendre les modules suivants (ceci est un choix de l'utilisateur)
			}else
				moduleToTake = TRUE;		// Le stock est plein

			if(moduleToTake)
				modules_taken++; // On incrémente l'index pour le prochain passage

			//S'il y a un module dans le balancer et au dessus, et que la position color sous le balancer est vide
			if(!STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_BALANCER , MODULE_STOCK_SMALL)&&!STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_CONTAINER , MODULE_STOCK_SMALL)&&STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_COLOR , MODULE_STOCK_SMALL)){
				//on fait progresser le balancer
				sub_act_anne_mae_prepare_modules_for_dispose(TRUE);
			}

			state = COMPUTE_ACTION;
			break;

		case COMPUTE_ACTION:
			if(moduleToTake == FALSE){
				state = DONE; // On a fini ou rien n'est possible de faire
			}else{
				if(modules_taken ==1){
					state = ACTION_PREPARE_ARM_AND_BALANCER;
				}else{
					state = ACTION_MOVE_AWAY_MULTIFONCTION;
				}
			}
			break;

		case ACTION_PREPARE_ARM_AND_BALANCER:
			if(entrance){
				ACT_push_order(ACT_SMALL_CYLINDER_BALANCER, ACT_SMALL_CYLINDER_BALANCER_IN);
				ACT_push_order(ACT_SMALL_CYLINDER_ARM, ACT_SMALL_CYLINDER_ARM_TAKE);
				state1 = IN_PROGRESS;
				state2 = IN_PROGRESS;
			}

			if(state1 == IN_PROGRESS){
				state1 = check_act_status(ACT_QUEUE_Small_cylinder_balancer, IN_PROGRESS, END_OK, NOT_HANDLED);
			}

			if(state2 == IN_PROGRESS){
				state2 = check_act_status(ACT_QUEUE_Small_cylinder_arm, IN_PROGRESS, END_OK, NOT_HANDLED);
			}

			if(state1 != IN_PROGRESS && state2 != IN_PROGRESS){
				state = ACTION_MOVE_AWAY_MULTIFONCTION;
			}

			break;

		case ACTION_MOVE_AWAY_MULTIFONCTION:
			state = ACTION_GO_TAKE_CYLINDER;

//			if (entrance){
//				ACT_push_order( ACT_SMALL_CYLINDER_MULTIFONCTION , ACT_SMALL_CYLINDER_MULTIFONCTION_OUT); //position a definir, entre lock et puch
////				ACT_push_order( ACT_SMALL_CYLINDER_MULTIFONCTION , ACT_SMALL_CYLINDER_MULTIFONCTION_PUSH);
//			}
//			state = check_act_status(ACT_QUEUE_Small_cylinder_multifonction, state, AVANCE, AVANCE);
			break;

		case ACTION_GO_TAKE_CYLINDER:
			if(entrance){
				//On active la pompe avant d'avancer
				ACT_push_order_with_param( ACT_SMALL_POMPE_PRISE , ACT_POMPE_SMALL_SLIDER_NORMAL, 100);
				ACT_push_order( ACT_SMALL_CYLINDER_SLIDER , ACT_SMALL_CYLINDER_SLIDER_OUT);

				//On redescend et active la pompe de l'elevator pour la prochaine prise
				ACT_push_order( ACT_SMALL_CYLINDER_ELEVATOR , ACT_SMALL_CYLINDER_ELEVATOR_BOTTOM);
				ACT_push_order_with_param( ACT_SMALL_POMPE_DISPOSE, ACT_POMPE_SMALL_ELEVATOR_NORMAL, 100);

				state1 = IN_PROGRESS;
				state2 = IN_PROGRESS;
				state3 = IN_PROGRESS;

			}

			//Et on avance en meme temps :

			if(state1 == IN_PROGRESS)
				state1 = check_act_status(ACT_QUEUE_Small_cylinder_slider, IN_PROGRESS, END_OK, NOT_HANDLED);

			if(state2 == IN_PROGRESS)
				state2 = check_act_status(ACT_QUEUE_Small_cylinder_elevator, IN_PROGRESS, END_OK, NOT_HANDLED);

			if(state3 == IN_PROGRESS)
				state3 =try_going(take_pos.x, take_pos.y, IN_PROGRESS, END_OK, NOT_HANDLED, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);

			if(state1 != IN_PROGRESS && state2 != IN_PROGRESS && state3 != IN_PROGRESS ){
				if(state3 == NOT_HANDLED){
					//on a pas avancé physiquement, on retente
					state = AVANCE;
					nbEssais = 0;
				}else{
					//On continue
					state = ACTION_BRING_BACK_CYLINDER;
				}
			}
			break;

		case AVANCE:
			if(entrance)
			{
				nbEssais += 1;
				if(nbEssais>=3)
					state = ACTION_BRING_BACK_CYLINDER;//ou DONE ? est-ce-qu'on continue la prise?
			}
			else	//On avance en meme temps pour la prise, car on se deplace plus vite que le slider
				state = try_going(take_pos.x, take_pos.y, state, ACTION_BRING_BACK_CYLINDER, AVANCE_ERROR, FAST, FORWARD, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

		case AVANCE_ERROR:
			//Si on a pas reussi a bien avancer, on reessaie
			state = try_going(store_pos.x, store_pos.y, state, AVANCE, AVANCE, FAST, BACKWARD, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

		case ACTION_BRING_BACK_CYLINDER:
			if(entrance){
				// On allume la pompe seulement a ce moment pour bien laisser le temps au stockage du module ^précédent de se faire
				ACT_push_order_with_param( ACT_SMALL_POMPE_DISPOSE, ACT_POMPE_SMALL_ELEVATOR_NORMAL, 100);
				//On rentre le bras dans le robot
				ACT_push_order( ACT_SMALL_CYLINDER_SLIDER, ACT_SMALL_CYLINDER_SLIDER_IN );

				// On redescent les slopes pour le stockage suivant
				if(STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_SLOPE, MODULE_STOCK_SMALL)){
					ACT_push_order(ACT_SMALL_CYLINDER_SLOPE, ACT_SMALL_CYLINDER_SLOPE_DOWN);
				}

				state1 = IN_PROGRESS;
				state2 = IN_PROGRESS;
			}

			// Vérification des ordres effectués
			if(state1 == IN_PROGRESS)
				state1 = check_act_status(ACT_QUEUE_Small_cylinder_slider, IN_PROGRESS, END_OK, NOT_HANDLED);

			if(state2 == IN_PROGRESS)
				state2 = try_going(store_pos.x, store_pos.y, IN_PROGRESS, END_OK, NOT_HANDLED, FAST, BACKWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);

			//On attends pour securiser le ventousage par l'elevator
			if(state1 != IN_PROGRESS && state2 != IN_PROGRESS){

				if (state2 == NOT_HANDLED){
					//on s'est pas reculé, donc on n'as pas d'espace suffisant pour monter le module
					state = RECULE;
					//On initialise le nombre d'essai
					nbEssais = 0;
				}else{
					//On continue
					state = ACTION_LOCK_MULTIFUNCTION;
				}

				// On met à jour les données
				STOCKS_addModule(moduleType, STOCK_POS_ENTRY, MODULE_STOCK_SMALL);
				ROCKETS_removeModule(rocket);
				moduleToTake = FALSE;

				//C'etait le dernier, on finit et on montera le module avec la sub prepare
				if(ROCKETS_isEmpty(rocket)){
					sub_act_anne_mae_store_modules(TRUE);
					state = DONE;
				}

			}
			break;

		case RECULE:{
			static enum state_e success_state;

			//On recule lors de la prise, car on se deplace plus vite que le slider
			if(entrance)
			{
				if(ROCKETS_isEmpty(rocket))
				{
					sub_act_anne_mae_store_modules(TRUE);
					success_state = DONE;
				}
				else
					success_state = ACTION_LOCK_MULTIFUNCTION;
			}
			//a la prise du dernier module on part en slow pour ralentir la chute du dernier
			state = try_going(store_pos.x, store_pos.y, state, success_state, RECULE_ERROR, (modules_taken==max_modules_to_take-1)?SLOW:FAST, BACKWARD, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);

			break;}

		case RECULE_ERROR:
			if(entrance)
			{
				nbEssais += 1;
				if(nbEssais>=3)
					state = DONE;
			}
			else
				state= try_going(take_pos.x, take_pos.y, state, RECULE, RECULE, FAST, FORWARD, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

		case ACTION_LOCK_MULTIFUNCTION:

//			if (entrance){
//				ACT_push_order( ACT_SMALL_CYLINDER_MULTIFONCTION , ACT_SMALL_CYLINDER_MULTIFONCTION_LOCK);
//			}
			if(STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_SLOPE , MODULE_STOCK_SMALL)){
				//state = check_act_status(ACT_QUEUE_Small_cylinder_multifonction, state, STOP_POMPE_SLIDER, STOP_POMPE_SLIDER);
				state = STOP_POMPE_SLIDER;
			}else{
				//state = check_act_status(ACT_QUEUE_Small_cylinder_multifonction, state, COMPUTE_NEXT_CYLINDER, COMPUTE_NEXT_CYLINDER);
				state = COMPUTE_NEXT_CYLINDER;
			}
			break;

		case STOP_POMPE_SLIDER:
			if(entrance){
				ACT_push_order_with_param( ACT_SMALL_POMPE_PRISE, ACT_POMPE_SMALL_SLIDER_REVERSE, 100);
				time_timeout = global.absolute_time + 1000;
			}

			// On attend une seconde le temps que le ventousage se fasse bien
			if(global.absolute_time > time_timeout){
				state = ACTION_UNLOCK_MULTIFUNCTION;

				// Mise a jour
				STOCKS_makeModuleProgressTo(STOCK_PLACE_ENTRY_TO_ELEVATOR, MODULE_STOCK_SMALL);
			}
			break;

		case ACTION_UNLOCK_MULTIFUNCTION:
			state = ACTION_BRING_UP_CYLINDER_WAIT_FOR_SLOPE;
//			if (entrance){
//				ACT_push_order( ACT_SMALL_CYLINDER_MULTIFONCTION , ACT_SMALL_CYLINDER_MULTIFONCTION_OUT);
//			}
//			state = check_act_status(ACT_QUEUE_Small_cylinder_multifonction, state, ACTION_BRING_UP_CYLINDER, ERROR);
			break;

		case ACTION_BRING_UP_CYLINDER_WAIT_FOR_SLOPE:
			if(entrance){
				//Si le stockage est possible
				if(STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_SLOPE, MODULE_STOCK_SMALL)){
					//On monte l'elevator jusqu'a la position d'attente de verouillage su slope
					ACT_push_order( ACT_SMALL_CYLINDER_ELEVATOR, ACT_SMALL_CYLINDER_ELEVATOR_WAIT_FOR_SLOPE);
				}
			}

			if(STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_SLOPE, MODULE_STOCK_SMALL)){
				// Vérification des ordres effectués
				state = check_act_status(ACT_QUEUE_Small_cylinder_elevator, state, ACTION_RISE_SLOPE, ERROR);
			}else{
				//Plus de place on arrete la prise
				moduleToTake = FALSE;
				state = COMPUTE_NEXT_CYLINDER;
			}
			break;

		case ACTION_RISE_SLOPE:
				if(entrance){
					//On verrouille un peu le slope
					ACT_push_order( ACT_SMALL_CYLINDER_SLOPE, ACT_SMALL_CYLINDER_SLOPE_ALMOST_UP);
				}

				// Vérification des ordres effectués
				state = check_act_status(ACT_QUEUE_Small_cylinder_slope, state, ACTION_BRING_CYLINDER_TO_TOP, ACTION_BRING_CYLINDER_TO_TOP);

				break;

		case ACTION_BRING_CYLINDER_TO_TOP:
			if(entrance){
				//On amene le'elevator au top
				ACT_push_order( ACT_SMALL_CYLINDER_ELEVATOR, ACT_SMALL_CYLINDER_ELEVATOR_TOP);
			}

			// Vérification des ordres effectués
			state = check_act_status(ACT_QUEUE_Small_cylinder_elevator, state, ACTION_STOCK_UP_CYLINDER, ACTION_STOCK_UP_CYLINDER);
			break;

		case ACTION_STOCK_UP_CYLINDER:
			if(entrance){
				//On finit de verouiller le slope
				ACT_push_order( ACT_SMALL_CYLINDER_SLOPE, ACT_SMALL_CYLINDER_SLOPE_UP);
			}

			// Vérification des ordres effectués
			state = check_act_status(ACT_QUEUE_Small_cylinder_slope, state, ACTION_PUT_CYLINDER_IN_CONTAINER, ACTION_BRING_CYLINDER_TO_TOP);

			if(ON_LEAVE()){
				// Mise à jour des données
				STOCKS_makeModuleProgressTo(STOCK_PLACE_ELEVATOR_TO_CONTAINER, MODULE_STOCK_SMALL);
			}
			break;


		case ACTION_PUT_CYLINDER_IN_CONTAINER:
			if(entrance){
				//on souffle un peu pour faciliter la depose
				ACT_push_order_with_param(ACT_SMALL_POMPE_DISPOSE, ACT_POMPE_SMALL_ELEVATOR_REVERSE, 100);

				time_timeout = global.absolute_time + 2000;
			}
			if(global.absolute_time > time_timeout){
				state = ACTION_PUT_SLOPE_VERY_UP;
			}
			//state = wait_time(1000, state, ACTION_PUT_SLOPE_DOWN);	// On attends un peu le temps que le cylindre roule
			break;

		case ACTION_PUT_SLOPE_VERY_UP:
			if(entrance){
				//On pousse le module avec le slope
				ACT_push_order(ACT_SMALL_CYLINDER_SLOPE, ACT_SMALL_CYLINDER_SLOPE_VERY_UP);
			}

			// Vérification des ordres effectués
			// Et on recommence une prise
			state = check_act_status(ACT_QUEUE_Small_cylinder_slope, state, COMPUTE_NEXT_CYLINDER, COMPUTE_NEXT_CYLINDER);
			break;

		case DONE:
			if(entrance){
				// On active la dépose
				set_sub_act_enable(SUB_ANNE_DEPOSE_MODULES, TRUE);
				// On range tout : Non tout doit être bon
				//ACT_push_order(ACT_SMALL_CYLINDER_SLIDER,        ACT_SMALL_CYLINDER_SLIDER_IN);
				//ACT_push_order(ACT_SMALL_CYLINDER_MULTIFONCTION, ACT_SMALL_CYLINDER_MULTIFONCTION_LOCK);
				//ACT_push_order(ACT_SMALL_CYLINDER_ELEVATOR,      ACT_SMALL_CYLINDER_ELEVATOR_BOTTOM);
			}
			RESET_MAE();
			on_turning_point();
			return END_OK;
			break;

		case ERROR:
			if(entrance){
				// On range tout
				ACT_push_order(ACT_SMALL_CYLINDER_SLIDER,        ACT_SMALL_CYLINDER_SLIDER_IN);
				//ACT_push_order(ACT_SMALL_CYLINDER_MULTIFONCTION, ACT_SMALL_CYLINDER_MULTIFONCTION_LOCK);
				ACT_push_order(ACT_SMALL_CYLINDER_ELEVATOR,      ACT_SMALL_CYLINDER_ELEVATOR_BOTTOM);
			}
			RESET_MAE();
			on_turning_point();
			return NOT_HANDLED;
			break;

		default:
			if(entrance)
				debug_printf("default case in sub_act_anne_take_rocket\n");
			break;
	}

	return IN_PROGRESS;
}
#else

// Subaction prise fusee plus rapide mais experimentale
error_e sub_act_anne_take_rocket_down_to_top(moduleRocketLocation_e rocket, bool_e module_very_down, bool_e module_down, bool_e module_top, bool_e module_very_top){

	//ATTENTION ! Verfifier de quel cote de la pompe est branche !!!

	CREATE_MAE_WITH_VERBOSE(SM_ID_ACT_ANNE_TAKE_ROCKET,
			INIT,
			COMPUTE_NEXT_CYLINDER,
			COMPUTE_ACTION,

			FIRST_ACTION_PREPARE_ARM_AND_BALANCER,
			FIRST_ACTION_CHECK_SLOPE_AND_ELEVATOR,
			ACTION_GO_TAKE_CYLINDER,
			AVANCE,
			AVANCE_ERROR,
			STORE_THE_TOP,
			REVERSE_POMPE_ELEVATOR,
			ACTION_BRING_BACK_CYLINDER,
			RECULE,
			RECULE_ERROR,
			SLIDER_IN,
			STOP_POMPE_SLIDER,
			BRING_UP_CYLINDER_WAIT_FOR_SLOPE,
			LOCK_THE_SLOPE,

			ERROR,
			DONE
		);

	// Positions du robot
	static GEOMETRY_point_t take_pos;  // position du robot lors de la prise
	static GEOMETRY_point_t store_pos; // position du robot lors du stockage

	static bool_e rocketSide[MAX_MODULE_ROCKET];
	static Uint8 indexSide = 0;
	static bool_e moduleToTake = FALSE;		// Module en cours de prise
	static error_e state1 = IN_PROGRESS, state2 = IN_PROGRESS, state3 = IN_PROGRESS, state4 = IN_PROGRESS;
	static moduleType_e moduleType = MODULE_EMPTY;
	static time32_t time_timeout;
	static bool_e prise_du_premier_module = TRUE;

#ifdef DISPLAY_STOCKS
	if(entrance){
		debug_printf("---------- MODULE_STOCK_SMALL ---------\n");
		STOCKS_print(MODULE_STOCK_SMALL);
	}
#endif

	switch(state){

		case INIT:

			if(rocket == MODULE_ROCKET_MONO_OUR_SIDE){
				if(global.color == BLUE){
					moduleType = MODULE_BLUE;
				}else{
					moduleType = MODULE_YELLOW;
				}
			}else{
				moduleType = MODULE_POLY;
			}
			moduleToTake = NO_SIDE;

			// Calcul des positions
			store_pos.x = global.pos.x;
			store_pos.y = global.pos.y;
			compute_take_point_rocket(&take_pos, NULL, store_pos, global.pos.angle, 70);//derniere valeur a modifier pour changer la distance d'avancement et de recule lors de la prise
			debug_printf("Take pos computed is (%d;%d)\n", take_pos.x, take_pos.y );

			rocketSide[0] = module_very_down;
			rocketSide[1] = module_down;
			rocketSide[2] = module_top;
			rocketSide[3] = module_very_top;
			state = COMPUTE_NEXT_CYLINDER;
			break;

		case COMPUTE_NEXT_CYLINDER:
			if(ELEMENTS_get_flag(FLAG_ANNE_DISABLE_MODULE)){
				moduleToTake = FALSE;
			}else if(ROCKETS_isEmpty(rocket) || indexSide >= 4){
				moduleToTake = FALSE;		// La fusée est vide, nous avons fini.
			}else if(rocketSide[indexSide] == FALSE){
				moduleToTake = FALSE; 	    // On ne doit pas prendre les modules suivants (ceci est un choix de l'utilisateur)
			}else if(rocketSide[indexSide] == TRUE && !STOCKS_isFull(MODULE_STOCK_SMALL)){
				moduleToTake = TRUE;		// On demande et il est dispo
			}else{
				moduleToTake = FALSE;		// Le stock est plein
			}

			indexSide++; // On incrémente l'index pour le prochain passage

			if(indexSide == 1){
				//La premiere fois on passe prise_du_premier_module a
				prise_du_premier_module = TRUE;
			}else{
				prise_du_premier_module = FALSE;
			}

			//S'il y a un module dans le balancer et au dessus, et que la position color sous le balancer est vide
			if(!STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_BALANCER , MODULE_STOCK_SMALL)&&!STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_CONTAINER , MODULE_STOCK_SMALL)&&STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_COLOR , MODULE_STOCK_SMALL)){
				//on fait progresser le balancer
				sub_act_anne_mae_prepare_modules_for_dispose(TRUE);
			}

			state = COMPUTE_ACTION;
			break;

		case COMPUTE_ACTION:
			if(moduleToTake == FALSE){
				state = DONE; // On a fini ou rien n'est possible de faire
			}else{
				if(prise_du_premier_module){
					//Lors de la prise du premier module, on initailise la position de nos actionneurs
					state = FIRST_ACTION_PREPARE_ARM_AND_BALANCER;
				}else{
					//Sinon on continue la prise
					state = ACTION_GO_TAKE_CYLINDER;
				}
			}
			break;

		case FIRST_ACTION_PREPARE_ARM_AND_BALANCER:
			if(entrance){
				ACT_push_order(ACT_SMALL_CYLINDER_BALANCER, ACT_SMALL_CYLINDER_BALANCER_IN);
				ACT_push_order(ACT_SMALL_CYLINDER_ARM, ACT_SMALL_CYLINDER_ARM_PREPARE_TO_TAKE);
				state1 = IN_PROGRESS;
				state2 = IN_PROGRESS;
			}

			if(state1 == IN_PROGRESS){
				state1 = check_act_status(ACT_QUEUE_Small_cylinder_balancer, IN_PROGRESS, END_OK, NOT_HANDLED);
			}

			if(state2 == IN_PROGRESS){
				state2 = check_act_status(ACT_QUEUE_Small_cylinder_arm, IN_PROGRESS, END_OK, NOT_HANDLED);
			}

			if(state1 != IN_PROGRESS && state2 != IN_PROGRESS){
				state = FIRST_ACTION_CHECK_SLOPE_AND_ELEVATOR;
			}

			break;

		case FIRST_ACTION_CHECK_SLOPE_AND_ELEVATOR:
			if(entrance){

				//On redescend et active la pompe de l'elevator pour la prochaine prise
				ACT_push_order( ACT_SMALL_CYLINDER_ELEVATOR , ACT_SMALL_CYLINDER_ELEVATOR_BOTTOM);
				ACT_push_order( ACT_SMALL_CYLINDER_SLOPE , ACT_SMALL_CYLINDER_SLOPE_DOWN);

				state1 = IN_PROGRESS;
				state2 = IN_PROGRESS;

			}

			if(state1 == IN_PROGRESS)
				state1 = check_act_status(ACT_QUEUE_Small_cylinder_slider, IN_PROGRESS, END_OK, NOT_HANDLED);

			if(state2 == IN_PROGRESS)
				state2 = check_act_status(ACT_QUEUE_Small_cylinder_elevator, IN_PROGRESS, END_OK, NOT_HANDLED);

			if(state1 != IN_PROGRESS && state2 != IN_PROGRESS){
				//On continue
				state = ACTION_GO_TAKE_CYLINDER;
			}
			break;

		case ACTION_GO_TAKE_CYLINDER:
			if(entrance){
				//On active la pompe avant d'avancer
				ACT_push_order_with_param( ACT_SMALL_POMPE_PRISE , ACT_POMPE_SMALL_SLIDER_NORMAL, 100);
				ACT_push_order( ACT_SMALL_CYLINDER_SLIDER , ACT_SMALL_CYLINDER_SLIDER_OUT);

				state1 = IN_PROGRESS;

				//un state aussi pour le try_going
				state2 = IN_PROGRESS;

				if(!prise_du_premier_module){
					//Si ce n'est pas le premier module, alors on a un module en cours de stockage et on finit de le stocker
					ACT_push_order(ACT_SMALL_CYLINDER_ELEVATOR , ACT_SMALL_CYLINDER_ELEVATOR_TOP);
					ACT_push_order(ACT_SMALL_CYLINDER_SLOPE, ACT_SMALL_CYLINDER_SLOPE_UP);
				}

				state3 = IN_PROGRESS;
				state4 = IN_PROGRESS;

			}

			//Et on avance en meme temps :

			if(state1 == IN_PROGRESS)
				state1 = check_act_status(ACT_QUEUE_Small_cylinder_slider, IN_PROGRESS, END_OK, NOT_HANDLED);

			if(state2 == IN_PROGRESS)
				state2 = try_going(take_pos.x, take_pos.y, IN_PROGRESS, END_OK, NOT_HANDLED, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);

			if(state3 == IN_PROGRESS && !prise_du_premier_module)
				state3 = check_act_status(ACT_QUEUE_Small_cylinder_elevator, IN_PROGRESS, END_OK, NOT_HANDLED);

			if(state4 == IN_PROGRESS && !prise_du_premier_module)
				state4 = check_act_status(ACT_QUEUE_Small_cylinder_slope, IN_PROGRESS, END_OK, NOT_HANDLED);

			if((state1 != IN_PROGRESS && state2 != IN_PROGRESS && prise_du_premier_module)||(state1 != IN_PROGRESS && state2 != IN_PROGRESS && state3 != IN_PROGRESS && state4 != IN_PROGRESS) ){
				if(state2 == NOT_HANDLED){
					//on a pas avancé physiquement, on retente
					state = AVANCE;
				}else{
					//On continue
					state = STORE_THE_TOP;
				}
			}
			break;

		case AVANCE:
			//On avance en meme temps pour la prise, car on se deplace plus vite que le slider
			state = try_going(take_pos.x, take_pos.y, state, STORE_THE_TOP, AVANCE_ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case AVANCE_ERROR:
			//Si on a pas reussi a bien avancer, on reessaie
			state = try_going(store_pos.x, store_pos.y, state, AVANCE, AVANCE, FAST, BACKWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case STORE_THE_TOP:
			if(entrance){
				//On pousse le cylindre avec l'air de la ventouse et verrouille le slope
				ACT_push_order_with_param( ACT_SMALL_POMPE_DISPOSE , ACT_POMPE_SMALL_ELEVATOR_REVERSE, 100);
				ACT_push_order(ACT_SMALL_CYLINDER_SLOPE, ACT_SMALL_CYLINDER_SLOPE_VERY_UP);

				//Un petit temps de deventousage est necessaire
				time_timeout = global.absolute_time + 1000;
			}

			if(global.absolute_time > time_timeout){
				state = check_act_status(ACT_QUEUE_Small_cylinder_slope, state, REVERSE_POMPE_ELEVATOR, REVERSE_POMPE_ELEVATOR);
			}
			break;

		case REVERSE_POMPE_ELEVATOR:
			if(entrance){
				//On pousse le cylindre avec l'air de la ventouse et verrouille le slope
				ACT_push_order(ACT_SMALL_CYLINDER_ELEVATOR, ACT_SMALL_CYLINDER_ELEVATOR_WAIT_FOR_SLOPE);
			}

			state = check_act_status(ACT_QUEUE_Small_cylinder_elevator, state, ACTION_BRING_BACK_CYLINDER, ACTION_BRING_BACK_CYLINDER);
			break;

		case ACTION_BRING_BACK_CYLINDER:
			if(entrance){
				//On active la pompe elevator
				ACT_push_order_with_param( ACT_SMALL_POMPE_DISPOSE , ACT_POMPE_SMALL_ELEVATOR_NORMAL, 100);

				//On rentre le bras dans le robot
				ACT_push_order( ACT_SMALL_CYLINDER_SLIDER, ACT_SMALL_CYLINDER_SLIDER_ALMOST_OUT_WITH_CYLINDER );
				//On baisse le bras en meme temps.
				ACT_push_order( ACT_SMALL_CYLINDER_ELEVATOR, ACT_SMALL_CYLINDER_ELEVATOR_BOTTOM);
				state1 = IN_PROGRESS;
				state2 = IN_PROGRESS;

				//On rajoute un state supplementaire pour le try_going
				state3 = IN_PROGRESS;
			}

			// Vérification des ordres effectués
			if(state1 == IN_PROGRESS){
				state1 = check_act_status(ACT_QUEUE_Small_cylinder_elevator, IN_PROGRESS, END_OK, NOT_HANDLED);
			}

			if(state2 == IN_PROGRESS){
				state2 = check_act_status(ACT_QUEUE_Small_cylinder_slider, IN_PROGRESS, END_OK, NOT_HANDLED);
			}

			if(state3 == IN_PROGRESS){
				state3 = try_going(store_pos.x, store_pos.y, IN_PROGRESS, END_OK, NOT_HANDLED, FAST, BACKWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			}

			//On attends pour securisé le ventousage par l'elevator
			if(state1 != IN_PROGRESS && state2 != IN_PROGRESS && state3 != IN_PROGRESS){

				if (state2 == NOT_HANDLED){
					//on s'est pas reculé, donc on n'as pas d'espace suffisant pour monter le module
					state = RECULE;
				}else{
					//On continue
					state = SLIDER_IN;
				}

				// On met à jour les données
				STOCKS_addModule(moduleType, STOCK_POS_ENTRY, MODULE_STOCK_SMALL);
				ROCKETS_removeModule(rocket);
				moduleToTake = FALSE;

				//C'etait le dernier, on finit et on montera le module avec la sub prepare
				if(ROCKETS_isEmpty(rocket)){
					sub_act_anne_mae_store_last_module(TRUE);
					state = DONE;
				}

			}
			break;

		case RECULE:
			//On recule lors de la prise, car on se deplace plus vite que le slider
			if(ROCKETS_isEmpty(rocket)){
				sub_act_anne_mae_store_last_module(TRUE);
				state = try_going(store_pos.x, store_pos.y, state, DONE, RECULE_ERROR, FAST, BACKWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			}else{
				state = try_going(store_pos.x, store_pos.y, state, SLIDER_IN, RECULE_ERROR, FAST, BACKWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			}
			break;

		case RECULE_ERROR:
			//Si on a pas reussi a bien reculer, on reessaie
			state= try_going(take_pos.x, take_pos.y, state, RECULE, RECULE, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case SLIDER_IN:

			if(entrance){

				// On allume la pompe de l'elevator
				ACT_push_order_with_param( ACT_SMALL_POMPE_DISPOSE, ACT_POMPE_SMALL_ELEVATOR_NORMAL, 100);

				//On rentre le bras dans le robot
				ACT_push_order( ACT_SMALL_CYLINDER_SLIDER, ACT_SMALL_CYLINDER_SLIDER_IN);
				state1 = IN_PROGRESS;

				//On prepare le slope
				if(STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_SLOPE , MODULE_STOCK_SMALL)){
					ACT_push_order( ACT_SMALL_CYLINDER_SLOPE, ACT_SMALL_CYLINDER_SLOPE_DOWN);
				}
				state2 = IN_PROGRESS;

				//Et on laisse un temps de ventousage
				time_timeout = global.absolute_time + 2000;
			}

			if(state1 == IN_PROGRESS){
				state1 = check_act_status(ACT_QUEUE_Small_cylinder_slider, IN_PROGRESS, END_OK, NOT_HANDLED);
			}

			if(state2 == IN_PROGRESS && STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_SLOPE, MODULE_STOCK_SMALL)){
				state2 = check_act_status(ACT_QUEUE_Small_cylinder_slider, IN_PROGRESS, END_OK, NOT_HANDLED);
			}

			if(global.absolute_time > time_timeout && state1 != IN_PROGRESS){
				if(state2 != IN_PROGRESS || !STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_SLOPE , MODULE_STOCK_SMALL)){
					if(STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_SLOPE , MODULE_STOCK_SMALL)){
						//state = check_act_status(ACT_QUEUE_Small_cylinder_multifonction, state, STOP_POMPE_SLIDER, STOP_POMPE_SLIDER);
						state = STOP_POMPE_SLIDER;
					}else{
						//state = check_act_status(ACT_QUEUE_Small_cylinder_multifonction, state, COMPUTE_NEXT_CYLINDER, COMPUTE_NEXT_CYLINDER);
						state = COMPUTE_NEXT_CYLINDER;
					}
				}
			}

			break;

		case STOP_POMPE_SLIDER:
			if(entrance){
				ACT_push_order_with_param( ACT_SMALL_POMPE_PRISE, ACT_POMPE_SMALL_SLIDER_REVERSE, 100);
				time_timeout = global.absolute_time + 1000;
			}

			// On attend une seconde le temps que le ventousage se fasse bien
			if(global.absolute_time > time_timeout){
				state = BRING_UP_CYLINDER_WAIT_FOR_SLOPE;

				// Mise a jour
				STOCKS_makeModuleProgressTo(STOCK_PLACE_ENTRY_TO_ELEVATOR, MODULE_STOCK_SMALL);
			}
			break;

		case BRING_UP_CYLINDER_WAIT_FOR_SLOPE:
			if(entrance){
				// Si le stockage est possible
				if(STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_SLOPE, MODULE_STOCK_SMALL)){
					//On monte l'elevator jusqu'a la position d'attente de verouillage su slope
					ACT_push_order( ACT_SMALL_CYLINDER_ELEVATOR, ACT_SMALL_CYLINDER_ELEVATOR_WAIT_FOR_SLOPE);
				}
			}

			if(STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_SLOPE, MODULE_STOCK_SMALL)){
				// Vérification des ordres effectués
				state = check_act_status(ACT_QUEUE_Small_cylinder_elevator, state, LOCK_THE_SLOPE, ERROR);
			}else{
				//Plus de place on arrete la prise
				moduleToTake = FALSE;
				state = COMPUTE_NEXT_CYLINDER;
			}
			break;

		case LOCK_THE_SLOPE:
			if(entrance){
				ACT_push_order(ACT_SMALL_CYLINDER_SLOPE, ACT_SMALL_CYLINDER_SLOPE_ALMOST_UP);
			}
			//Dans tout les cas on recommence une prise, car on pourra monter le slope plus tard
			state = check_act_status(ACT_QUEUE_Small_cylinder_slope, state, COMPUTE_NEXT_CYLINDER, COMPUTE_NEXT_CYLINDER);
			break;

		case DONE:
			if(entrance){
				// On active la dépose
				set_sub_act_enable(SUB_ANNE_DEPOSE_MODULES, TRUE);
				// On range tout : Non tout doit être bon
				//ACT_push_order(ACT_SMALL_CYLINDER_SLIDER,        ACT_SMALL_CYLINDER_SLIDER_IN);
				//ACT_push_order(ACT_SMALL_CYLINDER_MULTIFONCTION, ACT_SMALL_CYLINDER_MULTIFONCTION_LOCK);
				//ACT_push_order(ACT_SMALL_CYLINDER_ELEVATOR,      ACT_SMALL_CYLINDER_ELEVATOR_BOTTOM);
			}
			RESET_MAE();
			on_turning_point();
			return END_OK;
			break;

		case ERROR:
			if(entrance){
				// On range tout
				ACT_push_order(ACT_SMALL_CYLINDER_SLIDER,        ACT_SMALL_CYLINDER_SLIDER_IN);
				//ACT_push_order(ACT_SMALL_CYLINDER_MULTIFONCTION, ACT_SMALL_CYLINDER_MULTIFONCTION_LOCK);
				ACT_push_order(ACT_SMALL_CYLINDER_ELEVATOR,      ACT_SMALL_CYLINDER_ELEVATOR_BOTTOM);
			}
			RESET_MAE();
			on_turning_point();
			return NOT_HANDLED;
			break;

		default:
			if(entrance)
				debug_printf("default case in sub_act_anne_take_rocket\n");
			break;
	}

	return IN_PROGRESS;
}

#endif


#if defined(DISABLE_TAKE_ROCKET_SPEED)
// Subaction actionneur de stockage
error_e sub_act_anne_mae_store_modules(bool_e trigger){
	CREATE_MAE_WITH_VERBOSE(SM_ID_ACT_ANNE_MAE_STORE_MODULES,
			WAIT_TRIGGER,
			LAUNCH_CYLINDER_PREPARATION,
			INIT,
			COMPUTE_ACTION,

			// Le stockage des modules
			ELEVATOR_GO_BOTTOM,
			SLIDER_GO_IN,
			STOP_POMPE_SLIDER,
			CHECK_CONTAINER_IS_AVAILABLE,
			ELEVATOR_GO_MID_POS,
			PREPARE_SLOPE_FOR_ELEVATOR,
			ELEVATOR_WAIT_FOR_SLOPE,
			SLOPE_GO_ALMOST_UP_POS,
			ELEVATOR_GO_TOP_POS,
			STOCK_UP_CYLINDER,
			WAIT_STABILZATION,
			PUT_CYLINDER_IN_CONTAINER,
			SLOPE_GO_VERY_UP,

			// Actions à faire en fin de subaction
			ELEVATOR_GO_BOTTOM_TO_END,
			CHECK_DISPENSER_TO_END,
			DISPENSER_GO_UNLOCK_TO_END,
			WAIT_DISPENSER_TO_END,
			DISPENSER_GO_LOCK_TO_END,

			ERROR_DISABLE_ACT,
			ERROR,
			DONE

		);

	error_e ret = IN_PROGRESS;
	static time32_t time_timeout;

#ifdef DISPLAY_STOCKS
	if(entrance){
		debug_printf("---------- MODULE_STOCK_SMALL ---------\n");
		STOCKS_print(MODULE_STOCK_SMALL);
	}
#endif

	switch(state){

		case WAIT_TRIGGER:
			if(trigger){
				state = LAUNCH_CYLINDER_PREPARATION;
			}
			break;

		case LAUNCH_CYLINDER_PREPARATION:  // Parallélisation de la préparation de la couleur
			if(!STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_CONTAINER, MODULE_STOCK_SMALL)){
				sub_act_anne_mae_prepare_modules_for_dispose(TRUE);
			}
			state = INIT;
			break;

		case INIT:
			if(STOCKS_isFull(MODULE_STOCK_SMALL)){
				state = DONE;	// Il n'y a rien à faire
			}else{
				state = COMPUTE_ACTION; // Stockage possible
			}
			break;

		case COMPUTE_ACTION:
			if(ELEMENTS_get_flag(FLAG_ANNE_DISABLE_MODULE)){
				state = ERROR;
			}else if(!STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_ELEVATOR, MODULE_STOCK_SMALL) && STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_SLOPE, MODULE_STOCK_SMALL)){  // L'élévateur n'est pas vide et le stockage est possible
				state = PREPARE_SLOPE_FOR_ELEVATOR; // On cherche a stocker le module déjà présent dans l'élévateur
			}else if(!STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_ENTRY, MODULE_STOCK_SMALL) && STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_ELEVATOR, MODULE_STOCK_SMALL)){ // Il y a un module en bas et l'élévateur est vide
				state = ELEVATOR_GO_BOTTOM; // On cherche a stocker le module qui est en bas
			}else if(STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_ELEVATOR, MODULE_STOCK_SMALL)){
				state = ELEVATOR_GO_BOTTOM_TO_END; // On redescend l'élévateur si besoin avant de finir
			}else{ // les stocks sont plein ou il n'y a plus rien à faire
				state = DONE;
			}
			break;


		case ELEVATOR_GO_BOTTOM:
			if(entrance){
				// On sort légèrement le slider et on descend l'élévateur
				// Le slider doit sortir un peu pour laisser la place à l'élévateur de passer.
				ACT_push_order( ACT_SMALL_CYLINDER_SLIDER, ACT_SMALL_CYLINDER_SLIDER_ALMOST_OUT_WITH_CYLINDER);
				ACT_push_order( ACT_SMALL_CYLINDER_ELEVATOR, ACT_SMALL_CYLINDER_ELEVATOR_BOTTOM);
			}

			// Vérification des ordres effectués
			state = check_act_status(ACT_QUEUE_Small_cylinder_elevator, state, SLIDER_GO_IN, ERROR_DISABLE_ACT);
			break;

		case SLIDER_GO_IN:
			if(entrance){
				ACT_push_order( ACT_SMALL_CYLINDER_SLIDER , ACT_SMALL_CYLINDER_SLIDER_IN );

				// Si c'est possible, on prépare la SLOPE pour un futur stockage
				if(STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_SLOPE , MODULE_STOCK_SMALL)){
					ACT_push_order( ACT_SMALL_CYLINDER_SLOPE, ACT_SMALL_CYLINDER_SLOPE_DOWN);
				}
			}

			// Vérification des ordres effectués
			state = check_act_status(ACT_QUEUE_Small_cylinder_slider, state, STOP_POMPE_SLIDER, STOP_POMPE_SLIDER);
			break;

		case STOP_POMPE_SLIDER: // on change le sens de ventousage de la pompe (ventousage elevator, arrêt slider)
			if(entrance){
				time_timeout = global.absolute_time + 1000;
				ACT_push_order_with_param( ACT_SMALL_POMPE_PRISE , ACT_POMPE_SMALL_SLIDER_NORMAL, 100);
			}

			if(global.absolute_time > time_timeout){
				state = CHECK_CONTAINER_IS_AVAILABLE;

				// Mise à jour des données
				STOCKS_makeModuleProgressTo(STOCK_PLACE_ENTRY_TO_ELEVATOR, MODULE_STOCK_SMALL);
			}
			break;

		case CHECK_CONTAINER_IS_AVAILABLE:
			if(STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_SLOPE , MODULE_STOCK_SMALL)){
				state = ELEVATOR_GO_TOP_POS;
			}else{
				state = DONE;
			}
			break;

		case PREPARE_SLOPE_FOR_ELEVATOR:
			if(entrance){
				ACT_push_order( ACT_SMALL_CYLINDER_SLOPE, ACT_SMALL_CYLINDER_SLOPE_DOWN);
			}

			// Vérification des ordres effectués
			state = check_act_status(ACT_QUEUE_Small_cylinder_slope, state, ELEVATOR_WAIT_FOR_SLOPE, ELEVATOR_WAIT_FOR_SLOPE);
			break;

		case ELEVATOR_WAIT_FOR_SLOPE:
			if(entrance){
				ACT_push_order( ACT_SMALL_CYLINDER_ELEVATOR, ACT_SMALL_CYLINDER_ELEVATOR_WAIT_FOR_SLOPE);
			}

			// Vérification des ordres effectués
			state = check_act_status(ACT_QUEUE_Small_cylinder_elevator, state, SLOPE_GO_ALMOST_UP_POS, SLOPE_GO_ALMOST_UP_POS);
			break;

		case SLOPE_GO_ALMOST_UP_POS:
			if(entrance){
				ACT_push_order( ACT_SMALL_CYLINDER_SLOPE, ACT_SMALL_CYLINDER_SLOPE_ALMOST_UP);
			}

			// Vérification des ordres effectués
			state = check_act_status(ACT_QUEUE_Small_cylinder_slope, state, ELEVATOR_GO_TOP_POS, ELEVATOR_GO_TOP_POS);
			break;

		case ELEVATOR_GO_TOP_POS:
			if(entrance){
				ACT_push_order( ACT_SMALL_CYLINDER_ELEVATOR, ACT_SMALL_CYLINDER_ELEVATOR_TOP);
			}

			// Vérification des ordres effectués
			state = check_act_status(ACT_QUEUE_Small_cylinder_elevator, state, STOCK_UP_CYLINDER, STOCK_UP_CYLINDER);
			break;

		case STOCK_UP_CYLINDER:
			if(entrance){
				ACT_push_order( ACT_SMALL_CYLINDER_SLOPE, ACT_SMALL_CYLINDER_SLOPE_UP);
			}

			// Vérification des ordres effectués
			state = check_act_status(ACT_QUEUE_Small_cylinder_slope, state, WAIT_STABILZATION, WAIT_STABILZATION);
			break;

		case WAIT_STABILZATION:/*
			if(entrance){
				time_timeout = global.absolute_time + 400;
			}*/
			//if(global.absolute_time > time_timeout){
				state = PUT_CYLINDER_IN_CONTAINER;
			//}
			break;

		case PUT_CYLINDER_IN_CONTAINER:
			if(entrance){
				time_timeout = global.absolute_time + 400;
				ACT_push_order_with_param( ACT_SMALL_POMPE_PRISE, ACT_POMPE_SMALL_ELEVATOR_REVERSE, 100);	// un coup de reverse pour bien décoler le module
			}

			if(global.absolute_time > time_timeout){
				ACT_push_order_with_param( ACT_SMALL_POMPE_PRISE, ACT_POMPE_STOP, 0);	// on stoppe la pompe
				//state = SLOPE_GO_VERY_UP;
				state = COMPUTE_ACTION;

				// Mise à jour des données
				STOCKS_makeModuleProgressTo(STOCK_PLACE_ELEVATOR_TO_CONTAINER, MODULE_STOCK_SMALL);
			}
			break;

		case SLOPE_GO_VERY_UP: // état non utilisé
			if(entrance){
				ACT_push_order(ACT_SMALL_CYLINDER_SLOPE, ACT_SMALL_CYLINDER_SLOPE_VERY_UP);
			}

			// Vérification des ordres effectués
			state = check_act_status(ACT_QUEUE_Small_cylinder_slope, state,  COMPUTE_ACTION, COMPUTE_ACTION);
			break;

		case ELEVATOR_GO_BOTTOM_TO_END:
			if(entrance){
				// On descend l'élévateur
				ACT_push_order( ACT_SMALL_CYLINDER_ELEVATOR, ACT_SMALL_CYLINDER_ELEVATOR_BOTTOM);
			}

			// Vérification des ordres effectués
			state = check_act_status(ACT_QUEUE_Small_cylinder_elevator, state, DONE, DONE);
			break;

		case ERROR_DISABLE_ACT:
			ELEMENTS_set_flag(FLAG_ANNE_DISABLE_MODULE, TRUE);
			state = ERROR; // On ne peut plus rien faire
			break;

		case DONE:
			if(entrance){
				set_sub_act_enable(SUB_ANNE_DEPOSE_MODULES, TRUE);
			}
			RESET_MAE();
			on_turning_point();
			ret = END_OK;
			break;

		case ERROR:
			RESET_MAE();
			on_turning_point();
			ret = NOT_HANDLED;
			break;

		default:
			if(entrance)
				debug_printf("default case in sub_act_anne_mae_store_modules\n");
			break;
	}

	return ret;
}
#else
// Subaction actionneur de stockage
error_e sub_act_anne_mae_store_modules(bool_e trigger){
	CREATE_MAE_WITH_VERBOSE(SM_ID_ACT_ANNE_MAE_STORE_MODULES,
			WAIT_TRIGGER,
			LAUNCH_CYLINDER_PREPARATION,
			INIT,
			COMPUTE_ACTION,

			// Le stockage des modules
			ELEVATOR_GO_BOTTOM,
			SLIDER_GO_IN,
			STOP_POMPE_SLIDER,
			CHECK_CONTAINER_IS_AVAILABLE,
			ELEVATOR_GO_MID_POS,
			PREPARE_SLOPE_FOR_ELEVATOR,
			ELEVATOR_WAIT_FOR_SLOPE,
			SLOPE_GO_ALMOST_UP_POS,
			ELEVATOR_GO_TOP_POS,
			STOCK_UP_CYLINDER,
			WAIT_STABILZATION,
			PUT_CYLINDER_IN_CONTAINER,
			SLOPE_GO_VERY_UP,

			// Actions à faire en fin de subaction
			ELEVATOR_GO_BOTTOM_TO_END,
			CHECK_DISPENSER_TO_END,
			DISPENSER_GO_UNLOCK_TO_END,
			WAIT_DISPENSER_TO_END,
			DISPENSER_GO_LOCK_TO_END,

			ERROR_DISABLE_ACT,
			ERROR,
			DONE

		);

	error_e ret = IN_PROGRESS;
	static time32_t time_timeout;

#ifdef DISPLAY_STOCKS
	if(entrance){
		debug_printf("---------- MODULE_STOCK_SMALL ---------\n");
		STOCKS_print(MODULE_STOCK_SMALL);
	}
#endif

	switch(state){

		case WAIT_TRIGGER:
			if(trigger){
				state = LAUNCH_CYLINDER_PREPARATION;
			}
			break;

		case LAUNCH_CYLINDER_PREPARATION:  // Parallélisation de la préparation de la couleur
			if(!STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_CONTAINER, MODULE_STOCK_SMALL)){
				sub_act_anne_mae_prepare_modules_for_dispose(TRUE);
			}
			state = INIT;
			break;

		case INIT:
			if(STOCKS_isFull(MODULE_STOCK_SMALL)){
				state = DONE;	// Il n'y a rien à faire
			}else{
				state = COMPUTE_ACTION; // Stockage possible
			}
			break;

		case COMPUTE_ACTION:
			if(ELEMENTS_get_flag(FLAG_ANNE_DISABLE_MODULE)){
				state = ERROR;
			}else if(!STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_ELEVATOR, MODULE_STOCK_SMALL) && STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_SLOPE, MODULE_STOCK_SMALL)){  // L'élévateur n'est pas vide et le stockage est possible
				state = PREPARE_SLOPE_FOR_ELEVATOR; // On cherche a stocker le module déjà présent dans l'élévateur
			}else if(!STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_ENTRY, MODULE_STOCK_SMALL) && STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_ELEVATOR, MODULE_STOCK_SMALL)){ // Il y a un module en bas et l'élévateur est vide
				state = ELEVATOR_GO_BOTTOM; // On cherche a stocker le module qui est en bas
			}else if(STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_ELEVATOR, MODULE_STOCK_SMALL)){
				state = ELEVATOR_GO_BOTTOM_TO_END; // On redescend l'élévateur si besoin avant de finir
			}else{ // les stocks sont plein ou il n'y a plus rien à faire
				state = DONE;
			}
			break;


		case ELEVATOR_GO_BOTTOM:
			if(entrance){
				// On sort légèrement le slider et on descend l'élévateur
				// Le slider doit sortir un peu pour laisser la place à l'élévateur de passer.
				ACT_push_order( ACT_SMALL_CYLINDER_SLIDER, ACT_SMALL_CYLINDER_SLIDER_ALMOST_OUT_WITH_CYLINDER);
				ACT_push_order( ACT_SMALL_CYLINDER_ELEVATOR, ACT_SMALL_CYLINDER_ELEVATOR_BOTTOM);
			}

			// Vérification des ordres effectués
			state = check_act_status(ACT_QUEUE_Small_cylinder_elevator, state, SLIDER_GO_IN, ERROR_DISABLE_ACT);
			break;

		case SLIDER_GO_IN:
			if(entrance){
				ACT_push_order( ACT_SMALL_CYLINDER_SLIDER , ACT_SMALL_CYLINDER_SLIDER_IN );

				// Si c'est possible, on prépare la SLOPE pour un futur stockage
				if(STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_SLOPE , MODULE_STOCK_SMALL)){
					ACT_push_order( ACT_SMALL_CYLINDER_SLOPE, ACT_SMALL_CYLINDER_SLOPE_DOWN);
				}
			}

			// Vérification des ordres effectués
			state = check_act_status(ACT_QUEUE_Small_cylinder_slider, state, STOP_POMPE_SLIDER, STOP_POMPE_SLIDER);
			break;

		case STOP_POMPE_SLIDER: // on change le sens de ventousage de la pompe (ventousage elevator, arrêt slider)
			if(entrance){
				time_timeout = global.absolute_time + 1000;
				ACT_push_order_with_param( ACT_SMALL_POMPE_DISPOSE , ACT_POMPE_SMALL_ELEVATOR_NORMAL, 100);
			}

			if(global.absolute_time > time_timeout){
				state = CHECK_CONTAINER_IS_AVAILABLE;

				// Mise à jour des données
				STOCKS_makeModuleProgressTo(STOCK_PLACE_ENTRY_TO_ELEVATOR, MODULE_STOCK_SMALL);
			}
			break;

		case CHECK_CONTAINER_IS_AVAILABLE:
			if(STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_SLOPE , MODULE_STOCK_SMALL)){
				state = ELEVATOR_GO_TOP_POS;
			}else{
				state = DONE;
			}
			break;

		case PREPARE_SLOPE_FOR_ELEVATOR:
			if(entrance){
				ACT_push_order( ACT_SMALL_CYLINDER_SLOPE, ACT_SMALL_CYLINDER_SLOPE_DOWN);
			}

			// Vérification des ordres effectués
			state = check_act_status(ACT_QUEUE_Small_cylinder_slope, state, ELEVATOR_WAIT_FOR_SLOPE, ELEVATOR_WAIT_FOR_SLOPE);
			break;

		case ELEVATOR_WAIT_FOR_SLOPE:
			if(entrance){
				ACT_push_order( ACT_SMALL_CYLINDER_ELEVATOR, ACT_SMALL_CYLINDER_ELEVATOR_WAIT_FOR_SLOPE);
			}

			// Vérification des ordres effectués
			state = check_act_status(ACT_QUEUE_Small_cylinder_elevator, state, SLOPE_GO_ALMOST_UP_POS, SLOPE_GO_ALMOST_UP_POS);
			break;

		case SLOPE_GO_ALMOST_UP_POS:
			if(entrance){
				ACT_push_order( ACT_SMALL_CYLINDER_SLOPE, ACT_SMALL_CYLINDER_SLOPE_ALMOST_UP);
			}

			// Vérification des ordres effectués
			state = check_act_status(ACT_QUEUE_Small_cylinder_slope, state, ELEVATOR_GO_TOP_POS, ELEVATOR_GO_TOP_POS);
			break;

		case ELEVATOR_GO_TOP_POS:
			if(entrance){
				ACT_push_order( ACT_SMALL_CYLINDER_ELEVATOR, ACT_SMALL_CYLINDER_ELEVATOR_TOP);
			}

			// Vérification des ordres effectués
			state = check_act_status(ACT_QUEUE_Small_cylinder_elevator, state, STOCK_UP_CYLINDER, STOCK_UP_CYLINDER);
			break;

		case STOCK_UP_CYLINDER:
			if(entrance){
				ACT_push_order( ACT_SMALL_CYLINDER_SLOPE, ACT_SMALL_CYLINDER_SLOPE_UP);
			}

			// Vérification des ordres effectués
			state = check_act_status(ACT_QUEUE_Small_cylinder_slope, state, WAIT_STABILZATION, WAIT_STABILZATION);
			break;

		case WAIT_STABILZATION:/*
			if(entrance){
				time_timeout = global.absolute_time + 400;
			}*/
			//if(global.absolute_time > time_timeout){
				state = PUT_CYLINDER_IN_CONTAINER;
			//}
			break;

		case PUT_CYLINDER_IN_CONTAINER:
			if(entrance){
				time_timeout = global.absolute_time + 400;
				ACT_push_order_with_param( ACT_SMALL_POMPE_DISPOSE, ACT_POMPE_SMALL_ELEVATOR_REVERSE, 100);	// un coup de reverse pour bien décoler le module
			}

			if(global.absolute_time > time_timeout){
				ACT_push_order_with_param( ACT_SMALL_POMPE_DISPOSE, ACT_POMPE_STOP, 0);	// on stoppe la pompe
				//state = SLOPE_GO_VERY_UP;
				state = COMPUTE_ACTION;

				// Mise à jour des données
				STOCKS_makeModuleProgressTo(STOCK_PLACE_ELEVATOR_TO_CONTAINER, MODULE_STOCK_SMALL);
			}
			break;

		case SLOPE_GO_VERY_UP: // état non utilisé
			if(entrance){
				ACT_push_order(ACT_SMALL_CYLINDER_SLOPE, ACT_SMALL_CYLINDER_SLOPE_VERY_UP);
			}

			// Vérification des ordres effectués
			state = check_act_status(ACT_QUEUE_Small_cylinder_slope, state,  COMPUTE_ACTION, COMPUTE_ACTION);
			break;

		case ELEVATOR_GO_BOTTOM_TO_END:
			if(entrance){
				// On descend l'élévateur
				ACT_push_order( ACT_SMALL_CYLINDER_ELEVATOR, ACT_SMALL_CYLINDER_ELEVATOR_BOTTOM);
			}

			// Vérification des ordres effectués
			state = check_act_status(ACT_QUEUE_Small_cylinder_elevator, state, DONE, DONE);
			break;

		case ERROR_DISABLE_ACT:
			ELEMENTS_set_flag(FLAG_ANNE_DISABLE_MODULE, TRUE);
			state = ERROR; // On ne peut plus rien faire
			break;

		case DONE:
			if(entrance){
				set_sub_act_enable(SUB_ANNE_DEPOSE_MODULES, TRUE);
			}
			RESET_MAE();
			on_turning_point();
			ret = END_OK;
			break;

		case ERROR:
			RESET_MAE();
			on_turning_point();
			ret = NOT_HANDLED;
			break;

		default:
			if(entrance)
				debug_printf("default case in sub_act_anne_mae_store_modules\n");
			break;
	}

	return ret;
}
#endif

// Subaction actionneur de préparation de la dépose des modules
error_e sub_act_anne_mae_prepare_modules_for_dispose(bool_e trigger){
	CREATE_MAE_WITH_VERBOSE(SM_ID_ACT_ANNE_MAE_PREPARE_MODULES_FOR_DISPOSE,
			WAIT_TRIGGER,
			INIT,
			WAIT_STORAGE,
			WAIT_MODULE_FALL,
			MOVE_BALANCER_OUT,
			BEGIN_CHECK_POSITION_BALANCER,
			ERROR_MOVE_BALANCER_OUT,
			CHECK_IF_TURN_FOR_COLOR_NEEDED,
			TURN_FOR_COLOR,
			WAIT_OUR_COLOR,
			WAIT_TIME_IN_WHITE,
			WAIT_WHITE,
			STOP_TURN,
			END_CHECK_POSITION_BALANCER,
			END_MOVE_BALANCER_IN,
			ERROR,
			DONE
		);

	#define TIMEOUT_COLOR	(4000)  // Temps au dela duquel on arrête de tourner le module, on a échoué a détecté la couleur
	#define NB_TRY_BALANCER_TOLERATED (3)

	error_e ret = IN_PROGRESS;
	static error_e stateAct = IN_PROGRESS;
	static time32_t time_timeout = 0;
	static Uint8 nb_errors_balancer = 0;

	switch(state){

		case WAIT_TRIGGER:
			if(trigger){
				state = INIT;

				// On baisse le flag dans le cas où cela n'a pas encore été fait
				ELEMENTS_set_flag(FLAG_ANNE_MODULE_COLOR_SUCCESS, FALSE);
				ELEMENTS_set_flag(FLAG_ANNE_MODULE_COLOR_FINISH, FALSE);
				ELEMENTS_set_flag(FLAG_SMALL_BALANCER_FINISH, FALSE);

				nb_errors_balancer = 0;
			}
			break;

		case INIT:
			//Partie de code ajoutée à la coupe de Belgique qui permettait de mieux déposer les cylindres
			if(entrance){
				ACT_push_order(ACT_SMALL_CYLINDER_SLOPE, ACT_SMALL_CYLINDER_SLOPE_UP);
			}

			if(!STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_BALANCER, MODULE_STOCK_SMALL)){
				state = MOVE_BALANCER_OUT; // Préparation de la dépose possible
			}else{
				state = WAIT_STORAGE;	// Il n'y a pas de cylindre disponible //TODO déclencher le stockage
			}
			break;

		case WAIT_STORAGE:
			if(entrance){
				time_timeout = global.absolute_time + 10000;
				sub_act_anne_mae_store_modules(TRUE);
			}
			if(!STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_BALANCER, MODULE_STOCK_SMALL)){
				state = WAIT_MODULE_FALL;
			}else if(global.absolute_time > time_timeout){
				state = ERROR; // Le stockage a semble t il échoué, on ne peut rien faire de plus ici
			}
			break;

		case WAIT_MODULE_FALL:	// On attend que le module tombe bien dans le balancer, c'est à dire que le stockage se termine bien mécaniquement
			if(entrance){
				time_timeout = global.absolute_time + 1500;
			}
			if(global.absolute_time > time_timeout){
				state = MOVE_BALANCER_OUT;
			}
			break;

		case MOVE_BALANCER_OUT:
			if(entrance){
				ACT_push_order(ACT_SMALL_CYLINDER_BALANCER, ACT_SMALL_CYLINDER_BALANCER_OUT);
			}

			// Vérification des ordres effectués
			state = check_act_status(ACT_QUEUE_Small_cylinder_balancer, state, BEGIN_CHECK_POSITION_BALANCER, BEGIN_CHECK_POSITION_BALANCER);
			break;

		case BEGIN_CHECK_POSITION_BALANCER:
			// On utilise ACT_check_position_config_right pour des questions de réentrance car cette MAE est executée en parallèle
			stateAct = ACT_check_position_config_right(ACT_SMALL_CYLINDER_BALANCER, ACT_SMALL_CYLINDER_BALANCER_OUT);

			if(stateAct != IN_PROGRESS){
				if(stateAct == NOT_HANDLED || stateAct == END_WITH_TIMEOUT){



					// On incrémente l'erreur
					nb_errors_balancer++;

					// On regarde si on retente ou si on part en ERROR
					if(nb_errors_balancer < NB_TRY_BALANCER_TOLERATED){
						state = ERROR_MOVE_BALANCER_OUT;	// L'actionneur n'est pas en position, on doit le mettre
					}else{
						state = ERROR;
					}
				}else{

					state = CHECK_IF_TURN_FOR_COLOR_NEEDED;  // C'est bon l'actionneur est en position

					// Mise à jour des données : on fait progresser le module en POS_BALANCER vers la position POS_COLOR
					STOCKS_makeModuleProgressTo(STOCK_PLACE_BALANCER_TO_COLOR, MODULE_STOCK_SMALL);
				}
				ELEMENTS_set_flag(FLAG_SMALL_BALANCER_FINISH, TRUE);
			}
			break;

		case ERROR_MOVE_BALANCER_OUT:
			if(entrance){
				ACT_push_order(ACT_SMALL_CYLINDER_BALANCER, ACT_SMALL_CYLINDER_BALANCER_IN);
			}
			// Vérification des ordres effectués
			state = check_act_status(ACT_QUEUE_Small_cylinder_balancer, state, MOVE_BALANCER_OUT, MOVE_BALANCER_OUT);
			break;


		case CHECK_IF_TURN_FOR_COLOR_NEEDED:

			if(STOCKS_getModuleType(STOCK_POS_COLOR, MODULE_STOCK_SMALL) == MODULE_POLY){
				state = TURN_FOR_COLOR;  // Module polychrome : retournement nécéssaire
			}else{
				state = END_MOVE_BALANCER_IN; // Module unicouleur : pas de retournement
			}
			break;

		case TURN_FOR_COLOR:
			if(entrance){
				ACT_push_order(ACT_SMALL_CYLINDER_COLOR, ACT_SMALL_CYLINDER_COLOR_NORMAL_SPEED);
				time_timeout = global.absolute_time + 500;
			}
			if(global.absolute_time > time_timeout){	// Attente de la fin de la chute du module pour éviter les effets de bords de détection de couleur
				state = WAIT_OUR_COLOR;
			}
			break;

		case WAIT_OUR_COLOR:
			if(global.color == BLUE)
				state = ACT_wait_state_color_sensor(COLOR_SENSOR_I2C_BLUE, TIMEOUT_COLOR, state, WAIT_WHITE, STOP_TURN);
			else
				state = ACT_wait_state_color_sensor(COLOR_SENSOR_I2C_YELLOW, TIMEOUT_COLOR, state, WAIT_WHITE, STOP_TURN);
			break;

		case WAIT_WHITE:
			// On en profite pour retourner le balancer vers l'intérieur du robot pour gagner du temps
			// Pas de vérification du résultat ici, la couleur est prioritaire.
			if(entrance){
				ACT_push_order(ACT_SMALL_CYLINDER_BALANCER, ACT_SMALL_CYLINDER_BALANCER_IN);
				STOCKS_makeModuleProgressTo(STOCK_PLACE_CONTAINER_TO_BALANCER, MODULE_STOCK_SMALL);
			}
			state = ACT_wait_state_color_sensor(COLOR_SENSOR_I2C_WHITE, TIMEOUT_COLOR, state, WAIT_TIME_IN_WHITE, STOP_TURN);
			break;

		case WAIT_TIME_IN_WHITE:
			if(entrance){
				time_timeout = global.absolute_time + 400;
			}

			if(global.absolute_time > time_timeout){	// Attente du faible rotation
				state = STOP_TURN;
			}

			if(ON_LEAVE()){
				ELEMENTS_set_flag(FLAG_ANNE_MODULE_COLOR_SUCCESS, TRUE);
			}
			break;

		case STOP_TURN:
			if(entrance){
				ACT_push_order(ACT_SMALL_CYLINDER_COLOR, ACT_SMALL_CYLINDER_COLOR_ZERO_SPEED);
			}
			// Aucune vérification ici
			state = END_CHECK_POSITION_BALANCER;
			break;

		case END_CHECK_POSITION_BALANCER:
			stateAct = ACT_check_position_config(ACT_SMALL_CYLINDER_BALANCER, ACT_SMALL_CYLINDER_BALANCER_IN);

			if(stateAct != IN_PROGRESS){
				if(stateAct == NOT_HANDLED || stateAct == END_WITH_TIMEOUT){
					state = END_MOVE_BALANCER_IN;	// L'actionneur n'est pas en position, on doit le mettre
				}else{
					state = DONE;  // C'est bon l'actionneur est en position
				}
			}
			break;

		case END_MOVE_BALANCER_IN:
			if(entrance){
				ACT_push_order(ACT_SMALL_CYLINDER_BALANCER, ACT_SMALL_CYLINDER_BALANCER_IN);
			}

			// Aucune vérification
			state = DONE;

			// On exit
			if(state != END_MOVE_BALANCER_IN){
				// Mise à jour des données : on décale les modules du stock
				STOCKS_makeModuleProgressTo(STOCK_PLACE_CONTAINER_TO_BALANCER, MODULE_STOCK_SMALL);
			}
			break;

		case DONE:
			ELEMENTS_set_flag(FLAG_ANNE_MODULE_COLOR_FINISH, TRUE);
			ELEMENTS_set_flag(FLAG_SMALL_BALANCER_FINISH, FALSE);
			RESET_MAE();
			on_turning_point();
			ret = END_OK;
			break;

		case ERROR:
			ELEMENTS_set_flag(FLAG_ANNE_MODULE_COLOR_FINISH, TRUE);
			ELEMENTS_set_flag(FLAG_SMALL_BALANCER_FINISH, FALSE);
			RESET_MAE();
			on_turning_point();
			ret = NOT_HANDLED;
			break;

		default:
			if(entrance)
				debug_printf("default case in sub_act_anne_mae_prepare_modules_for_dispose\n");
			break;
	}

	return ret;
}



// Subaction actionneur de dépose des modules
error_e sub_act_anne_mae_dispose_modules(arg_dipose_mae_e arg_dispose){
	CREATE_MAE_WITH_VERBOSE(SM_ID_ACT_ANNE_MAE_DISPOSE_MODULES,
			INIT,
			INIT_ARM_SERVO,
			INIT_DISPOSE_SERVO,
			TRIGGER_CYLINDER_PREPARATION,
			WAIT_CYLINDER_PREPARATION,
			TAKE_CYLINDER,
			RAISE_CYLINDER,
			GET_OUT_CYLINDER_OF_ROBOT,
			UNFOLD_DISPOSE_SERVO,
			GO_TO_DISPOSE_POS,
			DISPOSE_CYLINDER,

			CHOOSE_ARM_STORAGE_POS,
			S1_MOVE_DISPOSE_SERVO,
			S1_MOVE_ARM_SERVO,
			S2_MOVE_DISPOSE_SERVO,
			S2_MOVE_ARM_SERVO,

			ERROR,
			DONE
		);

	error_e ret = IN_PROGRESS;
	static error_e stateAct = IN_PROGRESS;
	static bool_e anotherDisposeWillFollow = FALSE;
	static time32_t time_timeout = 0;

#ifdef DISPLAY_STOCKS
	if(entrance){
		debug_printf("---------- MODULE_STOCK_SMALL ---------\n");
		STOCKS_print(MODULE_STOCK_SMALL);
	}
#endif

	switch(state){

		case INIT:
			if(STOCKS_isEmpty(MODULE_STOCK_SMALL)){
				state = DONE;	// Il n'y a rien à faire
			}else{
				if(arg_dispose == ARG_DISPOSE_ONE_CYLINDER_FOLLOW_BY_ANOTHER){
					state = INIT_ARM_SERVO; // Dépose possible
					anotherDisposeWillFollow = TRUE; // Cette dépose n'est pas la dernière, une autre va suivre
				}else if(arg_dispose == ARG_DISPOSE_ONE_CYLINDER_AND_FINISH){
					state = INIT_ARM_SERVO; // Dépose possible
					anotherDisposeWillFollow = FALSE; // Cette dépose est la dernière, on doit ranger le bras à la fin
				}else{	// arg == ARG_STORE_ARM
					state = CHOOSE_ARM_STORAGE_POS;
					anotherDisposeWillFollow = FALSE; // Important : il n'y a pas de dépose à suivre, on tente de ranger le bras dans le robot
				}
			}
			break;

		case INIT_ARM_SERVO:
			if(entrance){
				ACT_push_order(ACT_SMALL_CYLINDER_ARM, ACT_SMALL_CYLINDER_ARM_PREPARE_TO_TAKE);
			}

			state = check_act_status(ACT_QUEUE_Small_cylinder_arm, state, INIT_DISPOSE_SERVO, INIT_DISPOSE_SERVO);
			break;

		case INIT_DISPOSE_SERVO:
			if(entrance){
				ACT_push_order(ACT_SMALL_CYLINDER_DISPOSE, ACT_SMALL_CYLINDER_DISPOSE_TAKE);
			}

			state = check_act_status(ACT_QUEUE_Small_cylinder_dispose, state, TRIGGER_CYLINDER_PREPARATION, TRIGGER_CYLINDER_PREPARATION);
			break;

		case TRIGGER_CYLINDER_PREPARATION:
			if(ELEMENTS_get_flag(FLAG_ANNE_MODULE_COLOR_FINISH)){
				state = TAKE_CYLINDER;
			}else{
				sub_act_anne_mae_prepare_modules_for_dispose(TRUE); // On lance la préparation d'un cylindre : balancer + mise en position pour la bonne couleur
				state = WAIT_CYLINDER_PREPARATION;
			}
			break;

		case WAIT_CYLINDER_PREPARATION :	// Ajouter un timeout
			// On attend la préparation du cylinder (retournement couleur)
			if(ELEMENTS_get_flag(FLAG_ANNE_MODULE_COLOR_FINISH)){
				state = TAKE_CYLINDER;
			}
			break;

		case TAKE_CYLINDER:  // On ventouse le cylindre pour le prendre
			if(entrance){
				ACT_push_order_with_param(ACT_SMALL_POMPE_DISPOSE, ACT_POMPE_SMALL_DEPOSE_NORMAL, 100);
				ACT_push_order(ACT_SMALL_CYLINDER_ARM, ACT_SMALL_CYLINDER_ARM_TAKE);
			}
			state = check_act_status(ACT_QUEUE_Small_cylinder_arm, state, RAISE_CYLINDER, RAISE_CYLINDER);
			break;


		case RAISE_CYLINDER:
			if(entrance){
				ACT_push_order(ACT_SMALL_CYLINDER_DISPOSE, ACT_SMALL_CYLINDER_DISPOSE_RAISE);
			}

			state = check_act_status(ACT_QUEUE_Small_cylinder_dispose, state, GET_OUT_CYLINDER_OF_ROBOT, GET_OUT_CYLINDER_OF_ROBOT);
			break;

		case GET_OUT_CYLINDER_OF_ROBOT:
			if(entrance){
				ACT_push_order(ACT_SMALL_CYLINDER_ARM, ACT_SMALL_CYLINDER_ARM_OUT);

				//if(anotherDisposeWillFollow){
					sub_act_anne_mae_prepare_modules_for_dispose(TRUE);
				//}

				stateAct = IN_PROGRESS;
			}

			if(stateAct == IN_PROGRESS){
				stateAct = check_act_status(ACT_QUEUE_Small_cylinder_arm, IN_PROGRESS, END_OK, NOT_HANDLED);
			}

			if(stateAct != IN_PROGRESS){
				if(/*!anotherDisposeWillFollow || */ELEMENTS_get_flag(FLAG_SMALL_BALANCER_FINISH)){
					state = UNFOLD_DISPOSE_SERVO;
				}
			}

			// On exit
			if(ON_LEAVE()){
				STOCKS_makeModuleProgressTo(STOCK_PLACE_COLOR_TO_ARM_DISPOSE, MODULE_STOCK_SMALL);
			}
			break;

		case UNFOLD_DISPOSE_SERVO:
			if(entrance){
				ACT_push_order(ACT_SMALL_CYLINDER_DISPOSE, ACT_SMALL_CYLINDER_DISPOSE_DISPOSE);

				// Une autre dépose va suivre, on peut dès à présent lancer la préparation du module suivant

			}
			state = check_act_status(ACT_QUEUE_Small_cylinder_dispose, state, GO_TO_DISPOSE_POS, GO_TO_DISPOSE_POS);
			break;

		case GO_TO_DISPOSE_POS:
			if(entrance){
				time_timeout = global.absolute_time + 400;  // Temporisation avant de déventouser
				ACT_push_order(ACT_SMALL_CYLINDER_ARM, ACT_SMALL_CYLINDER_ARM_DISPOSE);
			}

			stateAct = check_act_status(ACT_QUEUE_Small_cylinder_arm, IN_PROGRESS, END_OK, NOT_HANDLED);

			if(stateAct != IN_PROGRESS && global.absolute_time > time_timeout){
				state = DISPOSE_CYLINDER;
			}
			break;

		case DISPOSE_CYLINDER:
			if(entrance){
				time_timeout = global.absolute_time + 700;		// Temporisation pour déventouser
				ACT_push_order_with_param(ACT_SMALL_POMPE_DISPOSE, ACT_POMPE_STOP, 0);
			}

			// Pas de vérification ici car les pompes retournent toujours vrai
			if(global.absolute_time > time_timeout){
				state = CHOOSE_ARM_STORAGE_POS;

				// Mettre à jour les données
				STOCKS_makeModuleProgressTo(STOCK_PLACE_CLEAR_ARM_DISPOSE, MODULE_STOCK_SMALL);
			}
			break;

		case CHOOSE_ARM_STORAGE_POS:
			if(anotherDisposeWillFollow || !STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_ARM_DISPOSE, MODULE_STOCK_SMALL)){
				state = S1_MOVE_DISPOSE_SERVO;	// Scénarion 1: On doit positionner le bras pour une autre dépose
			}else{
				state = S2_MOVE_DISPOSE_SERVO;	// Scénario 2: on doit ranger le bras dans le robot
			}
			break;

		case S1_MOVE_DISPOSE_SERVO:
			if(entrance){
				ACT_push_order(ACT_SMALL_CYLINDER_DISPOSE, ACT_SMALL_CYLINDER_DISPOSE_TAKE);
			}

			state = check_act_status(ACT_QUEUE_Small_cylinder_dispose, state, S1_MOVE_ARM_SERVO, S1_MOVE_ARM_SERVO);
			break;

		case S1_MOVE_ARM_SERVO:
			if(entrance){
				ACT_push_order(ACT_SMALL_CYLINDER_ARM, ACT_SMALL_CYLINDER_ARM_PREPARE_TO_TAKE);
			}

			state = check_act_status(ACT_QUEUE_Small_cylinder_arm, state, DONE, DONE);
			break;

		case S2_MOVE_DISPOSE_SERVO:
			if(entrance){
				ACT_push_order(ACT_SMALL_CYLINDER_DISPOSE, ACT_SMALL_CYLINDER_DISPOSE_IDLE);
			}

			state = check_act_status(ACT_QUEUE_Small_cylinder_dispose, state, S2_MOVE_ARM_SERVO, S2_MOVE_ARM_SERVO);
			break;

		case S2_MOVE_ARM_SERVO:
			if(entrance){
				ACT_push_order(ACT_SMALL_CYLINDER_ARM, ACT_SMALL_CYLINDER_ARM_IN);
			}

			state = check_act_status(ACT_QUEUE_Small_cylinder_arm, state, DONE, DONE);
			break;


		case DONE:
			RESET_MAE();
			on_turning_point();
			ret = END_OK;
			break;

		case ERROR:
			RESET_MAE();
			on_turning_point();
			ret = NOT_HANDLED;
			break;

		default:
			if(entrance)
				debug_printf("default case in sub_act_anne_mae_dispose_modules\n");
			break;
	}

	return ret;
}
