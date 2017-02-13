#include "action_big.h"
#include "../../propulsion/movement.h"
#include "../../propulsion/astar.h"
#include "../../QS/QS_stateMachineHelper.h"
#include "../../QS/QS_CapteurCouleurCW.h"
#include "../../QS/QS_outputlog.h"
#include "../../utils/generic_functions.h"
#include "../../utils/actionChecker.h"
#include "../../elements.h"

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
			GET_IN_OUR_SIDE,
			GET_IN_MIDDLE_OUR_SIDE,
			GET_IN_MIDDLE_ADV_SIDE,
			GET_IN_ADV_SIDE,
			ERROR,
			DONE
		);

	switch(state){
		case INIT:
			if((modules == OUR_ELEMENT) && (basis_side == OUR_SIDE)){
				state = GET_IN_OUR_SIDE;
			}else if(((modules == OUR_ELEMENT) && (basis_side == ADV_SIDE)) || ((modules == NEUTRAL_ELEMENT) && (basis_side == OUR_SIDE))){
				state = GET_IN_MIDDLE_OUR_SIDE;
			}else if(((modules == NEUTRAL_ELEMENT) && (basis_side == ADV_SIDE))||((modules == ADV_ELEMENT) && (basis_side == OUR_SIDE))){
				state = GET_IN_MIDDLE_ADV_SIDE;
			}else if((modules == ADV_ELEMENT) && (basis_side == ADV_SIDE)){
				state = GET_IN_ADV_SIDE;
			}else
				state = ERROR;
			break;

		case GET_IN_OUR_SIDE:
			state = check_sub_action_result(sub_harry_get_in_our_side_depose_module_centre(), state, DONE, ERROR);
			break;

		case GET_IN_MIDDLE_OUR_SIDE:
			state = check_sub_action_result(sub_harry_get_in_middle_our_side_depose_module_centre(), state, DONE, ERROR);
			break;

		case GET_IN_MIDDLE_ADV_SIDE:
			state = check_sub_action_result(sub_harry_get_in_middle_adv_side_depose_module_centre(), state, DONE, ERROR);
			break;

		case GET_IN_ADV_SIDE:
			state = check_sub_action_result(sub_harry_get_in_adv_side_depose_module_centre(), state, DONE, ERROR);
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


error_e sub_harry_get_in_our_side_depose_modules_centre(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_GET_IN_OUR_SIDE_DEPOSE_MODULES_CENTRE,
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


error_e sub_harry_get_in_middle_our_side_depose_modules_centre(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_GET_IN_MIDDLE_OUR_SIDE_DEPOSE_MODULES_CENTRE,
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

error_e sub_harry_get_in_middle_adv_side_depose_modules_centre(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_GET_IN_MIDDLE_ADV_SIDE_DEPOSE_MODULES_CENTRE,
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


error_e sub_harry_get_in_adv_side_depose_modules_centre(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_GET_IN_ADV_SIDE_DEPOSE_MODULES_CENTRE,
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
			state = try_going(900, COLOR_Y(1000), state, DONE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT,END_AT_BRAKE);
			break;

		case GET_IN_FROM_MIDDLE_SQUARE:
			state = try_going(1000, COLOR_Y(2000), state, GET_IN_FROM_OUR_SQUARE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT,END_AT_BRAKE);
			break;

		case GET_IN_FROM_ADV_SQUARE:
			state = try_going(1450, COLOR_Y(2400), state, GET_IN_FROM_MIDDLE_SQUARE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT,END_AT_BRAKE);
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



error_e sub_harry_depose_modules_side(ELEMENTS_property_e modules){
	//liste de ce que je dois faire:
		//mieu definir les chemins après le square color
		//
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_DEPOSE_MODULES_SIDE,
			INIT,
			ERROR,
			DONE,

			LES_SQUARE_COLOR,
			NORD_1,
			NORD_EST_1,
			NORD_EST_2,
			SUD_EST_1,
			SUD_EST_2,
			SUD_EST_3,
			SUD_OUEST_1,
			NORD_OUEST_1,
			PATHFIND,
			FIN_DEPLACEMENTS,


			DETERMINE_NB_CYLINDRE_SUR_BASE,
			DETERMINE_NB_CYLINDRE_SUR_BASE_ROTATION,
			DETERMINE_NB_CYLINDRE_SUR_BASE_DESCENDRE_BRAS,
			DETERMINE_NB_CYLINDRE_SUR_BASE_AVANCER,
			DETERMINE_NB_CYLINDRE,
			DETERMINE_NB_CYLINDRE_SUR_BASE_REMONTER_BRAS,
			DETERMINE_NB_CYLINDRE_SUR_BASE_REPLACEMENT,


		//VERIFIER_COULEUR_CYLINDRE_DEJA_SUR_BASE:
			SI_4_CYLINDRES,//reflexion à faire sur l'importance de cette action car on ne va pas déposer...
			SI_3_CYLINDRES,
			SI_2_CYLINDRES,
			SI_1_CYLINDRES,
			SI_0_REPLACEMENT,

			ROTATION_CYLINDRE_DROIT_DESCENDRE_BRAS,
			ROTATION_CYLINDRE_DROIT,
			ROTATION_CYLINDRE_DROIT_MONTER_BRAS,

			ROTATION_CYLINDRE_GAUCHE_DESCENDRE_BRAS,
			ROTATION_CYLINDRE_GAUCHE,
			ROTATION_CYLINDRE_GAUCHE_MONTER_BRAS,

		//si ce n'est pas un cylindre que l'on à posé:
			ROTATION_CYLINDRE_INCONU_DROIT_DESCENDRE_BRAS,
			ROTATION_CYLINDRE_INCONU_DROIT,
			ROTATION_CYLINDRE_INCONU_DROIT_MONTER_BRAS,

			ROTATION_CYLINDRE_INCONU_GAUCHE_DESCENDRE_BRAS,
			ROTATION_CYLINDRE_INCONU_GAUCHE,
			ROTATION_CYLINDRE_INCONU_GAUCHE_MONTER_BRAS,//j'ai rien contre les gauchers... mais je ne le suis pas..

		//Maintenant on peut ajouter des cylindres:    ENFIN!
			CHOIX_DE_COTE_DEPOSE,

		//procedure de depose du cote droit du robot:
			DEPOSE_DROIT,
			DEPOSE_DROIT_DESCENDRE_PUSHER,

			VERIFICATION_COULEUR_DROIT,
			POUSSER_CYLINDRE_DROIT,
			PRISE_DE_DECISION
		);

	//MES VARIABLES:
	Uint8 nbCylindresSurBase = 0;
	typedef enum{
	   OUEST = 0,
	   EST = 1
	}endroit_depose_config_e;
	endroit_depose_config_e endroit_depose;
	//
	if(global.color==BLUE){
		if(OUR_ELEMENT){
			endroit_depose = OUEST;
		}
		else {
			endroit_depose = EST;
		}
	}
	else {
		if(OUR_ELEMENT){
			endroit_depose = EST;
		}
		else {
			endroit_depose = OUEST;
		}
	}
	typedef enum{
		NORD = 0,
		SUD = 1
	}POINT_DE_ROTATION_config_e;
	POINT_DE_ROTATION_config_e mon_point_de_rotation = SUD;

	switch(state){
		// Vérifier la présance de cylindre dans le robot... je sais pas faire... nul!

		case INIT:
			//test pour avoir un point de départ:
			state = try_going(1800,COLOR_Y(2200),INIT,LES_SQUARE_COLOR,ERROR,FAST, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);
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
			}
			break;



//DEPLACEMENTS:
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

			case PATHFIND:
				state=ASTAR_try_going(1250,COLOR_Y(280),PATHFIND,FIN_DEPLACEMENTS,ERROR,FAST,ANY_WAY,NO_DODGE_AND_WAIT,END_AT_LAST_POINT);
				break;

			case FIN_DEPLACEMENTS:
				state = try_going(1250,COLOR_Y(280),FIN_DEPLACEMENTS,DETERMINE_NB_CYLINDRE_SUR_BASE_ROTATION,ERROR,FAST, ANY_WAY, NO_DODGE_AND_WAIT, END_AT_LAST_POINT);
				break;


//Determiner le nombre de cylindre:
			case DETERMINE_NB_CYLINDRE_SUR_BASE_ROTATION:
				//Rotation vers PI:
				state = try_go_angle(PI4096, DETERMINE_NB_CYLINDRE_SUR_BASE_ROTATION, DETERMINE_NB_CYLINDRE_SUR_BASE_DESCENDRE_BRAS, ERROR, FAST, ANY_WAY, END_AT_LAST_POINT);
				break;

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
						state = try_rush(600,280,DETERMINE_NB_CYLINDRE_SUR_BASE_AVANCER,DETERMINE_NB_CYLINDRE,ERROR,FORWARD,NO_DODGE_AND_WAIT,TRUE);
					}
					else {
						state = try_rush(1250,280,DETERMINE_NB_CYLINDRE_SUR_BASE_AVANCER,DETERMINE_NB_CYLINDRE,ERROR,FORWARD,NO_DODGE_AND_WAIT,TRUE);
					}
				}
				else {
					if(mon_point_de_rotation==SUD){
						state = try_rush(600,2720,DETERMINE_NB_CYLINDRE_SUR_BASE_AVANCER,DETERMINE_NB_CYLINDRE,ERROR,FORWARD,NO_DODGE_AND_WAIT,TRUE);
					}
					else {
						state = try_rush(1250,2720,DETERMINE_NB_CYLINDRE_SUR_BASE_AVANCER,DETERMINE_NB_CYLINDRE,ERROR,FORWARD,NO_DODGE_AND_WAIT,TRUE);
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
							state = try_going(1000,280,DETERMINE_NB_CYLINDRE_SUR_BASE_REPLACEMENT,SI_4_CYLINDRES,ERROR,FAST,BACKWARD,DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
						}
						else if(nbCylindresSurBase == 3){
							state = try_going(900,280,DETERMINE_NB_CYLINDRE_SUR_BASE_REPLACEMENT,SI_3_CYLINDRES,ERROR,FAST,BACKWARD,DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
						}
						else if(nbCylindresSurBase == 2){
							state = try_going(800,280,DETERMINE_NB_CYLINDRE_SUR_BASE_REPLACEMENT,SI_2_CYLINDRES,ERROR,FAST,BACKWARD,DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
						}
						else if(nbCylindresSurBase == 1){
							state = try_going(700,280,DETERMINE_NB_CYLINDRE_SUR_BASE_REPLACEMENT,SI_1_CYLINDRES,ERROR,FAST,BACKWARD,DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
						}
						else if(nbCylindresSurBase == 0){
							state = SI_0_REPLACEMENT;
						}
					}
					else {
						if (nbCylindresSurBase == 4){
							state = try_going(750,280,DETERMINE_NB_CYLINDRE_SUR_BASE_REPLACEMENT,SI_4_CYLINDRES,ERROR,FAST,BACKWARD,DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
						}
						else if(nbCylindresSurBase == 3){
							state = try_going(850,280,DETERMINE_NB_CYLINDRE_SUR_BASE_REPLACEMENT,SI_3_CYLINDRES,ERROR,FAST,BACKWARD,DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
						}
						else if(nbCylindresSurBase == 2){
							state = try_going(950,280,DETERMINE_NB_CYLINDRE_SUR_BASE_REPLACEMENT,SI_2_CYLINDRES,ERROR,FAST,BACKWARD,DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
						}
						else if(nbCylindresSurBase == 1){
							state = try_going(1050,280,DETERMINE_NB_CYLINDRE_SUR_BASE_REPLACEMENT,SI_1_CYLINDRES,ERROR,FAST,BACKWARD,DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
						}
						else if(nbCylindresSurBase == 0){
							state = SI_0_REPLACEMENT;
						}
					}
				}
				else {
					if(mon_point_de_rotation==SUD){
						if (nbCylindresSurBase == 4){
							state = try_going(1000,2720,DETERMINE_NB_CYLINDRE_SUR_BASE_REPLACEMENT,SI_4_CYLINDRES,ERROR,FAST,BACKWARD,DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
						}
						else if(nbCylindresSurBase == 3){
							state = try_going(900,2720,DETERMINE_NB_CYLINDRE_SUR_BASE_REPLACEMENT,SI_3_CYLINDRES,ERROR,FAST,BACKWARD,DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
						}
						else if(nbCylindresSurBase == 2){
							state = try_going(800,2720,DETERMINE_NB_CYLINDRE_SUR_BASE_REPLACEMENT,SI_2_CYLINDRES,ERROR,FAST,BACKWARD,DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
						}
						else if(nbCylindresSurBase == 1){
							state = try_going(700,2720,DETERMINE_NB_CYLINDRE_SUR_BASE_REPLACEMENT,SI_1_CYLINDRES,ERROR,FAST,BACKWARD,DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
						}
						else if(nbCylindresSurBase == 0){
							state = SI_0_REPLACEMENT;
						}
					}
					else {
						if (nbCylindresSurBase == 4){
							state = try_going(750,2720,DETERMINE_NB_CYLINDRE_SUR_BASE_REPLACEMENT,SI_4_CYLINDRES,ERROR,FAST,BACKWARD,DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
						}
						else if(nbCylindresSurBase == 3){
							state = try_going(850,2720,DETERMINE_NB_CYLINDRE_SUR_BASE_REPLACEMENT,SI_3_CYLINDRES,ERROR,FAST,BACKWARD,DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
						}
						else if(nbCylindresSurBase == 2){
							state = try_going(950,2720,DETERMINE_NB_CYLINDRE_SUR_BASE_REPLACEMENT,SI_2_CYLINDRES,ERROR,FAST,BACKWARD,DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
						}
						else if(nbCylindresSurBase == 1){
							state = try_going(1050,2720,DETERMINE_NB_CYLINDRE_SUR_BASE_REPLACEMENT,SI_1_CYLINDRES,ERROR,FAST,BACKWARD,DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
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
						state = try_going(900,280,DETERMINE_NB_CYLINDRE_SUR_BASE_REPLACEMENT,SI_3_CYLINDRES,ERROR,FAST,BACKWARD,DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
					}
					else{
						//rotation cylindre Droit
						sub_harry_return_modules(RIGHT);
						state = try_going(850,280,DETERMINE_NB_CYLINDRE_SUR_BASE_REPLACEMENT,SI_3_CYLINDRES,ERROR,FAST,BACKWARD,DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
					}
				}
				else{
					if(mon_point_de_rotation==SUD){
						//rotation cylindre Droit
						sub_harry_return_modules(RIGHT);
						state = try_going(900,2720,DETERMINE_NB_CYLINDRE_SUR_BASE_REPLACEMENT,SI_3_CYLINDRES,ERROR,FAST,BACKWARD,DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
					}
					else{
						//rotation cylindre Gauche
						sub_harry_return_modules(LEFT);
						state = try_going(850,2720,DETERMINE_NB_CYLINDRE_SUR_BASE_REPLACEMENT,SI_3_CYLINDRES,ERROR,FAST,BACKWARD,DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
					}
				}
				break;

			case SI_3_CYLINDRES:
				if(endroit_depose == OUEST){
					if(mon_point_de_rotation==SUD){
						//rotation cylindre Gauche
						sub_harry_return_modules(LEFT);
						state = try_going(800,280,DETERMINE_NB_CYLINDRE_SUR_BASE_REPLACEMENT,SI_2_CYLINDRES,ERROR,FAST,BACKWARD,DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
					}
					else{
						//rotation cylindre Droit
						sub_harry_return_modules(RIGHT);
						state = try_going(950,280,DETERMINE_NB_CYLINDRE_SUR_BASE_REPLACEMENT,SI_2_CYLINDRES,ERROR,FAST,BACKWARD,DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
					}
				}
				else{
					if(mon_point_de_rotation==SUD){
						//rotation cylindre Droit
						sub_harry_return_modules(RIGHT);
						state = try_going(800,2720,DETERMINE_NB_CYLINDRE_SUR_BASE_REPLACEMENT,SI_2_CYLINDRES,ERROR,FAST,BACKWARD,DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
					}
					else{
						//rotation cylindre Gauche
						sub_harry_return_modules(LEFT);
						state = try_going(950,2720,DETERMINE_NB_CYLINDRE_SUR_BASE_REPLACEMENT,SI_2_CYLINDRES,ERROR,FAST,BACKWARD,DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
					}
				}
				break;

			case SI_2_CYLINDRES:
				if(endroit_depose == OUEST){
					if(mon_point_de_rotation==SUD){
						//rotation cylindre Gauche
						sub_harry_return_modules(LEFT);
						state = try_going(700,280,DETERMINE_NB_CYLINDRE_SUR_BASE_REPLACEMENT,SI_1_CYLINDRES,ERROR,FAST,BACKWARD,DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
					}
					else{
						//rotation cylindre Droit
						sub_harry_return_modules(RIGHT);
						state = try_going(1050,280,DETERMINE_NB_CYLINDRE_SUR_BASE_REPLACEMENT,SI_1_CYLINDRES,ERROR,FAST,BACKWARD,DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
					}
				}
				else{
					if(mon_point_de_rotation==SUD){
						//rotation cylindre Droit
						sub_harry_return_modules(RIGHT);
						state = try_going(700,2720,DETERMINE_NB_CYLINDRE_SUR_BASE_REPLACEMENT,SI_1_CYLINDRES,ERROR,FAST,BACKWARD,DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
					}
					else{
						//rotation cylindre Gauche
						sub_harry_return_modules(LEFT);
						state = try_going(1050,2720,DETERMINE_NB_CYLINDRE_SUR_BASE_REPLACEMENT,SI_1_CYLINDRES,ERROR,FAST,BACKWARD,DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
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
						state = try_going(1250,280,DETERMINE_NB_CYLINDRE_SUR_BASE_REPLACEMENT,CHOIX_DE_COTE_DEPOSE,ERROR,FAST,BACKWARD,DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
					}
					else {
						state = try_going(650,280,DETERMINE_NB_CYLINDRE_SUR_BASE_REPLACEMENT,CHOIX_DE_COTE_DEPOSE,ERROR,FAST,BACKWARD,DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
					}
				}
				else {
					if(mon_point_de_rotation==SUD){
						state = try_going(1250,2720,DETERMINE_NB_CYLINDRE_SUR_BASE_REPLACEMENT,CHOIX_DE_COTE_DEPOSE,ERROR,FAST,BACKWARD,DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
					}
					else {
						state = try_going(650,2720,DETERMINE_NB_CYLINDRE_SUR_BASE_REPLACEMENT,CHOIX_DE_COTE_DEPOSE,ERROR,FAST,BACKWARD,DODGE_AND_NO_WAIT,END_AT_LAST_POINT);
					}
				}
				break;



//rotation pour drop du bon coté:
			case CHOIX_DE_COTE_DEPOSE:
				state=DONE;
				//if (deposer à droite)
					state = try_go_angle(PI4096, state, state, ERROR, FAST, FORWARD, END_AT_LAST_POINT);
				//else
					state = try_go_angle(PI4096, state, state, ERROR, FAST, FORWARD, END_AT_LAST_POINT);
				break;

			case DEPOSE_DROIT:
				//Rotation vers PI
				//descendre le bras poussoir droit
				//Lacher le cylindre
				//ouvrir la trape latérale
				//changer de case ==> VERIFICATION_COULEUR
				state = VERIFICATION_COULEUR_DROIT;
				break;

			case VERIFICATION_COULEUR_DROIT:
				//if(le cylindre est monochrome:
					state = POUSSER_CYLINDRE_DROIT;
				//else:
					//BON LA C'EST LE BORDEL!!!
					//Mais une fois que le cylindre est de la bonne couleur:
					state = POUSSER_CYLINDRE_DROIT;
				break;

			case POUSSER_CYLINDRE_DROIT:
				//On pousse le cylindre
				//Puis on revien en position de depart:
				state = try_going(950,COLOR_Y(2250),POUSSER_CYLINDRE_DROIT,PRISE_DE_DECISION,ERROR,FAST, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);
				break;

			case PRISE_DE_DECISION:
				// Est ce qu'il reste des cylindres dans le robot?
				//if(nb_cylidre==0){
					//END
				//else{
					// Est ce qu'il reste de la place dans la base?
				//if(nb_plade_dispo==4){
					//END
				//else{
				break;

			default:
				if(entrance)
					debug_printf("default case in sub_harry_depose_modules_side\n");
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



	moduleDropLocation_e first_zone=MODULE_DROP_MIDDLE;
	moduleDropLocation_e second_zone=MODULE_DROP_OUR_CENTER;
	moduleDropLocation_e third_zone=MODULE_DROP_OUR_SIDE;
	moduleDropLocation_e fourth_zone=MODULE_DROP_ADV_CENTER;
	moduleDropLocation_e fifth_zone=MODULE_DROP_ADV_SIDE;
	moduleDropLocation_e prefered_zone = first_zone;

	switch(state){
		case INIT:
#ifdef SCAN_ELEMENTS
			//scan les bases de constructions pour compter et évaluer le remplissage
#else
			state=COMPUTE;
#endif
			break;

		case COMPUTE:
			if((getNbDrop(first_zone)<module_zone[first_zone].nb_cylinder_max)&&((module_zone[first_zone].enable_zone!=TRUE)||(i_am_in_square_color(module_zone[first_zone].xmin, module_zone[first_zone].xmax, module_zone[first_zone].ymin, module_zone[first_zone].ymax)))){
				prefered_zone=first_zone;
				state=MANAGE;
			}else if((getNbDrop(second_zone)<module_zone[second_zone].nb_cylinder_max)&&((module_zone[second_zone].enable_zone!=TRUE)||(i_am_in_square_color(module_zone[second_zone].xmin, module_zone[second_zone].xmax, module_zone[second_zone].ymin, module_zone[second_zone].ymax)))){
				prefered_zone=second_zone;
				state=MANAGE;
			}else if((getNbDrop(third_zone)<module_zone[third_zone].nb_cylinder_max)&&((module_zone[third_zone].enable_zone!=TRUE)||(i_am_in_square_color(module_zone[third_zone].xmin, module_zone[third_zone].xmax, module_zone[third_zone].ymin, module_zone[third_zone].ymax)))){
				prefered_zone=third_zone;
				state=MANAGE;
			}else if((getNbDrop(fourth_zone)<module_zone[fourth_zone].nb_cylinder_max)&&((module_zone[fourth_zone].enable_zone!=TRUE)||(i_am_in_square_color(module_zone[fourth_zone].xmin, module_zone[fourth_zone].xmax, module_zone[fourth_zone].ymin, module_zone[fourth_zone].ymax)))){
				prefered_zone=fourth_zone;
				state=MANAGE;
			}else if((getNbDrop(fifth_zone)<module_zone[fifth_zone].nb_cylinder_max)&&((module_zone[fifth_zone].enable_zone!=TRUE)||(i_am_in_square_color(module_zone[fifth_zone].xmin, module_zone[fifth_zone].xmax, module_zone[fifth_zone].ymin, module_zone[fifth_zone].ymax)))){
				prefered_zone=fifth_zone;
				state=MANAGE;
			}else
				state=ERROR;
			break;

		case MANAGE:
			if(prefered_zone==MODULE_DROP_MIDDLE){
				state=GO_TO_MIDDLE;
			}else if(prefered_zone==MODULE_DROP_OUR_CENTER){
				state=GO_TO_OUR_CENTER;
			}else if(prefered_zone==MODULE_DROP_OUR_SIDE){
				state=GO_TO_OUR_SIDE;
			}else if(prefered_zone==MODULE_DROP_ADV_CENTER){
				state=GO_TO_ADV_CENTER;
			}else if(prefered_zone==MODULE_DROP_ADV_SIDE){
				state=GO_TO_ADV_SIDE;
			}
			break;

		case GO_TO_MIDDLE:
			//state=check_sub_action_result(sub_harry_depose_modules_centre(NEUTRAL_ELEMENT),state,DONE,ERROR);
			break;

		case GO_TO_OUR_CENTER:
		//	state=check_sub_action_result(sub_harry_depose_modules_centre(OUR_ELEMENT),state,DONE,ERROR);
			break;

		case GO_TO_OUR_SIDE:
			state=check_sub_action_result(sub_harry_depose_modules_side(OUR_ELEMENT),state,DONE,ERROR);
			break;

		case GO_TO_ADV_CENTER:
		//	state=check_sub_action_result(sub_harry_depose_modules_centre(ADV_ELEMENT),state,DONE,ERROR);
			break;

		case GO_TO_ADV_SIDE:
			state=check_sub_action_result(sub_harry_depose_modules_side(ADV_ELEMENT),state,DONE,ERROR);
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
				if((global.color==BLUE)&&(CW_is_color_detected(0, 2))){ //jaune à gauche
					state=WAIT_WHITE;
				}else if((global.color==YELLOW)&&(CW_is_color_detected(0, 1))){ //bleu à gauche
					state=WAIT_WHITE;
				}
			}else{
				if((global.color==BLUE)&&(CW_is_color_detected(1, 2))){ //jaune à droite
					state=WAIT_WHITE;
				}else if((global.color==YELLOW)&&(CW_is_color_detected(1, 1))){ //bleu à droite
					state=WAIT_WHITE;
				}
			}
			break;

		case WAIT_WHITE:
			if(side==LEFT){
				if(CW_is_color_detected(0, 0)){ //blanc à gauche
					state=STOP;
				}
			}else{
				if(CW_is_color_detected(1, 0)){ //blanc à droite
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
error_e manager_return_modules(){
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
			//module au centre
			if(getModuleType(1, MODULE_DROP_MIDDLE) == MODULE_POLY){ 				//apelle de fonction pour aller à coté du module
				state = check_sub_action_result(/*deplacement1*/manager_return_modules(), state, RETURN_MODULE_DROP_MIDDLE, ERROR);	//state = try_going + try_go_angle
			}
			if(getModuleType(2, MODULE_DROP_MIDDLE) == MODULE_POLY){
				state = check_sub_action_result(/*deplacemen2t*/manager_return_modules(), state, RETURN_MODULE_DROP_MIDDLE, ERROR);
			}
			if(getModuleType(3, MODULE_DROP_MIDDLE) == MODULE_POLY){
				state = check_sub_action_result(/*deplacement3*/manager_return_modules(), state, RETURN_MODULE_DROP_MIDDLE, ERROR);
			}
			if(getModuleType(4, MODULE_DROP_MIDDLE) == MODULE_POLY){
				state = check_sub_action_result(/*deplacement*/manager_return_modules(), state, RETURN_MODULE_DROP_MIDDLE, ERROR);
			}
			if(getModuleType(5, MODULE_DROP_MIDDLE) == MODULE_POLY){
				state = check_sub_action_result(/*deplacement*/manager_return_modules(), state, RETURN_MODULE_DROP_MIDDLE, ERROR);
			}
			if(getModuleType(6, MODULE_DROP_MIDDLE) == MODULE_POLY){ // problème de position
				state = check_sub_action_result(/*deplacement*/manager_return_modules(), state, RETURN_MODULE_DROP_MIDDLE, ERROR);
			}

			//module a notre centre
			if(getModuleType(1, MODULE_DROP_OUR_CENTER) == MODULE_POLY){
				state = check_sub_action_result(/*deplacement*/manager_return_modules(), state, RETURN_MODULE_DROP_OUR_CENTER, ERROR);
			}
			if(getModuleType(2, MODULE_DROP_OUR_CENTER) == MODULE_POLY){
				state = check_sub_action_result(/*deplacement*/manager_return_modules(), state, RETURN_MODULE_DROP_OUR_CENTER, ERROR);
			}
			if(getModuleType(3, MODULE_DROP_OUR_CENTER) == MODULE_POLY){
				state = check_sub_action_result(/*deplacement*/manager_return_modules(), state, RETURN_MODULE_DROP_OUR_CENTER, ERROR);
			}
			if(getModuleType(4, MODULE_DROP_OUR_CENTER) == MODULE_POLY){
				state = check_sub_action_result(/*deplacement*/manager_return_modules(), state, RETURN_MODULE_DROP_OUR_CENTER, ERROR);
			}
			if(getModuleType(5, MODULE_DROP_OUR_CENTER) == MODULE_POLY){
				state = check_sub_action_result(/*deplacement*/manager_return_modules(), state, RETURN_MODULE_DROP_OUR_CENTER, ERROR);
			}
			if(getModuleType(6, MODULE_DROP_OUR_CENTER) == MODULE_POLY){
				state = check_sub_action_result(/*deplacement*/manager_return_modules(), state, RETURN_MODULE_DROP_OUR_CENTER, ERROR);
			}

			//module centre adverse
			if(getModuleType(1, MODULE_DROP_ADV_CENTER) == MODULE_POLY){
				state = check_sub_action_result(/*deplacement*/manager_return_modules(), state, RETURN_MODULE_DROP_ADV_CENTER, ERROR);
			}
			if(getModuleType(2, MODULE_DROP_ADV_CENTER) == MODULE_POLY){
				state = check_sub_action_result(/*deplacement*/manager_return_modules(), state, RETURN_MODULE_DROP_ADV_CENTER, ERROR);
			}
			if(getModuleType(3, MODULE_DROP_ADV_CENTER) == MODULE_POLY){
				state = check_sub_action_result(/*deplacement*/manager_return_modules(), state, RETURN_MODULE_DROP_ADV_CENTER, ERROR);
			}
			if(getModuleType(4, MODULE_DROP_ADV_CENTER) == MODULE_POLY){
				state = check_sub_action_result(/*deplacement*/manager_return_modules(), state, RETURN_MODULE_DROP_ADV_CENTER, ERROR);
			}
			if(getModuleType(5, MODULE_DROP_ADV_CENTER) == MODULE_POLY){
				state = check_sub_action_result(/*deplacement*/manager_return_modules(), state, RETURN_MODULE_DROP_ADV_CENTER, ERROR);
			}
			if(getModuleType(6, MODULE_DROP_ADV_CENTER) == MODULE_POLY){
				state = check_sub_action_result(/*deplacement*/manager_return_modules(), state, RETURN_MODULE_DROP_ADV_CENTER, ERROR);
			}

			//module notre cote
			if(getModuleType(1, MODULE_DROP_OUR_SIDE) == MODULE_POLY){
				state = check_sub_action_result(/*deplacement*/manager_return_modules(), state, RETURN_MODULE_DROP_OUR_SIDE, ERROR);
			}
			if(getModuleType(2, MODULE_DROP_OUR_SIDE) == MODULE_POLY){
				state = check_sub_action_result(/*deplacement*/manager_return_modules(), state, RETURN_MODULE_DROP_OUR_SIDE, ERROR);
			}
			if(getModuleType(3, MODULE_DROP_OUR_SIDE) == MODULE_POLY){
				state = check_sub_action_result(/*deplacement*/manager_return_modules(), state, RETURN_MODULE_DROP_OUR_SIDE, ERROR);
			}
			if(getModuleType(4, MODULE_DROP_OUR_SIDE) == MODULE_POLY){
				state = check_sub_action_result(/*deplacement*/manager_return_modules(), state, RETURN_MODULE_DROP_OUR_SIDE, ERROR);
			}

			//module cote adverse
			if(getModuleType(1, MODULE_DROP_ADV_SIDE) == MODULE_POLY){
				state = check_sub_action_result(/*deplacement*/manager_return_modules(), state, RETURN_MODULE_DROP_ADV_SIDE, ERROR);
			}
			if(getModuleType(2, MODULE_DROP_ADV_SIDE) == MODULE_POLY){
				state = check_sub_action_result(/*deplacement*/manager_return_modules(), state, RETURN_MODULE_DROP_ADV_SIDE, ERROR);
			}
			if(getModuleType(3, MODULE_DROP_ADV_SIDE) == MODULE_POLY){
				state = check_sub_action_result(/*deplacement*/manager_return_modules(), state, RETURN_MODULE_DROP_ADV_SIDE, ERROR);
			}
			if(getModuleType(4, MODULE_DROP_ADV_SIDE) == MODULE_POLY){
				state = check_sub_action_result(/*deplacement*/manager_return_modules(), state, RETURN_MODULE_DROP_ADV_SIDE, ERROR);
			}
			else{

			}
			break;

			//state = check_sub_action_result(sub_harry_return_modules(RIGHT,LEFT), state, DONE, ERROR);

		case RETURN_MODULE_DROP_MIDDLE:
			state = check_sub_action_result(sub_harry_return_modules(0), state, DONE, ERROR);   //choix gauche ou droite
			break;

		case RETURN_MODULE_DROP_OUR_CENTER:
			state = check_sub_action_result(sub_harry_return_modules(0), state, DONE, ERROR);
			break;

		case RETURN_MODULE_DROP_ADV_CENTER:
			state = check_sub_action_result(sub_harry_return_modules(0), state, DONE, ERROR);
			break;

		case RETURN_MODULE_DROP_OUR_SIDE:
			state = check_sub_action_result(sub_harry_return_modules(0), state, DONE, ERROR);
			break;

		case RETURN_MODULE_DROP_ADV_SIDE:
			state = check_sub_action_result(sub_harry_return_modules(0), state, DONE, ERROR);
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
}
