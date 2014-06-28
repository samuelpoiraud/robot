/*
 *	Club Robot ESEO 2012 - 2013
 *	Krusty & Tiny
 *
 *	Fichier : actions_gifts.c
 *	Package : Carte Strategie
 *	Description : Contient des sub_actions ou micro strat concernant la gestion des cadeaux pour TINY
 *	Auteur : nirgal
 *	Version 201305
 */
#if 0
#include "actions_gifts.h"
#include "actions_utils.h"
#include "../act_functions.h"
#include "../prop_functions.h"
#include "../QS/QS_outputlog.h"

#include "config_pin.h"


/*
1- ATTENTION, point de départ imposé, du coté de notre couleur
2- TINY descend tout les cadeaux lors d'un seule trajectoire
		Si rencontre adversaire -> NOT_HANDLED
3- A la fin du travail -> OK
-> A chaque cadeau descendu : mise à jour de l'environnement.
*/
error_e TINY_open_all_gifts_without_pause(void)
{
	typedef enum
	{
		INIT=0,
		OPENING_GIFTS,
		HAMMER_HOME,
		FAIL,
		FAIL_HAMMER_HOME,
		TURN_FOR_HAMMER_DOWN,
		DONE
	}state_e;
	static state_e state = INIT;
	static state_e previous_state = INIT;

	error_e ret = IN_PROGRESS;
	error_e sub_action;
	static avoidance_type_e avoidance = NO_AVOIDANCE;

	switch(state)
	{
		case INIT:
			TINY_hammer_open_all_gift(TRUE);
			avoidance = NO_AVOIDANCE;

			state = OPENING_GIFTS;
		break;
		case OPENING_GIFTS:

			TINY_hammer_open_all_gift(FALSE);	//Gestion du mouvement du bras...

			if((SWITCH_STRAT_5 && COLOR_Y(global.env.pos.y) > 1330) || (!SWITCH_STRAT_5 && COLOR_Y(global.env.pos.y) > 2000))
				avoidance = NO_DODGE_AND_WAIT;	//Activation de l'évitement à partir du franchissement du second cadeau

			if(SWITCH_STRAT_2)	//2 cadeaux
				sub_action = goto_pos_with_scan_foe((displacement_t[]){{{250,COLOR_Y(400)},90},{{170,COLOR_Y(600)},90},{{170,COLOR_Y(1400)},90}},3,(global.env.color==BLUE)?BACKWARD:FORWARD,avoidance);
			else	//4 cadeaux
				sub_action = goto_pos_with_scan_foe((displacement_t[]){{{250,COLOR_Y(400)},90},{{170,COLOR_Y(600)},90},{{170,COLOR_Y(2300)},90}},3,(global.env.color==BLUE)?BACKWARD:FORWARD,avoidance);
			switch(sub_action)
			{
				case END_OK:
					if(SWITCH_STRAT_2)
						state = TURN_FOR_HAMMER_DOWN;
					else
						state = HAMMER_HOME;
				break;
				case END_WITH_TIMEOUT:	//Echec de la mission
				case NOT_HANDLED:		//Echec de la mission
				case FOE_IN_PATH:
					state = FAIL;
				break;
				case IN_PROGRESS:
				break;
				default:
				break;
			}
		break;
		case TURN_FOR_HAMMER_DOWN:
			state = try_go_angle((global.env.color == RED)?PI4096:0, TURN_FOR_HAMMER_DOWN, HAMMER_HOME, HAMMER_HOME, FAST);
		break;
		case HAMMER_HOME:
			if(SWITCH_STRAT_2)
				global.env.map_elements[GOAL_Cadeau1] = ELEMENT_DONE;	//C'est bon pour le 2ème cadeau !
			else
				global.env.map_elements[GOAL_Cadeau3] = ELEMENT_DONE;	//C'est bon pour le 4ème cadeau !
			ACT_hammer_goto(HAMMER_POSITION_HOME);		//BAISSER BRAS
			state = DONE;
		break;
		case DONE:
			ret = END_OK;
			state = INIT;
		break;
		case FAIL:
			//L'angle choisi en cas de rencontre avec l'adversaire est celui dans lequel le bras est du coté de notre zone !
			state = try_go_angle((global.env.color == RED)?PI4096:0, FAIL, FAIL_HAMMER_HOME, FAIL_HAMMER_HOME, FAST);
		break;
		case FAIL_HAMMER_HOME:
			ACT_hammer_goto(HAMMER_POSITION_HOME);		//BAISSER BRAS
			state = INIT;
			ret = NOT_HANDLED;
		break;
		default:
		break;

	}

	if(state != previous_state)
	{
		debug_printf("T_gifts->");
		switch(state)
		{
			case INIT:				debug_printf("INIT\n");			 	break;
			case OPENING_GIFTS:		debug_printf("OPENING_GIFTS\n");	break;
			case HAMMER_HOME:		debug_printf("HAMMER_HOME\n");		break;
			case DONE:				debug_printf("DONE\n");			 	break;
			case FAIL:				debug_printf("FAIL\n");			 	break;
			case FAIL_HAMMER_HOME:	debug_printf("FAIL_HAMMER_HOME\n");	break;
			default:				debug_printf("???\n");				break;
		}

	}
	previous_state = state;

	return ret;
}


error_e TINY_forgotten_gift(map_goal_e forgotten_gift)
{
	typedef enum
	{
		INIT=0,
		GOTO_GIFT,
		ANGLE_HAMMER,
		HAMMER_UP,
		WAIT_HAMMER,
		HAMMER_DOWN,
		WAIT_HAMMER_DOWN,
		FAIL,
		DONE
	}state_e;

	static state_e state = INIT;
	error_e ret = IN_PROGRESS;

	switch(state)
	{
		case INIT:
			state = ANGLE_HAMMER;
			break;

		//case GOTO_GIFT:
		//	state=try_going(200, COLOR_Y((forgotten_gift==GOAL_Cadeau2)?1700:2300),GOTO_GIFT,ANGLE_HAMMER,FAIL, ANY_WAY, NO_DODGE_AND_WAIT);
		//break;

		case ANGLE_HAMMER:
			state=try_go_angle(PI4096/2, ANGLE_HAMMER, HAMMER_UP, FAIL, FAST);
		break;

		case HAMMER_UP:
				ACT_hammer_goto(HAMMER_POSITION_UP); 	//LEVER le bras
				state=WAIT_HAMMER;
		break;

		case WAIT_HAMMER:
			//						->In progress	->Success		->Fail
			state = wait_hammer(	WAIT_HAMMER,	HAMMER_DOWN,	FAIL);
		break;
		case HAMMER_DOWN:
			ACT_hammer_goto(HAMMER_POSITION_DOWN); 	//BAISSER BRAS
			state=WAIT_HAMMER_DOWN;
			break;
		case WAIT_HAMMER_DOWN:
			//					->In progress		->Success	->Fail
			state = wait_hammer(WAIT_HAMMER_DOWN,	DONE,		FAIL);
		break;
		case FAIL:
			state = INIT;
			ret = NOT_HANDLED;
		break;

		case DONE:
			global.env.map_elements[forgotten_gift] = ELEMENT_DONE;
			state = INIT;
			ret = END_OK;
		break;

		default:
		break;

	}
	if(ret != IN_PROGRESS)
		state = INIT;
	return ret;
}



void TINY_hammer_open_all_gift(bool_e reset)
{
	typedef enum
	{
		INIT=0,
		WAIT_FIRST_GIFT,
		OPENING_FIRST_GIFT,
		WAIT_SECOND_GIFT,
		OPENING_SECOND_GIFT,
		WAIT_THIRD_GIFT,
		OPENING_THIRD_GIFT,
		WAIT_FOURTH_GIFT,
		OPENING_FOURTH_GIFT,
		ALL_GIFTS_OPENED
	}state_e;
	static state_e state = INIT;

	if(reset)
		state = INIT;
	switch(state)
	{
		case INIT:
			ACT_hammer_goto(HAMMER_POSITION_DOWN);		//Bras : READY !
			PROP_WARNER_arm_y(COLOR_Y(480));	//On arme et on attend le premier cadeau.
			state = WAIT_FIRST_GIFT;
		break;
		case WAIT_FIRST_GIFT:
			if(global.env.prop.reach_y)
			{
				ACT_hammer_goto(HAMMER_POSITION_UP); 	//LEVER BRAS
				PROP_WARNER_arm_y(COLOR_Y(800));
				state = OPENING_FIRST_GIFT;
			}
		break;
		case OPENING_FIRST_GIFT:
			if(global.env.prop.reach_y)
			{
				global.env.map_elements[GOAL_Cadeau0] = ELEMENT_DONE;
				ACT_hammer_goto(HAMMER_POSITION_DOWN);		//BAISSER BRAS
				PROP_WARNER_arm_y(COLOR_Y(1000));
				state = WAIT_SECOND_GIFT;
			}
		break;
		case WAIT_SECOND_GIFT:
			if(global.env.prop.reach_y)
			{
				ACT_hammer_goto(HAMMER_POSITION_UP); 	//LEVER BRAS
				PROP_WARNER_arm_y(COLOR_Y(1400));
				state = OPENING_SECOND_GIFT;
			}
		break;
		case OPENING_SECOND_GIFT:
			if(global.env.prop.reach_y)
			{
				global.env.map_elements[GOAL_Cadeau1] = ELEMENT_DONE;
				ACT_hammer_goto(HAMMER_POSITION_DOWN);		//BAISSER BRAS
				PROP_WARNER_arm_y(COLOR_Y(1580));
				state = WAIT_THIRD_GIFT;
			}
		break;
		case WAIT_THIRD_GIFT:
			if(global.env.prop.reach_y)
			{
				ACT_hammer_goto(HAMMER_POSITION_UP); 	//LEVER BRAS
				PROP_WARNER_arm_y(COLOR_Y(2000));
				state = OPENING_THIRD_GIFT;
			}
		break;
		case OPENING_THIRD_GIFT:
			if(global.env.prop.reach_y)
			{
				global.env.map_elements[GOAL_Cadeau2] = ELEMENT_DONE;
				ACT_hammer_goto(HAMMER_POSITION_DOWN);		//BAISSER BRAS
				PROP_WARNER_arm_y(COLOR_Y(2180));
				state = WAIT_FOURTH_GIFT;
			}
		break;
		case WAIT_FOURTH_GIFT:
			if(global.env.prop.reach_y)
			{
				ACT_hammer_goto(HAMMER_POSITION_UP); 	//LEVER BRAS
				//PROP_WARNER_arm_y(COLOR_Y(2600));
				state = OPENING_FOURTH_GIFT;
			}
		break;
		case OPENING_FOURTH_GIFT:
			state = ALL_GIFTS_OPENED;
		break;
		case ALL_GIFTS_OPENED:
			//Nothing to do.
		break;
		default:
		break;

	}
}


error_e TINY_open_all_gifts_homolog(void)
{
	typedef enum
	{
		INIT=0,
		OPENING_GIFTS
	}state_e;
	static state_e state = INIT;

	error_e ret = IN_PROGRESS;
	error_e sub_action;
	static avoidance_type_e avoidance = NO_AVOIDANCE;

	switch(state)
	{
		case INIT:
			TINY_hammer_open_all_gift(TRUE);
			avoidance = DODGE_AND_NO_WAIT;
			state = OPENING_GIFTS;
		break;
		case OPENING_GIFTS:

			TINY_hammer_open_all_gift(FALSE);	//Gestion du mouvement du bras...

			if(		(global.env.color == BLUE && (global.env.pos.y < 1800))
				||	(global.env.color == RED && (global.env.pos.y > 1200))  )
				avoidance = NO_DODGE_AND_WAIT;	//Activation de l'évitement à partir du franchissement du second cadeau

			sub_action = goto_pos_with_scan_foe((displacement_t[]){{{250,COLOR_Y(400)},SLOW},{{200,COLOR_Y(600)},SLOW},{{200,COLOR_Y(2400)},SLOW}},3,(global.env.color==BLUE)?BACKWARD:FORWARD,avoidance);
			switch(sub_action)
			{
				case END_OK:
					ret = END_OK;
					state = INIT;
				break;
				case END_WITH_TIMEOUT:	//Echec de la mission
				case NOT_HANDLED:		//Echec de la mission
				case FOE_IN_PATH:
					ret = sub_action;
					state = INIT;
				break;
				case IN_PROGRESS:
				break;
				default:
				break;
			}
		break;

		default:
		break;

	}
	return ret;


}




#define X_TO_OPEN_GIFT	160
#define OPEN_ALL_GIFTS_NB_TRY 3
error_e TINY_open_all_gifts(void)
{
	typedef enum
	{
		INIT=0,
		GET_OUT,
		GOTO_FIRST_GIFT,
		ANGLE_FIRST_GIFT,
		OPEN_FIRST_GIFT,
		HAMMER_FIRST_GIFT,
		WAIT_HAMMER_DOWN_FIRST_GIFT,
		GOTO_SECOND_GIFT,
		OPEN_SECOND_GIFT,
		HAMMER_SECOND_GIFT,
		WAIT_HAMMER_DOWN_SECOND_GIFT,
		GOTO_THIRD_GIFT,
		OPEN_THIRD_GIFT,
		HAMMER_THIRD_GIFT,
		WAIT_HAMMER_DOWN_THIRD_GIFT,
		GOTO_FOURTH_GIFT,
		OPEN_FOURTH_GIFT,
		HAMMER_FOURTH_GIFT,
		WAIT_HAMMER_DOWN_FOURTH_GIFT,
		HAMMER_FAIL,
		PROPULSION_OR_AVOIDANCE_FAIL,
		TIMEOUT_FAIL,
		ALL_GIFTS_OPENED,
	}state_e;
	static state_e state = INIT;
	static state_e previous_state = INIT;

	error_e ret = IN_PROGRESS;
	error_e sub_action;
	static avoidance_type_e avoidance = NO_AVOIDANCE;
	static Uint8 nb_try = 0;

	if(COLOR_Y(global.env.pos.y) > 600)
		avoidance = NO_DODGE_AND_WAIT;	//Activation de l'évitement à partir du franchissement du first cadeau parce quen face c'est des momosexuels qui tracent.

	switch(state)
	{
		case INIT:
			avoidance = NO_AVOIDANCE;
			nb_try = 0;
			previous_state = INIT;	//facultatif
			state = GET_OUT;
		break;
		case GET_OUT:
			sub_action = goto_pos(250,COLOR_Y(360),SLOW,(global.env.color==BLUE)?BACKWARD:FORWARD,END_AT_LAST_POINT);
			switch(sub_action)
			{
				case END_OK:
					state = GOTO_FIRST_GIFT;
				break;
				case END_WITH_TIMEOUT:	//Echec de la mission
					previous_state = state;
					state = TIMEOUT_FAIL;
				break;
				case NOT_HANDLED:		//Echec de la mission
				case FOE_IN_PATH:
					previous_state = state;
					state = PROPULSION_OR_AVOIDANCE_FAIL;
				break;
				case IN_PROGRESS:
				break;
				default:
				break;
			}
		break;


		//-------------------------CADEAU n°1--------------------------------------

		case GOTO_FIRST_GIFT:
			sub_action = goto_pos(X_TO_OPEN_GIFT,COLOR_Y(500),SLOW,(global.env.color==BLUE)?BACKWARD:FORWARD,END_AT_LAST_POINT);
			switch(sub_action)
			{
				case END_OK:
					state = ANGLE_FIRST_GIFT;
				break;
				case END_WITH_TIMEOUT:	//Echec de la mission
					previous_state = state;
					state = TIMEOUT_FAIL;
				break;
				case NOT_HANDLED:		//Echec de la mission
				case FOE_IN_PATH:
					previous_state = state;
					state = PROPULSION_OR_AVOIDANCE_FAIL;
				break;
				case IN_PROGRESS:
				break;
				default:
				break;
			}
		break;
		case ANGLE_FIRST_GIFT:
			sub_action = goto_angle(PI4096/2,VERY_SLOW_TRANSLATION_AND_FAST_ROTATION);

			switch(sub_action)
			{
				case END_OK:
					state = OPEN_FIRST_GIFT;
				break;
				case END_WITH_TIMEOUT:	//Echec de la mission
					previous_state = state;
					state = TIMEOUT_FAIL;
				break;
				case NOT_HANDLED:		//Echec de la mission
					previous_state = state;
					state = PROPULSION_OR_AVOIDANCE_FAIL;
				break;
				case IN_PROGRESS:
				break;
				default:
				break;
			}
		break;
		case OPEN_FIRST_GIFT:
			ACT_hammer_goto(HAMMER_POSITION_UP); 	//LEVER BRAS
			state = HAMMER_FIRST_GIFT;
		break;
		case HAMMER_FIRST_GIFT:
			//						->In progress			->Success						->Fail
			state = wait_hammer(	HAMMER_FIRST_GIFT,		WAIT_HAMMER_DOWN_FIRST_GIFT,	HAMMER_FAIL);
			if(state == WAIT_HAMMER_DOWN_FIRST_GIFT)
				ACT_hammer_goto(HAMMER_POSITION_DOWN); 	//BAISSER BRAS
		break;
		case WAIT_HAMMER_DOWN_FIRST_GIFT:
			//						->In progress					->Success			->Fail
			state = wait_hammer(	WAIT_HAMMER_DOWN_FIRST_GIFT,	GOTO_SECOND_GIFT,	HAMMER_FAIL);
		break;



		//-------------------------CADEAU n°2--------------------------------------

		case GOTO_SECOND_GIFT:
			sub_action = goto_pos(X_TO_OPEN_GIFT,COLOR_Y(1100),SLOW,(global.env.color==BLUE)?BACKWARD:FORWARD,END_AT_LAST_POINT);
			switch(sub_action)
			{
				case END_OK:
					state = OPEN_SECOND_GIFT;
				break;
				case END_WITH_TIMEOUT:	//Echec de la mission
					previous_state = state;
					state = TIMEOUT_FAIL;
				break;
				case NOT_HANDLED:		//Echec de la mission
				case FOE_IN_PATH:
					previous_state = state;
					state = PROPULSION_OR_AVOIDANCE_FAIL;
				break;
				case IN_PROGRESS:
				break;
				default:
				break;
			}
		break;
		case OPEN_SECOND_GIFT:
			ACT_hammer_goto(HAMMER_POSITION_UP); 	//LEVER BRAS
			state = HAMMER_SECOND_GIFT;
		break;
		case HAMMER_SECOND_GIFT:
			//						->In progress			->Success						->Fail
			state = wait_hammer(	HAMMER_SECOND_GIFT,		WAIT_HAMMER_DOWN_SECOND_GIFT,	HAMMER_FAIL);
			if(state == WAIT_HAMMER_DOWN_SECOND_GIFT)
				ACT_hammer_goto(HAMMER_POSITION_DOWN); 	//BAISSER BRAS
		break;
		case WAIT_HAMMER_DOWN_SECOND_GIFT:
			//						->In progress					->Success			->Fail
			state = wait_hammer(	WAIT_HAMMER_DOWN_SECOND_GIFT,	GOTO_THIRD_GIFT,	HAMMER_FAIL);
		break;


		//-------------------------CADEAU n°3--------------------------------------

		case GOTO_THIRD_GIFT:
		//Cette action doit se terminer lorsque le robot arrive, et pas lorsqu'il freine.
			sub_action = goto_pos_with_scan_foe((displacement_t[]){{{X_TO_OPEN_GIFT,COLOR_Y(1700)},SLOW}},1,(global.env.color==BLUE)?BACKWARD:FORWARD,avoidance);
			switch(sub_action)
			{
				case END_OK:
					state = OPEN_THIRD_GIFT;
				break;
				case END_WITH_TIMEOUT:	//Echec de la mission
					previous_state = state;
					state = TIMEOUT_FAIL;
				break;
				case NOT_HANDLED:		//Echec de la mission
				case FOE_IN_PATH:
					previous_state = state;
					state = PROPULSION_OR_AVOIDANCE_FAIL;
				break;
				case IN_PROGRESS:
				break;
				default:
				break;
			}
		break;
		case OPEN_THIRD_GIFT:
			ACT_hammer_goto(HAMMER_POSITION_UP); 	//LEVER BRAS
			state = HAMMER_THIRD_GIFT;
		break;
		case HAMMER_THIRD_GIFT:
			//						->In progress			->Success						->Fail
			state = wait_hammer(	HAMMER_THIRD_GIFT,		WAIT_HAMMER_DOWN_THIRD_GIFT,	HAMMER_FAIL);
			if(state == WAIT_HAMMER_DOWN_THIRD_GIFT)
				ACT_hammer_goto(HAMMER_POSITION_DOWN); 	//BAISSER BRAS
		break;
		case WAIT_HAMMER_DOWN_THIRD_GIFT:
			//						->In progress					->Success			->Fail
			state = wait_hammer(	WAIT_HAMMER_DOWN_THIRD_GIFT,	GOTO_FOURTH_GIFT,	HAMMER_FAIL);
		break;

		//-------------------------CADEAU n°4--------------------------------------

		case GOTO_FOURTH_GIFT:	sub_action = goto_pos_with_scan_foe((displacement_t[]){{{X_TO_OPEN_GIFT,COLOR_Y(2300)},SLOW}},1,(global.env.color==BLUE)?BACKWARD:FORWARD,avoidance);
			switch(sub_action)
			{
				case END_OK:
					state = OPEN_FOURTH_GIFT;
				break;
				case END_WITH_TIMEOUT:	//Echec de la mission
					previous_state = state;
					state = TIMEOUT_FAIL;
				break;
				case NOT_HANDLED:		//Echec de la mission
				case FOE_IN_PATH:
					previous_state = state;
					state = PROPULSION_OR_AVOIDANCE_FAIL;
				break;
				case IN_PROGRESS:
				break;
				default:
				break;
			}
		break;
		case OPEN_FOURTH_GIFT:
			ACT_hammer_goto(HAMMER_POSITION_UP); 	//LEVER BRAS
			state = HAMMER_FOURTH_GIFT;
		break;
		case HAMMER_FOURTH_GIFT:
			//						->In progress			->Success						->Fail
			state = wait_hammer(	HAMMER_FOURTH_GIFT,		WAIT_HAMMER_DOWN_FOURTH_GIFT,	HAMMER_FAIL);
			if(state == WAIT_HAMMER_DOWN_FOURTH_GIFT)
				ACT_hammer_goto(HAMMER_POSITION_DOWN); 	//BAISSER BRAS
		break;
		case WAIT_HAMMER_DOWN_FOURTH_GIFT:
			//						->In progress					->Success			->Fail
			state = wait_hammer(	WAIT_HAMMER_DOWN_FOURTH_GIFT,	ALL_GIFTS_OPENED,	HAMMER_FAIL);
		break;



		case ALL_GIFTS_OPENED:
			ret = END_OK;
		break;
		case HAMMER_FAIL:
			ret = NOT_HANDLED;
		break;
		case TIMEOUT_FAIL:					//@pre IL FAUT AVOIR RENSEIGNE LE previous_state
			nb_try++;
			if(nb_try < OPEN_ALL_GIFTS_NB_TRY)
				state = previous_state;	//Timeout ?? -> On y retourne !
			else
				ret = END_WITH_TIMEOUT;
		break;
		case PROPULSION_OR_AVOIDANCE_FAIL:	//@pre IL FAUT AVOIR RENSEIGNE LE previous_state
			nb_try++;
			if(nb_try < OPEN_ALL_GIFTS_NB_TRY)
				state = previous_state;	//Failed ?? -> On y retourne !
			else
				ret = NOT_HANDLED;
		break;
		default:
		break;

	}
	if(ret != IN_PROGRESS)
		state = INIT;
	return ret;
}
#endif
