#include "strat_inutile.h"
#include "../../propulsion/movement.h"
#include "../../QS/QS_stateMachineHelper.h"
#include "../../propulsion/astar.h"
#include "../../utils/generic_functions.h"

#include "../../actuator/act_functions.h"
#include "../../actuator/queue.h"
#include "../../utils/actionChecker.h"

void maxime_strat_inutile_big(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_INUTILE,
			INIT,
			MOVE_BACKWARD,
			MOVE_FORWARD,
			RETURN_TO_POS_BACKWARD,
			RETURN_TO_POS_FORWARD,
			TEST_POSITION_MOVE_BACKWARD,
			TEST_POSITION_MOVE_FORWARD,
			START_TIMER_BACKWARD,
			START_TIMER_FORWARD,
			ESCAPE_FROM_BOTTOM,
			ESCAPE_FROM_TOP,
			ERROR,
			DONE
		);


	const displacement_t curve_backward[3] =
			{{(GEOMETRY_point_t){874,COLOR_Y(1206)}, FAST},
			{(GEOMETRY_point_t){1016, COLOR_Y(917)}, FAST},
			{(GEOMETRY_point_t){1263, COLOR_Y(725)}, FAST}};

	const displacement_t curve_forward[3] =
			{{(GEOMETRY_point_t){1016, COLOR_Y(916)}, FAST},
			{(GEOMETRY_point_t){874, COLOR_Y(1206)}, FAST},
			{(GEOMETRY_point_t){850, COLOR_Y(1521)}, FAST}};


	static time32_t tempsDepart;


		switch (state){
		case INIT :
			state=try_going(850, COLOR_Y(1521), INIT, MOVE_BACKWARD, RESET, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

		case MOVE_BACKWARD :
			state=try_going_multipoint(curve_backward, 3, MOVE_BACKWARD, MOVE_FORWARD, START_TIMER_BACKWARD, BACKWARD, NO_DODGE_AND_NO_WAIT, END_AT_BRAKE);
			break;

		case MOVE_FORWARD:
			state=try_going_multipoint(curve_forward, 3, MOVE_FORWARD, MOVE_BACKWARD, START_TIMER_FORWARD, FORWARD, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

		case START_TIMER_BACKWARD :
			tempsDepart = global.match_time;
			state=TEST_POSITION_MOVE_BACKWARD;
			break;

		case TEST_POSITION_MOVE_BACKWARD :
			if (global.match_time >= tempsDepart + 5000){
				if (i_am_in_square_color(761, 1030, 1630, 1390)){
					state=ESCAPE_FROM_TOP;
				}else{
					state=RETURN_TO_POS_BACKWARD;
				}
			}
			break;

		case RETURN_TO_POS_BACKWARD :
			state=try_going(850, COLOR_Y(1521), RETURN_TO_POS_BACKWARD, MOVE_BACKWARD, RETURN_TO_POS_BACKWARD, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

		case START_TIMER_FORWARD :
			tempsDepart = global.match_time;
			state=TEST_POSITION_MOVE_FORWARD;
			break;

		case TEST_POSITION_MOVE_FORWARD :
			if (global.match_time >= tempsDepart + 5000){
				if (i_am_in_square_color(1064, 1460, 792, 594)){
					state=ESCAPE_FROM_BOTTOM;
				}else{
					state=RETURN_TO_POS_FORWARD;
				}
			}
			break;

		case RETURN_TO_POS_FORWARD :
			state=try_going(1263, COLOR_Y(725), RETURN_TO_POS_FORWARD, MOVE_FORWARD, RETURN_TO_POS_FORWARD, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

		case ESCAPE_FROM_BOTTOM :
			state=try_going(1524, COLOR_Y(560), ESCAPE_FROM_BOTTOM, DONE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
			break;

		case ESCAPE_FROM_TOP :
			state=try_going(862, COLOR_Y(1862), ESCAPE_FROM_TOP, DONE, ERROR, FAST, ANY_WAY, NO_DODGE_AND_NO_WAIT, END_AT_LAST_POINT);
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
