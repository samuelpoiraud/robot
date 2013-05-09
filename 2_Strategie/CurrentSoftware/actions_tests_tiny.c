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
		BP,
		WAIT,
		//FAILING PARTIAL ACTIONS...
		SUBACTION_OPEN_SOME_FORGOTTEN_GIFTS,
		SUBACTION_OPEN_SOME_FORGOTTEN_CANDLES,
		PROTECT_GLASSES,
				DONE,
		FTW
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
			if(SWITCH_STRAT_3 == 0)	//Désactivation du Steal.
			{
				state = BP;
				break;
			}

			sub_action = STRAT_TINY_scan_and_steal_adversary_glasses(FALSE);
			
			/* Code commenté car trop dangereux d'arreter sans reset de TOUTES les machines a état d'en dessous !
			 * Ce n'est pas très génant d'attendre la fin du STEAL (échec ou réussite) avant de prendre une décision...
			 if(global.env.match_time >= 70000)	//S'il reste des chose à faire et qu'on a plus que 20 secondes... on abandonne le vol et on y va..
			{
				if( all_gifts_done()==FALSE ||  (all_candles_done == FALSE) )
				{
					sub_action = END_OK;
					debug_printf("Il reste 20 sec..\n");
					STRAT_TINY_scan_and_steal_adversary_glasses(TRUE);	//Reset forcé de la MAE !
				}
			}*/
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
		case BP:
			//Position d'attente quand on a plus rien à faire. (ou qu'on attend un peu avant de retourner au gateau)
			state = try_going(1000,COLOR_Y(1800), BP, WAIT, WAIT, ANY_WAY, NO_DODGE_AND_WAIT);
		break;
		case WAIT:
			if(global.env.match_time >= 0)	//S'il reste des chose à faire et qu'on a plus que 30 secondes... on abandonne le vol et on y va..
			{
				if(all_gifts_done() == FALSE)
					state = SUBACTION_OPEN_SOME_FORGOTTEN_GIFTS;	//Il reste des cadeaux à ouvrir... on y retourne.
				else if(!all_candles_done)
					state = SUBACTION_OPEN_SOME_FORGOTTEN_CANDLES;	//Il reste des bougies à souffler... on y retourne.
				else if(SWITCH_STRAT_3 == 1)
					state = SUBACTION_STEAL_ADVERSARY_GLASSES;		//C'est le moment de (re)faire un scan
				else if(SWITCH_LAST_POS==1)
					state = PROTECT_GLASSES;
				else
					state = FTW;
			}
		break;

		case PROTECT_GLASSES:
			//Position d'attente quand on a plus rien à faire. (ou qu'on attend un peu avant de retourner au gateau)
			state = try_going((global.env.color==BLUE)?660:700,(global.env.color==BLUE)?400:450, PROTECT_GLASSES,DONE,DONE,FORWARD, NO_DODGE_AND_WAIT);
		break;

		case FTW:
			//Position d'attente quand on a plus rien à faire.
			sub_action = STRAT_TINY_test_moisson_micro();
				switch(sub_action)
				{
					case IN_PROGRESS:
					break;
					case END_OK:
						//no break !
					case END_WITH_TIMEOUT:
					case NOT_HANDLED:
					case FOE_IN_PATH:
						if(all_gifts_done() == FALSE)
							state = SUBACTION_OPEN_SOME_FORGOTTEN_GIFTS;	//Il reste des cadeaux à ouvrir... on y retourne.
						else
							state = PROTECT_GLASSES;		//On a plus rien à faire.. on continue d'emmerder l'adversaire.
					break;
					default:
					break;
				}

		break;

		case DONE:
		break;

		case SUBACTION_OPEN_SOME_FORGOTTEN_CANDLES:
				//sub_action = STRAT_TINY_goto_cake_and_blow_candles();
				sub_action = TINY_forgotten_candles();
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
			case BP:										debug_printf("BP\n");												break;
			case WAIT:										debug_printf("Wait\n");												break;
			default:										debug_printf("???\n");												break;
		}
	}	
	previous_state = state;
}


/*
	Stratégie d'homologation. Marque quelques points faciles.
	Evite l'adversaire à coup sur !
*/
void STRAT_TINY_4_gifts_only(void)
{
	typedef enum
	{
		//STARTER
		GET_OUT = 0,
		GET_OUT_IF_NO_CALIBRATION,
		TURN_IF_NO_CALIBRATION,

		//NOMINAL MATCH
		SUBACTION_OPEN_ALL_GIFTS,
		COME_BACK,
		WAIT,
	}state_e;
	static state_e state = GET_OUT;
	static state_e previous_state = GET_OUT;

	error_e sub_action;

	//Les variables en minuscule pour pas confondre avec des états et static pour garder la valeur entre plusieurs appel de la fonction (et donc entre plusieurs états)

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
					state = COME_BACK;
				break;
				case END_WITH_TIMEOUT:
				case NOT_HANDLED:
				case FOE_IN_PATH:
					debug_printf("Je n'ai pas fini les cadeaux.\n");
					state = COME_BACK;
				break;
				default:
				break;
            }
		break;

		
		case COME_BACK:
			//Position d'attente quand on a plus rien à faire. (ou qu'on attend un peu avant de retourner au gateau)
			state = try_going(250,COLOR_Y(450), COME_BACK, WAIT, WAIT, ANY_WAY, NO_DODGE_AND_WAIT);
		break;
		case WAIT:
			//NOTHING !
		break;
		
		default:
			state = WAIT;
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
			case COME_BACK:									debug_printf("COME_BACK\n");										break;
			case WAIT:										debug_printf("Wait\n");												break;
			default:										debug_printf("???\n");												break;
		}
	}
	previous_state = state;
}



/*
 Idée de strat (contre un adversaire qui NE COMMENCE PAS PAR LE GATEAU !) :
 * -> Départ case proche gateau. (bras coté bordure gateau)
 * -> Avance jusqu'au gateau en sortant le bras
 * rotation pr bougie 1
 * soufflage complet
 * -> goto cadeaux (le 4 de préférence).
 * -> ouverture des cadeaux.
 * -> Scan & Steal
 *
 * Etapes intermédiaire :
 *		goto cadeaux + rapide, vers cadeau 4
 *		ouverture des cadeaux avec warner 4->1... (et 3->1 en cas de pb)
 * Strat pertinente contre un adversaire qui fonce vers les cadeaux et QU'ON A PEUR D'EVITER !
 */

/*
 Autre IdéeS de strat tordue (IDEES EN VRAC !)
 * -> Foncer déplacer les assiettes adverses à la fin des cadeaux
 * -> Foncer vers le gateau adverse pour le faire vers nous ?
 * -> Foncer vers le cadeau 4 pour les faire en revenant (permet d'aller plus vite !)
 * -> faire que quelques scans... et le reste du temps, empecher l'accès au gateau ou aux cadeaux.. en fonction de ce que l'adverisaire n'a pas encore fait.
 *     (cette info peut être choisie par config sur la carte strat par exemple)
 *      Si on sait qu'il font le gateau en dernier... il faut les empecher d'y aller... quitte a faire un scan à 30 sec de la fin (si on a mis tt nos points)
 *
 */

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
	    TINY_CANDLES,
		COMEBACK2CODEUR,
		DONE
	}state_e;

	error_e sub_action;
	static state_e state = GET_OUT;

	switch(state)
	{

		case GET_OUT:

			state = try_going(250, COLOR_Y(865), GET_OUT, TINY_CANDLES,TINY_CANDLES,FORWARD,NO_DODGE_AND_WAIT);
		break;

		case TINY_CANDLES:
			sub_action = TINY_forgotten_candles();
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
             state = try_going(500, COLOR_Y(2154), COMEBACK2CODEUR, DONE, DONE, (global.env.color==BLUE)?FORWARD:BACKWARD,NO_DODGE_AND_WAIT);

		break;
		case DONE:
		break;
		default:
		break;
	}
}


//Strat de test qui va faire du steal.
void STRAT_TINY_test_steals(void)
{
	typedef enum
	{
		//STARTER
		GET_OUT = 0,
		GET_OUT_IF_NO_CALIBRATION,
		TURN_IF_NO_CALIBRATION,

		//NOMINAL MATCH
		SUBACTION_STEAL_ADVERSARY_GLASSES,
	}state_e;
	static state_e state = GET_OUT;
	static state_e previous_state = GET_OUT;

	error_e sub_action;

	//Les variables en minuscule pour pas confondre avec des états et static pour garder la valeur entre plusieurs appel de la fonction (et donc entre plusieurs états)

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
				state = SUBACTION_STEAL_ADVERSARY_GLASSES;
			else
				state = GET_OUT_IF_NO_CALIBRATION;	//En bleu, il faut se retourner si on s'est pas calibré !
		break;
		case GET_OUT_IF_NO_CALIBRATION:
			//									in_progress					success					failed
			state = try_going(250, COLOR_Y(150), GET_OUT_IF_NO_CALIBRATION, TURN_IF_NO_CALIBRATION, TURN_IF_NO_CALIBRATION, ANY_WAY, NO_AVOIDANCE);
		break;
		case TURN_IF_NO_CALIBRATION:
			//								in_progress				success						failed
			state = try_go_angle(PI4096/2, TURN_IF_NO_CALIBRATION, SUBACTION_STEAL_ADVERSARY_GLASSES, SUBACTION_STEAL_ADVERSARY_GLASSES, FAST);
		break;

		case SUBACTION_STEAL_ADVERSARY_GLASSES:
			sub_action = STRAT_TINY_scan_and_steal_adversary_glasses(FALSE);
			switch(sub_action)
            {
				case IN_PROGRESS:
				break;
				case END_OK:
					//NO BREAK !!! c'est volontaire...
				case END_WITH_TIMEOUT:
				case NOT_HANDLED:
				case FOE_IN_PATH:
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
			case SUBACTION_STEAL_ADVERSARY_GLASSES:			debug_printf("steal_aversary_glasses\n");							break;
			default:										debug_printf("???\n");												break;
		}
	}
	previous_state = state;
}




void STRAT_TINY_test_moisson(void){

	typedef enum{
		GO_INIT = 0,
		GO_HOME,
		P_1,
		P_2,
		P_3,
		P_4,
		DONE

	}state_e;

	static state_e state = GO_INIT;
	static state_e previousState = GO_INIT;

	static Uint8 count = 0;

	switch(state){
		case GO_INIT:
			count = 0;
			state = P_1;
			previousState = GO_INIT;

			break;
		case P_1:
			//									  in_progress	success	failed
			state = try_going(300, COLOR_Y(2000), P_1, P_2, GO_HOME,FORWARD,NO_DODGE_AND_WAIT);
			previousState = P_1;
			break;
		case P_2:
			if(count < 2){
				count++;
				ACT_plier_open();
				state = try_going(600, COLOR_Y(2000), P_2, GO_HOME, P_4,FORWARD,NO_DODGE_AND_WAIT);	
			}else{
				state = DONE;
			}
			previousState = P_2;
			break;
		case P_3:
			//risque de boucle entre P_2 P_4
			state = try_going(900, COLOR_Y(2000), P_3, P_4, P_2,FORWARD,NO_DODGE_AND_WAIT);
			previousState = P_3;
			break;

		case P_4:
			//risque de boucle P_1 P_2 P_4
			state = try_going(800, COLOR_Y(1000), P_4, GO_HOME,P_1,FORWARD,NO_DODGE_AND_WAIT);
			previousState = P_4;
			
			break;

		case GO_HOME:
			
			switch(previousState){
				case P_1:
					//non
					state = DONE;
					break;
				case P_2:
					state = try_going(300, COLOR_Y(300), GO_HOME,P_3 ,DONE,FORWARD,NO_DODGE_AND_WAIT);
					break;
				case P_3:
					//non
					state = DONE;
					break;
				case P_4:
					//a la fin
					state = DONE;
					break;
				case GO_INIT:
					// non pas possible
					state = DONE;
					break;
				case GO_HOME:
					//non pas possible 
					state = DONE;
					break;
				case DONE:
					state = DONE;
					break;
				default:
					state = DONE;
					break;
			}
			//previousState = GO_HOME;
			break;
		case DONE:
			ACT_plier_close();
			break;
		default:
			
			break;
	}

}
