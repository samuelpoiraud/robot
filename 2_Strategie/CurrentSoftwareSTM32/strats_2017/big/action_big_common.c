#include "action_big.h"
#include "../../propulsion/movement.h"
#include "../../propulsion/astar.h"
#include "../../QS/QS_stateMachineHelper.h"

error_e sub_harry_initiale(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_INITIALE,
			INIT,
			ROCKER,
			MODULE_AND_FUSE_MULTICOLOR,
			MODULE_AND_ORE,
			MODULE_AND_FUSE_UNICOLOR,

			MODULES_AND_FUSE_MULTICOLOR,
			MODULES_AND_ORE,
			MODULES_AND_FUSE_UNICOLOR,
			MODULES_AND_PUT_OFF,

			FUSE_UNICOLOR,
			FUSE_MULTICOLOR,
			ORE,
			MODULE,
			PUT_OFF,
			ERROR,
			GET_OUT,
			DONE
		);

	switch(state){
		case INIT:
/*
			if(i_am_in_square_color(0, 360, 0, 360)){   //le robot est dans la zone avant la bascule
				state = ROCKER;
			}

			if(switch_module && switch_fusée_multicolor){
				state = MODULE_AND_FUSE_MULTICOLOR;
			}else if(switch_module && switch_balle){
				state = MODULE_AND_ORE;
			}else if(switch_module && switch_fusée_unicolor){
				state = MODULE_AND_FUSE_UNICOLOR
			}
			else if(switch_modulesssss && switch_fusée_multicolor){ //!!
				state = MODULE_AND_FUSE_MULTICOLOR;
			}else if(switch_modulesssss && switch_balle){
				state = MODULE_AND_ORE;
			}else if(switch_modulesssss && switch_fusée_unicolor){
				state = MODULES_AND_FUSE_UNICOLOR;
			}else if(switch_modulesssss && switch_depose_module){
				state = MODULES_AND_PUT_OFF;
			}
			else if(switch_fusée_multicolor){						//!!
				state = FUSE_MULTICOLOR;
			}else if(switch_fusée_unicolor){
				state = FUSE_UNICOLOR;
			}else if(switch_balle){
				state = state = ORE;
			}
*/

			break;
//if0
		case ROCKER:
			//state=check_sub_action_result(sub_bascule,state, INIT, ERROR);
			break;
//else1
		case MODULE_AND_FUSE_MULTICOLOR:
			//state=check_sub_action_result(sub_module,state, FUSE_MULTICOLOR, ERROR);
			break;
		case FUSE_MULTICOLOR:
			//state=check_sub_action_result(sub_fuse_multicolor,state, DONE, ERROR);
			break;
//else2
		case MODULE_AND_ORE:
			//state=check_sub_action_result(sub_module,state, ORE, ERROR);
			break;
		case ORE:
			//state=check_sub_action_result(sub_fuse_multicolor,state, DONE, ERROR);
			break;
//else3
		case MODULE_AND_FUSE_UNICOLOR:
			//state=check_sub_action_result(sub_module,state, FUSE_UNICOLOR, ERROR);
			break;
		case FUSE_UNICOLOR:
			//state=check_sub_action_result(sub_fuse_multicolor,state, DONE, ERROR);
			break;
//else4																									//!! moduleSSSS
		case MODULES_AND_FUSE_MULTICOLOR:
			//state=check_sub_action_result(sub_moduleS,state, FUSE_MULTICOLOR, ERROR);
			break;

//else5
		case MODULES_AND_ORE:
			//state=check_sub_action_result(sub_moduleS,state, ORE, ERROR);
			break;

//else6
		case MODULES_AND_FUSE_UNICOLOR:
			//state=check_sub_action_result(sub_moduleS,state, FUSE_UNICOLOR, ERROR);
			break;

//else7
		case  MODULES_AND_PUT_OFF:
			//state=check_sub_action_result(sub_moduleS,state, PUT_OFF, ERROR);
			break;
		case PUT_OFF:
			//state=check_sub_action_result(sub_depose_module,state, DONE, ERROR);
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

error_e sub_harry_end_of_match(){
	CREATE_MAE_WITH_VERBOSE(SM_ID_STRAT_HARRY_END_OF_MATCH,
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





