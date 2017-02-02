#include "action_big.h"
#include "../../propulsion/movement.h"
#include "../../propulsion/astar.h"
#include "../../QS/QS_stateMachineHelper.h"
#include "../../utils/generic_functions.h"
#include "../../utils/actionChecker.h"
#include "../../elements.h"


error_e sub_harry_depose_modules_centre(ELEMENTS_property_e modules){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_DEPOSE_MODULES_CENTRE,
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
			DETERMINE_NB_CYLINDRE_SUR_BASE_REPLACEMENT,

			CHOIX_DE_COTE_DEPOSE,
			DEPOSE_DROIT,
			VERIFICATION_COULEUR_DROIT,
			POUSSER_CYLINDRE_DROIT,
			PRISE_DE_DECISION
		);


	switch(state){
		// Vérifier la présance de cylindre dans le robot... je sais pas faire... nul!

		case INIT:
			//test pour avoir un point de départ:
			state = try_going(1800,COLOR_Y(2200),INIT,LES_SQUARE_COLOR,ERROR,FAST, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case ERROR:
			RESET_MAE();
			return NOT_HANDLED;
			break;

		case DONE:
			RESET_MAE();
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
				//Descendre bras poussoir gauche
				state = DETERMINE_NB_CYLINDRE_SUR_BASE_AVANCER;
				break;

			case DETERMINE_NB_CYLINDRE_SUR_BASE_AVANCER:
				//Avancer doucement jusqu'a bloquage:
				state = try_rush(600,COLOR_Y(280),DETERMINE_NB_CYLINDRE_SUR_BASE_AVANCER,DETERMINE_NB_CYLINDRE,ERROR,FORWARD,NO_DODGE_AND_WAIT,TRUE);
				break;

			case DETERMINE_NB_CYLINDRE:
				if (i_am_in_square_color(550,750,0,500)){
					//J'ai pas de cylindre dans la base:
					state = DETERMINE_NB_CYLINDRE_SUR_BASE_REPLACEMENT;
					}
				else if (i_am_in_square_color(750,850,0,500)){
					//J'ai 1 cylindre dans la base:
					state = DETERMINE_NB_CYLINDRE_SUR_BASE_REPLACEMENT;
					}
				else if (i_am_in_square_color(850,950,0,500)){
					//J'ai 2 cylindre dans la base:
					state = DETERMINE_NB_CYLINDRE_SUR_BASE_REPLACEMENT;
					}
				else if (i_am_in_square_color(950,1050,0,500)){
					//J'ai 3 cylindre dans la base:
					state = DETERMINE_NB_CYLINDRE_SUR_BASE_REPLACEMENT;
					}
				else if (i_am_in_square_color(1050,1150,0,500)){
					//J'ai 4 cylindre dans la base:
					state = DETERMINE_NB_CYLINDRE_SUR_BASE_REPLACEMENT;
					}
				else{
					//ERROR
					state = ERROR;
				}
				break;


			case DETERMINE_NB_CYLINDRE_SUR_BASE_REPLACEMENT:
				//revenir en arrière jusqu'au point (1250,COLOR_Y(280))
				state = try_going(1250,COLOR_Y(280),DETERMINE_NB_CYLINDRE_SUR_BASE_REPLACEMENT,DONE,ERROR,FAST,BACKWARD,DODGE_AND_NO_WAIT,END_AT_LAST_POINT);

				//if (il reste de la place pour déposer des cylindres
				//		changer de case ==> CHOIX_DE_COTE_DEPOSE
				//else
				//		changer de case ==> END
				break;

			case CHOIX_DE_COTE_DEPOSE:
				state=DONE;
				//if (deposer à droite)
				//		changer de case ==> DEPOSE_DROIT
				//else
				//		changer de case ==> DEPOSE_GAUCHE
				break;

			case DEPOSE_DROIT:
				//Rotation vers PI
				state = try_go_angle(PI4096, state, state, ERROR, FAST, FORWARD, END_AT_LAST_POINT);
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
				//if(nb_plade_dispo==6){
					//END
				//else{
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
	moduleDropLocation_e prefered_zone;

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
			state=check_sub_action_result(sub_harry_depose_modules_centre(NEUTRAL_ELEMENT),state,DONE,ERROR);
			break;

		case GO_TO_OUR_CENTER:
			state=check_sub_action_result(sub_harry_depose_modules_centre(OUR_ELEMENT),state,DONE,ERROR);
			break;

		case GO_TO_OUR_SIDE:
			state=check_sub_action_result(sub_harry_depose_modules_side(OUR_ELEMENT),state,DONE,ERROR);
			break;

		case GO_TO_ADV_CENTER:
			state=check_sub_action_result(sub_harry_depose_modules_centre(ADV_ELEMENT),state,DONE,ERROR);
			break;

		case GO_TO_ADV_SIDE:
			state=check_sub_action_result(sub_harry_depose_modules_side(ADV_ELEMENT),state,DONE,ERROR);
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
			return NOT_HANDLED;
			break;

		case DONE:
			RESET_MAE();
			return END_OK;
			break;
	}

	return IN_PROGRESS;
}
