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
/* 								Strat�gies de test                     			 */
/* ----------------------------------------------------------------------------- */
/*

-> Tiny part de la case situ�e au plus pr�t des cadeaux (id�alement dans le bon sens, ��d le bras vers les cadeaux)
-> Il fonce pour descendre sans s'arr�ter les 4 cadeaux
	-> S'il rencontre un adversaire sur cette trajectoire, il recule et passe � l'�tape suivante
-> Tiny file ensuite vers le gateau (sans rentrer dans Krusty...)
-> Il souffle les bougies

*/
void STRAT_TINY_gifts_and_cake(void)
{
	static enum
	{
		GET_OUT = 0,
		GET_OUT_IF_NO_CALIBRATION,
		TURN_IF_NO_CALIBRATION,
		SUBACTION_OPEN_ALL_GIFTS,
		FAIL_TO_OPEN_GIFTS,
		ALL_GIFTS_OPENED,
		PARTIAL_COMING_BACK,
		FULL_COMING_BACK,
		GOTO_MIDLE_OF_AREA,
		SUB_WHITE_CANDLE,
		BRING_GLASS,
		GOTO_CAKE,
		BLOW_ALL_CANDLES,
		FAIL_TO_BLOW_CANDLES,
		TRY_OPEN_FORGOT_GIFT,
		TRY_BLOW_FORGOT_CAKE_PART,
		DONE
	}state = GET_OUT;

	avoidance_type_e avoidance_after_gift_before_candles = NO_DODGE_AND_WAIT; //NO_AVOIDANCE;  //evitement a utiliser pourles deplacement entre les cadeaux et le gateau (quand tiny passe au milieu du terrain)

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
                        state = FULL_COMING_BACK;
			//Echec de la mission... en fonction de ma position, je recule puis, je tenterais de me diriger vers le gateau.
			//if(COLOR_Y(global.env.pos.y)<1500)	//Je suis "dans mon camp", il abuse l'adversaire !
			//	state = FULL_COMING_BACK;		//Je tente quand m�me
			//else
			//	state = FULL_COMING_BACK;		//Si c'est moi qui ai abus�, je recule un poil... pour faire bien devant l'arbitre
		break;
		case ALL_GIFTS_OPENED:			//Cadeaux termin�s
			state = FULL_COMING_BACK;	//C'est parti pour de nouvelles aventures.
		break;
		case PARTIAL_COMING_BACK:
			//Je recule de 200
			//Je ramasse les verres que je peux...
			sub_action = goto_pos_with_scan_foe((displacement_t[]){{{global.env.pos.x,COLOR_Y(COLOR_Y(global.env.pos.y)-200)},FAST}},1,ANY_WAY,avoidance_after_gift_before_candles);
			switch(sub_action)
            {
				case END_OK:
					state=FULL_COMING_BACK;
				break;				
				case END_WITH_TIMEOUT:
				case NOT_HANDLED:
					state = FULL_COMING_BACK;	//Ras le bol, je tente quand m�me !
				break;
				case IN_PROGRESS:	
				default:
				break;
            }
		break;
		case FULL_COMING_BACK:
			//sub_action = goto_pos_with_scan_foe((displacement_t[]){{{550,global.env.pos.y},FAST},{{550,COLOR_Y(900)},FAST}},2,FORWARD,NO_DODGE_AND_WAIT);
			sub_action = goto_pos_with_scan_foe((displacement_t[]){{{300,COLOR_Y(1500)},SLOW}},1,ANY_WAY,avoidance_after_gift_before_candles);
			switch(sub_action)
            {
				case END_OK:
					state = GOTO_MIDLE_OF_AREA;
				break;				
				case END_WITH_TIMEOUT:
				case NOT_HANDLED:
					state = GOTO_MIDLE_OF_AREA;	//Ras le bol, je tente quand m�me !
				break;
				case IN_PROGRESS:	
				default:
				break;
            }
			//Ramasser les verres proprement.
		break;
		
		case GOTO_MIDLE_OF_AREA:
			//if(global.env.match_time > 30000) {
			sub_action = goto_pos_with_scan_foe((displacement_t[]){{{1300,COLOR_Y(1500)},SLOW}},1,(global.env.color==BLUE)?FORWARD:BACKWARD,avoidance_after_gift_before_candles);
			switch(sub_action)
            {
				case END_OK:
					state = SUB_WHITE_CANDLE;
				break;				
				case END_WITH_TIMEOUT:
				case NOT_HANDLED:
					state = SUB_WHITE_CANDLE;	//Ras le bol, je tente quand m�me !
				break;
				case IN_PROGRESS:	
				default:
				break;
            }
		break;

		case SUB_WHITE_CANDLE:					//Souffler bougies
				sub_action = TINY_white_candles();
				switch(sub_action)
				{
					case END_OK:
						state = DONE;
					break;
					case END_WITH_TIMEOUT:
					case NOT_HANDLED:
						state = DONE;
					break;
					case IN_PROGRESS:
					default:
					break;
				}
		
		break;
		case BLOW_ALL_CANDLES:			//Subaction de soufflage des bougies
		
		break;
		case FAIL_TO_BLOW_CANDLES:		//Echec lors du gateau
		
		break;
		case TRY_OPEN_FORGOT_GIFT:		//tentative d'ouverture d'un cadeau �chou�
		
		break;
		case TRY_BLOW_FORGOT_CAKE_PART:	//tentative de soufflage d'une partie de gateau �chou�e
		
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
			sub_action = TINY_white_candles();
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