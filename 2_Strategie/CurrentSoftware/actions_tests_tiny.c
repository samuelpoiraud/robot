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

#define DEFAULT_SPEED	(SLOW)

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
			//Echec de la mission... en fonction de ma position, je recule puis, je tenterais de me diriger vers le gateau.
			if(COLOR_Y(global.env.pos.y)<1500)	//Je suis "dans mon camp", il abuse l'adversaire !
				state = PARTIAL_COMING_BACK;		//Je tente quand même
			else
				state = FULL_COMING_BACK;		//Si c'est moi qui ai abusé, je recule un poil... pour faire bien devant l'arbitre
		break;
		case ALL_GIFTS_OPENED:			//Cadeaux terminés
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
					state = FULL_COMING_BACK;	//Ras le bol, je tente quand même !	
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
					state = BRING_GLASS;	//Ras le bol, je tente quand même !	
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
					state = GOTO_CAKE;	//Ras le bol, je tente quand même !	
				break;
				case IN_PROGRESS:	
				default:
				break;
            }
		break;
		case GOTO_CAKE:					//Déplacement vers le gateau
			
		
		break;
		case BLOW_ALL_CANDLES:			//Subaction de soufflage des bougies
		
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

/* ----------------------------------------------------------------------------- */
/* 							Tests state_machines multiple              			 */
/* ----------------------------------------------------------------------------- */
