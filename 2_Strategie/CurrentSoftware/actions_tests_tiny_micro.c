/*
 *	Club Robot ESEO 2008 - 2012
 *	Archi'Tech, CHOMP, Shark & Fish, Krusty & Tiny
 *
 *	Fichier : actions_tests_tiny_micro.c
 *	Package : Carte Principale
 *	Description : Tests des actions r�alisables par le robot
 *	Auteur : Cyril, modifi� par Vincent , Antoine , LMG , Herzaeone , Hoobbes
 *	Version 2012/01/14
 */

#define ACTIONS_TEST_TINY_MICRO_C

#include "actions_tests_tiny_micro.h"


#define DEFAULT_SPEED	(SLOW)

#define USE_CURVE	0





/* ----------------------------------------------------------------------------- */
/* 						Actions �l�mentaires de construction                     */
/* ----------------------------------------------------------------------------- */

//Cette fonction est appel�e sur TINY � la fin du match.
void T_BALLINFLATER_start(void)
{
	ACT_ball_inflater_inflate(10);
}	

/* ----------------------------------------------------------------------------- */
/* 								Fonction diverses                     			 */
/* ----------------------------------------------------------------------------- */

#define HAMMER_POSITION_UP		(0)
#define HAMMER_POSITION_DOWN	(85)
#define HAMMER_POSITION_HOME	(90)
				
void TINY_hammer_open_all_gift(bool_e reset)
{
	static enum
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
	}state = INIT;

	if(reset)
		state = INIT;
	switch(state)
	{
		case INIT:
			ACT_hammer_goto(HAMMER_POSITION_DOWN);		//Bras : READY !
			ASSER_WARNER_arm_y(COLOR_Y(400));	//On arme et on attend le premier cadeau.
			state = WAIT_FIRST_GIFT;
		break;
		case WAIT_FIRST_GIFT:		
			if(global.env.asser.reach_y)
			{
				ACT_hammer_goto(HAMMER_POSITION_UP); 	//LEVER BRAS
				ASSER_WARNER_arm_y(COLOR_Y(600));
				state = OPENING_FIRST_GIFT;
			}			
		break;
		case OPENING_FIRST_GIFT:
			if(global.env.asser.reach_y)
			{
				//TODO pr�venir environnement
				global.env.map_elements[GOAL_Cadeau0] = DONE;
				ACT_hammer_goto(HAMMER_POSITION_DOWN);		//BAISSER BRAS
				ASSER_WARNER_arm_y(COLOR_Y(1000));
				state = WAIT_SECOND_GIFT;
			}
		break;
		case WAIT_SECOND_GIFT:		
			if(global.env.asser.reach_y)
			{
				ACT_hammer_goto(HAMMER_POSITION_UP); 	//LEVER BRAS
				ASSER_WARNER_arm_y(COLOR_Y(1200));
				state = OPENING_SECOND_GIFT;
			}			
		break;
		case OPENING_SECOND_GIFT:
			if(global.env.asser.reach_y)
			{
				//TODO pr�venir environnement
				global.env.map_elements[GOAL_Cadeau1] = DONE;
				ACT_hammer_goto(HAMMER_POSITION_DOWN);		//BAISSER BRAS
				ASSER_WARNER_arm_y(COLOR_Y(1600));
				state = WAIT_THIRD_GIFT;
			}
		break;
		case WAIT_THIRD_GIFT:		
			if(global.env.asser.reach_y)
			{
				ACT_hammer_goto(HAMMER_POSITION_UP); 	//LEVER BRAS
				ASSER_WARNER_arm_y(COLOR_Y(1800));
				state = OPENING_THIRD_GIFT;
			}			
		break;
		case OPENING_THIRD_GIFT:
			if(global.env.asser.reach_y)
			{
				//TODO pr�venir environnement
				global.env.map_elements[GOAL_Cadeau2] = DONE;
				ACT_hammer_goto(HAMMER_POSITION_DOWN);		//BAISSER BRAS
				ASSER_WARNER_arm_y(COLOR_Y(2200));
				state = WAIT_FOURTH_GIFT;
			}
		break;
		case WAIT_FOURTH_GIFT:		
			if(global.env.asser.reach_y)
			{
				ACT_hammer_goto(HAMMER_POSITION_UP); 	//LEVER BRAS
				ASSER_WARNER_arm_y(COLOR_Y(2400));
				state = OPENING_FOURTH_GIFT;
			}			
		break;
		case OPENING_FOURTH_GIFT:
			if(global.env.asser.reach_y)
			{
				//TODO pr�venir environnement
				global.env.map_elements[GOAL_Cadeau3] = DONE;
				ACT_hammer_goto(HAMMER_POSITION_HOME);		//BAISSER BRAS
				state = ALL_GIFTS_OPENED;
			}
		break;
		case ALL_GIFTS_OPENED:
			//Nothing to do.
		break;
		default:
		break;
		
	}
}	

/* ----------------------------------------------------------------------------- */
/* 							D�coupe de strat�gies                     			 */
/* ----------------------------------------------------------------------------- */

/*
1- ATTENTION, point de d�part impos�, du cot� de notre couleur
2- TINY descend tout les cadeaux lors d'un seule trajectoire
		Si rencontre adversaire -> NOT_HANDLED
3- A la fin du travail -> OK
-> A chaque cadeau descendu : mise � jour de l'environnement.
*/
error_e TINY_open_all_gifts_without_pause(void)
{
	static enum
	{
		INIT=0,
		OPENING_GIFTS
	}state = INIT;	

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
			
			if(COLOR_Y(global.env.pos.y) > 1200)
				avoidance = NO_DODGE_AND_WAIT;	//Activation de l'�vitement � partir du franchissement du second cadeau
		
			sub_action = goto_pos_with_scan_foe((displacement_t[]){{{250,COLOR_Y(400)},FAST},{{140,COLOR_Y(600)},FAST},{{140,COLOR_Y(2400)},FAST}},3,(global.env.color==BLUE)?BACKWARD:FORWARD,avoidance);
			switch(sub_action)
            {
				case END_OK:
					ret = END_OK;
					state = INIT;
				break;
				case END_WITH_TIMEOUT:	//Echec de la mission
				case NOT_HANDLED:		//Echec de la mission
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

Uint16 wait_hammer(Uint16 progress, Uint16 success, Uint16 fail)
{
	Uint16 ret = progress;
	switch(ACT_get_last_action_result(ACT_QUEUE_Hammer))
	{
		case ACT_FUNCTION_Done:
			ret = success;
		break;
		case ACT_FUNCTION_ActDisabled:
		case ACT_FUNCTION_RetryLater:
			ret = fail;
		break;
		case ACT_FUNCTION_InProgress:
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

	if(COLOR_Y(global.env.pos.y) > 1200)
		avoidance = NO_DODGE_AND_WAIT;	//Activation de l'�vitement � partir du franchissement du second cadeau

	switch(state)
	{
		case INIT:
			avoidance = NO_AVOIDANCE;
			nb_try = 0;
			previous_state = INIT;	//facultatif
			state = GET_OUT;
		break;
		case GET_OUT:
			sub_action = goto_pos(250,COLOR_Y(350),FAST,(global.env.color==BLUE)?BACKWARD:FORWARD,END_AT_LAST_POINT);
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
					previous_state = state;
					state = PROPULSION_OR_AVOIDANCE_FAIL;
				break;
				case IN_PROGRESS:
				break;
				default:
				break;
            }
		break;


		//-------------------------CADEAU n�1--------------------------------------

		case GOTO_FIRST_GIFT:
			sub_action = goto_pos(X_TO_OPEN_GIFT,COLOR_Y(500),FAST,(global.env.color==BLUE)?BACKWARD:FORWARD,END_AT_LAST_POINT);
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
			sub_action = goto_angle(PI4096/2,FAST);
			
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



		//-------------------------CADEAU n�2--------------------------------------

		case GOTO_SECOND_GIFT:
			sub_action = goto_pos(X_TO_OPEN_GIFT,COLOR_Y(1100),FAST,(global.env.color==BLUE)?BACKWARD:FORWARD,END_AT_LAST_POINT);
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


		//-------------------------CADEAU n�3--------------------------------------

		case GOTO_THIRD_GIFT:
		//Cette action doit se terminer lorsque le robot arrive, et pas lorsqu'il freine.
			sub_action = goto_pos_with_scan_foe((displacement_t[]){{{X_TO_OPEN_GIFT,COLOR_Y(1700)},FAST}},1,(global.env.color==BLUE)?BACKWARD:FORWARD,avoidance);
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

		//-------------------------CADEAU n�4--------------------------------------

		case GOTO_FOURTH_GIFT:	sub_action = goto_pos_with_scan_foe((displacement_t[]){{{X_TO_OPEN_GIFT,COLOR_Y(2300)},FAST}},1,(global.env.color==BLUE)?BACKWARD:FORWARD,avoidance);
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

	
//Souffle toutes les bougies de notre couleur, sans s'arr�ter
//  pr�condition : il faut �tre positionn� du cot� ROUGE pour commencer
error_e TINY_blow_candles_from_red_to_blue(void)
{
	return NOT_HANDLED;
}	

//Souffle toutes les bougies de notre couleur, sans s'arr�ter
//  pr�condition : il faut �tre positionn� du cot� BLEU pour commencer
error_e TINY_blow_candles_from_blue_to_red(void)
{
	return NOT_HANDLED;
}

/*
	Le gateau est divis� en 4 parts...
		QUATER_BLUE, MIDLE_BLUE, MIDLE_RED, QUATER_RED
		
|		cot� BLUE
|----
|    \		QUATER_BLUE
|     \		--------------
|      |	MIDLE_BLUE
|      |    -----------
|      |	MIDLE_RED
|     /		---------
|    /		QUATER_RED
|----
|		cot� RED
*/
typedef enum
{
	CAKE_PART_QUATER_BLUE,
	CAKE_PART_MIDLE_BLUE,
	CAKE_PART_MIDLE_RED,
	CAKE_PART_QUATER_RED
}cake_part_e;
/*
Souffler un quart du gateau :
1- se rendre sur place
2- souffler la part du gateau demand�e
3- mise � jour de l'environnement � chaque bougie souffl�e, et lorsqu'un quart est termin�
*/
error_e TINY_blow_quater(cake_part_e cake_part)
{
	return NOT_HANDLED;
}
	

error_e TINY_open_all_gifts_homolog(void)
{
	static enum
	{
		INIT=0,
		OPENING_GIFTS
	}state = INIT;

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
				avoidance = NO_DODGE_AND_WAIT;	//Activation de l'�vitement � partir du franchissement du second cadeau

			sub_action = goto_pos_with_scan_foe((displacement_t[]){{{250,COLOR_Y(400)},FAST},{{200,COLOR_Y(600)},FAST},{{200,COLOR_Y(2400)},FAST}},3,(global.env.color==BLUE)?BACKWARD:FORWARD,avoidance);
			switch(sub_action)
            {
				case END_OK:
					ret = END_OK;
					state = INIT;
				break;
				case END_WITH_TIMEOUT:	//Echec de la mission
				case NOT_HANDLED:		//Echec de la mission
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








error_e TINY_white_candles(void)
{
	typedef enum
	{
		INIT=0,
                OPEN_HAMMER_FIRST_CANDLE_1,
                        GOTO_FIRST_WHITE_CANDLE,
		HAMMER_FIRST_CANDLE,
		WAIT_HAMMER_DOWN_FIRST_CANDLE,
		OPEN_HAMMER_FIRST_CANDLE_2,
                /*        GOTO_SECOND_WHITE_CANDLE,
		OPEN_HAMMER_SECOND_CANDLE_1,
		HAMMER_SECOND_CANDLE,
		WAIT_HAMMER_DOWN_SECOND_CANDLE,
		OPEN_HAMMER_SECOND_CANDLE_2,
                        GOTO_THIRD_WHITE_CANDLE,
		OPEN_HAMMER_THIRD_CANDLE_1,
		HAMMER_THIRD_CANDLE,
		WAIT_HAMMER_DOWN_THIRD_CANDLE,
		OPEN_HAMMER_THIRD_CANDLE_2,
                        GOTO_FOURTH_WHITE_CANDLE,
		OPEN_HAMMER_FOURTH_CANDLE_1,
		HAMMER_FOURTH_CANDLE,
		WAIT_HAMMER_DOWN_FOURTH_CANDLE,
		OPEN_HAMMER_FOURTH_CANDLE_2,*/
		HAMMER_FAIL,
		PROPULSION_OR_AVOIDANCE_FAIL,
		TIMEOUT_FAIL,
		ALL_CANDLES_BLOWN,
	}state_e;
	static state_e state = INIT;
	static state_e previous_state = INIT;

	error_e ret = IN_PROGRESS;
	error_e sub_action;
	static avoidance_type_e avoidance = NO_DODGE_AND_NO_WAIT;
	static Uint8 nb_try = 0;


	switch(state)
	{
		case INIT:
			sub_action = goto_pos_with_scan_foe((displacement_t[]){{{1700,COLOR_Y(800)},FAST}},1,(global.env.color==BLUE)?FORWARD:BACKWARD,avoidance);
			switch(sub_action)
                        {
				case END_OK:
					state = OPEN_HAMMER_FIRST_CANDLE_1;
				break;
				case END_WITH_TIMEOUT:	//Echec de la mission
                                        state = OPEN_HAMMER_FIRST_CANDLE_1;
					//previous_state = state;
					//state = TIMEOUT_FAIL;
				break;
				case NOT_HANDLED:		//Echec de la mission
                                        state = OPEN_HAMMER_FIRST_CANDLE_1;
					//previous_state = state;
					//state = PROPULSION_OR_AVOIDANCE_FAIL;
				break;
				case IN_PROGRESS:
				break;
				default:
				break;
                         }
                        break;

                case OPEN_HAMMER_FIRST_CANDLE_1:
			ACT_hammer_goto(HAMMER_POSITION_UP); 	//LEVER BRAS
			state = GOTO_FIRST_WHITE_CANDLE;
		break;

		//-------------------------FIRST GODAMN WHITY CANDLE--------------------------------------

		case GOTO_FIRST_WHITE_CANDLE:
			sub_action = goto_pos_with_scan_foe((displacement_t[]){{{1369,COLOR_Y(1346)},FAST}},1,(global.env.color==BLUE)?FORWARD:BACKWARD,avoidance);
			switch(sub_action)
                        {
				case END_OK:
					state = HAMMER_FIRST_CANDLE;
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

		case HAMMER_FIRST_CANDLE:
			state = wait_hammer(	HAMMER_FIRST_CANDLE,		WAIT_HAMMER_DOWN_FIRST_CANDLE,	HAMMER_FAIL);
			if(state == WAIT_HAMMER_DOWN_FIRST_CANDLE)
				ACT_hammer_goto(HAMMER_POSITION_DOWN); 	//BAISSER BRAS
		break;
		case WAIT_HAMMER_DOWN_FIRST_CANDLE:
			//						->In progress			->Success						->Fail
			state = wait_hammer(	WAIT_HAMMER_DOWN_FIRST_CANDLE,	OPEN_HAMMER_FIRST_CANDLE_2,	HAMMER_FAIL);
		break;
		case OPEN_HAMMER_FIRST_CANDLE_2:
			//						->In progress					->Success			->Fail
			ACT_hammer_goto(HAMMER_POSITION_UP); 	//LEVER BRAS ENCORE UNE FOIS
			state = ALL_CANDLES_BLOWN;
		break;


		case ALL_CANDLES_BLOWN:
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

