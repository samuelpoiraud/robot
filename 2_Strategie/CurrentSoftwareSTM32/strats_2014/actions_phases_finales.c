#include "actions_phases_finales.h"
#include "actions_guy.h"
#include "../QS/QS_outputlog.h"
#include "../state_machine_helper.h"
#include "../high_level_strat.h"
#include "../elements.h"
#include "../Geometry.h"
#include "../Pathfind.h"
#include "../zone_mutex.h"
#include "../QS/QS_can_over_xbee.h"
#include "../act_functions.h"
#include "../config/config_pin.h"
#include "../maths_home.h"
#include "../Supervision/SD/SD.h"
#include "../Supervision/Buzzer.h"
#include "actions_both_2014.h"
#include <math.h>

#define MAX_HEIGHT_ARM	143


//Essai de prendre un feu situé à la position indiquée sur un foyer. (va à la position, descend ET remonte !)
//retourne NOT_HANDLED en cas de rush plus bas que le feu absent
//retourne END_OK en cas de rush contre un feu
error_e sub_try_take_fire_on_heart(GEOMETRY_point_t pos, bool_e on_floor);

//Essai de lâcher un feu à une position située sur notre gauche, au niveau indiqué
//retourne NOT_HANDLED en cas de problème
//retourne END_OK en cas de réussite
error_e sub_drop_fire_at_left(void);


//Essai de retourner un feu à une position située sur notre gauche
//retourne NOT_HANDLED en cas de problème
//retourne END_OK en cas de réussite
error_e sub_return_fire_at_left(void);


// vidage du foyer adverse contenant 6 feux dont on assume la position "parfaite"
// wait_time : temps d'attente au wait_point. si 0 : pas d'attente, le point wait_point peut être NULL.
error_e sub_steal_space_crackers(GEOMETRY_point_t wait_point, time32_t wait_time, bool_e scan_before)
{
	CREATE_MAE_WITH_VERBOSE(SM_ID_SUB_STEAL_SPACE_CRACKERS,
		INIT,
		GOTO_VISIBILITY_POINT,	//On regarde si l'adversaire est près du foyer
		WAIT_FOR_ADVERSARY_LEAVING_HEART,
		COMPUTE,
		GET_IN,
		GOTO_SCAN_POINT,

		SCAN,
		REMOVE_OBSTACLE,
		TAKE_OBSTACLE,
		BACK_WITH_OBSTACLE,
		DISPOSE_OBSTACLE,

		GOTO_HEART,
		COMPUTE_POS_FIRE,
		TAKE_FIRE,
		BACK_WITH_FIRE,
		RETURN_FIRE,
		DISPOSE_FIRE,

		EXTRACT_FROM_HEART,
		ERROR,
		DONE

		);
	static enum state_e success_state, fail_state;
	static Sint16 angle, dist;
	static time32_t t;
	static GEOMETRY_point_t pos;
	static Uint8 fire_index = 0, took_fire_index = 0;
	static bool_e already_removed_obstacle = FALSE;
	switch(state)
	{
		case INIT:
			already_removed_obstacle = FALSE;
			fire_index = 0;
			took_fire_index = 0;
			if(global.env.we_posed_on_adversary_hearth)
				state = DONE;
			else
				state = GOTO_VISIBILITY_POINT;
			break;
		case GOTO_VISIBILITY_POINT:	//On regarde si l'adversaire est près du foyer
			if(entrance)
			{
				angle = (global.env.color == YELLOW)?(PI4096/4):((3*PI4096)/4);
			}
			state = try_go_angle(angle,state,WAIT_FOR_ADVERSARY_LEAVING_HEART,ERROR,FAST);
			break;
		case WAIT_FOR_ADVERSARY_LEAVING_HEART:
			if(entrance)
			{
				t = global.env.match_time;
			}
			if(!foe_in_square(FALSE,1600,2000,COLOR_Y(2600),COLOR_Y(3000)))
				state = COMPUTE;
			else
			{
				if(global.env.match_time > t+wait_time)
					state = ERROR;
			}
			break;
		case COMPUTE:
			if(i_am_in_square(1000,1600,COLOR_Y(2400),COLOR_Y(2700)))
				state = GOTO_SCAN_POINT;
			else
				state = GET_IN;
			break;
		case GET_IN:
			state = PATHFIND_try_going((global.env.color == RED)?Z2:A2,state,GOTO_SCAN_POINT,ERROR,ANY_WAY,FAST,DODGE_AND_WAIT,END_AT_LAST_POINT);
			break;
		case GOTO_SCAN_POINT:
			state = try_going(1600,COLOR_Y(2600),state,SCAN,ERROR,FAST,ANY_WAY,DODGE_AND_WAIT);
			break;

		case SCAN:
			//TODO
			state = GOTO_HEART;
			break;
		case REMOVE_OBSTACLE:
			//On est arrivé en erreur sur le foyer. On recule de 3cm, et on essaye de virer l'obstacle
			state = try_advance(30, entrance, state,TAKE_OBSTACLE,EXTRACT_FROM_HEART,FAST,BACKWARD,NO_DODGE_AND_WAIT);
			break;
		case TAKE_OBSTACLE:
			if(entrance)
			{
				pos.x = 1790;
				pos.y = COLOR_Y(2790);
			}
			switch(sub_try_take_fire_on_heart(pos,TRUE))
			{
				case END_OK:
					state = BACK_WITH_OBSTACLE;
					break;
				case IN_PROGRESS:
					break;
				default:	//FAIL
					state = ERROR;
					break;
			}
			break;
		case BACK_WITH_OBSTACLE:
			state = try_advance(450, entrance, state,DISPOSE_OBSTACLE,ERROR,FAST,BACKWARD,NO_DODGE_AND_WAIT);
			break;
		case DISPOSE_OBSTACLE:
			switch(sub_drop_fire_at_left())
			{
				case IN_PROGRESS:
					break;
				default:	//FAIL ou END_OK
					state = GOTO_HEART;
					already_removed_obstacle = TRUE;
					break;
			}
			break;
		case GOTO_HEART:
			if(entrance)
			{
				if(entrance)
					ASSER_set_threshold_error_translation(50,FALSE);
				if(global.env.color == RED)
				{
					pos.x = 1750;
					pos.y = 2750;
				}
				else
				{
					pos.x = 1750;
					pos.y = 250;
				}
			}
			state = try_going(pos.x, pos.y,state,TAKE_FIRE,(already_removed_obstacle)?TAKE_FIRE:REMOVE_OBSTACLE,SLOW,FORWARD,NO_DODGE_AND_NO_WAIT);
			if(ON_LEAVING(GOTO_HEART))
				ASSER_set_threshold_error_translation(0,TRUE);
			break;
		case COMPUTE_POS_FIRE:
#define NB_FIRE_TO_TAKE	4
			switch(fire_index)
			{
				case 0:		pos = (GEOMETRY_point_t){1810,COLOR_Y(2930)};		break;
				case 1:		pos = (GEOMETRY_point_t){1810,COLOR_Y(2865)};		break;
				case 2:		pos = (GEOMETRY_point_t){1940,COLOR_Y(2865)};		break;
				case 3:	//No break
				default:	pos = (GEOMETRY_point_t){1940,COLOR_Y(2790)};		break;
				//case 4:		pos = (GEOMETRY_point_t){1940,COLOR_Y(2930)};		break;
			}
			state = TAKE_FIRE;
			break;
		case TAKE_FIRE:
			//TODO : L'action est suffisamment commencée pour ne pas la retenter ensuite


			//Position du feu : en x et y.
			switch(sub_try_take_fire_on_heart(pos,FALSE))
			{
				case END_OK:
					state = BACK_WITH_FIRE;
					break;
				case IN_PROGRESS:
					break;
				default:	//FAIL
					state = COMPUTE_POS_FIRE;
					break;
			}

			if(ON_LEAVING(TAKE_FIRE))
				fire_index++;
			break;
		case BACK_WITH_FIRE:
			if(entrance)
			{
				switch(took_fire_index)
				{
					case 0:
						dist = 100;
						success_state = RETURN_FIRE;
						fail_state = DISPOSE_FIRE;
						break;
					case 1:
						dist = 300;
						success_state = RETURN_FIRE;
						fail_state = DISPOSE_FIRE;
						break;
					case 2:
						dist = 100;
						success_state = DISPOSE_FIRE;
						fail_state = DISPOSE_FIRE;
					//case 3:
					default:
						dist = 300;
						success_state = DISPOSE_FIRE;
						fail_state = DISPOSE_FIRE;
					//case 4:
						//Ou success_state = POSE_FIRE;	//pour empiler les feux adverses et gagner du temps...
						break;
				}
			}
			state = try_advance(dist, entrance, state,success_state,fail_state,FAST,BACKWARD,NO_DODGE_AND_WAIT);

			if(ON_LEAVING(BACK_WITH_FIRE))
				took_fire_index++;
			break;

		case DISPOSE_FIRE:	//On pose le feu tel quel (on le lache au niveau 2)
			switch(sub_drop_fire_at_left())
			{
				case END_OK:
					if(fire_index < NB_FIRE_TO_TAKE)
						state = GOTO_HEART;
					else
						state = EXTRACT_FROM_HEART;
					break;
				case IN_PROGRESS:
					break;
				default:	//FAIL
					state = GOTO_HEART;
					break;
			}
			break;
		case RETURN_FIRE:
			switch(sub_return_fire_at_left())
			{
				case END_OK:
					if(fire_index < NB_FIRE_TO_TAKE)
						state = GOTO_HEART;
					else
						state = EXTRACT_FROM_HEART;
					break;
				case IN_PROGRESS:
					break;
				default:	//FAIL
					state = GOTO_HEART;
					break;
			}
			break;

		case EXTRACT_FROM_HEART:
			state = try_advance(150,entrance,state,DONE,ERROR,FAST,BACKWARD,DODGE_AND_WAIT);
			break;
		case ERROR:
			if(i_am_in_square(1700,2000,COLOR_Y(2700),COLOR_Y(3000)))
				state = EXTRACT_FROM_HEART;	//Je suis trop proche du foyer, je dois encore m'éloigner
			else
				state = DONE;
			break;
		case DONE:
			ACT_arm_goto(ACT_ARM_POS_PARKED);
			state = INIT;
			return END_OK;
			break;

	}
	return IN_PROGRESS;
}


//Essai de prendre un feu situé à la position indiquée sur un foyer. (va à la position, descend ET remonte !)
//retourne NOT_HANDLED en cas de rush plus bas que le feu absent ou prôblème bras
//retourne END_OK en cas de rush contre un feu
// ATTENTION : Ne rentre pas le bras
error_e sub_try_take_fire_on_heart(GEOMETRY_point_t pos, bool_e on_floor){
	CREATE_MAE_WITH_VERBOSE(0,
		IDLE,
		FIRE_AGAIN,
		FIRE,
		WAIT_STABILISATION,
		DOWN_ARM,
		UP_ARM,
		UP_ARM_ERROR,
		ARM_OPEN,
		ERROR,
		DONE
	);
	static Uint8 nb_try;

	switch(state){
		case IDLE :
			nb_try = 0;
			state = FIRE;
			break;

		case FIRE_AGAIN:
			state = FIRE;
			break;
		case FIRE :
			if(entrance)
				nb_try++;
			state = ACT_arm_move(ACT_ARM_POS_ON_TRIANGLE, pos.x, pos.y, FIRE, WAIT_STABILISATION, (nb_try > 2)?ARM_OPEN:FIRE_AGAIN);
			break;

		case WAIT_STABILISATION :
			state = ELEMENT_wait_time(300, WAIT_STABILISATION, DOWN_ARM);
			break;

		case DOWN_ARM:
			if(entrance)
				ACT_pompe_order(ACT_POMPE_NORMAL, 100);

			state = ACT_elevator_arm_rush_in_the_floor((on_floor)?30:60, DOWN_ARM, UP_ARM, UP_ARM_ERROR);
			break;

		case UP_ARM:
			state = ACT_elevator_arm_move(MAX_HEIGHT_ARM, UP_ARM, ARM_OPEN, ARM_OPEN);
			break;

		case UP_ARM_ERROR:
			if(entrance)
				ACT_pompe_order(ACT_POMPE_STOP, 0);

			state = ACT_elevator_arm_move(MAX_HEIGHT_ARM, UP_ARM_ERROR, ARM_OPEN, ARM_OPEN);
			break;

		case ARM_OPEN:
			state = ACT_arm_move(ACT_ARM_POS_OPEN, 0, 0, ARM_OPEN, (last_state == UP_ARM)?DONE:ERROR, (last_state == UP_ARM)?DONE:ERROR);
			break;

		case DONE:
			RESET_MAE();
			return END_OK;
			break;

		case ERROR:
			ACT_pompe_order(ACT_POMPE_STOP, 0);
			RESET_MAE();
			return NOT_HANDLED;
			break;
	}

	return IN_PROGRESS;
}


//Essai de lâcher un feu à une position située sur notre gauche, à l'étage 2
//retourne NOT_HANDLED en cas de problème
//retourne END_OK en cas de réussite
//Coupe la pompe à la fin !
error_e sub_drop_fire_at_left(void){
	CREATE_MAE_WITH_VERBOSE(0,
		IDLE,
		ARM_RETURN,
		INVERSE_PUMP,
		PREPARE_RETURN,
		ARM_OPEN,
		ERROR,
		DONE
	);

	switch(state){
		case IDLE :
			state = ARM_RETURN;
			break;

		case ARM_RETURN:
			state = ACT_arm_move(ACT_ARM_POS_TO_RETURN,0, 0, ARM_RETURN, INVERSE_PUMP, INVERSE_PUMP);
			break;

		case INVERSE_PUMP:
			if(entrance)
				ACT_pompe_order(ACT_POMPE_REVERSE, 100);

			state = ELEMENT_wait_time(300, INVERSE_PUMP, ARM_OPEN);
			break;

		case ARM_OPEN:
			state = ACT_arm_move(ACT_ARM_POS_OPEN, 0, 0, ARM_OPEN, DONE, ERROR);
			break;

		case DONE:
			ACT_pompe_order(ACT_POMPE_STOP, 0);
			RESET_MAE();
			return END_OK;
			break;

		case ERROR:
			ACT_pompe_order(ACT_POMPE_STOP, 0);
			RESET_MAE();
			return NOT_HANDLED;
			break;
	}

	return IN_PROGRESS;
}


//Essai de retourner un feu à une position située sur notre gauche, à l'étage 2
//retourne NOT_HANDLED en cas de problème
//retourne END_OK en cas de réussite
//Coupe la pompe à la fin !
error_e sub_return_fire_at_left(void){
	CREATE_MAE_WITH_VERBOSE(0,
		IDLE,
		ARM_RETURN,
		SMALL_ARM_PREPARE,
		WAIT_RETURN,
		INVERSE_PUMP,
		PREPARE_RETURN,
		SMALL_ARM_DEPLOYED,
		SMALL_ARM_PARKED,
		ERROR,
		DONE
	);

	switch(state){
		case IDLE :
			state = ARM_RETURN;
			break;

		case ARM_RETURN:
			state = ACT_arm_move(ACT_ARM_POS_TO_RETURN,0, 0, ARM_RETURN, SMALL_ARM_PREPARE, ERROR);
			break;

		case SMALL_ARM_PREPARE:
			state = ACT_small_arm_move(ACT_SMALL_ARM_MID, SMALL_ARM_PREPARE, WAIT_RETURN, WAIT_RETURN);
			break;

		case WAIT_RETURN:
			state = ELEMENT_wait_time(300,WAIT_RETURN,INVERSE_PUMP);
			break;

		case INVERSE_PUMP:
			if(entrance)
				ACT_pompe_order(ACT_POMPE_REVERSE, 100);

			state = ELEMENT_wait_time(300, INVERSE_PUMP, PREPARE_RETURN);
			break;

		case PREPARE_RETURN:
			state = ACT_arm_move(ACT_ARM_POS_WAIT_RETURN,0, 0, PREPARE_RETURN, SMALL_ARM_DEPLOYED, SMALL_ARM_DEPLOYED);
			break;

		case SMALL_ARM_DEPLOYED:
			state = ACT_small_arm_move(ACT_SMALL_ARM_DEPLOYED, SMALL_ARM_DEPLOYED, SMALL_ARM_PARKED, SMALL_ARM_PARKED);
			break;

		case SMALL_ARM_PARKED:
			state = ACT_small_arm_move(ACT_SMALL_ARM_IDLE, SMALL_ARM_PARKED, DONE, ERROR);
			break;

		case DONE:
			ACT_pompe_order(ACT_POMPE_STOP, 0);
			RESET_MAE();
			return END_OK;
			break;

		case ERROR:
			ACT_pompe_order(ACT_POMPE_STOP, 0);
			RESET_MAE();
			return NOT_HANDLED;
			break;
	}

	return IN_PROGRESS;
}




error_e put_fire_down(Sint16 dropXpos, Sint16 dropYpos){
	CREATE_MAE_WITH_VERBOSE(SM_ID_GUY_PUT_FIRE_DOWN,
			IDLE,
			POS_START,
			DROP,
			PARKED_NOT_HANDLED,
			EXTRACT,
			CORRECTLY_ENDED,
			ERROR
			);


	const Sint16 extractXpos = 1600, extractYpos = 400;

	switch(state){
	case IDLE:

		if(i_am_in_square(1200, 2000, COLOR_Y(1000), COLOR_Y(2000)))
			state = POS_START;
		else
			state = ERROR;						// On a pas lieux d'être appelé si on est pas dans la bonne zone
		break;

	case POS_START:
		state = try_going(dropXpos,COLOR_Y(dropYpos),POS_START, DROP, PARKED_NOT_HANDLED,FAST,FORWARD,NO_DODGE_AND_WAIT);
		break;

	case DROP:
		ACT_pompe_order(ACT_POMPE_REVERSE, 100);
		state = CORRECTLY_ENDED;
		break;

	case PARKED_NOT_HANDLED:{
		static enum state_e state1, state2;

		if(entrance){
			ACT_pompe_order(ACT_POMPE_STOP, 0);
			state1 = PARKED_NOT_HANDLED;
			state2 = PARKED_NOT_HANDLED;
		}

		if(state1 == PARKED_NOT_HANDLED)
			state1 = ACT_arm_move(ACT_ARM_POS_PARKED,0, 0, PARKED_NOT_HANDLED, ERROR, ERROR);
		if(state2 == PARKED_NOT_HANDLED)
			state2 = ACT_small_arm_move(ACT_SMALL_ARM_IDLE, PARKED_NOT_HANDLED, ERROR, ERROR);

		if((state1 == ERROR && state2 != PARKED_NOT_HANDLED) || (state1 != PARKED_NOT_HANDLED && state2 == ERROR))
			state = ERROR;
		else if(state1 != PARKED_NOT_HANDLED && state2 != PARKED_NOT_HANDLED)
			state = ERROR;
	}break;

	case EXTRACT:
		state = try_going(extractXpos, extractYpos, EXTRACT, CORRECTLY_ENDED, ERROR, FAST, BACKWARD, NO_DODGE_AND_WAIT);
		break;

	case ERROR:
		state = IDLE;
		return NOT_HANDLED;
		break;

	case CORRECTLY_ENDED:
		state = IDLE;
		return END_OK;
		break;
	default:
		break;
	}

	return IN_PROGRESS;
}


