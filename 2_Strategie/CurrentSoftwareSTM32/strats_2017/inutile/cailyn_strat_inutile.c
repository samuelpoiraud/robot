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

#include "../big/action_big.h"
#include "../../QS/QS_types.h"
#include "../../QS/QS_outputlog.h"
#include "../../QS/QS_IHM.h"
#include "../../QS/QS_CapteurCouleurCW.h"
#include "../propulsion/prop_functions.h"

static bool_e color_blue_is_detected(CW_sensor_e sensor, bool_e color_white, bool_e color_yellow, bool_e entrance) {
	assert(sensor <= 1); // CW_SENSOR_LEFT or CW_SENSOR_RIGHT
	static bool_e color_detected[2];
	static time32_t time_detection[2];
	bool_e color_response = FALSE;

	if(entrance){
		color_detected[sensor] = FALSE;
		error_printf("color_blue init\n");
	}

	if(!color_white && !color_yellow && !color_detected[sensor]) {
		color_detected[sensor] = TRUE;
		time_detection[sensor] = global.absolute_time + 100;
		error_printf("Color blue is detected\n\n");
	}else if(!color_white && !color_yellow && color_detected[sensor] && global.absolute_time > time_detection[sensor]){
		color_response = TRUE;
	}else if(color_white || color_yellow){
		color_detected[sensor] = FALSE;
		color_response = FALSE;
	}

	return color_response;
}

void cailyn_strat_inutile_big(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_INUTILE,
			INIT,
			WAIT,
			TURN_FOR_COLOR,
			WAIT_OUR_COLOR,
			WAIT_WHITE,
			STOP_TURN,
			ERROR,
			DONE
		);

	static time32_t time_timeout;
	moduleStockLocation_e storage = MODULE_STOCK_LEFT;
	bool_e color_white = 0, color_blue = 0, color_yellow = 0;

	switch(state){
		case INIT:
			ACT_push_order(ACT_CYLINDER_BALANCER_LEFT, ACT_CYLINDER_BALANCER_LEFT_IN);
			ACT_push_order(ACT_CYLINDER_BALANCER_RIGHT, ACT_CYLINDER_BALANCER_RIGHT_IN);
			ACT_push_order(ACT_CYLINDER_ARM_LEFT, ACT_CYLINDER_ARM_LEFT_OUT);
			ACT_push_order(ACT_CYLINDER_ARM_RIGHT, ACT_CYLINDER_ARM_RIGHT_OUT);
			ACT_push_order(ACT_CYLINDER_DISPOSE_LEFT, ACT_CYLINDER_DISPOSE_LEFT_TAKE);
			ACT_push_order(ACT_CYLINDER_DISPOSE_RIGHT, ACT_CYLINDER_DISPOSE_RIGHT_TAKE);
			time_timeout = global.absolute_time + 8000;
			state = WAIT;
			break;

		case WAIT:
			if(global.absolute_time > time_timeout){
				state = TURN_FOR_COLOR; // Le stockage a semble t il échoué, on ne peut rien faire de plus ici
			}
			break;


		case TURN_FOR_COLOR:
			if(entrance){
				if(storage == MODULE_STOCK_RIGHT){
					ACT_push_order(ACT_CYLINDER_COLOR_RIGHT, ACT_CYLINDER_COLOR_RIGHT_NORMAL_SPEED);
				}else{
					ACT_push_order(ACT_CYLINDER_COLOR_LEFT, ACT_CYLINDER_COLOR_LEFT_NORMAL_SPEED);
				}
				time_timeout = global.absolute_time + 10000;
			}
			// Aucune vérification ici
			state = WAIT_OUR_COLOR;
			break;

		case WAIT_OUR_COLOR:
			if(global.absolute_time > time_timeout){
				state = STOP_TURN;   // Problème : on arrive pas a déterminer la couleur
			}else if(storage == MODULE_STOCK_RIGHT){
				color_white = CW_is_color_detected(CW_SENSOR_RIGHT, CW_Channel_White, FALSE);
				color_yellow = CW_is_color_detected(CW_SENSOR_RIGHT, CW_Channel_Yellow, FALSE);
				color_blue = color_blue_is_detected(CW_SENSOR_RIGHT, color_white, color_yellow, entrance);

				if( ((global.color == BLUE) && color_blue) || ((global.color == YELLOW) && color_yellow)){
					state = WAIT_WHITE;
				}
			}else{
				color_white = CW_is_color_detected(CW_SENSOR_LEFT, CW_Channel_White, FALSE);
				color_yellow = CW_is_color_detected(CW_SENSOR_LEFT, CW_Channel_Yellow, FALSE);
				color_blue = color_blue_is_detected(CW_SENSOR_LEFT, color_white, color_yellow, entrance);

				if( ((global.color == BLUE) && color_blue) || ((global.color == YELLOW) && color_yellow)){
					state = WAIT_WHITE;
				}
			}
			break;

		case WAIT_WHITE:
			// On en profite pour retourner le balancer vers l'intérieur du robot pour gagner du temps
			// Pas de vérification du résultat ici, la couleur est prioritaire.
			if(entrance){
				// On déclenche les balancers, on effectuera une vérification plus tard
				if(storage == MODULE_STOCK_RIGHT){
					ACT_push_order(ACT_CYLINDER_BALANCER_RIGHT, ACT_CYLINDER_BALANCER_RIGHT_IN);
				}else{
					ACT_push_order(ACT_CYLINDER_BALANCER_LEFT, ACT_CYLINDER_BALANCER_LEFT_IN);
				}
			}

			// On attend le blanc
			if(global.absolute_time > time_timeout){
				state = STOP_TURN;   // Problème : on arrive pas a déterminer la couleur
			}else if(storage == MODULE_STOCK_RIGHT){
				if(CW_is_color_detected(CW_SENSOR_RIGHT, CW_Channel_White, FALSE)){
					ELEMENTS_set_flag(FLAG_HARRY_MODULE_COLOR_RIGHT_SUCCESS, TRUE);
					state=STOP_TURN;
				}
			}else{
				if(CW_is_color_detected(CW_SENSOR_LEFT, CW_Channel_White, FALSE)){
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
			state = DONE;
			break;

		case ERROR:
			break;

		case DONE:
			break;
	}
}

void cailyn_strat_inutile_small(){
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
