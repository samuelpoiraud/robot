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
#include "QS/QS_adc.h"

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

error_e steal_glasses(girafe_t * g, bool_e reset)
{
		//Le type avec les �tats est d�fini s�par�ment de la variable pour que mplab x comprenne ce qu'on veut faire ...
	typedef enum
	{
		INIT,
		TA,
		TB,
		OPEN_HAMMERS,
		TC,
		TD,
		FAIL,
		SUCCESS
	}state_e;
	static state_e state = INIT;
	static state_e previous_state = INIT;

	typedef struct
	{
		Sint16 x;
		Sint16 y;
	}point_t;
	static point_t ta;
	static point_t tb;
	static point_t tc;
	static point_t td;

	error_e ret = IN_PROGRESS;

	if(reset)
	{
		state = INIT;
		return END_OK;
	}
	switch(state)
	{
		case INIT:
			//calcul des points selon la position de la girafe...
			ta.x = (g->x_end+g->x_begin)/2;
			ta.y = g->y_middle + (global.env.color == RED)?-300:300;
			td.y = ta.y;

			//B est du cot� o� on a le plus de place pour reculer.
			//C est de l'autre cot�... un peu plus loin que les verres...
			if(ta.x < 1000)
			{
				tb.x = MAX(g->x_end, g->x_begin) + 200;
				tc.x = MIN(g->x_end, g->x_begin) - 100;
				td.x = tc.x - 200;
			}
			else
			{
				tb.x = MIN(g->x_end, g->x_begin) - 200;
				tc.x = MAX(g->x_end, g->x_begin) + 100;
				td.x = tc.x + 200;
			}

			//ECRETAGE...
			tb.y = g->y_middle;
			if(tb.y < 150)	tb.y = 150;
			if(tb.y > 2850)	tb.y = 2850;
			tc.y = tb.y;

			if(tb.x < 250)	tb.x = 250;
			if(tb.x > 2750)	tb.x = 2750;
			if(tc.x < 250)	tc.x = 250;
			if(tc.x > 2750)	tc.x = 2750;
			if(td.x < 250)	tc.x = 250;
			if(td.x > 2750)	tc.x = 2750;

			debug_printf("STEAL : ta=(%d,%d) tb=(%d,%d) tc=(%d,%d) td(%d,%d)\n", ta.x, ta.y, tb.x, tb.y, tc.x, tc.y, td.x, td.y);

			state = TA;
		break;
		case TA:
			//On se place � cot� de la girafe... pour faire un cr�neau et la surprendre par derri�re...
			state = try_going(ta.x, ta.y,	TA, TB,	FAIL,	ANY_WAY, NO_DODGE_AND_WAIT);
		break;
		case TB:
			//On recule "derri�re" la girafe
			//Le point vis� d�pend d'o� on se trouve.. pour favoriser le fait de reculer du cot� o� il y a le plus de place.
			state = try_going(tb.x, tb.y,	TB, OPEN_HAMMERS,	FAIL,	BACKWARD, NO_DODGE_AND_WAIT);
		break;
		case OPEN_HAMMERS:
			ACT_plier_open();
			state = TC;
		break;
		case TC:
			//On ramasse la girafe...
			state = try_going(tc.x, tc.y,	TC, TD,	FAIL,	FORWARD, NO_DODGE_AND_WAIT);
		break;
		case TD:
			//On ressort...
			state = try_going(td.x, td.y,	TD, SUCCESS,	FAIL,	FORWARD, NO_DODGE_AND_WAIT);
		break;
		case FAIL:
			ACT_plier_close();
			state = INIT;	//O� que je sois, je m'arr�te l�... (ca fera une occasion de r�fl�chir � faire autre chose...)
			ret = NOT_HANDLED;
		break;
		case SUCCESS:
			//L�, c'est vraiment la f�te du slip !!!
			state = INIT;
			ret = END_OK;
		break;
		default:
			state = INIT;
		break;
	}

	if(previous_state != state)
	{
		debug_printf("Steal->");
		switch(state)
		{
			case INIT:						debug_printf("INIT");				break;
			case TA:						debug_printf("TA");					break;
			case TB:						debug_printf("TB");					break;
			case OPEN_HAMMERS:				debug_printf("OPEN_HAMMERS");		break;
			case TC:						debug_printf("TC");					break;
			case TD:						debug_printf("TD");					break;
			case FAIL:						debug_printf("FAIL");				break;
			case SUCCESS:					debug_printf("SUCCESS");			break;
			default:						debug_printf("???");				break;
		}
		debug_printf("\n");
	}
	previous_state = state;
	return ret;
}

/* ----------------------------------------------------------------------------- */
/* 							D�coupe de strat�gies                     			 */
/* ----------------------------------------------------------------------------- */


error_e STRAT_TINY_scan_and_steal_adversary_glasses(bool_e reset)
{
		//Le type avec les �tats est d�fini s�par�ment de la variable pour que mplab x comprenne ce qu'on veut faire ...
	typedef enum
	{
		INIT,
		BP,
		SA,
		SC,
		GO_ANGLE,
		SCAN_GLASSES,
		DECISION,
		SUBACTION_STEAL,
		COME_BACK_HOME,
		FAIL,
		SUCCESS
	}state_e;
	static state_e state = INIT;
	static state_e previous_state = INIT;
	static state_e from = INIT;
	static bool_e entrance = TRUE;
	error_e sub_action;
	error_e ret = IN_PROGRESS;
	static girafe_t * g;

	if(reset)
	{
		state = INIT;
		return END_OK;
	}
	switch(state)
	{
		case INIT:		//D�cision initiale de trajet
			debug_printf("point de d�part du steal : ");
			from = BP;
			if(global.env.pos.x > 1000)
				state = SC;
			else
				state = SA;
		break;
		/////////////////////////////////////////
		case SA:
			//										in_progress		success		failed
			state = try_going(160, COLOR_Y(2480),	SA,				GO_ANGLE,	BP,		ANY_WAY, NO_DODGE_AND_WAIT);
			if(state != SA)
				from = SA;
		break;
		case SC:
			//										in_progress		success		failed
			state = try_going(1840, COLOR_Y(2480),	SC,				GO_ANGLE,	BP,		ANY_WAY, NO_DODGE_AND_WAIT);
			if(state != SC)
				from = SC;
		break;
		case BP:
			//									in_progress		success					failed
			state=try_going(1000,COLOR_Y(1800),	BP,				(from == SA)? SC:SA,	(from == SA)?SA:SC,		ANY_WAY, NO_DODGE_AND_WAIT);
			if(state != BP)
				from = BP;
		break;

		case GO_ANGLE:
			state = try_go_angle((global.env.color == RED)?PI4096:0, GO_ANGLE, SCAN_GLASSES, SCAN_GLASSES, FAST);
		break;

		case SCAN_GLASSES:
			if(entrance)
				scan_for_glasses(TRUE);

			if(SWITCH_STRAT_3 == 0)	//D�sactivation du Steal.
				state = COME_BACK_HOME;

			state = try_going((from == SC)?160:1840, COLOR_Y(2480),	SCAN_GLASSES, DECISION,	FAIL, ANY_WAY, NO_DODGE_AND_WAIT);

			if(global.env.pos.updated)
				scan_for_glasses(FALSE);

			if(state != SCAN_GLASSES)
				from = (global.env.pos.x>1000)?SC:SA;

		break;
		case DECISION:
			//Si pas de d�cision positive.. on scanne dans l'autre sens...
			g = look_for_the_best_girafe();
			if(g->nb_glasses > 0)
			{
				state = SUBACTION_STEAL;
			}
			else
				state = SCAN_GLASSES;
//#define I_HAVE_TESTED_THE_FOLLOWING_PART_OF_THIS_FUNCTION
#warning "attention code non debogue"
			#ifndef I_HAVE_TESTED_THE_FOLLOWING_PART_OF_THIS_FUNCTION
				#warning "ce bouchon pourra etre desactive quand le reste de la fonction sera teste correctement !!!"
				//En l'�tat, on refuse d'aller plus loin... pour ne pas comprommetre une homologation par exemple !
				state = FAIL;
			#endif
		break;
		case SUBACTION_STEAL:
			if(entrance)
				steal_glasses(g,TRUE);	//reset MAE.
			sub_action = steal_glasses(g, FALSE);
			switch(sub_action)
			{
				case IN_PROGRESS:
				break;
				case END_OK:
					state = COME_BACK_HOME;
				break;
				default:
					state = FAIL;
				break;
			}
		break;
		case COME_BACK_HOME:
			//Point 1 : chez l'adversaire, en X = 250.
			//Point 2 : la d�pose dans la zone de d�part TINY (d�but de zone !) ATTENTION A NE PAS DETRUIRE NOS VERRES.
			//Point 3 : on retourne sur la ligne du SCAN
			sub_action = goto_pos_with_scan_foe((displacement_t[]){{{250,COLOR_Y(2135)},SLOW},{{250,COLOR_Y(450)},SLOW},{{250,COLOR_Y(2135)},FAST}},3,FORWARD,NO_DODGE_AND_WAIT);
			switch(sub_action)
			{
				case IN_PROGRESS:
				break;
				case END_OK:
					state = SUCCESS;
				break;
				default:
					state = FAIL;
				break;
			}
			state = SUCCESS;
		break;
		case FAIL:
			ACT_plier_close();
			state = INIT;	//O� que je sois, je m'arr�te l�... (ca fera une occasion de r�fl�chir � faire autre chose...)
			ret = NOT_HANDLED;
		break;
		case SUCCESS:
			ACT_plier_close();
			//L�, c'est vraiment la f�te du slip !!!
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
		debug_printf("Scan&Steal->");
		switch(state)
		{
			case INIT:						debug_printf("INIT");				break;
			case BP:						debug_printf("BP");					break;
			case SA:						debug_printf("SA");					break;
			case SC:						debug_printf("SC");					break;
			case GO_ANGLE:					debug_printf("GO_ANGLE");			break;
			case SCAN_GLASSES:				debug_printf("SCAN_GLASSES");		break;
			case DECISION:					debug_printf("DECISION");			break;
			case SUBACTION_STEAL:			debug_printf("SUBACTION_STEAL");	break;
			case COME_BACK_HOME:			debug_printf("COME_BACK_HOME");		break;
			case FAIL:						debug_printf("FAIL");				break;
			case SUCCESS:					debug_printf("SUCCESS");			break;
			default:						debug_printf("???");				break;
		}
		debug_printf("\n");
	}
	previous_state = state;
	return ret;
}

error_e STRAT_TINY_goto_forgotten_gift(void)
{
	//Le type avec les �tats est d�fini s�par�ment de la variable pour que mplab x comprenne ce qu'on veut faire ...
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
	static state_e from = INIT;
	static map_goal_e goal;

	error_e sub_action;
	error_e ret = IN_PROGRESS;
	switch(state)
	{
		case INIT:		//D�cision initiale de trajet
			if(	global.env.map_elements[GOAL_Cadeau3] == ELEMENT_DONE
					&&
				global.env.map_elements[GOAL_Cadeau2] == ELEMENT_DONE	)
			{
				goal = 0;
				ret = END_OK;	//Ne devrait pas arriver... nous appeler alors qu'il n'y a pas de cadeau � faire.. ? cela n'a pas de sens.
			}
			else
			{
				if(global.env.map_elements[GOAL_Cadeau2] == ELEMENT_DONE)
					goal = GOAL_Cadeau3;	//Cadeau 2 est fait -> on va au cadeau 3.
				else
					goal = GOAL_Cadeau2;	//Cadeau 2 pas fait (ou les deux !) -> on va au cadeau 2.

				debug_printf("point de d�part vers le cadeau %d : ", goal);

				if(global.env.pos.x > 800)
				{
					state = BP;
					if(COLOR_Y(global.env.pos.y) > 1800)
						from = EB;	//On fait comme si on venait de EB
					else
						from = MB;	//On fait comme si on venait de MB
				}
				else
				{
					if(goal == GOAL_Cadeau2)
						state = GA;
					else
						state = EA;
					from = BP;
				}
			}
		break;
		/////////////////////////////////////////
		case BP:
			//									in_progress		success							failed
			state=try_going(870,COLOR_Y(1800),	BP,				(goal == GOAL_Cadeau2)? GA:EA,	from,		ANY_WAY, NO_DODGE_AND_WAIT);
			if(state != BP)
				from = BP;
		break;
		case GA:
			//										in_progress		success						failed
			state = try_going(360, COLOR_Y(1700),	GA,				SUBACTION_FORGOTTEN_GIFT,	from,		ANY_WAY, NO_DODGE_AND_WAIT);
			if(state != GA)
				from = GA;
		break;
		case EA:
			//										in_progress		success						failed
			state = try_going(360, COLOR_Y(2135),	EA,				SUBACTION_FORGOTTEN_GIFT,	from,		ANY_WAY, NO_DODGE_AND_WAIT);
			if(state != EA)
				from = EA;
		break;

		/////////////////////////////////////////
		//POINTS DE REPLI...
		case EB:
			//										in_progress	success	failed
			state = try_going(1380, COLOR_Y(2135),	EB,			BP,		MB,		ANY_WAY, NO_DODGE_AND_WAIT);
			if(state != EB)
				from = EB;
		break;
		case MB:
			//										in_progress	success	failed
			state = try_going(1340, COLOR_Y(1500),	MB,			BP,		EB,		ANY_WAY, NO_DODGE_AND_WAIT);
			if(state != MB)
				from = MB;

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
					state = FAIL;
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
				state = SUBACTION_FORGOTTEN_GIFT;
			}
			else if(global.env.map_elements[GOAL_Cadeau2] != ELEMENT_DONE)
			{
				goal = GOAL_Cadeau2;
				state = SUBACTION_FORGOTTEN_GIFT;
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
		debug_printf("forgGift ->");
		switch(state)
		{
			case INIT:						debug_printf("INIT");					break;
			case BP:						debug_printf("BP");						break;
			case GA:						debug_printf("GA");						break;
			case EB:						debug_printf("EB");						break;
			case MB:						debug_printf("MB");						break;
			case SUBACTION_FORGOTTEN_GIFT:	debug_printf("SUBACTION_FORGOTTEN_GIFT");	break;
			case FAIL:						debug_printf("FAIL");					break;
			case SUCCESS:					debug_printf("SUCCESS");				break;
			default:						debug_printf("???");					break;
		}
		debug_printf("\n");
	}
	previous_state = state;
	return ret;
}



/*

-> Tiny part de la case situ�e au plus pr�t des cadeaux (id�alement dans le bon sens, ��d le bras vers les cadeaux)
-> Il fonce pour descendre sans s'arr�ter les 4 cadeaux
	-> S'il rencontre un adversaire sur cette trajectoire, il recule et passe � l'�tape suivante
-> Tiny file ensuite vers le gateau (sans rentrer dans Krusty...)
-> Il souffle les bougies

*/
error_e STRAT_TINY_goto_cake_and_blow_candles(void)
{
	//Le type avec les �tats est d�fini s�par�ment de la variable pour que mplab x comprenne ce qu'on veut faire ...
	typedef enum
	{
		INIT,
		EA,
		MA,
		EB,
		MB,
		SB,
		E_C,	//on peut pas utiliser EC (d�fini ds les libs microchip)
		SC,
		BLOW_ALL_CANDLES,
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

	//avoidance_type_e avoidance_after_gift_before_candles = NO_DODGE_AND_WAIT; //NO_AVOIDANCE;  //evitement a utiliser pourles deplacement entre les cadeaux et le gateau (quand tiny passe au milieu du terrain)

	error_e sub_action;
	error_e ret = IN_PROGRESS;
	switch(state)
	{
		case INIT:		//D�cision initiale de trajet
			goto_end = FALSE;
			debug_printf("choix du point de d�part vers le gateau : ");
			//4 zones sont d�finies... en fonction de l'endroit o� on est, on vise un point ou un autre...
			if(global.env.pos.x > 400)
			{	//C'est le cas lorsqu'on vient d'un STEAL
				if(COLOR_Y(global.env.pos.y) > 1800)	//On est plus pr�s de la position vis�e pour le gateau adverse
				{
					debug_printf("EB\n");
					state = EB;
					from = EA;	//On fait comme si on venait du cadeau
				}
				else
				{
					debug_printf("MB\n");
					state = MB;
					from = MA;	//On fait comme si on venait du cadeau
				}
			}
			else
			{	//C'est le cas lorsqu'on vient d'avoir ouvert des cadeaux
				if(COLOR_Y(global.env.pos.y) > 2135)	//Le cas nominal correspond � 2300 (pour la position du cadeau 4).
				{
					debug_printf("EA\n");
					state = EA;	//Je tente d'aller vers le gateau adverse
					from = MA;
				}
				else
				{
					debug_printf("MA\n");
					state = MA;				//Je me replie pour aller vers mon cot� du gateau.
					from = EA;
				}
			}
		break;

		//POSITIONS Cot� CADEAUX
		case EA:
			//										in_progress		success		failed
			state = try_going(360, COLOR_Y(2135),	EA,				EB,			MA,		ANY_WAY, NO_DODGE_AND_WAIT);
			if(state == EB)
				from = EA;
		break;

		case MA:
			//										in_progress		success		failed
			state = try_going(600, COLOR_Y(1650),	MA,				MB,			EA,		ANY_WAY, NO_DODGE_AND_WAIT); //Je dois passer par le milieu a tt prix !
			if(state == MB)
				from = MA;
		break;

		//POSITIONS Cot� GATEAU
		case EB:
			if(goto_end)	//On va bient�t quitter cette sub_action, mais il faut la finir proprement.
			{
				failed_state = BP;
				end_ok_state = END_STATE;
			}
			else
			{
				if(from == E_C)
					end_ok_state = MB;
				else
					end_ok_state = E_C;
				if(from==EA)
					failed_state = MA;
				else //A priori, c'est que (from==EC ou MB)
					failed_state = MB;
			}

			//										in_progress		success			failed
			state = try_going(1380, COLOR_Y(2135),	EB,				end_ok_state,	failed_state, ANY_WAY, NO_DODGE_AND_WAIT);
			if(state != EB)
				from = EB;	//On sort..alors on sauvegarde d'o� on vient.
		break;

		case MB:
			if(goto_end)	//On va bient�t quitter cette sub_action, mais il faut la finir proprement.
			{
				end_ok_state = END_STATE;
				if(COLOR_Y(global.env.pos.y) < 1500)
					failed_state = SB;
				else
					failed_state = BP;
			}
			else
			{
				if(from==SB)
					end_ok_state = EB;
				else
					end_ok_state = SB;
				if(from==MA)
					failed_state = EA;
				else //A priori, c'est que (from==SAFE_TERRITORY_CAKE_PART)
					failed_state = EB;
			}

			//										in_progress		success			failed
			state = try_going(1340, COLOR_Y(1600),	MB,				end_ok_state,	failed_state, ANY_WAY, NO_DODGE_AND_WAIT);

			if(state != MB)	//Sortie de l'�tat...
				from = MB;	//On sort..alors on sauvegarde d'o� on vient.

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
			state = try_going(1380, COLOR_Y(865),	SB,				end_ok_state,	failed_state, ANY_WAY, NO_DODGE_AND_WAIT);
			if(state != SB)
				from = SB;
		break;
		//Approche du gateau
		case E_C:
			//									in_progress		success				failed
			state=try_going(1916,COLOR_Y(2135), E_C,			BLOW_ALL_CANDLES,	EB,(global.env.color==BLUE)?FORWARD:BACKWARD, NO_DODGE_AND_WAIT);
			if(state != E_C)
				from = E_C;

		break;

		case SC:
			//									in_progress		success				failed
			state=try_going(1916,COLOR_Y(865),	SC,				BLOW_ALL_CANDLES,	SB,(global.env.color==BLUE)?BACKWARD:FORWARD, NO_DODGE_AND_WAIT);
			if(state != SC)
				from = SC;
		break;

		case BLOW_ALL_CANDLES:
			state = SUBACTION_BLOW_CANDLES;
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
			//On vient de terminer (en �chec ou pas.. de souffler les bougies...)
			//En fonction de notre position, on rejoint un point �loign� du gateau pour terminer les choses proprement
			if(COLOR_Y(global.env.pos.y) > 1500)	//Je suis plus pr�s de chez l'adversaire
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
			state=try_going(870,COLOR_Y(1800),	BP,				END_STATE,		EB,		ANY_WAY, NO_DODGE_AND_WAIT);
		break;

		case END_STATE:
			//Nous sommes soit en F, soit en O, soit en ENNEMY_TERRITORY_CAKE_POS. On arr�te l�.
			state = INIT;
			entrance = TRUE;
			if(all_candles_blown)
				return END_OK;	//On a fait le boulot.
			else
				return NOT_HANDLED; //On a pas fait le boulot.
		break;
		default:
		break;
	}

	entrance = (previous_state != state)?TRUE:FALSE;	//Pour le prochain passage dans la machine.
	if(entrance)
	{
		//Ces printf ne sont pas trop g�nant, car ils ne sont affich�s que sur des �v�nements "rares"...
		//Ils sont tr�s importants pour savoir ce que le robot � fait, du point de vue STRAT HAUT NIVEAU !
		debug_printf("Cake ->");
		switch(state)
		{
			case INIT:						debug_printf("INIT");					break;
			case EA:						debug_printf("EA");						break;
			case MA:						debug_printf("MA");						break;
			case EB:						debug_printf("EB");						break;
			case MB:						debug_printf("MB");						break;
			case SB:						debug_printf("SB");						break;
			case E_C:						debug_printf("EC");						break;
			case SC:						debug_printf("SC");						break;
			case BLOW_ALL_CANDLES:			debug_printf("BLOW_ALL_CANDLES");		break;
			case SUBACTION_BLOW_CANDLES:	debug_printf("SUBACTION_BLOW_CANDLES");	break;
			case CANDLES_FAIL:				debug_printf("CANDLES_FAIL");			break;
			case CANDLES_SUCCESS:			debug_printf("CANDLES_SUCCESS");		break;
			case BP:						debug_printf("BP");						break;
			case END_STATE:					debug_printf("END_STATE");				break;
			default:						debug_printf("???");					break;
		}
		debug_printf("\n");
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

//Return TRUE si des verres ont �t� vus depuis le reset du scan.
//reset : TRUE pour mettre � 0 l'ensemble des acquisitions
bool_e scan_for_glasses(bool_e reset)
{
	Uint16 value;
	Sint32 value32;
	if(reset)
	{
		nb_glasses = 0;
		glasses_seen = FALSE;
	}

	value = ADC_getValue(ADC_PIN_DT10_GLASSES);
	value32 = ((Sint32)(39970UL * value) >> 16) - 56;
	//Value 32 est en mm.
	if(value32 > 20 && value32 < 380 && nb_glasses < NB_MAX_GLASSES)
	{
		if(global.env.color == RED)
		{
			glasses_y[nb_glasses] = global.env.pos.y + (Sint16)(value32) + 100;	//TODO + delta entre capteur et centre robot
			glasses_x[nb_glasses] = global.env.pos.x;						//TODO + delta entre capteur et centre robot
		}
		else
		{
			glasses_y[nb_glasses] = global.env.pos.y - (Sint16)(value32) - 100;	//TODO - delta entre capteur et centre robot
			glasses_x[nb_glasses] = global.env.pos.x;						//TODO + delta entre capteur et centre robot
		}
		glasses_seen = TRUE;
		nb_glasses++;
	}

	return glasses_seen;
}

#define DISTANCE_TWO_GLASSES_IN_GIRAFE	(100)	//mm
girafe_t * look_for_the_best_girafe(void)
{
	static girafe_t g;
	Uint8 i;
	best_girafe = (girafe_t){0,0,0,0};
	if(nb_glasses == 0)
		return &best_girafe;
	for(i=0;i<nb_glasses;i++)
	{
		g.nb_glasses = 1;
		g.x_begin = glasses_x[i];
		g.x_end = glasses_x[i];
		g.y_middle = glasses_y[i];

		for(;i+1<nb_glasses && abs(glasses_x[i+1] - glasses_x[i])< DISTANCE_TWO_GLASSES_IN_GIRAFE;i++)
		{
			g.nb_glasses++;
			g.y_middle += glasses_y[i];
			g.x_end = glasses_x[i];
		}
		g.y_middle /= g.nb_glasses;	//On calcule la moyenne des y.
		if(g.nb_glasses >= best_girafe.nb_glasses)
			best_girafe = g;		//On garde la plus grande girafe. En cas d'�galit�, on garde LA DERNIERE (car + proche de nous).
	}
	return &best_girafe;
	/*best_girafe contient :
	 * -> les x_begin et x_end de la girafe (peuvent �tre identiques si un seul verre)
	 * -> le y moyen de la girafe
	*/
}







//END
