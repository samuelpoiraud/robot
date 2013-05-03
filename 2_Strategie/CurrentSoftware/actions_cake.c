/*
 *	Club Robot ESEO 2012 - 2013
 *	Krusty & Tiny
 *
 *	Fichier : actions_cake.c
 *	Package : Carte Strategie
 *	Description : Contient des sub_actions ou micro strat concernant la gestion du gateau pour TINY.
 *	Auteur : nirgal
 *	Version 201305
 */

#include "actions_cake.h"
#include "actions_utils.h"
#include "act_functions.h"

//STRAT BOUGIES


typedef struct{
	Sint16 x;
	Sint16 y;
	Sint16 teta;
}candle_t;

const candle_t candles[12]=
{{1916,865,-536},  //bougie coté rouge
{1768,909,-1608},
{1633,992,-2380},
{1519,1110,-3052},
{1380,1255,-PI4096/2},
{1380,1416,-PI4096/2},
{1380,1584,-PI4096/2},
{1380,1745,-PI4096/2},
{1519,1890,-9726},
{1633,2008,-10184},
{1768,2091,-11256},
{1916,2135,-12328}};  //bougie coté bleu.


/*{{1916,865,-536},  //bougie coté rouge
{1768,904,-1608},
{1633,976,-2380},
{1519,1078,-3052},
{1380,1330,-PI4096/2},
{1380,1445,-PI4096/2},
{1380,1555,-PI4096/2},
{1380,1670,-PI4096/2},
{1519,1922,-9726},
{1633,2024,-10184},
{1768,2096,-11256},
{1916,2135,-12328}};  //bougie coté bleu.*/



//BOUGIES AVEC WARNER !!!
//WARRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRNEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEER !

typedef struct{
	Sint16 x;
	Sint16 y;
}candle_warner;

const candle_warner wcandles[30]=
{							//bougie coté rouge
			{1916,865},
			{1854,877},
			{1793,894},
			{1734,918},
			{1678,947},
			{1624,982},
			{1575,1022},
			{1529,1066},
			{1489,1115},
			{1453,1168},
			{1423,1223},
			{1398,1282},
			{1380,1343},
			{1367,1405},
			{1361,1468},
			{1361,1532},
			{1367,1595},
			{1380,1657},
			{1398,1718},
			{1423,1777},
			{1453,1832},
			{1489,1885},
			{1529,1934},
			{1575,1978},
			{1624,2018},
			{1678,2053},
			{1734,2082},
			{1793,2106},
			{1854,2123},
			{1916,2135}
};  //bougie coté bleu.


error_e TINY_warner_around_cake(Sint8 way)
{
	if(way == 1){
		return goto_pos_with_scan_foe((displacement_t[]){

			//{{wcandles[0].x,wcandles[0].y},SLOW},
			{{wcandles[1].x,wcandles[1].y},SLOW},
			{{wcandles[2].x,wcandles[2].y},SLOW},
			{{wcandles[3].x,wcandles[3].y},SLOW},
			{{wcandles[4].x,wcandles[4].y},SLOW},
			{{wcandles[5].x,wcandles[5].y},SLOW},
			{{wcandles[6].x,wcandles[6].y},SLOW},
			{{wcandles[7].x,wcandles[7].y},SLOW},
			{{wcandles[8].x,wcandles[8].y},SLOW},
			{{wcandles[9].x,wcandles[9].y},SLOW},
			{{wcandles[10].x,wcandles[10].y},SLOW},
			{{wcandles[11].x,wcandles[11].y},SLOW},
			{{wcandles[12].x,wcandles[12].y},SLOW},
			{{wcandles[13].x,wcandles[13].y},SLOW},
			{{wcandles[14].x,wcandles[14].y},SLOW},
			{{wcandles[15].x,wcandles[15].y},SLOW},
			{{wcandles[16].x,wcandles[16].y},SLOW},
			{{wcandles[17].x,wcandles[17].y},SLOW},
			{{wcandles[18].x,wcandles[18].y},SLOW},
			{{wcandles[19].x,wcandles[19].y},SLOW},
			{{wcandles[20].x,wcandles[20].y},SLOW},
			{{wcandles[21].x,wcandles[21].y},SLOW},
			{{wcandles[22].x,wcandles[22].y},SLOW},
			{{wcandles[23].x,wcandles[23].y},SLOW},
			{{wcandles[24].x,wcandles[24].y},SLOW},
			{{wcandles[25].x,wcandles[25].y},SLOW},
			{{wcandles[26].x,wcandles[26].y},SLOW},
			{{wcandles[27].x,wcandles[27].y},SLOW},
			{{wcandles[28].x,wcandles[28].y},SLOW},
			{{wcandles[29].x,wcandles[29].y},SLOW}
			},29,BACKWARD,NO_DODGE_AND_WAIT);
	}else{
		return goto_pos_with_scan_foe((displacement_t[]){

			//{{wcandles[29].x,wcandles[29].y},SLOW},
			{{wcandles[28].x,wcandles[28].y},SLOW},
			{{wcandles[27].x,wcandles[27].y},SLOW},
			{{wcandles[26].x,wcandles[26].y},SLOW},
			{{wcandles[25].x,wcandles[25].y},SLOW},
			{{wcandles[24].x,wcandles[24].y},SLOW},
			{{wcandles[23].x,wcandles[23].y},SLOW},
			{{wcandles[22].x,wcandles[22].y},SLOW},
			{{wcandles[21].x,wcandles[21].y},SLOW},
			{{wcandles[20].x,wcandles[20].y},SLOW},
			{{wcandles[19].x,wcandles[19].y},SLOW},
			{{wcandles[18].x,wcandles[18].y},SLOW},
			{{wcandles[17].x,wcandles[17].y},SLOW},
			{{wcandles[16].x,wcandles[16].y},SLOW},
			{{wcandles[15].x,wcandles[15].y},SLOW},
			{{wcandles[14].x,wcandles[14].y},SLOW},
			{{wcandles[13].x,wcandles[13].y},SLOW},
			{{wcandles[12].x,wcandles[12].y},SLOW},
			{{wcandles[11].x,wcandles[11].y},SLOW},
			{{wcandles[10].x,wcandles[10].y},SLOW},
			{{wcandles[9].x,wcandles[9].y},SLOW},
			{{wcandles[8].x,wcandles[8].y},SLOW},
			{{wcandles[7].x,wcandles[7].y},SLOW},
			{{wcandles[6].x,wcandles[6].y},SLOW},
			{{wcandles[5].x,wcandles[5].y},SLOW},
			{{wcandles[4].x,wcandles[4].y},SLOW},
			{{wcandles[3].x,wcandles[3].y},SLOW},
			{{wcandles[2].x,wcandles[2].y},SLOW},
			{{wcandles[1].x,wcandles[1].y},SLOW},
			{{wcandles[0].x,wcandles[0].y},SLOW}
			},29,FORWARD,NO_DODGE_AND_WAIT);

	}

}


error_e TINY_warner_blow_all_candles(void)
{
	typedef enum
	{
		INIT=0,
		ANGLE_HAMMER,
		HAMMER_UP,
	    WAIT_HAMMER,
		ANGLE_HAMMER_READY,
		HAMMER_CANDLE,
		SUB_BLOW_CANDLES,
		WAIT_HAMMER_DOWN_CANDLE,
		ALL_CANDLES_BLOWN,
		RETURN_HOME,
		HAMMER_FINAL_POS,
		LAST_WAIT_HAMMER_DOWN,
		FAIL,
		FAIL_GO_ANGLE,
		FAIL_HAMMER_DOWN,
		DONE
	}state_e;
	static state_e state = INIT;
	static Sint8 way;
	static color_e color_begin_cake;

	error_e ret = IN_PROGRESS;
	error_e sub_action;

	switch(state)
	{
		case INIT:
			//EN arrivant dans cette fonction, on est d'un des cotés du gateau... on observe ici lequel... et on fait le gateau dans le sens qui va bien.

			if(global.env.pos.y > 1500)	//Nous sommes a coté du gateau, près du coté obscure (bleu) (quelle que soit notre couleur)
			{
				color_begin_cake = BLUE;
				way = -1;	//On décrémente les bougies
			}
			else						//Nous sommes a coté du gateau, près du coté des gentils (rouge) (quelle que soit notre couleur)
			{
				color_begin_cake = RED;
				way = 1;	//On incrémente les bougies
			}
			TINY_warner_blow_one_candle(TRUE);
			state = ANGLE_HAMMER;
			break;

		case ANGLE_HAMMER:
			state=try_go_angle((color_begin_cake==BLUE)?-9726:-3752, ANGLE_HAMMER, HAMMER_UP, HAMMER_UP, FAST);
		break;

		case HAMMER_UP:
				ACT_hammer_goto(HAMMER_POSITION_UP); 	//LEVER le bras
				state=WAIT_HAMMER;
		break;

		case WAIT_HAMMER:
			//						->In progress			->Success						->Fail
			state = wait_hammer(	WAIT_HAMMER,	ANGLE_HAMMER_READY,	ANGLE_HAMMER_READY);
		break;

		case ANGLE_HAMMER_READY:
			state=try_go_angle((color_begin_cake==BLUE)?-12328:-536,ANGLE_HAMMER_READY,HAMMER_CANDLE,HAMMER_CANDLE, FAST);
		break;

		case HAMMER_CANDLE:
				ACT_hammer_blow_candle(); 	//BAISSER BRAS
				state=WAIT_HAMMER_DOWN_CANDLE;
		break;
		case WAIT_HAMMER_DOWN_CANDLE:
			//						->In progress			->Success						->Fail
			state = wait_hammer(	WAIT_HAMMER_DOWN_CANDLE,SUB_BLOW_CANDLES, FAIL);
			//TODO : prévenir l'environnement à chaque bougie soufflée !
		break;

		case SUB_BLOW_CANDLES:
			TINY_warner_blow_one_candle(FALSE);
			sub_action = TINY_warner_around_cake(way);	//On tourne autour du gateau.
			switch(sub_action)
			{
				case IN_PROGRESS:
				break;
				case END_OK:
					state = ALL_CANDLES_BLOWN;
				break;
				case NOT_HANDLED:
				case FOE_IN_PATH:
				case END_WITH_TIMEOUT:
					state = FAIL;
				break;
				default:
				break;
			}
		break;

		 //FINISH

		case ALL_CANDLES_BLOWN:
			state = try_go_angle((color_begin_cake==BLUE)?-3752:-9726, ALL_CANDLES_BLOWN,  HAMMER_FINAL_POS,  HAMMER_FINAL_POS, FAST);
			//TODO : prévenir l'environnement que le gateau est fini !
		break;

		case HAMMER_FINAL_POS:
			ACT_hammer_goto(HAMMER_POSITION_DOWN); 	//BAISSER BRAS
			state=LAST_WAIT_HAMMER_DOWN;
			break;
		case LAST_WAIT_HAMMER_DOWN:
			//						->In progress			->Success	->Fail
			state = wait_hammer(	LAST_WAIT_HAMMER_DOWN,	DONE,		DONE);
		break;
		case FAIL:
			state = FAIL_GO_ANGLE;

		break;
		case FAIL_GO_ANGLE:
			//En cas d'échec, on rejoint un angle pour baisser le bras.
			state = try_go_angle((global.env.pos.y > 1500)?0:PI4096, FAIL_GO_ANGLE,  FAIL_HAMMER_DOWN,  FAIL_HAMMER_DOWN, FAST);
		break;
		case FAIL_HAMMER_DOWN:
			ACT_hammer_goto(HAMMER_POSITION_HOME); 	//RANGER BRAS
			state = INIT;
			ret = NOT_HANDLED;
		break;
		case DONE:
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





error_e TINY_warner_blow_one_candle(bool_e reset)
{
	typedef enum
	{
		INIT,
		WAIT_CANDLE,
		BLOW_CANDLE,
		DONE
	}state_e;

	static state_e state = INIT;
	static Uint8 candle_index;
	static Uint8 last_candle;
	static Uint8 way;
	static color_e color_begin_cake;
	error_e ret = IN_PROGRESS;

	if(reset)
	{
		state=INIT;
		return END_OK;
	}


	switch(state)
	{
		case INIT:
			if(global.env.pos.y > 1500)	//Nous sommes a coté du gateau, près du coté obscure (bleu) (quelle que soit notre couleur)
			{
				color_begin_cake = BLUE;
				candle_index=10;	//On commence par la bougie 10
				last_candle = 0;
				way = -1;	//On décrémente les bougies
			}
			else						//Nous sommes a coté du gateau, près du coté des gentils (rouge) (quelle que soit notre couleur)
			{
				color_begin_cake = RED;
				candle_index=1;	//On commence par la bougie 1
				last_candle = 11;
				way = 1;	//On incrémente les bougies
			}
			state=WAIT_CANDLE;
		break;

		case WAIT_CANDLE:
			ASSER_WARNER_arm_y(candles[candle_index].y);
			state = BLOW_CANDLE;
		break;

		case BLOW_CANDLE:
			if(global.env.asser.reach_y)
			{
				ACT_hammer_blow_candle(); 	//souffler pfuuuuuu pfuuuuuuu
				if(candle_index == last_candle){
						state = DONE;	//On vient de faire la dernière bougie
				}else{
						//global.env.map_elements[GOAL_Etage1Bougie0] = ELEMENT_DONE;
						candle_index += way;
						state=WAIT_CANDLE;
				}
			}
		break;

		case DONE:
			state = INIT;
			ret = END_OK;
		break;

        default:
			state = INIT;
		break;

	}
	if(ret != IN_PROGRESS)
		state = INIT;
	return ret;
}



error_e TINY_blow_all_candles(void)
{
	typedef enum
	{
		INIT=0,
		ANGLE_HAMMER,
		HAMMER_UP,
	    WAIT_HAMMER,
		SUB_BLOW_CANDLES,
		ALL_CANDLES_BLOWN,
		RETURN_HOME,
		HAMMER_FINAL_POS,
		LAST_WAIT_HAMMER_DOWN,
		FAIL,
		DONE
	}state_e;
	static state_e state = INIT;
	static Sint8 way;
	static color_e color_begin_cake;
	static Uint8 candle_index;
	static Uint8 last_candle;

	error_e ret = IN_PROGRESS;
	error_e sub_action;

	switch(state)
	{
		case INIT:
			//EN arrivant dans cette fonction, on est d'un des cotés du gateau... on observe ici lequel... et on fait le gateau dans le sens qui va bien.

			if(global.env.pos.y > 1500)	//Nous sommes a coté du gateau, près du coté obscure (bleu) (quelle que soit notre couleur)
			{
				color_begin_cake = BLUE;
				candle_index=11;	//On commence par la bougie 11
				last_candle = 0;
				way = -1;	//On décrémente les bougies
			}
			else						//Nous sommes a coté du gateau, près du coté des gentils (rouge) (quelle que soit notre couleur)
			{
				color_begin_cake = RED;
				candle_index=0;	//On commence par la bougie 0
				last_candle = 11;
				way = 1;	//On incrémente les bougies
			}
			state = ANGLE_HAMMER;
			break;

		case ANGLE_HAMMER:
			state=try_go_angle((color_begin_cake==BLUE)?-9726:-3752, ANGLE_HAMMER, HAMMER_UP, HAMMER_UP, FAST);
		break;

		case HAMMER_UP:
				ACT_hammer_goto(HAMMER_POSITION_UP); 	//LEVER le bras
				state=WAIT_HAMMER;
		break;

		case WAIT_HAMMER:
			//						->In progress			->Success						->Fail
			state = wait_hammer(	WAIT_HAMMER,	SUB_BLOW_CANDLES,	SUB_BLOW_CANDLES);
		break;

		case SUB_BLOW_CANDLES:
			sub_action = TINY_blow_one_candle(candle_index,way);	//On souffle une bougie
			switch(sub_action)
			{
				case IN_PROGRESS:
				break;
				case END_OK:
					if(candle_index == last_candle)
						state = ALL_CANDLES_BLOWN;	//On vient de faire la dernière bougie
					else
						candle_index += way;	//On reste ici pour la prochaine bougie...
				break;
				case NOT_HANDLED:
				case FOE_IN_PATH:
				case END_WITH_TIMEOUT:
					state = FAIL;
				break;
				default:
				break;
			}
		break;

		 //FINISH

		case ALL_CANDLES_BLOWN:
			state = try_go_angle((color_begin_cake==BLUE)?-3752:-9726, ALL_CANDLES_BLOWN,  HAMMER_FINAL_POS,  HAMMER_FINAL_POS, FAST);
			//TODO : prévenir l'environnement que le gateau est fini !
		break;

		case HAMMER_FINAL_POS:
			ACT_hammer_goto(HAMMER_POSITION_DOWN); 	//BAISSER BRAS
			state=LAST_WAIT_HAMMER_DOWN;
			break;
		case LAST_WAIT_HAMMER_DOWN:
			//						->In progress					->Success						->Fail
			state = wait_hammer(	LAST_WAIT_HAMMER_DOWN,	DONE,	DONE);
		break;
		case FAIL:
			state = INIT;
			ret = NOT_HANDLED;
		break;

		case DONE:
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


//Way vaut soit +1 (rouge vers bleu), soit -1 (bleu vers rouge).
error_e TINY_blow_one_candle(Uint8 i, Sint8 way)
{
	typedef enum
	{
		INIT,
		GOTO_CANDLE,
		ANGLE_CANDLE,
		HAMMER_CANDLE,
		WAIT_HAMMER_DOWN_CANDLE,
		HAMMER_FAIL,
		GOTO_FAIL,
		DONE
	}state_e;

	static state_e state = INIT;

	error_e ret = IN_PROGRESS;

	switch(state)
	{
		case INIT:
			if( (way == 1 && i==0) || (way == -1 && i==11) )
				state = ANGLE_CANDLE;	//Première bougie -> angle directement.
			else
				state = GOTO_CANDLE;	//Bougies suivantes -> goto bougie, puis angle.
		break;
		case GOTO_CANDLE:
			state=try_going(candles[i].x,candles[i].y,GOTO_CANDLE, ANGLE_CANDLE, GOTO_FAIL,(way==1)?BACKWARD:FORWARD, NO_DODGE_AND_WAIT);
		break;

	    case ANGLE_CANDLE:
			state=try_go_angle(candles[i].teta, ANGLE_CANDLE, HAMMER_CANDLE, GOTO_FAIL, FAST);
		break;

		case HAMMER_CANDLE:
				ACT_hammer_blow_candle(); 	//BAISSER BRAS
				state=WAIT_HAMMER_DOWN_CANDLE;
		break;
		case WAIT_HAMMER_DOWN_CANDLE:
			//						->In progress			->Success						->Fail
			state = wait_hammer(	WAIT_HAMMER_DOWN_CANDLE, DONE, DONE);
			//TODO : prévenir l'environnement à chaque bougie soufflée !
		break;

		case HAMMER_FAIL:
			state = INIT;
			ret = NOT_HANDLED;
		break;
		case GOTO_FAIL:
			state = INIT;
			ret = NOT_HANDLED;
		break;
		case DONE:
			state = INIT;
			ret = END_OK;
		break;

        default:
			state = INIT;
		break;

	}
	if(ret != IN_PROGRESS)
		state = INIT;
	return ret;
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



//Error_e qui fonctionne sans vérification a chaque fois des angles. codé crade.
#define X_TO_BLOW_WHITE_CANDLES	1380
error_e old_TINY_all_candles(void){
	typedef enum
	{
		INIT=0,
				GOTO_FIRST_ENNEMY_CANDLE,
		ANGLE_HAMMER,
		HAMMER_UP,
	    WAIT_HAMMER,
		ANGLE_FIRST_CANDLE,
		HAMMER_FIRST_CANDLE,
		WAIT_HAMMER_DOWN_FIRST_CANDLE,
                GOTO_SECOND_ENNEMY_CANDLE,
		HAMMER_SECOND_CANDLE,
		WAIT_HAMMER_DOWN_SECOND_CANDLE,
                GOTO_THIRD_ENNEMY_CANDLE,
		HAMMER_THIRD_CANDLE,
		WAIT_HAMMER_DOWN_THIRD_CANDLE,
                GOTO_FOURTH_ENNEMY_CANDLE,
		HAMMER_FOURTH_CANDLE,
		WAIT_HAMMER_DOWN_FOURTH_CANDLE,


		WHITE,
				GOTO_FIFTH_CANDLE,
		ANGLE_FIFTH_CANDLE,
		HAMMER_FIFTH_CANDLE,
		WAIT_HAMMER_DOWN_FIFTH_CANDLE,
		WAIT_HAMMER_FIFTH_CANDLE_REUP,
                GOTO_SIX_WHITE_CANDLE,
		HAMMER_SIX_CANDLE,
		WAIT_HAMMER_DOWN_SIX_CANDLE,
		WAIT_HAMMER_SIX_CANDLE_REUP,
                GOTO_SEVENTH_WHITE_CANDLE,
		HAMMER_SEVENTH_CANDLE,
		WAIT_HAMMER_DOWN_SEVENTH_CANDLE,
		WAIT_HAMMER_SEVENTH_CANDLE_REUP,
                GOTO_EIGHT_WHITE_CANDLE,
		HAMMER_EIGHT_CANDLE,
		WAIT_HAMMER_DOWN_EIGHT_CANDLE,
		WAIT_HAMMER_EIGHT_CANDLE_REUP,


		SAFE,
				GOTO_NINE_SAFE_CANDLE,
		HAMMER_NINE_CANDLE,
		WAIT_HAMMER_DOWN_NINE_CANDLE,
                GOTO_TENTH_SAFE_CANDLE,
		HAMMER_TENTH_CANDLE,
		WAIT_HAMMER_DOWN_TENTH_CANDLE,
                GOTO_ELEVENTH_SAFE_CANDLE,
		HAMMER_ELEVENTH_CANDLE,
		WAIT_HAMMER_DOWN_ELEVENTH_CANDLE,
                GOTO_TWELVE_SAFE_CANDLE,
		HAMMER_TWELVE_CANDLE,
		WAIT_HAMMER_DOWN_TWELVE_CANDLE,

		ALL_CANDLES_BLOWN,
		RETURN_HOME,
		HAMMER_FINAL_POS,
		LAST_WAIT_HAMMER_DOWN,
		HAMMER_FAIL,
		TIMEOUT_FAIL,
		PROPULSION_OR_AVOIDANCE_FAIL,
		DONE
	}state_e;
	static state_e state = INIT;
	static state_e previous_state = INIT;

	error_e ret = IN_PROGRESS;
	error_e sub_action;
	static Uint8 nb_try = 0;


	switch(state)
	{
		case INIT:
			state = GOTO_FIRST_ENNEMY_CANDLE;
			break;




		//-------------------------FIRST GODAMN ENNEMY CANDLE--------------------------------------

		case GOTO_FIRST_ENNEMY_CANDLE:
			state=try_going(1916,COLOR_Y(2135),GOTO_FIRST_ENNEMY_CANDLE, ANGLE_HAMMER, ANGLE_HAMMER,(global.env.color==BLUE)?FORWARD:BACKWARD, NO_DODGE_AND_WAIT);
			break;


	    case ANGLE_HAMMER:
			sub_action = goto_angle(-PI4096/2,VERY_SLOW_TRANSLATION_AND_FAST_ROTATION);

			switch(sub_action)
            {
				case END_OK:
					state = HAMMER_UP;
				break;
				case END_WITH_TIMEOUT:	//Echec de la mission
					previous_state = state;
					state = HAMMER_UP;
				break;
				case NOT_HANDLED:		//Echec de la mission
				case FOE_IN_PATH:
					previous_state = state;
					state = HAMMER_UP;
				break;
				case IN_PROGRESS:
				break;
				default:
				break;
            }
		break;

		case HAMMER_UP:
				ACT_hammer_goto(HAMMER_POSITION_UP); 	//BAISSER BRAS
				state=WAIT_HAMMER;
		break;

		case WAIT_HAMMER:
			//						->In progress			->Success						->Fail
			state = wait_hammer(	WAIT_HAMMER,	ANGLE_FIRST_CANDLE,	ANGLE_FIRST_CANDLE); //L'autre robot aka pa faire iech.
		break;

	    case ANGLE_FIRST_CANDLE:
			sub_action = goto_angle(PI4096+1608,SLOW);

			switch(sub_action)
            {
				case END_OK:
					state = HAMMER_FIRST_CANDLE;
				break;
				case END_WITH_TIMEOUT:	//Echec de la mission
					previous_state = state;
					state = HAMMER_FIRST_CANDLE;
				break;
				case NOT_HANDLED:		//Echec de la mission
				case FOE_IN_PATH:
					previous_state = state;
					state = HAMMER_FIRST_CANDLE;
				break;
				case IN_PROGRESS:
				break;
				default:
				break;
            }
		break;

		case HAMMER_FIRST_CANDLE:
				ACT_hammer_blow_candle(); 	//BAISSER BRAS
				state=WAIT_HAMMER_DOWN_FIRST_CANDLE;
		break;
		case WAIT_HAMMER_DOWN_FIRST_CANDLE:
			//						->In progress			->Success						->Fail
			state = wait_hammer(	WAIT_HAMMER_DOWN_FIRST_CANDLE,	GOTO_SECOND_ENNEMY_CANDLE,	GOTO_SECOND_ENNEMY_CANDLE);
		 break;


                //Deuxieme bougie

        case GOTO_SECOND_ENNEMY_CANDLE:
			state=try_going(1768,COLOR_Y(2096), GOTO_SECOND_ENNEMY_CANDLE, HAMMER_SECOND_CANDLE, HAMMER_SECOND_CANDLE,(global.env.color==BLUE)?BACKWARD:FORWARD, NO_DODGE_AND_WAIT);
        break;

		case HAMMER_SECOND_CANDLE:
				ACT_hammer_blow_candle(); 	//BAISSER BRAS
				state=WAIT_HAMMER_DOWN_SECOND_CANDLE;
		break;
		case WAIT_HAMMER_DOWN_SECOND_CANDLE:
			//						->In progress			->Success						->Fail
			state = wait_hammer(	WAIT_HAMMER_DOWN_SECOND_CANDLE,	GOTO_THIRD_ENNEMY_CANDLE,	GOTO_THIRD_ENNEMY_CANDLE);
		break;


                //troisieme bougie

        case GOTO_THIRD_ENNEMY_CANDLE:
			state=try_going(1633,COLOR_Y(2024), GOTO_THIRD_ENNEMY_CANDLE, HAMMER_THIRD_CANDLE, HAMMER_THIRD_CANDLE,(global.env.color==BLUE)?BACKWARD:FORWARD, NO_DODGE_AND_WAIT);
        break;

		case HAMMER_THIRD_CANDLE:
				ACT_hammer_blow_candle(); 	//BAISSER BRAS
				state=WAIT_HAMMER_DOWN_THIRD_CANDLE;
		break;
		case WAIT_HAMMER_DOWN_THIRD_CANDLE:
			//						->In progress			->Success						->Fail
			state = wait_hammer(	WAIT_HAMMER_DOWN_THIRD_CANDLE,	GOTO_FOURTH_ENNEMY_CANDLE,	GOTO_FOURTH_ENNEMY_CANDLE);
		break;

                //quatrieme bougie

        case GOTO_FOURTH_ENNEMY_CANDLE:
			state=try_going(1519,COLOR_Y(1922), GOTO_FOURTH_ENNEMY_CANDLE, HAMMER_FOURTH_CANDLE, HAMMER_FOURTH_CANDLE,(global.env.color==BLUE)?BACKWARD:FORWARD, NO_DODGE_AND_WAIT);
        break;

		case HAMMER_FOURTH_CANDLE:

				ACT_hammer_blow_candle(); 	//BAISSER BRAS
				state=WAIT_HAMMER_DOWN_FOURTH_CANDLE;
		break;

		case WAIT_HAMMER_DOWN_FOURTH_CANDLE:
			//						->In progress			->Success						->Fail
			state = wait_hammer(	WAIT_HAMMER_DOWN_FOURTH_CANDLE,	WHITE,	WHITE);
		break;


		//WHITE

		case WHITE:
			state = GOTO_FIFTH_CANDLE;
			break;




		//-------------------------FIRST GODAMN WHITY CANDLE--------------------------------------

		case GOTO_FIFTH_CANDLE:
			state=try_going(X_TO_BLOW_WHITE_CANDLES,COLOR_Y(1670), GOTO_FIFTH_CANDLE, ANGLE_FIFTH_CANDLE, ANGLE_FIFTH_CANDLE,(global.env.color==BLUE)?BACKWARD:FORWARD, NO_DODGE_AND_WAIT);
				if(state==WAIT_HAMMER){
					ACT_hammer_goto(HAMMER_POSITION_UP);
				}
			break;


	    case ANGLE_FIFTH_CANDLE:
			sub_action = goto_angle(-PI4096/2,VERY_SLOW_TRANSLATION_AND_FAST_ROTATION);

			switch(sub_action)
            {
				case END_OK:
					state = HAMMER_FIFTH_CANDLE;
				break;
				case END_WITH_TIMEOUT:	//Echec de la mission
					previous_state = state;
					state = HAMMER_FIFTH_CANDLE;
				break;
				case NOT_HANDLED:		//Echec de la mission
				case FOE_IN_PATH:
					previous_state = state;
					state = HAMMER_FIFTH_CANDLE;
				break;
				case IN_PROGRESS:
				break;
				default:
				break;
            }
		break;

		case HAMMER_FIFTH_CANDLE:
				ACT_hammer_goto(HAMMER_POSITION_CANDLE); 	//BAISSER BRAS
				state=WAIT_HAMMER_DOWN_FIFTH_CANDLE;
		break;
		case WAIT_HAMMER_DOWN_FIFTH_CANDLE:
			//						->In progress			->Success						->Fail
			state = wait_hammer(	WAIT_HAMMER_DOWN_FIFTH_CANDLE,	WAIT_HAMMER_FIFTH_CANDLE_REUP,	WAIT_HAMMER_FIFTH_CANDLE_REUP);
			if(state == WAIT_HAMMER_FIFTH_CANDLE_REUP)
				ACT_hammer_goto(HAMMER_POSITION_UP); 	//RELEVER BRAS
		break;
		case WAIT_HAMMER_FIFTH_CANDLE_REUP:
			//						->In progress					->Success						->Fail
			state = wait_hammer(	WAIT_HAMMER_FIFTH_CANDLE_REUP,	GOTO_SIX_WHITE_CANDLE,	GOTO_SIX_WHITE_CANDLE);
		break;



                //Deuxieme bougie

        case GOTO_SIX_WHITE_CANDLE:
			state=try_going(X_TO_BLOW_WHITE_CANDLES,COLOR_Y(1555), GOTO_SIX_WHITE_CANDLE, HAMMER_SIX_CANDLE, HAMMER_SIX_CANDLE,(global.env.color==BLUE)?BACKWARD:FORWARD, NO_DODGE_AND_WAIT);
                break;

		case HAMMER_SIX_CANDLE:
				ACT_hammer_goto(HAMMER_POSITION_CANDLE); 	//BAISSER BRAS
				state=WAIT_HAMMER_DOWN_SIX_CANDLE;
		break;
		case WAIT_HAMMER_DOWN_SIX_CANDLE:
			//						->In progress			->Success						->Fail
			state = wait_hammer(	WAIT_HAMMER_DOWN_SIX_CANDLE,	WAIT_HAMMER_SIX_CANDLE_REUP,	WAIT_HAMMER_SIX_CANDLE_REUP);
			if(state == WAIT_HAMMER_SIX_CANDLE_REUP)
				ACT_hammer_goto(HAMMER_POSITION_UP); 	//RELEVER BRAS
		break;
		case WAIT_HAMMER_SIX_CANDLE_REUP:
			//						->In progress					->Success						->Fail
			state = wait_hammer(	WAIT_HAMMER_SIX_CANDLE_REUP,	GOTO_SEVENTH_WHITE_CANDLE,	GOTO_SEVENTH_WHITE_CANDLE);
		break;

                //troisieme bougie

        case GOTO_SEVENTH_WHITE_CANDLE:
			state=try_going(X_TO_BLOW_WHITE_CANDLES,COLOR_Y(1445), GOTO_SEVENTH_WHITE_CANDLE, HAMMER_SEVENTH_CANDLE, HAMMER_SEVENTH_CANDLE,(global.env.color==BLUE)?BACKWARD:FORWARD, NO_DODGE_AND_WAIT);
                break;

		case HAMMER_SEVENTH_CANDLE:
				ACT_hammer_goto(HAMMER_POSITION_CANDLE); 	//BAISSER BRAS
				state=WAIT_HAMMER_DOWN_SEVENTH_CANDLE;
		break;
		case WAIT_HAMMER_DOWN_SEVENTH_CANDLE:
			//						->In progress			->Success						->Fail
			state = wait_hammer(	WAIT_HAMMER_DOWN_SEVENTH_CANDLE,	WAIT_HAMMER_SEVENTH_CANDLE_REUP,	WAIT_HAMMER_SEVENTH_CANDLE_REUP);
			if(state == WAIT_HAMMER_SEVENTH_CANDLE_REUP)
				ACT_hammer_goto(HAMMER_POSITION_UP); 	//RELEVER BRAS
		break;
		case WAIT_HAMMER_SEVENTH_CANDLE_REUP:
			//						->In progress					->Success						->Fail
			state = wait_hammer(	WAIT_HAMMER_SEVENTH_CANDLE_REUP,	GOTO_EIGHT_WHITE_CANDLE,	GOTO_EIGHT_WHITE_CANDLE);
		break;


                //quatrieme bougie

        case GOTO_EIGHT_WHITE_CANDLE:
			state=try_going(X_TO_BLOW_WHITE_CANDLES,COLOR_Y(1330), GOTO_EIGHT_WHITE_CANDLE, HAMMER_EIGHT_CANDLE, HAMMER_EIGHT_CANDLE,(global.env.color==BLUE)?BACKWARD:FORWARD, NO_DODGE_AND_WAIT);
                break;

		case HAMMER_EIGHT_CANDLE:

				ACT_hammer_goto(HAMMER_POSITION_CANDLE); 	//BAISSER BRAS
				state=WAIT_HAMMER_DOWN_EIGHT_CANDLE;
		break;

		case WAIT_HAMMER_DOWN_EIGHT_CANDLE:
			//						->In progress			->Success						->Fail
			state = wait_hammer(	WAIT_HAMMER_DOWN_EIGHT_CANDLE,	WAIT_HAMMER_EIGHT_CANDLE_REUP,	WAIT_HAMMER_EIGHT_CANDLE_REUP);
			if(state == WAIT_HAMMER_EIGHT_CANDLE_REUP)
				ACT_hammer_goto(HAMMER_POSITION_UP); 	//RELEVER BRAS
		break;

		case WAIT_HAMMER_EIGHT_CANDLE_REUP:
			//						->In progress					->Success						->Fail
			state = wait_hammer(	WAIT_HAMMER_EIGHT_CANDLE_REUP,SAFE,SAFE);
		break;






		//SAFE

		case SAFE:
			state = GOTO_NINE_SAFE_CANDLE;
			break;

		//-------------------------FIRST GODAMN SAFE CANDLE--------------------------------------

		case GOTO_NINE_SAFE_CANDLE:
			state=try_going(1519,COLOR_Y(1078), GOTO_NINE_SAFE_CANDLE, HAMMER_NINE_CANDLE, HAMMER_NINE_CANDLE,(global.env.color==BLUE)?BACKWARD:FORWARD, NO_DODGE_AND_WAIT);
			break;


	    /*case ANGLE_HAMMER:
			sub_action = goto_angle(-PI4096/2,VERY_SLOW);

			switch(sub_action)
            {
				case END_OK:
					state = HAMMER_UP;
				break;
				case END_WITH_TIMEOUT:	//Echec de la mission
					previous_state = state;
					state = HAMMER_UP;
				break;
				case NOT_HANDLED:		//Echec de la mission
				case FOE_IN_PATH:
					previous_state = state;
					state = HAMMER_UP;
				break;
				case IN_PROGRESS:
				break;
				default:
				break;
            }
		break;

		case HAMMER_UP:
				ACT_hammer_goto(HAMMER_POSITION_UP); 	//BAISSER BRAS
				state=WAIT_HAMMER;
		break;

		case WAIT_HAMMER:
			//						->In progress			->Success						->Fail
			state = wait_hammer(	WAIT_HAMMER,	ANGLE_FIRST_CANDLE,	ANGLE_FIRST_CANDLE); //L'autre robot aka pa faire iech.
		break;

	    case ANGLE_FIRST_CANDLE:
			sub_action = goto_angle(-PI4096/8,SLOW);

			switch(sub_action)
            {
				case END_OK:
					state = HAMMER_FIRST_CANDLE;
				break;
				case END_WITH_TIMEOUT:	//Echec de la mission
					previous_state = state;
					state = HAMMER_FIRST_CANDLE;
				break;
				case NOT_HANDLED:		//Echec de la mission
				case FOE_IN_PATH:
					previous_state = state;
					state = HAMMER_FIRST_CANDLE;
				break;
				case IN_PROGRESS:
				break;
				default:
				break;
            }
		break;*/

		case HAMMER_NINE_CANDLE:
				ACT_hammer_blow_candle(); 	//BAISSER BRAS
				state=WAIT_HAMMER_DOWN_NINE_CANDLE;
		break;


		case WAIT_HAMMER_DOWN_NINE_CANDLE:
			//						->In progress			->Success						->Fail
			state = wait_hammer(	WAIT_HAMMER_DOWN_NINE_CANDLE,	GOTO_TENTH_SAFE_CANDLE,	GOTO_TENTH_SAFE_CANDLE);
		break;

                //Deuxieme bougie
        case GOTO_TENTH_SAFE_CANDLE:
			state=try_going(1633,COLOR_Y(976), GOTO_TENTH_SAFE_CANDLE, HAMMER_TENTH_CANDLE, HAMMER_TENTH_CANDLE,(global.env.color==BLUE)?BACKWARD:FORWARD, NO_DODGE_AND_WAIT);
        break;

		case HAMMER_TENTH_CANDLE:
				ACT_hammer_blow_candle(); 	//BAISSER BRAS
				state=WAIT_HAMMER_DOWN_TENTH_CANDLE;
		break;

		case WAIT_HAMMER_DOWN_TENTH_CANDLE:
			//						->In progress			->Success						->Fail
			state = wait_hammer(	WAIT_HAMMER_DOWN_TENTH_CANDLE,	GOTO_ELEVENTH_SAFE_CANDLE,	GOTO_ELEVENTH_SAFE_CANDLE);
		break;

                //troisieme bougie

        case GOTO_ELEVENTH_SAFE_CANDLE:
			state=try_going(1768,COLOR_Y(904), GOTO_ELEVENTH_SAFE_CANDLE, HAMMER_ELEVENTH_CANDLE, HAMMER_ELEVENTH_CANDLE,(global.env.color==BLUE)?BACKWARD:FORWARD, NO_DODGE_AND_WAIT);
                break;

		case HAMMER_ELEVENTH_CANDLE:
				ACT_hammer_blow_candle(); 	//BAISSER BRAS
				state=WAIT_HAMMER_DOWN_ELEVENTH_CANDLE;
		break;
		case WAIT_HAMMER_DOWN_ELEVENTH_CANDLE:
			//						->In progress			->Success						->Fail
			state = wait_hammer(	WAIT_HAMMER_DOWN_ELEVENTH_CANDLE,	GOTO_TWELVE_SAFE_CANDLE,	GOTO_TWELVE_SAFE_CANDLE);
		break;


                //quatrieme bougie

        case GOTO_TWELVE_SAFE_CANDLE:
			state=try_going(1916,COLOR_Y(865), GOTO_TWELVE_SAFE_CANDLE, HAMMER_TWELVE_CANDLE, HAMMER_TWELVE_CANDLE,(global.env.color==BLUE)?BACKWARD:FORWARD, NO_DODGE_AND_WAIT);
                break;

		case HAMMER_TWELVE_CANDLE:

				ACT_hammer_blow_candle(); 	//BAISSER BRAS
				state=WAIT_HAMMER_DOWN_TWELVE_CANDLE;
		break;

		case WAIT_HAMMER_DOWN_TWELVE_CANDLE:
			//						->In progress			->Success						->Fail
			state = wait_hammer(	WAIT_HAMMER_DOWN_TWELVE_CANDLE,	ALL_CANDLES_BLOWN,	ALL_CANDLES_BLOWN);
		break;


		//FINISH

		case ALL_CANDLES_BLOWN:
			state = DONE;
		break;

		case HAMMER_FINAL_POS:
			ACT_hammer_goto(HAMMER_POSITION_DOWN); 	//BAISSER BRAS
			state=LAST_WAIT_HAMMER_DOWN;
			break;
		case LAST_WAIT_HAMMER_DOWN:
			//						->In progress					->Success						->Fail
			state = wait_hammer(	LAST_WAIT_HAMMER_DOWN,	DONE,	DONE);
		break;

		case HAMMER_FAIL:
			ret = NOT_HANDLED;
		break;
		case TIMEOUT_FAIL:					//@pre IL FAUT AVOIR RENSEIGNE LE previous_state
			nb_try++;
			if(nb_try < 5)
				state = previous_state;	//Timeout ?? -> On y retourne !
			else
			{
				ret = END_WITH_TIMEOUT;
				ACT_hammer_goto(HAMMER_POSITION_DOWN); 	//BAISSER BRAS
			}
		break;
		case PROPULSION_OR_AVOIDANCE_FAIL:	//@pre IL FAUT AVOIR RENSEIGNE LE previous_state
			nb_try++;
			if(nb_try < 5)
				state = previous_state;	//Failed ?? -> On y retourne !
			else
			{
				ret = NOT_HANDLED;
				ACT_hammer_goto(HAMMER_POSITION_DOWN); 	//BAISSER BRAS
			}
		break;
		case DONE:
			ret = END_OK;
		break;

            default:
		break;

	}
	if(ret != IN_PROGRESS)
		state = INIT;
	return ret;
}

