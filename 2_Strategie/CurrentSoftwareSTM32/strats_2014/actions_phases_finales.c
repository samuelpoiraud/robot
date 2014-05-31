
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


//Essai de prendre un feu situé à la position indiquée sur un foyer. (va à la position, descend ET remonte !)
//retourne NOT_HANDLED en cas de rush plus bas que le feu absent
//retourne END_OK en cas de rush contre un feu
error_e sub_try_take_fire_on_heart(GEOMETRY_point_t pos);

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
	switch(state)
	{
		case INIT:
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
			if(i_am_in_square(500,1600,COLOR_Y(1900),COLOR_Y(2700)))
				state = GOTO_SCAN_POINT;
			else
				state = GET_IN;
			break;
		case GET_IN:
			state = PATHFIND_try_going((global.env.color == RED)?A2:Z2,state,GOTO_SCAN_POINT,ERROR,ANY_WAY,FAST,DODGE_AND_WAIT,END_AT_LAST_POINT);
			break;
		case GOTO_SCAN_POINT:
			state = try_going(1600,COLOR_Y(2600),state,SCAN,ERROR,FAST,ANY_WAY,DODGE_AND_WAIT);
			break;

		case SCAN:
			//TODO
			state = GOTO_HEART;
			break;
		case REMOVE_OBSTACLE:
			break;

		case GOTO_HEART:
			if(entrance)
			{
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
			state = try_going(pos.x, pos.y,state,TAKE_FIRE,EXTRACT_FROM_HEART,SLOW,FORWARD,NO_DODGE_AND_NO_WAIT);
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
			switch(sub_try_take_fire_on_heart(pos))
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
			state = try_going(1600,COLOR_Y(2600),state,DONE,ERROR,FAST,ANY_WAY,DODGE_AND_WAIT);
			break;
		case ERROR:
			if(i_am_in_square(1700,2000,COLOR_Y(2700),COLOR_Y(3000)))
				state = EXTRACT_FROM_HEART;	//Je suis trop proche du foyer, je dois encore m'éloigner
			else
				state = DONE;
			break;
		case DONE:
			state = INIT;
			return END_OK;
			break;

	}
	return IN_PROGRESS;
}


//Essai de prendre un feu situé à la position indiquée sur un foyer. (va à la position, descend ET remonte !)
//retourne NOT_HANDLED en cas de rush plus bas que le feu absent
//retourne END_OK en cas de rush contre un feu
error_e sub_try_take_fire_on_heart(GEOMETRY_point_t pos)
{
	return END_OK;
}


//Essai de lâcher un feu à une position située sur notre gauche, à l'étage 2
//retourne NOT_HANDLED en cas de problème
//retourne END_OK en cas de réussite
//Coupe la pompe à la fin !
error_e sub_drop_fire_at_left(void)
{
	return END_OK;
}


//Essai de retourner un feu à une position située sur notre gauche, à l'étage 2
//retourne NOT_HANDLED en cas de problème
//retourne END_OK en cas de réussite
//Coupe la pompe à la fin !
error_e sub_return_fire_at_left(void)
{
	return END_OK;
}




error_e put_fire_down(void){
	CREATE_MAE_WITH_VERBOSE(SM_ID_GUY_PUT_FIRE_DOWN,
			IDLE,
			POS_START,
			DROP,
			PARKED_NOT_HANDLED,
			EXTRACT,
			CORRECTLY_ENDED,
			ERROR
			);

	const Sint16 dropXpos = 1650, dropYpos = 350;
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


