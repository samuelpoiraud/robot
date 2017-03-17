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

//permet de régler la distance entre la base côté et le robot(largeurBase+distance):
#define DISTANCE_BASE_SIDE_ET_ROBOT	((Uint16) 120+200)

// Fonctions privées
error_e sub_harry_get_in_our_side_depose_module_centre();
error_e sub_harry_get_in_middle_adv_side_depose_module_centre();
error_e sub_harry_get_in_adv_side_depose_module_centre();
error_e sub_harry_get_in_middle_our_side_depose_module_centre();


error_e sub_harry_depose_modules_centre(ELEMENTS_property_e modules, ELEMENTS_side_e robot_side, ELEMENTS_side_match_e basis_side){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_DEPOSE_MODULES_CENTRE,
			INIT,
			GET_IN,

			ERROR,
			DONE
		);

	switch(state){
		case INIT:
			state=GET_IN;
			break;

		case GET_IN:
			//state=check_sub_action_result(sub_harry_get_in_depose_modules_centre(modules, basis_side), state, GO_TO_DEPOSE_MODULE, ERROR);
			state = DONE;
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
				debug_printf("default case in sub_harry_depose_modules_centre\n");
			break;
	}

	return IN_PROGRESS;
}


error_e sub_harry_get_in_depose_modules_centre(ELEMENTS_property_e modules, ELEMENTS_side_match_e basis_side){
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

	switch(state){
		case INIT:
			if((modules == OUR_ELEMENT) && (basis_side == OUR_SIDE)){
				state = GET_IN_POS_1;
			}else if((modules == OUR_ELEMENT) && (basis_side == ADV_SIDE)){
				state = GET_IN_POS_2;
			}else if((modules == NEUTRAL_ELEMENT) && (basis_side == OUR_SIDE)){
				state = GET_IN_POS_3;
			}else if((modules == NEUTRAL_ELEMENT) && (basis_side == ADV_SIDE)){
				state = GET_IN_POS_4;
			}else if((modules == ADV_ELEMENT) && (basis_side == OUR_SIDE)){
				state = GET_IN_POS_5;
			}else if((modules == ADV_ELEMENT) && (basis_side == ADV_SIDE)){
				state = GET_IN_POS_6;
			}else
				state = ERROR;
			break;

		case GET_IN_POS_1:
			state = check_sub_action_result(sub_harry_get_in_pos_1_depose_module_centre(), state, DONE, ERROR);
			break;

		case GET_IN_POS_2:
			state = check_sub_action_result(sub_harry_get_in_pos_2_depose_module_centre(), state, DONE, ERROR);
			break;

		case GET_IN_POS_3:
			state = check_sub_action_result(sub_harry_get_in_pos_3_depose_module_centre(), state, DONE, ERROR);
			break;

		case GET_IN_POS_4:
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


error_e sub_harry_get_in_pos_1_depose_module_centre(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_GET_IN_POS_1_DEPOSE_MODULES_CENTRE,
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
			if(i_am_in_square(800, 1400, 300, 900)){
				state = GET_IN_FROM_OUR_SQUARE;
			}else if(i_am_in_square(200, 1100, 900, 2100)){
				state = GET_IN_FROM_MIDDLE_SQUARE;
			}else if(i_am_in_square(800, 1400, 2100, 2700)){
				state = GET_IN_FROM_ADV_SQUARE;
			}else{
				state = PATHFIND;
			}
			break;

		case GET_IN_FROM_OUR_SQUARE:
			state = try_going(1450, COLOR_Y(600), state, DONE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT,END_AT_BRAKE);
			break;

		case GET_IN_FROM_MIDDLE_SQUARE:
			state = try_going(1000, COLOR_Y(1000), state, GET_IN_FROM_OUR_SQUARE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT,END_AT_BRAKE);
			break;

		case GET_IN_FROM_ADV_SQUARE:
			state = try_going(900, COLOR_Y(2000), state, GET_IN_FROM_MIDDLE_SQUARE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT,END_AT_BRAKE);
			break;

		case PATHFIND:
			state = ASTAR_try_going(1450, COLOR_Y(600), state, DONE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_BRAKE);
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
			PATHFIND,
			ERROR,
			DONE
		);

	switch(state){
		case INIT:
			if((i_am_in_square(800, 1400, 300, 900))||(i_am_in_square(200, 1100, 900, 2100))){
				state = GET_IN_FROM_OUR_SQUARE;
			//}else if(i_am_in_square(200, 1100, 900, 2100)){
			//	state = GET_IN_FROM_MIDDLE_SQUARE;
			}else if(i_am_in_square(800, 1400, 2100, 2700)){
				state = GET_IN_FROM_ADV_SQUARE;
			}else{
				state = PATHFIND;
			}
			break;

		case GET_IN_FROM_OUR_SQUARE:
			state = try_going(925, COLOR_Y(1100), state, DONE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT,END_AT_BRAKE);
			break;

		case GET_IN_FROM_MIDDLE_SQUARE:
			state = try_going(1000, COLOR_Y(1000), state, GET_IN_FROM_OUR_SQUARE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT,END_AT_BRAKE);
			break;

		case GET_IN_FROM_ADV_SQUARE:
			state = try_going(825, COLOR_Y(2000), state, GET_IN_FROM_OUR_SQUARE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT,END_AT_BRAKE);
			break;

		case PATHFIND:
			state = ASTAR_try_going(925, COLOR_Y(1100), state, DONE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_BRAKE);
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
			GET_IN_FROM_MIDDLE_SQUARE,
			GET_IN_FROM_ADV_SQUARE,
			PATHFIND,
			ERROR,
			DONE
		);

	switch(state){
		case INIT:
			if((i_am_in_square(800, 1400, 300, 900))||(i_am_in_square(200, 1100, 900, 2100))){
				state = GET_IN_FROM_OUR_SQUARE;
			//}else if(i_am_in_square(200, 1100, 900, 2100)){
			//	state = GET_IN_FROM_MIDDLE_SQUARE;
			}else if(i_am_in_square(800, 1400, 2100, 2700)){
				state = GET_IN_FROM_ADV_SQUARE;
			}else{
				state = PATHFIND;
			}
			break;

		case GET_IN_FROM_OUR_SQUARE:
			state = try_going(975, COLOR_Y(1300), state, DONE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT,END_AT_BRAKE);
			break;

		case GET_IN_FROM_MIDDLE_SQUARE:
			state = try_going(1000, COLOR_Y(1250), state, GET_IN_FROM_OUR_SQUARE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT,END_AT_BRAKE);
			break;

		case GET_IN_FROM_ADV_SQUARE:
			state = try_going(825, COLOR_Y(2000), state, GET_IN_FROM_OUR_SQUARE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT,END_AT_BRAKE);
			break;

		case PATHFIND:
			state = ASTAR_try_going(975, COLOR_Y(1300), state, DONE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_BRAKE);
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

error_e sub_harry_get_in_pos_4_depose_module_centre(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_GET_IN_POS_4_DEPOSE_MODULES_CENTRE,
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
			if(i_am_in_square(800, 1400, 300, 900)){
				state = GET_IN_FROM_OUR_SQUARE;
		//	}else if(i_am_in_square(200, 1100, 900, 2100)){
		//		state = GET_IN_FROM_MIDDLE_SQUARE;
			}else if((i_am_in_square(800, 1400, 2100, 2700))||(i_am_in_square(200, 1100, 900, 2100))){
				state = GET_IN_FROM_ADV_SQUARE;
			}else{
				state = PATHFIND;
			}
			break;

		case GET_IN_FROM_OUR_SQUARE:
			state = try_going(975, COLOR_Y(1700), state, GET_IN_FROM_ADV_SQUARE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT,END_AT_BRAKE);
			break;

		case GET_IN_FROM_MIDDLE_SQUARE:
			state = try_going(1000, COLOR_Y(1750), state, GET_IN_FROM_ADV_SQUARE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT,END_AT_BRAKE);
			break;

		case GET_IN_FROM_ADV_SQUARE:
			state = try_going(825, COLOR_Y(1900), state, DONE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT,END_AT_BRAKE);
			break;

		case PATHFIND:
			state = ASTAR_try_going(925, COLOR_Y(1700), state, DONE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_BRAKE);
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
			GET_IN_FROM_ADV_SQUARE,
			PATHFIND,
			ERROR,
			DONE
		);

	switch(state){
		case INIT:
			if(i_am_in_square(800, 1400, 300, 900)){
				state = GET_IN_FROM_OUR_SQUARE;
		//	}else if(i_am_in_square(200, 1100, 900, 2100)){
		//		state = GET_IN_FROM_MIDDLE_SQUARE;
			}else if((i_am_in_square(800, 1400, 2100, 2700))||(i_am_in_square(200, 1100, 900, 2100))){
				state = GET_IN_FROM_ADV_SQUARE;
			}else{
				state = PATHFIND;
			}
			break;

		case GET_IN_FROM_OUR_SQUARE:
			state = try_going(825, COLOR_Y(1000), state, GET_IN_FROM_ADV_SQUARE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT,END_AT_BRAKE);
			break;

		case GET_IN_FROM_MIDDLE_SQUARE:
			state = try_going(1000, COLOR_Y(1000), state, GET_IN_FROM_OUR_SQUARE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT,END_AT_BRAKE);
			break;

		case GET_IN_FROM_ADV_SQUARE:
			state = try_going(925, COLOR_Y(1900), state, DONE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT,END_AT_BRAKE);
			break;

		case PATHFIND:
			state = ASTAR_try_going(925, COLOR_Y(1900), state, DONE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_BRAKE);
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
			if(i_am_in_square(800, 1400, 300, 900)){
				state = GET_IN_FROM_OUR_SQUARE;
			}else if(i_am_in_square(200, 1100, 900, 2100)){
				state = GET_IN_FROM_MIDDLE_SQUARE;
			}else if(i_am_in_square(800, 1400, 2100, 2700)){
				state = GET_IN_FROM_ADV_SQUARE;
			}else{
				state = PATHFIND;
			}
			break;

		case GET_IN_FROM_OUR_SQUARE:
			state = try_going(900, COLOR_Y(1000), state, GET_IN_FROM_MIDDLE_SQUARE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT,END_AT_BRAKE);
			break;

		case GET_IN_FROM_MIDDLE_SQUARE:
			state = try_going(1000, COLOR_Y(2000), state, GET_IN_FROM_ADV_SQUARE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT,END_AT_BRAKE);
			break;

		case GET_IN_FROM_ADV_SQUARE:
			state = try_going(1450, COLOR_Y(2400), state, DONE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT,END_AT_BRAKE);
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



error_e sub_harry_depose_modules_side(ELEMENTS_property_e modules,ELEMENTS_side_e robot_side, ELEMENTS_side_match_e basis_side){
	//liste de ce que je dois faire:
		//gérer les actionneurs pour la dépose
		//DETERMINE_NB_CYLINDRE_SUR_BASE_DESCENDRE_BRAS ACT_push_order...
		//VERIFICATION_ACTIONNEUR a faire!!!!
		//faire Procedure de depose

	/*
	 * Tu peux créer une autre sub pour tes get_in/déplacements
	 *
	 * Pour voir si tu as un module, tu utilises la fonction STOCKS_moduleStockPlaceIsEmpty(STOCK_POS_ENTRY, param );
	 * Si ça t'intéresse elle est utilisée dans le fichier action_big_act_modules.c dans la sub sub_act_harry_mae_modules()
	 *
	 * Autre commentaire : line 455
	 */

	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_DEPOSE_MODULES_SIDE,
			INIT,
			ERROR,
			DONE,

			LES_SQUARE_COLOR,
			//NORD_1,
			//NORD_EST_1,
			//NORD_EST_2,
			//SUD_EST_1,
			//SUD_EST_2,
			//SUD_EST_3,
			//SUD_OUEST_1,
			//NORD_OUEST_1,
			PATHFIND,
			FIN_DEPLACEMENTS,


			//DETERMINE_NB_CYLINDRE_SUR_BASE,
			//DETERMINE_NB_CYLINDRE_SUR_BASE_ROTATION,
			DETERMINE_NB_CYLINDRE_SUR_BASE_DESCENDRE_BRAS,
			DETERMINE_NB_CYLINDRE_SUR_BASE_AVANCER,
			DETERMINE_NB_CYLINDRE,
			DETERMINE_NB_CYLINDRE_SUR_BASE_REMONTER_BRAS,
			DETERMINE_NB_CYLINDRE_SUR_BASE_REPLACEMENT,


		//VERIFIER_COULEUR_CYLINDRE_DEJA_SUR_BASE:
			SI_4_CYLINDRES,//A revoir car on ne peut plus tourner les cylindres...
			SI_3_CYLINDRES,
			SI_2_CYLINDRES,
			SI_1_CYLINDRES,
			SI_0_REPLACEMENT,

		//Maintenant on peut ajouter des cylindres:    ENFIN!
			CHOIX_DE_COTE_DEPOSE,


//Procedure de depose partie comune à l'ouset et l'est:
			PREPARATION_NEXT,
			ROTATION_MODULE,
			TAKE_MODULE,
			POMPE_ACTIVATION,
			DROP_MODULE,
			POMPE_STOP,

//Procedure de depose OUEST:

			PREPARATION_PUSHER_OUEST,
			PUSHER_DOWN_OUEST,
			PUSH_MODULES_OUEST,
			PUSHER_UP_OUEST,
			PUSH_MODULES_RETURN_TO_POSITION_OUEST,

			//DROP_PUT_AWAY_OUEST,

		//Procedure de depose EST:

			PREPARATION_PUSHER_EST,
			PUSHER_DOWN_EST,
			PUSH_MODULES_EST,
			PUSHER_UP_EST,
			PUSH_MODULES_RETURN_TO_POSITION_EST,

			//DROP_PUT_AWAY_EST,



		//Procédure de fin de sub!!!!
			//VERIFICATION_ACTIONNEUR,

			FIN_TENTATIVE_SUD,
			FIN_TENTATIVE_NORD,
			POINT_DE_ROTATION1,
			POINT_DE_ROTATION2,
			POINT_DE_ROTATION3
		);

	//MES VARIABLES:
	Uint8 nbCylindresSurBase = 0;
	typedef enum{
	   OUEST = 0,
	   EST = 1
	}endroit_depose_config_e;
	endroit_depose_config_e endroit_depose;
	//

	typedef enum{
		NORD = 0,
		SUD = 1
	}POINT_DE_ROTATION_config_e;
	POINT_DE_ROTATION_config_e mon_point_de_rotation;


	typedef enum{
		DROITE = 0,
		GAUCHE = 1
	}COTE_DE_DEPOSE_config_e;
	COTE_DE_DEPOSE_config_e cote_depose;

	mon_point_de_rotation = SUD;
	endroit_depose = OUEST;
	cote_depose = DROITE;

	switch(state){
		// Vérifier la présance de cylindre dans le robot... je sais pas faire... nul!

		case INIT:
			if(entrance){
				// Vérifier la présance de cylindre dans le robot:
				if(robot_side==LEFT){
					if(STOCKS_isEmpty(MODULE_STOCK_LEFT)){
						state = DONE;
					}
				}
				if(robot_side==RIGHT){
					if(STOCKS_isEmpty(MODULE_STOCK_RIGHT)){
						state = DONE;
					}
				}

				if(global.color==BLUE){
					if(basis_side==OUR_SIDE){
						endroit_depose = OUEST;
					}
					else {
						endroit_depose = EST;
					}
				}
				else {
					if(basis_side==OUR_SIDE){
						endroit_depose = EST;
					}
					else {
						endroit_depose = OUEST;
					}
				}
			}

			//test pour avoir un point de départ:
			state = try_going(900,COLOR_Y(1500),state,LES_SQUARE_COLOR,ERROR,FAST,ANY_WAY,DODGE_AND_WAIT,END_AT_LAST_POINT);
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


		case LES_SQUARE_COLOR:
			/*
			if (i_am_in_square_color(0,1150,750,2250)){
				state = NORD_1;
			}

			else if (i_am_in_square_color(350,1150,2250,3000)){
				state = NORD_EST_1;
			}

			else if (i_am_in_square_color(1150,1400,2000,3000)){
				state = SUD_EST_1;
			}

			else if (i_am_in_square_color(1150,1400,0,1000)){
				state = SUD_OUEST_1;
			}

			else if (i_am_in_square_color(350,1150,0,750)){
				state = NORD_OUEST_1;
			}

			else{
				state=PATHFIND;
			}*/
			state=PATHFIND;
			break;



//DEPLACEMENTS:
			/*
			case NORD_1:
				state = try_going(1000,COLOR_Y(750),NORD_1,FIN_DEPLACEMENTS,ERROR,FAST,ANY_WAY,NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
				break;

			case NORD_EST_1:
				state = try_going(1000,COLOR_Y(2250),NORD_EST_1,NORD_EST_2,ERROR,FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
				break;
			case NORD_EST_2:
				state = try_going(1000,COLOR_Y(750),NORD_EST_2,FIN_DEPLACEMENTS,ERROR,FAST,ANY_WAY,NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
				break;

			case SUD_EST_1:
				state = try_going(1150,COLOR_Y(2250),SUD_EST_1,SUD_EST_2,ERROR,FAST,ANY_WAY,NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
				break;
			case SUD_EST_2:
				state = try_going(1000,COLOR_Y(1500),SUD_EST_2,SUD_EST_3,ERROR,FAST,ANY_WAY,NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
				break;
			case SUD_EST_3:
				state = try_going(1000,COLOR_Y(750),SUD_EST_3,FIN_DEPLACEMENTS,ERROR,FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
				break;

			case SUD_OUEST_1:
				state = try_going(1150,COLOR_Y(450),SUD_OUEST_1,FIN_DEPLACEMENTS,ERROR,FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
				break;

			case NORD_OUEST_1:
				state = try_going(1150,COLOR_Y(450),NORD_OUEST_1,FIN_DEPLACEMENTS,ERROR,FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
				break;
			*/

			case PATHFIND:
				if(endroit_depose==OUEST){
					if(mon_point_de_rotation==SUD){
						state=ASTAR_try_going(1250,DISTANCE_BASE_SIDE_ET_ROBOT,PATHFIND,FIN_DEPLACEMENTS,ERROR,FAST,ANY_WAY,NO_DODGE_AND_WAIT,END_AT_LAST_POINT);
					}
					else {
						state=ASTAR_try_going(650,DISTANCE_BASE_SIDE_ET_ROBOT,PATHFIND,FIN_DEPLACEMENTS,ERROR,FAST,ANY_WAY,NO_DODGE_AND_WAIT,END_AT_LAST_POINT);
					}
				}
				else{
					if(mon_point_de_rotation==SUD){
						state=ASTAR_try_going(1250,3000-DISTANCE_BASE_SIDE_ET_ROBOT,PATHFIND,FIN_DEPLACEMENTS,ERROR,FAST,ANY_WAY,NO_DODGE_AND_WAIT,END_AT_LAST_POINT);
					}
					else {
						state=ASTAR_try_going(650,3000-DISTANCE_BASE_SIDE_ET_ROBOT,PATHFIND,FIN_DEPLACEMENTS,ERROR,FAST,ANY_WAY,NO_DODGE_AND_WAIT,END_AT_LAST_POINT);
					}
				}
				break;

			case FIN_DEPLACEMENTS:
				if(endroit_depose==OUEST){
					if(mon_point_de_rotation==SUD){
						state = try_going(1250,DISTANCE_BASE_SIDE_ET_ROBOT,FIN_DEPLACEMENTS,DETERMINE_NB_CYLINDRE_SUR_BASE_AVANCER,ERROR,FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
					}
					else {
						state = try_going(650,DISTANCE_BASE_SIDE_ET_ROBOT,FIN_DEPLACEMENTS,DETERMINE_NB_CYLINDRE_SUR_BASE_AVANCER,ERROR,FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
					}
				}
				else {
					if(mon_point_de_rotation==SUD){
						state = try_going(1250,3000-DISTANCE_BASE_SIDE_ET_ROBOT,FIN_DEPLACEMENTS,DETERMINE_NB_CYLINDRE_SUR_BASE_AVANCER,ERROR,FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
					}
					else {
						state = try_going(650,3000-DISTANCE_BASE_SIDE_ET_ROBOT,FIN_DEPLACEMENTS,DETERMINE_NB_CYLINDRE_SUR_BASE_AVANCER,ERROR,FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
					}
				}
				break;

/*
//Determiner le nombre de cylindre:
			case DETERMINE_NB_CYLINDRE_SUR_BASE_ROTATION:
				//Rotation vers PI:
				state = try_go_angle(PI4096, DETERMINE_NB_CYLINDRE_SUR_BASE_ROTATION, DETERMINE_NB_CYLINDRE_SUR_BASE_DESCENDRE_BRAS, ERROR, FAST, ANY_WAY, END_AT_LAST_POINT);
				break;
*/

			case DETERMINE_NB_CYLINDRE_SUR_BASE_DESCENDRE_BRAS:
				if(endroit_depose==OUEST){
					if(mon_point_de_rotation==SUD){
						//Descendre bras poussoir gauche;
					}
					else{
						//Descendre bras poussoir droit;
					}
				}
				else{
					if(mon_point_de_rotation==SUD){
						//Descendre bras poussoir droit;
					}
					else{
						//Descendre bras poussoir gauche;
					}
				}
				state = DETERMINE_NB_CYLINDRE_SUR_BASE_AVANCER;
				break;

			case DETERMINE_NB_CYLINDRE_SUR_BASE_AVANCER:
				//Avancer doucement jusqu'a bloquage:
				if(endroit_depose==OUEST){
					if(mon_point_de_rotation==SUD){
						state = try_rush(600,DISTANCE_BASE_SIDE_ET_ROBOT,DETERMINE_NB_CYLINDRE_SUR_BASE_AVANCER,DETERMINE_NB_CYLINDRE,DETERMINE_NB_CYLINDRE,FORWARD,NO_DODGE_AND_WAIT,TRUE);
					}
					else {
						state = try_rush(1250,DISTANCE_BASE_SIDE_ET_ROBOT,DETERMINE_NB_CYLINDRE_SUR_BASE_AVANCER,DETERMINE_NB_CYLINDRE,DETERMINE_NB_CYLINDRE,FORWARD,NO_DODGE_AND_WAIT,TRUE);
					}
				}
				else {
					if(mon_point_de_rotation==SUD){
						state = try_rush(600,3000-DISTANCE_BASE_SIDE_ET_ROBOT,DETERMINE_NB_CYLINDRE_SUR_BASE_AVANCER,DETERMINE_NB_CYLINDRE,DETERMINE_NB_CYLINDRE,FORWARD,NO_DODGE_AND_WAIT,TRUE);
					}
					else {
						state = try_rush(1250,3000-DISTANCE_BASE_SIDE_ET_ROBOT,DETERMINE_NB_CYLINDRE_SUR_BASE_AVANCER,DETERMINE_NB_CYLINDRE,DETERMINE_NB_CYLINDRE,FORWARD,NO_DODGE_AND_WAIT,TRUE);
					}
				}
				break;

			case DETERMINE_NB_CYLINDRE:
				if(mon_point_de_rotation==SUD){
					if (i_am_in_square_color(550,750,0,3000)){
						nbCylindresSurBase = 0;
						}
					else if (i_am_in_square_color(750,850,0,3000)){
						nbCylindresSurBase = 1;
						}
					else if (i_am_in_square_color(850,950,0,3000)){
						nbCylindresSurBase = 2;
						}
					else if (i_am_in_square_color(950,1050,0,3000)){
						nbCylindresSurBase = 3;
						}
					else if (i_am_in_square_color(1050,1150,0,3000)){
						nbCylindresSurBase = 4;
						}
					else{
						//ERROR
						state = ERROR;
					}
				}
				else {
					if (i_am_in_square_color(550,750,0,3000)){
						nbCylindresSurBase = 4;
						}
					else if (i_am_in_square_color(750,850,0,3000)){
						nbCylindresSurBase = 3;
						}
					else if (i_am_in_square_color(850,950,0,3000)){
						nbCylindresSurBase = 2;
						}
					else if (i_am_in_square_color(950,1050,0,3000)){
						nbCylindresSurBase = 1;
						}
					else if (i_am_in_square_color(1050,1150,0,3000)){
						nbCylindresSurBase = 0;
						}
					else{
						//ERROR
						state = ERROR;
					}
				}
				state = DETERMINE_NB_CYLINDRE_SUR_BASE_REMONTER_BRAS;
				break;

			case DETERMINE_NB_CYLINDRE_SUR_BASE_REMONTER_BRAS:
				if(endroit_depose==OUEST){
					if(mon_point_de_rotation==SUD){
						//MONTER bras poussoir gauche;
					}
					else{
						//MONTER bras poussoir droit;
					}
				}
				else{
					if(mon_point_de_rotation==SUD){
						//MONTER bras poussoir droit;
					}
					else{
						//MONTER bras poussoir gauche;
					}
				}
				state = DETERMINE_NB_CYLINDRE_SUR_BASE_REPLACEMENT;
				break;


			case DETERMINE_NB_CYLINDRE_SUR_BASE_REPLACEMENT:
				if(endroit_depose==OUEST){
					if(mon_point_de_rotation==SUD){
						if (nbCylindresSurBase == 4){
							state = try_going(1000,DISTANCE_BASE_SIDE_ET_ROBOT,DETERMINE_NB_CYLINDRE_SUR_BASE_REPLACEMENT,SI_4_CYLINDRES,ERROR,FAST,BACKWARD,DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
						}
						else if(nbCylindresSurBase == 3){
							state = try_going(900,DISTANCE_BASE_SIDE_ET_ROBOT,DETERMINE_NB_CYLINDRE_SUR_BASE_REPLACEMENT,SI_3_CYLINDRES,ERROR,FAST,BACKWARD,DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
						}
						else if(nbCylindresSurBase == 2){
							state = try_going(800,DISTANCE_BASE_SIDE_ET_ROBOT,DETERMINE_NB_CYLINDRE_SUR_BASE_REPLACEMENT,SI_2_CYLINDRES,ERROR,FAST,BACKWARD,DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
						}
						else if(nbCylindresSurBase == 1){
							state = try_going(700,DISTANCE_BASE_SIDE_ET_ROBOT,DETERMINE_NB_CYLINDRE_SUR_BASE_REPLACEMENT,SI_1_CYLINDRES,ERROR,FAST,BACKWARD,DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
						}
						else if(nbCylindresSurBase == 0){
							state = SI_0_REPLACEMENT;
						}
					}
					else {
						if (nbCylindresSurBase == 4){
							state = try_going(750,DISTANCE_BASE_SIDE_ET_ROBOT,DETERMINE_NB_CYLINDRE_SUR_BASE_REPLACEMENT,SI_4_CYLINDRES,ERROR,FAST,BACKWARD,DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
						}
						else if(nbCylindresSurBase == 3){
							state = try_going(850,DISTANCE_BASE_SIDE_ET_ROBOT,DETERMINE_NB_CYLINDRE_SUR_BASE_REPLACEMENT,SI_3_CYLINDRES,ERROR,FAST,BACKWARD,DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
						}
						else if(nbCylindresSurBase == 2){
							state = try_going(950,DISTANCE_BASE_SIDE_ET_ROBOT,DETERMINE_NB_CYLINDRE_SUR_BASE_REPLACEMENT,SI_2_CYLINDRES,ERROR,FAST,BACKWARD,DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
						}
						else if(nbCylindresSurBase == 1){
							state = try_going(1050,DISTANCE_BASE_SIDE_ET_ROBOT,DETERMINE_NB_CYLINDRE_SUR_BASE_REPLACEMENT,SI_1_CYLINDRES,ERROR,FAST,BACKWARD,DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
						}
						else if(nbCylindresSurBase == 0){
							state = SI_0_REPLACEMENT;
						}
					}
				}
				else {
					if(mon_point_de_rotation==SUD){
						if (nbCylindresSurBase == 4){
							state = try_going(1000,3000-DISTANCE_BASE_SIDE_ET_ROBOT,DETERMINE_NB_CYLINDRE_SUR_BASE_REPLACEMENT,SI_4_CYLINDRES,ERROR,FAST,BACKWARD,DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
						}
						else if(nbCylindresSurBase == 3){
							state = try_going(900,3000-DISTANCE_BASE_SIDE_ET_ROBOT,DETERMINE_NB_CYLINDRE_SUR_BASE_REPLACEMENT,SI_3_CYLINDRES,ERROR,FAST,BACKWARD,DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
						}
						else if(nbCylindresSurBase == 2){
							state = try_going(800,3000-DISTANCE_BASE_SIDE_ET_ROBOT,DETERMINE_NB_CYLINDRE_SUR_BASE_REPLACEMENT,SI_2_CYLINDRES,ERROR,FAST,BACKWARD,DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
						}
						else if(nbCylindresSurBase == 1){
							state = try_going(700,3000-DISTANCE_BASE_SIDE_ET_ROBOT,DETERMINE_NB_CYLINDRE_SUR_BASE_REPLACEMENT,SI_1_CYLINDRES,ERROR,FAST,BACKWARD,DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
						}
						else if(nbCylindresSurBase == 0){
							state = SI_0_REPLACEMENT;
						}
					}
					else {
						if (nbCylindresSurBase == 4){
							state = try_going(750,3000-DISTANCE_BASE_SIDE_ET_ROBOT,DETERMINE_NB_CYLINDRE_SUR_BASE_REPLACEMENT,SI_4_CYLINDRES,ERROR,FAST,BACKWARD,DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
						}
						else if(nbCylindresSurBase == 3){
							state = try_going(850,3000-DISTANCE_BASE_SIDE_ET_ROBOT,DETERMINE_NB_CYLINDRE_SUR_BASE_REPLACEMENT,SI_3_CYLINDRES,ERROR,FAST,BACKWARD,DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
						}
						else if(nbCylindresSurBase == 2){
							state = try_going(950,3000-DISTANCE_BASE_SIDE_ET_ROBOT,DETERMINE_NB_CYLINDRE_SUR_BASE_REPLACEMENT,SI_2_CYLINDRES,ERROR,FAST,BACKWARD,DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
						}
						else if(nbCylindresSurBase == 1){
							state = try_going(1050,3000-DISTANCE_BASE_SIDE_ET_ROBOT,DETERMINE_NB_CYLINDRE_SUR_BASE_REPLACEMENT,SI_1_CYLINDRES,ERROR,FAST,BACKWARD,DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
						}
						else if(nbCylindresSurBase == 0){
							state = SI_0_REPLACEMENT;
						}
					}
				}

				break;



//VERIFIER_COULEUR_CYLINDRE_DEJA_SUR_BASE:
//ATTENTION : ROTATION d'un cylindre inconus == monocouleur!

			case SI_4_CYLINDRES:
				if(endroit_depose == OUEST){
					if(mon_point_de_rotation==SUD){
						//rotation cylindre Gauche
						sub_harry_return_modules(LEFT);
						state = try_going(900,DISTANCE_BASE_SIDE_ET_ROBOT,SI_4_CYLINDRES,SI_3_CYLINDRES,ERROR,FAST,BACKWARD,DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
					}
					else{
						//rotation cylindre Droit
						sub_harry_return_modules(RIGHT);
						state = try_going(850,DISTANCE_BASE_SIDE_ET_ROBOT,SI_4_CYLINDRES,SI_3_CYLINDRES,ERROR,FAST,BACKWARD,DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
					}
				}
				else{
					if(mon_point_de_rotation==SUD){
						//rotation cylindre Droit
						sub_harry_return_modules(RIGHT);
						state = try_going(900,3000-DISTANCE_BASE_SIDE_ET_ROBOT,SI_4_CYLINDRES,SI_3_CYLINDRES,ERROR,FAST,BACKWARD,DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
					}
					else{
						//rotation cylindre Gauche
						sub_harry_return_modules(LEFT);
						state = try_going(850,3000-DISTANCE_BASE_SIDE_ET_ROBOT,SI_4_CYLINDRES,SI_3_CYLINDRES,ERROR,FAST,BACKWARD,DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
					}
				}
				break;

			case SI_3_CYLINDRES:
				if(endroit_depose == OUEST){
					if(mon_point_de_rotation==SUD){
						//rotation cylindre Gauche
						sub_harry_return_modules(LEFT);
						state = try_going(800,DISTANCE_BASE_SIDE_ET_ROBOT,SI_3_CYLINDRES,SI_2_CYLINDRES,ERROR,FAST,BACKWARD,DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
					}
					else{
						//rotation cylindre Droit
						sub_harry_return_modules(RIGHT);
						state = try_going(950,DISTANCE_BASE_SIDE_ET_ROBOT,SI_3_CYLINDRES,SI_2_CYLINDRES,ERROR,FAST,BACKWARD,DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
					}
				}
				else{
					if(mon_point_de_rotation==SUD){
						//rotation cylindre Droit
						sub_harry_return_modules(RIGHT);
						state = try_going(800,3000-DISTANCE_BASE_SIDE_ET_ROBOT,SI_3_CYLINDRES,SI_2_CYLINDRES,ERROR,FAST,BACKWARD,DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
					}
					else{
						//rotation cylindre Gauche
						sub_harry_return_modules(LEFT);
						state = try_going(950,3000-DISTANCE_BASE_SIDE_ET_ROBOT,SI_3_CYLINDRES,SI_2_CYLINDRES,ERROR,FAST,BACKWARD,DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
					}
				}
				break;

			case SI_2_CYLINDRES:
				if(endroit_depose == OUEST){
					if(mon_point_de_rotation==SUD){
						//rotation cylindre Gauche
						sub_harry_return_modules(LEFT);
						state = try_going(700,DISTANCE_BASE_SIDE_ET_ROBOT,SI_2_CYLINDRES,SI_1_CYLINDRES,ERROR,FAST,BACKWARD,DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
					}
					else{
						//rotation cylindre Droit
						sub_harry_return_modules(RIGHT);
						state = try_going(1050,DISTANCE_BASE_SIDE_ET_ROBOT,SI_2_CYLINDRES,SI_1_CYLINDRES,ERROR,FAST,BACKWARD,DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
					}
				}
				else{
					if(mon_point_de_rotation==SUD){
						//rotation cylindre Droit
						sub_harry_return_modules(RIGHT);
						state = try_going(700,3000-DISTANCE_BASE_SIDE_ET_ROBOT,SI_2_CYLINDRES,SI_1_CYLINDRES,ERROR,FAST,BACKWARD,DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
					}
					else{
						//rotation cylindre Gauche
						sub_harry_return_modules(LEFT);
						state = try_going(1050,3000-DISTANCE_BASE_SIDE_ET_ROBOT,SI_2_CYLINDRES,SI_1_CYLINDRES,ERROR,FAST,BACKWARD,DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
					}
				}
				break;

			case SI_1_CYLINDRES:
				if(endroit_depose == OUEST){
					if(mon_point_de_rotation==SUD){
						//rotation cylindre Gauche
						sub_harry_return_modules(LEFT);
					}
					else{
						//rotation cylindre Droit
						sub_harry_return_modules(RIGHT);
					}
				}
				else{
					if(mon_point_de_rotation==SUD){
						//rotation cylindre Droit
						sub_harry_return_modules(RIGHT);
					}
					else{
						//rotation cylindre Gauche
						sub_harry_return_modules(LEFT);
					}
				}
				state = SI_0_REPLACEMENT;
				break;

			case SI_0_REPLACEMENT:
				if(endroit_depose == OUEST){
					if(mon_point_de_rotation==SUD){
						state = try_going(1250,DISTANCE_BASE_SIDE_ET_ROBOT,SI_0_REPLACEMENT,CHOIX_DE_COTE_DEPOSE,ERROR,FAST,BACKWARD,DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
					}
					else {
						state = try_going(650,DISTANCE_BASE_SIDE_ET_ROBOT,SI_0_REPLACEMENT,CHOIX_DE_COTE_DEPOSE,ERROR,FAST,BACKWARD,DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
					}
				}
				else {
					if(mon_point_de_rotation==SUD){
						state = try_going(1250,3000-DISTANCE_BASE_SIDE_ET_ROBOT,SI_0_REPLACEMENT,CHOIX_DE_COTE_DEPOSE,ERROR,FAST,BACKWARD,DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
					}
					else {
						state = try_going(650,3000-DISTANCE_BASE_SIDE_ET_ROBOT,SI_0_REPLACEMENT,CHOIX_DE_COTE_DEPOSE,ERROR,FAST,BACKWARD,DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
					}
				}
				break;



//rotation pour drop du bon coté:
			case CHOIX_DE_COTE_DEPOSE:
				state=FIN_TENTATIVE_NORD;
				//if (deposer à droite)
					//state = try_go_angle(PI4096, state, state, ERROR, FAST, FORWARD, END_AT_LAST_POINT);
				//else
					//state = try_go_angle(PI4096, state, state, ERROR, FAST, FORWARD, END_AT_LAST_POINT);
				break;


//DEPOSE:
			case PREPARATION_NEXT:
				if(cote_depose == DROITE){
					//actionneur pour metre en place la rotation: state = ROTATION_MODULE;
				}
				else{
					//actionneur pour metre en place la rotation: state = ROTATION_MODULE;
				}
				break;

			case ROTATION_MODULE:
				if(modules == NEUTRAL_ELEMENT){
					if(cote_depose == DROITE){
						//FONCTION DE ROTATION DROIT: state = TAKE_MODULE
					}
					else{
						//FONCTION DE ROTATION GAUCHE: state = TAKE_MODULE
					}
				}
				else{
					state = TAKE_MODULE;
				}
				break;
			case TAKE_MODULE:
				if(cote_depose == DROITE){
					//actoinneur poser vantouse sur le cylindre à retirer: state = POMPE_ACTIVATION
				}
				else{
					//actoinneur poser vantouse sur le cylindre à retirer: state = POMPE_ACTIVATION
				}
				break;

			case POMPE_ACTIVATION:
				if(cote_depose == DROITE){
					//activer pompe droite: state = DROP_MODULE
				}
				else{
					//activer pompe gauche: state = DROP_MODULE
				}
				break;

			case DROP_MODULE:
				if(cote_depose == DROITE){
					//actoinneur poser vantouse sur le cylindre à retirer: state = POMPE_STOP
				}
				else{
					//actoinneur poser vantouse sur le cylindre à retirer: state = POMPE_STOP
				}
				break;

			case POMPE_STOP:
				if(cote_depose == DROITE){
					//desactiver pompe droite: state =
				}
				else{
					//desactiver pompe gauche: state =
				}
				break;


//DEPOSE OUEST:
			case PREPARATION_PUSHER_OUEST:
				if(mon_point_de_rotation==NORD){
					if(cote_depose==DROITE){
						state = PUSHER_DOWN_OUEST;
					}
					else{
						state = try_going(550,DISTANCE_BASE_SIDE_ET_ROBOT,state,PUSHER_DOWN_OUEST,ERROR,FAST,BACKWARD,DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
					}
				}
				else{
					if(cote_depose==DROITE){
						state = try_going(1200,DISTANCE_BASE_SIDE_ET_ROBOT,state,PUSHER_DOWN_OUEST,ERROR,FAST,BACKWARD,DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
					}
					else{
						state = PUSHER_DOWN_OUEST;
					}
				}
				break;

			case PUSHER_DOWN_OUEST:
				if(cote_depose==DROITE){
					//bras poussoir droit down
				}
				else{
					//bras poussoir gauche down
				}
				break;
			case PUSH_MODULES_OUEST:
				if(nbCylindresSurBase!=4){
					if(mon_point_de_rotation==NORD){
						state = try_going(850,DISTANCE_BASE_SIDE_ET_ROBOT,state,PUSHER_UP_OUEST,ERROR,FAST,BACKWARD,DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
					}
					else{
						state = try_going(1000,DISTANCE_BASE_SIDE_ET_ROBOT,state,PUSHER_UP_OUEST,ERROR,FAST,BACKWARD,DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
					}
				}
				else{
					state = PUSHER_UP_OUEST;
				}
				break;

			case PUSHER_UP_OUEST:
				if(cote_depose==DROITE){
					//bras poussoir droit up
				}
				else{
					//bras poussoir gauche up
				}
				break;

			case PUSH_MODULES_RETURN_TO_POSITION_OUEST:
				if(mon_point_de_rotation==SUD){
					state = try_going(1250,DISTANCE_BASE_SIDE_ET_ROBOT,state,CHOIX_DE_COTE_DEPOSE,ERROR,FAST,BACKWARD,DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
				}
				else {
					state = try_going(650,DISTANCE_BASE_SIDE_ET_ROBOT,state,CHOIX_DE_COTE_DEPOSE,ERROR,FAST,BACKWARD,DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
				}
				break;



//DEPOSE EST:
			case PREPARATION_PUSHER_EST:
				if(mon_point_de_rotation==NORD){
					if(cote_depose==DROITE){
						state = PUSHER_DOWN_EST;
					}
					else{
						state = try_going(550,DISTANCE_BASE_SIDE_ET_ROBOT,state,PUSHER_DOWN_EST,ERROR,FAST,BACKWARD,DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
					}
				}
				else{
					if(cote_depose==DROITE){
						state = try_going(1200,DISTANCE_BASE_SIDE_ET_ROBOT,state,PUSHER_DOWN_EST,ERROR,FAST,BACKWARD,DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
					}
					else{
						state = PUSHER_DOWN_EST;
					}
				}
				break;

			case PUSHER_DOWN_EST:
				if(cote_depose==DROITE){
					//bras poussoir droit down
				}
				else{
					//bras poussoir gauche down
				}
				break;

			case PUSH_MODULES_EST:
				if(nbCylindresSurBase!=4){
					if(mon_point_de_rotation==NORD){
						state = try_going(850,DISTANCE_BASE_SIDE_ET_ROBOT,state,PUSHER_UP_EST,ERROR,FAST,BACKWARD,DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
					}
					else{
						state = try_going(1000,DISTANCE_BASE_SIDE_ET_ROBOT,state,PUSHER_UP_EST,ERROR,FAST,BACKWARD,DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
					}
				}
				else{
					state = PUSHER_UP_EST;
				}
				break;

			case PUSHER_UP_EST:
				if(cote_depose==DROITE){
					//bras poussoir droit up
				}
				else{
					//bras poussoir gauche up
				}
				break;

			case PUSH_MODULES_RETURN_TO_POSITION_EST:
				if(mon_point_de_rotation==SUD){
					state = try_going(1250,DISTANCE_BASE_SIDE_ET_ROBOT,state,CHOIX_DE_COTE_DEPOSE,ERROR,FAST,BACKWARD,DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
				}
				else {
					state = try_going(650,DISTANCE_BASE_SIDE_ET_ROBOT,state,CHOIX_DE_COTE_DEPOSE,ERROR,FAST,BACKWARD,DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
				}
				break;




//PROCEDURE DE FIN:
			case FIN_TENTATIVE_SUD:
				if(endroit_depose == OUEST){
					state = try_going(1250,DISTANCE_BASE_SIDE_ET_ROBOT,state,POINT_DE_ROTATION1,FIN_TENTATIVE_NORD,FAST,ANY_WAY,NO_DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
				}
				else {
					state = try_going(1250,3000-DISTANCE_BASE_SIDE_ET_ROBOT,state,POINT_DE_ROTATION1,FIN_TENTATIVE_NORD,FAST,ANY_WAY,NO_DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
				}
				break;

			case FIN_TENTATIVE_NORD:
				if(endroit_depose == OUEST){
					state = try_going(650,DISTANCE_BASE_SIDE_ET_ROBOT,state,POINT_DE_ROTATION2,FIN_TENTATIVE_SUD,FAST,ANY_WAY,NO_DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
				}
				else {
					state = try_going(650,3000-DISTANCE_BASE_SIDE_ET_ROBOT,state,POINT_DE_ROTATION2,FIN_TENTATIVE_SUD,FAST,ANY_WAY,NO_DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
				}
				break;

			case POINT_DE_ROTATION1:
				state = try_going(1200,COLOR_Y(380),state,DONE,POINT_DE_ROTATION3,FAST,ANY_WAY,NO_DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
				break;

			case POINT_DE_ROTATION2:
				state = try_going(1000,COLOR_Y(380),state,DONE,POINT_DE_ROTATION3,FAST,ANY_WAY,NO_DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
				break;

			case POINT_DE_ROTATION3:
				state = try_going(800,COLOR_Y(380),state,DONE,POINT_DE_ROTATION1,FAST,ANY_WAY,NO_DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
				break;
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

	switch(state){
		case INIT:
			if(IHM_switchs_get(SWITCH_DISABLE_MODULE_RIGHT) && IHM_switchs_get(SWITCH_DISABLE_MODULE_LEFT)){
				state = ERROR; // Actionneur désactivé, on a rien pu prendre
			}else if(STOCKS_isEmpty(MODULE_STOCK_LEFT) && STOCKS_isEmpty(MODULE_STOCK_LEFT)){
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
#warning CORENTIN : Que fait t on sinon ? On reste planté là ??
				state = ERROR;
			}
			break;

		case GO_TO_MIDDLE:
			//state=check_sub_action_result(sub_harry_depose_modules_centre(NEUTRAL_ELEMENT),state,DONE,ERROR);
			state = DONE;
			break;

		case GO_TO_OUR_CENTER:
			//state=check_sub_action_result(sub_harry_depose_modules_centre(OUR_ELEMENT),state,DONE,ERROR);
			state = DONE;
			break;

		case GO_TO_OUR_SIDE:
			state=check_sub_action_result(sub_harry_depose_modules_side(OUR_ELEMENT,RIGHT, OUR_SIDE),state,DONE,ERROR);
			break;

		case GO_TO_ADV_CENTER:
			//state=check_sub_action_result(sub_harry_depose_modules_centre(ADV_ELEMENT),state,DONE,ERROR);
			state = DONE;
			break;

		case GO_TO_ADV_SIDE:
			state=check_sub_action_result(sub_harry_depose_modules_side(ADV_ELEMENT,RIGHT, OUR_SIDE),state,DONE,ERROR);
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
				if((global.color==BLUE)&&(CW_is_color_detected(CW_SENSOR_LEFT, CW_Channel_Yellow))){ //jaune à gauche
					state=WAIT_WHITE;
				}else if((global.color==YELLOW)&&(CW_is_color_detected(CW_SENSOR_LEFT, CW_Channel_Blue))){ //bleu à gauche
					state=WAIT_WHITE;
				}
			}else{
				if((global.color==BLUE)&&(CW_is_color_detected(CW_SENSOR_RIGHT, CW_Channel_Yellow))){ //jaune à droite
					state=WAIT_WHITE;
				}else if((global.color==YELLOW)&&(CW_is_color_detected(CW_SENSOR_RIGHT, CW_Channel_Blue))){ //bleu à droite
					state=WAIT_WHITE;
				}
			}
			break;

		case WAIT_WHITE:
			if(side==LEFT){
				if(CW_is_color_detected(CW_SENSOR_LEFT, CW_Channel_White)){ //blanc à gauche
					state=STOP;
				}
			}else{
				if(CW_is_color_detected(CW_SENSOR_RIGHT, CW_Channel_White)){ //blanc à droite
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
