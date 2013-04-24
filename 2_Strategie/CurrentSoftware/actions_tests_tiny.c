/*
 *	Club Robot ESEO 2008 - 2012
 *	Archi'Tech, CHOMP, Shark & Fish, Krusty & Tiny
 *
 *	Fichier : actions_tests_tiny.c
 *	Package : Carte Principale
 *	Description : Tests des actions r�alisables par le robot
 *	Auteur : Cyril, modifi� par Vincent , Antoine , LMG , Herzaeone , Hoobbes
 *	Version 2012/01/14
 */

#include "actions_tests_tiny.h"

#define DEFAULT_SPEED	(FAST)

#define USE_CURVE	0



/* ----------------------------------------------------------------------------- */
/* 							Fonctions d'homologation			                 */
/* ----------------------------------------------------------------------------- */

/*
	Strat�gie d'homologation. Marque quelques points faciles.
	Evite l'adversaire � coup sur !
*/
void TEST_STRAT_T_homologation(void)
{
	#warning "TODO strat�gie simple d'homologation... reprendre un bout des strat�gies plus compliqu�es"
	/*
		REMARQUE : en homologation, l'id�al est de produire une strat�gie semblable � celle vis�e en match.
		Cependant, si on se prend une remarque, ou qu'un truc foire, il peut �tre TRES int�ressant d'avoir sous le coude
		 une strat�gie �l�mentaire qui permet d'�tre homologable
		 	- sortir de la zone, marquer 1 point
		 	- �viter correctement un adversaire (donc il faut un minimum de d�placement quand m�me)
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
						//Sortie de la zone de d�part, on rejoint le point de d�part pour les cadeaux
			//POSITION DE DEPART :  x=250 | y= coll� contre bordure | sens = bras du cot� des cadeaux (ou pas si lancement sans calibration)
			//REMARQUE
			/*
				SI la calibration est effectu�e, on est dans le bon sens quelque soit la couleur
				SINON, (hors match... pour gain de temps) une rotation aura lieu d�s qu'on est "extrait" de la bordure (sens impos�) !
			*/
		case GET_OUT:
			if(global.env.asser.calibrated || global.env.color == RED)
				state = SUBACTION_OPEN_ALL_GIFTS;
			else
				state = GET_OUT_IF_NO_CALIBRATION;	//En bleu, il faut se retourner si on s'est pas calibr� !
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

	//Les variables en minuscule pour pas confondre avec des �tats et static pour garder la valeur entre plusieurs appel de la fonction (et donc entre plusieurs �tats)
	static bool_e all_gifts_done = FALSE; //De base on fait tous les cadeaux, le 4�me inclu. Si on ne pourra pas le faire, cette variable sera mise � FALSE (voir l'�tat FAIL_TO_OPEN_GIFTS)
	static bool_e all_candles_done = FALSE;

	//Mieux vaut utiliser l'environnement qui doit �tre correctement inform� que ces bool�ens redondants...
/*	static bool_e red_cake_blowed = FALSE;  //TRUE quand on a fait la partie du gateau cot� rouge, sinon FALSE
	static bool_e blue_cake_blowed = FALSE;  //TRUE quand on a fait la partie du gateau cot� bleu, sinon FALSE
	static bool_e white_cake_blowed = FALSE;  //TRUE quand on a fait la partie du gateau cot� bougies blanches, sinon FALSE
*/


	switch(state)
	{
						//Sortie de la zone de d�part, on rejoint le point de d�part pour les cadeaux
			//POSITION DE DEPART :  x=250 | y= coll� contre bordure | sens = bras du cot� des cadeaux (ou pas si lancement sans calibration)
			//REMARQUE
			/*
				SI la calibration est effectu�e, on est dans le bon sens quelque soit la couleur
				SINON, (hors match... pour gain de temps) une rotation aura lieu d�s qu'on est "extrait" de la bordure (sens impos�) !
			*/
		case GET_OUT:
			if(global.env.asser.calibrated || global.env.color == RED)
				state = SUBACTION_OPEN_ALL_GIFTS;
			else
				state = GET_OUT_IF_NO_CALIBRATION;	//En bleu, il faut se retourner si on s'est pas calibr� !
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
					all_gifts_done = TRUE;	//On pourrait remplacer ceci par un acc�s � environnement.c
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
					all_candles_done = TRUE;	//On pourrait remplacer ceci par un acc�s � environnement.c
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
						state = SUBACTION_OPEN_SOME_FORGOTTEN_GIFTS;	//Il reste des cadeaux � ouvrir... on y retourne.
					}
				break;
				
				break;
				default:
				break;
            }
		break;
		
		case SUBACTION_STEAL_ADVERSARY_GLASSES:
			//TODO !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			#warning "subaction � faire !"
			sub_action = END_OK;	//Temporaire
			
			if(global.env.match_time >= 80000)	//S'il reste des chose � faire et qu'on a plus que 10 secondes... on abandonne le vol et on y va..
			{
				if( (all_gifts_done == FALSE) ||  (all_candles_done == FALSE) )
				{
					sub_action = END_OK;
					debug_printf("Il reste 10 sec..\n");
					#warning "il faut penser � reseter la MAE de la sub_action pour pouvoir y retourner sans tout p�ter !"
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
						state = SUBACTION_OPEN_SOME_FORGOTTEN_GIFTS;	//Il reste des cadeaux � ouvrir... on y retourne.
						debug_printf("Je retourne m'occuper des cadeaux.\n");
					}
					else if(!all_candles_done)
					{
						state = SUBACTION_GOTO_CAKE_AND_BLOW_CANDLES;	//Il reste des bougies � souffler... on y retourne.
						debug_printf("Je retourne m'occuper des bougies.\n");
					}
					else
					{
						state = SUBACTION_STEAL_ADVERSARY_GLASSES;		//On a plus rien � faire.. on continue d'emmerder l'adversaire.
						debug_printf("Je continue d'emmerder l'adversaire.\n");
					}
				break;
				default:
				break;
            }
		break;

		case SUBACTION_OPEN_SOME_FORGOTTEN_CANDLES:
			#warning "TODO... une subaction qui va faire les bougies oubli�es... et seulement celles ci... "
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
							state = SUBACTION_GOTO_CAKE_AND_BLOW_CANDLES;	//Il reste des bougies � souffler... on y retourne.
							debug_printf("Je retourne m'occuper des bougies.\n");
						}
						else
						{
							state = SUBACTION_STEAL_ADVERSARY_GLASSES;		//On a plus rien � faire.. on continue d'emmerder l'adversaire.
							debug_printf("Je vais emmerder l'adversaire.\n");
						}
					break;
					default:
					break;
				}

		break;
		case SUBACTION_OPEN_SOME_FORGOTTEN_GIFTS:
			#warning "TODO... une subaction qui va faire les cadeaux oubli�s... et seulement ceux l�... "
		break;
		default:
			state = SUBACTION_STEAL_ADVERSARY_GLASSES;
		break;
	}
}



/* ----------------------------------------------------------------------------- */
/* 								Strat�gies de test                     			 */
/* ----------------------------------------------------------------------------- */
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
			state = try_going(360, COLOR_Y(1500),	MA,				MB,			EA,		ANY_WAY, NO_DODGE_AND_WAIT); //Je dois passer par le milieu a tt prix !
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
				end_ok_state = E_C;
				if(from==EA)
					failed_state = MA;
				else //A priori, c'est que (from==ENNEMY_TERRITORY_CAKE_PART)
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
			state = try_going(1380, COLOR_Y(1500),	MB,				end_ok_state,	failed_state, ANY_WAY, NO_DODGE_AND_WAIT);
			
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

	#warning "Samuel : on pourrait ajouter la gestion d'un timeout pour cette subaction... mais je crois que c'est inutile, compte tenu de son placement dans le match"

	entrance = (previous_state != state)?TRUE:FALSE;	//Pour le prochain passage dans la machine.
	if(entrance)
	{
		//Ces printf ne sont pas trop g�nant, car ils ne sont affich�s que sur des �v�nements "rares"...
		//Ils sont tr�s importants pour savoir ce que le robot � fait, du point de vue STRAT HAUT NIVEAU !
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
