#include "action_big.h"
#include "../../propulsion/movement.h"
#include "../../propulsion/astar.h"
#include "../../QS/QS_stateMachineHelper.h"
#include "../../QS/QS_types.h"
#include "../../QS/QS_outputlog.h"
#include "../../QS/QS_IHM.h"
#include "../../QS/QS_CapteurCouleurCW.h"
#include  "../../propulsion/astar.h"
#include  "../../utils/generic_functions.h"
#include "../../actuator/act_functions.h"
#include "../../actuator/queue.h"
#include "../../utils/actionChecker.h"
#include "../../elements.h"
#include "../../high_level_strat.h"

error_e init_all_actionneur(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_INIT_CYLINDER,
			INIT,
			INIT_ACTION_SLIDER_LEFT,
			INIT_ACTION_SLIDER_RIGHT,
			INIT_ACTION_ELEVATOR_LEFT,
			INIT_ACTION_ELEVATOR_RIGHT,
			INIT_ACTION_SLOPE_LEFT,
			INIT_ACTION_SLOPE_RIGHT,
			ERROR,
			DONE
		);

	//l'initalisation des pompes est-elle necesaire ?

	switch(state){
		case INIT:
			state = INIT_ACTION_SLIDER_RIGHT;
			break;

		case INIT_ACTION_SLIDER_RIGHT:
			if(entrance){
				/*
				if (STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_ENTRY , MODULE_STOCK_RIGHT)){
					ACT_push_order( ACT_POMPE_SLIDER_RIGHT , ACT_POMPE_STOP );
				}*/
				ACT_push_order(ACT_CYLINDER_SLIDER_RIGHT, ACT_CYLINDER_SLIDER_RIGHT_IN);
			}
			if (STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_ELEVATOR , MODULE_STOCK_RIGHT)){
				state= check_act_status(ACT_QUEUE_Cylinder_slider_right, INIT_ACTION_SLIDER_RIGHT, INIT_ACTION_ELEVATOR_RIGHT, ERROR);
			}else{
				state= check_act_status(ACT_QUEUE_Cylinder_slider_right, INIT_ACTION_SLIDER_RIGHT, INIT_ACTION_SLIDER_LEFT, ERROR);
			}
			break;

		case INIT_ACTION_ELEVATOR_RIGHT:
			if(entrance){
				/*
				if (STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_ELEVATOR , MODULE_STOCK_LEFT)){
					ACT_push_order( ACT_POMPE_ELEVATOR_RIGHT , ACT_POMPE_STOP );
				}*/
				ACT_push_order(ACT_CYLINDER_ELEVATOR_RIGHT, ACT_CYLINDER_ELEVATOR_RIGHT_BOTTOM);
			}
			if (STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_4_TO_OUT , MODULE_STOCK_RIGHT)){
				state= check_act_status(ACT_QUEUE_Cylinder_elevator_right, INIT_ACTION_ELEVATOR_RIGHT, INIT_ACTION_SLOPE_RIGHT, ERROR);
			}else{
				state= check_act_status(ACT_QUEUE_Cylinder_elevator_right, INIT_ACTION_ELEVATOR_RIGHT, INIT_ACTION_SLIDER_LEFT, ERROR);
			}
			break;

		case INIT_ACTION_SLOPE_RIGHT:
			if(entrance){
				ACT_push_order(ACT_CYLINDER_SLOPE_RIGHT, ACT_CYLINDER_SLOPE_RIGHT_DOWN);
			}
			state= check_act_status(ACT_QUEUE_Cylinder_slope_right, INIT_ACTION_SLOPE_RIGHT, INIT_ACTION_SLIDER_LEFT, ERROR);
			break;

		case INIT_ACTION_SLIDER_LEFT:
			if(entrance){
				/*
				if (STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_ENTRY , MODULE_STOCK_LEFT)){
					ACT_push_order( ACT_POMPE_SLIDER_LEFT , ACT_POMPE_STOP );
				}*/
				ACT_push_order(ACT_CYLINDER_SLIDER_LEFT, ACT_CYLINDER_SLIDER_LEFT_IN);}
			if (STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_ELEVATOR , MODULE_STOCK_LEFT)){
				state= check_act_status(ACT_QUEUE_Cylinder_slider_left, INIT_ACTION_SLIDER_LEFT, INIT_ACTION_ELEVATOR_LEFT, ERROR);
			}else{
				state= check_act_status(ACT_QUEUE_Cylinder_slider_left, INIT_ACTION_SLIDER_LEFT, DONE, ERROR);
			}
			break;

		case INIT_ACTION_ELEVATOR_LEFT:
			if(entrance){
				/*
				if (STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_ELEVATOR , MODULE_STOCK_LEFT)){
					ACT_push_order( ACT_POMPE_ELEVATOR_LEFT, ACT_POMPE_STOP );
				}*/
				ACT_push_order(ACT_CYLINDER_ELEVATOR_LEFT, ACT_CYLINDER_ELEVATOR_LEFT_BOTTOM);
			}
			if (STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_4_TO_OUT , MODULE_STOCK_LEFT)){
				state= check_act_status(ACT_QUEUE_Cylinder_elevator_left, INIT_ACTION_ELEVATOR_LEFT, INIT_ACTION_SLOPE_LEFT, ERROR);
			}else{
				state= check_act_status(ACT_QUEUE_Cylinder_elevator_left, INIT_ACTION_ELEVATOR_LEFT, DONE, ERROR);
			}
			break;

		case INIT_ACTION_SLOPE_LEFT :
			if(entrance){
				ACT_push_order(ACT_CYLINDER_SLOPE_LEFT, ACT_CYLINDER_SLOPE_LEFT_DOWN);
			}
			state = check_act_status(ACT_QUEUE_Cylinder_slope_left, INIT_ACTION_SLOPE_LEFT, DONE, ERROR);
			break;

		case DONE:
			RESET_MAE();
			return END_OK;
			break;

		case ERROR:
			RESET_MAE();
			return NOT_HANDLED;
			break;

		default:
			if(entrance)
				debug_printf("default case in init_all_actionneur\n");
			break;

	}

	return IN_PROGRESS;
}


// Subaction actionneur de prise fusée v1
error_e boucle_charge_module(moduleStockLocation_e nb_cylinder_big_right,moduleStockLocation_e nb_cylinder_big_left,moduleRocketLocation_e nb_cylinder_fusee, moduleType_e myModule,  bool_e right_side ){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_INIT_CYLINDER,
			INIT,
			ACTION_GO_TAKE_CYLINDER,
			CHECK_STATUS_OTHER_SLINDER,
			PROTECT_NEXT_FALL,
			ACTION_BRING_BACK_CYLINDER,
			CHECK_STATUS_OTHER_SLINDER_2,
			CHECK_STATUS_ELEVATOR,
			ACTION_START_BRING_UP_CYLINDER,
			CHECK_STATUS_SLOPE,
			ACTION_END_BRING_UP_CYLINDER,
			CHECK_STATUS_SLIDER,
			ACTION_STOCK_UP_CYLINDER,
			CHECK_STATUS_OTHER_SLINDER_3,
			RESTART_CONDITION,
			ACTION_RETRACT_ALL,
			ACTION_FINAL_1_ON_3,
			ACTION_FINAL_2_ON_3,
			ACTION_FINAL_3_ON_3,
			CHECK_STATUS_OTHER_CYLINDER_FINAL,
			ERROR,
			DONE
		);

	switch(state){
		case INIT:
			state = ACTION_GO_TAKE_CYLINDER;
			break;

		case ACTION_GO_TAKE_CYLINDER:

			if (right_side){
				if (entrance) {
				//On active la pompe avant d'avancer
				ACT_push_order( ACT_POMPE_SLIDER_RIGHT , ACT_POMPE_NORMAL );
				ACT_push_order( ACT_CYLINDER_SLIDER_RIGHT , ACT_CYLINDER_SLIDER_RIGHT_OUT );

				// On avance l'autre bras pour bloquer la chute du cylindre d'apres
				ACT_push_order( ACT_CYLINDER_SLIDER_LEFT , ACT_CYLINDER_SLIDER_LEFT_ALMOST_OUT );}

				state = check_act_status(ACT_QUEUE_Cylinder_slider_right, ACTION_GO_TAKE_CYLINDER,CHECK_STATUS_OTHER_SLINDER, ERROR);
			}else{
				if (entrance) {
				ACT_push_order( ACT_POMPE_SLIDER_LEFT , ACT_POMPE_NORMAL);
				ACT_push_order( ACT_CYLINDER_SLIDER_LEFT , ACT_CYLINDER_SLIDER_LEFT_OUT );

				ACT_push_order( ACT_CYLINDER_SLIDER_RIGHT , ACT_CYLINDER_SLIDER_RIGHT_ALMOST_OUT );}

				state = check_act_status(ACT_QUEUE_Cylinder_slider_left, ACTION_GO_TAKE_CYLINDER,CHECK_STATUS_OTHER_SLINDER, ERROR);
			}
			break;

		case CHECK_STATUS_OTHER_SLINDER:
			if (right_side){
				state = check_act_status(ACT_QUEUE_Cylinder_slider_left, CHECK_STATUS_OTHER_SLINDER,PROTECT_NEXT_FALL, ERROR);
			}else{
				state = check_act_status(ACT_QUEUE_Cylinder_slider_right, CHECK_STATUS_OTHER_SLINDER,PROTECT_NEXT_FALL, ERROR);
			}
			break;

		case PROTECT_NEXT_FALL:
			if (right_side){
				if(entrance){
				ACT_push_order( ACT_CYLINDER_SLIDER_RIGHT , ACT_CYLINDER_SLIDER_RIGHT_ALMOST_OUT_WITH_CYLINDER);}
				state = check_act_status(ACT_QUEUE_Cylinder_slider_right, PROTECT_NEXT_FALL,ACTION_BRING_BACK_CYLINDER, ERROR);
			}else{
				if(entrance){
				ACT_push_order( ACT_CYLINDER_SLIDER_LEFT , ACT_CYLINDER_SLIDER_LEFT_ALMOST_OUT_WITH_CYLINDER);}
				state = check_act_status(ACT_QUEUE_Cylinder_slider_left, PROTECT_NEXT_FALL,ACTION_BRING_BACK_CYLINDER, ERROR);
			}
			break;

		case ACTION_BRING_BACK_CYLINDER:
			if (right_side){
				if (entrance) {
				ACT_push_order( ACT_CYLINDER_SLIDER_RIGHT , ACT_CYLINDER_SLIDER_RIGHT_IN );

				//On avance en profite pour bouger de l'autre cote
				ACT_push_order( ACT_POMPE_SLIDER_LEFT , ACT_POMPE_NORMAL );
				//vacuose ici?
				ACT_push_order( ACT_CYLINDER_SLIDER_LEFT , ACT_CYLINDER_SLIDER_LEFT_OUT );

				//On redescend le bras et desactive la pompe du cycle precedent si pas utilise par le stockage
				// attention a 4 on est quand meme en haut, il faut descendre !
				if ((STOCKS_getNbModules(nb_cylinder_big_left)<5)){
					ACT_push_order( ACT_POMPE_ELEVATOR_LEFT , ACT_POMPE_STOP );
					//vacuose ici?!
					ACT_push_order( ACT_CYLINDER_ELEVATOR_LEFT , ACT_CYLINDER_ELEVATOR_LEFT_BOTTOM );}}

				//capteur inuctif fin de course a la place du check at status ?
				state = check_act_status(ACT_QUEUE_Cylinder_slider_right, ACTION_BRING_BACK_CYLINDER,CHECK_STATUS_OTHER_SLINDER_2, ERROR);
			}else{
				if (entrance) {
				ACT_push_order( ACT_CYLINDER_SLIDER_LEFT , ACT_CYLINDER_SLIDER_LEFT_IN );

				ACT_push_order( ACT_POMPE_SLIDER_RIGHT , ACT_POMPE_NORMAL );
				//vacuose ici?
				ACT_push_order( ACT_CYLINDER_SLIDER_RIGHT , ACT_CYLINDER_SLIDER_RIGHT_OUT );

				if ((STOCKS_getNbModules(nb_cylinder_big_right)<5)){
				ACT_push_order( ACT_POMPE_ELEVATOR_RIGHT , ACT_POMPE_STOP );
				//vacuose ici?
				ACT_push_order( ACT_CYLINDER_ELEVATOR_RIGHT , ACT_CYLINDER_ELEVATOR_RIGHT_BOTTOM );}}

				//capteur inuctif fin de course a la place du check at status ?
				state = check_act_status(ACT_QUEUE_Cylinder_slider_left, ACTION_BRING_BACK_CYLINDER,CHECK_STATUS_OTHER_SLINDER_2, ERROR);
			}break;

		case CHECK_STATUS_OTHER_SLINDER_2:
			if (right_side){
				//pas besoin de verifier l'elevator si il a pas ete actionne
				if ((STOCKS_getNbModules(nb_cylinder_big_left)<5)){
					state = check_act_status(ACT_QUEUE_Cylinder_slider_left, CHECK_STATUS_OTHER_SLINDER_2,CHECK_STATUS_ELEVATOR, ERROR);
				}else{
					state = check_act_status(ACT_QUEUE_Cylinder_slider_left, CHECK_STATUS_OTHER_SLINDER_2,ACTION_START_BRING_UP_CYLINDER, ERROR);}
			}else{
				if ((STOCKS_getNbModules(nb_cylinder_big_right)<5)){
					state = check_act_status(ACT_QUEUE_Cylinder_slider_right, CHECK_STATUS_OTHER_SLINDER_2,CHECK_STATUS_ELEVATOR, ERROR);
				}else{
					state = check_act_status(ACT_QUEUE_Cylinder_slider_right, CHECK_STATUS_OTHER_SLINDER_2,ACTION_START_BRING_UP_CYLINDER, ERROR);}
			}
			break;

		case CHECK_STATUS_ELEVATOR:
			if (right_side){
				state = check_act_status(ACT_QUEUE_Cylinder_elevator_left, CHECK_STATUS_ELEVATOR,ACTION_START_BRING_UP_CYLINDER, ERROR);
			}else{
				state = check_act_status(ACT_QUEUE_Cylinder_elevator_right, CHECK_STATUS_ELEVATOR,ACTION_START_BRING_UP_CYLINDER, ERROR);
			}
			break;

		case ACTION_START_BRING_UP_CYLINDER:

			if (right_side){
				if (entrance) {
				ACT_push_order( ACT_POMPE_ELEVATOR_RIGHT , ACT_POMPE_NORMAL );
				ACT_push_order( ACT_POMPE_SLIDER_RIGHT , ACT_POMPE_STOP );
				// ici rajouter le check de la pompe avec vacuose pour les deux
				ACT_push_order( ACT_CYLINDER_ELEVATOR_RIGHT , ACT_CYLINDER_ELEVATOR_RIGHT_LOCK_WITH_CYLINDER );

				// on redescend en meme temps le clapet du cylindre precedent si il n'est pas bloque
				if ((STOCKS_getNbModules(nb_cylinder_big_left)<4)){
					ACT_push_order( ACT_CYLINDER_SLOPE_LEFT , ACT_CYLINDER_SLOPE_LEFT_DOWN );}}

				//la on doit actionner le slider du cote de l'elevator pour qu'il parte en ALMOST_OUT
				//le plus efficace serait-il d'utiliser Wait() plutot que de creer un nouveau case pour separer la monter en deux ? !!!
				//Attention le wait peut etre dangereux, mechaniquement, si le slider avance trop tot
				state = check_act_status(ACT_QUEUE_Cylinder_elevator_right, ACTION_START_BRING_UP_CYLINDER,CHECK_STATUS_SLOPE, ERROR);
			}else{
				if (entrance) {
				ACT_push_order( ACT_POMPE_ELEVATOR_LEFT , ACT_POMPE_NORMAL );
				ACT_push_order( ACT_POMPE_SLIDER_LEFT , ACT_POMPE_STOP );
				// vacuose
				ACT_push_order( ACT_CYLINDER_ELEVATOR_LEFT , ACT_CYLINDER_ELEVATOR_LEFT_LOCK_WITH_CYLINDER );
				if ((STOCKS_getNbModules(nb_cylinder_big_right)<4)){ACT_push_order( ACT_CYLINDER_SLOPE_RIGHT , ACT_CYLINDER_SLOPE_RIGHT_DOWN );}}
				state = check_act_status(ACT_QUEUE_Cylinder_elevator_left, ACTION_START_BRING_UP_CYLINDER,CHECK_STATUS_SLOPE, ERROR);
			}break;

		case CHECK_STATUS_SLOPE:
			//necessaire de le passer si non utilise?
			if (right_side){
				state = check_act_status(ACT_QUEUE_Cylinder_slope_left, CHECK_STATUS_SLOPE,ACTION_END_BRING_UP_CYLINDER, ERROR);
			}else{
				state = check_act_status(ACT_QUEUE_Cylinder_slope_right, CHECK_STATUS_SLOPE,ACTION_END_BRING_UP_CYLINDER, ERROR);
			}
			break;

		case ACTION_END_BRING_UP_CYLINDER:

			if (right_side){
				if (entrance) {
				//on finit le mouvement en simultanee avec la sortie du slider
				ACT_push_order( ACT_CYLINDER_SLIDER_RIGHT , ACT_CYLINDER_SLIDER_RIGHT_ALMOST_OUT );
				if ((STOCKS_getNbModules(nb_cylinder_big_right)<4)){
				ACT_push_order( ACT_CYLINDER_ELEVATOR_RIGHT , ACT_CYLINDER_ELEVATOR_RIGHT_TOP );}}
				state = check_act_status(ACT_QUEUE_Cylinder_elevator_right, ACTION_END_BRING_UP_CYLINDER,CHECK_STATUS_SLIDER, ERROR);
			}else{
				if (entrance) {
				ACT_push_order( ACT_CYLINDER_SLIDER_LEFT , ACT_CYLINDER_SLIDER_LEFT_ALMOST_OUT );
				if ((STOCKS_getNbModules(nb_cylinder_big_left)<4)){ACT_push_order( ACT_CYLINDER_ELEVATOR_LEFT , ACT_CYLINDER_ELEVATOR_LEFT_TOP );}}
				state = check_act_status(ACT_QUEUE_Cylinder_elevator_left, ACTION_END_BRING_UP_CYLINDER,CHECK_STATUS_SLIDER, ERROR);
			}break;

		case CHECK_STATUS_SLIDER:
			if (right_side){
				state = check_act_status(ACT_QUEUE_Cylinder_slider_right, CHECK_STATUS_SLIDER,ACTION_STOCK_UP_CYLINDER, ERROR);
			}else{
				state = check_act_status(ACT_QUEUE_Cylinder_slider_left, CHECK_STATUS_SLIDER,ACTION_STOCK_UP_CYLINDER, ERROR);
			}
			break;

		case ACTION_STOCK_UP_CYLINDER:

			if (right_side){
				if (entrance) {
				// on verouille le slope
				ACT_push_order( ACT_CYLINDER_SLOPE_RIGHT , ACT_CYLINDER_SLOPE_RIGHT_UP );

				//et en meme temps on prepare la collecte d'apres
				ACT_push_order( ACT_CYLINDER_SLIDER_LEFT , ACT_CYLINDER_SLIDER_LEFT_ALMOST_OUT_WITH_CYLINDER );}

				state = check_act_status(ACT_QUEUE_Cylinder_slope_right, ACTION_STOCK_UP_CYLINDER, CHECK_STATUS_OTHER_SLINDER_3, ERROR);

			}else{
				if (entrance) {
				ACT_push_order( ACT_CYLINDER_SLOPE_LEFT , ACT_CYLINDER_SLOPE_LEFT_UP );
				ACT_push_order( ACT_CYLINDER_SLIDER_RIGHT , ACT_CYLINDER_SLIDER_RIGHT_ALMOST_OUT_WITH_CYLINDER );}
				state = check_act_status(ACT_QUEUE_Cylinder_slope_right, ACTION_STOCK_UP_CYLINDER, CHECK_STATUS_OTHER_SLINDER_3, ERROR);}
			break;

		case CHECK_STATUS_OTHER_SLINDER_3:
			if (right_side){
				state = check_act_status(ACT_QUEUE_Cylinder_slider_left, CHECK_STATUS_OTHER_SLINDER_3,RESTART_CONDITION, ERROR);
			}else{
				state = check_act_status(ACT_QUEUE_Cylinder_slider_right, CHECK_STATUS_OTHER_SLINDER_3,RESTART_CONDITION, ERROR);
			}
			break;

		case RESTART_CONDITION:
			ROCKETS_removeModule(nb_cylinder_fusee);
			if ((ROCKETS_getNbModules(nb_cylinder_fusee)>0)){
				if (right_side){
					//on actualise le nombre de cylindre stocke dans le robot
					STOCKS_addModule(MODULE_POLY,nb_cylinder_big_right);
					//On change de cote
					right_side = FALSE;
					if ((STOCKS_getNbModules(nb_cylinder_big_right)<5)){
						//et on recommence
						state = ACTION_BRING_BACK_CYLINDER;
					}else{
						state = ACTION_FINAL_1_ON_3;
					}
				}else{
					STOCKS_addModule(MODULE_POLY,nb_cylinder_big_left);
					right_side = TRUE;
					if ((STOCKS_getNbModules(nb_cylinder_big_left)<5)){
					state = ACTION_BRING_BACK_CYLINDER;
					}else{state = ACTION_FINAL_1_ON_3;}}
			}else{
				state=DONE;}
				break;

		case ACTION_FINAL_1_ON_3:
			if (right_side){
				if (entrance) {
				ACT_push_order( ACT_POMPE_SLIDER_RIGHT , ACT_POMPE_NORMAL );
				ACT_push_order( ACT_CYLINDER_SLIDER_RIGHT , ACT_CYLINDER_SLIDER_RIGHT_OUT );}
				state = check_act_status(ACT_QUEUE_Cylinder_slider_right, ACTION_FINAL_1_ON_3, ACTION_FINAL_2_ON_3, ERROR);

			}else{
				if (entrance) {
				ACT_push_order( ACT_POMPE_SLIDER_LEFT , ACT_POMPE_NORMAL );
				ACT_push_order( ACT_CYLINDER_SLIDER_LEFT , ACT_CYLINDER_SLIDER_LEFT_OUT );}
				state = check_act_status(ACT_QUEUE_Cylinder_slider_left, ACTION_FINAL_1_ON_3, ACTION_FINAL_2_ON_3, ERROR);}
			break;

		case ACTION_FINAL_2_ON_3:
			//si on arrive ici car notre robot est plein mais il reste des cylindres dans la fusee
			//on doit quand meme prevoir la chute d'un cylindre
			if (right_side){
				if (entrance) {
				ACT_push_order( ACT_CYLINDER_SLIDER_RIGHT , ACT_CYLINDER_SLIDER_RIGHT_ALMOST_OUT_WITH_CYLINDER );}
				state = check_act_status(ACT_QUEUE_Cylinder_slope_right, ACTION_FINAL_2_ON_3, ACTION_FINAL_3_ON_3, ERROR);

			}else{
				if (entrance) {
				ACT_push_order( ACT_CYLINDER_SLIDER_LEFT , ACT_CYLINDER_SLIDER_LEFT_ALMOST_OUT_WITH_CYLINDER );}
				state = check_act_status(ACT_QUEUE_Cylinder_slope_left, ACTION_FINAL_2_ON_3, ACTION_FINAL_3_ON_3, ERROR);}
			break;

		case ACTION_FINAL_3_ON_3:
			if (entrance) {
			ACT_push_order( ACT_CYLINDER_SLIDER_LEFT , ACT_CYLINDER_SLIDER_LEFT_IN );
			ACT_push_order( ACT_CYLINDER_SLIDER_RIGHT , ACT_CYLINDER_SLIDER_RIGHT_IN );}
			state = check_act_status(ACT_QUEUE_Cylinder_slope_right, ACTION_FINAL_3_ON_3, CHECK_STATUS_OTHER_CYLINDER_FINAL, ERROR);
			break;

		case CHECK_STATUS_OTHER_CYLINDER_FINAL :
			state = check_act_status(ACT_QUEUE_Cylinder_slope_left, CHECK_STATUS_OTHER_CYLINDER_FINAL, DONE, ERROR);
			break;

		case DONE:
			if(entrance){
				set_sub_act_enable(SUB_HARRY_DEPOSE_MODULES, TRUE);
			}
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
				debug_printf("default case in sub_harry_rocket_multicolor\n");
			break;
	}

	return IN_PROGRESS;
}








// Subaction actionneur de prise fusée v2
error_e sub_act_harry_take_rocket_down_to_top(moduleRocketLocation_e rocket, ELEMENTS_side_e module_very_down, ELEMENTS_side_e module_down, ELEMENTS_side_e module_top, ELEMENTS_side_e module_very_top){
	CREATE_MAE_WITH_VERBOSE(SM_ID_ACT_HARRY_TAKE_ROCKET,
			INIT,
			COMPUTE_NEXT_CYLINDER,
			COMPUTE_ACTION,

			ACTION_GO_TAKE_CYLINDER,
			ERROR_ACTION_GO_TAKE_CYLINDER,
			ERROR_ACTION_GO_TAKE_CYLINDER_RETRY,
			NO_CYLINDER_DETECTED,

			PROTECT_NEXT_FALL,
			ACTION_BRING_BACK_CYLINDER,
			ACTION_PREPARE_ELEVATOR,
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
				moduleToTake = NO_SIDE; 	// On ne doit pas prendre les modules suivants (ceci est un choix de l'utilisateur
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
			if(entrance){
				if(moduleToTake == RIGHT){
					//On active la pompe avant d'avancer
					ACT_push_order( ACT_POMPE_SLIDER_RIGHT , ACT_POMPE_NORMAL );
					ACT_push_order( ACT_CYLINDER_SLIDER_RIGHT , ACT_CYLINDER_SLIDER_RIGHT_OUT);

					// Si c'est possible, on avance l'autre bras pour bloquer la chute du cylindre d'apres
					if(!ELEMENTS_get_flag(FLAG_HARRY_DISABLE_MODULE_LEFT) /*&& on a pas de cylindre devant*/){
						ACT_push_order( ACT_CYLINDER_SLIDER_LEFT , ACT_CYLINDER_SLIDER_LEFT_ALMOST_OUT );
					}
				}else{
					//On active la pompe avant d'avancer
					ACT_push_order( ACT_POMPE_SLIDER_LEFT , ACT_POMPE_NORMAL);
					ACT_push_order( ACT_CYLINDER_SLIDER_LEFT , ACT_CYLINDER_SLIDER_LEFT_OUT );

					// Si c'est possible, on avance l'autre bras pour bloquer la chute du cylindre d'apres
					if(!ELEMENTS_get_flag(FLAG_HARRY_DISABLE_MODULE_RIGHT)  /*&& on a pas de cylindre devant*/){
						ACT_push_order( ACT_CYLINDER_SLIDER_RIGHT , ACT_CYLINDER_SLIDER_RIGHT_ALMOST_OUT );
					}
				}

				// On finit de stocker le cylindre d'avant (Si moduleToStore == NO_SIDE, il n'y a rien a faire)
				if(moduleToStore == RIGHT){
					ACT_push_order(ACT_CYLINDER_ELEVATOR_RIGHT, ACT_CYLINDER_ELEVATOR_RIGHT_BOTTOM);
				}else if(moduleToStore == LEFT){
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
				if(/*state1 == END_OK &&*/ state2 == END_OK /*&& state3 == END_OK*/){
					if((moduleToTake == RIGHT && !ELEMENTS_get_flag(FLAG_HARRY_DISABLE_MODULE_LEFT)) || (moduleToTake == LEFT && !ELEMENTS_get_flag(FLAG_HARRY_DISABLE_MODULE_RIGHT))){
						state = PROTECT_NEXT_FALL;
					}else{
						state = ACTION_BRING_BACK_CYLINDER;
					}
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

		case PROTECT_NEXT_FALL:
			if(entrance){
				if(moduleToTake == RIGHT){
					ACT_push_order( ACT_CYLINDER_SLIDER_RIGHT , ACT_CYLINDER_SLIDER_RIGHT_ALMOST_OUT_WITH_CYLINDER );
				}else{
					ACT_push_order( ACT_CYLINDER_SLIDER_LEFT , ACT_CYLINDER_SLIDER_LEFT_ALMOST_OUT_WITH_CYLINDER );
				}
			}

			// Pas de gestion d'erreur ici, on continue
			// Cette action n'étant pas essentielle, on essaie de pousuivre l'action car on veut des points
			state = ACTION_BRING_BACK_CYLINDER;
			break;

		case ACTION_BRING_BACK_CYLINDER:
			if(entrance){
				if(moduleToTake == RIGHT){
					//On rentre le bras dans le robot
					ACT_push_order( ACT_CYLINDER_SLIDER_RIGHT, ACT_CYLINDER_SLIDER_RIGHT_IN );

					// On baisse l'élévateur si c'est possible (en prévision d'un futur stockage)
					if(STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_ELEVATOR, MODULE_STOCK_RIGHT)){
						ACT_push_order( ACT_CYLINDER_ELEVATOR_RIGHT, ACT_CYLINDER_ELEVATOR_RIGHT_BOTTOM);
					}

				}else{
					//On rentre le bras dans le robot
					ACT_push_order( ACT_CYLINDER_SLIDER_LEFT, ACT_CYLINDER_SLIDER_LEFT_IN );

					// On baisse l'élévateur si c'est possible (en prévision d'un futur stockage)
					if(STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_ELEVATOR, MODULE_STOCK_LEFT)){
						ACT_push_order( ACT_CYLINDER_ELEVATOR_LEFT, ACT_CYLINDER_ELEVATOR_LEFT_BOTTOM);
					}
				}
			}

			// Vérification des ordres effectués
			if(moduleToTake == RIGHT){
				state1 = check_act_status(ACT_QUEUE_Cylinder_slider_right, IN_PROGRESS, END_OK, NOT_HANDLED);
				state2 = check_act_status(ACT_QUEUE_Cylinder_elevator_right, IN_PROGRESS, END_OK, NOT_HANDLED);
			}else{
				state1 = check_act_status(ACT_QUEUE_Cylinder_slider_left, IN_PROGRESS, END_OK, NOT_HANDLED);
				state2 = check_act_status(ACT_QUEUE_Cylinder_elevator_left, IN_PROGRESS, END_OK, NOT_HANDLED);
			}

			if(state1 != IN_PROGRESS){
				if(state1 == END_OK){
					if((moduleToTake == RIGHT && state2 == END_OK && STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_ELEVATOR, MODULE_STOCK_RIGHT))
					|| (moduleToTake == LEFT && state2 == END_OK  && STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_ELEVATOR, MODULE_STOCK_LEFT))){
						state = ACTION_PREPARE_ELEVATOR;
					}else if((moduleToTake == RIGHT && !STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_ELEVATOR, MODULE_STOCK_RIGHT))
					|| (moduleToTake == LEFT && !STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_ELEVATOR, MODULE_STOCK_LEFT))){
						state = COMPUTE_NEXT_CYLINDER;
					}else{
						state = ERROR_DISABLE_ACT;  // En cas d'échec de l'élévateur
						if(moduleToTake == RIGHT){
							ELEMENTS_set_flag(FLAG_HARRY_DISABLE_MODULE_RIGHT, TRUE);
						}else{
							ELEMENTS_set_flag(FLAG_HARRY_DISABLE_MODULE_LEFT, TRUE);
						}
					}

					// On met à jour les données
					if(moduleToTake == RIGHT){
						STOCKS_addModule(moduleType, MODULE_STOCK_RIGHT);
					}else{
						STOCKS_addModule(moduleType, MODULE_STOCK_LEFT);
					}
					ROCKETS_removeModule(rocket);
					moduleToStore = moduleToTake;
					moduleToTake = NO_SIDE;
				}else{
					state = ERROR_DISABLE_ACT;	// En cas d'échec du slider
					if(moduleToTake == RIGHT){
						ELEMENTS_set_flag(FLAG_HARRY_DISABLE_MODULE_RIGHT, TRUE);
					}else{
						ELEMENTS_set_flag(FLAG_HARRY_DISABLE_MODULE_LEFT, TRUE);
					}
				}
			}
			break;

		case ACTION_PREPARE_ELEVATOR:
			if(entrance){
				if(moduleToStore == RIGHT){
					ACT_push_order( ACT_POMPE_ELEVATOR_RIGHT, ACT_POMPE_NORMAL );
					ACT_push_order( ACT_POMPE_SLIDER_RIGHT, ACT_POMPE_STOP );

					// Si c'est possible, on prépare la SLOPE pour un futur stockage
					if(STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_4_TO_OUT, MODULE_STOCK_RIGHT)){
						ACT_push_order( ACT_CYLINDER_SLOPE_RIGHT, ACT_CYLINDER_SLOPE_RIGHT_DOWN);
					}
				}else{
					ACT_push_order( ACT_POMPE_ELEVATOR_LEFT, ACT_POMPE_NORMAL );
					ACT_push_order( ACT_POMPE_SLIDER_LEFT, ACT_POMPE_STOP );

					// Si c'est possible, on prépare la SLOPE pour un futur stockage
					if(STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_4_TO_OUT, MODULE_STOCK_LEFT)){
						ACT_push_order( ACT_CYLINDER_SLOPE_LEFT, ACT_CYLINDER_SLOPE_LEFT_DOWN);
					}
				}
			}

			// Vérification des ordres effectués
			if(moduleToStore == RIGHT){
				state1 = check_act_status(ACT_QUEUE_Cylinder_slope_right, IN_PROGRESS, END_OK, NOT_HANDLED);
			}else{
				state1 = check_act_status(ACT_QUEUE_Cylinder_slope_left, IN_PROGRESS, END_OK, NOT_HANDLED);
			}


			/*if((moduleToStore == RIGHT && state1 == END_OK && STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_4_TO_OUT , MODULE_STOCK_RIGHT))
			|| (moduleToStore == LEFT && state1 == END_OK && STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_4_TO_OUT , MODULE_STOCK_LEFT))
			|| (moduleToStore == RIGHT && !STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_4_TO_OUT , MODULE_STOCK_RIGHT))
			|| (moduleToStore == LEFT && !STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_4_TO_OUT , MODULE_STOCK_LEFT))){*/
			if(1){
				state = wait_time(1000, state, ACTION_BRING_UP_CYLINDER);
			}else{
				state = ERROR_DISABLE_ACT;
				if(moduleToStore == RIGHT){
					ELEMENTS_set_flag(FLAG_HARRY_DISABLE_MODULE_RIGHT, TRUE);
				}else{
					ELEMENTS_set_flag(FLAG_HARRY_DISABLE_MODULE_LEFT, TRUE);
				}
			}


			// Mise à jour des données
			if(moduleToStore == RIGHT){
				STOCKS_makeModuleProgressTo(STOCK_PLACE_ENTRY_TO_ELEVATOR, MODULE_STOCK_RIGHT);
			}else{
				STOCKS_makeModuleProgressTo(STOCK_PLACE_ENTRY_TO_ELEVATOR, MODULE_STOCK_LEFT);
			}
			break;

		case ACTION_BRING_UP_CYLINDER:
			if(entrance){
				if(moduleToStore == RIGHT){
					// Si le stockage est possible
					if(STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_4_TO_OUT , MODULE_STOCK_RIGHT)){
						ACT_push_order( ACT_CYLINDER_ELEVATOR_RIGHT, ACT_CYLINDER_ELEVATOR_RIGHT_TOP);
					}else{ // sinon stocke en position milieu
						ACT_push_order( ACT_CYLINDER_ELEVATOR_RIGHT, ACT_CYLINDER_ELEVATOR_RIGHT_LOCK_WITH_CYLINDER);
					}
				}else{
					// Si le stockage est possible
					if(STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_4_TO_OUT , MODULE_STOCK_LEFT)){
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
				if(state1 == END_OK){
					state = ACTION_STOCK_UP_CYLINDER;

					// Mise à jour des données
					if(moduleToStore == RIGHT && STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_4_TO_OUT , MODULE_STOCK_RIGHT)){
						STOCKS_makeModuleProgressTo(STOCK_PLACE_ELEVATOR_TO_CONTAINER_IN, MODULE_STOCK_RIGHT);
					}else if(moduleToStore == LEFT && STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_4_TO_OUT , MODULE_STOCK_LEFT)){
						STOCKS_makeModuleProgressTo(STOCK_PLACE_ELEVATOR_TO_CONTAINER_IN, MODULE_STOCK_LEFT);
					}
				}else{
					state = ERROR_DISABLE_ACT;
					if(moduleToStore == RIGHT){
						ELEMENTS_set_flag(FLAG_HARRY_DISABLE_MODULE_RIGHT, TRUE);
					}else{
						ELEMENTS_set_flag(FLAG_HARRY_DISABLE_MODULE_LEFT, TRUE);
					}
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
				state1 = check_act_status(ACT_QUEUE_Cylinder_slope_right, IN_PROGRESS, END_OK, NOT_HANDLED);
			}else{
				state1 = check_act_status(ACT_QUEUE_Cylinder_slope_left, IN_PROGRESS, END_OK, NOT_HANDLED);
			}

			if(state1 != IN_PROGRESS){
				if(state1 == END_OK){
					state = ACTION_PUT_CYLINDER_IN_CONTAINER;
				}
			}
			break;

		case ACTION_PUT_CYLINDER_IN_CONTAINER:
			if(entrance){
				if(moduleToStore == RIGHT){
					ACT_push_order(ACT_POMPE_ELEVATOR_RIGHT, ACT_POMPE_STOP);
					//ACT_push_order(ACT_CYLINDER_SLOPE_RIGHT, ACT_CYLINDER_SLOPE_RIGHT_VERY_UP);
				}else{
					ACT_push_order(ACT_POMPE_ELEVATOR_LEFT, ACT_POMPE_STOP);
					//ACT_push_order(ACT_CYLINDER_SLOPE_LEFT, ACT_CYLINDER_SLOPE_LEFT_VERY_UP);
				}
			}

			// Vérification des ordres effectués
			if(moduleToStore == RIGHT){
				state1 = check_act_status(ACT_QUEUE_Cylinder_slope_right, IN_PROGRESS, END_OK, NOT_HANDLED);
			}else{
				state1 = check_act_status(ACT_QUEUE_Cylinder_slope_left, IN_PROGRESS, END_OK, NOT_HANDLED);
			}

			if(state1 != IN_PROGRESS){
				if(state1 == END_OK){
					state = wait_time(2000, state, ACTION_PUT_SLOPE_DOWN);
				}
			}
			break;

		case ACTION_PUT_SLOPE_DOWN:
			if(entrance){
				if(moduleToStore == RIGHT){
					ACT_push_order(ACT_CYLINDER_SLOPE_RIGHT, ACT_CYLINDER_SLOPE_RIGHT_DOWN);
				}else{
					ACT_push_order(ACT_CYLINDER_SLOPE_LEFT, ACT_CYLINDER_SLOPE_LEFT_DOWN);
				}
			}

			// Vérification des ordres effectués
			if(moduleToStore == RIGHT){
				state1 = check_act_status(ACT_QUEUE_Cylinder_slope_right, IN_PROGRESS, END_OK, NOT_HANDLED);
			}else{
				state1 = check_act_status(ACT_QUEUE_Cylinder_slope_left, IN_PROGRESS, END_OK, NOT_HANDLED);
			}

			if(state1 != IN_PROGRESS){
				if(state1 == END_OK){
					state = COMPUTE_NEXT_CYLINDER;
				}
			}
			break;

		case ERROR_DISABLE_ACT:
			state = COMPUTE_NEXT_CYLINDER;
			break;

		case DONE:
			if(entrance){
				set_sub_act_enable(SUB_HARRY_DEPOSE_MODULES, TRUE);
			}
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
				debug_printf("default case in sub_act_harry_take_rocket\n");
			break;
	}

	return IN_PROGRESS;
}







// Subaction actionneur de stockage
error_e sub_act_harry_mae_store_modules(moduleStockLocation_e storage, bool_e trigger){
	CREATE_MAE_WITH_VERBOSE(SM_ID_ACT_HARRY_MAE_STORE_MODULES,
			WAIT_TRIGGER,
			INIT,
			COMPUTE_ACTION,

			// Gestion des erreurs du DISPENSER
			COMPUTE_ERROR_DISPENSER,
			ERROR_DISPENSER_GO_LOCK_1,
			ERROR_WAIT_DISPENSER_LOCK_1,
			ERROR_DISPENSER_GO_UNLOCK,
			ERROR_WAIT_DISPENSER_UNLOCK,
			ERROR_DISPENSER_GO_LOCK_2,
			ERROR_WAIT_DISPENSER_LOCK_2,

			// Le stockage des modules
			ELEVATOR_GO_BOTTOM,
			ACTION_PREPARE_ELEVATOR,
			CHECK_CONTAINER_IS_AVAILABLE,
			ELEVATOR_GO_MID_POS,
			PREPARE_SLOPE_FOR_ELEVATOR,
			ELEVATOR_GO_TOP_POS,
			STOCK_MODULE_IN_CONTAINER,

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


	static enum state_e stateRight = WAIT_TRIGGER, stateLeft = WAIT_TRIGGER;
	static error_e stateAct = IN_PROGRESS;

	if(storage == MODULE_STOCK_RIGHT){
		state = stateRight;
	}else if(storage == MODULE_STOCK_LEFT){
		state = stateLeft;
	}else{
		error_printf("sub_act_harry_mae_store_modules could only be called with MODULE_STOCK_RIGHT ou MODULE_STOCK_LEFT\n");
		return NOT_HANDLED;
	}


	switch(state){

		case WAIT_TRIGGER:
			if(trigger){
				state = INIT;
			}
			break;

		case INIT:
			if(STOCKS_isFull(storage)){
				state = DONE;	// Il n'y a rien à faire
			}else{
				state = COMPUTE_ACTION; // Stockage possible
			}
			break;

		case COMPUTE_ACTION:
			if((storage == MODULE_STOCK_RIGHT && ELEMENTS_get_flag(FLAG_HARRY_DISABLE_MODULE_RIGHT))
			|| (storage == MODULE_STOCK_LEFT && ELEMENTS_get_flag(FLAG_HARRY_DISABLE_MODULE_LEFT))){
				state = ERROR;
			}else if(!STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_ELEVATOR, storage) && STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_4_TO_OUT, storage)){  // L'élévateur n'est pas vide et le stockage est possible
				state = PREPARE_SLOPE_FOR_ELEVATOR; // On cherche a stocker le module déjà présent dans l'élévateur
			}else if(!STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_ENTRY, storage) && STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_ELEVATOR, storage)){ // Il y a un module en bas et l'élévateur est vide
				state = ELEVATOR_GO_BOTTOM; // On cherche a stocker le module qui est en bas
			}else if(STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_ELEVATOR, storage)){
				state = ELEVATOR_GO_BOTTOM_TO_END; // On redescend l'élévateur si besoin avant de finir
			}else{ // les stocks sont plein ou il n'y a plus rien à faire
				state = DONE;
			}
			break;

		case COMPUTE_ERROR_DISPENSER:
			// Cet état est chargé de faire de la place lorsque le dispenser a mal été utilisé.
			if(STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_1_TO_OUT, storage)
			&& ((storage == MODULE_STOCK_RIGHT && ELEMENTS_get_flag(FLAG_HARRY_DISPENSER_RIGHT_OUT))
			|| (storage == MODULE_STOCK_LEFT && ELEMENTS_get_flag(FLAG_HARRY_DISPENSER_LEFT_OUT)))){
				state = ERROR_DISPENSER_GO_LOCK_1;
			}else if(STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_1_TO_OUT, storage)
			&& ((storage == MODULE_STOCK_RIGHT && !ELEMENTS_get_flag(FLAG_HARRY_DISPENSER_RIGHT_OUT))
			|| (storage == MODULE_STOCK_LEFT && !ELEMENTS_get_flag(FLAG_HARRY_DISPENSER_LEFT_OUT)))){
				state = ERROR_DISPENSER_GO_UNLOCK;
			}else{ // Sinon le dispenser est mal seulement mal orienté mais un module est présent en STOCK_POS_1_OUT
				state = ERROR_DISPENSER_GO_LOCK_2;
			}
			break;

		case ERROR_DISPENSER_GO_LOCK_1:
			if(entrance){
				// On remet le dispenser en position LOCK
				if(storage == MODULE_STOCK_RIGHT){
					ACT_push_order( ACT_CYLINDER_BALANCER_RIGHT, ACT_CYLINDER_BALANCER_RIGHT_IN);
					ELEMENTS_set_flag(FLAG_HARRY_DISPENSER_RIGHT_OUT, FALSE);
					STOCKS_makeModuleProgressTo(STOCK_PLACE_DISPENSER_TO_CONTAINER_OUT, MODULE_STOCK_RIGHT);
				}else{
					ACT_push_order( ACT_CYLINDER_BALANCER_LEFT, ACT_CYLINDER_BALANCER_LEFT_IN);
					ELEMENTS_set_flag(FLAG_HARRY_DISPENSER_LEFT_OUT, FALSE);
					STOCKS_makeModuleProgressTo(STOCK_PLACE_DISPENSER_TO_CONTAINER_OUT, MODULE_STOCK_LEFT);
				}
			}

			// Vérification des ordres effectués
			if(storage == MODULE_STOCK_RIGHT){
				state = ERROR_WAIT_DISPENSER_LOCK_1; // On considère que c'est ok
				//state = check_act_status(ACT_QUEUE_Cylinder_dispenser_right, state, ERROR_WAIT_DISPENSER_LOCK_1, ERROR_WAIT_DISPENSER_LOCK_1);
			}else{
				state = ERROR_WAIT_DISPENSER_LOCK_1; // On considère que c'est ok
				//state = check_act_status(ACT_QUEUE_Cylinder_dispenser_left, state, ERROR_WAIT_DISPENSER_LOCK_1, ERROR_WAIT_DISPENSER_LOCK_1);
			}
			break;

		case ERROR_WAIT_DISPENSER_LOCK_1:
			state = wait_time(1000, state, ERROR_DISPENSER_GO_UNLOCK);
			break;

		case ERROR_DISPENSER_GO_UNLOCK:
			if(entrance){
				// On fait descendre un module grâce au dispenser
				if(storage == MODULE_STOCK_RIGHT){
					ACT_push_order( ACT_CYLINDER_BALANCER_RIGHT, ACT_CYLINDER_BALANCER_RIGHT_OUT);
					ELEMENTS_set_flag(FLAG_HARRY_DISPENSER_RIGHT_OUT, TRUE);
					STOCKS_makeModuleProgressTo(STOCK_PLACE_CONTAINER_IN_TO_DISPENSER, MODULE_STOCK_RIGHT);
				}else{
					ACT_push_order( ACT_CYLINDER_BALANCER_LEFT, ACT_CYLINDER_BALANCER_LEFT_OUT);
					ELEMENTS_set_flag(FLAG_HARRY_DISPENSER_LEFT_OUT, TRUE);
					STOCKS_makeModuleProgressTo(STOCK_PLACE_CONTAINER_IN_TO_DISPENSER, MODULE_STOCK_LEFT);
				}
			}

			// Vérification des ordres effectués
			if(storage == MODULE_STOCK_RIGHT){
				state =  ERROR_WAIT_DISPENSER_UNLOCK;  // ON considère que c'est ok
				//state = check_act_status(ACT_QUEUE_Cylinder_dispenser_right, state, ERROR_WAIT_DISPENSER_UNLOCK, ERROR_WAIT_DISPENSER_UNLOCK);
			}else{
				state =  ERROR_WAIT_DISPENSER_UNLOCK;  // ON considère que c'est ok
				//state = check_act_status(ACT_QUEUE_Cylinder_dispenser_left, state, ERROR_WAIT_DISPENSER_UNLOCK, ERROR_WAIT_DISPENSER_UNLOCK);
			}
			break;

		case ERROR_WAIT_DISPENSER_UNLOCK:
			state = wait_time(1000, state, ERROR_DISPENSER_GO_LOCK_2);
			break;


		case ERROR_DISPENSER_GO_LOCK_2:
			if(entrance){
				// On remet le dispenser en position LOCK
				if(storage == MODULE_STOCK_RIGHT){
					ACT_push_order( ACT_CYLINDER_BALANCER_RIGHT, ACT_CYLINDER_BALANCER_RIGHT_IN);
					ELEMENTS_set_flag(FLAG_HARRY_DISPENSER_RIGHT_OUT, FALSE);
					STOCKS_makeModuleProgressTo(STOCK_PLACE_DISPENSER_TO_CONTAINER_OUT, MODULE_STOCK_RIGHT);
				}else{
					ACT_push_order( ACT_CYLINDER_BALANCER_LEFT, ACT_CYLINDER_BALANCER_LEFT_IN);
					ELEMENTS_set_flag(FLAG_HARRY_DISPENSER_LEFT_OUT, FALSE);
					STOCKS_makeModuleProgressTo(STOCK_PLACE_DISPENSER_TO_CONTAINER_OUT, MODULE_STOCK_LEFT);
				}
			}

			// Vérification des ordres effectués
			if(storage == MODULE_STOCK_RIGHT){
				state = ERROR_WAIT_DISPENSER_LOCK_2; // On considère que c'est ok
				//state = check_act_status(ACT_QUEUE_Cylinder_dispenser_right, state, ERROR_WAIT_DISPENSER_LOCK_2, ERROR_WAIT_DISPENSER_LOCK_2);
			}else{
				state = ERROR_WAIT_DISPENSER_LOCK_2; // On considère que c'est ok
				//state = check_act_status(ACT_QUEUE_Cylinder_dispenser_left, state, ERROR_WAIT_DISPENSER_LOCK_2, ERROR_WAIT_DISPENSER_LOCK_2);
			}
			break;

		case ERROR_WAIT_DISPENSER_LOCK_2:
				state = wait_time(1000, state, COMPUTE_ACTION);
				break;

		case ELEVATOR_GO_BOTTOM:
			if(entrance){
				// On descend l'élévateur
				if(storage == MODULE_STOCK_RIGHT){
					ACT_push_order( ACT_CYLINDER_ELEVATOR_RIGHT, ACT_CYLINDER_ELEVATOR_RIGHT_BOTTOM);
				}else{
					ACT_push_order( ACT_CYLINDER_ELEVATOR_LEFT, ACT_CYLINDER_ELEVATOR_LEFT_BOTTOM);
				}
			}

			// Vérification des ordres effectués
			if(storage == MODULE_STOCK_RIGHT){
				state = check_act_status(ACT_QUEUE_Cylinder_elevator_right, state, ACTION_PREPARE_ELEVATOR, ERROR_DISABLE_ACT);
			}else{
				state = check_act_status(ACT_QUEUE_Cylinder_elevator_left, state, ACTION_PREPARE_ELEVATOR, ERROR_DISABLE_ACT);
			}
			break;

		case ACTION_PREPARE_ELEVATOR:
			if(entrance){
				if(storage == MODULE_STOCK_RIGHT){
					ACT_push_order( ACT_POMPE_ELEVATOR_RIGHT , ACT_POMPE_NORMAL );
					ACT_push_order( ACT_POMPE_SLIDER_RIGHT , ACT_POMPE_STOP );

					// Si c'est possible, on prépare la SLOPE pour un futur stockage
					if(STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_4_TO_OUT , MODULE_STOCK_RIGHT)){
						ACT_push_order( ACT_CYLINDER_SLOPE_RIGHT, ACT_CYLINDER_SLOPE_RIGHT_DOWN);
					}

					// On bouge le dispenser si possible
					if(!ELEMENTS_get_flag(FLAG_HARRY_DISPENSER_RIGHT_OUT) && STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_1_TO_OUT, MODULE_STOCK_RIGHT)){
						ACT_push_order( ACT_CYLINDER_BALANCER_RIGHT, ACT_CYLINDER_BALANCER_RIGHT_OUT);
						ELEMENTS_set_flag(FLAG_HARRY_DISPENSER_RIGHT_OUT, TRUE);
						STOCKS_makeModuleProgressTo(STOCK_PLACE_DISPENSER_TO_CONTAINER_OUT, MODULE_STOCK_RIGHT);
					}
				}else{
					ACT_push_order( ACT_POMPE_ELEVATOR_LEFT , ACT_POMPE_NORMAL );
					ACT_push_order( ACT_POMPE_SLIDER_LEFT , ACT_POMPE_STOP );
					// Si c'est possible, on prépare la SLOPE pour un futur stockage
					if(STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_4_TO_OUT , MODULE_STOCK_LEFT)){
						ACT_push_order( ACT_CYLINDER_SLOPE_LEFT, ACT_CYLINDER_SLOPE_LEFT_DOWN);
					}
					// On bouge le dispenser si possible
					if(!ELEMENTS_get_flag(FLAG_HARRY_DISPENSER_LEFT_OUT) && STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_1_TO_OUT, MODULE_STOCK_LEFT)){
						ACT_push_order( ACT_CYLINDER_BALANCER_LEFT, ACT_CYLINDER_BALANCER_LEFT_OUT);
						ELEMENTS_set_flag(FLAG_HARRY_DISPENSER_LEFT_OUT, TRUE);
						STOCKS_makeModuleProgressTo(STOCK_PLACE_DISPENSER_TO_CONTAINER_OUT, MODULE_STOCK_LEFT);
					}
				}
			}

			// Vérification des ordres effectués
			if(storage == MODULE_STOCK_RIGHT){
				stateAct = check_act_status(ACT_QUEUE_Cylinder_slope_right, IN_PROGRESS, END_OK, NOT_HANDLED);
			}else{
				stateAct = check_act_status(ACT_QUEUE_Cylinder_slope_left, IN_PROGRESS, END_OK, NOT_HANDLED);
			}

			if((storage == MODULE_STOCK_RIGHT && stateAct == END_OK && STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_4_TO_OUT , MODULE_STOCK_RIGHT))
			|| (storage == MODULE_STOCK_LEFT && stateAct == END_OK && STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_4_TO_OUT , MODULE_STOCK_LEFT))
			|| (storage == MODULE_STOCK_RIGHT && !STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_4_TO_OUT , MODULE_STOCK_RIGHT))
			|| (storage == MODULE_STOCK_LEFT && !STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_4_TO_OUT , MODULE_STOCK_LEFT))){
				state = CHECK_CONTAINER_IS_AVAILABLE;
			}else{
				state = ERROR_DISABLE_ACT;
				if(storage == MODULE_STOCK_RIGHT){
					ELEMENTS_set_flag(FLAG_HARRY_DISABLE_MODULE_RIGHT, TRUE);
				}else{
					ELEMENTS_set_flag(FLAG_HARRY_DISABLE_MODULE_LEFT, TRUE);
				}
			}
			// Mise à jour des données
			STOCKS_makeModuleProgressTo(STOCK_PLACE_ENTRY_TO_ELEVATOR, storage);
			break;

		case CHECK_CONTAINER_IS_AVAILABLE:
			if((storage == MODULE_STOCK_RIGHT && STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_4_TO_OUT , MODULE_STOCK_RIGHT))
			|| (storage == MODULE_STOCK_LEFT && STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_4_TO_OUT , MODULE_STOCK_LEFT)) ){
				state = ELEVATOR_GO_TOP_POS;
			}else{
				state = ELEVATOR_GO_MID_POS;
			}
			break;

		case ELEVATOR_GO_MID_POS:
			if(entrance){
				if(storage == MODULE_STOCK_RIGHT){
					ACT_push_order( ACT_CYLINDER_ELEVATOR_RIGHT, ACT_CYLINDER_ELEVATOR_RIGHT_LOCK_WITH_CYLINDER);

					// On bouge le dispenser si possible
					if(ELEMENTS_get_flag(FLAG_HARRY_DISPENSER_RIGHT_OUT)){
						ACT_push_order( ACT_CYLINDER_BALANCER_RIGHT, ACT_CYLINDER_BALANCER_RIGHT_IN);
						ELEMENTS_set_flag(FLAG_HARRY_DISPENSER_RIGHT_OUT, FALSE);
						STOCKS_makeModuleProgressTo(STOCK_PLACE_CONTAINER_IN_TO_DISPENSER, MODULE_STOCK_RIGHT);
					}
				}else{
					ACT_push_order( ACT_CYLINDER_ELEVATOR_LEFT, ACT_CYLINDER_ELEVATOR_LEFT_LOCK_WITH_CYLINDER);

					// On bouge le dispenser si possible
					if(ELEMENTS_get_flag(FLAG_HARRY_DISPENSER_LEFT_OUT)){
						ACT_push_order( ACT_CYLINDER_BALANCER_LEFT, ACT_CYLINDER_BALANCER_LEFT_IN);
						ELEMENTS_set_flag(FLAG_HARRY_DISPENSER_LEFT_OUT, FALSE);
						STOCKS_makeModuleProgressTo(STOCK_PLACE_CONTAINER_IN_TO_DISPENSER, MODULE_STOCK_LEFT);
					}
				}
			}

			// Vérification des ordres effectués
			if(storage == MODULE_STOCK_RIGHT){
				state = check_act_status(ACT_QUEUE_Cylinder_elevator_right, state, COMPUTE_ACTION, ERROR_DISABLE_ACT);
			}else{
				state = check_act_status(ACT_QUEUE_Cylinder_elevator_left, state, COMPUTE_ACTION, ERROR_DISABLE_ACT);
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

			// Vérification des ordres effectués
			if(storage == MODULE_STOCK_RIGHT){
				state = check_act_status(ACT_QUEUE_Cylinder_slope_right, state, ELEVATOR_GO_TOP_POS, ERROR_DISABLE_ACT);
			}else{
				state = check_act_status(ACT_QUEUE_Cylinder_slope_left, state, ELEVATOR_GO_TOP_POS, ERROR_DISABLE_ACT);
			}
			break;

		case ELEVATOR_GO_TOP_POS:
			if(entrance){
				if(storage == MODULE_STOCK_RIGHT){
					ACT_push_order( ACT_CYLINDER_ELEVATOR_RIGHT, ACT_CYLINDER_ELEVATOR_RIGHT_TOP);

					// On bouge le dispenser si possible
					if(ELEMENTS_get_flag(FLAG_HARRY_DISPENSER_RIGHT_OUT)){
						ACT_push_order( ACT_CYLINDER_BALANCER_RIGHT, ACT_CYLINDER_BALANCER_RIGHT_IN);
						ELEMENTS_set_flag(FLAG_HARRY_DISPENSER_RIGHT_OUT, FALSE);
						STOCKS_makeModuleProgressTo(STOCK_PLACE_CONTAINER_IN_TO_DISPENSER, MODULE_STOCK_RIGHT);
					}
				}else{
					ACT_push_order( ACT_CYLINDER_ELEVATOR_LEFT, ACT_CYLINDER_ELEVATOR_LEFT_TOP);

					// On bouge le dispenser si possible
					if(ELEMENTS_get_flag(FLAG_HARRY_DISPENSER_LEFT_OUT)){
						ACT_push_order( ACT_CYLINDER_BALANCER_LEFT, ACT_CYLINDER_BALANCER_LEFT_IN);
						ELEMENTS_set_flag(FLAG_HARRY_DISPENSER_LEFT_OUT, FALSE);
						STOCKS_makeModuleProgressTo(STOCK_PLACE_CONTAINER_IN_TO_DISPENSER, MODULE_STOCK_LEFT);
					}
				}
			}

			// Vérification des ordres effectués
			if(storage == MODULE_STOCK_RIGHT){
				state = check_act_status(ACT_QUEUE_Cylinder_elevator_right, state, STOCK_MODULE_IN_CONTAINER, ERROR_DISABLE_ACT);
			}else{
				state = check_act_status(ACT_QUEUE_Cylinder_elevator_left, state, STOCK_MODULE_IN_CONTAINER, ERROR_DISABLE_ACT);
			}
			break;

		case STOCK_MODULE_IN_CONTAINER:
			if(entrance){
				if(storage == MODULE_STOCK_RIGHT){
					ACT_push_order( ACT_POMPE_ELEVATOR_RIGHT, ACT_POMPE_STOP);
					ACT_push_order( ACT_CYLINDER_SLOPE_RIGHT, ACT_CYLINDER_SLOPE_RIGHT_UP);
				}else{
					ACT_push_order( ACT_POMPE_ELEVATOR_LEFT, ACT_POMPE_STOP);
					ACT_push_order( ACT_CYLINDER_SLOPE_RIGHT, ACT_CYLINDER_SLOPE_RIGHT_UP);
				}
			}

			// Vérification des ordres effectués
			if(storage == MODULE_STOCK_RIGHT){
				stateAct = check_act_status(ACT_QUEUE_Cylinder_slope_right, IN_PROGRESS, END_OK, NOT_HANDLED);
			}else{
				stateAct = check_act_status(ACT_QUEUE_Cylinder_slope_left, IN_PROGRESS, END_OK, NOT_HANDLED);
			}

			if(stateAct == END_OK){
				state = COMPUTE_ACTION;

				// Mise à jour des données
				STOCKS_makeModuleProgressTo(STOCK_PLACE_ELEVATOR_TO_CONTAINER_IN, storage);
			}else{
				state = ERROR_DISABLE_ACT;
			}
			break;

		case ELEVATOR_GO_BOTTOM_TO_END:
			if(entrance){
				// On descend l'élévateur
				if(storage == MODULE_STOCK_RIGHT){
					ACT_push_order( ACT_CYLINDER_ELEVATOR_RIGHT, ACT_CYLINDER_ELEVATOR_RIGHT_BOTTOM);
				}else{
					ACT_push_order( ACT_CYLINDER_ELEVATOR_LEFT, ACT_CYLINDER_ELEVATOR_LEFT_BOTTOM);
				}
			}

			// Vérification des ordres effectués
			if(storage == MODULE_STOCK_RIGHT){
				state = check_act_status(ACT_QUEUE_Cylinder_elevator_right, state, CHECK_DISPENSER_TO_END, ERROR_DISABLE_ACT);
			}else{
				state = check_act_status(ACT_QUEUE_Cylinder_elevator_left, state, CHECK_DISPENSER_TO_END, ERROR_DISABLE_ACT);
			}
			break;

		case CHECK_DISPENSER_TO_END:
			if(STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_1_TO_OUT, storage)){ // condition peut-être à compléter encore
				state = DISPENSER_GO_UNLOCK_TO_END; // On doit encore bouger le dispenser avant de finir
			}else{
				state = DONE; // C'est bon, on a plus rien a faire
			}
			break;

		case DISPENSER_GO_UNLOCK_TO_END:
			if(entrance){
				// On fait descendre un module grâce au dispenser
				if(storage == MODULE_STOCK_RIGHT){
					ACT_push_order( ACT_CYLINDER_BALANCER_RIGHT, ACT_CYLINDER_BALANCER_RIGHT_OUT);
					ELEMENTS_set_flag(FLAG_HARRY_DISPENSER_RIGHT_OUT, TRUE);
					STOCKS_makeModuleProgressTo(STOCK_PLACE_DISPENSER_TO_CONTAINER_OUT, MODULE_STOCK_RIGHT);
				}else{
					ACT_push_order( ACT_CYLINDER_BALANCER_LEFT, ACT_CYLINDER_BALANCER_LEFT_OUT);
					ELEMENTS_set_flag(FLAG_HARRY_DISPENSER_LEFT_OUT, TRUE);
					STOCKS_makeModuleProgressTo(STOCK_PLACE_DISPENSER_TO_CONTAINER_OUT, MODULE_STOCK_LEFT);
				}
			}

			// Vérification des ordres effectués
			if(storage == MODULE_STOCK_RIGHT){
				state = check_act_status(ACT_QUEUE_Cylinder_balancer_right, state, WAIT_DISPENSER_TO_END, ERROR_DISABLE_ACT);
			}else{
				state = check_act_status(ACT_QUEUE_Cylinder_balancer_left, state, WAIT_DISPENSER_TO_END, ERROR_DISABLE_ACT);
			}
			break;

		case WAIT_DISPENSER_TO_END:
			state = wait_time(1000, state, DISPENSER_GO_LOCK_TO_END);
			break;

		case DISPENSER_GO_LOCK_TO_END:
			if(entrance){
				// On remet le dispenser en position LOCK
				if(storage == MODULE_STOCK_RIGHT){
					ACT_push_order( ACT_CYLINDER_BALANCER_RIGHT, ACT_CYLINDER_BALANCER_RIGHT_IN);
					ELEMENTS_set_flag(FLAG_HARRY_DISPENSER_RIGHT_OUT, FALSE);
					STOCKS_makeModuleProgressTo(STOCK_PLACE_CONTAINER_IN_TO_DISPENSER, MODULE_STOCK_RIGHT);
				}else{
					ACT_push_order( ACT_CYLINDER_BALANCER_LEFT, ACT_CYLINDER_BALANCER_LEFT_IN);
					ELEMENTS_set_flag(FLAG_HARRY_DISPENSER_LEFT_OUT, FALSE);
					STOCKS_makeModuleProgressTo(STOCK_PLACE_CONTAINER_IN_TO_DISPENSER, MODULE_STOCK_LEFT);
				}
			}

			// Vérification des ordres effectués
			if(storage == MODULE_STOCK_RIGHT){
				state = check_act_status(ACT_QUEUE_Cylinder_balancer_right, state, DONE, DONE);	// On considère que c'est ok
			}else{
				state = check_act_status(ACT_QUEUE_Cylinder_balancer_left, state, DONE, DONE);		// On considère que c'est ok
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
			}
			//RESET_MAE();
			on_turning_point();
			return END_OK;
			break;

		case ERROR:
			//RESET_MAE();
			on_turning_point();
			return NOT_HANDLED;
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
	} // else L'erreur a déjà été affichée

	return IN_PROGRESS;
}








// Subaction actionneur de préparation de la dépose des modules
error_e sub_act_harry_mae_prepare_modules_for_dispose(moduleStockLocation_e storage, bool_e trigger){
	CREATE_MAE_WITH_VERBOSE(SM_ID_ACT_HARRY_MAE_PREPARE_MODULES_FOR_DISPOSE,
			WAIT_TRIGGER,
			INIT,
			MOVE_BALANCER_OUT,
			TURN_FOR_COLOR,
			WAIT_ADV_COLOR,
			WAIT_WHITE,
			STOP_TURN,
			END_CHECK_POSITION_BALANCER,
			END_MOVE_BALANCER_IN,
			ERROR,
			DONE
		);

	#define TIMEOUT_COLOR	(4000)


	static enum state_e stateRight = WAIT_TRIGGER, stateLeft = WAIT_TRIGGER;
	static error_e stateAct = IN_PROGRESS;
	static time32_t time_timeout = 0;

	if(storage == MODULE_STOCK_RIGHT){
		state = stateRight;
	}else if(storage == MODULE_STOCK_LEFT){
		state = stateLeft;
	}else{
		error_printf("sub_act_harry_mae_prepare_modules_for_dispose could only be called with MODULE_STOCK_RIGHT ou MODULE_STOCK_LEFT\n");
		return NOT_HANDLED;
	}

	switch(state){

		case WAIT_TRIGGER:
			if(trigger){
				state = INIT;

				// On baisse le flag dans le cas où cela n'a pas encore été fait
				if(storage == MODULE_STOCK_RIGHT){
					ELEMENTS_set_flag(FLAG_HARRY_MODULE_COLOR_RIGHT_SUCCESS, FALSE);
					ELEMENTS_set_flag(FLAG_HARRY_MODULE_COLOR_RIGHT_FINISH, FALSE);
				}else{
					ELEMENTS_set_flag(FLAG_HARRY_MODULE_COLOR_LEFT_SUCCESS, FALSE);
					ELEMENTS_set_flag(FLAG_HARRY_MODULE_COLOR_LEFT_FINISH, FALSE);
				}
			}
			break;

		case INIT:
			if(STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_1_TO_OUT, storage)){ // TODO : A revoir
				state = MOVE_BALANCER_OUT; // Préparation de la dépose possible
			}else{
				state = ERROR;	// Il n'y a pas de cylindre disponible
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

			// Vérification des ordres effectués
			if(storage == MODULE_STOCK_RIGHT){
				state = check_act_status(ACT_QUEUE_Cylinder_balancer_right, state, TURN_FOR_COLOR, ERROR);
			}else{
				state = check_act_status(ACT_QUEUE_Cylinder_balancer_left, state, TURN_FOR_COLOR, ERROR);
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
			// Aucune vérification ici
			state = WAIT_ADV_COLOR;
			break;

		case WAIT_ADV_COLOR:
			if(global.absolute_time > time_timeout){
				state = STOP_TURN;   // Problème : on arrive pas a déterminer la couleur
			}else if(storage == MODULE_STOCK_RIGHT){
				if((global.color==BLUE)&&(CW_is_color_detected(CW_SENSOR_RIGHT, CW_Channel_Yellow))){ //jaune à droite
					state=WAIT_WHITE;
				}else if((global.color==YELLOW)&&(CW_is_color_detected(CW_SENSOR_RIGHT, CW_Channel_Blue))){ //bleu à droite
					state=WAIT_WHITE;
				}
			}else{
				if((global.color==BLUE)&&(CW_is_color_detected(CW_SENSOR_LEFT, CW_Channel_Yellow))){ //jaune à gauche
					state=WAIT_WHITE;
				}else if((global.color==YELLOW)&&(CW_is_color_detected(CW_SENSOR_LEFT, CW_Channel_Blue))){ //bleu à gauche
					state=WAIT_WHITE;
				}
			}
			break;

		case WAIT_WHITE:
			// On en profite pour retourner le balancer vers l'intérieur du robot pour gagner du temps
			// Pas de vérification du résultat ici, la couleur est prioritaire.
			if(entrance){
				if(storage == MODULE_STOCK_RIGHT){
					ACT_push_order(ACT_CYLINDER_BALANCER_RIGHT, ACT_CYLINDER_BALANCER_RIGHT_OUT);
				}else{
					ACT_push_order(ACT_CYLINDER_BALANCER_LEFT, ACT_CYLINDER_BALANCER_LEFT_OUT);
				}
			}

			// On attend la couleur blanche
			if(global.absolute_time > time_timeout){
				state = STOP_TURN;   // Problème : on arrive pas a déterminer la couleur
			}else if(storage == MODULE_STOCK_RIGHT){
				if(CW_is_color_detected(CW_SENSOR_RIGHT, CW_Channel_White)){ //blanc à gauche
					ELEMENTS_set_flag(FLAG_HARRY_MODULE_COLOR_RIGHT_SUCCESS, TRUE);
					state=STOP_TURN;
				}
			}else{
				if(CW_is_color_detected(CW_SENSOR_LEFT, CW_Channel_White)){ //blanc à droite
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
			// Aucune vérification ici
			state = END_CHECK_POSITION_BALANCER;
			break;

		case END_CHECK_POSITION_BALANCER:
			if(storage == MODULE_STOCK_RIGHT){
				stateAct = ACT_check_position_config(ACT_CYLINDER_BALANCER_RIGHT, ACT_CYLINDER_BALANCER_RIGHT_IN);
			}else{
				stateAct = ACT_check_position_config(ACT_CYLINDER_BALANCER_LEFT, ACT_CYLINDER_BALANCER_LEFT_IN);
			}

			if(stateAct == NOT_HANDLED || stateAct == END_WITH_TIMEOUT){
				state = END_MOVE_BALANCER_IN;
			}else{
				state = DONE;  // C'est bon l'actionneur est en position
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

			// Aucune vérification
			state = DONE;
			break;

		case DONE:
			if(storage == MODULE_STOCK_RIGHT){
				ELEMENTS_set_flag(FLAG_HARRY_MODULE_COLOR_RIGHT_FINISH, TRUE);
			}else{
				ELEMENTS_set_flag(FLAG_HARRY_MODULE_COLOR_LEFT_FINISH, TRUE);
			}
			RESET_MAE();
			on_turning_point();
			return END_OK;
			break;

		case ERROR:
			if(storage == MODULE_STOCK_RIGHT){
				ELEMENTS_set_flag(FLAG_HARRY_MODULE_COLOR_RIGHT_FINISH, TRUE);
			}else{
				ELEMENTS_set_flag(FLAG_HARRY_MODULE_COLOR_LEFT_FINISH, TRUE);
			}
			RESET_MAE();
			on_turning_point();
			return NOT_HANDLED;
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
	} // else L'erreur a déjà été affichée

	return IN_PROGRESS;
}








// Subaction actionneur de dépose des modules
error_e sub_act_harry_mae_dispose_modules(moduleStockLocation_e storage, arg_dipose_mae_e arg_dispose){
	CREATE_MAE_WITH_VERBOSE(SM_ID_ACT_HARRY_MAE_DISPOSE_MODULES,
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


	static enum state_e stateRight = INIT, stateLeft = INIT;
	//static error_e stateAct = IN_PROGRESS;
	static bool_e anotherDisposeWillFollow = FALSE;

	if(storage == MODULE_STOCK_RIGHT){
		state = stateRight;
	}else if(storage == MODULE_STOCK_LEFT){
		state = stateLeft;
	}else{
		error_printf("sub_act_harry_mae_dispose_modules could only be called with MODULE_STOCK_RIGHT ou MODULE_STOCK_LEFT\n");
		return NOT_HANDLED;
	}


	switch(state){

		case INIT:
			if(STOCKS_isEmpty(storage)){
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

		// Sortir le bras
		// Tourner balancer pour positionner un cylindre sur le retourneur de couleur (voir stockage si necéssaire)
		// Tourner le cylindre pour la couleur + tourner balancer pour recharger un cylindre
		// Ventousage du cylindre
		// Monter le bras
		// Sortir le cylindre du robot
		// Dépose du cylindre (1er mouvement) + balancer
		// Dépose du cylindre (2eme mouvement) + couleur + balancer pour recharger un cylindre
		// Replier le bras (en 2 mouvements)

		// Parallélisation des actions : mouvements du bras // couleur // balancer
		// Pour la couleur : ca serait bien d'avoir une fonction ou une MAE indépendante avec déclenchement par trigger (un flag indique quand l'action est finie)
		// En gros il faudrait baser la subaction sur les mouvements du bras. les mouvements du balancer et la couleur
		// peuvent être parallèlisés et être rajoutés par la suite.
		// Ici il n'y a pas de boucle a priori dans cette MAE

		// Remarque :  est il pertinent de n'avoir que cette subaction qui bouge le balancer ?


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
				state = check_act_status(ACT_QUEUE_Cylinder_arm_right, state, INIT_DISPOSE_SERVO, INIT_DISPOSE_SERVO);
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
				state = check_act_status(ACT_QUEUE_Cylinder_dispose_right, state, TRIGGER_CYLINDER_PREPARATION, TRIGGER_CYLINDER_PREPARATION);
			}
			break;

		case TRIGGER_CYLINDER_PREPARATION:
			if(storage == MODULE_STOCK_RIGHT){
				if(ELEMENTS_get_flag(FLAG_HARRY_MODULE_COLOR_RIGHT_FINISH)){
					state = TAKE_CYLINDER;
				}else{
					sub_act_harry_mae_prepare_modules_for_dispose(MODULE_STOCK_RIGHT, TRUE); // On lance la préparation d'un cylindre : balancer + mise en position pour la bonne couleur
					state = WAIT_CYLINDER_PREPARATION;
				}
			}else{
				if(ELEMENTS_get_flag(FLAG_HARRY_MODULE_COLOR_LEFT_FINISH)){
					state = TAKE_CYLINDER;
				}else{
					sub_act_harry_mae_prepare_modules_for_dispose(MODULE_STOCK_LEFT, TRUE); // On lance la préparation d'un cylindre : balancer + mise en position pour la bonne couleur
					state = WAIT_CYLINDER_PREPARATION;
				}
			}
			break;

		case WAIT_CYLINDER_PREPARATION :	// Ajouter un timeout
			// On attend la préparation du cylinder (retournement couleur)
			if((storage == MODULE_STOCK_RIGHT && ELEMENTS_get_flag(FLAG_HARRY_MODULE_COLOR_RIGHT_FINISH))
			|| (storage == MODULE_STOCK_LEFT && ELEMENTS_get_flag(FLAG_HARRY_MODULE_COLOR_LEFT_FINISH))){
				state = TAKE_CYLINDER;
			}
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
				state = check_act_status(ACT_QUEUE_Cylinder_arm_right, state, RAISE_CYLINDER, RAISE_CYLINDER);
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
				state = check_act_status(ACT_QUEUE_Cylinder_dispose_right, state, GET_OUT_CYLINDER_OF_ROBOT, GET_OUT_CYLINDER_OF_ROBOT);
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
				state = check_act_status(ACT_QUEUE_Cylinder_arm_right, state, UNFOLD_DISPOSE_SERVO, UNFOLD_DISPOSE_SERVO);
			}
			break;

		case UNFOLD_DISPOSE_SERVO:
			if(entrance){
				if(storage == MODULE_STOCK_RIGHT){
					ACT_push_order(ACT_CYLINDER_DISPOSE_RIGHT, ACT_CYLINDER_DISPOSE_RIGHT_DISPOSE);
				}else{
					ACT_push_order(ACT_CYLINDER_DISPOSE_LEFT, ACT_CYLINDER_DISPOSE_LEFT_DISPOSE);
				}

				// Une autre dépose va suivre, on peut dès à présent lancer la préparation du module suivant
				if(anotherDisposeWillFollow){
					sub_act_harry_mae_prepare_modules_for_dispose(storage, TRUE);
				}
			}

			if(storage == MODULE_STOCK_RIGHT){
				state = check_act_status(ACT_QUEUE_Cylinder_dispose_right, state, GO_TO_DISPOSE_POS, GO_TO_DISPOSE_POS);
			}else{
				state = check_act_status(ACT_QUEUE_Cylinder_dispose_right, state, GO_TO_DISPOSE_POS, GO_TO_DISPOSE_POS);
			}
			break;

		case GO_TO_DISPOSE_POS:
			if(entrance){
				if(storage == MODULE_STOCK_RIGHT){
					ACT_push_order(ACT_CYLINDER_ARM_RIGHT, ACT_CYLINDER_ARM_RIGHT_DISPOSE);
				}else{
					ACT_push_order(ACT_CYLINDER_ARM_LEFT, ACT_CYLINDER_ARM_LEFT_DISPOSE);
				}
			}

			if(storage == MODULE_STOCK_RIGHT){
				state = check_act_status(ACT_QUEUE_Cylinder_arm_right, state, DISPOSE_CYLINDER, DISPOSE_CYLINDER);
			}else{
				state = check_act_status(ACT_QUEUE_Cylinder_arm_right, state, DISPOSE_CYLINDER, DISPOSE_CYLINDER);
			}
			break;

		case DISPOSE_CYLINDER:
			if(entrance){
				if(storage == MODULE_STOCK_RIGHT){
					ACT_push_order(ACT_POMPE_DISPOSE_RIGHT, ACT_POMPE_STOP);
				}else{
					ACT_push_order(ACT_POMPE_DISPOSE_LEFT, ACT_POMPE_STOP);
				}
			}

			// Pas de vérification ici car les pompes retournent toujours vrai
			state = CHOOSE_ARM_STORAGE_POS;
			break;

		case CHOOSE_ARM_STORAGE_POS:
			if(anotherDisposeWillFollow){
				state = S1_MOVE_DISPOSE_SERVO;	// Scénarion 1: On doit positionner le bras pour une autre dépose
			}else{
				state = S2_MOVE_DISPOSE_SERVO;	// Scénario 2: on doit ranger le bras dans le robot
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
				state = check_act_status(ACT_QUEUE_Cylinder_dispose_right, state, S1_MOVE_ARM_SERVO, S1_MOVE_ARM_SERVO);
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
				state = check_act_status(ACT_QUEUE_Cylinder_arm_right, state, DONE, DONE);
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
				state = check_act_status(ACT_QUEUE_Cylinder_dispose_right, state, S2_MOVE_ARM_SERVO, S2_MOVE_ARM_SERVO);
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
				state = check_act_status(ACT_QUEUE_Cylinder_arm_right, state, DONE, DONE);
			}
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
				debug_printf("default case in sub_act_harry_mae_dispose_modules\n");
			break;
	}

	if(storage == MODULE_STOCK_RIGHT){
		stateRight = state;
	}else if(storage == MODULE_STOCK_LEFT){
		stateLeft = state;
	} // else L'erreur a déjà été affichée

	return IN_PROGRESS;
}

