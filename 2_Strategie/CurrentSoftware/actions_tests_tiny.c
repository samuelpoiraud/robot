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

#define DEFAULT_SPEED	(FAST)

#define USE_CURVE	0



/* ----------------------------------------------------------------------------- */
/* 							Fonctions d'homologation			                 */
/* ----------------------------------------------------------------------------- */

/*
	Stratégie d'homologation. Marque quelques points faciles.
	Evite l'adversaire à coup sur !
*/
void TEST_STRAT_T_homologation(void)
{
	#warning "TODO stratégie simple d'homologation... reprendre un bout des stratégies plus compliquées"
	/*
		REMARQUE : en homologation, l'idéal est de produire une stratégie semblable à celle visée en match.
		Cependant, si on se prend une remarque, ou qu'un truc foire, il peut être TRES intéressant d'avoir sous le coude
		 une stratégie élémentaire qui permet d'être homologable
		 	- sortir de la zone, marquer 1 point
		 	- éviter correctement un adversaire (donc il faut un minimum de déplacement quand même)
	*/
static enum
	{
		GET_OUT = 0,
		GET_OUT_IF_NO_CALIBRATION,
		TURN_IF_NO_CALIBRATION,
		SUBACTION_OPEN_ALL_GIFTS,
		FAIL_TO_OPEN_GIFTS
		
	}state = GET_OUT;

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
	error_e sub_action;

	//Les variables en minuscule pour pas confondre avec des états et static pour garder la valeur entre plusieurs appel de la fonction (et donc entre plusieurs états)
	static bool_e all_gifts_done = FALSE; //De base on fait tous les cadeaux, le 4ème inclu. Si on ne pourra pas le faire, cette variable sera mise à FALSE (voir l'état FAIL_TO_OPEN_GIFTS)
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
			sub_action = TINY_open_all_gifts();
			switch(sub_action)
            {
				case IN_PROGRESS:
				break;
				case END_OK:
					all_gifts_done = TRUE;	//On pourrait remplacer ceci par un accès à environnement.c
					debug_printf("J'ai fini les cadeaux. Je vais m'occuper des bougies.\n");
					state = SUBACTION_GOTO_CAKE_AND_BLOW_CANDLES;
				break;
				case END_WITH_TIMEOUT:
				case NOT_HANDLED:
				case FOE_IN_PATH:
					debug_printf("Je n'ai pas fini les cadeaux. Je vais m'occuper des bougies.\n");
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
					if(all_gifts_done)
					{
						debug_printf("Je vais emmerder l'adversaire.\n");
						state = SUBACTION_STEAL_ADVERSARY_GLASSES;	//Cas nominal -> on a mis tout nos points.
					}
					else
					{
						debug_printf("Je retourne m'occuper des cadeaux.\n");
						state = SUBACTION_OPEN_SOME_FORGOTTEN_GIFTS;	//Il reste des cadeaux à ouvrir... on y retourne.
					}
				break;
				
				break;
				default:
				break;
            }
		break;
		
		case SUBACTION_STEAL_ADVERSARY_GLASSES:
			//TODO !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			#warning "subaction à faire !"
			sub_action = END_OK;	//Temporaire
			
			if(global.env.match_time >= 80000)	//S'il reste des chose à faire et qu'on a plus que 10 secondes... on abandonne le vol et on y va..
			{
				if( (all_gifts_done == FALSE) ||  (all_candles_done == FALSE) )
				{
					sub_action = END_OK;
					debug_printf("Il reste 10 sec..\n");
					#warning "il faut penser à reseter la MAE de la sub_action pour pouvoir y retourner sans tout péter !"
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
					if(!all_gifts_done)
					{
						state = SUBACTION_OPEN_SOME_FORGOTTEN_GIFTS;	//Il reste des cadeaux à ouvrir... on y retourne.
						debug_printf("Je retourne m'occuper des cadeaux.\n");
					}
					else if(!all_candles_done)
					{
						state = SUBACTION_GOTO_CAKE_AND_BLOW_CANDLES;	//Il reste des bougies à souffler... on y retourne.
						debug_printf("Je retourne m'occuper des bougies.\n");
					}
					else
					{
						state = SUBACTION_STEAL_ADVERSARY_GLASSES;		//On a plus rien à faire.. on continue d'emmerder l'adversaire.
						debug_printf("Je continue d'emmerder l'adversaire.\n");
					}
				break;
				default:
				break;
            }
		break;

		case SUBACTION_OPEN_SOME_FORGOTTEN_CANDLES:
			#warning "TODO... une subaction qui va faire les bougies oubliées... et seulement celles ci... "
		//case SUB_WHITE_CANDLES:					//Souffler bougies
				sub_action = TINY_blow_all_candles();
				switch(sub_action)
				{
					case IN_PROGRESS:
					break;
					case END_OK:
						all_candles_done = TRUE;
						//no break !
					case END_WITH_TIMEOUT:
					case NOT_HANDLED:
						if(!all_candles_done)
						{
							state = SUBACTION_GOTO_CAKE_AND_BLOW_CANDLES;	//Il reste des bougies à souffler... on y retourne.
							debug_printf("Je retourne m'occuper des bougies.\n");
						}
						else
						{
							state = SUBACTION_STEAL_ADVERSARY_GLASSES;		//On a plus rien à faire.. on continue d'emmerder l'adversaire.
							debug_printf("Je vais emmerder l'adversaire.\n");
						}
					break;
					default:
					break;
				}

		break;
		case SUBACTION_OPEN_SOME_FORGOTTEN_GIFTS:
			#warning "TODO... une subaction qui va faire les cadeaux oubliés... et seulement ceux là... "
		break;
		default:
			state = SUBACTION_STEAL_ADVERSARY_GLASSES;
		break;
	}
}



/* ----------------------------------------------------------------------------- */
/* 								Stratégies de test                     			 */
/* ----------------------------------------------------------------------------- */
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
		case INIT:		//Décision initiale de trajet
			goto_end = FALSE;
			debug_printf("choix du point de départ vers le gateau : ");
			//4 zones sont définies... en fonction de l'endroit où on est, on vise un point ou un autre...
			if(global.env.pos.x > 400)
			{	//C'est le cas lorsqu'on vient d'un STEAL
				if(COLOR_Y(global.env.pos.y) > 1800)	//On est plus près de la position visée pour le gateau adverse
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
				if(COLOR_Y(global.env.pos.y) > 2135)	//Le cas nominal correspond à 2300 (pour la position du cadeau 4).
				{
					debug_printf("EA\n");
					state = EA;	//Je tente d'aller vers le gateau adverse
					from = MA;
				}
				else
				{
					debug_printf("MA\n");
					state = MA;				//Je me replie pour aller vers mon coté du gateau.
					from = EA;
				}
			}
		break;
		
		//POSITIONS Coté CADEAUX
		case EA:
			//										in_progress		success		failed
			state = try_going(360, COLOR_Y(2135),	EA,				EB,			MA,		ANY_WAY, NO_DODGE_AND_WAIT);
			if(state == EB)
				from = EA;
		break;

		case MA:
			//										in_progress		success		failed
			state = try_going(360, COLOR_Y(1500),	MA,				MB,			EA,		ANY_WAY, NO_DODGE_AND_WAIT); //Je dois passer par le milieu a tt prix !
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
				end_ok_state = E_C;
				if(from==EA)
					failed_state = MA;
				else //A priori, c'est que (from==ENNEMY_TERRITORY_CAKE_PART)
					failed_state = MB;
			}

			//										in_progress		success			failed
			state = try_going(1380, COLOR_Y(2135),	EB,				end_ok_state,	failed_state, ANY_WAY, NO_DODGE_AND_WAIT);
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
			state = try_going(1380, COLOR_Y(1500),	MB,				end_ok_state,	failed_state, ANY_WAY, NO_DODGE_AND_WAIT);
			
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
				sub_action = TINY_blow_all_candles();
				
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
			state=try_going(870,COLOR_Y(1800),	BP,				END_STATE,		EB,		ANY_WAY, NO_DODGE_AND_WAIT);
		break;
		
		case END_STATE:
			//Nous sommes soit en F, soit en O, soit en ENNEMY_TERRITORY_CAKE_POS. On arrête là.
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

	#warning "Samuel : on pourrait ajouter la gestion d'un timeout pour cette subaction... mais je crois que c'est inutile, compte tenu de son placement dans le match"

	entrance = (previous_state != state)?TRUE:FALSE;	//Pour le prochain passage dans la machine.
	if(entrance)
	{
		//Ces printf ne sont pas trop génant, car ils ne sont affichés que sur des évènements "rares"...
		//Ils sont très importants pour savoir ce que le robot à fait, du point de vue STRAT HAUT NIVEAU !
		debug_printf("STATE :");
		switch(previous_state)
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
		debug_printf("->");
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


/* ----------------------------------------------------------------------------- */
/* 							Autre strats de test             			 */
/* ----------------------------------------------------------------------------- */




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
			state = try_going(1000, COLOR_Y(1000), GOTO_C, GOTO_D, GOTO_B, BACKWARD,NO_DODGE_AND_WAIT);
		break;
		case GOTO_D:
			state = try_going(500, COLOR_Y(1000), GOTO_D, GOTO_A, GOTO_C, BACKWARD,NO_DODGE_AND_WAIT);
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
	static enum
	{
		GET_OUT = 0,
                GOTO_CAKE,
	    TINY_CANDLES,
		COMEBACK2CODEUR,
		DONE
	}state = GET_OUT;

	error_e sub_action;

	switch(state)
	{

		case GET_OUT:

			state = try_going(250, COLOR_Y(2135), GET_OUT, TINY_CANDLES,TINY_CANDLES, (global.env.color==BLUE)?BACKWARD:FORWARD,NO_DODGE_AND_WAIT);
		break;
		case TINY_CANDLES:
			sub_action = TINY_blow_all_candles();
			switch(sub_action)
            {
				case END_OK:
					state=COMEBACK2CODEUR;
				break;
				case END_WITH_TIMEOUT:
					state=COMEBACK2CODEUR;
				case NOT_HANDLED:
					state =COMEBACK2CODEUR;
				break;
				case IN_PROGRESS:
				default:
				break;
            }
		break;


		case COMEBACK2CODEUR:
             state = try_going(500, COLOR_Y(500), COMEBACK2CODEUR, DONE, DONE, (global.env.color==BLUE)?FORWARD:BACKWARD,NO_DODGE_AND_WAIT);

		break;


		case DONE:
		break;
		default:
		break;
	}
}
