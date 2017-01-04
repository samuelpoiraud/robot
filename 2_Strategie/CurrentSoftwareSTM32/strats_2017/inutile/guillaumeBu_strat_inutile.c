#include "strat_inutile.h"
#include "../../propulsion/movement.h"
#include "../../propulsion/astar.h"
#include "../../QS/QS_stateMachineHelper.h"
#include "../../utils/generic_functions.h"

void guillaumeBu_strat_inutile_big(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_INUTILE,
			INIT,
			DEFAUT,
			CARRE_HAUT_GAUCHE,
			CARRE_HAUT_DROIT,
			CARRE_BAS_DROIT,
			MISE_EN_PLACE,
			ERROR,
			DONE
		);

	switch(state){
		case INIT:
			if (i_am_in_square_color(0,700,750,1300))
			{
				state = CARRE_HAUT_GAUCHE;
			}
			else if (i_am_in_square_color(700,1000,750,1300))
			{
				state = CARRE_HAUT_DROIT;
			}
			else if (i_am_in_square_color(690,1000,390,750))
			{
				state = CARRE_BAS_DROIT;
			}
			else
			{
				state = DEFAUT;
			}
			break;

		case CARRE_HAUT_GAUCHE:
			state = try_going(850,COLOR_Y(1100),CARRE_HAUT_GAUCHE,INIT,ERROR,FAST, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case CARRE_HAUT_DROIT:
			state = try_going(860,COLOR_Y(570),CARRE_HAUT_DROIT,INIT,ERROR,FAST, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case CARRE_BAS_DROIT:
			state = try_going(1000,COLOR_Y(200),CARRE_BAS_DROIT,MISE_EN_PLACE,ERROR,FAST, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case MISE_EN_PLACE:
			state = try_going(400,COLOR_Y(200),MISE_EN_PLACE,DONE,ERROR,FAST, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case DEFAUT:
			state = ASTAR_try_going(850,COLOR_Y(1100),CARRE_HAUT_GAUCHE,INIT,ERROR,FAST, ANY_WAY, DODGE_AND_WAIT, END_AT_LAST_POINT);;
			break;

		case ERROR:
			break;

		case DONE:
			break;
	}
}

void guillaumeBu_strat_inutile_small(){
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
