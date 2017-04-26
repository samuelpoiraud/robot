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
	bool_e white_color, blue_color, yellow_color;
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


/// HARRY





// Subaction actionneur de prise fusée v2
error_e sub_act_anne_take_rocket_down_to_top(moduleRocketLocation_e rocket, ELEMENTS_side_e module_very_down, ELEMENTS_side_e module_down, ELEMENTS_side_e module_top, ELEMENTS_side_e module_very_top){
	CREATE_MAE_WITH_VERBOSE(SM_ID_ACT_HARRY_TAKE_ROCKET,
			INIT,
			COMPUTE_NEXT_CYLINDER,
			COMPUTE_ACTION,

			ACTION_MOVE_AWAY_MULTIFONCTION,
			ACTION_GO_TAKE_CYLINDER,
			ACTION_GO_TAKE_CYLINDER_2,
			ERROR_ACTION_GO_TAKE_CYLINDER,
			ERROR_ACTION_GO_TAKE_CYLINDER_RETRY,
			NO_CYLINDER_DETECTED,

			PROTECT_NEXT_FALL,
			ACTION_BRING_BACK_CYLINDER,
			ACTION_BRING_BACK_CYLINDER_2,
			STOP_POMPE_SLIDER,
			ACTION_BRING_UP_CYLINDER,
			ACTION_STOCK_UP_CYLINDER,
			ACTION_PUT_CYLINDER_IN_CONTAINER,
			ACTION_PUT_SLOPE_DOWN,

			ERROR_DISABLE_ACT,
			ERROR,
			DONE
		);

	static ELEMENTS_side_e rocketSide[MAX_MODULE_ROCKET];
	static Uint8 indexSide = 0;
	static ELEMENTS_side_e moduleToTake = NO_SIDE;		// Module en cours de prise
	static error_e state1 = IN_PROGRESS, state2 = IN_PROGRESS;
	static error_e state1bis = IN_PROGRESS, state2bis = IN_PROGRESS;
	static moduleType_e moduleType = MODULE_EMPTY;
	static time32_t time_timeout;


	switch(state){

		// penser a baisser le couple du moteur, pour reduire la vitesse de sortie du slider
		// si on le decale en prenant, on ne peut pas le remettre en place

		case INIT:

			//SWITH SMALL

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

			rocketSide[0] = module_very_down;
			rocketSide[1] = module_down;
			rocketSide[2] = module_top;
			rocketSide[3] = module_very_top;
			state = COMPUTE_NEXT_CYLINDER;
			break;

		case COMPUTE_NEXT_CYLINDER:
			if(ROCKETS_isEmpty(rocket) || indexSide >= 4){
				moduleToTake = NO_SIDE;		// La fusée est vide, nous avons fini.
			}else if(rocketSide[indexSide] == NO_SIDE){
				moduleToTake = NO_SIDE; 	// On ne doit pas prendre les modules suivants (ceci est un choix de l'utilisateur)
			}else if(STOCKS_isFull(MODULE_STOCK_SMALL) || ELEMENTS_get_flag(FLAG_ANNE_DISABLE_MODULE)){
				moduleToTake = NO_SIDE;
			}
			indexSide++; // On incrémente l'index pour le prochain passage
			state = COMPUTE_ACTION;
			break;

		case COMPUTE_ACTION:
			if(moduleToTake == NO_SIDE){
				state = DONE; // On a fini ou rien n'est possible de faire
			}else{
				state = ACTION_MOVE_AWAY_MULTIFONCTION;
			}
			break;

		case ACTION_MOVE_AWAY_MULTIFONCTION:
			if (entrance){
				ACT_push_order( ACT_SMALL_CYLINDER_MULTIFONCTION , ACT_SMALL_CYLINDER_MULTIFONCTION_OUT);
			}
			state = check_act_status(ACT_QUEUE_Small_cylinder_multifonction, IN_PROGRESS, ACTION_GO_TAKE_CYLINDER, ERROR);
			break;

		case ACTION_GO_TAKE_CYLINDER:
			if (entrance){
				ACT_push_order( ACT_SMALL_CYLINDER_ELEVATOR , ACT_SMALL_CYLINDER_ELEVATOR_LOCK_WITH_CYLINDER);
			}
			state = check_act_status(ACT_QUEUE_Small_cylinder_elevator, IN_PROGRESS, ACTION_GO_TAKE_CYLINDER_2, ERROR);

			break;

		case ACTION_GO_TAKE_CYLINDER_2:
			if(entrance){
				//On active la pompe avant d'avancer
				ACT_push_order( ACT_SMALL_POMPE_PRISE , ACT_POMPE_SMALL_SLIDER_NORMAL );
				ACT_push_order( ACT_SMALL_CYLINDER_SLIDER , ACT_SMALL_CYLINDER_SLIDER_OUT);
			}

			state1 = check_act_status(ACT_QUEUE_Small_cylinder_slider, IN_PROGRESS, END_OK, NOT_HANDLED);
			state2 = check_act_status(ACT_QUEUE_Small_pompe_prise, IN_PROGRESS, END_OK, NOT_HANDLED); // Retour d'info par vacuose

			if(state1 != IN_PROGRESS && state2 != IN_PROGRESS ){
				if(state1 == END_OK && state2 == END_OK){
					// On l'a bien ventouse donc on continue
					state = ACTION_BRING_BACK_CYLINDER;
				}else if(state1 == END_OK && state2 == NOT_HANDLED){
					//Le bras est sortit sortit ou on a pas ventouse
					state = ERROR_ACTION_GO_TAKE_CYLINDER;
				}
			}
			break;

		case ERROR_ACTION_GO_TAKE_CYLINDER:
			//On retente les memes actions
			if(entrance){
				//On active la pompe avant d'avancer
				ACT_push_order( ACT_SMALL_POMPE_PRISE , ACT_POMPE_SMALL_SLIDER_NORMAL );
				ACT_push_order( ACT_SMALL_CYLINDER_SLIDER , ACT_SMALL_CYLINDER_SLIDER_OUT);
			}

			state1bis = check_act_status(ACT_QUEUE_Small_cylinder_slider, IN_PROGRESS, END_OK, NOT_HANDLED);
			state2bis = check_act_status(ACT_QUEUE_Small_pompe_prise, IN_PROGRESS, END_OK, NOT_HANDLED); // Retour d'info par vacuose

			if(state1bis != IN_PROGRESS && state2bis != IN_PROGRESS ){
				if(state1bis == END_OK && state2bis == END_OK){
					// On l'a bien ventouse donc on continue
					state = ACTION_BRING_BACK_CYLINDER;
				}else if(state1 == NOT_HANDLED && state1bis == NOT_HANDLED){
					//Le bras n'est pas sortit les deux fois, on part en error
					//On ne leve pas le flag car si le bras se debloque en bougeant, ca marchera la fois d'apres
					state = ERROR;
				}else if(state2 == NOT_HANDLED && state2bis == NOT_HANDLED){
					//On n'a pas ventouse les deux fois, il n'y a pas de cylindre
					state = NO_CYLINDER_DETECTED;
				}else{
					//On a eu des problemes differents les deux dernieres fois on ressaye
					state = ERROR_ACTION_GO_TAKE_CYLINDER_RETRY;
				}
			}
			break;

		case ERROR_ACTION_GO_TAKE_CYLINDER_RETRY:
			//On retente les memes actions
			if(entrance){
				//On active la pompe avant d'avancer
				ACT_push_order( ACT_SMALL_POMPE_PRISE , ACT_POMPE_NORMAL );
				ACT_push_order( ACT_SMALL_CYLINDER_SLIDER , ACT_SMALL_CYLINDER_SLIDER_OUT);
			}

			state1 = check_act_status(ACT_QUEUE_Small_cylinder_slider, IN_PROGRESS, END_OK, NOT_HANDLED);
			state2 = check_act_status(ACT_QUEUE_Small_pompe_prise, IN_PROGRESS, END_OK, NOT_HANDLED); // Retour d'info par vacuose

			if(state1 != IN_PROGRESS && state2 != IN_PROGRESS ){
				if(state1 == END_OK && state2 == END_OK){
					// On l'a bien ventouse donc on continue
					state = ACTION_BRING_BACK_CYLINDER;
				}else{
					//troisieme essai echoue, on vide la fusee et on part
					state = NO_CYLINDER_DETECTED;
				}
			}
			break;

		case NO_CYLINDER_DETECTED:
			if(entrance){
					ACT_push_order( ACT_SMALL_POMPE_PRISE, ACT_POMPE_STOP );
					ACT_push_order( ACT_SMALL_CYLINDER_SLIDER, ACT_SMALL_CYLINDER_SLIDER_IN );
			}
			// On vide entièrement la fusée et on a fini
			ROCKETS_removeAllModules(rocket);
			state = DONE;
			break;

		case ACTION_BRING_BACK_CYLINDER:
			if (entrance){
				if(STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_ELEVATOR, MODULE_STOCK_SMALL)){
					ACT_push_order( ACT_SMALL_CYLINDER_ELEVATOR , ACT_SMALL_CYLINDER_ELEVATOR_BOTTOM);
					state = check_act_status(ACT_QUEUE_Small_cylinder_elevator, IN_PROGRESS, ACTION_BRING_BACK_CYLINDER_2, ERROR);
				}else{
					state = ACTION_BRING_BACK_CYLINDER_2;
				}
			}

			break;

		case ACTION_BRING_BACK_CYLINDER_2:
			if(entrance){
				// Activation de la pompe de l'élévateur si on stocke le cylindre après
				if(STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_ELEVATOR, MODULE_STOCK_SMALL)){
					ACT_push_order( ACT_SMALL_POMPE_PRISE, ACT_POMPE_NORMAL );
				}
				//On rentre le bras dans le robot
				ACT_push_order( ACT_SMALL_CYLINDER_SLIDER, ACT_SMALL_CYLINDER_SLIDER_IN );

				// On redescent les slopes pour le stockage suivant
				if(STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_SLOPE, MODULE_STOCK_SMALL)){
					ACT_push_order(ACT_SMALL_CYLINDER_SLOPE, ACT_SMALL_CYLINDER_SLOPE_DOWN);
				}
			}

			// Vérification des ordres effectués
			state1 = check_act_status(ACT_QUEUE_Small_cylinder_slider, IN_PROGRESS, END_OK, NOT_HANDLED);

			if(state1 != IN_PROGRESS){
				if((STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_ELEVATOR, MODULE_STOCK_SMALL))){
					state = STOP_POMPE_SLIDER;	// Si l'élévateur n'est pas occupé, on continue le stockage
				}else if((!STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_ELEVATOR, MODULE_STOCK_SMALL))){
					state = COMPUTE_NEXT_CYLINDER;		// Si l'élévateur est occupé, on va prendre le cylindre suivant
				}

				// On met à jour les données
				STOCKS_addModule(moduleType, MODULE_STOCK_SMALL, STOCK_POS_ENTRY);

				ROCKETS_removeModule(rocket);
				moduleToTake = NO_SIDE;
			}
			break;

		case STOP_POMPE_SLIDER:
			if(entrance){
				time_timeout = global.absolute_time + 1000;
			}

			// On attend une seconde le temps que le ventousage se fasse bien
			if(global.absolute_time > time_timeout){
				state = ACTION_BRING_UP_CYLINDER;
				ACT_push_order( ACT_SMALL_POMPE_PRISE, ACT_POMPE_STOP );

				// Mise à jour des données
				STOCKS_makeModuleProgressTo(STOCK_PLACE_ENTRY_TO_ELEVATOR, MODULE_STOCK_SMALL);

			}
			break;

		case ACTION_BRING_UP_CYLINDER:
			if(entrance){
				// Si le stockage est possible
				if(STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_SLOPE , MODULE_STOCK_SMALL)){
					ACT_push_order( ACT_SMALL_CYLINDER_ELEVATOR, ACT_SMALL_CYLINDER_ELEVATOR_TOP);
				}else{ // sinon stocke en position milieu
					ACT_push_order( ACT_SMALL_CYLINDER_ELEVATOR, ACT_SMALL_CYLINDER_ELEVATOR_LOCK_WITH_CYLINDER);
				}
			}

			// Vérification des ordres effectués
			state1 = check_act_status(ACT_QUEUE_Small_cylinder_elevator, IN_PROGRESS, END_OK, NOT_HANDLED);

			if(state1 != IN_PROGRESS){
				state = ACTION_STOCK_UP_CYLINDER;

				// Mise à jour des données
				if(STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_SLOPE , MODULE_STOCK_SMALL)){
					STOCKS_makeModuleProgressTo(STOCK_PLACE_ELEVATOR_TO_CONTAINER, MODULE_STOCK_SMALL);
					state = ACTION_STOCK_UP_CYLINDER; // On continue le stockage
				}else{
					state = COMPUTE_NEXT_CYLINDER; // On passe au cylindre suivant
				}
			}
			break;

		case ACTION_STOCK_UP_CYLINDER:
			if(entrance){
				ACT_push_order( ACT_SMALL_CYLINDER_SLOPE, ACT_SMALL_CYLINDER_SLOPE_UP);
			}

			// Vérification des ordres effectués
			state = check_act_status(ACT_QUEUE_Small_cylinder_slope, state, ACTION_PUT_CYLINDER_IN_CONTAINER, ACTION_PUT_CYLINDER_IN_CONTAINER);

			break;

		case ACTION_PUT_CYLINDER_IN_CONTAINER:
			if(entrance){
				ACT_push_order(ACT_SMALL_POMPE_PRISE, ACT_POMPE_STOP);
			}

			state = wait_time(1000, state, ACTION_PUT_SLOPE_DOWN);	// On attends un peu le temps que le cylindre roule

			if(ON_LEAVE()){
				// Mise à jour des données
				STOCKS_makeModuleProgressTo(STOCK_PLACE_ELEVATOR_TO_CONTAINER, MODULE_STOCK_SMALL);
			}
			break;

		case ACTION_PUT_SLOPE_DOWN:
			if(entrance){
				ACT_push_order(ACT_SMALL_CYLINDER_SLOPE, ACT_SMALL_CYLINDER_SLOPE_VERY_UP);
			}

			// Vérification des ordres effectués
			state = check_act_status(ACT_QUEUE_Small_cylinder_slope, state, COMPUTE_NEXT_CYLINDER, COMPUTE_NEXT_CYLINDER);
			break;

		case ERROR_DISABLE_ACT:
			state = COMPUTE_NEXT_CYLINDER;
			break;

		case DONE:
			if(entrance){
				// On active la dépose
				set_sub_act_enable(SUB_HARRY_DEPOSE_MODULES, TRUE);
				// On range les sliders
				ACT_push_order(ACT_SMALL_CYLINDER_SLIDER, ACT_SMALL_CYLINDER_SLIDER_IN);
			}
			RESET_MAE();
			on_turning_point();
			return END_OK;
			break;

		case ERROR:
			if(entrance){
				// On range les sliders
				ACT_push_order(ACT_SMALL_CYLINDER_SLIDER, ACT_SMALL_CYLINDER_SLIDER_IN);
			}
			RESET_MAE();
			on_turning_point();
			return NOT_HANDLED;
			break;

		default:
			if(entrance)
				debug_printf("default case in sub_act_harry_take_rocket\n");
			break;
	}

	return IN_PROGRESS;
}
