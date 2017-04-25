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
	static ELEMENTS_side_e moduleToStore = NO_SIDE;		// Module en cours de stockage
	static error_e state1 = IN_PROGRESS, state2 = IN_PROGRESS;
	static error_e state3 = IN_PROGRESS;
	static moduleType_e moduleType = MODULE_EMPTY;
	static time32_t time_timeout;


	switch(state){
		case INIT:
			if(IHM_switchs_get(SWITCH_DISABLE_MODULE_RIGHT)){
				ELEMENTS_set_flag(FLAG_HARRY_DISABLE_MODULE_RIGHT, TRUE);
				debug_printf("DESACTIVATION module right\n");
			}

			if(IHM_switchs_get(SWITCH_DISABLE_MODULE_LEFT)){
				ELEMENTS_set_flag(FLAG_HARRY_DISABLE_MODULE_LEFT, TRUE);
				debug_printf("DESACTIVATION module left\n");
			}

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
			moduleToStore = NO_SIDE;

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
			}else if(rocketSide[indexSide] == RIGHT && !STOCKS_isFull(MODULE_STOCK_RIGHT) && !ELEMENTS_get_flag(FLAG_HARRY_DISABLE_MODULE_RIGHT)){
				moduleToTake = RIGHT;		// On demande RIGHT et il est dispo
			}else if(rocketSide[indexSide] == LEFT && !STOCKS_isFull(MODULE_STOCK_LEFT) && !ELEMENTS_get_flag(FLAG_HARRY_DISABLE_MODULE_LEFT)){
				moduleToTake = LEFT;		// On demande LEFT et il est dispo
			}else if(rocketSide[indexSide] == RIGHT && !STOCKS_isFull(MODULE_STOCK_LEFT) && !ELEMENTS_get_flag(FLAG_HARRY_DISABLE_MODULE_LEFT)){
				moduleToTake = LEFT;		// On demande RIGHT mais il n'est pas dispo par contre LEFT est dispo
			}else if(rocketSide[indexSide] == LEFT && !STOCKS_isFull(MODULE_STOCK_RIGHT) && !ELEMENTS_get_flag(FLAG_HARRY_DISABLE_MODULE_RIGHT)){
				moduleToTake = RIGHT;		// On demande LEFT mais il n'est pas dispo par contre RIGHT est dispo
			}else{
				moduleToTake = NO_SIDE;		// Soit les 2 stocks sont plein soit plus aucun actionneur ne fonctionne.
			}
			indexSide++; // On incrémente l'index pour le prochain passage
			state = COMPUTE_ACTION;
			break;

		case COMPUTE_ACTION:
			if(moduleToTake == NO_SIDE){
				state = DONE; // On a fini ou rien n'est possible de faire
			}else{
				state = ACTION_GO_TAKE_CYLINDER;
			}
			break;

		case ACTION_GO_TAKE_CYLINDER:
			if (entrance){
				if(moduleToTake == RIGHT){
					ACT_push_order( ACT_CYLINDER_ELEVATOR_RIGHT , ACT_CYLINDER_ELEVATOR_RIGHT_LOCK_WITH_CYLINDER);
					state = check_act_status(ACT_QUEUE_Cylinder_elevator_right, IN_PROGRESS, ACTION_GO_TAKE_CYLINDER_2, ERROR);
				}else{
					ACT_push_order( ACT_CYLINDER_ELEVATOR_LEFT , ACT_CYLINDER_ELEVATOR_LEFT_LOCK_WITH_CYLINDER);
					state = check_act_status(ACT_QUEUE_Cylinder_elevator_left, IN_PROGRESS, ACTION_GO_TAKE_CYLINDER_2, ERROR);
				}
			}
			break;
		case ACTION_GO_TAKE_CYLINDER_2:
			if(entrance){
				if(moduleToTake == RIGHT){
					//On active la pompe avant d'avancer
					ACT_push_order( ACT_POMPE_SLIDER_RIGHT , ACT_POMPE_NORMAL );
					ACT_push_order( ACT_CYLINDER_SLIDER_RIGHT , ACT_CYLINDER_SLIDER_RIGHT_OUT);

					// Si c'est possible, on avance l'autre bras pour bloquer la chute du cylindre d'apres
					if(!ELEMENTS_get_flag(FLAG_HARRY_DISABLE_MODULE_LEFT) && STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_ENTRY, MODULE_STOCK_LEFT)){
						ACT_push_order( ACT_CYLINDER_SLIDER_LEFT , ACT_CYLINDER_SLIDER_LEFT_ALMOST_OUT );
					}
				}else{
					//On active la pompe avant d'avancer
					ACT_push_order( ACT_POMPE_SLIDER_LEFT , ACT_POMPE_NORMAL);
					ACT_push_order( ACT_CYLINDER_SLIDER_LEFT , ACT_CYLINDER_SLIDER_LEFT_OUT );

					// Si c'est possible, on avance l'autre bras pour bloquer la chute du cylindre d'apres
					if(!ELEMENTS_get_flag(FLAG_HARRY_DISABLE_MODULE_RIGHT)  && STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_ENTRY, MODULE_STOCK_RIGHT)){
						ACT_push_order( ACT_CYLINDER_SLIDER_RIGHT , ACT_CYLINDER_SLIDER_RIGHT_ALMOST_OUT );
					}
				}

				// On redescent les elevateurs pour le stockage suivant
				if(STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_ELEVATOR, MODULE_STOCK_RIGHT)){
					ACT_push_order(ACT_CYLINDER_ELEVATOR_RIGHT, ACT_CYLINDER_ELEVATOR_RIGHT_BOTTOM);
				}
				if(STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_ELEVATOR, MODULE_STOCK_LEFT)){
					ACT_push_order(ACT_CYLINDER_ELEVATOR_LEFT, ACT_CYLINDER_ELEVATOR_LEFT_BOTTOM);
				}
			}

			// Vérification des ordres effectués
			if(moduleToTake == RIGHT){
				state1 = check_act_status(ACT_QUEUE_Cylinder_slider_right, IN_PROGRESS, END_OK, NOT_HANDLED);
				state2 = check_act_status(ACT_QUEUE_Pompe_act_slider_right, IN_PROGRESS, END_OK, NOT_HANDLED); // Retour d'info par vacuose
				state3 = check_act_status(ACT_QUEUE_Cylinder_slider_left, IN_PROGRESS, END_OK, NOT_HANDLED);
			}else{
				state1 = check_act_status(ACT_QUEUE_Cylinder_slider_left, IN_PROGRESS, END_OK, NOT_HANDLED);
				state2 = check_act_status(ACT_QUEUE_Pompe_act_slider_left, IN_PROGRESS, END_OK, NOT_HANDLED); // Retour d'info par vacuose
				state3 = check_act_status(ACT_QUEUE_Cylinder_slider_right, IN_PROGRESS, END_OK, NOT_HANDLED);
			}

			if(state1 != IN_PROGRESS && state2 != IN_PROGRESS && state3 != IN_PROGRESS){
				if(state1 == END_OK && state2 == END_OK){
					// PROTECT_NEXT_FALL introduit une protection supplémentaire en retirant le cylindre en 2 mouvements
					//state = PROTECT_NEXT_FALL;
					// ACTION_BRING_BACK_CYLINDER retire le cylindre en un seul mouvement
					state = ACTION_BRING_BACK_CYLINDER;
				}else{
					state = ERROR_ACTION_GO_TAKE_CYLINDER;
				}
			}
			break;

		case ERROR_ACTION_GO_TAKE_CYLINDER:
			if(entrance){
				if(state2 == NOT_HANDLED){	// La pompe n'a pas ventousé, on se remet en place pour réessayer
					if(moduleToTake == RIGHT){
						ACT_push_order( ACT_CYLINDER_SLIDER_RIGHT , ACT_CYLINDER_SLIDER_RIGHT_IN );
						if(!ELEMENTS_get_flag(FLAG_HARRY_DISABLE_MODULE_LEFT) && state3 == END_OK){
							ACT_push_order( ACT_CYLINDER_SLIDER_LEFT , ACT_CYLINDER_SLIDER_LEFT_OUT );//Pas ventouser donc peut etre decaler, alors OUT pour remettre en place
						}
					}else if(moduleToTake == LEFT){
						ACT_push_order( ACT_CYLINDER_SLIDER_LEFT , ACT_CYLINDER_SLIDER_LEFT_IN );
						if(!ELEMENTS_get_flag(FLAG_HARRY_DISABLE_MODULE_RIGHT) && state3 == END_OK){
							ACT_push_order( ACT_CYLINDER_SLIDER_RIGHT , ACT_CYLINDER_SLIDER_RIGHT_OUT );
						}
					}
				}
				if(state3 == NOT_HANDLED){	// Le bras de l'autre côté est partie en erreur, on le rentre
					if(moduleToTake == RIGHT && !ELEMENTS_get_flag(FLAG_HARRY_DISABLE_MODULE_LEFT)){
						ACT_push_order( ACT_CYLINDER_SLIDER_LEFT , ACT_CYLINDER_SLIDER_LEFT_IN );
					}else if(moduleToTake == LEFT && !ELEMENTS_get_flag(FLAG_HARRY_DISABLE_MODULE_RIGHT)){
						ACT_push_order( ACT_CYLINDER_SLIDER_RIGHT , ACT_CYLINDER_SLIDER_RIGHT_IN );
					}
				}
			}

			if(state2 == NOT_HANDLED){
				state = ERROR_ACTION_GO_TAKE_CYLINDER_RETRY;
			}else{
				state = ERROR_DISABLE_ACT;
				if(moduleToTake == RIGHT){
					ELEMENTS_set_flag(FLAG_HARRY_DISABLE_MODULE_RIGHT, TRUE);
				}else{
					ELEMENTS_set_flag(FLAG_HARRY_DISABLE_MODULE_LEFT, TRUE);
				}
			}
			break;

		case ERROR_ACTION_GO_TAKE_CYLINDER_RETRY:
			if(entrance){
				if(moduleToTake == RIGHT){
					ACT_push_order( ACT_CYLINDER_SLIDER_RIGHT , ACT_CYLINDER_SLIDER_RIGHT_OUT );

					// Si c'est possible, on avance l'autre bras pour bloquer la chute du cylindre d'apres
					if(!ELEMENTS_get_flag(FLAG_HARRY_DISABLE_MODULE_LEFT)){
						ACT_push_order( ACT_CYLINDER_SLIDER_LEFT , ACT_CYLINDER_SLIDER_LEFT_ALMOST_OUT );
					}
				}else{
					ACT_push_order( ACT_CYLINDER_SLIDER_LEFT , ACT_CYLINDER_SLIDER_LEFT_OUT );

					// Si c'est possible, on avance l'autre bras pour bloquer la chute du cylindre d'apres
					if(!ELEMENTS_get_flag(FLAG_HARRY_DISABLE_MODULE_RIGHT)){
						ACT_push_order( ACT_CYLINDER_SLIDER_RIGHT , ACT_CYLINDER_SLIDER_RIGHT_ALMOST_OUT );
					}
				}
			}

			// Vérification des ordres effectués
			if(moduleToTake == RIGHT){
				state1 = check_act_status(ACT_QUEUE_Cylinder_slider_right, IN_PROGRESS, END_OK, NOT_HANDLED);
				state2 = check_act_status(ACT_QUEUE_Pompe_act_slider_right, IN_PROGRESS, END_OK, NOT_HANDLED); // Retour d'info par vacuose
				state3 = check_act_status(ACT_QUEUE_Cylinder_slider_left, IN_PROGRESS, END_OK, NOT_HANDLED);
			}else{
				state1 = check_act_status(ACT_QUEUE_Cylinder_slider_left, IN_PROGRESS, END_OK, NOT_HANDLED);
				state2 = check_act_status(ACT_QUEUE_Pompe_act_slider_left, IN_PROGRESS, END_OK, NOT_HANDLED); // Retour d'info par vacuose
				state3 = check_act_status(ACT_QUEUE_Cylinder_slider_right, IN_PROGRESS, END_OK, NOT_HANDLED);
			}

			if(state1 != IN_PROGRESS && state2 != IN_PROGRESS){
				if(state2 == END_OK){  // On a réussi
					if((moduleToTake == RIGHT && ELEMENTS_get_flag(FLAG_HARRY_DISABLE_MODULE_LEFT)) || (moduleToTake == LEFT && ELEMENTS_get_flag(FLAG_HARRY_DISABLE_MODULE_RIGHT))){
						state = PROTECT_NEXT_FALL;
					}else{
						state = ACTION_BRING_BACK_CYLINDER;
					}
				}else{
					state = NO_CYLINDER_DETECTED;	// On a encore échoué => il n'y a plus de cylindre ici
				}
			}
			break;

		case NO_CYLINDER_DETECTED:
			if(entrance){
				if(moduleToTake == RIGHT){
					ACT_push_order( ACT_POMPE_SLIDER_RIGHT , ACT_POMPE_STOP );
					ACT_push_order( ACT_CYLINDER_SLIDER_RIGHT , ACT_CYLINDER_SLIDER_RIGHT_IN );
					if(!ELEMENTS_get_flag(FLAG_HARRY_DISABLE_MODULE_LEFT) && state3 == END_OK){
						ACT_push_order( ACT_CYLINDER_SLIDER_LEFT , ACT_CYLINDER_SLIDER_LEFT_OUT );
					}
				}else if(moduleToTake == LEFT){
					ACT_push_order( ACT_POMPE_SLIDER_LEFT , ACT_POMPE_STOP );
					ACT_push_order( ACT_CYLINDER_SLIDER_LEFT , ACT_CYLINDER_SLIDER_LEFT_IN );
					if(!ELEMENTS_get_flag(FLAG_HARRY_DISABLE_MODULE_RIGHT) && state3 == END_OK){
						ACT_push_order( ACT_CYLINDER_SLIDER_RIGHT , ACT_CYLINDER_SLIDER_RIGHT_OUT );
					}
				}
			}

			// On vide entièrement la fusée et on a fini
			ROCKETS_removeAllModules(rocket);
			state = DONE;
			break;

		case PROTECT_NEXT_FALL: // On retire le cylindre en deux fois pour mieux protéger la chute du prochain cylindre
			if(entrance){
				if(moduleToTake == RIGHT){
					ACT_push_order( ACT_CYLINDER_SLIDER_RIGHT , ACT_CYLINDER_SLIDER_RIGHT_ALMOST_OUT_WITH_CYLINDER );
				}else{
					ACT_push_order( ACT_CYLINDER_SLIDER_LEFT , ACT_CYLINDER_SLIDER_LEFT_ALMOST_OUT_WITH_CYLINDER );
				}
			}

			// Pas de gestion d'erreur ici, on continue (check_act_status nécéssaire pour attendre la fin de l'action)
			// Cette action n'étant pas essentielle, on essaie de pousuivre l'action car on veut des points
			if(moduleToTake == RIGHT){
				state = check_act_status(ACT_QUEUE_Cylinder_slider_right, state, ACTION_BRING_BACK_CYLINDER, ACTION_BRING_BACK_CYLINDER);
			}else{
				state = check_act_status(ACT_QUEUE_Cylinder_slider_right, state, ACTION_BRING_BACK_CYLINDER, ACTION_BRING_BACK_CYLINDER);
			}
			break;

		case ACTION_BRING_BACK_CYLINDER:
			if (entrance){
				if(moduleToTake == RIGHT){
					if(STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_ELEVATOR, MODULE_STOCK_RIGHT)){
						ACT_push_order( ACT_CYLINDER_ELEVATOR_RIGHT , ACT_CYLINDER_ELEVATOR_RIGHT_BOTTOM);
						state = check_act_status(ACT_QUEUE_Cylinder_elevator_right, IN_PROGRESS, ACTION_BRING_BACK_CYLINDER_2, ERROR);
					}else{
						state = ACTION_BRING_BACK_CYLINDER_2;
					}

				}else{
					if(STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_ELEVATOR, MODULE_STOCK_LEFT)){
						ACT_push_order( ACT_CYLINDER_ELEVATOR_LEFT , ACT_CYLINDER_ELEVATOR_LEFT_BOTTOM);
						state = check_act_status(ACT_QUEUE_Cylinder_elevator_left, IN_PROGRESS, ACTION_BRING_BACK_CYLINDER_2, ERROR);
					}else{
						state = ACTION_BRING_BACK_CYLINDER_2;
					}
				}
			}
			break;

		case ACTION_BRING_BACK_CYLINDER_2:
			if(entrance){
				if(moduleToTake == RIGHT){
					// Activation de la pompe de l'élévateur si on stocke le cylindre après
					if(STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_ELEVATOR, MODULE_STOCK_RIGHT)){
						ACT_push_order( ACT_POMPE_ELEVATOR_RIGHT, ACT_POMPE_NORMAL );
					}
					//On rentre le bras dans le robot
					ACT_push_order( ACT_CYLINDER_SLIDER_RIGHT, ACT_CYLINDER_SLIDER_RIGHT_IN );
				}else{
					// Activation de la pompe de l'élévateur si on stocke le cylindre après
					if(STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_ELEVATOR, MODULE_STOCK_LEFT)){
						ACT_push_order( ACT_POMPE_ELEVATOR_LEFT, ACT_POMPE_NORMAL );
					}
					//On rentre le bras dans le robot
					ACT_push_order( ACT_CYLINDER_SLIDER_LEFT, ACT_CYLINDER_SLIDER_LEFT_IN );
				}

				// On redescent les slopes pour le stockage suivant
				if(STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_SLOPE, MODULE_STOCK_RIGHT)){
					ACT_push_order(ACT_CYLINDER_SLOPE_RIGHT, ACT_CYLINDER_SLOPE_RIGHT_DOWN);
				}
				if(STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_SLOPE, MODULE_STOCK_LEFT)){
					ACT_push_order(ACT_CYLINDER_SLOPE_LEFT, ACT_CYLINDER_SLOPE_LEFT_DOWN);
				}
			}

			// Vérification des ordres effectués
			if(moduleToTake == RIGHT){
				state1 = check_act_status(ACT_QUEUE_Cylinder_slider_right, IN_PROGRESS, END_OK, NOT_HANDLED);
			}else{
				state1 = check_act_status(ACT_QUEUE_Cylinder_slider_left, IN_PROGRESS, END_OK, NOT_HANDLED);
			}

			if(state1 != IN_PROGRESS){
				if((moduleToTake == RIGHT && STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_ELEVATOR, MODULE_STOCK_RIGHT))
				|| (moduleToTake == LEFT && STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_ELEVATOR, MODULE_STOCK_LEFT))){
					state = STOP_POMPE_SLIDER;	// Si l'élévateur n'est pas occupé, on continue le stockage
				}else if((moduleToTake == RIGHT && !STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_ELEVATOR, MODULE_STOCK_RIGHT))
				|| (moduleToTake == LEFT && !STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_ELEVATOR, MODULE_STOCK_LEFT))){
					state = COMPUTE_NEXT_CYLINDER;		// Si l'élévateur est occupé, on va prendre le cylindre suivant
				}

				// On met à jour les données
				if(moduleToTake == RIGHT){
					STOCKS_addModule(moduleType, MODULE_STOCK_RIGHT, STOCK_POS_ENTRY);
				}else{
					STOCKS_addModule(moduleType, MODULE_STOCK_LEFT, STOCK_POS_ENTRY);
				}
				ROCKETS_removeModule(rocket);
				moduleToStore = moduleToTake;
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
				if(moduleToStore == RIGHT){
					ACT_push_order( ACT_POMPE_SLIDER_RIGHT, ACT_POMPE_STOP );
				}else{
					ACT_push_order( ACT_POMPE_SLIDER_LEFT, ACT_POMPE_STOP );
				}

				// Mise à jour des données
				if(moduleToStore == RIGHT){
					STOCKS_makeModuleProgressTo(STOCK_PLACE_ENTRY_TO_ELEVATOR, MODULE_STOCK_RIGHT);
				}else{
					STOCKS_makeModuleProgressTo(STOCK_PLACE_ENTRY_TO_ELEVATOR, MODULE_STOCK_LEFT);
				}
			}
			break;

		case ACTION_BRING_UP_CYLINDER:
			if(entrance){
				if(moduleToStore == RIGHT){
					// Si le stockage est possible
					if(STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_SLOPE , MODULE_STOCK_RIGHT)){
						ACT_push_order( ACT_CYLINDER_ELEVATOR_RIGHT, ACT_CYLINDER_ELEVATOR_RIGHT_TOP);
					}else{ // sinon stocke en position milieu
						ACT_push_order( ACT_CYLINDER_ELEVATOR_RIGHT, ACT_CYLINDER_ELEVATOR_RIGHT_LOCK_WITH_CYLINDER);
					}
				}else{
					// Si le stockage est possible
					if(STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_SLOPE , MODULE_STOCK_LEFT)){
						ACT_push_order( ACT_CYLINDER_ELEVATOR_LEFT, ACT_CYLINDER_ELEVATOR_LEFT_TOP);
					}else{ // sinon stocke en position milieu
						ACT_push_order( ACT_CYLINDER_ELEVATOR_LEFT, ACT_CYLINDER_ELEVATOR_LEFT_LOCK_WITH_CYLINDER);
					}
				}
			}

			// Vérification des ordres effectués
			if(moduleToStore == RIGHT){
				state1 = check_act_status(ACT_QUEUE_Cylinder_elevator_right, IN_PROGRESS, END_OK, NOT_HANDLED);
			}else{
				state1 = check_act_status(ACT_QUEUE_Cylinder_elevator_left, IN_PROGRESS, END_OK, NOT_HANDLED);
			}

			if(state1 != IN_PROGRESS){
				state = ACTION_STOCK_UP_CYLINDER;

				// Mise à jour des données
				if(moduleToStore == RIGHT && STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_SLOPE , MODULE_STOCK_RIGHT)){
					STOCKS_makeModuleProgressTo(STOCK_PLACE_ELEVATOR_TO_CONTAINER, MODULE_STOCK_RIGHT);
					state = ACTION_STOCK_UP_CYLINDER; // On continue le stockage
				}else if(moduleToStore == LEFT && STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_SLOPE , MODULE_STOCK_LEFT)){
					STOCKS_makeModuleProgressTo(STOCK_PLACE_ELEVATOR_TO_CONTAINER, MODULE_STOCK_LEFT);
					state = ACTION_STOCK_UP_CYLINDER; // On continue le stockage
				}else{
					state = COMPUTE_NEXT_CYLINDER; // On passe au cylindre suivant
				}
			}
			break;

		case ACTION_STOCK_UP_CYLINDER:
			if(entrance){
				if(moduleToStore == RIGHT){
					ACT_push_order( ACT_CYLINDER_SLOPE_RIGHT, ACT_CYLINDER_SLOPE_RIGHT_UP);
				}else{
					ACT_push_order( ACT_CYLINDER_SLOPE_LEFT, ACT_CYLINDER_SLOPE_LEFT_UP);
				}
			}

			// Vérification des ordres effectués
			if(moduleToStore == RIGHT){
				state = check_act_status(ACT_QUEUE_Cylinder_slope_right, state, ACTION_PUT_CYLINDER_IN_CONTAINER, ACTION_PUT_CYLINDER_IN_CONTAINER);
			}else{
				state = check_act_status(ACT_QUEUE_Cylinder_slope_left, state, ACTION_PUT_CYLINDER_IN_CONTAINER, ACTION_PUT_CYLINDER_IN_CONTAINER);
			}
			break;

		case ACTION_PUT_CYLINDER_IN_CONTAINER:
			if(entrance){
				if(moduleToStore == RIGHT){
					ACT_push_order(ACT_POMPE_ELEVATOR_RIGHT, ACT_POMPE_STOP);
				}else{
					ACT_push_order(ACT_POMPE_ELEVATOR_LEFT, ACT_POMPE_STOP);
				}
			}

			state = wait_time(1000, state, ACTION_PUT_SLOPE_DOWN);	// On attends un peu le temps que le cylindre roule

			if(ON_LEAVE()){
				// Mise à jour des données
				if(moduleToStore == RIGHT){
					STOCKS_makeModuleProgressTo(STOCK_PLACE_ELEVATOR_TO_CONTAINER, MODULE_STOCK_RIGHT);
				}else if(moduleToStore == LEFT ){
					STOCKS_makeModuleProgressTo(STOCK_PLACE_ELEVATOR_TO_CONTAINER, MODULE_STOCK_LEFT);
				}
			}
			break;

		case ACTION_PUT_SLOPE_DOWN:
			if(entrance){
				if(moduleToStore == RIGHT){
					ACT_push_order(ACT_CYLINDER_SLOPE_RIGHT, ACT_CYLINDER_SLOPE_RIGHT_VERY_UP);
				}else{
					ACT_push_order(ACT_CYLINDER_SLOPE_LEFT, ACT_CYLINDER_SLOPE_LEFT_VERY_UP);
				}
			}

			// Vérification des ordres effectués
			if(moduleToStore == RIGHT){
				state = check_act_status(ACT_QUEUE_Cylinder_slope_right, state, COMPUTE_NEXT_CYLINDER, COMPUTE_NEXT_CYLINDER);
			}else{
				state = check_act_status(ACT_QUEUE_Cylinder_slope_left, state, COMPUTE_NEXT_CYLINDER, COMPUTE_NEXT_CYLINDER);
			}
			break;

		case ERROR_DISABLE_ACT:
			state = COMPUTE_NEXT_CYLINDER;
			break;

		case DONE:
			if(entrance){
				// On active la dépose
				set_sub_act_enable(SUB_HARRY_DEPOSE_MODULES, TRUE);
				// On range les sliders
				ACT_push_order(ACT_CYLINDER_SLIDER_RIGHT, ACT_CYLINDER_SLIDER_RIGHT_IN);
				ACT_push_order(ACT_CYLINDER_SLIDER_RIGHT, ACT_CYLINDER_SLIDER_RIGHT_IN);
			}
			RESET_MAE();
			on_turning_point();
			return END_OK;
			break;

		case ERROR:
			if(entrance){
				// On range les sliders
				ACT_push_order(ACT_CYLINDER_SLIDER_RIGHT, ACT_CYLINDER_SLIDER_RIGHT_IN);
				ACT_push_order(ACT_CYLINDER_SLIDER_RIGHT, ACT_CYLINDER_SLIDER_RIGHT_IN);
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
