#include "action_big.h"
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


// Private functions
//static GEOMETRY_point_t compute_take_point_rocket(GEOMETRY_point_t store_point, Sint16 angle_robot, Uint16 dist);


// Subaction actionneur de prise fus�e v2
error_e sub_act_harry_take_rocket_down_to_top(moduleRocketLocation_e rocket, ELEMENTS_side_e module_very_down, ELEMENTS_side_e module_down, ELEMENTS_side_e module_top, ELEMENTS_side_e module_very_top){
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
			ACTION_PUT_SLOPE_VERY_UP,
			AVANCE,
			AVANCE_ERROR,
			RECULE,
			RECULE_ERROR,

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
	static time32_t time_timeout_before_pompe_stop;
	static time32_t time_timeout_after_pompe_stop;
	static bool_e pompe_stop = FALSE;

	/*if(entrance){
		debug_printf("---------- MODULE_STOCK_LEFT ---------\n");
		STOCKS_print(MODULE_STOCK_LEFT);
		debug_printf("---------- MODULE_STOCK_RIGHT ---------\n");
		STOCKS_print(MODULE_STOCK_RIGHT);
	}*/

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
			indexSide = 0;
			state = COMPUTE_NEXT_CYLINDER;
			break;

		case COMPUTE_NEXT_CYLINDER:
			if(ROCKETS_isEmpty(rocket) || indexSide >= 4){
				moduleToTake = NO_SIDE;		// La fus�e est vide, nous avons fini.
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
			indexSide++; // On incr�mente l'index pour le prochain passage
			state = COMPUTE_ACTION;
			break;

		case COMPUTE_ACTION:
			if(moduleToTake == NO_SIDE){
				state = DONE; // On a fini ou rien n'est possible de faire
			}else{
				state = AVANCE;
			}
			break;

		case AVANCE:
			state = try_advance(NULL, entrance, 50, state, ACTION_GO_TAKE_CYLINDER, AVANCE_ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case AVANCE_ERROR:
			state= try_advance(NULL, entrance, 50, state, AVANCE, AVANCE, FAST, BACKWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case ACTION_GO_TAKE_CYLINDER:
			if (entrance){
				if(moduleToTake == RIGHT){
					ACT_push_order( ACT_CYLINDER_ELEVATOR_RIGHT , ACT_CYLINDER_ELEVATOR_RIGHT_LOCK_WITH_CYLINDER);
				}else{
					ACT_push_order( ACT_CYLINDER_ELEVATOR_LEFT , ACT_CYLINDER_ELEVATOR_LEFT_LOCK_WITH_CYLINDER);
				}
			}
			if(moduleToTake == RIGHT){
				state = check_act_status(ACT_QUEUE_Cylinder_elevator_right, state, ACTION_GO_TAKE_CYLINDER_2, ACTION_GO_TAKE_CYLINDER_2);
			}else{
				state = check_act_status(ACT_QUEUE_Cylinder_elevator_left, state, ACTION_GO_TAKE_CYLINDER_2, ACTION_GO_TAKE_CYLINDER_2);
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
				if(moduleToTake == RIGHT && STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_ELEVATOR, MODULE_STOCK_RIGHT)){
					ACT_push_order(ACT_CYLINDER_ELEVATOR_LEFT, ACT_CYLINDER_ELEVATOR_LEFT_BOTTOM);
				}else if(moduleToTake == LEFT && STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_ELEVATOR, MODULE_STOCK_LEFT)){
					ACT_push_order(ACT_CYLINDER_ELEVATOR_RIGHT, ACT_CYLINDER_ELEVATOR_RIGHT_BOTTOM);
				}
			}
			/*--------------------------------------------------------------------
			// On decide volontairement de pas checker pour gain de temps

			if(moduleToTake == RIGHT){
				state = check_act_status(ACT_QUEUE_Cylinder_elevator_left, state, ACTION_BRING_BACK_CYLINDER_2_STATES, ERROR);
			}else{
				state = check_act_status(ACT_QUEUE_Cylinder_elevator_right, state, ACTION_BRING_BACK_CYLINDER_2_STATES, ERROR);
			}
			----------------------------------------------------------------------*/

			// V�rification des ordres effectu�s
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
				/*if(state1 == END_OK && state2 == END_OK){
					// PROTECT_NEXT_FALL introduit une protection suppl�mentaire en retirant le cylindre en 2 mouvements
					//state = PROTECT_NEXT_FALL;
					// ACTION_BRING_BACK_CYLINDER retire le cylindre en un seul mouvement
					state = ACTION_BRING_BACK_CYLINDER;
				}else{
					state = ERROR_ACTION_GO_TAKE_CYLINDER;
				}*/

				// On met � jour les donn�es : Passage du module dans la position POS_ENTRY
				if(moduleToTake == RIGHT){
					STOCKS_addModule(moduleType, STOCK_POS_ENTRY, MODULE_STOCK_RIGHT);
				}else{
					STOCKS_addModule(moduleType, STOCK_POS_ENTRY, MODULE_STOCK_LEFT);
				}
				state = ACTION_BRING_BACK_CYLINDER;
			}
			break;

		case ERROR_ACTION_GO_TAKE_CYLINDER:
			if(entrance){
				if(state2 == NOT_HANDLED){	// La pompe n'a pas ventous�, on se remet en place pour r�essayer
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
				if(state3 == NOT_HANDLED){	// Le bras de l'autre c�t� est partie en erreur, on le rentre
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

			// V�rification des ordres effectu�s
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
				if(state2 == END_OK){  // On a r�ussi
					if((moduleToTake == RIGHT && ELEMENTS_get_flag(FLAG_HARRY_DISABLE_MODULE_LEFT)) || (moduleToTake == LEFT && ELEMENTS_get_flag(FLAG_HARRY_DISABLE_MODULE_RIGHT))){
						state = PROTECT_NEXT_FALL;
					}else{
						state = ACTION_BRING_BACK_CYLINDER;
					}
				}else{
					state = NO_CYLINDER_DETECTED;	// On a encore �chou� => il n'y a plus de cylindre ici
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

			// On vide enti�rement la fus�e et on a fini
			ROCKETS_removeAllModules(rocket);
			state = DONE;
			break;

		case PROTECT_NEXT_FALL: // On retire le cylindre en deux fois pour mieux prot�ger la chute du prochain cylindre
			if(entrance){
				if(moduleToTake == RIGHT){
					ACT_push_order( ACT_CYLINDER_SLIDER_RIGHT , ACT_CYLINDER_SLIDER_RIGHT_ALMOST_OUT_WITH_CYLINDER );
				}else{
					ACT_push_order( ACT_CYLINDER_SLIDER_LEFT , ACT_CYLINDER_SLIDER_LEFT_ALMOST_OUT_WITH_CYLINDER );
				}
			}

			// Pas de gestion d'erreur ici, on continue (check_act_status n�c�ssaire pour attendre la fin de l'action)
			// Cette action n'�tant pas essentielle, on essaie de pousuivre l'action car on veut des points
			if(moduleToTake == RIGHT){
				state = check_act_status(ACT_QUEUE_Cylinder_slider_right, state, ACTION_BRING_BACK_CYLINDER, ACTION_BRING_BACK_CYLINDER);
			}else{
				state = check_act_status(ACT_QUEUE_Cylinder_slider_left, state, ACTION_BRING_BACK_CYLINDER, ACTION_BRING_BACK_CYLINDER);
			}
			break;

		case ACTION_BRING_BACK_CYLINDER:
			if (entrance){
				if(moduleToTake == RIGHT){
					if(STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_ELEVATOR, MODULE_STOCK_RIGHT)){
						ACT_push_order( ACT_CYLINDER_ELEVATOR_RIGHT , ACT_CYLINDER_ELEVATOR_RIGHT_BOTTOM);
					}

				}else{
					if(STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_ELEVATOR, MODULE_STOCK_LEFT)){
						ACT_push_order( ACT_CYLINDER_ELEVATOR_LEFT , ACT_CYLINDER_ELEVATOR_LEFT_BOTTOM);
					}
				}
			}
			if(moduleToTake == RIGHT && STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_ELEVATOR, MODULE_STOCK_RIGHT)){
				state = check_act_status(ACT_QUEUE_Cylinder_elevator_right, state, ACTION_BRING_BACK_CYLINDER_2, ERROR);
			}
			else if(moduleToTake == LEFT && STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_ELEVATOR, MODULE_STOCK_LEFT)){
				state = check_act_status(ACT_QUEUE_Cylinder_elevator_left, state, ACTION_BRING_BACK_CYLINDER_2, ERROR);
			}
			else {
				state = ACTION_BRING_BACK_CYLINDER_2;
			}
			break;

		case ACTION_BRING_BACK_CYLINDER_2:
			if(entrance){
				if(moduleToTake == RIGHT){
					// Activation de la pompe de l'�l�vateur si on stocke le cylindre apr�s
					if(STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_ELEVATOR, MODULE_STOCK_RIGHT)){
						ACT_push_order( ACT_POMPE_ELEVATOR_RIGHT, ACT_POMPE_NORMAL );
					}
					//On rentre le bras dans le robot
					ACT_push_order( ACT_CYLINDER_SLIDER_RIGHT, ACT_CYLINDER_SLIDER_RIGHT_IN );
				}else{
					// Activation de la pompe de l'�l�vateur si on stocke le cylindre apr�s
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

			// V�rification des ordres effectu�s
			if(moduleToTake == RIGHT){
				state1 = check_act_status(ACT_QUEUE_Cylinder_slider_right, IN_PROGRESS, END_OK, NOT_HANDLED);
			}else{
				state1 = check_act_status(ACT_QUEUE_Cylinder_slider_left, IN_PROGRESS, END_OK, NOT_HANDLED);
			}

			if(state1 != IN_PROGRESS){
				if((moduleToTake == RIGHT && STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_ELEVATOR, MODULE_STOCK_RIGHT))
				|| (moduleToTake == LEFT && STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_ELEVATOR, MODULE_STOCK_LEFT))){
					state = STOP_POMPE_SLIDER;	// Si l'�l�vateur n'est pas occup�, on continue le stockage
				}else if((moduleToTake == RIGHT && !STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_ELEVATOR, MODULE_STOCK_RIGHT))
				|| (moduleToTake == LEFT && !STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_ELEVATOR, MODULE_STOCK_LEFT))){
					state = COMPUTE_NEXT_CYLINDER;		// Si l'�l�vateur est occup�, on va prendre le cylindre suivant
				}
			}
			break;

		case STOP_POMPE_SLIDER:
			if(entrance){
				time_timeout_before_pompe_stop = global.absolute_time + 1000;
				time_timeout_after_pompe_stop = global.absolute_time + 2000;
				pompe_stop =  FALSE;
			}

			if(global.absolute_time > time_timeout_before_pompe_stop && pompe_stop == FALSE){
				if(moduleToTake == RIGHT){
					ACT_push_order( ACT_POMPE_SLIDER_RIGHT, ACT_POMPE_STOP );
				}else{
					ACT_push_order( ACT_POMPE_SLIDER_LEFT, ACT_POMPE_STOP );
				}
				pompe_stop=TRUE;
			}

			// On attend une seconde le temps que le ventousage se fasse bien
			if(global.absolute_time > time_timeout_after_pompe_stop){
				pompe_stop=FALSE;
				state = RECULE;

				// On met � jour les donn�es : Passage du module de POS_ENTRY � POS_ELEVATOR
				if(moduleToTake == RIGHT){
					STOCKS_makeModuleProgressTo(STOCK_PLACE_ENTRY_TO_ELEVATOR, MODULE_STOCK_RIGHT);
				}else{
					STOCKS_makeModuleProgressTo(STOCK_PLACE_ENTRY_TO_ELEVATOR, MODULE_STOCK_LEFT);
				}
				ROCKETS_removeModule(rocket); // Suppression du module de la fus�e
				moduleToStore = moduleToTake;
				moduleToTake = NO_SIDE;
			}
			break;

		case RECULE:
			state = try_advance(NULL, entrance, 50, state, ACTION_BRING_UP_CYLINDER, RECULE_ERROR, FAST, BACKWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case RECULE_ERROR:
			state= try_advance(NULL, entrance, 50, state, RECULE, RECULE, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
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

			// V�rification des ordres effectu�s
			if(moduleToStore == RIGHT){
				state1 = check_act_status(ACT_QUEUE_Cylinder_elevator_right, IN_PROGRESS, END_OK, NOT_HANDLED);
			}else{
				state1 = check_act_status(ACT_QUEUE_Cylinder_elevator_left, IN_PROGRESS, END_OK, NOT_HANDLED);
			}

			if(state1 != IN_PROGRESS){
				if((moduleToStore == RIGHT && STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_SLOPE , MODULE_STOCK_RIGHT))
					|| (moduleToStore == LEFT && STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_SLOPE , MODULE_STOCK_LEFT)) ){
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

			// V�rification des ordres effectu�s
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

			state = wait_time(1000, state, ACTION_PUT_SLOPE_VERY_UP);	// On attends un peu le temps que le cylindre roule

			if(ON_LEAVE()){
				// On met � jour les donn�es : Passage du module de POS_ELEVATOR vers le container (POS_SLOPE, POS_CONTAINER ou POS_BALANCER)
				if(moduleToStore == RIGHT){
					STOCKS_makeModuleProgressTo(STOCK_PLACE_ELEVATOR_TO_CONTAINER, MODULE_STOCK_RIGHT);
				}else{
					STOCKS_makeModuleProgressTo(STOCK_PLACE_ELEVATOR_TO_CONTAINER, MODULE_STOCK_LEFT);
				}
			}
			break;

		case ACTION_PUT_SLOPE_VERY_UP:
			if(entrance){
				if(moduleToStore == RIGHT){
					ACT_push_order(ACT_CYLINDER_SLOPE_RIGHT, ACT_CYLINDER_SLOPE_RIGHT_VERY_UP);
				}else{
					ACT_push_order(ACT_CYLINDER_SLOPE_LEFT, ACT_CYLINDER_SLOPE_LEFT_VERY_UP);
				}
			}

			// V�rification des ordres effectu�s
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
				// On active la d�pose
				set_sub_act_enable(SUB_HARRY_DEPOSE_MODULES, TRUE);
				// On range les sliders
				ACT_push_order(ACT_CYLINDER_SLIDER_RIGHT, ACT_CYLINDER_SLIDER_RIGHT_IN);
				ACT_push_order(ACT_CYLINDER_SLIDER_LEFT, ACT_CYLINDER_SLIDER_LEFT_IN);
			}
			RESET_MAE();
			on_turning_point();
			return END_OK;
			break;

		case ERROR:
			if(entrance){
				// On range les sliders
				ACT_push_order(ACT_CYLINDER_SLIDER_RIGHT, ACT_CYLINDER_SLIDER_RIGHT_IN);
				ACT_push_order(ACT_CYLINDER_SLIDER_LEFT, ACT_CYLINDER_SLIDER_LEFT_IN);
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

// Subaction actionneur de prise module sur le terrain
error_e sub_act_harry_get_module(moduleStockPosition_e pos, ELEMENTS_side_e side){
	CREATE_MAE_WITH_VERBOSE(SM_ID_START_HARRY_GET_ONE_MODULE,
			INIT,

			STOP_POMPE_SLIDER,
			ACTION_BRING_UP_CYLINDER,
			ACTION_STOCK_UP_CYLINDER,
			ACTION_PUT_CYLINDER_IN_CONTAINER,
			ACTION_PUT_SLOPE_DOWN,

			ERROR_DISABLE_ACT,
			ERROR,
			DONE
		);


	static ELEMENTS_side_e moduleToTake = NO_SIDE;		// Module en cours de prise
	static ELEMENTS_side_e moduleToStore = NO_SIDE;		// Module en cours de stockage
	static time32_t time_timeout_before_pompe_stop;
	static time32_t time_timeout_after_pompe_stop;
	static bool_e pompe_stop;

	static error_e state1 = IN_PROGRESS;

	switch(state){
	case INIT:
		moduleToTake = side;
		state = STOP_POMPE_SLIDER;
		break;

	case STOP_POMPE_SLIDER:
			if(entrance){
				time_timeout_before_pompe_stop = global.absolute_time + 2000;
				time_timeout_after_pompe_stop = global.absolute_time + 3000;
				pompe_stop =  FALSE;
			}

			if(global.absolute_time > time_timeout_before_pompe_stop && pompe_stop == FALSE){
				if(side == RIGHT){
					ACT_push_order( ACT_POMPE_SLIDER_RIGHT, ACT_POMPE_STOP );
				}else{
					ACT_push_order( ACT_POMPE_SLIDER_LEFT, ACT_POMPE_STOP );
				}
				pompe_stop=TRUE;
			}

			// On attend une seconde le temps que le ventousage se fasse bien
			if(global.absolute_time > time_timeout_after_pompe_stop){
				pompe_stop=FALSE;
				state = ACTION_BRING_UP_CYLINDER;

				// On met � jour les donn�es : Passage du module de POS_ENTRY � POS_ELEVATOR
				if(side == RIGHT){
					STOCKS_makeModuleProgressTo(STOCK_PLACE_ENTRY_TO_ELEVATOR, MODULE_STOCK_RIGHT);
				}else{
					STOCKS_makeModuleProgressTo(STOCK_PLACE_ENTRY_TO_ELEVATOR, MODULE_STOCK_LEFT);
				}
				moduleToStore = moduleToTake;
				moduleToTake = NO_SIDE;
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

			// V�rification des ordres effectu�s
			if(moduleToStore == RIGHT){
				state1 = check_act_status(ACT_QUEUE_Cylinder_elevator_right, IN_PROGRESS, END_OK, NOT_HANDLED);
			}else{
				state1 = check_act_status(ACT_QUEUE_Cylinder_elevator_left, IN_PROGRESS, END_OK, NOT_HANDLED);
			}

			if(state1 != IN_PROGRESS){
				if((moduleToStore == RIGHT && STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_SLOPE , MODULE_STOCK_RIGHT))
					|| (moduleToStore == LEFT && STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_SLOPE , MODULE_STOCK_LEFT)) ){
					state = ACTION_STOCK_UP_CYLINDER; // On continue le stockage
				}else{
					state = DONE; // On passe au cylindre suivant
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

			// V�rification des ordres effectu�s
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
				// On met � jour les donn�es : Passage du module de POS_ELEVATOR vers le container (POS_SLOPE, POS_CONTAINER ou POS_BALANCER)
				if(moduleToStore == RIGHT){
					STOCKS_makeModuleProgressTo(STOCK_PLACE_ELEVATOR_TO_CONTAINER, MODULE_STOCK_RIGHT);
				}else{
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

			// V�rification des ordres effectu�s
			if(moduleToStore == RIGHT){
				state = check_act_status(ACT_QUEUE_Cylinder_slope_right, state, DONE, DONE);
			}else{
				state = check_act_status(ACT_QUEUE_Cylinder_slope_left, state, DONE, DONE);
			}
			break;

		case ERROR_DISABLE_ACT:
			state = DONE;
			break;

		case DONE:
			if(entrance){
				// On active la d�pose
				set_sub_act_enable(SUB_HARRY_DEPOSE_MODULES, TRUE);
				// On range les sliders
				ACT_push_order(ACT_CYLINDER_SLIDER_RIGHT, ACT_CYLINDER_SLIDER_RIGHT_IN);
				ACT_push_order(ACT_CYLINDER_SLIDER_LEFT, ACT_CYLINDER_SLIDER_LEFT_IN);
			}
			RESET_MAE();
			on_turning_point();
			return END_OK;
			break;

		case ERROR:
			if(entrance){
				// On range les sliders
				ACT_push_order(ACT_CYLINDER_SLIDER_RIGHT, ACT_CYLINDER_SLIDER_RIGHT_IN);
				ACT_push_order(ACT_CYLINDER_SLIDER_LEFT, ACT_CYLINDER_SLIDER_LEFT_IN);
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



// Subaction actionneur de stockage
error_e sub_act_harry_mae_store_modules(moduleStockLocation_e storage, bool_e trigger){
	CREATE_MAE_ACT(SM_ID_ACT_HARRY_MAE_STORE_MODULES,
			WAIT_TRIGGER,
			LAUNCH_CYLINDER_PREPARATION, // Parall�lisation de la pr�paration de la couleur
			INIT,
			COMPUTE_ACTION,

			// Le stockage des modules
			ELEVATOR_GO_BOTTOM,
			SLIDER_GO_IN,
			STOP_POMPE_SLIDER,
			CHECK_CONTAINER_IS_AVAILABLE,
			ELEVATOR_GO_MID_POS,
			PREPARE_SLOPE_FOR_ELEVATOR,
			ELEVATOR_GO_TOP_POS,
			STOCK_UP_CYLINDER,
			WAIT_STABILZATION,
			PUT_CYLINDER_IN_CONTAINER,
			SLOPE_GO_VERY_UP,

			// Actions � faire en fin de subaction
			ELEVATOR_GO_BOTTOM_TO_END,
			CHECK_DISPENSER_TO_END,
			DISPENSER_GO_UNLOCK_TO_END,
			WAIT_DISPENSER_TO_END,
			DISPENSER_GO_LOCK_TO_END,

			ERROR_DISABLE_ACT,
			ERROR,
			DONE
		);

	//-------------------- D�but gestion de la r�entrance ---------------------------
	static enum state_e stateRight = WAIT_TRIGGER, stateLeft = WAIT_TRIGGER;
	static enum state_e lastStateRight = WAIT_TRIGGER, lastStateLeft = WAIT_TRIGGER;
	error_e ret = IN_PROGRESS;

	// On charge l'�tat courant
	if(storage == MODULE_STOCK_RIGHT){
		state = stateRight;
		entrance = (stateRight != lastStateRight);
		last_state = lastStateRight;
		lastStateRight = stateRight;
	}else if(storage == MODULE_STOCK_LEFT){
		state = stateLeft;
		entrance = (stateLeft != lastStateLeft);
		last_state = lastStateLeft;
		lastStateLeft = stateLeft;
	}else{
		error_printf("sub_act_harry_mae_store_modules could only be called with MODULE_STOCK_RIGHT ou MODULE_STOCK_LEFT\n");
		return NOT_HANDLED;
	}

	// Verbose
	if(entrance && initialized){
		info_printf("[%s]", (storage == MODULE_STOCK_RIGHT)? "RIGHT" : "LEFT");
		UTILS_LOG_state_changed(WORD_CONVERT_TO_STRING(SM_ID_ACT_HARRY_MAE_STORE_MODULES), SM_ID_ACT_HARRY_MAE_STORE_MODULES, state_str[last_state], last_state, state_str[state], state);
	}
	if(!initialized){
		info_printf("[%s]", (storage == MODULE_STOCK_RIGHT)? "RIGHT" : "LEFT");
		UTILS_LOG_init_state(WORD_CONVERT_TO_STRING(SM_ID_ACT_HARRY_MAE_STORE_MODULES), SM_ID_ACT_HARRY_MAE_STORE_MODULES, state_str[state], state);
	}
	//---------------------- Fin gestion de la r�entrance ---------------------------

	//static error_e stateAct = IN_PROGRESS;
	static time32_t time_timeout;

#ifdef DISPLAY_STOCKS
	if(entrance){
		debug_printf("---------- MODULE_STOCK_LEFT ---------\n");
		STOCKS_print(MODULE_STOCK_LEFT);
		debug_printf("---------- MODULE_STOCK_RIGHT ---------\n");
		STOCKS_print(MODULE_STOCK_RIGHT);
	}
#endif

	switch(state){

		case WAIT_TRIGGER:
			if(trigger){
				state = LAUNCH_CYLINDER_PREPARATION;
				ELEMENTS_set_flag(FLAG_HARRY_STORAGE_LEFT_FINISH, FALSE);
				ELEMENTS_set_flag(FLAG_HARRY_STORAGE_RIGHT_FINISH, FALSE);
			}
			break;

		case LAUNCH_CYLINDER_PREPARATION:  // Parall�lisation de la pr�paration de la couleur
			if(!STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_CONTAINER, storage)){
				sub_act_harry_mae_prepare_modules_for_dispose(storage, TRUE);
			}
			state = INIT;
			break;

		case INIT:
			if(STOCKS_isFull(storage)){
				state = DONE;	// Il n'y a rien � faire
			}else{
				state = COMPUTE_ACTION; // Stockage possible
			}
			break;

		case COMPUTE_ACTION:
			if((storage == MODULE_STOCK_RIGHT && ELEMENTS_get_flag(FLAG_HARRY_DISABLE_MODULE_RIGHT))
			|| (storage == MODULE_STOCK_LEFT && ELEMENTS_get_flag(FLAG_HARRY_DISABLE_MODULE_LEFT))){
				state = ERROR;
			}else if(!STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_ELEVATOR, storage) && STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_SLOPE, storage)){  // L'�l�vateur n'est pas vide et le stockage est possible
				state = PREPARE_SLOPE_FOR_ELEVATOR; // On cherche a stocker le module d�j� pr�sent dans l'�l�vateur
			}else if(!STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_ENTRY, storage) && STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_ELEVATOR, storage)){ // Il y a un module en bas et l'�l�vateur est vide
				state = ELEVATOR_GO_BOTTOM; // On cherche a stocker le module qui est en bas
			}else if(STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_ELEVATOR, storage)){
				state = ELEVATOR_GO_BOTTOM_TO_END; // On redescend l'�l�vateur si besoin avant de finir
			}else{ // les stocks sont plein ou il n'y a plus rien � faire
				state = DONE;
			}
			break;


		case ELEVATOR_GO_BOTTOM:
			if(entrance){
				// On sort l�g�rement le slider et on descend l'�l�vateur
				// Le slider doit sortir un peu pour laisser la place � l'�l�vateur de passer.
				if(storage == MODULE_STOCK_RIGHT){
					ACT_push_order( ACT_CYLINDER_SLIDER_RIGHT, ACT_CYLINDER_SLIDER_RIGHT_ALMOST_OUT_WITH_CYLINDER);
					ACT_push_order( ACT_CYLINDER_ELEVATOR_RIGHT, ACT_CYLINDER_ELEVATOR_RIGHT_BOTTOM);
				}else{
					ACT_push_order( ACT_CYLINDER_SLIDER_LEFT, ACT_CYLINDER_SLIDER_LEFT_ALMOST_OUT_WITH_CYLINDER);
					ACT_push_order( ACT_CYLINDER_ELEVATOR_LEFT, ACT_CYLINDER_ELEVATOR_LEFT_BOTTOM);
				}
			}

			// V�rification des ordres effectu�s
			if(storage == MODULE_STOCK_RIGHT){
				state = check_act_status(ACT_QUEUE_Cylinder_elevator_right, state, SLIDER_GO_IN, ERROR_DISABLE_ACT);
			}else{
				state = check_act_status(ACT_QUEUE_Cylinder_elevator_left, state, SLIDER_GO_IN, ERROR_DISABLE_ACT);
			}
			break;

		case SLIDER_GO_IN:
			if(entrance){
				if(storage == MODULE_STOCK_RIGHT){
					ACT_push_order( ACT_POMPE_ELEVATOR_RIGHT , ACT_POMPE_NORMAL );
					ACT_push_order( ACT_CYLINDER_SLIDER_RIGHT , ACT_CYLINDER_SLIDER_RIGHT_IN );

					// Si c'est possible, on pr�pare la SLOPE pour un futur stockage
					if(STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_SLOPE , MODULE_STOCK_RIGHT)){
						ACT_push_order( ACT_CYLINDER_SLOPE_RIGHT, ACT_CYLINDER_SLOPE_RIGHT_DOWN);
					}

				}else{
					ACT_push_order( ACT_POMPE_ELEVATOR_LEFT , ACT_POMPE_NORMAL );
					ACT_push_order( ACT_CYLINDER_SLIDER_LEFT , ACT_CYLINDER_SLIDER_LEFT_IN );

					// Si c'est possible, on pr�pare la SLOPE pour un futur stockage
					if(STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_SLOPE , MODULE_STOCK_LEFT)){
						ACT_push_order( ACT_CYLINDER_SLOPE_LEFT, ACT_CYLINDER_SLOPE_LEFT_DOWN);
					}
				}
			}

			// V�rification des ordres effectu�s
			if(storage == MODULE_STOCK_RIGHT){
				state = check_act_status(ACT_QUEUE_Cylinder_slider_right, state, STOP_POMPE_SLIDER, STOP_POMPE_SLIDER);
			}else{
				state = check_act_status(ACT_QUEUE_Cylinder_slider_left, state, STOP_POMPE_SLIDER, STOP_POMPE_SLIDER);
			}
			break;

		case STOP_POMPE_SLIDER:
			if(entrance){
				time_timeout = global.absolute_time + 1000;
			}

			if(global.absolute_time > time_timeout){
				state = CHECK_CONTAINER_IS_AVAILABLE;
				if(storage == MODULE_STOCK_RIGHT){
					ACT_push_order( ACT_POMPE_SLIDER_RIGHT , ACT_POMPE_STOP );
				}else{
					ACT_push_order( ACT_POMPE_SLIDER_LEFT , ACT_POMPE_STOP );
				}

				// Mise � jour des donn�es
				STOCKS_makeModuleProgressTo(STOCK_PLACE_ENTRY_TO_ELEVATOR, storage);
			}
			break;

		case CHECK_CONTAINER_IS_AVAILABLE:
			if((storage == MODULE_STOCK_RIGHT && STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_SLOPE , MODULE_STOCK_RIGHT))
			|| (storage == MODULE_STOCK_LEFT && STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_SLOPE , MODULE_STOCK_LEFT)) ){
				state = ELEVATOR_GO_TOP_POS;
			}else{
				state = ELEVATOR_GO_MID_POS;
			}
			break;

		case ELEVATOR_GO_MID_POS:
			if(entrance){
				if(storage == MODULE_STOCK_RIGHT){
					ACT_push_order( ACT_CYLINDER_ELEVATOR_RIGHT, ACT_CYLINDER_ELEVATOR_RIGHT_LOCK_WITH_CYLINDER);
				}else{
					ACT_push_order( ACT_CYLINDER_ELEVATOR_LEFT, ACT_CYLINDER_ELEVATOR_LEFT_LOCK_WITH_CYLINDER);
				}
			}

			// V�rification des ordres effectu�s
			if(storage == MODULE_STOCK_RIGHT){
				state = check_act_status(ACT_QUEUE_Cylinder_elevator_right, state, COMPUTE_ACTION, COMPUTE_ACTION);
			}else{
				state = check_act_status(ACT_QUEUE_Cylinder_elevator_left, state, COMPUTE_ACTION, COMPUTE_ACTION);
			}
			break;

		case PREPARE_SLOPE_FOR_ELEVATOR:
			if(entrance){
				if(storage == MODULE_STOCK_RIGHT){
					ACT_push_order( ACT_CYLINDER_SLOPE_RIGHT, ACT_CYLINDER_SLOPE_RIGHT_DOWN);
				}else{
					ACT_push_order( ACT_CYLINDER_SLOPE_LEFT, ACT_CYLINDER_SLOPE_LEFT_DOWN);
				}
			}

			// V�rification des ordres effectu�s
			if(storage == MODULE_STOCK_RIGHT){
				state = check_act_status(ACT_QUEUE_Cylinder_slope_right, state, ELEVATOR_GO_TOP_POS, ELEVATOR_GO_TOP_POS);
			}else{
				state = check_act_status(ACT_QUEUE_Cylinder_slope_left, state, ELEVATOR_GO_TOP_POS, ELEVATOR_GO_TOP_POS);
			}
			break;

		case ELEVATOR_GO_TOP_POS:
			if(entrance){
				if(storage == MODULE_STOCK_RIGHT){
					ACT_push_order( ACT_CYLINDER_ELEVATOR_RIGHT, ACT_CYLINDER_ELEVATOR_RIGHT_TOP);
				}else{
					ACT_push_order( ACT_CYLINDER_ELEVATOR_LEFT, ACT_CYLINDER_ELEVATOR_LEFT_TOP);
				}
			}

			// V�rification des ordres effectu�s
			if(storage == MODULE_STOCK_RIGHT){
				state = check_act_status(ACT_QUEUE_Cylinder_elevator_right, state, STOCK_UP_CYLINDER, STOCK_UP_CYLINDER);
			}else{
				state = check_act_status(ACT_QUEUE_Cylinder_elevator_left, state, STOCK_UP_CYLINDER, STOCK_UP_CYLINDER);
			}
			break;

		case STOCK_UP_CYLINDER:
			if(entrance){
				if(storage == MODULE_STOCK_RIGHT){
					ACT_push_order( ACT_CYLINDER_SLOPE_RIGHT, ACT_CYLINDER_SLOPE_RIGHT_UP);
				}else{
					ACT_push_order( ACT_CYLINDER_SLOPE_LEFT, ACT_CYLINDER_SLOPE_LEFT_UP);
				}
			}

			// V�rification des ordres effectu�s
			if(storage == MODULE_STOCK_RIGHT){
				state = check_act_status(ACT_QUEUE_Cylinder_slope_right, state, WAIT_STABILZATION, WAIT_STABILZATION);
			}else{
				state = check_act_status(ACT_QUEUE_Cylinder_slope_left, state, WAIT_STABILZATION, WAIT_STABILZATION);
			}
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
				if(storage == MODULE_STOCK_RIGHT){
					ACT_push_order( ACT_POMPE_ELEVATOR_RIGHT, ACT_POMPE_STOP);
				}else{
					ACT_push_order( ACT_POMPE_ELEVATOR_LEFT, ACT_POMPE_STOP);
				}
			}

			if(global.absolute_time > time_timeout){
				//state = SLOPE_GO_VERY_UP;
				state = COMPUTE_ACTION;

				// Mise � jour des donn�es
				STOCKS_makeModuleProgressTo(STOCK_PLACE_ELEVATOR_TO_CONTAINER, storage);
			}
			break;

		case SLOPE_GO_VERY_UP: // �tat non utilis�
			if(entrance){
				if(storage == MODULE_STOCK_RIGHT){
					ACT_push_order(ACT_CYLINDER_SLOPE_RIGHT, ACT_CYLINDER_SLOPE_RIGHT_VERY_UP);
				}else{
					ACT_push_order(ACT_CYLINDER_SLOPE_LEFT, ACT_CYLINDER_SLOPE_LEFT_VERY_UP);
				}
			}

			// V�rification des ordres effectu�s
			if(storage == MODULE_STOCK_RIGHT){
				state = check_act_status(ACT_QUEUE_Cylinder_slope_right, state, COMPUTE_ACTION, COMPUTE_ACTION);
			}else{
				state = check_act_status(ACT_QUEUE_Cylinder_slope_left, state,  COMPUTE_ACTION, COMPUTE_ACTION);
			}
			break;

		case ELEVATOR_GO_BOTTOM_TO_END:
			if(entrance){
				// On descend l'�l�vateur
				if(storage == MODULE_STOCK_RIGHT){
					ACT_push_order( ACT_CYLINDER_ELEVATOR_RIGHT, ACT_CYLINDER_ELEVATOR_RIGHT_BOTTOM);
				}else{
					ACT_push_order( ACT_CYLINDER_ELEVATOR_LEFT, ACT_CYLINDER_ELEVATOR_LEFT_BOTTOM);
				}
			}

			// V�rification des ordres effectu�s
			if(storage == MODULE_STOCK_RIGHT){
				state = check_act_status(ACT_QUEUE_Cylinder_elevator_right, state, DONE, DONE);
			}else{
				state = check_act_status(ACT_QUEUE_Cylinder_elevator_left, state, DONE, DONE);
			}
			break;

		case ERROR_DISABLE_ACT:
			if(storage == MODULE_STOCK_RIGHT){
				ELEMENTS_set_flag(FLAG_HARRY_DISABLE_MODULE_RIGHT, TRUE);
			}else{
				ELEMENTS_set_flag(FLAG_HARRY_DISABLE_MODULE_LEFT, TRUE);
			}
			state = ERROR; // On ne peut plus rien faire
			break;

		case DONE:
			if(entrance){
				set_sub_act_enable(SUB_HARRY_DEPOSE_MODULES, TRUE);
				if(storage == MODULE_STOCK_RIGHT){
					debug_printf("Set flag FLAG_HARRY_STORAGE_RIGHT_FINISH\n");
					ELEMENTS_set_flag(FLAG_HARRY_STORAGE_RIGHT_FINISH, TRUE);
				}else if(storage == MODULE_STOCK_LEFT){
					debug_printf("Set flag FLAG_HARRY_STORAGE_LEFT_FINISH\n");
					ELEMENTS_set_flag(FLAG_HARRY_STORAGE_LEFT_FINISH, TRUE);
				}
			}
			RESET_MAE();
			on_turning_point();
			ret = END_OK;
			break;

		case ERROR:
			if(entrance){
				if(storage == MODULE_STOCK_RIGHT){
					debug_printf("Set flag FLAG_HARRY_STORAGE_RIGHT_FINISH\n");
					ELEMENTS_set_flag(FLAG_HARRY_STORAGE_RIGHT_FINISH, TRUE);
				}else if(storage == MODULE_STOCK_LEFT){
					debug_printf("Set flag FLAG_HARRY_STORAGE_LEFT_FINISH\n");
					ELEMENTS_set_flag(FLAG_HARRY_STORAGE_LEFT_FINISH, TRUE);
				}
			}
			RESET_MAE();
			on_turning_point();
			ret = NOT_HANDLED;
			break;

		default:
			if(entrance)
				debug_printf("default case in sub_act_harry_mae_store_modules\n");
			break;
	}

	if(storage == MODULE_STOCK_RIGHT){
		stateRight = state;
	}else if(storage == MODULE_STOCK_LEFT){
		stateLeft = state;
	} // else L'erreur a d�j� �t� affich�e

	return ret;
}








// Subaction actionneur de pr�paration de la d�pose des modules
error_e sub_act_harry_mae_prepare_modules_for_dispose(moduleStockLocation_e storage, bool_e trigger){
	CREATE_MAE_ACT(SM_ID_ACT_HARRY_MAE_PREPARE_MODULES_FOR_DISPOSE,
			WAIT_TRIGGER,
			INIT,
			WAIT_STORAGE,
			WAIT_MODULE_FALL,
			MOVE_BALANCER_OUT,
			BEGIN_CHECK_POSITION_BALANCER,
			ERROR_MOVE_BALANCER_OUT,
			CHECK_IF_TURN_FOR_COLOR_NEEDED,
			TURN_FOR_COLOR,
			WAIT_WHITE,
			WAIT_OUR_COLOR,
			STOP_TURN,
			END_CHECK_POSITION_BALANCER,
			END_MOVE_BALANCER_IN,
			ERROR,
			DONE
		);

	//-------------------- D�but gestion de la r�entrance ---------------------------
	static enum state_e stateRight = WAIT_TRIGGER, stateLeft = WAIT_TRIGGER;
	static enum state_e lastStateRight = DONE, lastStateLeft = DONE;
	error_e ret = IN_PROGRESS;

	// On charge l'�tat courant
	if(storage == MODULE_STOCK_RIGHT){
		state = stateRight;
		entrance = (stateRight != lastStateRight);
		last_state = lastStateRight;
		lastStateRight = stateRight;
	}else if(storage == MODULE_STOCK_LEFT){
		state = stateLeft;
		entrance = (stateLeft != lastStateLeft);
		last_state = lastStateLeft;
		lastStateLeft = stateLeft;
	}else{
		error_printf("sub_act_harry_mae_prepare_modules_for_dispose could only be called with MODULE_STOCK_RIGHT ou MODULE_STOCK_LEFT\n");
		return NOT_HANDLED;
	}

	// Verbose
	if(entrance && initialized){
		info_printf("[%s]", (storage == MODULE_STOCK_RIGHT)? "RIGHT" : "LEFT");
		UTILS_LOG_state_changed(WORD_CONVERT_TO_STRING(SM_ID_ACT_HARRY_MAE_PREPARE_MODULES_FOR_DISPOSE), SM_ID_ACT_HARRY_MAE_PREPARE_MODULES_FOR_DISPOSE, state_str[last_state], last_state, state_str[state], state);
	}
	if(!initialized){
		info_printf("[%s]", (storage == MODULE_STOCK_RIGHT)? "RIGHT" : "LEFT");
		UTILS_LOG_init_state(WORD_CONVERT_TO_STRING(SM_ID_ACT_HARRY_MAE_PREPARE_MODULES_FOR_DISPOSE), SM_ID_ACT_HARRY_MAE_PREPARE_MODULES_FOR_DISPOSE, state_str[state], state);
	}
	//---------------------- Fin gestion de la r�entrance ---------------------------

	#define TIMEOUT_COLOR	(4000)  // Temps au dela duquel on arr�te de tourner le module, on a �chou� a d�tect� la couleur
	#define NB_TRY_BALANCER_TOLERATED (3)
	static error_e stateAct = IN_PROGRESS;
	static time32_t time_timeout = 0;
	bool_e color_white = 0, color_blue = 0, color_yellow = 0;
	static Uint8 nb_errors_balancer_left = 0, nb_errors_balancer_right = 0;

	switch(state){

		case WAIT_TRIGGER:
			if(trigger){
				state = INIT;

				// On baisse le flag dans le cas o� cela n'a pas encore �t� fait
				if(storage == MODULE_STOCK_RIGHT){
					ELEMENTS_set_flag(FLAG_HARRY_MODULE_COLOR_RIGHT_SUCCESS, FALSE);
					ELEMENTS_set_flag(FLAG_HARRY_MODULE_COLOR_RIGHT_FINISH, FALSE);
					nb_errors_balancer_right = 0;
				}else{
					ELEMENTS_set_flag(FLAG_HARRY_MODULE_COLOR_LEFT_SUCCESS, FALSE);
					ELEMENTS_set_flag(FLAG_HARRY_MODULE_COLOR_LEFT_FINISH, FALSE);
					nb_errors_balancer_left = 0;
				}
			}
			break;

		case INIT:
			//Partie de code ajout�e � la coupe de Belgique qui permettait de mieux d�poser les cylindres
			if(entrance){
				if(storage == MODULE_STOCK_RIGHT){
					ACT_push_order(ACT_CYLINDER_SLOPE_RIGHT, ACT_CYLINDER_SLOPE_RIGHT_UP);
				}
				else{
					ACT_push_order(ACT_CYLINDER_SLOPE_LEFT, ACT_CYLINDER_SLOPE_LEFT_UP);
				}
			}

			if(!STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_BALANCER, storage)){
				state = MOVE_BALANCER_OUT; // Pr�paration de la d�pose possible
			}else{
				state = WAIT_STORAGE;	// Il n'y a pas de cylindre disponible //TODO d�clencher le stockage
			}
			break;

		case WAIT_STORAGE:
			if(entrance){
				time_timeout = global.absolute_time + 10000;
				sub_act_harry_mae_store_modules(storage, TRUE);
			}
			if(!STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_BALANCER, storage)){
				state = WAIT_MODULE_FALL;
			}else if(global.absolute_time > time_timeout){
				state = ERROR; // Le stockage a semble t il �chou�, on ne peut rien faire de plus ici
			}
			break;

		case WAIT_MODULE_FALL:	// On attend que le module tombe bien dans le balancer, c'est � dire que le stockage se termine bien m�caniquement
			if(entrance){
				time_timeout = global.absolute_time + 1500;
			}
			if(global.absolute_time > time_timeout){
				state = MOVE_BALANCER_OUT;
			}
			break;

		case MOVE_BALANCER_OUT:
			if(entrance){
				if(storage == MODULE_STOCK_RIGHT){
					ACT_push_order(ACT_CYLINDER_BALANCER_RIGHT, ACT_CYLINDER_BALANCER_RIGHT_OUT);
				}else{
					ACT_push_order(ACT_CYLINDER_BALANCER_LEFT, ACT_CYLINDER_BALANCER_LEFT_OUT);
				}
			}

			// V�rification des ordres effectu�s
			if(storage == MODULE_STOCK_RIGHT){
				state = check_act_status(ACT_QUEUE_Cylinder_balancer_right, state, BEGIN_CHECK_POSITION_BALANCER, BEGIN_CHECK_POSITION_BALANCER);
			}else{
				state = check_act_status(ACT_QUEUE_Cylinder_balancer_left, state, BEGIN_CHECK_POSITION_BALANCER, BEGIN_CHECK_POSITION_BALANCER);
			}
			break;

		case BEGIN_CHECK_POSITION_BALANCER:
			if(storage == MODULE_STOCK_RIGHT){
				stateAct = ACT_check_position_config(ACT_CYLINDER_BALANCER_RIGHT, ACT_CYLINDER_BALANCER_RIGHT_OUT);
			}else{
				stateAct = ACT_check_position_config(ACT_CYLINDER_BALANCER_LEFT, ACT_CYLINDER_BALANCER_LEFT_OUT);
			}

			if(stateAct != IN_PROGRESS){
				if(stateAct == NOT_HANDLED || stateAct == END_WITH_TIMEOUT){

					// On incr�mente l'erreur
					if(storage == MODULE_STOCK_RIGHT){
						nb_errors_balancer_right++;
					}else{
						nb_errors_balancer_left++;
					}

					// On regarde si on retente ou si on part en ERROR
					if((storage == MODULE_STOCK_RIGHT && nb_errors_balancer_right == NB_TRY_BALANCER_TOLERATED)
					|| (storage == MODULE_STOCK_LEFT && nb_errors_balancer_left == NB_TRY_BALANCER_TOLERATED)){
						state = ERROR_MOVE_BALANCER_OUT;	// L'actionneur n'est pas en position, on doit le mettre
					}else{
						state = ERROR;
					}
				}else{
					state = CHECK_IF_TURN_FOR_COLOR_NEEDED;  // C'est bon l'actionneur est en position

					// Mise � jour des donn�es : on fait progresser le module en POS_BALANCER vers la position POS_COLOR
					STOCKS_makeModuleProgressTo(STOCK_PLACE_BALANCER_TO_COLOR, storage);
				}
			}
			break;

		case ERROR_MOVE_BALANCER_OUT:
			if(entrance){
				if(storage == MODULE_STOCK_RIGHT){
					ACT_push_order(ACT_CYLINDER_BALANCER_RIGHT, ACT_CYLINDER_BALANCER_RIGHT_OUT);
				}else{
					ACT_push_order(ACT_CYLINDER_BALANCER_LEFT, ACT_CYLINDER_BALANCER_LEFT_OUT);
				}
			}
			// V�rification des ordres effectu�s
			if(storage == MODULE_STOCK_RIGHT){
				state = check_act_status(ACT_QUEUE_Cylinder_balancer_right, state, MOVE_BALANCER_OUT, MOVE_BALANCER_OUT);
			}else{
				state = check_act_status(ACT_QUEUE_Cylinder_balancer_left, state, MOVE_BALANCER_OUT, MOVE_BALANCER_OUT);
			}
			break;


		case CHECK_IF_TURN_FOR_COLOR_NEEDED:
			if(STOCKS_getModuleType(STOCK_POS_COLOR, storage) == MODULE_POLY){
				state = TURN_FOR_COLOR;  // Module polychrome : retournement n�c�ssaire
			}else{
				state = END_MOVE_BALANCER_IN; // Module unicouleur : pas de retournement
			}
			break;

		case TURN_FOR_COLOR:
			if(entrance){
				time_timeout = global.absolute_time + TIMEOUT_COLOR;
				if(storage == MODULE_STOCK_RIGHT){
					ACT_push_order(ACT_CYLINDER_COLOR_RIGHT, ACT_CYLINDER_COLOR_RIGHT_NORMAL_SPEED);
				}else{
					ACT_push_order(ACT_CYLINDER_COLOR_LEFT, ACT_CYLINDER_COLOR_LEFT_NORMAL_SPEED);
				}
			}
			// Aucune v�rification ici
			state = WAIT_WHITE;
			break;

		case WAIT_WHITE:
			if(global.absolute_time > time_timeout){
				state = STOP_TURN;   // Probl�me : on arrive pas a d�terminer la couleur
			}else if(storage == MODULE_STOCK_RIGHT){
				if(CW_is_color_detected(CW_SENSOR_RIGHT, CW_Channel_White, FALSE)){
					state = WAIT_OUR_COLOR;
				}
			}else{
				if(CW_is_color_detected(CW_SENSOR_LEFT, CW_Channel_White, FALSE)){
					state = WAIT_OUR_COLOR;
				}
			}
			break;

		case WAIT_OUR_COLOR:
			// On en profite pour retourner le balancer vers l'int�rieur du robot pour gagner du temps
			// Pas de v�rification du r�sultat ici, la couleur est prioritaire.
			if(entrance){
				if(storage == MODULE_STOCK_RIGHT){
					ACT_push_order(ACT_CYLINDER_BALANCER_RIGHT, ACT_CYLINDER_BALANCER_RIGHT_IN);
				}else{
					ACT_push_order(ACT_CYLINDER_BALANCER_LEFT, ACT_CYLINDER_BALANCER_LEFT_IN);
				}
				STOCKS_makeModuleProgressTo(STOCK_PLACE_CONTAINER_TO_BALANCER, storage);
			}

			// On attend notre couleur
			if(global.absolute_time > time_timeout){
				state = STOP_TURN;   // Probl�me : on arrive pas a d�terminer la couleur
			}else if(storage == MODULE_STOCK_RIGHT){

				color_white = CW_is_color_detected(CW_SENSOR_RIGHT, CW_Channel_White, FALSE);
				color_yellow = CW_is_color_detected(CW_SENSOR_RIGHT, CW_Channel_Yellow, FALSE);
				color_blue = (!color_white && !color_yellow);

				if( ((global.color == BLUE) && color_blue) || ((global.color == YELLOW) && color_yellow)){
					ELEMENTS_set_flag(FLAG_HARRY_MODULE_COLOR_RIGHT_SUCCESS, TRUE);
					state=STOP_TURN;
				}
			}else{
				color_white = CW_is_color_detected(CW_SENSOR_LEFT, CW_Channel_White, FALSE);
				color_yellow = CW_is_color_detected(CW_SENSOR_LEFT, CW_Channel_Yellow, FALSE);
				color_blue = (!color_white && !color_yellow);

				if( ((global.color == BLUE) && color_blue) || ((global.color == YELLOW) && color_yellow)){
					ELEMENTS_set_flag(FLAG_HARRY_MODULE_COLOR_LEFT_SUCCESS, TRUE);
					state=STOP_TURN;
				}
			}
			break;

		case STOP_TURN:
			if(entrance){
				if(storage == MODULE_STOCK_RIGHT){
					ACT_push_order(ACT_CYLINDER_COLOR_RIGHT, ACT_CYLINDER_COLOR_RIGHT_ZERO_SPEED);
				}else{
					ACT_push_order(ACT_CYLINDER_COLOR_LEFT, ACT_CYLINDER_COLOR_LEFT_ZERO_SPEED);
				}
			}
			// Aucune v�rification ici
			state = END_CHECK_POSITION_BALANCER;
			break;

		case END_CHECK_POSITION_BALANCER:
			if(storage == MODULE_STOCK_RIGHT){
				stateAct = ACT_check_position_config(ACT_CYLINDER_BALANCER_RIGHT, ACT_CYLINDER_BALANCER_RIGHT_IN);
			}else{
				stateAct = ACT_check_position_config(ACT_CYLINDER_BALANCER_LEFT, ACT_CYLINDER_BALANCER_LEFT_IN);
			}

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
				if(storage == MODULE_STOCK_RIGHT){
					ACT_push_order(ACT_CYLINDER_BALANCER_RIGHT, ACT_CYLINDER_BALANCER_RIGHT_IN);
				}else{
					ACT_push_order(ACT_CYLINDER_BALANCER_LEFT, ACT_CYLINDER_BALANCER_LEFT_IN);
				}

			}

			// Aucune v�rification
			state = DONE;

			// On exit
			if(state != END_MOVE_BALANCER_IN){
				// Mise � jour des donn�es : on d�cale les modules du stock
				STOCKS_makeModuleProgressTo(STOCK_PLACE_CONTAINER_TO_BALANCER, storage);
			}
			break;

		case DONE:
			if(storage == MODULE_STOCK_RIGHT){
				ELEMENTS_set_flag(FLAG_HARRY_MODULE_COLOR_RIGHT_FINISH, TRUE);
			}else{
				ELEMENTS_set_flag(FLAG_HARRY_MODULE_COLOR_LEFT_FINISH, TRUE);
			}
			RESET_MAE();
			on_turning_point();
			ret = END_OK;
			break;

		case ERROR:
			if(storage == MODULE_STOCK_RIGHT){
				ELEMENTS_set_flag(FLAG_HARRY_MODULE_COLOR_RIGHT_FINISH, TRUE);
			}else{
				ELEMENTS_set_flag(FLAG_HARRY_MODULE_COLOR_LEFT_FINISH, TRUE);
			}
			RESET_MAE();
			on_turning_point();
			ret = NOT_HANDLED;
			break;

		default:
			if(entrance)
				debug_printf("default case in sub_act_harry_mae_prepare_modules_for_dispose\n");
			break;
	}

	if(storage == MODULE_STOCK_RIGHT){
		stateRight = state;
	}else if(storage == MODULE_STOCK_LEFT){
		stateLeft = state;
	} // else L'erreur a d�j� �t� affich�e

	return ret;
}








// Subaction actionneur de d�pose des modules
error_e sub_act_harry_mae_dispose_modules(moduleStockLocation_e storage, arg_dipose_mae_e arg_dispose){
	CREATE_MAE_ACT(SM_ID_ACT_HARRY_MAE_DISPOSE_MODULES,
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

	//-------------------- D�but gestion de la r�entrance ---------------------------
	static enum state_e stateRight = INIT, stateLeft = INIT;
	static enum state_e lastStateRight = DONE, lastStateLeft = DONE;
	error_e ret = IN_PROGRESS;
	// On charge l'�tat courant
	if(storage == MODULE_STOCK_RIGHT){
		state = stateRight;
		entrance = (stateRight != lastStateRight);
		last_state = lastStateRight;
		lastStateRight = stateRight;
	}else if(storage == MODULE_STOCK_LEFT){
		state = stateLeft;
		entrance = (stateLeft != lastStateLeft);
		last_state = lastStateLeft;
		lastStateLeft = stateLeft;
	}else{
		error_printf("sub_act_harry_mae_dispose_modules could only be called with MODULE_STOCK_RIGHT ou MODULE_STOCK_LEFT\n");
		return NOT_HANDLED;
	}

	// Verbose
	if(entrance && initialized){
		info_printf("[%s]", (storage == MODULE_STOCK_RIGHT)? "RIGHT" : "LEFT");
		UTILS_LOG_state_changed(WORD_CONVERT_TO_STRING(SM_ID_ACT_HARRY_MAE_DISPOSE_MODULES), SM_ID_ACT_HARRY_MAE_DISPOSE_MODULES, state_str[last_state], last_state, state_str[state], state);
	}
	if(!initialized){
		info_printf("[%s]", (storage == MODULE_STOCK_RIGHT)? "RIGHT" : "LEFT");
		UTILS_LOG_init_state(WORD_CONVERT_TO_STRING(SM_ID_ACT_HARRY_MAE_DISPOSE_MODULES), SM_ID_ACT_HARRY_MAE_DISPOSE_MODULES, state_str[state], state);
	}
	//---------------------- Fin gestion de la r�entrance ---------------------------

	static error_e stateAct = IN_PROGRESS;
	static bool_e anotherDisposeWillFollow = FALSE;
	static time32_t time_timeout = 0;
	static bool_e subaction_sucess = TRUE;

#ifdef DISPLAY_STOCKS
	if(entrance){
		debug_printf("---------- MODULE_STOCK_LEFT ---------\n");
		STOCKS_print(MODULE_STOCK_LEFT);
		debug_printf("---------- MODULE_STOCK_RIGHT ---------\n");
		STOCKS_print(MODULE_STOCK_RIGHT);
	}
#endif

	switch(state){

		case INIT:
			if(STOCKS_isEmpty(storage)){
				state = DONE;	// Il n'y a rien � faire
			}else{
				if(arg_dispose == ARG_DISPOSE_ONE_CYLINDER_FOLLOW_BY_ANOTHER){
					state = INIT_ARM_SERVO; // D�pose possible
					anotherDisposeWillFollow = TRUE; // Cette d�pose n'est pas la derni�re, une autre va suivre
				}else if(arg_dispose == ARG_DISPOSE_ONE_CYLINDER_AND_FINISH){
					state = INIT_ARM_SERVO; // D�pose possible
					anotherDisposeWillFollow = FALSE; // Cette d�pose est la derni�re, on doit ranger le bras � la fin
				}else{	// arg == ARG_STORE_ARM
					state = CHOOSE_ARM_STORAGE_POS;
					anotherDisposeWillFollow = FALSE; // Important : il n'y a pas de d�pose � suivre, on tente de ranger le bras dans le robot
				}
			}
			break;

		case INIT_ARM_SERVO:
			if(entrance){
				if(storage == MODULE_STOCK_RIGHT){
					ACT_push_order(ACT_CYLINDER_ARM_RIGHT, ACT_CYLINDER_ARM_RIGHT_PREPARE_TO_TAKE);
				}else{
					ACT_push_order(ACT_CYLINDER_ARM_LEFT, ACT_CYLINDER_ARM_LEFT_PREPARE_TO_TAKE);
				}
			}

			if(storage == MODULE_STOCK_RIGHT){
				state = check_act_status(ACT_QUEUE_Cylinder_arm_right, state, INIT_DISPOSE_SERVO, INIT_DISPOSE_SERVO);
			}else{
				state = check_act_status(ACT_QUEUE_Cylinder_arm_left, state, INIT_DISPOSE_SERVO, INIT_DISPOSE_SERVO);
			}
			break;

		case INIT_DISPOSE_SERVO:
			if(entrance){
				if(storage == MODULE_STOCK_RIGHT){
					ACT_push_order(ACT_CYLINDER_DISPOSE_RIGHT, ACT_CYLINDER_DISPOSE_RIGHT_TAKE);
				}else{
					ACT_push_order(ACT_CYLINDER_DISPOSE_LEFT, ACT_CYLINDER_DISPOSE_LEFT_TAKE);
				}
			}

			if(storage == MODULE_STOCK_RIGHT){
				state = check_act_status(ACT_QUEUE_Cylinder_dispose_right, state, TRIGGER_CYLINDER_PREPARATION, TRIGGER_CYLINDER_PREPARATION);
			}else{
				state = check_act_status(ACT_QUEUE_Cylinder_dispose_left, state, TRIGGER_CYLINDER_PREPARATION, TRIGGER_CYLINDER_PREPARATION);
			}
			break;

		case TRIGGER_CYLINDER_PREPARATION:
			if(storage == MODULE_STOCK_RIGHT){
				if(ELEMENTS_get_flag(FLAG_HARRY_MODULE_COLOR_RIGHT_FINISH)){
					state = TAKE_CYLINDER;
				}else{
					sub_act_harry_mae_prepare_modules_for_dispose(MODULE_STOCK_RIGHT, TRUE); // On lance la pr�paration d'un cylindre : balancer + mise en position pour la bonne couleur
					state = WAIT_CYLINDER_PREPARATION;
				}
			}else{
				if(ELEMENTS_get_flag(FLAG_HARRY_MODULE_COLOR_LEFT_FINISH)){
					state = TAKE_CYLINDER;
				}else{
					sub_act_harry_mae_prepare_modules_for_dispose(MODULE_STOCK_LEFT, TRUE); // On lance la pr�paration d'un cylindre : balancer + mise en position pour la bonne couleur
					state = WAIT_CYLINDER_PREPARATION;
				}
			}
			break;

		case WAIT_CYLINDER_PREPARATION :	// Ajouter un timeout
			// On attend la pr�paration du cylinder (retournement couleur)
			if((storage == MODULE_STOCK_RIGHT && ELEMENTS_get_flag(FLAG_HARRY_MODULE_COLOR_RIGHT_FINISH))
			|| (storage == MODULE_STOCK_LEFT && ELEMENTS_get_flag(FLAG_HARRY_MODULE_COLOR_LEFT_FINISH))){
				state = TAKE_CYLINDER;
			}// TODO G�rer erreur ici
			break;

		case TAKE_CYLINDER:  // On ventouse le cylindre pour le prendre
			if(entrance){
				if(storage == MODULE_STOCK_RIGHT){
					ACT_push_order(ACT_POMPE_DISPOSE_RIGHT, ACT_POMPE_NORMAL);
					ACT_push_order(ACT_CYLINDER_ARM_RIGHT, ACT_CYLINDER_ARM_RIGHT_TAKE);
				}else{
					ACT_push_order(ACT_POMPE_DISPOSE_LEFT, ACT_POMPE_NORMAL);
					ACT_push_order(ACT_CYLINDER_ARM_LEFT, ACT_CYLINDER_ARM_LEFT_TAKE);
				}
			}
			if(storage == MODULE_STOCK_RIGHT){
				state = check_act_status(ACT_QUEUE_Cylinder_arm_right, state, RAISE_CYLINDER, RAISE_CYLINDER);
			}else{
				state = check_act_status(ACT_QUEUE_Cylinder_arm_left, state, RAISE_CYLINDER, RAISE_CYLINDER);
			}
			break;

		case RAISE_CYLINDER:
			if(entrance){
				if(storage == MODULE_STOCK_RIGHT){
					ACT_push_order(ACT_CYLINDER_DISPOSE_RIGHT, ACT_CYLINDER_DISPOSE_RIGHT_RAISE);
				}else{
					ACT_push_order(ACT_CYLINDER_DISPOSE_LEFT, ACT_CYLINDER_DISPOSE_LEFT_RAISE);
				}
			}

			if(storage == MODULE_STOCK_RIGHT){
				state = check_act_status(ACT_QUEUE_Cylinder_dispose_right, state, GET_OUT_CYLINDER_OF_ROBOT, GET_OUT_CYLINDER_OF_ROBOT);
			}else{
				state = check_act_status(ACT_QUEUE_Cylinder_dispose_left, state, GET_OUT_CYLINDER_OF_ROBOT, GET_OUT_CYLINDER_OF_ROBOT);
			}
			break;

		case GET_OUT_CYLINDER_OF_ROBOT:
			if(entrance){
				if(storage == MODULE_STOCK_RIGHT){
					ACT_push_order(ACT_CYLINDER_ARM_RIGHT, ACT_CYLINDER_ARM_RIGHT_OUT);
				}else{
					ACT_push_order(ACT_CYLINDER_ARM_LEFT, ACT_CYLINDER_ARM_LEFT_OUT);
				}
			}

			if(storage == MODULE_STOCK_RIGHT){
				state = check_act_status(ACT_QUEUE_Cylinder_arm_right, state, UNFOLD_DISPOSE_SERVO, UNFOLD_DISPOSE_SERVO);
			}else{
				state = check_act_status(ACT_QUEUE_Cylinder_arm_left, state, UNFOLD_DISPOSE_SERVO, UNFOLD_DISPOSE_SERVO);
			}

			// On exit
			if(ON_LEAVE()){
				STOCKS_makeModuleProgressTo(STOCK_PLACE_COLOR_TO_ARM_DISPOSE, storage);
			}
			break;

		case UNFOLD_DISPOSE_SERVO:
			if(entrance){
				if(storage == MODULE_STOCK_RIGHT){
					ACT_push_order(ACT_CYLINDER_DISPOSE_RIGHT, ACT_CYLINDER_DISPOSE_RIGHT_DISPOSE);
				}else{
					ACT_push_order(ACT_CYLINDER_DISPOSE_LEFT, ACT_CYLINDER_DISPOSE_LEFT_DISPOSE);
				}

				// Une autre d�pose va suivre, on peut d�s � pr�sent lancer la pr�paration du module suivant
				if(anotherDisposeWillFollow){
					sub_act_harry_mae_prepare_modules_for_dispose(storage, TRUE);
				}
			}

			if(storage == MODULE_STOCK_RIGHT){
				state = check_act_status(ACT_QUEUE_Cylinder_dispose_right, state, GO_TO_DISPOSE_POS, GO_TO_DISPOSE_POS);
			}else{
				state = check_act_status(ACT_QUEUE_Cylinder_dispose_left, state, GO_TO_DISPOSE_POS, GO_TO_DISPOSE_POS);
			}
			break;

		case GO_TO_DISPOSE_POS:
			if(entrance){
				time_timeout = global.absolute_time + 400;  // Temporisation avant de d�ventouser
				if(storage == MODULE_STOCK_RIGHT){
					ACT_push_order(ACT_CYLINDER_ARM_RIGHT, ACT_CYLINDER_ARM_RIGHT_DISPOSE);
				}else{
					ACT_push_order(ACT_CYLINDER_ARM_LEFT, ACT_CYLINDER_ARM_LEFT_DISPOSE);
				}
			}

			if(storage == MODULE_STOCK_RIGHT){
				stateAct = check_act_status(ACT_QUEUE_Cylinder_arm_right, IN_PROGRESS, END_OK, NOT_HANDLED);
			}else{
				stateAct = check_act_status(ACT_QUEUE_Cylinder_arm_left, IN_PROGRESS, END_OK, NOT_HANDLED);
			}

			if(stateAct != IN_PROGRESS && global.absolute_time > time_timeout){
				state = DISPOSE_CYLINDER;
			}
			break;

		case DISPOSE_CYLINDER:
			if(entrance){
				time_timeout = global.absolute_time + 700;		// Temporisation pour d�ventouser
				if(storage == MODULE_STOCK_RIGHT){
					ACT_push_order(ACT_POMPE_DISPOSE_RIGHT, ACT_POMPE_STOP);
				}else{
					ACT_push_order(ACT_POMPE_DISPOSE_LEFT, ACT_POMPE_STOP);
				}
			}

			// Pas de v�rification ici car les pompes retournent toujours vrai
			if(global.absolute_time > time_timeout){
				state = CHOOSE_ARM_STORAGE_POS;

				// Mettre � jour les donn�es
				STOCKS_makeModuleProgressTo(STOCK_PLACE_CLEAR_ARM_DISPOSE, storage);
			}
			break;

		case CHOOSE_ARM_STORAGE_POS:
			if(anotherDisposeWillFollow || !STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_ARM_DISPOSE, storage)){
				state = S1_MOVE_DISPOSE_SERVO;	// Sc�narion 1: On doit positionner le bras pour une autre d�pose
			}else{
				state = S2_MOVE_DISPOSE_SERVO;	// Sc�nario 2: on doit ranger le bras dans le robot
			}
			break;

		case S1_MOVE_DISPOSE_SERVO:
			if(entrance){
				if(storage == MODULE_STOCK_RIGHT){
					ACT_push_order(ACT_CYLINDER_DISPOSE_RIGHT, ACT_CYLINDER_DISPOSE_RIGHT_TAKE);
				}else{
					ACT_push_order(ACT_CYLINDER_DISPOSE_LEFT, ACT_CYLINDER_DISPOSE_LEFT_TAKE);
				}
			}

			if(storage == MODULE_STOCK_RIGHT){
				state = check_act_status(ACT_QUEUE_Cylinder_dispose_right, state, S1_MOVE_ARM_SERVO, S1_MOVE_ARM_SERVO);
			}else{
				state = check_act_status(ACT_QUEUE_Cylinder_dispose_left, state, S1_MOVE_ARM_SERVO, S1_MOVE_ARM_SERVO);
			}
			break;

		case S1_MOVE_ARM_SERVO:
			if(entrance){
				if(storage == MODULE_STOCK_RIGHT){
					ACT_push_order(ACT_CYLINDER_ARM_RIGHT, ACT_CYLINDER_ARM_RIGHT_PREPARE_TO_TAKE);
				}else{
					ACT_push_order(ACT_CYLINDER_ARM_LEFT, ACT_CYLINDER_ARM_LEFT_PREPARE_TO_TAKE);
				}
			}
			if(storage == MODULE_STOCK_RIGHT){
				state = check_act_status(ACT_QUEUE_Cylinder_arm_right, state, DONE, DONE);
			}else{
				state = check_act_status(ACT_QUEUE_Cylinder_arm_left, state, DONE, DONE);
			}
			break;

		case S2_MOVE_DISPOSE_SERVO:
			if(entrance){
				if(storage == MODULE_STOCK_RIGHT){
					ACT_push_order(ACT_CYLINDER_DISPOSE_RIGHT, ACT_CYLINDER_DISPOSE_RIGHT_IDLE);
				}else{
					ACT_push_order(ACT_CYLINDER_DISPOSE_LEFT, ACT_CYLINDER_DISPOSE_LEFT_IDLE);
				}
			}

			if(storage == MODULE_STOCK_RIGHT){
				state = check_act_status(ACT_QUEUE_Cylinder_dispose_right, state, S2_MOVE_ARM_SERVO, S2_MOVE_ARM_SERVO);
			}else{
				state = check_act_status(ACT_QUEUE_Cylinder_dispose_left, state, S2_MOVE_ARM_SERVO, S2_MOVE_ARM_SERVO);
			}
			break;

		case S2_MOVE_ARM_SERVO:
			if(entrance){
				if(storage == MODULE_STOCK_RIGHT){
					ACT_push_order(ACT_CYLINDER_ARM_RIGHT, ACT_CYLINDER_ARM_RIGHT_IN);
				}else{
					ACT_push_order(ACT_CYLINDER_ARM_LEFT, ACT_CYLINDER_ARM_LEFT_IN);
				}
			}

			if(storage == MODULE_STOCK_RIGHT){
				state = check_act_status(ACT_QUEUE_Cylinder_arm_right, state, DONE, DONE);
			}else{
				state = check_act_status(ACT_QUEUE_Cylinder_arm_left, state, DONE, DONE);
			}
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
				debug_printf("default case in sub_act_harry_mae_dispose_modules\n");
			break;
	}

	if(storage == MODULE_STOCK_RIGHT){
		stateRight = state;
	}else if(storage == MODULE_STOCK_LEFT){
		stateLeft = state;
	} // else L'erreur a d�j� �t� affich�e

	return ret;
}










































































// Subaction actionneur de prise fus�e v3
error_e sub_act_harry_take_rocket_parallel_down_to_top(moduleRocketLocation_e rocket, ELEMENTS_side_e module_very_down, ELEMENTS_side_e module_down, ELEMENTS_side_e module_top, ELEMENTS_side_e module_very_top){
	CREATE_MAE_WITH_VERBOSE(SM_ID_ACT_HARRY_TAKE_ROCKET_PARALLEL,
			INIT,
			COMPUTE_NEXT_CYLINDER,
			COMPUTE_ACTION,

			AVANCE,
			AVANCE_ERROR,

			ACTION_GO_TAKE_CYLINDER,
			ACTION_GO_TAKE_CYLINDER_2,

			PROTECT_NEXT_FALL,
			ACTION_BRING_BACK_CYLINDER,
			ACTION_BRING_BACK_CYLINDER_2,
			//STOP_POMPE_SLIDER,

			RECULE,
			RECULE_ERROR,
			STORE_CYLINDER,

			ERROR_DISABLE_ACT,
			ERROR,
			DONE
		);

	// Positions du robot
	static GEOMETRY_point_t take_pos; // position du robot lors de la prise
	static GEOMETRY_point_t store_pos; // position du robot lors du stockage

	static ELEMENTS_side_e rocketSide[MAX_MODULE_ROCKET];
	static Uint8 indexSide = 0;
	static ELEMENTS_side_e moduleToTake = NO_SIDE;		// Module en cours de prise
	static bool_e needToStoreRight, needToStoreLeft;
	static error_e state1 = IN_PROGRESS, state2 = IN_PROGRESS;
	static error_e state3 = IN_PROGRESS;
	static moduleType_e moduleType = MODULE_EMPTY;
	static time32_t time_timeout_before_pompe_stop;
	static time32_t time_timeout_after_pompe_stop;
	static bool_e pompe_stop = FALSE;

#ifdef DISPLAY_STOCKS
	if(entrance){
		debug_printf("---------- MODULE_STOCK_LEFT ---------\n");
		STOCKS_print(MODULE_STOCK_LEFT);
		debug_printf("---------- MODULE_STOCK_RIGHT ---------\n");
		STOCKS_print(MODULE_STOCK_RIGHT);
	}
#endif

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
			needToStoreRight = FALSE;
			needToStoreLeft = FALSE;

			// Calcul des positions
			store_pos.x = global.pos.x;
			store_pos.y = global.pos.y;
			take_pos = compute_take_point_rocket(store_pos, global.pos.angle, 50);
			debug_printf("Take pos computed is (%d;%d)\n", take_pos.x, take_pos.y );

			rocketSide[0] = module_very_down;
			rocketSide[1] = module_down;
			rocketSide[2] = module_top;
			rocketSide[3] = module_very_top;
			indexSide = 0;
			state = COMPUTE_NEXT_CYLINDER;
			break;

		case COMPUTE_NEXT_CYLINDER:
			if(ROCKETS_isEmpty(rocket) || indexSide >= 4){
				moduleToTake = NO_SIDE;		// La fus�e est vide, nous avons fini.
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
			indexSide++; // On incr�mente l'index pour le prochain passage
			state = COMPUTE_ACTION;
			break;

		case COMPUTE_ACTION:
			if(!needToStoreLeft && !needToStoreRight && moduleToTake == NO_SIDE){
				state = DONE; // On a fini ou rien n'est possible de faire
			}else if(needToStoreLeft && needToStoreRight){
				state = RECULE; // On doit stocker 2 modules
			}else if(needToStoreLeft &&  moduleToTake == LEFT){
				state = RECULE; // On doit stocker le module gauche
			}else if(needToStoreRight &&  moduleToTake == RIGHT){
				state = RECULE; // On doit stocker le module droit
			}else if((moduleToTake == LEFT || moduleToTake == RIGHT) && (needToStoreLeft || needToStoreRight)){
				state = ACTION_GO_TAKE_CYLINDER; // on doit prendre � partir de la position de prise
			}else if(moduleToTake == LEFT || moduleToTake == RIGHT){
				state = AVANCE; // on doit prendre un cylindre � partir de la position de stockage
			}else{
				state = COMPUTE_NEXT_CYLINDER;
			}
			break;

		case AVANCE:
			state = try_going(take_pos.x, take_pos.y, state, ACTION_GO_TAKE_CYLINDER, AVANCE_ERROR, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case AVANCE_ERROR:
			state = try_going(store_pos.x, store_pos.y, state, AVANCE, AVANCE, FAST, BACKWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case ACTION_GO_TAKE_CYLINDER:
			if (entrance){
				if(moduleToTake == RIGHT){
					ACT_push_order( ACT_CYLINDER_ELEVATOR_RIGHT , ACT_CYLINDER_ELEVATOR_RIGHT_LOCK_WITH_CYLINDER);
				}else{
					ACT_push_order( ACT_CYLINDER_ELEVATOR_LEFT , ACT_CYLINDER_ELEVATOR_LEFT_LOCK_WITH_CYLINDER);
				}
			}
			if(moduleToTake == RIGHT){
				state = check_act_status(ACT_QUEUE_Cylinder_elevator_right, state, ACTION_GO_TAKE_CYLINDER_2, ACTION_GO_TAKE_CYLINDER_2);
			}else{
				state = check_act_status(ACT_QUEUE_Cylinder_elevator_left, state, ACTION_GO_TAKE_CYLINDER_2, ACTION_GO_TAKE_CYLINDER_2);
			}

			break;
		case ACTION_GO_TAKE_CYLINDER_2:
			if(entrance){
				if(moduleToTake == RIGHT){
					//On active la pompe avant d'avancer
					ACT_push_order( ACT_POMPE_SLIDER_RIGHT , ACT_POMPE_NORMAL );
					ACT_push_order( ACT_CYLINDER_SLIDER_RIGHT , ACT_CYLINDER_SLIDER_RIGHT_OUT);

					// Si c'est possible, on avance l'autre bras pour bloquer la chute du cylindre d'apres
					if(!ELEMENTS_get_flag(FLAG_HARRY_DISABLE_MODULE_LEFT) && STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_ENTRY, MODULE_STOCK_LEFT) && !needToStoreLeft){
						ACT_push_order( ACT_CYLINDER_SLIDER_LEFT , ACT_CYLINDER_SLIDER_LEFT_ALMOST_OUT );
					}
				}else{
					//On active la pompe avant d'avancer
					ACT_push_order( ACT_POMPE_SLIDER_LEFT , ACT_POMPE_NORMAL);
					ACT_push_order( ACT_CYLINDER_SLIDER_LEFT , ACT_CYLINDER_SLIDER_LEFT_OUT );

					// Si c'est possible, on avance l'autre bras pour bloquer la chute du cylindre d'apres
					if(!ELEMENTS_get_flag(FLAG_HARRY_DISABLE_MODULE_RIGHT)  && STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_ENTRY, MODULE_STOCK_RIGHT) && !needToStoreRight){
						ACT_push_order( ACT_CYLINDER_SLIDER_RIGHT , ACT_CYLINDER_SLIDER_RIGHT_ALMOST_OUT );
					}
				}

				// On redescent les elevateurs pour le stockage suivant
				if(moduleToTake == RIGHT && STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_ELEVATOR, MODULE_STOCK_RIGHT)){
					ACT_push_order(ACT_CYLINDER_ELEVATOR_LEFT, ACT_CYLINDER_ELEVATOR_LEFT_BOTTOM);
				}else if(moduleToTake == LEFT && STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_ELEVATOR, MODULE_STOCK_LEFT)){
					ACT_push_order(ACT_CYLINDER_ELEVATOR_RIGHT, ACT_CYLINDER_ELEVATOR_RIGHT_BOTTOM);
				}
			}
			/*--------------------------------------------------------------------
			// On decide volontairement de pas checker pour gain de temps

			if(moduleToTake == RIGHT){
				state = check_act_status(ACT_QUEUE_Cylinder_elevator_left, state, ACTION_BRING_BACK_CYLINDER_2_STATES, ERROR);
			}else{
				state = check_act_status(ACT_QUEUE_Cylinder_elevator_right, state, ACTION_BRING_BACK_CYLINDER_2_STATES, ERROR);
			}
			----------------------------------------------------------------------*/

			// V�rification des ordres effectu�s
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
				state = ACTION_BRING_BACK_CYLINDER;
			}
			break;

		case PROTECT_NEXT_FALL: // On retire le cylindre en deux fois pour mieux prot�ger la chute du prochain cylindre
			if(entrance){
				if(moduleToTake == RIGHT){
					ACT_push_order( ACT_CYLINDER_SLIDER_RIGHT , ACT_CYLINDER_SLIDER_RIGHT_ALMOST_OUT_WITH_CYLINDER );
				}else{
					ACT_push_order( ACT_CYLINDER_SLIDER_LEFT , ACT_CYLINDER_SLIDER_LEFT_ALMOST_OUT_WITH_CYLINDER );
				}
			}

			// Pas de gestion d'erreur ici, on continue (check_act_status n�c�ssaire pour attendre la fin de l'action)
			// Cette action n'�tant pas essentielle, on essaie de pousuivre l'action car on veut des points
			if(moduleToTake == RIGHT){
				state = check_act_status(ACT_QUEUE_Cylinder_slider_right, state, ACTION_BRING_BACK_CYLINDER, ACTION_BRING_BACK_CYLINDER);
			}else{
				state = check_act_status(ACT_QUEUE_Cylinder_slider_left, state, ACTION_BRING_BACK_CYLINDER, ACTION_BRING_BACK_CYLINDER);
			}
			break;

		case ACTION_BRING_BACK_CYLINDER:
			if (entrance){
				if(moduleToTake == RIGHT){
					if(STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_ELEVATOR, MODULE_STOCK_RIGHT)){
						ACT_push_order( ACT_CYLINDER_ELEVATOR_RIGHT , ACT_CYLINDER_ELEVATOR_RIGHT_BOTTOM);
					}

				}else{
					if(STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_ELEVATOR, MODULE_STOCK_LEFT)){
						ACT_push_order( ACT_CYLINDER_ELEVATOR_LEFT , ACT_CYLINDER_ELEVATOR_LEFT_BOTTOM);
					}
				}
			}
			if(moduleToTake == RIGHT && STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_ELEVATOR, MODULE_STOCK_RIGHT)){
				state = check_act_status(ACT_QUEUE_Cylinder_elevator_right, state, ACTION_BRING_BACK_CYLINDER_2, ERROR);
			}
			else if(moduleToTake == LEFT && STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_ELEVATOR, MODULE_STOCK_LEFT)){
				state = check_act_status(ACT_QUEUE_Cylinder_elevator_left, state, ACTION_BRING_BACK_CYLINDER_2, ERROR);
			}
			else {
				state = ACTION_BRING_BACK_CYLINDER_2;
			}
			break;

		case ACTION_BRING_BACK_CYLINDER_2:
			if(entrance){
				if(moduleToTake == RIGHT){
					// Activation de la pompe de l'�l�vateur si on stocke le cylindre apr�s
					if(STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_ELEVATOR, MODULE_STOCK_RIGHT)){
						ACT_push_order( ACT_POMPE_ELEVATOR_RIGHT, ACT_POMPE_NORMAL );
					}
					//On rentre le bras dans le robot
					ACT_push_order( ACT_CYLINDER_SLIDER_RIGHT, ACT_CYLINDER_SLIDER_RIGHT_IN );
				}else{
					// Activation de la pompe de l'�l�vateur si on stocke le cylindre apr�s
					if(STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_ELEVATOR, MODULE_STOCK_LEFT)){
						ACT_push_order( ACT_POMPE_ELEVATOR_LEFT, ACT_POMPE_NORMAL );
					}
					//On rentre le bras dans le robot
					ACT_push_order( ACT_CYLINDER_SLIDER_LEFT, ACT_CYLINDER_SLIDER_LEFT_IN );
				}

				// On redescent les slopes pour le stockage suivant (Why not)
				if(STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_SLOPE, MODULE_STOCK_RIGHT)){
					ACT_push_order(ACT_CYLINDER_SLOPE_RIGHT, ACT_CYLINDER_SLOPE_RIGHT_DOWN);
				}
				if(STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_SLOPE, MODULE_STOCK_LEFT)){
					ACT_push_order(ACT_CYLINDER_SLOPE_LEFT, ACT_CYLINDER_SLOPE_LEFT_DOWN);
				}
			}

			// V�rification des ordres effectu�s
			if(moduleToTake == RIGHT){
				state1 = check_act_status(ACT_QUEUE_Cylinder_slider_right, IN_PROGRESS, END_OK, NOT_HANDLED);
			}else{
				state1 = check_act_status(ACT_QUEUE_Cylinder_slider_left, IN_PROGRESS, END_OK, NOT_HANDLED);
			}

			if(state1 != IN_PROGRESS){

				if((moduleToTake == RIGHT && STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_ELEVATOR, MODULE_STOCK_RIGHT))
				|| (moduleToTake == LEFT && STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_ELEVATOR, MODULE_STOCK_LEFT))){
					state = COMPUTE_ACTION;	// Si l'�l�vateur n'est pas occup�, on continue le stockage
				}else if((moduleToTake == RIGHT && !STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_ELEVATOR, MODULE_STOCK_RIGHT))
				|| (moduleToTake == LEFT && !STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_ELEVATOR, MODULE_STOCK_LEFT))){
					state = COMPUTE_ACTION;		// Si l'�l�vateur est occup�, on va prendre le cylindre suivant
				}

				// On met � jour les donn�es : Passage du module dans la position POS_ENTRY
				if(moduleToTake == RIGHT){
					needToStoreRight = TRUE;
					STOCKS_addModule(moduleType, STOCK_POS_ENTRY, MODULE_STOCK_RIGHT);
				}else{
					needToStoreLeft = TRUE;
					STOCKS_addModule(moduleType, STOCK_POS_ENTRY, MODULE_STOCK_LEFT);
				}

				ROCKETS_removeModule(rocket); // Suppression du module de la fus�e

				if(state == COMPUTE_ACTION){ // Tr�s important
					moduleToTake = NO_SIDE;
				}
			}
			break;

		/*case STOP_POMPE_SLIDER:
			if(entrance){
				time_timeout_before_pompe_stop = global.absolute_time + 1000;
				time_timeout_after_pompe_stop = global.absolute_time + 2000;
				pompe_stop =  FALSE;
			}

			if(global.absolute_time > time_timeout_before_pompe_stop && pompe_stop == FALSE){
				if(moduleToTake == RIGHT){
					ACT_push_order( ACT_POMPE_SLIDER_RIGHT, ACT_POMPE_STOP );
				}else{
					ACT_push_order( ACT_POMPE_SLIDER_LEFT, ACT_POMPE_STOP );
				}
				pompe_stop=TRUE;
			}

			// On attend une seconde le temps que le ventousage se fasse bien
			if(global.absolute_time > time_timeout_after_pompe_stop){
				pompe_stop=FALSE;
				state = COMPUTE_ACTION;

				// On met � jour les donn�es : Passage du module de POS_ENTRY � POS_ELEVATOR
				if(moduleToTake == RIGHT){
					STOCKS_makeModuleProgressTo(STOCK_PLACE_ENTRY_TO_ELEVATOR, MODULE_STOCK_RIGHT);
				}else{
					STOCKS_makeModuleProgressTo(STOCK_PLACE_ENTRY_TO_ELEVATOR, MODULE_STOCK_LEFT);
				}
				moduleToTake = NO_SIDE;
			}
			break;*/

		case RECULE:
			if(entrance){
				if(needToStoreLeft){
					ACT_push_order( ACT_POMPE_SLIDER_LEFT, ACT_POMPE_STOP );
				}
				if(needToStoreRight){
					ACT_push_order( ACT_POMPE_SLIDER_RIGHT, ACT_POMPE_STOP );
				}
			}
			state = try_going(store_pos.x, store_pos.y, state, STORE_CYLINDER, RECULE_ERROR, FAST, BACKWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case RECULE_ERROR:
			state= try_going(take_pos.x, take_pos.y, state, RECULE, RECULE, FAST, FORWARD, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case STORE_CYLINDER:
			if(entrance){
				if(needToStoreLeft){
					STOCKS_makeModuleProgressTo(STOCK_PLACE_ENTRY_TO_ELEVATOR, MODULE_STOCK_LEFT);
					sub_act_harry_mae_store_modules(MODULE_STOCK_LEFT, TRUE);
				}
				if(needToStoreRight){
					STOCKS_makeModuleProgressTo(STOCK_PLACE_ENTRY_TO_ELEVATOR, MODULE_STOCK_RIGHT);
					sub_act_harry_mae_store_modules(MODULE_STOCK_RIGHT, TRUE);
				}
			}

			if(indexSide >= 4){
				state = DONE; // pas besoin d'attendre
				needToStoreLeft = FALSE;
				needToStoreRight = FALSE;
			}else if((needToStoreLeft && ELEMENTS_get_flag(FLAG_HARRY_STORAGE_LEFT_FINISH)
			&& needToStoreRight && ELEMENTS_get_flag(FLAG_HARRY_STORAGE_RIGHT_FINISH))
			|| (needToStoreLeft && !needToStoreRight && ELEMENTS_get_flag(FLAG_HARRY_STORAGE_LEFT_FINISH))
			|| (!needToStoreLeft && needToStoreRight && ELEMENTS_get_flag(FLAG_HARRY_STORAGE_RIGHT_FINISH))
			){
				state = COMPUTE_NEXT_CYLINDER;
				needToStoreLeft = FALSE;
				needToStoreRight = FALSE;
			}
			break;

		case ERROR_DISABLE_ACT:
			state = COMPUTE_ACTION;
			break;

		case DONE:
			if(entrance){
				// On active la d�pose
				set_sub_act_enable(SUB_HARRY_DEPOSE_MODULES, TRUE);
				// On range les sliders
				ACT_push_order(ACT_CYLINDER_SLIDER_RIGHT, ACT_CYLINDER_SLIDER_RIGHT_IN);
				ACT_push_order(ACT_CYLINDER_SLIDER_LEFT, ACT_CYLINDER_SLIDER_LEFT_IN);
			}
			RESET_MAE();
			on_turning_point();
			return END_OK;
			break;

		case ERROR:
			if(entrance){
				// On range les sliders
				ACT_push_order(ACT_CYLINDER_SLIDER_RIGHT, ACT_CYLINDER_SLIDER_RIGHT_IN);
				ACT_push_order(ACT_CYLINDER_SLIDER_LEFT, ACT_CYLINDER_SLIDER_LEFT_IN);
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
