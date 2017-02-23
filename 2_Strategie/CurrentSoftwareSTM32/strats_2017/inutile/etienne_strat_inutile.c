#include "strat_inutile.h"
#include "../../propulsion/movement.h"
#include "../../propulsion/astar.h"
#include "../../actuator/act_functions.h"
#include "../../actuator/queue.h"
#include "../../QS/QS_stateMachineHelper.h"
#include "../../utils/generic_functions.h"
#include "../../utils/actionChecker.h"
#include "../../elements.h"
#include "../../strats_2017/big/action_big.h"
#include "../../strats_2017/small/action_small.h"
#include "../../strats_2017/actions_both_2017.h"

void etienne_strat_inutile_big(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_INUTILE,
				INIT,
				INIT1,
				ERROR,
				DONE
			);

		switch(state){
			case INIT:
				if(entrance){
					global.color==YELLOW;
					//global.color==BLUE;
				}
				//state = try_advance(NULL,entrance,400,state,INIT1,ERROR,FAST,BACKWARD,DODGE_AND_WAIT, END_AT_LAST_POINT);
				state = INIT1;
				break;

			case INIT1:
				state = check_sub_action_result(sub_harry_depose_modules_side(ADV_ELEMENT),state,DONE,ERROR);
				break;

			case ERROR:
				break;

			case DONE:
				break;
		}
}
/*

		case INIT:
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
