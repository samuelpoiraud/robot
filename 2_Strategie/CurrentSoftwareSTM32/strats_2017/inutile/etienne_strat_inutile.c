#include "strat_inutile.h"
#include "../../propulsion/movement.h"
#include "../../QS/QS_stateMachineHelper.h"

void etienne_strat_inutile_big(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_BOTH_WAIT,
			INIT,

			LES_SQUARE_COLOR,
			CARRE_BAS_GAUCHE,
			CARRE_BAS_DROIT,
			CARRE_MILIEU_GAUCHE,
			CARRE_MILIEU_DROIT_BAS,
			CARRE_MILIEU_DROIT_HAUT,
			CARRE_HAUT_DROIT,
			CARRE_HAUT_GAUCHE,

			DETERMINE_NB_CYLINDRE_SUR_BASE,
			DETERMINE_NB_CYLINDRE_SUR_BASE_ROTATION,
			DETERMINE_NB_CYLINDRE_SUR_BASE_DESCENDRE_BRAS,
			DETERMINE_NB_CYLINDRE_SUR_BASE_AVANCER,
			DETERMINE_NB_CYLINDRE_SUR_BASE_REPLACEMENT,

			CHOIX_DE_COTE_DEPOSE,
			DEPOSE_DROIT,
			VERIFICATION_COULEUR_DROIT,
			POUSSER_CYLINDRE_DROIT,
			PRISE_DE_DECISION,

			DEFAUT,
			ERROR,
			DONE
			);

	switch(state){

		// Vérifier la présance de cylindre dans le robot... je sais pas faire... nul!

		case INIT:
			//test pour avoir un point de départ:
			state = try_going(2000,COLOR_Y(1000),INIT,INIT,ERROR,FAST, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case LES_SQUARE_COLOR:
			if (i_am_in_square_color(372,750,1198,0)){
				state = CARRE_BAS_GAUCHE;
			}

			else if (i_am_in_square_color(1199,1004,2000,0)){
				state = CARRE_BAS_DROIT;
			}

			else if (i_am_in_square_color(0,2250,1198,760)){
				state = CARRE_MILIEU_GAUCHE;
			}

			else if (i_am_in_square_color(1201,1505,2000,1000)){
				state = CARRE_MILIEU_DROIT_BAS;
			}

			else if (i_am_in_square_color(1203,2000,2000,1500)){
				state = CARRE_MILIEU_DROIT_HAUT;
			}

			else if (i_am_in_square_color(1191,3000,2000,2000)){
				state = CARRE_HAUT_DROIT;
			}

			else if (i_am_in_square_color(372,3000,1204,2245)){
				state = CARRE_HAUT_GAUCHE;
			}
			else{
				state = DEFAUT;
			}
			break;

		case CARRE_BAS_GAUCHE:
			state = try_going(1140,COLOR_Y(200),CARRE_BAS_GAUCHE,DETERMINE_NB_CYLINDRE_SUR_BASE,ERROR,FAST, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case CARRE_BAS_DROIT:
			state = try_going(1250,COLOR_Y(300),CARRE_BAS_DROIT,DETERMINE_NB_CYLINDRE_SUR_BASE,ERROR,FAST, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case CARRE_MILIEU_GAUCHE:
			state = try_going(1130,COLOR_Y(845),CARRE_MILIEU_GAUCHE,DETERMINE_NB_CYLINDRE_SUR_BASE,ERROR,FAST, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case CARRE_MILIEU_DROIT_BAS:
			state = try_going(1130,COLOR_Y(845),CARRE_MILIEU_DROIT_BAS,DETERMINE_NB_CYLINDRE_SUR_BASE,ERROR,FAST, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case CARRE_MILIEU_DROIT_HAUT:
			state = try_going(1000,COLOR_Y(1700),CARRE_MILIEU_DROIT_HAUT,DETERMINE_NB_CYLINDRE_SUR_BASE,ERROR,FAST, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case CARRE_HAUT_DROIT:
			state = try_going(1345,COLOR_Y(2255),CARRE_HAUT_DROIT,DETERMINE_NB_CYLINDRE_SUR_BASE,ERROR,FAST, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case CARRE_HAUT_GAUCHE:
			state = try_going(950,COLOR_Y(2250),CARRE_HAUT_GAUCHE,DETERMINE_NB_CYLINDRE_SUR_BASE,ERROR,FAST, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case DETERMINE_NB_CYLINDRE_SUR_BASE_ROTATION:
			//Rotation vers PI:
			state = try_go_angle(PI4096, state, state, ERROR, FAST, FORWARD, END_AT_LAST_POINT);
			break;

		case DETERMINE_NB_CYLINDRE_SUR_BASE_DESCENDRE_BRAS:
			//Descendre bras poussoir gauche
			break;

		case DETERMINE_NB_CYLINDRE_SUR_BASE_AVANCER:
			//Avancer doucement jusqu'a bloquage:
			state = try_advance(NULL, FALSE,600,state, state, ERROR,SLOW,FORWARD,NO_DODGE_AND_WAIT,END_AT_LAST_POINT);

			//refaire 5 if (i_am_in_square_color()) OU (global.pos.x) pour savoir combien il y a de cylindre:
			if (i_am_in_square_color(372,750,1198,0)){
				//Il y a 5 cylindre:
#warning			state = DONE;
			}
			if (i_am_in_square_color(372,750,1198,0)){
				//Il y a 4 cylindre:
				state = DETERMINE_NB_CYLINDRE_SUR_BASE_REPLACEMENT;
			}
			if (i_am_in_square_color(372,750,1198,0)){
				//Il y a 3 cylindre:
				state = DETERMINE_NB_CYLINDRE_SUR_BASE_REPLACEMENT;
			}
			if (i_am_in_square_color(372,750,1198,0)){
				//Il y a 2 cylindre:
				state = DETERMINE_NB_CYLINDRE_SUR_BASE_REPLACEMENT;
			}
			if (i_am_in_square_color(372,750,1198,0)){
				//Il y a 1 cylindre:
				state = DETERMINE_NB_CYLINDRE_SUR_BASE_REPLACEMENT;
			}
			if (i_am_in_square_color(372,750,1198,0)){
				//Il y a 0 cylindre:
				state = DETERMINE_NB_CYLINDRE_SUR_BASE_REPLACEMENT;
			}

			break;

		case DETERMINE_NB_CYLINDRE_SUR_BASE_REPLACEMENT:
			//revenir en arrière jusqu'au point (1140,COLOR_Y(200))
			state = try_going(1140,COLOR_Y(200),CARRE_HAUT_DROIT,DETERMINE_NB_CYLINDRE_SUR_BASE,ERROR,FAST, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);

			//if (il reste de la place pour déposer des cylindres //// global.pos.x
			//		changer de case ==> CHOIX_DE_COTE_DEPOSE
			//else
			//		changer de case ==> END
			break;

		case CHOIX_DE_COTE_DEPOSE:
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
		state = try_going(950,COLOR_Y(2250),POUSSER_CYLINDRE_DROIT,PRISE_DE_DECISION,ERROR,FAST, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);

			// Est ce qu'il reste des cylindres dans le robot?
		//if(nb_cylidre==0){
			//END
		//else{

			// Est ce qu'il reste de la place dans la base


/*		case INIT:
			state=try_advance(NULL, entrance, 204, state, GO_ZONEDEPART, state, SLOW,BACKWARD, DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case GO_ZONEDEPART:
			state=try_going(310, COLOR_Y(1793), state, GO_CYLINDRE1, GO_CYLINDRE1, SLOW, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		//Chemin narmol:
		case GO_CYLINDRE1:
			state = try_going(570, COLOR_Y(1130), state , GO_CYLINDRE2, ERROR_GO_CYLINDRE1, SLOW, BACKWARD, DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case ERROR_GO_CYLINDRE1:
			state = try_going(1100, COLOR_Y(1000), state , GO_CYLINDRE2, ERROR_GO_CYLINDRE1_2, SLOW, BACKWARD, DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case ERROR_GO_CYLINDRE1_2:
			state = try_going(1140, COLOR_Y(1150), state , GO_CYLINDRE2, ERROR_GO_CYLINDRE2_1, SLOW, BACKWARD, DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case GO_CYLINDRE2:
			state = try_going(973,COLOR_Y(570), state, GO_CYLINDRE3,ERROR_GO_CYLINDRE2_1, SLOW, BACKWARD,DODGE_AND_WAIT,END_AT_LAST_POINT);
			break;

		case ERROR_GO_CYLINDRE2_1:
			state = try_going(1100, COLOR_Y(1000), state , ERROR_GO_CYLINDRE2_2, ERROR_GO_CYLINDRE2_2, SLOW, BACKWARD, DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case ERROR_GO_CYLINDRE2_2:
			state = try_going(1217, COLOR_Y(752), state , GO_CYLINDRE3, ERROR_GO_CYLINDRE2_2, SLOW, BACKWARD, DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case GO_CYLINDRE3:
			state = try_going(1280, COLOR_Y(830), state , SYM_GO_MID, SYM_GO_MID, SLOW, BACKWARD, DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;


		//Chemin symetrique:
		case SYM_GO_MID:
			state = try_going(673, COLOR_Y(1522), state , SYM_GO_CYLINDRE1, SYM_GO_CYLINDRE1, SLOW, BACKWARD, DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;
		case SYM_GO_CYLINDRE1:
			state = try_going(625, COLOR_Y(1889), state , SYM_GO_CYLINDRE2, SYM_ERROR_GO_CYLINDRE1, SLOW, BACKWARD, DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case SYM_ERROR_GO_CYLINDRE1:
			state = try_going(1900, COLOR_Y(1000), state , SYM_GO_CYLINDRE2, SYM_ERROR_GO_CYLINDRE2_1, SLOW, BACKWARD, DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case SYM_ERROR_GO_CYLINDRE1_2:
			state = try_going(1860, COLOR_Y(1150), state , SYM_GO_CYLINDRE2, SYM_ERROR_GO_CYLINDRE2_1, SLOW, BACKWARD, DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;
//REVOIR LES COORDONEE:
		case SYM_GO_CYLINDRE2:
			state = try_going(2014,COLOR_Y(1444), state, SYM_GO_CYLINDRE3,SYM_ERROR_GO_CYLINDRE2_1, SLOW, BACKWARD,DODGE_AND_WAIT,END_AT_LAST_POINT);
			break;

		case SYM_ERROR_GO_CYLINDRE2_1:
			state = try_going(1900, COLOR_Y(1000), state , SYM_ERROR_GO_CYLINDRE2_2, SYM_ERROR_GO_CYLINDRE2_2, SLOW, BACKWARD, DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case SYM_ERROR_GO_CYLINDRE2_2:
			state = try_going(1783, COLOR_Y(752), state , SYM_GO_CYLINDRE3, SYM_ERROR_GO_CYLINDRE2_2, SLOW, BACKWARD, DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case SYM_GO_CYLINDRE3:
			state = try_going(1280, COLOR_Y(2142), state , DONE, DONE, SLOW, BACKWARD, DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		//Test de courbe
		case COURBE:{
			displacement_t curve[]={
					{(GEOMETRY_point_t){744, COLOR_Y(835)}, FAST},
					{(GEOMETRY_point_t){841, COLOR_Y(618)}, FAST},
					{(GEOMETRY_point_t){943, COLOR_Y(513)}, FAST},
					{(GEOMETRY_point_t){1158, COLOR_Y(405)}, FAST},
					{(GEOMETRY_point_t){1427, COLOR_Y(524)}, SLOW},
					{(GEOMETRY_point_t){1508, COLOR_Y(802)}, SLOW},
			};
			state = try_going_multipoint(curve, 6, state, DONE, ERROR, BACKWARD, DODGE_AND_WAIT, END_AT_BRAKE);
		}break;*/

		case ERROR:
			break;

		case DONE:
			break;
	}
}


void etienne_strat_inutile_small(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_ANNE_INUTILE,
			INIT,
			ERROR,
			DONE
		);

	switch(state){
		case INIT:
			break;

		case ERROR:
			break;

		case DONE:
			break;
	}
}
