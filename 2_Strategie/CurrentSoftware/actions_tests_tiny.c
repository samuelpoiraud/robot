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

#define DEFAULT_SPEED	(SLOW)

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
		BRING_GLASS,
		GOTO_CAKE,
		BLOW_ALL_CANDLES,
		FAIL_TO_BLOW_CANDLES,
		TRY_OPEN_FORGOT_GIFT,
		TRY_BLOW_FORGOT_CAKE_PART,
		DONE
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
			//Echec de la mission... en fonction de ma position, je recule puis, je tenterais de me diriger vers le gateau.
			if(COLOR_Y(global.env.pos.y)<1500)	//Je suis "dans mon camp", il abuse l'adversaire !
				state = PARTIAL_COMING_BACK;		//Je tente quand m�me
			else
				state = FULL_COMING_BACK;		//Si c'est moi qui ai abus�, je recule un poil... pour faire bien devant l'arbitre
		break;
		case ALL_GIFTS_OPENED:			//Cadeaux termin�s
			state = FULL_COMING_BACK;	//C'est parti pour de nouvelles aventures.
		break;
		case PARTIAL_COMING_BACK:
			//Je recule de 200
			//Je ramasse les verres que je peux...
			sub_action = goto_pos_with_scan_foe((displacement_t[]){{{global.env.pos.x,COLOR_Y(COLOR_Y(global.env.pos.y)-200)},FAST}},1,ANY_WAY,NO_DODGE_AND_WAIT);
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
			sub_action = goto_pos_with_scan_foe((displacement_t[]){{{550,global.env.pos.y},FAST},{{550,COLOR_Y(900)},FAST}},2,FORWARD,NO_DODGE_AND_WAIT);
			switch(sub_action)
            {
				case END_OK:
					state = BRING_GLASS;
				break;				
				case END_WITH_TIMEOUT:
				case NOT_HANDLED:
					state = BRING_GLASS;	//Ras le bol, je tente quand m�me !	
				break;
				case IN_PROGRESS:	
				default:
				break;
            }
			//Ramasser les verres proprement.
		break;
		case BRING_GLASS:
			sub_action = goto_pos_with_scan_foe((displacement_t[]){{{250,COLOR_Y(400)},FAST}},1,ANY_WAY,NO_DODGE_AND_WAIT);
			switch(sub_action)
            {
				case END_OK:
					state = GOTO_CAKE;
				break;				
				case END_WITH_TIMEOUT:
				case NOT_HANDLED:
					state = GOTO_CAKE;	//Ras le bol, je tente quand m�me !	
				break;
				case IN_PROGRESS:	
				default:
				break;
            }
		break;
		case GOTO_CAKE:					//D�placement vers le gateau
			
		
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

/* ----------------------------------------------------------------------------- */
/* 							Tests state_machines multiple              			 */
/* ----------------------------------------------------------------------------- */
