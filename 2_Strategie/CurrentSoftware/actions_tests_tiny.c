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
/* 								Stratégies de test                     			 */
/* ----------------------------------------------------------------------------- */
/*

-> Tiny part de la case située au plus prêt des cadeaux (idéalement dans le bon sens, çàd le bras vers les cadeaux)
-> Il fonce pour descendre sans s'arrêter les 4 cadeaux
	-> S'il rencontre un adversaire sur cette trajectoire, il recule et passe à l'étape suivante
-> Tiny file ensuite vers le gateau (sans rentrer dans Krusty...)
-> Il souffle les bougies

*/
void STRAT_TINY_gifts_and_cakecooking(void)
{
	static enum
	{
		GET_OUT = 0,
		GET_OUT_IF_NO_CALIBRATION,
		TURN_IF_NO_CALIBRATION,
		SUBACTION_OPEN_ALL_GIFTS,
		FAIL_TO_OPEN_GIFTS,
		ALL_GIFTS_OPENED,

		SAFE_TERRITORY_CAKE_POS,
		ENNEMY_TERRITORY_CAKE_POS,
	    SAFE_TERRITORY_GIFTS_POS,
		ENNEMY_TERRITORY_GIFTS_POS,
				ENNEMY_TERRITORY_CAKE_PART,
				SAFE_TERRITORY_CAKE_PART,
				MID_GIFTS_POS,
				MID_CAKE_POS,
				SUB_WHITE_CANDLES,
				SUB_RED_CANDLES,
				SUB_BLUE_CANDLES,
				ENNEMY_TERRITORY_CAKE_REFLEXION,
				SAFE_TERRITORY_CAKE_REFLEXION,
		TEPU_MODE,
				TRY_4_GIFT_AGAIN,
		FAIL_TO_BLOW_CANDLES,
		TRY_OPEN_FORGOT_GIFT,
		TRY_BLOW_FORGOT_CAKE_PART,
		DONE
	}state = GET_OUT;

	//avoidance_type_e avoidance_after_gift_before_candles = NO_DODGE_AND_WAIT; //NO_AVOIDANCE;  //evitement a utiliser pourles deplacement entre les cadeaux et le gateau (quand tiny passe au milieu du terrain)

	error_e sub_action;
	bool_e GOTO_4_GIFT;
	bool_e BLOW_RED_CAKE;
	bool_e BLOW_BLUE_CAKE;
	
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
			sub_action = goto_pos(250,COLOR_Y(150),FAST,ANY_WAY,END_AT_LAST_POINT);
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
			sub_action = TINY_open_all_gifts();
			switch(sub_action)
            {
				case END_OK:
					state=ALL_GIFTS_OPENED;
				break;				
				case END_WITH_TIMEOUT:
				case NOT_HANDLED:
					state = FAIL_TO_OPEN_GIFTS;	
				break;
				case IN_PROGRESS:	
				default:
				break;
            }
		break;
		case FAIL_TO_OPEN_GIFTS:		//Echec d'ouverture d'un (des) cadeau(x)
			 GOTO_4_GIFT=FALSE;
             state = MID_GIFTS_POS;			//Jme replie.
		break;

		case ALL_GIFTS_OPENED:			//Cadeaux terminés
			state = ENNEMY_TERRITORY_GIFTS_POS;	//J'attaque
		break;

		//POSITIONS Coté CADEAUX
		case ENNEMY_TERRITORY_GIFTS_POS:
			state = try_going(160, COLOR_Y(2135), ENNEMY_TERRITORY_GIFTS_POS, ENNEMY_TERRITORY_CAKE_POS, MID_GIFTS_POS, (global.env.color==BLUE)?FORWARD:BACKWARD);
		break;

		case MID_GIFTS_POS:
			state = try_going(160, COLOR_Y(1500), MID_GIFTS_POS, MID_CAKE_POS, MID_CAKE_POS, (global.env.color==BLUE)?FORWARD:BACKWARD); //Je dois passer par le milieu a tt prix !
		break;

		case SAFE_TERRITORY_GIFTS_POS:
			state = try_going(160, COLOR_Y(865), SAFE_TERRITORY_GIFTS_POS, MID_GIFTS_POS, MID_GIFTS_POS, (global.env.color==BLUE)?FORWARD:BACKWARD);
		break;

		//POSITIONS Coté GATEAU
		case ENNEMY_TERRITORY_CAKE_POS:
			state = try_going(1380, COLOR_Y(2135), ENNEMY_TERRITORY_CAKE_POS, ENNEMY_TERRITORY_CAKE_PART, MID_CAKE_POS, (global.env.color==BLUE)?FORWARD:BACKWARD);
		break;

		case MID_CAKE_POS:
			state = try_going(1380, COLOR_Y(1500), MID_CAKE_POS, ENNEMY_TERRITORY_CAKE_POS, SAFE_TERRITORY_CAKE_POS, (global.env.color==BLUE)?FORWARD:BACKWARD);
		break;
		
		case SAFE_TERRITORY_CAKE_POS:
			state = try_going(1380, COLOR_Y(865), SAFE_TERRITORY_CAKE_POS, SAFE_TERRITORY_CAKE_PART,SAFE_TERRITORY_CAKE_PART , (global.env.color==BLUE)?FORWARD:BACKWARD); // Je suis dans mon camp !!! si ya un robot, jle bloque.
		break;


		//Choix de la part de gateau a souffler.
		case ENNEMY_TERRITORY_CAKE_PART:
			state = (global.env.color==BLUE)?SUB_RED_CANDLES:SUB_BLUE_CANDLES;
		break;

		case SAFE_TERRITORY_CAKE_PART:
			state = (global.env.color==BLUE)?SUB_BLUE_CANDLES:SUB_RED_CANDLES;
		break;

		//Mode tepu (juste avant la fin de match si on a marqué des points).
		case TEPU_MODE:
			state = try_going(1000, COLOR_Y(2000), TEPU_MODE, DONE, DONE, (global.env.color==BLUE)?FORWARD:BACKWARD);
		break;

		//Mode réflexion si on a des not handled ou timeout;
		case ENNEMY_TERRITORY_CAKE_REFLEXION:
			if(GOTO_4_GIFT==FALSE){
				state=TRY_4_GIFT_AGAIN;
			}
			state = (global.env.color==BLUE)?ENNEMY_TERRITORY_CAKE_POS:SAFE_TERRITORY_CAKE_POS;
		break;

		case SAFE_TERRITORY_CAKE_REFLEXION:
			state = (global.env.color==BLUE)?SAFE_TERRITORY_CAKE_POS:ENNEMY_TERRITORY_CAKE_POS;
		break;

		//SUB ACTIONS QUAND YA EU DES FAILS.
		case TRY_4_GIFT_AGAIN:
			//Je retente le 4e cadeau que j'ai raté.
		break;

		//SUB_ACTIONS des bougies !
		

		case SUB_WHITE_CANDLES:					//Souffler bougies
				sub_action = TINY_white_candles(TRUE);
				switch(sub_action)
				{
					case END_OK:
						state = SAFE_TERRITORY_CAKE_POS;
					break;
					case END_WITH_TIMEOUT:
					case NOT_HANDLED:
						state = MID_CAKE_POS;
					break;
					case IN_PROGRESS:
					default:
					break;
				}
		
		break;

		case SUB_RED_CANDLES:					//Souffler bougies coté rouge
				sub_action = TINY_REDSIDE_candles();
				switch(sub_action)
				{
					case END_OK:
						BLOW_RED_CAKE=TRUE;
						state = (global.env.color==BLUE)?SUB_WHITE_CANDLES:TEPU_MODE;
					break;
					case END_WITH_TIMEOUT:
					case NOT_HANDLED:
						state= (global.env.color==BLUE)?ENNEMY_TERRITORY_CAKE_REFLEXION:SAFE_TERRITORY_CAKE_REFLEXION;
					break;
					case IN_PROGRESS:
					default:
					break;
				}

		break;

		case SUB_BLUE_CANDLES:					//Souffler bougies bleu
				sub_action = TINY_BLUESIDE_candles();
				switch(sub_action)
				{
					case END_OK:
						BLOW_BLUE_CAKE=TRUE;
						state = (global.env.color==BLUE)?TEPU_MODE:SUB_WHITE_CANDLES;
					break;
					case END_WITH_TIMEOUT:
					case NOT_HANDLED:
						state= (global.env.color==BLUE)?SAFE_TERRITORY_CAKE_REFLEXION:ENNEMY_TERRITORY_CAKE_REFLEXION;
					break;
					case IN_PROGRESS:
					default:
					break;
				}

		break;

		case FAIL_TO_BLOW_CANDLES:		//Echec lors du gateau
		
		break;
		case TRY_OPEN_FORGOT_GIFT:		//tentative d'ouverture d'un cadeau échoué
		
		break;
		case TRY_BLOW_FORGOT_CAKE_PART:	//tentative de soufflage d'une partie de gateau échouée
		
		break;
		case DONE:
		break;
		default:
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
				default:
					state = GOTO_A;
					break;
			}
		break;
		case GOTO_A:
			state = try_going(500, COLOR_Y(500), GOTO_A, GOTO_B, GOTO_D, FORWARD);
		break;
		case GOTO_B:
			state = try_going(1000, COLOR_Y(500), GOTO_B, GOTO_C, GOTO_A, FORWARD);
		break;
		case GOTO_C:
			state = try_going(1000, COLOR_Y(2000), GOTO_C, GOTO_D, GOTO_B, BACKWARD);
		break;
		case GOTO_D:
			state = try_going(500, COLOR_Y(2000), GOTO_D, GOTO_A, GOTO_C, BACKWARD);
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
				POS_MID,
	    TINY_BLUESIDE_BLOWJOB,
		BLOW_ALL_WHITY_CANDLES,
		COMEBACK2MID,
		TINY_REDSIDE_BLOWJOB,
		COMEBACK2CODEUR,
		DONE
	}state = GET_OUT;

	error_e sub_action;

	switch(state)
	{
					
		case GET_OUT:
			state = try_going(250, COLOR_Y(2135), GET_OUT, POS_MID, POS_MID, (global.env.color==BLUE)?BACKWARD:FORWARD);

		break;

		case POS_MID:
			state = try_going(1380, COLOR_Y(2135),POS_MID, TINY_BLUESIDE_BLOWJOB, TINY_BLUESIDE_BLOWJOB, (global.env.color==BLUE)?FORWARD:BACKWARD);

		break;



		case TINY_BLUESIDE_BLOWJOB:
			sub_action = TINY_BLUESIDE_candles();
			switch(sub_action)
            {
				case END_OK:
					state=BLOW_ALL_WHITY_CANDLES;
				break;
				case END_WITH_TIMEOUT:
					state=BLOW_ALL_WHITY_CANDLES;
				case NOT_HANDLED:
					state =BLOW_ALL_WHITY_CANDLES;
				break;
				case IN_PROGRESS:
				default:
				break;
            }
		break;


               

		case BLOW_ALL_WHITY_CANDLES:	 // EHHHH SOUFFLER BOUGIES BLANCHES 
			sub_action = TINY_white_candles(TRUE);
			switch(sub_action)
            {
				case END_OK:
					state=COMEBACK2MID;
				break;
				case END_WITH_TIMEOUT:
					state=COMEBACK2MID;
				case NOT_HANDLED:
					state = COMEBACK2MID;
				break;
				case IN_PROGRESS:
				default:
				break;
            }
		break;

		case COMEBACK2MID:
             state = try_going(1380, COLOR_Y(865), COMEBACK2MID, TINY_REDSIDE_BLOWJOB, TINY_REDSIDE_BLOWJOB, (global.env.color==BLUE)?BACKWARD:FORWARD);

		break;


		case TINY_REDSIDE_BLOWJOB:
			sub_action = TINY_REDSIDE_candles();
			switch(sub_action)
            {
				case END_OK:
					state=COMEBACK2CODEUR;
				break;
				case END_WITH_TIMEOUT:
					state=COMEBACK2CODEUR;
				case NOT_HANDLED:
					state = COMEBACK2CODEUR;
				break;
				case IN_PROGRESS:
				default:
				break;
            }
		break;

		case COMEBACK2CODEUR:
             state = try_going(500, COLOR_Y(500), COMEBACK2CODEUR, DONE, DONE, (global.env.color==BLUE)?FORWARD:BACKWARD);

		break;


		case DONE:
		break;
		default:
		break;
	}
}