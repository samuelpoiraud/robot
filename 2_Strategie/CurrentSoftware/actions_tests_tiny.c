/*
 *	Club Robot ESEO 2008 - 2012
 *	Archi'Tech, CHOMP, Shark & Fish, Krusty & Tiny
 *
 *	Fichier : actions_tests_tiny.c
 *	Package : Carte Principale
 *	Description : Tests des actions réalisables par le robot
 *	Auteur : Cyril, modifié par Vincent , Antoine , LMG , Herzaeone , Hoobbes
 *	Version 2012/01/14
 */

#include "actions_tests_tiny.h"
#include "actions_utils.h"

#define DEFAULT_SPEED	(FAST)

#define USE_CURVE	0

error_e STRAT_TINY_goto_forgotten_gift(void);
error_e STRAT_TINY_scan_and_steal_adversary_glasses(bool_e reset);


/* ----------------------------------------------------------------------------- */
/* 							Fonctions d'homologation			                 */
/* ----------------------------------------------------------------------------- */



/* ----------------------------------------------------------------------------- */
/* 								La strat haut niveau de TINY          			 */
/*		3 grandes actions :														 */
/*			-> Cadeaux															 */
/*			-> aller vers le gateau et souffler les bougies						 */
/*			-> aller emmerder l'adversaire et voler les verres					 */
/* ----------------------------------------------------------------------------- */
void STRAT_TINY_gifts_cake_and_steal(void)
{
	typedef enum
	{
		//STARTER
		GET_OUT = 0,
		GET_OUT_IF_NO_CALIBRATION,
		TURN_IF_NO_CALIBRATION,

		//NOMINAL MATCH
		SUBACTION_OPEN_ALL_GIFTS,
		SUBACTION_GOTO_CAKE_AND_BLOW_CANDLES,
		SUBACTION_STEAL_ADVERSARY_GLASSES,
		
		//FAILING PARTIAL ACTIONS...
		SUBACTION_OPEN_SOME_FORGOTTEN_GIFTS,
		SUBACTION_OPEN_SOME_FORGOTTEN_CANDLES
	}state_e;
	static state_e state = GET_OUT;
	static state_e previous_state = GET_OUT;
	
	error_e sub_action;

	//Les variables en minuscule pour pas confondre avec des états et static pour garder la valeur entre plusieurs appel de la fonction (et donc entre plusieurs états)
	static bool_e all_candles_done = FALSE;

	//Mieux vaut utiliser l'environnement qui doit être correctement informé que ces booléens redondants...
/*	static bool_e red_cake_blowed = FALSE;  //TRUE quand on a fait la partie du gateau coté rouge, sinon FALSE
	static bool_e blue_cake_blowed = FALSE;  //TRUE quand on a fait la partie du gateau coté bleu, sinon FALSE
	static bool_e white_cake_blowed = FALSE;  //TRUE quand on a fait la partie du gateau coté bougies blanches, sinon FALSE
*/


	switch(state)
	{
						//Sortie de la zone de départ, on rejoint le point de départ pour les cadeaux
			//POSITION DE DEPART :  x=250 | y= collé contre bordure | sens = bras du coté des cadeaux (ou pas si lancement sans calibration)
			//REMARQUE
			/*
				SI la calibration est effectuée, on est dans le bon sens quelque soit la couleur
				SINON, (hors match... pour gain de temps) une rotation aura lieu dès qu'on est "extrait" de la bordure (sens imposé) !
			*/
		case GET_OUT:
			if(global.env.asser.calibrated || global.env.color == RED)
				state = SUBACTION_OPEN_ALL_GIFTS;
			else
				state = GET_OUT_IF_NO_CALIBRATION;	//En bleu, il faut se retourner si on s'est pas calibré !
		break;
		case GET_OUT_IF_NO_CALIBRATION:
			//									in_progress					success					failed
			state = try_going(250, COLOR_Y(150), GET_OUT_IF_NO_CALIBRATION, TURN_IF_NO_CALIBRATION, TURN_IF_NO_CALIBRATION, ANY_WAY, NO_AVOIDANCE);
		break;
		case TURN_IF_NO_CALIBRATION:
			//								in_progress				success						failed
			state = try_go_angle(PI4096/2, TURN_IF_NO_CALIBRATION, SUBACTION_OPEN_ALL_GIFTS, SUBACTION_OPEN_ALL_GIFTS, FAST);
		break;
		case SUBACTION_OPEN_ALL_GIFTS:	//Subaction d'ouverture des cadeaux
			//sub_action = TINY_open_all_gifts();
			sub_action = TINY_open_all_gifts_without_pause();
			switch(sub_action)
            {
				case IN_PROGRESS:
				break;
				case END_OK:
					debug_printf("J'ai fini les cadeaux.\n");
					state = SUBACTION_GOTO_CAKE_AND_BLOW_CANDLES;
				break;
				case END_WITH_TIMEOUT:
				case NOT_HANDLED:
				case FOE_IN_PATH:
					debug_printf("Je n'ai pas fini les cadeaux.\n");
					state = SUBACTION_GOTO_CAKE_AND_BLOW_CANDLES;
				break;
				default:
				break;
            }
		break;
		
		case SUBACTION_GOTO_CAKE_AND_BLOW_CANDLES:
			sub_action = STRAT_TINY_goto_cake_and_blow_candles();
			switch(sub_action)
            {
				case IN_PROGRESS:
				break;
				case END_OK:
					all_candles_done = TRUE;	//On pourrait remplacer ceci par un accès à environnement.c
					debug_printf("J'ai fini les bougies.\n");
					//NO BREAK !!! c'est volontaire...
				case END_WITH_TIMEOUT:
				case NOT_HANDLED:
				case FOE_IN_PATH:
					if(all_gifts_done())
						state = SUBACTION_STEAL_ADVERSARY_GLASSES;	//Cas nominal -> on a mis tout nos points.
					else
						state = SUBACTION_OPEN_SOME_FORGOTTEN_GIFTS;	//Il reste des cadeaux à ouvrir... on y retourne.
				break;
				
				break;
				default:
				break;
            }
		break;
		
		case SUBACTION_STEAL_ADVERSARY_GLASSES:
			sub_action = STRAT_TINY_scan_and_steal_adversary_glasses(FALSE);
			
			if(global.env.match_time >= 70000)	//S'il reste des chose à faire et qu'on a plus que 20 secondes... on abandonne le vol et on y va..
			{
				if( all_gifts_done()==FALSE ||  (all_candles_done == FALSE) )
				{
					sub_action = END_OK;
					debug_printf("Il reste 20 sec..\n");
					STRAT_TINY_scan_and_steal_adversary_glasses(TRUE);	//Reset forcé de la MAE !
				}
			}
			switch(sub_action)
            {
				case IN_PROGRESS:
				break;
				case END_OK:
					//NO BREAK !!! c'est volontaire...
				case END_WITH_TIMEOUT:
				case NOT_HANDLED:
				case FOE_IN_PATH:
					if(all_gifts_done() == FALSE)
						state = SUBACTION_OPEN_SOME_FORGOTTEN_GIFTS;	//Il reste des cadeaux à ouvrir... on y retourne.
					else if(!all_candles_done)
						state = SUBACTION_GOTO_CAKE_AND_BLOW_CANDLES;	//Il reste des bougies à souffler... on y retourne.
					else
						state = SUBACTION_STEAL_ADVERSARY_GLASSES;		//On a plus rien à faire.. on continue d'emmerder l'adversaire.
				break;
				default:
				break;
            }
		break;

		case SUBACTION_OPEN_SOME_FORGOTTEN_CANDLES:
			#warning "TODO... une subaction qui va faire les bougies oubliées... et seulement celles ci... "
				sub_action = STRAT_TINY_goto_cake_and_blow_candles();
				switch(sub_action)
				{
					case IN_PROGRESS:
					break;
					case END_OK:
						all_candles_done = TRUE;
						//no break !
					case END_WITH_TIMEOUT:
					case NOT_HANDLED:
					case FOE_IN_PATH:
						if(all_gifts_done() == FALSE)
							state = SUBACTION_OPEN_SOME_FORGOTTEN_GIFTS;	//Il reste des cadeaux à ouvrir... on y retourne.
						else
							state = SUBACTION_STEAL_ADVERSARY_GLASSES;		//On a plus rien à faire.. on continue d'emmerder l'adversaire.
					break;
					default:
					break;
				}

		break;
		case SUBACTION_OPEN_SOME_FORGOTTEN_GIFTS:
			sub_action = STRAT_TINY_goto_forgotten_gift();
			switch(sub_action)
			{
				case IN_PROGRESS:
				break;
				case END_OK:
				case END_WITH_TIMEOUT:
				case NOT_HANDLED:
				case FOE_IN_PATH:
					if(all_candles_done == FALSE)
						state = SUBACTION_OPEN_SOME_FORGOTTEN_CANDLES;	//Il reste des bougies à souffler... on y retourne.
					else
						state = SUBACTION_STEAL_ADVERSARY_GLASSES;		//On a plus rien à faire.. on continue d'emmerder l'adversaire.
				break;
				default:
				break;
			}
			
		break;
		default:
			state = SUBACTION_STEAL_ADVERSARY_GLASSES;
		break;
	}
	
	if(previous_state != state)
	{
		debug_printf("T_STRAT->");
		switch(state)
		{
			case GET_OUT:									debug_printf("get_out\n");											break;
			case GET_OUT_IF_NO_CALIBRATION:					debug_printf("get_out_if_no_calibration\n");						break;
			case TURN_IF_NO_CALIBRATION:					debug_printf("turn_if_no_calibration\n");							break;
			case SUBACTION_OPEN_ALL_GIFTS:					debug_printf("open_all_gifts\n");									break;
			case SUBACTION_GOTO_CAKE_AND_BLOW_CANDLES:		debug_printf("goto_cake_and_blow_candles\n");						break;
			case SUBACTION_STEAL_ADVERSARY_GLASSES:			debug_printf("steal_aversary_glasses\n");							break;
			case SUBACTION_OPEN_SOME_FORGOTTEN_CANDLES:		debug_printf("open_some_forgotten_candles\n");						break;
			case SUBACTION_OPEN_SOME_FORGOTTEN_GIFTS:		debug_printf("open_some_forgotten_gifts\n");						break;
			default:										debug_printf("???\n");												break;
		}
	}	
	previous_state = state;
}








/* ----------------------------------------------------------------------------- */
/* 							Autre strats de test             			 */
/* ----------------------------------------------------------------------------- */


/*
	Stratégie d'homologation. Marque quelques points faciles.
	Evite l'adversaire à coup sur !
*/
void TEST_STRAT_T_homologation(void)
{
	#warning "TODO strategie simple d'homologation... reprendre un bout des strategies plus compliquees"
	/*
		REMARQUE : en homologation, l'idéal est de produire une stratégie semblable à celle visée en match.
		Cependant, si on se prend une remarque, ou qu'un truc foire, il peut être TRES intéressant d'avoir sous le coude
		 une stratégie élémentaire qui permet d'être homologable
		 	- sortir de la zone, marquer 1 point
		 	- éviter correctement un adversaire (donc il faut un minimum de déplacement quand même)
	*/
	typedef enum
	{
		GET_OUT = 0,
		GET_OUT_IF_NO_CALIBRATION,
		TURN_IF_NO_CALIBRATION,
		SUBACTION_OPEN_ALL_GIFTS,
		FAIL_TO_OPEN_GIFTS

	}state_e;
	state_e state = GET_OUT;

	error_e sub_action;

	switch(state)
	{
						//Sortie de la zone de départ, on rejoint le point de départ pour les cadeaux
			//POSITION DE DEPART :  x=250 | y= collé contre bordure | sens = bras du coté des cadeaux (ou pas si lancement sans calibration)
			//REMARQUE
			/*
				SI la calibration est effectuée, on est dans le bon sens quelque soit la couleur
				SINON, (hors match... pour gain de temps) une rotation aura lieu dès qu'on est "extrait" de la bordure (sens imposé) !
			*/
		case GET_OUT:
			if(global.env.asser.calibrated || global.env.color == RED)
				state = SUBACTION_OPEN_ALL_GIFTS;
			else
				state = GET_OUT_IF_NO_CALIBRATION;	//En bleu, il faut se retourner si on s'est pas calibré !
		break;
		case GET_OUT_IF_NO_CALIBRATION:
			sub_action = goto_pos(250,COLOR_Y(300),FAST,ANY_WAY,END_AT_LAST_POINT);
			switch(sub_action)
            {
				case END_OK:
				case END_WITH_TIMEOUT:	//Je ne sais pas quoi faire d'autre... CA DOIT MARCHER !
				case NOT_HANDLED:		//Je ne sais pas quoi faire d'autre... CA DOIT MARCHER !
				case FOE_IN_PATH:
					state = TURN_IF_NO_CALIBRATION;
				break;
				case IN_PROGRESS:
				break;
				default:
				break;
            }
		break;
		case TURN_IF_NO_CALIBRATION:
			sub_action = goto_angle(PI4096/2, FAST);
			switch(sub_action)
            {
				case END_OK:
				case END_WITH_TIMEOUT:	//Je ne sais pas quoi faire d'autre... CA DOIT MARCHER !
				case NOT_HANDLED:		//Je ne sais pas quoi faire d'autre... CA DOIT MARCHER !
					state = SUBACTION_OPEN_ALL_GIFTS;
				break;
				case IN_PROGRESS:
				default:
				break;
            }
		break;
		case SUBACTION_OPEN_ALL_GIFTS:	//Subaction d'ouverture des cadeaux
			sub_action = TINY_open_all_gifts_homolog();
			switch(sub_action)
            {
				case END_OK:
					state = FAIL_TO_OPEN_GIFTS;
				break;
				case END_WITH_TIMEOUT:
				case NOT_HANDLED:
					state = FAIL_TO_OPEN_GIFTS;
				break;
				case IN_PROGRESS:
				default:
				break;
            }
            case FAIL_TO_OPEN_GIFTS:

            break;
        }
}


void STRAT_TINY_test_avoidance_goto_pos_no_dodge_and_wait(void)
{
	typedef enum{
		SORTIR = 0,
		GOTO_A,
		GOTO_B,
		GOTO_C,
		GOTO_D,
		DONE
	}state_e;
	static state_e state = SORTIR;
	static state_e previous_state = SORTIR;

	error_e sub_action;

	switch(state){
		case SORTIR:
			sub_action = goto_pos(600,COLOR_Y(380),FAST,FORWARD,END_AT_LAST_POINT);
			switch(sub_action)
			{
				case IN_PROGRESS:
					break;
				case NOT_HANDLED:
				case END_OK:
				case END_WITH_TIMEOUT:
				case FOE_IN_PATH:
				default:
					state = GOTO_A;
					break;
			}
		break;
		case GOTO_A:
			state = try_going(500, COLOR_Y(500), GOTO_A, GOTO_B, GOTO_D, FORWARD,NO_DODGE_AND_WAIT);
		break;
		case GOTO_B:
			state = try_going(1000, COLOR_Y(500), GOTO_B, GOTO_C, GOTO_A, FORWARD,NO_DODGE_AND_WAIT);
		break;
		case GOTO_C:
			state = try_going(1000, COLOR_Y(1500), GOTO_C, GOTO_D, GOTO_B, BACKWARD,NO_DODGE_AND_WAIT);
		break;
		case GOTO_D:
			state = try_going(500, COLOR_Y(1500), GOTO_D, GOTO_A, GOTO_C, BACKWARD,NO_DODGE_AND_WAIT);
		break;
		case DONE:	//Never happen dans cette strat de test.
		break;
		default:
		break;

	}

	if(previous_state != state)
	{
		debug_printf("%d %c->%c\n",(Uint16)(global.env.match_time & 0xFFFF), 'A' + previous_state - GOTO_A,  'A' + state - GOTO_A);
		previous_state = state;
	}

}

/* ----------------------------------------------------------------------------- */
/* 							Tests state_machines multiple              			 */
/* ----------------------------------------------------------------------------- */


void STRAT_TINY_all_candles(void)
{
	typedef enum
	{
		GET_OUT = 0,
		POS,
	    TINY_CANDLES,
		COMEBACK2CODEUR,
				test1,
				test2,
				mid,
		DONE
	}state_e;

	error_e sub_action;
	static state_e state = GET_OUT;

	switch(state)
	{

		case GET_OUT:

			state = try_going(250, COLOR_Y(865), GET_OUT, POS,POS,FORWARD,NO_DODGE_AND_WAIT);
		break;

		case POS:

			state = try_going(1914, COLOR_Y(865), POS, TINY_CANDLES,TINY_CANDLES,FORWARD,NO_DODGE_AND_WAIT);
		break;
		case TINY_CANDLES:
			sub_action = TINY_warner_blow_all_candles();
			switch(sub_action)
            {
				case END_OK:
					state=COMEBACK2CODEUR;
				break;
				case END_WITH_TIMEOUT:
					state=COMEBACK2CODEUR;
				case NOT_HANDLED:
				case FOE_IN_PATH:
					state =COMEBACK2CODEUR;
				break;
				case IN_PROGRESS:
				default:
				break;
            }
		break;


		case COMEBACK2CODEUR:
             state = try_going(500, COLOR_Y(2154), COMEBACK2CODEUR, test1, test1, (global.env.color==BLUE)?FORWARD:BACKWARD,NO_DODGE_AND_WAIT);

		break;

		case test1:
			sub_action=TINY_forgotten_gift(GOAL_Cadeau2);
			switch(sub_action)
            {
				case END_OK:
					state=mid;
				break;
				case END_WITH_TIMEOUT:
					state=mid;
				case NOT_HANDLED:
				case FOE_IN_PATH:
					state =mid;
				break;
				case IN_PROGRESS:
				default:
				break;
            }
   
		break;

		case mid:
             state = try_going(1000, COLOR_Y(1500), mid,test2, test2, (global.env.color==BLUE)?FORWARD:BACKWARD,NO_DODGE_AND_WAIT);

		break;

		case test2:
             sub_action=TINY_forgotten_gift(GOAL_Cadeau3);
			switch(sub_action)
            {
				case END_OK:
					state=DONE;
				break;
				case END_WITH_TIMEOUT:
					state=DONE;
				case NOT_HANDLED:
				case FOE_IN_PATH:
					state =DONE;
				break;
				case IN_PROGRESS:
				default:
				break;
            }

		break;
		case DONE:
		break;
		default:
		break;
	}
}
