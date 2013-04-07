/*
 *	Club Robot ESEO 2008 - 2012
 *	Archi'Tech, CHOMP, Shark & Fish, Krusty & Tiny
 *
 *	Fichier : actions_tests_tiny_micro.c
 *	Package : Carte Principale
 *	Description : Tests des actions réalisables par le robot
 *	Auteur : Cyril, modifié par Vincent , Antoine , LMG , Herzaeone , Hoobbes
 *	Version 2012/01/14
 */

#define ACTIONS_TEST_TINY_MICRO_C

#include "actions_tests_tiny_micro.h"


#define DEFAULT_SPEED	(SLOW)

#define USE_CURVE	0





/* ----------------------------------------------------------------------------- */
/* 						Actions élémentaires de construction                     */
/* ----------------------------------------------------------------------------- */

//Cette fonction est appelée sur TINY à la fin du match.
void T_BALLINFLATER_start(void)
{
	ACT_ball_inflater_inflate(10);
}	

/* ----------------------------------------------------------------------------- */
/* 								Fonction diverses                     			 */
/* ----------------------------------------------------------------------------- */

#define HAMMER_POSITION_UP		(0)
#define HAMMER_POSITION_DOWN	(30)
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
				ASSER_WARNER_arm_y(COLOR_Y(500));
				state = OPENING_FIRST_GIFT;
			}			
		break;
		case OPENING_FIRST_GIFT:
			if(global.env.asser.reach_y)
			{
				//TODO prévenir environnement
				ACT_hammer_goto(HAMMER_POSITION_DOWN);		//BAISSER BRAS
				ASSER_WARNER_arm_y(COLOR_Y(1000));
				state = WAIT_SECOND_GIFT;
			}
		break;
		case WAIT_SECOND_GIFT:		
			if(global.env.asser.reach_y)
			{
				ACT_hammer_goto(HAMMER_POSITION_UP); 	//LEVER BRAS
				ASSER_WARNER_arm_y(COLOR_Y(1100));
				state = OPENING_SECOND_GIFT;
			}			
		break;
		case OPENING_SECOND_GIFT:
			if(global.env.asser.reach_y)
			{
				//TODO prévenir environnement
				ACT_hammer_goto(HAMMER_POSITION_DOWN);		//BAISSER BRAS
				ASSER_WARNER_arm_y(COLOR_Y(1600));
				state = WAIT_THIRD_GIFT;
			}
		break;
		case WAIT_THIRD_GIFT:		
			if(global.env.asser.reach_y)
			{
				ACT_hammer_goto(HAMMER_POSITION_UP); 	//LEVER BRAS
				ASSER_WARNER_arm_y(COLOR_Y(1700));
				state = OPENING_THIRD_GIFT;
			}			
		break;
		case OPENING_THIRD_GIFT:
			if(global.env.asser.reach_y)
			{
				//TODO prévenir environnement
				ACT_hammer_goto(HAMMER_POSITION_DOWN);		//BAISSER BRAS
				ASSER_WARNER_arm_y(COLOR_Y(2200));
				state = WAIT_FOURTH_GIFT;
			}
		break;
		case WAIT_FOURTH_GIFT:		
			if(global.env.asser.reach_y)
			{
				ACT_hammer_goto(HAMMER_POSITION_UP); 	//LEVER BRAS
				ASSER_WARNER_arm_y(COLOR_Y(2300));
				state = OPENING_FOURTH_GIFT;
			}			
		break;
		case OPENING_FOURTH_GIFT:
			if(global.env.asser.reach_y)
			{
				//TODO prévenir environnement
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
/* 							Découpe de stratégies                     			 */
/* ----------------------------------------------------------------------------- */

/*
1- ATTENTION, point de départ imposé, du coté de notre couleur
2- TINY descend tout les cadeaux lors d'un seule trajectoire
		Si rencontre adversaire -> NOT_HANDLED
3- A la fin du travail -> OK
-> A chaque cadeau descendu : mise à jour de l'environnement.
*/
error_e TINY_open_all_gifts(void)
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
			
			if(		(global.env.color == BLUE && (global.env.pos.y < 1800))
				||	(global.env.color == RED && (global.env.pos.y > 1200))  )
				avoidance = NO_DODGE_AND_WAIT;	//Activation de l'évitement à partir du franchissement du second cadeau
		
			sub_action = goto_pos_with_scan_foe((displacement_t[]){{{250,COLOR_Y(300)},FAST},{{200,COLOR_Y(600)},FAST},{{200,COLOR_Y(2400)},FAST}},3,(global.env.color==BLUE)?BACKWARD:FORWARD,avoidance);
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

	
//Souffle toutes les bougies de notre couleur, sans s'arrêter
//  précondition : il faut être positionné du coté ROUGE pour commencer
error_e TINY_blow_candles_from_red_to_blue(void)
{
	return NOT_HANDLED;
}	

//Souffle toutes les bougies de notre couleur, sans s'arrêter
//  précondition : il faut être positionné du coté BLEU pour commencer
error_e TINY_blow_candles_from_blue_to_red(void)
{
	return NOT_HANDLED;
}

/*
	Le gateau est divisé en 4 parts...
		QUATER_BLUE, MIDLE_BLUE, MIDLE_RED, QUATER_RED
		
|		coté BLUE
|----
|    \		QUATER_BLUE
|     \		--------------
|      |	MIDLE_BLUE
|      |    -----------
|      |	MIDLE_RED
|     /		---------
|    /		QUATER_RED
|----
|		coté RED
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
2- souffler la part du gateau demandée
3- mise à jour de l'environnement à chaque bougie soufflée, et lorsqu'un quart est terminé
*/
error_e TINY_blow_quater(cake_part_e cake_part)
{
	return NOT_HANDLED;
}
	



