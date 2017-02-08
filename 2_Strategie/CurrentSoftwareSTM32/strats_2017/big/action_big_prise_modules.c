#include "action_big.h"
#include "../../propulsion/movement.h"
#include "../../propulsion/astar.h"
#include "../../QS/QS_stateMachineHelper.h"
#include "../../QS/QS_types.h"

#include  "../../propulsion/astar.h"
#include  "../../utils/generic_functions.h"
#include "../../actuator/act_functions.h"
#include "../../actuator/queue.h"
#include "../../utils/actionChecker.h"
#include "../../elements.h"

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
error_e sub_harry_rocket_multicolor(ELEMENTS_property_e fusee, bool_e right_side){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_PRISE_ROCKET_MULTICOLOR,
				INIT,

				ALL_THE_GET_IN,
				GET_IN_DIRECT,
				GET_IN_AVOID_CRATERE,
				GET_IN_AVOID_START_ZONE,
				GET_IN_FROM_CLOSE_ADV_ZONE,
				GET_IN_FROM_FAR_ADV_ZONE,
				GET_IN_ASTAR,

				FAILED_INIT_ACTION,
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

				ERROR,
				DONE
			);

	moduleDropLocation_e nb_cylinder_big_left= NB_MODULE_BIG_LEFT_SIDE;
	moduleDropLocation_e nb_cylinder_big_right=NB_MODULE_BIG_RIGHT_SIDE;
	moduleDropLocation_e nb_cylinder_fusee=		NB_MODULE_FUSEE_MULTICOLOR;
	//rendre fusee public !
	//initialiser la fusee rempli
	//enlever module fusee

	switch(state){

		case INIT:{
				error_e result = init_all_actionneur(nb_cylinder_big_right, nb_cylinder_big_left); // on appelle une fonction qui verifie la position initiale de tout nos actionneurs
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
			//On active la pompe avant d'avancer
			ACT_push_order( ACT_POMPE_SLIDER_RIGHT , ACT_POMPE_NORMAL );
			ACT_push_order( ACT_CYLINDER_SLIDER_RIGHT , ACT_CYLINDER_SLIDER_RIGHT_OUT );

			// On avance l'autre bras pour bloquer la chute du cylindre d'apres
			ACT_push_order( ACT_CYLINDER_SLIDER_LEFT , ACT_CYLINDER_SLIDER_LEFT_ALMOST_OUT );}

			state = check_act_status(ACT_QUEUE_Cylinder_slider_right, ACTION_GO_TAKE_CYLINDER,CHECK_STATUS_OTHER_SLINDER, FAILED_INIT_ACTION);
		}else{
			if (entrance) {
			ACT_push_order( ACT_POMPE_SLIDER_LEFT , ACT_POMPE_NORMAL);
			ACT_push_order( ACT_CYLINDER_SLIDER_LEFT , ACT_CYLINDER_SLIDER_LEFT_OUT );

			ACT_push_order( ACT_CYLINDER_SLIDER_RIGHT , ACT_CYLINDER_SLIDER_RIGHT_ALMOST_OUT );}

			state = check_act_status(ACT_QUEUE_Cylinder_slider_left, ACTION_GO_TAKE_CYLINDER,CHECK_STATUS_OTHER_SLINDER, FAILED_INIT_ACTION);
		}
		break;

	case CHECK_STATUS_OTHER_SLINDER:
		if (right_side){
			state = check_act_status(ACT_QUEUE_Cylinder_slider_left, CHECK_STATUS_OTHER_SLINDER,PROTECT_NEXT_FALL, FAILED_INIT_ACTION);
		}else{
			state = check_act_status(ACT_QUEUE_Cylinder_slider_right, CHECK_STATUS_OTHER_SLINDER,PROTECT_NEXT_FALL, FAILED_INIT_ACTION);
		}
		break;

	case PROTECT_NEXT_FALL:
		if (right_side){
			if(entrance){
			ACT_push_order( ACT_CYLINDER_SLIDER_RIGHT , ACT_CYLINDER_SLIDER_RIGHT_ALMOST_OUT_WITH_CYLINDER);}
			state = check_act_status(ACT_QUEUE_Cylinder_slider_right, PROTECT_NEXT_FALL,ACTION_BRING_BACK_CYLINDER, FAILED_INIT_ACTION);
		}else{
			if(entrance){
			ACT_push_order( ACT_CYLINDER_SLIDER_LEFT , ACT_CYLINDER_SLIDER_LEFT_ALMOST_OUT_WITH_CYLINDER);}
			state = check_act_status(ACT_QUEUE_Cylinder_slider_left, PROTECT_NEXT_FALL,ACTION_BRING_BACK_CYLINDER, FAILED_INIT_ACTION);
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

			//On redescend le bras et desactive la pompe du cycle precedent
			ACT_push_order( ACT_POMPE_ELEVATOR_LEFT , ACT_POMPE_STOP );
			//vacuose ici?!
			ACT_push_order( ACT_CYLINDER_ELEVATOR_RIGHT , ACT_CYLINDER_ELEVATOR_RIGHT_BOTTOM );}

			//capteur inuctif fin de course a la place du check at status ?
			state = check_act_status(ACT_QUEUE_Cylinder_slider_right, ACTION_BRING_BACK_CYLINDER,CHECK_STATUS_OTHER_SLINDER_2, FAILED_INIT_ACTION);
		}else{
			if (entrance) {
			ACT_push_order( ACT_CYLINDER_SLIDER_LEFT , ACT_CYLINDER_SLIDER_LEFT_IN );

			ACT_push_order( ACT_POMPE_SLIDER_RIGHT , ACT_POMPE_NORMAL );
			//vacuose ici?
			ACT_push_order( ACT_CYLINDER_SLIDER_RIGHT , ACT_CYLINDER_SLIDER_RIGHT_OUT );

			ACT_push_order( ACT_POMPE_ELEVATOR_RIGHT , ACT_POMPE_STOP );
			//vacuose ici?
			ACT_push_order( ACT_CYLINDER_ELEVATOR_RIGHT , ACT_CYLINDER_ELEVATOR_RIGHT_BOTTOM );}

			//capteur inuctif fin de course a la place du check at status ?
			state = check_act_status(ACT_QUEUE_Cylinder_slider_left, ACTION_BRING_BACK_CYLINDER,CHECK_STATUS_OTHER_SLINDER_2, FAILED_INIT_ACTION);
		}break;

	case CHECK_STATUS_OTHER_SLINDER_2:
		if (right_side){
			state = check_act_status(ACT_QUEUE_Cylinder_slider_left, CHECK_STATUS_OTHER_SLINDER_2,CHECK_STATUS_ELEVATOR, FAILED_INIT_ACTION);
		}else{
			state = check_act_status(ACT_QUEUE_Cylinder_slider_right, CHECK_STATUS_OTHER_SLINDER_2,CHECK_STATUS_ELEVATOR, FAILED_INIT_ACTION);
		}
		break;

	case CHECK_STATUS_ELEVATOR:
		if (right_side){
			state = check_act_status(ACT_QUEUE_Cylinder_elevator_left, CHECK_STATUS_ELEVATOR,ACTION_START_BRING_UP_CYLINDER, FAILED_INIT_ACTION);
		}else{
			state = check_act_status(ACT_QUEUE_Cylinder_elevator_right, CHECK_STATUS_ELEVATOR,ACTION_START_BRING_UP_CYLINDER, FAILED_INIT_ACTION);
		}
		break;

	case ACTION_START_BRING_UP_CYLINDER:

		if (right_side){
			if (entrance) {
			ACT_push_order( ACT_POMPE_ELEVATOR_RIGHT , ACT_POMPE_NORMAL );
			ACT_push_order( ACT_POMPE_SLIDER_RIGHT , ACT_POMPE_STOP );
			// ici rajouter le check de la pompe avec vacuose pour les deux
			ACT_push_order( ACT_CYLINDER_ELEVATOR_RIGHT , ACT_CYLINDER_ELEVATOR_RIGHT_LOCK_WITH_CYLINDER );

			// on redescend en meme temps le clapet du cylindre precedent
			ACT_push_order( ACT_CYLINDER_SLOPE_LEFT , ACT_CYLINDER_SLOPE_LEFT_UNLOCK );}

			//la on doit actionner le slider du cote de l'elevator pour qu'il parte en ALMOST_OUT
			//le plus efficace serait-il d'utiliser Wait() plutot que de creer un nouveau case pour separer la monter en deux ? !!!
			//Attention le wait peut etre dangereux, mechaniquement, sile sider avance trop tot
			state = check_act_status(ACT_QUEUE_Cylinder_elevator_right, ACTION_START_BRING_UP_CYLINDER,CHECK_STATUS_SLOPE, FAILED_INIT_ACTION);
		}else{
			if (entrance) {
			ACT_push_order( ACT_POMPE_ELEVATOR_LEFT , ACT_POMPE_NORMAL );
			ACT_push_order( ACT_POMPE_SLIDER_LEFT , ACT_POMPE_STOP );
			// ici rajouter le check de la pompe avec vacuose pour les deux
			ACT_push_order( ACT_CYLINDER_ELEVATOR_LEFT , ACT_CYLINDER_ELEVATOR_LEFT_LOCK_WITH_CYLINDER );
			ACT_push_order( ACT_CYLINDER_SLOPE_RIGHT , ACT_CYLINDER_SLOPE_RIGHT_UNLOCK );}
			state = check_act_status(ACT_QUEUE_Cylinder_elevator_left, ACTION_START_BRING_UP_CYLINDER,CHECK_STATUS_SLOPE, FAILED_INIT_ACTION);
		}break;

	case CHECK_STATUS_SLOPE:
		if (right_side){
			state = check_act_status(ACT_QUEUE_Cylinder_slope_left, CHECK_STATUS_SLOPE,ACTION_END_BRING_UP_CYLINDER, FAILED_INIT_ACTION);
		}else{
			state = check_act_status(ACT_QUEUE_Cylinder_slope_right, CHECK_STATUS_SLOPE,ACTION_END_BRING_UP_CYLINDER, FAILED_INIT_ACTION);
		}
		break;

	case ACTION_END_BRING_UP_CYLINDER:

		if (right_side){
			if (entrance) {
			//on finit le mouvement en simultanee avec la sortie du slider
			ACT_push_order( ACT_CYLINDER_SLIDER_RIGHT , ACT_CYLINDER_SLIDER_RIGHT_ALMOST_OUT );
			ACT_push_order( ACT_CYLINDER_ELEVATOR_RIGHT , ACT_CYLINDER_ELEVATOR_RIGHT_TOP );}
			state = check_act_status(ACT_QUEUE_Cylinder_elevator_right, ACTION_END_BRING_UP_CYLINDER,CHECK_STATUS_SLIDER, FAILED_INIT_ACTION);
		}else{
			if (entrance) {
			ACT_push_order( ACT_CYLINDER_SLIDER_LEFT , ACT_CYLINDER_SLIDER_LEFT_ALMOST_OUT );
			ACT_push_order( ACT_CYLINDER_ELEVATOR_LEFT , ACT_CYLINDER_ELEVATOR_LEFT_TOP );}
			state = check_act_status(ACT_QUEUE_Cylinder_elevator_left, ACTION_END_BRING_UP_CYLINDER,CHECK_STATUS_SLIDER, FAILED_INIT_ACTION);
		}break;

	case CHECK_STATUS_SLIDER:
		if (right_side){
			state = check_act_status(ACT_QUEUE_Cylinder_slider_right, CHECK_STATUS_SLIDER,ACTION_STOCK_UP_CYLINDER, FAILED_INIT_ACTION);
		}else{
			state = check_act_status(ACT_QUEUE_Cylinder_slider_left, CHECK_STATUS_SLIDER,ACTION_STOCK_UP_CYLINDER, FAILED_INIT_ACTION);
		}
		break;

	case ACTION_STOCK_UP_CYLINDER:

		if (right_side){
			if (entrance) {
			// on verouille le slope
			ACT_push_order( ACT_CYLINDER_SLOPE_RIGHT , ACT_CYLINDER_SLOPE_RIGHT_LOCK );

			//et en meme temps on prepare la collecte d'apres
			ACT_push_order( ACT_CYLINDER_SLIDER_LEFT , ACT_CYLINDER_SLIDER_LEFT_ALMOST_OUT_WITH_CYLINDER );}

			state = check_act_status(ACT_QUEUE_Cylinder_slope_right, ACTION_STOCK_UP_CYLINDER, CHECK_STATUS_OTHER_SLINDER_3, FAILED_INIT_ACTION);

		}else{
			if (entrance) {
			ACT_push_order( ACT_CYLINDER_SLOPE_LEFT , ACT_CYLINDER_SLOPE_LEFT_LOCK );
			ACT_push_order( ACT_CYLINDER_SLIDER_RIGHT , ACT_CYLINDER_SLIDER_RIGHT_ALMOST_OUT_WITH_CYLINDER );}
			state = check_act_status(ACT_QUEUE_Cylinder_slope_right, ACTION_STOCK_UP_CYLINDER, CHECK_STATUS_OTHER_SLINDER_3, FAILED_INIT_ACTION);}
		break;

	case CHECK_STATUS_OTHER_SLINDER_3:
		if (right_side){
			state = check_act_status(ACT_QUEUE_Cylinder_slider_left, CHECK_STATUS_OTHER_SLINDER_3,RESTART_CONDITION, FAILED_INIT_ACTION);
		}else{
			state = check_act_status(ACT_QUEUE_Cylinder_slider_right, CHECK_STATUS_OTHER_SLINDER_3,RESTART_CONDITION, FAILED_INIT_ACTION);
		}
		break;

	case RESTART_CONDITION:
		//subModule(MODULE_POLY,nb_cylinder_fusee);
		if ((getNbDrop(nb_cylinder_fusee)>0)){
			if (right_side){
				//on actualise le nombre de cylindre stocke dans le robot
				addModule(MODULE_POLY,nb_cylinder_big_right);

				//On cahnge de cote
				right_side = FALSE;

				//et on recommence
				state = ACTION_BRING_BACK_CYLINDER;
			}else{
				addModule(MODULE_POLY,nb_cylinder_big_left);
				right_side = TRUE;
				state = ACTION_BRING_BACK_CYLINDER;}
		}else{
			state=ACTION_RETRACT_ALL;}
			break;

		case ACTION_RETRACT_ALL:{
			error_e result = init_all_actionneur(nb_cylinder_big_right, nb_cylinder_big_left);
			if(result == END_OK){   state=DONE;
			}else{                  state=FAILED_INIT_ACTION;
			}}break;

		case FAILED_INIT_ACTION:
			//stop?
			state = ERROR;
			break;

		case DONE:
			break;

		case ERROR:
			break;
		}

		return IN_PROGRESS;

}

error_e init_all_actionneur(moduleDropLocation_e nb_cylinder_big_right,moduleDropLocation_e nb_cylinder_big_left){
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
			ACT_push_order(ACT_CYLINDER_SLIDER_RIGHT, ACT_CYLINDER_SLIDER_RIGHT_IN);}
			if ((getNbDrop(nb_cylinder_big_right)<5)){
				state= check_act_status(ACT_QUEUE_Cylinder_slider_right, INIT_ACTION_SLIDER_RIGHT, INIT_ACTION_ELEVATOR_RIGHT, ERROR);
			}else{
				state= check_act_status(ACT_QUEUE_Cylinder_slider_right, INIT_ACTION_SLIDER_RIGHT, INIT_ACTION_SLIDER_LEFT, ERROR);
			}
			break;

		case INIT_ACTION_ELEVATOR_RIGHT:
			if(entrance){
			ACT_push_order(ACT_CYLINDER_ELEVATOR_RIGHT, ACT_CYLINDER_SLIDER_RIGHT_OUT);}
			if ((getNbDrop(nb_cylinder_big_right)<4)){
				state= check_act_status(ACT_QUEUE_Cylinder_elevator_right, INIT_ACTION_ELEVATOR_RIGHT, INIT_ACTION_SLOPE_RIGHT, ERROR);
			}else{
				state= check_act_status(ACT_QUEUE_Cylinder_elevator_right, INIT_ACTION_ELEVATOR_RIGHT, INIT_ACTION_SLIDER_LEFT, ERROR);
			}
			break;

		case INIT_ACTION_SLOPE_RIGHT:
			if(entrance){
				ACT_push_order(ACT_CYLINDER_SLOPE_RIGHT, ACT_CYLINDER_SLOPE_RIGHT_UNLOCK);}
			state= check_act_status(ACT_QUEUE_Cylinder_slope_right, INIT_ACTION_SLOPE_RIGHT, INIT_ACTION_SLIDER_LEFT, ERROR);
			break;

		case INIT_ACTION_SLIDER_LEFT:
			if(entrance){
			ACT_push_order(ACT_CYLINDER_SLIDER_LEFT, ACT_CYLINDER_SLIDER_LEFT_IN);}
			if ((getNbDrop(nb_cylinder_big_left)<5)){
				state= check_act_status(ACT_QUEUE_Cylinder_slider_left, INIT_ACTION_SLIDER_LEFT, INIT_ACTION_ELEVATOR_LEFT, ERROR);
			}else{
				state= check_act_status(ACT_QUEUE_Cylinder_slider_left, INIT_ACTION_SLIDER_LEFT, DONE, ERROR);
			}
			break;

		case INIT_ACTION_ELEVATOR_LEFT:
			if(entrance){
			ACT_push_order(ACT_CYLINDER_ELEVATOR_LEFT, ACT_CYLINDER_ELEVATOR_LEFT_BOTTOM);}
			if ((getNbDrop(nb_cylinder_big_left)<4)){
				state= check_act_status(ACT_QUEUE_Cylinder_elevator_left, INIT_ACTION_ELEVATOR_LEFT, INIT_ACTION_SLOPE_LEFT, ERROR);
			}else{
				state= check_act_status(ACT_QUEUE_Cylinder_elevator_left, INIT_ACTION_ELEVATOR_LEFT, DONE, ERROR);
			}
			break;

		case INIT_ACTION_SLOPE_LEFT:
			if(entrance){
			ACT_push_order(ACT_CYLINDER_SLOPE_LEFT, ACT_CYLINDER_SLOPE_LEFT_UNLOCK);}
			state= check_act_status(ACT_QUEUE_Cylinder_slope_left, INIT_ACTION_SLOPE_LEFT, DONE, ERROR);
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
