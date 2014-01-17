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
#include "actions_utils.h"
#include "../QS/QS_adc.h"

#include "config_pin.h"

#define DEFAULT_SPEED	(SLOW)

#define USE_CURVE	0


//CONFIG DU MODE DE STEAL

	#define STEAL_MODE_WITH_SCAN
	//#define STEAL_CUSTOM

		#define BUFFET_GIFT	210
		#define CASE_0_X	250
		#define CASE_1_X	600
		#define CASE_2_X	1000
		#define CASE_3_X	1400
		#define CASE_4_X	1750
		#define BUFFET_CAKE	1790

		#define ALL_CASES_Y	(180)


/*  --------------POSITION DES VERRES ENNEMIS------------------------*/
#define r3ea_x 1000
#define r3ea_y 370 
// 1000 370 + 600 200

#define projet_and_tech_x  990
#define projet_and_tech_y  370

#define pm_robotic_x	1000 
#define pm_robotic_y	230

#define	space_crakers_x 250		// 250 200 + 1350 190
#define	space_crakers_y  200

#define telecom_robotics_x 600 
#define telecom_robotics_y 200

#define crap_x  790 
#define crap_y 370

#define mine_de_douai_x  600
#define mine_de_douai_y 190

#define unive_angers_x 1200
#define unive_angers_y  270

#define rcva_x  1000
#define rcva_y 150

#define colors_team_x  1000
#define colors_team_y  320

#define uart_x  1000
#define uart_y 150

#define ensim_elec_x 1000
#define ensim_elec_y  130

#define oleg_x 700
#define oleg_y  170

#define supeaero_x 1200
#define supeaero_y 140

#define insa_toulouse
#define insa_toulouse

#define apbteam
#define apbteam

#define sudriabotik
#define sudriabotik

#define oufteam
#define oufteam

#define cybernetique_en_nord
#define cybernetique_en_nord

#define robotic_system
#define robotic_system

#define comet
#define comet

#define bh_team
#define bh_team

#define linuxteam
#define linuxteam

#define rir_robotique
#define rir_robotique

/*-----------------------------------------------------------*/
//REGLAGE DES POSITIONS DE LA TOUR A VOLER

#define GIRAFE_X	CASE_1_X	//En mode custom, cette position est choisie
#define GIRAFE_Y	COLOR_Y(3000-ALL_CASES_Y)	//De la forme : COLOR_Y(3000-le_define)

/* ----------------------------------------------------------------------------- */
/* 						Actions élémentaires de construction                     */
/* ----------------------------------------------------------------------------- */

//Cette fonction est appelée sur TINY à la fin du match.
void T_BALLINFLATER_start(void)
{
	ACT_ball_inflater_inflate(10);
}	

Uint16 wait_hammers(Uint16 progress, Uint16 success, Uint16 fail)
{
	Uint16 ret = progress;
	switch(ACT_get_last_action_result(ACT_QUEUE_Plier))
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

/* ----------------------------------------------------------------------------- */
/* 								Fonction diverses                     			 */
/* ----------------------------------------------------------------------------- */

error_e steal_glasses(girafe_t * g)
{
		//Le type avec les états est défini séparément de la variable pour que mplab x comprenne ce qu'on veut faire ...
	typedef enum
	{
		INIT,
		TA,
		ANGLE_GLASSES,
		OPEN_HAMMERS,
		TB,
		BACK_10CM,
		CLOSE_HAMMERS,
		TC,
		TD,
		FAIL,
		SUCCESS
	}state_e;
	static state_e state = INIT;
	static state_e previous_state = INIT;
	static bool_e entrance = TRUE;
	typedef struct
	{
		Sint16 x;
		Sint16 y;
	}point_t;
	static point_t ta;
	static point_t tb;
	static point_t tc;
//	static point_t td;

	error_e ret = IN_PROGRESS;


	switch(state)
	{
		case INIT:
			//calcul des points selon la position de la girafe...
			ta.x = (g->x_end+g->x_begin)/2;
			ta.y = g->y_middle + ((global.env.color == RED)?(-300):300);

			if(ta.x < 210)	ta.x = 210;	//Buffet cadeaux
			if(ta.x > 1790)	ta.x = 1790;
			tb.x = ta.x;
			tb.y = g->y_middle;
			if(tb.y < 180) tb.y = 180;
			if(tb.y > 2820) tb.y = 2820;
			
			tc.x = ta.x;
			tc.y = ta.y;
					
			//td.y = ta.y;

			//B est du coté où on a le plus de place pour reculer.
			//C est de l'autre coté... un peu plus loin que les verres...
			/*if(ta.x < 1000)
			{
				tb.x = MAX(g->x_end, g->x_begin) + 300;
				tc.x = MIN(g->x_end, g->x_begin) - 30;
				td.x = tc.x - 100;
			}
			else
			{
				tb.x = MIN(g->x_end, g->x_begin) - 300;
				tc.x = MAX(g->x_end, g->x_begin) + 30;
				td.x = tc.x + 100;
			}
			*/

			//ECRETAGE...
	/*		tb.y = g->y_middle;
			if(tb.y < 220)	tb.y = 220;
			if(tb.y > 2780)	tb.y = 2780;
			tc.y = tb.y;

			if(tb.x < 250)	tb.x = 250;
			if(tb.x > 2750)	tb.x = 2750;
			if(tc.x < 250)	tc.x = 250;
			if(tc.x > 2750)	tc.x = 2750;
			if(td.x < 250)	td.x = 250;
			if(td.x > 2750)	td.x = 2750;

			SD_printf("STEAL : ta=(%d,%d) tb=(%d,%d) tc=(%d,%d) td(%d,%d)\n", ta.x, ta.y, tb.x, tb.y, tc.x, tc.y, td.x, td.y);
*/



			state = TA;
		break;
		case TA:
			//On se place à coté de la girafe... 
			state = try_going(ta.x, ta.y,	TA, ANGLE_GLASSES,	FAIL,	FAST, ANY_WAY, NO_DODGE_AND_WAIT);
		break;
		case ANGLE_GLASSES:
			state = try_go_angle(((global.env.color == RED)?PI4096/2:-PI4096/2), ANGLE_GLASSES, OPEN_HAMMERS,OPEN_HAMMERS,FAST);
		break;
		case OPEN_HAMMERS:
			if(entrance)
			{
				//#ifdef STEAL_MODE_CASE_0
				//	ACT_hammer_goto(HAMMER_POSITION_CANDLE);
				//#endif
				ACT_plier_open();
			}
			state = wait_hammers(OPEN_HAMMERS,TB,TB);
		break;
		case TB:
			state = try_going(tb.x, tb.y,	TB, CLOSE_HAMMERS,	BACK_10CM,	SLOW,FORWARD, NO_DODGE_AND_WAIT);
		break;
		case BACK_10CM:	//En cas d'erreur, je recule un peu pour pouvoir refermer mes bras
			state = try_going(tb.x, ((global.env.color == RED)?(tb.y+1000):(tb.y-1000)),	BACK_10CM, FAIL,	FAIL,	SLOW,BACKWARD, NO_DODGE_AND_WAIT);
		break;
		case CLOSE_HAMMERS:
			if(entrance)
				ACT_plier_close();
			state = wait_hammers(CLOSE_HAMMERS,TC,TC);
		break;
		
		case TC:
			//On ramasse la girafe...
			state = try_going_until_break(tc.x, tc.y,	TC, SUCCESS,	FAIL,	SLOW,BACKWARD, NO_DODGE_AND_WAIT);
		break;
		/*case TD:
			//On ressort...
			state = try_going_slow(td.x, td.y,	TD, SUCCESS,	FAIL,	FORWARD, NO_DODGE_AND_WAIT);
		break;*/
		case FAIL:
			ACT_plier_close();
			state = INIT;	//Où que je sois, je m'arrête là... (ca fera une occasion de réfléchir à faire autre chose...)
			ret = NOT_HANDLED;
		break;
		case SUCCESS:
			//Là, c'est vraiment la fête du slip !!!
			state = INIT;
			ret = END_OK;
		break;
		default:
			state = INIT;
		break;
	}

	entrance = (previous_state != state)?TRUE:FALSE;
	if(previous_state != state)
	{
		SD_printf("Steal->");
		switch(state)
		{
			case INIT:						SD_printf("INIT");				break;
			case TA:						SD_printf("TA");					break;
			case ANGLE_GLASSES:				SD_printf("ANGLE_GLASSES");		break;
			case TB:						SD_printf("TB");					break;
			case OPEN_HAMMERS:				SD_printf("OPEN_HAMMERS");		break;
			case CLOSE_HAMMERS:				SD_printf("CLOSE_HAMMERS");		break;
			case BACK_10CM:					SD_printf("BACK_10CM");			break;
			case TC:						SD_printf("TC");					break;
			case TD:						SD_printf("TD");					break;
			case FAIL:						SD_printf("FAIL");				break;
			case SUCCESS:					SD_printf("SUCCESS");			break;
			default:						SD_printf("???");				break;
		}
		SD_printf("\n");
	}
	previous_state = state;
	return ret;
}

/* ----------------------------------------------------------------------------- */
/* 							Découpe de stratégies                     			 */
/* ----------------------------------------------------------------------------- */

bool_e scan_for_glasses_on_bar(void);
static bool_e adversary_is_in_zone = FALSE;
error_e STRAT_TINY_scan_and_steal_adversary_glasses(bool_e reset)
{
		//Le type avec les états est défini séparément de la variable pour que mplab x comprenne ce qu'on veut faire ...
	typedef enum
	{
		INIT,
		BP,
		SA,
		SC,
		GO_ANGLE,
		SCAN_GLASSES_OUTSIDE_ZONE,
		TC,
		TA,
		SCAN_GIFT_BAR,
		SCAN_CAKE_BAR,
		SCAN_GLASSES_INSIDE_ZONE,
		DECISION,
		SUBACTION_STEAL,
		GOTO_MIDDLE,
		COME_BACK_HOME,
		ANGLE_TO_SEE_HOME,
		OPEN_HAMMERS_IN_HOME,
		WAIT_OPEN_HAMMERS_IN_HOME,
		BACK_TO_CLOSE,
		CLOSE_HAMMERS,
		RETURNING_TO_C2,
		FAIL,
		SUCCESS
	}state_e;
	static state_e state = INIT;
	static state_e previous_state = INIT;
	static state_e from = INIT;
	static bool_e entrance = TRUE;
	static bool_e glasses_on_bar;
	error_e sub_action;
	error_e ret = IN_PROGRESS;
	static girafe_t * g;
	static girafe_t manual_girafe;

	if(reset)
	{
		state = INIT;
		return END_OK;
	}
	switch(state)
	{
		case INIT:		//Décision initiale de trajet
			state = SUBACTION_STEAL;	//Default


			#if (		!defined(STEAL_MODE_WITH_SCAN)	&&  !defined(STEAL_CUSTOM)			)
			#warning "VOUS DEVEZ DEFINIR UN MODE DE STEAL. Mode par défaut : STEAL_WITH_SCAN"
			#define STEAL_MODE_WITH_SCAN
			#endif
			#ifdef STEAL_MODE_WITH_SCAN
				from = BP;
				if(global.env.pos.x > 1000)
					state = SC;
				else
					state = SA;
			#endif

				

			#ifdef STEAL_CUSTOM
				manual_girafe.nb_glasses = 1;
				g = &manual_girafe;
				manual_girafe.x_begin = GIRAFE_X;
				manual_girafe.y_middle = GIRAFE_Y;
				manual_girafe.x_end = manual_girafe.x_begin;
			#endif
		break;
		/////////////////////////////////////////
		case SA:
			//										in_progress		success		failed
			state = try_going(170, COLOR_Y(2480),	SA,				GO_ANGLE,	BP,		FAST, ANY_WAY, NO_DODGE_AND_WAIT);
			if(state != SA)
				from = SA;
		break;
		case SC:
			//										in_progress		success		failed
			state = try_going(1830, COLOR_Y(2480),	SC,				GO_ANGLE,	BP,	FAST,	ANY_WAY, NO_DODGE_AND_WAIT);
			if(state != SC)
				from = SC;
		break;
		case BP:
			//									in_progress		success					failed
			state=try_going(1000,COLOR_Y(1800),	BP,				(from == SA)? SC:SA,	(from == SA)?SA:SC,	FAST,	ANY_WAY, NO_DODGE_AND_WAIT);
			if(state != BP)
				from = BP;
		break;

		case GO_ANGLE:
			state = try_go_angle((global.env.color == RED)?PI4096:0, GO_ANGLE, SCAN_GLASSES_OUTSIDE_ZONE, SCAN_GLASSES_OUTSIDE_ZONE, FAST);
		break;

		case SCAN_GLASSES_OUTSIDE_ZONE:
			if(entrance)
				scan_for_glasses(TRUE);
			state = try_going((from == SC)?170:1830, COLOR_Y(2480),	SCAN_GLASSES_OUTSIDE_ZONE, DECISION,	FAIL, SLOW,ANY_WAY, NO_DODGE_AND_WAIT);

			if(global.env.pos.updated)
				scan_for_glasses(FALSE);

			if(state != SCAN_GLASSES_OUTSIDE_ZONE)
				from = (global.env.pos.x>1000)?SC:SA;

		break;

		case TC:
			state = try_going(1680, COLOR_Y(2680),	TC, SCAN_CAKE_BAR,	FAIL, SLOW,ANY_WAY, NO_DODGE_AND_WAIT);
			if(state != TC)
				from = TC;
		break;
		case TA:
			state = try_going(320, COLOR_Y(2680),	TA, SCAN_GIFT_BAR,	FAIL, SLOW,ANY_WAY, NO_DODGE_AND_WAIT);
			if(state != TA)
				from = TA;
		break;
		case SCAN_GIFT_BAR:
			if(entrance)
				glasses_on_bar = FALSE;
			if(global.env.pos.updated)
				glasses_on_bar |= scan_for_glasses_on_bar();
			state = try_go_angle(((global.env.color == RED)?PI4096:0),SCAN_GIFT_BAR,SCAN_GLASSES_INSIDE_ZONE,SCAN_GLASSES_INSIDE_ZONE,SLOW);
			if(state == SCAN_GLASSES_INSIDE_ZONE && glasses_on_bar)
			{
				manual_girafe.nb_glasses = 1;
				manual_girafe.x_begin = 210;
				manual_girafe.x_end = 210;
				manual_girafe.y_middle = (global.env.color == RED)?2820:180;
				g = &manual_girafe;
				state = SUBACTION_STEAL;
			}
		break;
		case SCAN_CAKE_BAR:
			if(entrance)
				glasses_on_bar = FALSE;
			if(global.env.pos.updated)
				glasses_on_bar |= scan_for_glasses_on_bar();
			state = try_go_angle(((global.env.color == RED)?PI4096:0),SCAN_CAKE_BAR,SCAN_GLASSES_INSIDE_ZONE,SCAN_GLASSES_INSIDE_ZONE,SLOW);
			if(state == SCAN_GLASSES_INSIDE_ZONE && glasses_on_bar)
			{
				manual_girafe.nb_glasses = 1;
				manual_girafe.x_begin = 1790;
				manual_girafe.x_end = 1790;
				manual_girafe.y_middle = (global.env.color == RED)?2820:180;
				g = &manual_girafe;
				state = SUBACTION_STEAL;
			}
		break;
		case SCAN_GLASSES_INSIDE_ZONE:
			if(entrance)
			{
				scan_for_glasses(TRUE);
			}

			state = try_going((from == TC)?320:1680, COLOR_Y(2680),	SCAN_GLASSES_INSIDE_ZONE, DECISION,	FAIL, SLOW,ANY_WAY, NO_DODGE_AND_WAIT);

			if(global.env.pos.updated)
				scan_for_glasses(FALSE);
			if(state == FAIL)	//En cas de fail pendant le scan, si on a vu des verres, on tente quand meme de les prendre.
			{
				if(g->nb_glasses > 0)
					state = DECISION;
			}
			if(state != SCAN_GLASSES_INSIDE_ZONE)
				from = (global.env.pos.x>1000)?TC:TA;

		break;

		case DECISION:
			//Si pas de décision positive.. on scanne dans l'autre sens...
			g = look_for_the_best_girafe();
			if(g->nb_glasses > 0)
				state = SUBACTION_STEAL;
			else
			{
				if(from == SC)
					state = TC;
				else if(from == SA)
					state = TA;
				else //(from == TC ou TA)
					state = SUCCESS;

				//if(adversary_is_in_zone)
				//	state = FAIL;
			}
		break;
		case SUBACTION_STEAL:
			sub_action = steal_glasses(g);
			switch(sub_action)
			{
				case IN_PROGRESS:
				break;
				case END_OK:
					if(global.env.pos.x > 1000)
						state = GOTO_MIDDLE;
					else
						state = ANGLE_TO_SEE_HOME;
				break;
				default:
					state = FAIL;
				break;
			}
		break;
		case GOTO_MIDDLE:
			//Point 1 : chez l'adversaire, en X = 250.
			state = try_going_until_break(800,COLOR_Y(1900),	GOTO_MIDDLE, COME_BACK_HOME,	FAIL, SLOW,FORWARD, NO_DODGE_AND_WAIT);
		break;
	//	case ANGLE_TO_SEE_HOME:
	//		state = try_go_angle(((global.env.color == RED)?(-PI4096):PI4096),ANGLE_TO_SEE_HOME,COME_BACK_HOME,COME_BACK_HOME,SLOW);
	//	break;
		case COME_BACK_HOME:
			if(entrance)
				ASSER_WARNER_arm_y(COLOR_Y(500));
			//Point 2 : la dépose dans la zone de départ TINY (début de zone !) ATTENTION A NE PAS DETRUIRE NOS VERRES.
			state = try_going(300,COLOR_Y(400),	COME_BACK_HOME, BACK_TO_CLOSE,	FAIL, SLOW,FORWARD, NO_DODGE_AND_WAIT);
			if(global.env.asser.reach_y)
				ACT_plier_open();
		break;
	//	case OPEN_HAMMERS_IN_HOME:
			
	//		state = WAIT_OPEN_HAMMERS_IN_HOME;
	//	break;
	//	case WAIT_OPEN_HAMMERS_IN_HOME:
	//		state = wait_hammers(WAIT_OPEN_HAMMERS_IN_HOME, BACK_TO_CLOSE, BACK_TO_CLOSE);
	//	break;
		case BACK_TO_CLOSE:
			if(global.env.asser.reach_y)
				ACT_plier_open();
			state = try_going(300,COLOR_Y(650),	BACK_TO_CLOSE, CLOSE_HAMMERS,	FAIL, FAST,BACKWARD, NO_DODGE_AND_WAIT);
		break;
		case CLOSE_HAMMERS:
			ACT_plier_close();
			state = RETURNING_TO_C2;
		break;
		case RETURNING_TO_C2:
			state = try_going(300,COLOR_Y(1100),	RETURNING_TO_C2, SUCCESS,	FAIL, FAST,BACKWARD, NO_DODGE_AND_WAIT);
		break;
		case FAIL:
			ACT_plier_close();
			state = INIT;	//Où que je sois, je m'arrête là... (ca fera une occasion de réfléchir à faire autre chose...)
			ret = NOT_HANDLED;
		break;
		case SUCCESS:
			//Là, c'est vraiment la fête du slip !!!
			state = INIT;
			ret = END_OK;
		break;
		default:
			state = INIT;
		break;
	}
	entrance = (state != previous_state)?TRUE:FALSE;
	if(previous_state != state)
	{
		SD_printf("Scan&Steal->");
		switch(state)
		{
			case INIT:						SD_printf("INIT");				break;
			case BP:						SD_printf("BP");					break;
			case SA:						SD_printf("SA");					break;
			case SC:						SD_printf("SC");					break;
			case GO_ANGLE:					SD_printf("GO_ANGLE");			break;
			case SCAN_GLASSES_OUTSIDE_ZONE:	SD_printf("SCAN_GLASSES");		break;
			case DECISION:					SD_printf("DECISION");			break;
			case SUBACTION_STEAL:			SD_printf("SUBACTION_STEAL");	break;
			case GOTO_MIDDLE:				SD_printf("GOTO_4TH_GIFT");		break;
			case ANGLE_TO_SEE_HOME:			SD_printf("ANGLE_TO_SEE_HOME");	break;
			case COME_BACK_HOME:			SD_printf("COME_BACK_HOME");		break;
			case OPEN_HAMMERS_IN_HOME:		SD_printf("OPEN_HAMMERS_IN_HOME");	break;
			case WAIT_OPEN_HAMMERS_IN_HOME:	SD_printf("WAIT_OPEN_HAMMERS_IN_HOME");	break;
			case BACK_TO_CLOSE:				SD_printf("BACK_TO_CLOSE");		break;
			case RETURNING_TO_C2:			SD_printf("RETURNING_FOR_SCAN");	break;
			case FAIL:						SD_printf("FAIL");				break;
			case SUCCESS:					SD_printf("SUCCESS");			break;
			default:						SD_printf("???");				break;
		}
		SD_printf("\n");
	}
	previous_state = state;
	return ret;
}

error_e STRAT_TINY_goto_forgotten_gift(void)
{
	//Le type avec les états est défini séparément de la variable pour que mplab x comprenne ce qu'on veut faire ...
	typedef enum
	{
		INIT,
		BP,
		EA,
		GA,
		EB,
		MB,
		SUBACTION_FORGOTTEN_GIFT,
		FAIL,
		SUCCESS
	}state_e;
	static state_e state = INIT;
	static state_e previous_state = INIT;
	static state_e repli = INIT;
	static map_goal_e goal;

	error_e sub_action;
	error_e ret = IN_PROGRESS;
	switch(state)
	{
		case INIT:		//Décision initiale de trajet
			if(	global.env.map_elements[GOAL_Cadeau3] == ELEMENT_DONE
					&&
				global.env.map_elements[GOAL_Cadeau2] == ELEMENT_DONE	)
			{
				goal = 0;
				ret = END_OK;	//Ne devrait pas arriver... nous appeler alors qu'il n'y a pas de cadeau à faire.. ? cela n'a pas de sens.
			}
			else
			{
				//CHOIX DU CADEAU A ATTEINDRE.
				if(global.env.map_elements[GOAL_Cadeau2] == ELEMENT_DONE)
					goal = GOAL_Cadeau3;	//Cadeau 2 est fait -> on va au cadeau 3.
				else if(global.env.map_elements[GOAL_Cadeau3] == ELEMENT_DONE)
					goal = GOAL_Cadeau2;	//Cadeau 2 pas fait -> on va au cadeau 2.
				else	//Aucun n'est fait, je choisis le plus proche
				{
					if(COLOR_Y(global.env.pos.y) > 2000)
						goal = GOAL_Cadeau3;
					else 
						goal = GOAL_Cadeau2;
				}

				SD_printf("choix cadeau %d : ", goal);

				if(goal == GOAL_Cadeau2)
					state = GA;
				else
					state = EA;


				if(global.env.pos.x > 800)
				{
					if(COLOR_Y(global.env.pos.y) > 1800)
						repli = EB;	//On fait comme si on venait de EB
					else
						repli = MB;	//On fait comme si on venait de MB
				}
				else
				{
					repli = BP;
				}
			}
		break;
		/////////////////////////////////////////
		
		case GA:
			//										in_progress		success						failed
			state = try_going(200, COLOR_Y(1700),	GA,				SUBACTION_FORGOTTEN_GIFT,	repli,		FAST,ANY_WAY, NO_DODGE_AND_WAIT);
		break;
		case EA:
			//										in_progress		success						failed
			state = try_going(200, COLOR_Y(2300),	EA,				SUBACTION_FORGOTTEN_GIFT,	repli,		FAST,ANY_WAY, NO_DODGE_AND_WAIT);
		break;

		/////////////////////////////////////////
		//POINTS DE REPLIS
		case EB:
			//										in_progress	success	failed
			state = try_going(1380, COLOR_Y(2135),	EB,			FAIL,		FAIL,		FAST,ANY_WAY, NO_DODGE_AND_WAIT);
		break;
		case MB:
			//										in_progress	success	failed
			state = try_going(1300, COLOR_Y(1600),	MB,			FAIL,		FAIL,		FAST,ANY_WAY, NO_DODGE_AND_WAIT);
		break;
		case BP:
			//									in_progress		success							failed
			state=try_going(870,COLOR_Y(1800),		BP,			FAIL,		FAIL,		FAST,ANY_WAY, NO_DODGE_AND_WAIT);
		break;

		case SUBACTION_FORGOTTEN_GIFT:
			sub_action = TINY_forgotten_gift(goal);
			switch(sub_action)
			{
				case IN_PROGRESS:
				break;
				case END_OK:
					state = SUCCESS;
				break;
				case END_WITH_TIMEOUT:
				case NOT_HANDLED:
				case FOE_IN_PATH:
				default:
					state = GA;
				break;
			}
		break;
		case FAIL:
			state = INIT;
			ret = NOT_HANDLED;
		break;
		case SUCCESS:
			if(global.env.map_elements[GOAL_Cadeau3] != ELEMENT_DONE)
			{
				goal = GOAL_Cadeau3;
				state = EA;
				repli = BP;
			}
			else if(global.env.map_elements[GOAL_Cadeau2] != ELEMENT_DONE)
			{
				goal = GOAL_Cadeau2;
				state = GA;
				repli = BP;
			}
			else
			{
				//Fini pour les cadeaux !
				state = INIT;
				ret = END_OK;
			}
		break;
		default:
			state = INIT;
		break;
	}

	if(previous_state != state)
	{
		SD_printf("forgGift ->");
		switch(state)
		{
			case INIT:						SD_printf("INIT");					break;
			case BP:						SD_printf("BP");						break;
			case GA:						SD_printf("GA");						break;
			case EB:						SD_printf("EB");						break;
			case MB:						SD_printf("MB");						break;
			case SUBACTION_FORGOTTEN_GIFT:	SD_printf("SUBACTION_FORGOTTEN_GIFT");	break;
			case FAIL:						SD_printf("FAIL");					break;
			case SUCCESS:					SD_printf("SUCCESS");				break;
			default:						SD_printf("???");					break;
		}
		SD_printf("\n");
	}
	previous_state = state;
	return ret;
}

error_e STRAT_TINY_move_adversary_plates_near_bar(bool_e reset)
{
	typedef enum
    {
        INIT = 0,
		GOTO_MIDDLE,
		ENTER_BETWEEN_TWO_PLATES,
		FIRST_BAR,
		SECOND_BAR,		//I'm drunk now
		SMALL_BACK,
		GOING_OUT,
		FAIL,
		DONE
    }state_e;
	static bool_e failed = FALSE;
    static state_e state = INIT;
    static state_e previous_state = INIT-1;
    //static state_e from_state = INIT;
    bool_e entrance = FALSE;
	error_e ret;

    if(reset == TRUE)
    {
        state = INIT;
        previous_state = INIT;
         //from_state = INIT;
        entrance = TRUE;
    }
    else if(state != previous_state)
    {
        entrance = TRUE;
        //from_state = previous_state;
    }
    previous_state = state;
	ret = IN_PROGRESS;

	if(entrance)
	{
		SD_printf("Plates ->");
		switch(state)
		{
			case INIT:						SD_printf("INIT");					break;
			case GOTO_MIDDLE:						SD_printf("GOTO_MIDDLE");						break;
			case ENTER_BETWEEN_TWO_PLATES:						SD_printf("ENTER_BETWEEN_TWO_PLATES");						break;
			case FIRST_BAR:						SD_printf("FIRST_BAR");						break;
			case SECOND_BAR:						SD_printf("SECOND_BAR");						break;
			case SMALL_BACK:						SD_printf("SMALL_BACK");						break;
			case GOING_OUT:						SD_printf("GOING_OUT");						break;
			case FAIL:						SD_printf("FAIL");					break;
			case DONE:					SD_printf("DONE");				break;
			default:						SD_printf("???");					break;
		}
		SD_printf("\n");
	}

    switch(state)
    {
        case INIT:
			failed = FALSE;
			state = GOTO_MIDDLE;
        break;
		case GOTO_MIDDLE:
			state = try_going(1200, COLOR_Y(2550),	GOTO_MIDDLE,				ENTER_BETWEEN_TWO_PLATES,			FAIL,		FAST,ANY_WAY, NO_DODGE_AND_WAIT);
        break;
		case ENTER_BETWEEN_TWO_PLATES:
			state = try_going(1200, COLOR_Y(2850),	ENTER_BETWEEN_TWO_PLATES,				FIRST_BAR,			FAIL,		FAST,ANY_WAY, NO_DODGE_AND_WAIT);
        break;
		case FIRST_BAR:
			state = try_going(380, COLOR_Y(2800),	FIRST_BAR,				SECOND_BAR,			FAIL,		FAST,ANY_WAY, NO_DODGE_AND_WAIT);
        break;
		case SECOND_BAR:
			state = try_going(1450, COLOR_Y(2800),	SECOND_BAR,				SMALL_BACK,			FAIL,		FAST,ANY_WAY, NO_DODGE_AND_WAIT);
        break;
		case SMALL_BACK:
			state = try_going_until_break(1250, COLOR_Y(2800),	SMALL_BACK,				GOING_OUT,			FAIL,		FAST,ANY_WAY, NO_DODGE_AND_WAIT);
        break;
		case GOING_OUT:
			state = try_going_until_break(1200, COLOR_Y(2550),	GOING_OUT,				DONE,			FAIL,		FAST,ANY_WAY, NO_DODGE_AND_WAIT);
		break;
		case FAIL:
			state = GOING_OUT;
			failed = TRUE;
		break;
		case DONE:
			if(failed)
				ret = NOT_HANDLED;
			else
				ret = END_OK;
		break;
		default:
		break;
    }
	if(ret!=IN_PROGRESS)
		state = INIT;

	return ret;
}

/*

-> Tiny part de la case située au plus prêt des cadeaux (idéalement dans le bon sens, çàd le bras vers les cadeaux)
-> Il fonce pour descendre sans s'arrêter les 4 cadeaux
	-> S'il rencontre un adversaire sur cette trajectoire, il recule et passe à l'étape suivante
-> Tiny file ensuite vers le gateau (sans rentrer dans Krusty...)
-> Il souffle les bougies

*/
error_e STRAT_TINY_goto_cake_and_blow_candles(void)
{
	//Le type avec les états est défini séparément de la variable pour que mplab x comprenne ce qu'on veut faire ...
	typedef enum
	{
		INIT,
		EA,
		MA,
		EB,
		MB,
		SB,
		E_C,	//on peut pas utiliser EC (défini ds les libs microchip)
		BC,
		RC,
		SC,
		RUSH,
		BLOW_ALL_CANDLES,
		BLOW_PARTIAL_CANDLES,
		SUBACTION_BLOW_CANDLES,
		CANDLES_FAIL,
		CANDLES_SUCCESS,
		BP,
		END_STATE
	}state_e;
	static state_e state = INIT;
	static state_e previous_state = INIT;
	static state_e from = INIT;
	static state_e failed_state = INIT;
	static state_e end_ok_state = INIT;
	static bool_e entrance = TRUE;
	static bool_e goto_end = FALSE;
	static bool_e all_candles_blown = FALSE;
	static bool_e pt_EC = FALSE, pt_BC = FALSE, pt_RC = FALSE, pt_SC = FALSE;
	static time32_t start_time = 0;

	error_e sub_action;
	error_e ret = IN_PROGRESS;
	switch(state)
	{
		case INIT:		//Décision initiale de trajet
			goto_end = FALSE;
			start_time = global.env.match_time;
			//get_partial_cake(&pt_EC, &pt_BC, &pt_RC, &pt_SC);

			pt_EC = TRUE;
			pt_SC = TRUE;

			SD_printf("choix du point de départ vers le gateau : ");
			//4 zones sont définies... en fonction de l'endroit où on est, on vise un point ou un autre...

			if(global.env.pos.x > 400)
			{	//C'est le cas lorsqu'on vient d'un STEAL
				if(COLOR_Y(global.env.pos.y) > 1800)	//On est plus près de la position visée pour le gateau adverse
				{
					SD_printf("EB\n");
					state = EB;
					from = EA;	//On fait comme si on venait du cadeau
				}
				else
				{
						SD_printf("MB\n");
						state = MB;
						from = MA;	//On fait comme si on venait du cadeau
				}
			}
			else
			{	//C'est le cas lorsqu'on vient d'avoir ouvert des cadeaux
				if(COLOR_Y(global.env.pos.y) > 2135)	//Le cas nominal correspond à 2300 (pour la position du cadeau 4).
				{
					SD_printf("EA\n");
					state = EB;	//Je tente d'aller vers le gateau adverse
					from = MA;
				}
				else
				{
					SD_printf("MA\n");
					state = MA;				//Je me replie pour aller vers mon coté du gateau.
					from = EA;
				}
			}
			if(SWITCH_STRAT_4 == 1){
						state=RUSH;
						from = MA;
			}
		break;

		case RUSH:
			sub_action = TINY_rush();	//On rush les verres de l'ennemi
			switch(sub_action)
			{
				case IN_PROGRESS:
				break;
				case END_OK:
					state = BLOW_ALL_CANDLES;
				break;
				case NOT_HANDLED:
				case FOE_IN_PATH:
				case END_WITH_TIMEOUT:
					if(global.env.pos.x < 600)
					{
						state = EA;
						from = MA;
					}
					else if (global.env.pos.x < 1380)
					{
						from = EB;
						state = MB;
					}
					else
					{
						state = EB;
						from = E_C;
					}
				break;
				default:
				break;
			}
			if(state == BLOW_ALL_CANDLES)
				from = RUSH;
		break;

		//POSITIONS Coté CADEAUX
		case EA:
			//										in_progress		success		failed
			state = try_going(360, COLOR_Y(2135),	EA,				EB,			MA,		FAST,ANY_WAY, NO_DODGE_AND_WAIT);
			if(state == EB)
				from = EA;
		break;

		case MA:
			//										in_progress		success		failed
			state = try_going_until_break(600, COLOR_Y(1650),	MA,				MB,			EA,		FAST,ANY_WAY, NO_DODGE_AND_WAIT); //Je dois passer par le milieu a tt prix !
			if(state == MB)
				from = MA;
		break;

		//POSITIONS Coté GATEAU
		case EB:
			if(goto_end)	//On va bientôt quitter cette sub_action, mais il faut la finir proprement.
			{
				failed_state = BP;
				end_ok_state = END_STATE;
			}
			else
			{
				if(from==EA)
					failed_state = MA;
				else //A priori, c'est que (from==EC ou MB)
					failed_state = MB;

				if(from == E_C || from == BC)
					end_ok_state = MB;
				else
				{
					if(pt_BC)
						end_ok_state = BC;
					else if(pt_EC)
						end_ok_state = E_C;
					else
					{
						failed_state = EB;	//allers-retours .. on doit de toute facon aller en MB... ! car le quart du gateau manquant est coté safe.
						end_ok_state = MB;
					}
				}
				
			}

			//										in_progress		success			failed
			state = try_going_until_break(1380, COLOR_Y(2135),	EB,				end_ok_state,	failed_state, FAST,ANY_WAY, NO_DODGE_AND_WAIT);
			if(state != EB)
				from = EB;	//On sort..alors on sauvegarde d'où on vient.
		break;

		case MB:
			if(goto_end)	//On va bientôt quitter cette sub_action, mais il faut la finir proprement.
			{
				end_ok_state = END_STATE;
				if(COLOR_Y(global.env.pos.y) < 1500)
					failed_state = SB;
				else
					failed_state = BP;
			}
			else
			{
				if(from == SB || from == MB)
				{
					if(pt_BC)
						end_ok_state = BC;
					else
						end_ok_state = EB;
				}
				else
				{
					if(pt_BC)
						end_ok_state = BC;
					else if(pt_RC)
						end_ok_state = RC;
					else
						end_ok_state = SB;
				}
				if(from==MA)
					failed_state = EA;
				else //A priori, c'est que (from==SAFE_TERRITORY_CAKE_PART)
					failed_state = EB;
			}

			//										in_progress		success			failed
			if(from == EB || from == SB)
				state = try_going_until_break(1300, COLOR_Y(1600),	MB,				end_ok_state,	failed_state, FAST,ANY_WAY, NO_DODGE_AND_WAIT);
			else
				state = try_going(1300, COLOR_Y(1600),	MB,				end_ok_state,	failed_state, FAST,ANY_WAY, NO_DODGE_AND_WAIT);

			if(state != MB)	//Sortie de l'état...
				from = MB;	//On sort..alors on sauvegarde d'où on vient.

		break;

		case SB:
			if(from==MB)
			{
				end_ok_state = SC;
				failed_state = MB;
			}
			else
			{
				end_ok_state = MB;
				failed_state = SC;
			}

			//										in_progress		success			failed
			state = try_going_until_break(1480, COLOR_Y(990),	SB,				end_ok_state,	failed_state, FAST,ANY_WAY, NO_DODGE_AND_WAIT);
			if(state != SB)
				from = SB;
		break;
		//Approche du gateau
		case E_C:
			state=try_going(1830,COLOR_Y(2115), E_C,			(pt_EC && pt_SC)?BLOW_ALL_CANDLES:BLOW_PARTIAL_CANDLES,	EB,FAST,(global.env.color==BLUE)?FORWARD:BACKWARD, NO_DODGE_AND_WAIT);

			if(state != E_C)
				from = E_C;

		break;
		case BC:
			state=try_going(1453,COLOR_Y(1832), BC,			BLOW_PARTIAL_CANDLES,	EB,FAST,(global.env.color==BLUE)?FORWARD:BACKWARD, NO_DODGE_AND_WAIT);

			if(state != BC)
				from = BC;
		break;
		case RC:
			//									in_progress		success				failed
			state=try_going(1453,COLOR_Y(1168),	RC,				BLOW_PARTIAL_CANDLES,	MB,FAST,(global.env.color==BLUE)?BACKWARD:FORWARD, NO_DODGE_AND_WAIT);
			//state=try_going(1916,COLOR_Y(865),	SC,				BLOW_ALL_CANDLES,	SB,(global.env.color==BLUE)?BACKWARD:FORWARD, NO_DODGE_AND_WAIT);
			if(state != RC)
				from = RC;
		break;

		case SC:
			//									in_progress		success				failed
			state=try_going(1830,COLOR_Y(885),	SC,				(pt_EC && pt_SC)?BLOW_ALL_CANDLES:BLOW_PARTIAL_CANDLES,	SB,FAST,(global.env.color==BLUE)?BACKWARD:FORWARD, NO_DODGE_AND_WAIT);
			//state=try_going(1916,COLOR_Y(865),	SC,				BLOW_ALL_CANDLES,	SB,(global.env.color==BLUE)?BACKWARD:FORWARD, NO_DODGE_AND_WAIT);
			if(state != SC)
				from = SC;
		break;


		case BLOW_ALL_CANDLES:
			state = SUBACTION_BLOW_CANDLES;
		break;
		case BLOW_PARTIAL_CANDLES:
			// argument :
			/*Uint8 point;

			if(from == E_C)
				point = POINT_EC;
			else if(from == BC)
				point = POINT_BC;
			else if(from == RC)
				point = POINT_RC;
			else
				point = POINT_SC;
			blow_partial_candle(point);
			 */

		break;

		case SUBACTION_BLOW_CANDLES:					//Souffler bougies
			sub_action = TINY_warner_blow_all_candles();
			switch(sub_action)
			{
				case IN_PROGRESS:
				break;
				case END_OK:
					state = CANDLES_SUCCESS;
					all_candles_blown=TRUE;
				break;
				case END_WITH_TIMEOUT:
				case NOT_HANDLED:
				case FOE_IN_PATH:
				default:
					state = CANDLES_FAIL;
				break;
			}
		break;

		case CANDLES_FAIL:	//No break...
		case CANDLES_SUCCESS:
			//On vient de terminer (en échec ou pas.. de souffler les bougies...)
			//En fonction de notre position, on rejoint un point éloigné du gateau pour terminer les choses proprement
			if(COLOR_Y(global.env.pos.y) > 1500)	//Je suis plus près de chez l'adversaire
			{
				state = EB;
				from = E_C;
			}
			else
			{
				state = SB;
				from = SC;
			}
			goto_end = TRUE;	//On va terminer cette MAE proprement... on se dirige vers les points de FIN...
		break;
		case BP:
			//									in_progress		success			failed
			state=try_going(870,COLOR_Y(1800),	BP,				END_STATE,		EB,		FAST,ANY_WAY, NO_DODGE_AND_WAIT);
		break;

		case END_STATE:
			//Nous sommes soit en F, soit en O, soit en EC. On arrête là.
			state = INIT;
			if(all_candles_blown)
				ret = END_OK;	//On a fait le boulot.
			else
				ret = NOT_HANDLED; //On a pas fait le boulot.
		break;
		default:
		break;
	}

	entrance = (previous_state != state)?TRUE:FALSE;	//Pour le prochain passage dans la machine.

	//GESTION DU TIMEOUT
	if(entrance)
	{
		switch(state)
		{
			case EA:
			case MA:
			case EB:
			case MB:
				if(goto_end == FALSE && ((global.env.match_time - start_time) > 15000))
				{
					ret = NOT_HANDLED;
					state = INIT;
					SD_printf("CAKE TIMEOUT\n");
				}
			break;
			case INIT:
			case SUBACTION_BLOW_CANDLES:
			case E_C:
			case SC:
			case SB:
			case RUSH:
				//Hors de question de partir en timeout sur ces points... c'est vital pour la suite.
			default:
				//Je viens de changer d'ordre... c'est le moment de vérifier qu'on est pas en timeout de gateau.
				
			break;
		}
	}
			
	
	if(entrance)
	{
		SD_printf("Cake ->");
		//Ces printf ne sont pas trop génant, car ils ne sont affichés que sur des évènements "rares"...
		//Ils sont très importants pour savoir ce que le robot à fait, du point de vue STRAT HAUT NIVEAU !
		switch(state)
		{
			case INIT:						SD_printf("INIT\n");					break;
			case EA:						SD_printf("EA\n");						break;
			case MA:						SD_printf("MA\n");						break;
			case EB:						SD_printf("EB\n");						break;
			case MB:						SD_printf("MB\n");						break;
			case SB:						SD_printf("SB\n");						break;
			case E_C:						SD_printf("EC\n");						break;
			case BC:						SD_printf("BC\n");						break;
			case RC:						SD_printf("RC\n");						break;
			case SC:						SD_printf("SC\n");						break;
			case BLOW_ALL_CANDLES:			SD_printf("BLOW_ALL_CANDLES\n");		break;
			case BLOW_PARTIAL_CANDLES:		SD_printf("BLOW_PARTIAL_CANDLES\n");	break;
			case SUBACTION_BLOW_CANDLES:	SD_printf("SUBACTION_BLOW_CANDLES\n");	break;
			case CANDLES_FAIL:				SD_printf("CANDLES_FAIL\n");			break;
			case CANDLES_SUCCESS:			SD_printf("CANDLES_SUCCESS\n");		break;
			case BP:						SD_printf("BP\n");						break;
			case RUSH:						SD_printf("RUSH\n");					break;
			case END_STATE:					SD_printf("END_STATE\n");				break;
			default:						SD_printf("???\n");					break;
		}
	}

	previous_state = state;

	return ret;
}







#define NB_MAX_GLASSES ((Uint8)(32))
	static Uint8 nb_glasses = 0;
	static Sint16 glasses_y[NB_MAX_GLASSES];
	static Sint16 glasses_x[NB_MAX_GLASSES];
	static bool_e glasses_seen = FALSE;


	static girafe_t best_girafe;
	

//Return TRUE si des verres ont été vus depuis le reset du scan.
//reset : TRUE pour mettre à 0 l'ensemble des acquisitions
bool_e scan_for_glasses(bool_e reset)
{
	Uint16 value;
	Sint32 value32, max_distance;
	if(reset)
	{
		nb_glasses = 0;
		glasses_seen = FALSE;
		adversary_is_in_zone = FALSE;
	}
	if ((global.env.foe[FOE_1].angle > -2144) || (global.env.foe[FOE_1].angle < -8578) || (global.env.foe[FOE_2].angle > -2144) || (global.env.foe[FOE_2].angle < -8578))
			adversary_is_in_zone = TRUE;
	value = 0;//ADC_getValue(ADC_PIN_DT10_GLASSES);
	value32 = ((Sint32)(39970UL * value) >> 16) - 56;
	//Value 32 est en mm.
	max_distance = (global.env.pos.y < 420 || global.env.pos.y > 2580)?180:380;	//Si je suis en scan INSIDE, distance max vaut 18cm, sinon 38cm.
	if(value32 > 20 && value32 < max_distance && nb_glasses < NB_MAX_GLASSES)
	{
		if(global.env.color == RED)
		{
			glasses_y[nb_glasses] = global.env.pos.y + (Sint16)(value32) + 100;	//TODO + delta entre capteur et centre robot
			glasses_x[nb_glasses] = global.env.pos.x + 30;						//TODO + delta entre capteur et centre robot
		}
		else
		{
			glasses_y[nb_glasses] = global.env.pos.y - (Sint16)(value32) - 100;	//TODO - delta entre capteur et centre robot
			glasses_x[nb_glasses] = global.env.pos.x - 30;						//TODO + delta entre capteur et centre robot
		}
		glasses_seen = TRUE;
		nb_glasses++;
	}

	return glasses_seen;
}


bool_e scan_for_glasses_on_bar(void)
{
	Uint16 value;
	Sint32 value32;

	value = 0;//ADC_getValue(ADC_PIN_DT10_GLASSES);
	value32 = ((Sint32)(39970UL * value) >> 16) - 56;
	//Value 32 est en mm.

	if(value32 > 10 && value32 < 250)
		return TRUE;
	else
		return FALSE;
}


#define DISTANCE_TWO_GLASSES_IN_GIRAFE	(100)	//mm
girafe_t * look_for_the_best_girafe(void)
{
	static girafe_t g;
	Uint8 i;
	best_girafe = (girafe_t){0,0,0,0};
	if(nb_glasses == 0 || adversary_is_in_zone)
		return &best_girafe;

	for(i=0;i<nb_glasses;i++)
	{
		g.nb_glasses = 1;
		g.x_begin = glasses_x[i];
		g.x_end = glasses_x[i];
		g.y_middle = glasses_y[i];

		for(;i+1<nb_glasses && absolute(glasses_x[i+1] - glasses_x[i])< DISTANCE_TWO_GLASSES_IN_GIRAFE;i++)
		{
			g.nb_glasses++;
			g.y_middle += glasses_y[i];
			g.x_end = glasses_x[i];
		}
		g.y_middle /= g.nb_glasses;	//On calcule la moyenne des y.
		SD_printf("n%d: x%d %d y%ld\n",g.nb_glasses, g.x_begin, g.x_end, g.y_middle);
		if(g.nb_glasses >= best_girafe.nb_glasses)
			best_girafe = g;		//On garde la plus grande girafe. En cas d'égalité, on garde LA DERNIERE (car + proche de nous).
	}
	SD_printf("BEST n%d: x%d %d y%ld\n",best_girafe.nb_glasses, best_girafe.x_begin, best_girafe.x_end, best_girafe.y_middle);

	return &best_girafe;
	/*best_girafe contient :
	 * -> les x_begin et x_end de la girafe (peuvent être identiques si un seul verre)
	 * -> le y moyen de la girafe
	*/
}



error_e TINY_rush()
{
		return goto_pos_with_scan_foe((displacement_t[]){
			{{550,COLOR_Y(1800)},FAST},
			{{800,COLOR_Y(1950)},FAST},
			{{1050,COLOR_Y(2100)},FAST},
			{{1380,COLOR_Y(2115)},FAST},
			{{1830,COLOR_Y(2115)},SLOW},
			},5,(global.env.color == RED)?FORWARD:BACKWARD,NO_DODGE_AND_WAIT);
}

#define BCL_EB_BP 3

/*error_e STRAT_TINY_test_moisson_micro(void){

	typedef enum{
		GO_INIT = 0,
		GO_HOME,
		EB,
		BP,
		MA,
		HB,
		MOVE_FAILED,
		POSE_DONE,
		DONE,
		FAIL_CASE,
		CLOSE_ACT_FAIL

	}state_e;
	
	
	 
	error_e ret = IN_PROGRESS;
	static state_e state = GO_INIT;
	static state_e previousState = GO_INIT;
	static Uint8 count = 0;

	bool_e entrance = (previousState == state)?FALSE:TRUE;
	error_e success_state;
	error_e fail_state;
	way_e way;


	switch(state){
		case GO_INIT:
			count = 0;
			state = EB;
			previousState = GO_INIT;
			
			break;
		case EB:

			//EB							  in_progress	success	failed
			success_state = MA;
			fail_state = HB;
			state = try_going(1380, COLOR_Y(2135), EB,success_state,fail_state,BACKWARD,NO_DODGE_AND_WAIT);
			break;

		case BP:
			//BP
			success_state = MA;
			fail_state = DONE;
			if(previousState == MA)
				way = BACKWARD;
			else
				way = FORWARD;
			state = try_going(870, COLOR_Y(1800), BP,success_state,fail_state,way,NO_DODGE_AND_WAIT);
			break;

		case MA:
			//MA
			if(entrance)
				ACT_plier_open();
			success_state = GO_HOME;
			fail_state = BP;
				state = try_going(360, COLOR_Y(1500), MA, success_state, fail_state, FORWARD,NO_DODGE_AND_WAIT);
			break;

		case HB:
			//HB
			//on peu metre un count ici
			if(previousState == MA){//interet?
				success_state = DONE;
				fail_state = HB;
				way = BACKWARD;
			}else if(previousState == GO_HOME){
				success_state = DONE;
				fail_state = MA;
				way = BACKWARD;
			}else{
				if(previousState == EB)
					way = FORWARD;
				else
					way = BACKWARD;
				success_state = BP;
				fail_state = DONE;
			}
			state = try_going(1300, COLOR_Y(1600), HB, success_state, fail_state,way,NO_DODGE_AND_WAIT);
			break;

		case GO_HOME:
			success_state = POSE_DONE;
			fail_state = HB;
			state = try_going(300, COLOR_Y(450), GO_HOME,success_state, fail_state,FORWARD,NO_DODGE_AND_WAIT);
			break;

		case POSE_DONE:
			success_state = DONE;
			fail_state = HB;
			state = try_going(300, COLOR_Y(1100),POSE_DONE,success_state, fail_state,BACKWARD,NO_DODGE_AND_WAIT);
			break;

		case DONE:
			state = GO_INIT;
			ret = END_OK;
			ACT_plier_close();
			break;

		case FAIL_CASE:
			success_state = CLOSE_ACT_FAIL;
			fail_state = CLOSE_ACT_FAIL;
			state = try_relative_move(50,SLOW,BACKWARD,END_AT_LAST_POINT,FAIL_CASE,success_state, fail_state);
			break;

		case CLOSE_ACT_FAIL:
			state = GO_INIT;
			ret = END_WITH_TIMEOUT;
			ACT_plier_close();
			break;

		default:
			state = GO_INIT;
			ret = NOT_HANDLED;
			break;
	}

	previousState = state;
	return ret;

}
*/


error_e STRAT_TINY_test_moisson_micro(void){

	typedef enum{
		GO_INIT = 0,
		GO_HOME,
		EB,
		BP,
		MA,
		HB,
		MOVE_FAILED,
		POSE_DONE,
		DONE,
		FAIL_CASE,
		CLOSE_ACT_FAIL

	}state_e;
	/*
	END_OK=0,
		IN_PROGRESS,
		END_WITH_TIMEOUT,
		NOT_HANDLED,
		FOE_IN_PATH
	 */
	error_e ret = IN_PROGRESS;
	static state_e state = GO_INIT;
	static state_e previousState = GO_INIT;
	static state_e from = GO_INIT;

	static bool_e entrance = TRUE;
	error_e success_state;
	error_e fail_state;

	way_e way;


	switch(state){
		case GO_INIT:
			state = EB;
			previousState = GO_INIT;
			from = EB;
			break;
		case EB:

			//EB							  in_progress	success	failed
			success_state = MA;
			fail_state = HB;
			state = try_going(1380, COLOR_Y(2135), EB,success_state,fail_state,FAST,BACKWARD,NO_DODGE_AND_WAIT);
			if(state != EB)
				from = EB;
			break;

		case BP:
			//BP
			success_state = MA;
			fail_state = DONE;
			if(from == MA)
				way = BACKWARD;
			else
				way = FORWARD;
			state = try_going(870, COLOR_Y(1800), BP,success_state,fail_state,FAST,way,NO_DODGE_AND_WAIT);
			if(state != BP)
				from = BP;
			break;

		case MA:
			//MA
			if(entrance)
				ACT_plier_open();
			success_state = GO_HOME;
			if(from == EB)
				fail_state = FAIL_CASE;
			else
				fail_state = BP;
				state = try_going(360, COLOR_Y(1500), MA, success_state, fail_state, FAST,FORWARD,NO_DODGE_AND_WAIT);
				if(state != MA)
				from = MA;
			break;

		case HB:
			//HB
			//on peu metre un count ici
			if(from == MA){//interet?
				success_state = DONE;
				fail_state = HB;
				way = BACKWARD;
			}else if(from == GO_HOME){
				success_state = DONE;
				fail_state = MA;
				way = BACKWARD;
			}else{
				if(from == EB)
					way = FORWARD;
				else
					way = BACKWARD;
				success_state = BP;
				fail_state = DONE;
			}
			state = try_going(1300, COLOR_Y(1600), HB, success_state, fail_state,FAST,way,NO_DODGE_AND_WAIT);
			if(state != HB)
				from = HB;
			break;

		case GO_HOME:
			success_state = POSE_DONE;
			if(from == MA)
				fail_state = FAIL_CASE;
			else
				fail_state = POSE_DONE;
			state = try_going(300, COLOR_Y(450), GO_HOME,success_state, fail_state,FAST,ANY_WAY,NO_DODGE_AND_WAIT);
			if(state != GO_HOME)
				from = GO_HOME;
			break;

		case POSE_DONE:
			success_state = DONE;
			fail_state = GO_HOME;
			state = try_going(300, COLOR_Y(1100),POSE_DONE,success_state, fail_state,FAST,ANY_WAY,NO_DODGE_AND_WAIT);
			if(state != POSE_DONE)
				from = POSE_DONE;
			break;

		case DONE:
			state = GO_INIT;
			ret = END_OK;
			ACT_plier_close();
			break;

		case FAIL_CASE:
			success_state = CLOSE_ACT_FAIL;
			fail_state = CLOSE_ACT_FAIL;
			state = try_relative_move(50,FAIL_CASE,success_state, fail_state,SLOW,BACKWARD,END_AT_LAST_POINT);
			break;

		case CLOSE_ACT_FAIL:
			state = GO_INIT;
			ret = NOT_HANDLED;
			ACT_plier_close();
			break;

		default:
			state = GO_INIT;
			ret = NOT_HANDLED;
			break;
	}

	
	entrance = (previousState == state)?FALSE:TRUE;
	previousState = state;
	return ret;

}



error_e TINY_protect_glasses(void)
{
	typedef enum
	{
		INIT,
		GA,
		BOUCLIER,
		END_STATE
	}state_e;
	static state_e state = INIT;
	static bool_e bouclier_on = FALSE;

	error_e ret = IN_PROGRESS;
	switch(state)
	{
		case INIT:		//Décision initiale de trajet
			bouclier_on = FALSE;
			state = GA;
			

		break;

		case GA:
			//										in_progress		success						failed
			state = try_going(360, COLOR_Y(1700),	GA,	BOUCLIER,	END_STATE,		FAST,ANY_WAY, NO_DODGE_AND_WAIT);
		break;


		case BOUCLIER:
			bouclier_on = TRUE;
			state = try_going(350,COLOR_Y(450), BOUCLIER,END_STATE,END_STATE,FAST,FORWARD, NO_DODGE_AND_WAIT);
		break;


		case END_STATE:
			state = INIT;
			if(bouclier_on)
				ret = END_OK;	//On a fait le boulot.
			else
				ret = NOT_HANDLED; //On a pas fait le boulot.
		break;
		default:
		break;
	}

	return ret;
}

//END
