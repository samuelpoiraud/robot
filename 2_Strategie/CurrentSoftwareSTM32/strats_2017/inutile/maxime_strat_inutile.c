#include "strat_inutile.h"
#include "../../propulsion/movement.h"
#include "../../QS/QS_stateMachineHelper.h"
#include "../../propulsion/astar.h"
#include "../../utils/generic_functions.h"

void maxime_strat_inutile_big(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_INUTILE,
			INIT,
			GET_IN,
			PRENDRE_1ER,
			GET_IN_DEPUIS_CARRE1,
			GET_IN_DEPUIS_CARRE2,
			ROTATION_POUR_1,
			GET_IN_DEFAULT,
			PRENDRE_2EME,
			PRENDRE_3EME,
			ERROR,
			DONE
		);

	bool_e prendreLes3 = TRUE;

	const displacement_t curve_depuis_zone_adverse[3] =
			{{(GEOMETRY_point_t){860,COLOR_Y(1980)}, FAST},
			{(GEOMETRY_point_t){717, COLOR_Y(1530)}, FAST},
			{(GEOMETRY_point_t){670, COLOR_Y(1065)}, FAST},};

	switch(state)
	{
		case INIT:
		state = try_going(947,COLOR_Y(2162), INIT, GET_IN, ERROR, FAST, FORWARD, DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case GET_IN:
			if (i_am_in_square_color(0,1150,1110,1900)){ //Le carre entre les 2 zones de départ
				state = GET_IN_DEPUIS_CARRE1;
			}else if(i_am_in_square_color(400, 2000, 1400, 2885)){ //Le robot est dans la zone adverse
				state = GET_IN_DEPUIS_CARRE2;
			}else{
				state = GET_IN_DEFAULT;
			}
			break;

		case GET_IN_DEPUIS_CARRE1 :
			state = try_going(670,COLOR_Y(1065), GET_IN_DEPUIS_CARRE1, PRENDRE_1ER, ERROR, FAST, FORWARD, DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case GET_IN_DEPUIS_CARRE2 :
			state = try_going_multipoint(curve_depuis_zone_adverse, 3, GET_IN_DEPUIS_CARRE2, ROTATION_POUR_1, ERROR, FORWARD, DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case ROTATION_POUR_1 :
			state = try_go_angle(-PI4096/4, GET_IN_DEPUIS_CARRE2, PRENDRE_1ER, ERROR, FAST, FORWARD, END_AT_LAST_POINT);
			break;

		case GET_IN_DEFAULT :
			state = ASTAR_try_going(670, COLOR_Y(1065), GET_IN_DEFAULT, ROTATION_POUR_1, ERROR, FAST, FORWARD, DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case PRENDRE_1ER:
			state = PRENDRE_2EME; //Normalement on active les actionneurs
			break;

		case PRENDRE_2EME :
			if (prendreLes3){
				state = try_going(1114, COLOR_Y(505), PRENDRE_2EME, PRENDRE_3EME, ERROR, FAST, FORWARD, DODGE_AND_WAIT, END_AT_LAST_POINT);
			}else{
				state = DONE;
			}
			break;

		case PRENDRE_3EME :
			state = try_going(1407, COLOR_Y(857), PRENDRE_3EME, DONE, ERROR, FAST, FORWARD, DODGE_AND_WAIT, END_AT_LAST_POINT);
			break;

		case ERROR:
			break;

		case DONE:
			break;
	}
}

void maxime_strat_inutile_small(){
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
