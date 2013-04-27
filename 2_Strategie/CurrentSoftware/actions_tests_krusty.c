/*
 *	Club Robot ESEO 2008 - 2012
 *	Archi'Tech, CHOMP, Shark & Fish, Krusty & Tiny
 *
 *	Fichier : actions_tests_krusty.c
 *	Package : Carte Principale
 *	Description : Tests des actions réalisables par le robot
 *	Auteur : Cyril, modifié par Vincent , Antoine , LMG , Herzaeone , Hoobbes
 *	Version 2012/01/14
 */

#include "actions_tests_krusty.h"
#include "actions_glasses.h"


#define DEFAULT_SPEED	(SLOW)

#define USE_CURVE	0



/* ----------------------------------------------------------------------------- */
/* 							Fonctions d'homologation			                 */
/* ----------------------------------------------------------------------------- */

/*
	Stratégie d'homologation. Marque quelques points faciles.
	Evite l'adversaire à coup sur !
*/
void TEST_STRAT_K_homologation(void)
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


void K_Strat_Coupe(void){
    static enum{
        SORTIR = 0,
        VERRE_1,
        VERRE_2,
        POSER_VERRE,
        ASSIETTE_2,
        ASSIETTE_4,
        ASSIETTE_5,
        DONE,
    }state=ASSIETTE_2;  //SORTIR

    static error_e sub_action;

    switch(state){
        case SORTIR :
            sub_action = K_Sortie();
            switch(sub_action)
            {
                case END_OK:
                    state=ASSIETTE_2;
                    break;
                case END_WITH_TIMEOUT:
                    state=ASSIETTE_2;
                    break;
                case NOT_HANDLED:
                    state=ASSIETTE_2;
                    break;
                case IN_PROGRESS:
                    break;
                default:
                    break;
            }
            break;

        case VERRE_1 :
            sub_action = K_push_half_row_glasses();
            switch(sub_action)
            {
                case END_OK:
                    state=VERRE_2;
                    break;
                case END_WITH_TIMEOUT:
                    state=VERRE_2;
                    break;
                case NOT_HANDLED:
                    state=VERRE_2;
                    break;
                case IN_PROGRESS:
                    break;
                default:
                    break;
            }
            break;

        case VERRE_2 :
            sub_action = K_push_back_row_glasses();
            switch(sub_action)
            {
                case END_OK:
                    state=ASSIETTE_2;
                    break;
                case END_WITH_TIMEOUT:
                    state=ASSIETTE_2;
                    break;
                case NOT_HANDLED:
                    state=ASSIETTE_2;
                    break;
                case IN_PROGRESS:
                    break;
                default:
                    break;
            }
            break;

        case ASSIETTE_2 :
            sub_action = TEST_STRAT_assiettes_evitement_2();
            switch(sub_action)
            {
                case END_OK:
                    state=ASSIETTE_4;
                    break;
                case END_WITH_TIMEOUT:
                    state=ASSIETTE_4;
                    break;
                case NOT_HANDLED:
                    state=ASSIETTE_4;
                    break;
                case IN_PROGRESS:
                    break;
                default:
                    break;
            }
            break;

        case POSER_VERRE :
            //sub_action = Lacher_verres();
            switch(sub_action)
            {
                case END_OK:
                    state=ASSIETTE_4;
                    break;
                case END_WITH_TIMEOUT:
                    state=ASSIETTE_4;
                    break;
                case NOT_HANDLED:
                    state=ASSIETTE_4;
                    break;
                case IN_PROGRESS:
                    break;
                default:
                    break;
            }
            break;

        case ASSIETTE_4 :
            sub_action = Assiete_2_4_lanceur();
            switch(sub_action)
            {
                case END_OK:
                    state=ASSIETTE_5;
                    break;
                case END_WITH_TIMEOUT:
                    state=ASSIETTE_5;
                    break;
                case NOT_HANDLED:
                    break;
                case IN_PROGRESS:
                    break;
                default:
                    break;
            }
            break;

        case ASSIETTE_5 :
            sub_action = Assiete_5_lanceur();
            switch(sub_action)
            {
                case END_OK:
                    state=DONE;
                    break;
                case END_WITH_TIMEOUT:
                    state=DONE;
                    break;
                case NOT_HANDLED:
                    break;
                case IN_PROGRESS:
                    break;
                default:
                    break;
            }
            break;

        case DONE :
            break;
        default :
            break;
    }


}

error_e Assiete_2_4_lanceur(void){
    static error_e sub_action;
    static enum{
        ASSIETTE_2=0,
        ASSIETTE_4,
        LANCEUR,
                LANCEUR2,
        DONE,
    }state=ASSIETTE_2;

    switch (state)
    {
        case ASSIETTE_2:
            sub_action=TEST_STRAT_assiettes_evitement_2();
            switch(sub_action)
            {
                case END_OK:
                    state=ASSIETTE_4;
                    break;

                case END_WITH_TIMEOUT:
                    state=DONE;
                    return END_WITH_TIMEOUT;
                    break;

                case NOT_HANDLED:
                    return NOT_HANDLED;
                    break;

                case IN_PROGRESS:
                    return IN_PROGRESS;
                    break;

                default:
                    state=ASSIETTE_2;
                    return NOT_HANDLED;
                    break;
            }
            break;

        case ASSIETTE_4:
            sub_action=TEST_STRAT_assiettes_evitement_4();
            switch(sub_action)
            {
                case END_OK:
                    state=LANCEUR;
                    break;

                case END_WITH_TIMEOUT:
                    state=DONE;
                    return END_WITH_TIMEOUT;
                    break;

                case NOT_HANDLED:
                    return NOT_HANDLED;
                    break;

                case IN_PROGRESS:
                    return IN_PROGRESS;
                    break;

                default:
                    state=ASSIETTE_2;
                    return NOT_HANDLED;
                    break;
            }
            break;
        case LANCEUR:
            sub_action=TEST_Launcher_ball_gateau_double();
            switch(sub_action)
            {
                case END_OK:
                    state=DONE;
                    return END_OK;
                    break;

                case END_WITH_TIMEOUT:
                    state=DONE;
                    return END_WITH_TIMEOUT;
                    break;
                case NOT_HANDLED:
                    state=LANCEUR2;
                    break;

                case IN_PROGRESS:
                    return IN_PROGRESS;
                    break;

                default:
                    state=ASSIETTE_2;
                    return NOT_HANDLED;
                    break;
            }

            break;

            case LANCEUR2:
            sub_action=TEST_Launcher_ball_mid_double();
            switch(sub_action)
            {
                case END_OK:
                    state=DONE;
                    return END_OK;
                    break;

                case END_WITH_TIMEOUT:
                    state=DONE;
                    return END_WITH_TIMEOUT;
                    break;
                case NOT_HANDLED:
                    state=LANCEUR;
                    break;

                case IN_PROGRESS:
                    return IN_PROGRESS;
                    break;

                default:
                    state=ASSIETTE_2;
                    return NOT_HANDLED;
                    break;
            }

            break;

        case DONE:
            return END_OK;
            break;
    }
    return IN_PROGRESS;
}

error_e Assiete_5_lanceur(void){
    static error_e sub_action;
    static enum{
        ASSIETTE=0,
        LANCEUR,
                LANCEUR2,
        DONE,
    }state=ASSIETTE;

    switch (state)
    {
        case ASSIETTE:
            sub_action=TEST_STRAT_assiettes_evitement_5();
            switch(sub_action)
            {
                case END_OK:
                    state=LANCEUR;
                    break;

                case END_WITH_TIMEOUT:
                    state=DONE;
                    return END_WITH_TIMEOUT;
                    break;
                case NOT_HANDLED:
                    return NOT_HANDLED;
                    break;

                case IN_PROGRESS:
                    return IN_PROGRESS;
                    break;

                default:
                    state=ASSIETTE;
                    return NOT_HANDLED;
                    break;
            }
            break;

        case LANCEUR:
            sub_action=TEST_Launcher_ball_gateau();
            switch(sub_action)
            {
                case END_OK:
                    state=DONE;
                    return END_OK;
                    break;

                case END_WITH_TIMEOUT:
                    state=DONE;
                    return END_WITH_TIMEOUT;
                    break;
                case NOT_HANDLED:
                    state=LANCEUR2;
                    break;

                case IN_PROGRESS:
                    return IN_PROGRESS;
                    break;

                default:
                    state=ASSIETTE;
                    return NOT_HANDLED;
                    break;
            }
            break;

        case LANCEUR2:
            sub_action=TEST_Launcher_ball_mid();
            switch(sub_action)
            {
                case END_OK:
                    state=DONE;
                    return END_OK;
                    break;

                case END_WITH_TIMEOUT:
                    state=DONE;
                    return END_WITH_TIMEOUT;
                    break;
                case NOT_HANDLED:
                    state=LANCEUR;
                    break;

                case IN_PROGRESS:
                    return IN_PROGRESS;
                    break;

                default:
                    state=ASSIETTE;
                    return NOT_HANDLED;
                    break;
            }
            break;

        case DONE:
            return END_OK;
            break;
    }
    return IN_PROGRESS;
}

void TEST_STRAT_ALEXIS() {
//	static bool_e must_init = TRUE;
//	if(must_init) {
//		must_init = FALSE;
//		grab_glass(TRUE, ACT_LIFT_Left);
//		grab_glass(TRUE, ACT_LIFT_Right);
//	}
//
//	grab_glass(FALSE, ACT_LIFT_Left);
//	grab_glass(FALSE, ACT_LIFT_Right);
	
	enum state_e {
		DO_GLASSES,
		DO_PLATES
	};
	static enum state_e state = DO_GLASSES;
	error_e sub_action;

	switch(state) {
		case DO_GLASSES:
			sub_action = K_STRAT_sub_glasses_alexis();
			if(sub_action != IN_PROGRESS)
				state = DO_PLATES;
			break;

		case DO_PLATES:
			K_Strat_Coupe();
			break;
	}

}


error_e drink(void){
	static enum{
		DRINK,
		ALCOOLIC,
		DROP
	}state = DRINK;

	static error_e sub_action;
	error_e ret = IN_PROGRESS;

	switch(state){
		case DRINK:
			sub_action = TEST_SAMUEL_two_first_rows();//two_first_rows();
			if(sub_action != IN_PROGRESS)
				state = ALCOOLIC;
			break;
		case ALCOOLIC:
			sub_action = try_last_row();
			if(sub_action != IN_PROGRESS)
				state = DROP;
			break;
		case DROP:
			sub_action = Lacher_verres(1);
			if(sub_action != IN_PROGRESS)
				ret = END_OK;
			break;
		default:
			break;
	}
	return ret;
}


void K_test_strat_unitaire(void){
	static enum{
		SORTIR,
		ACTION,
		DONE
	}state = SORTIR;

	static error_e sub_action;

	/*if(PORTBbits.RB3)//gauche
		LED_ERROR = 1;
	else
		LED_ERROR = 0;

	if(PORTBbits.RB5)//droite
		LED_USER2 = 1;
	else
		LED_USER2 = 0;*/
	switch(state){
		case SORTIR:
			sub_action = goto_pos(1000,COLOR_Y(380),FAST,FORWARD,END_AT_BREAK);
			switch(sub_action){
				case IN_PROGRESS:
					break;
				case NOT_HANDLED:
					state = ACTION;
					break;
				case END_OK:
					state = ACTION;
					break;
				case END_WITH_TIMEOUT:
					state = ACTION;
					break;
				default:
					state = ACTION;
					break;
			}
			break;
		case ACTION:
			sub_action = drink(); //Mettez ici le nom de votre micro-strat à tester
			//sub_action = two_first_rows(); //Mettez ici le nom de votre micro-strat à tester
			sub_action = TEST_SAMUEL_two_first_rows(); //Mettez ici le nom de votre micro-strat à tester
			switch(sub_action){
				case IN_PROGRESS:
					break;
				case END_OK:
				case NOT_HANDLED:
				case END_WITH_TIMEOUT:
				default:
					state = DONE;
					break;
			}
			break;
		case DONE:
			break;
	}
}

void K_TEST_STRAT_avoidance(void){
	static enum{
		SORTIR,
		DEPLACEMENT1,
		DEPLACEMENT2,
		DONE,
	}state = SORTIR;

	static error_e sub_action;

	switch(state){
		case SORTIR:
			sub_action = goto_pos(600,COLOR_Y(380),FAST,FORWARD,END_AT_BREAK);
			switch(sub_action){
				case IN_PROGRESS:
					break;
				case NOT_HANDLED:
					state = DEPLACEMENT1;
					break;
				case END_OK:
					state = DEPLACEMENT1;
					break;
				case END_WITH_TIMEOUT:
					state = DEPLACEMENT1;
					break;
				default:
					state = DEPLACEMENT1;
					break;
			}

			break;

		case DEPLACEMENT1:
			sub_action = goto_pos_with_scan_foe((displacement_t[]){{{500, COLOR_Y(700)},SLOW},
					{{500, COLOR_Y(1000)},SLOW}},2,ANY_WAY,NO_DODGE_AND_NO_WAIT);
			switch(sub_action){
				case IN_PROGRESS:
					break;
				case NOT_HANDLED:
					state = DONE;
					break;
				case END_OK:
					state = DONE;
					break;
				case END_WITH_TIMEOUT:
					state = DONE;
					break;
				default:
					state = DONE;
					break;
			}
			break;

		case DEPLACEMENT2:
			sub_action = goto_pos_with_scan_foe((displacement_t[]){{{600,COLOR_Y(380)}}},1,ANY_WAY,NO_DODGE_AND_NO_WAIT);
			switch(sub_action){
				case IN_PROGRESS:
					break;
				case NOT_HANDLED:
					state = DONE;
					break;
				case END_OK:
					state = DONE;
					break;
				case END_WITH_TIMEOUT:
					state = DONE;
					break;
				default:
					state = DONE;
					break;
			}
			break;
		case DONE:
			break;
		default:
			break;
			
	}


}

void TEST_STRAT_kdo(void){
	static enum{
		SORTIR = 0,
				KDO1,
				KDO2,
				KDO3,
				KDO4,
				DONE,
	}state = SORTIR;

	static error_e sub_action;

	switch(state){
		case SORTIR:
			sub_action = goto_pos(600,COLOR_Y(380),FAST,FORWARD,END_AT_BREAK);
			switch(sub_action){
				case IN_PROGRESS:
					break;
				case NOT_HANDLED:
					state = KDO1;
					break;
				case END_OK:
					state = KDO1;
					break;
				case END_WITH_TIMEOUT:
					state = KDO1;
					break;
				default:
					state = KDO1;
					break;
			}
			break;
			
		case KDO1:
			sub_action = K_CADEAU1();
			switch(sub_action){
				case IN_PROGRESS:
					break;
				case NOT_HANDLED:
					state = KDO2;
					break;
				case END_OK:
					state = KDO2;
					break;
				case END_WITH_TIMEOUT:
					state = KDO2;
					break;
				default:
					state = KDO2;
					break;
			}
			break;

		case KDO2:
			sub_action = K_CADEAU2();
			switch(sub_action){
				case IN_PROGRESS:
					break;
				case NOT_HANDLED:
					state = KDO3;
					break;
				case END_OK:
					state = KDO3;
					break;
				case END_WITH_TIMEOUT:
					state = KDO3;
					break;
				default:
					state = KDO3;
					break;
			}
			break;

		case KDO3:
			sub_action = K_CADEAU3();
			switch(sub_action){
				case IN_PROGRESS:
					break;
				case NOT_HANDLED:
					state = KDO4;
					break;
				case END_OK:
					state = KDO4;
					break;
				case END_WITH_TIMEOUT:
					state = KDO4;
					break;
				default:
					state = KDO4;
					break;
			}
			break;

		case KDO4:
			sub_action = K_CADEAU4();
			switch(sub_action){
				case IN_PROGRESS:
					break;
				case NOT_HANDLED:
					state = DONE;
					break;
				case END_OK:
					state = DONE;
					break;
				case END_WITH_TIMEOUT:
					state = DONE;
					break;
				default:
					state = DONE;
					break;
			}
			break;
		case DONE:
			break;
	}
}


void TEST_STRAT_assiettes(void){
	static enum{
		SORTIR = 0,
				ASSIETTE1,
				DONE,
	}state = SORTIR;

	static error_e sub_action;

	switch(state){
		case SORTIR:
			sub_action = goto_pos(600,COLOR_Y(380),FAST,FORWARD,END_AT_BREAK);
			switch(sub_action){
				case IN_PROGRESS:
					break;
				case NOT_HANDLED:
					state = ASSIETTE1;
					break;
				case END_OK:
					state = ASSIETTE1;
					break;
				case END_WITH_TIMEOUT:
					state = ASSIETTE1;
					break;
				default:
					state = ASSIETTE1;
					break;
			}
			break;

		case ASSIETTE1:
			sub_action = K_ASSIETE1();

			switch(sub_action){
				case IN_PROGRESS:
					break;
				case NOT_HANDLED:
					state = DONE;
					break;
				case END_OK:
					state = DONE;
					break;
				case END_WITH_TIMEOUT:
					state = DONE;
					break;
				default:
					state = DONE;
					break;
			}
			break;
		case DONE:
			break;
	}

}

void TEST_STRAT_assiettes_lanceur(void){
	static enum{
		SORTIR = 0,
				ASSIETTE1,
                        LANCEUR,
                        DONE,
	}state = SORTIR;

	static error_e sub_action;

	switch(state){
		case SORTIR:
			sub_action = goto_pos(600,COLOR_Y(380),FAST,FORWARD,END_AT_BREAK);
			switch(sub_action){
				case IN_PROGRESS:
					break;
				case NOT_HANDLED:
					state = ASSIETTE1;
					break;
				case END_OK:
					state = ASSIETTE1;
					break;
				case END_WITH_TIMEOUT:
					state = ASSIETTE1;
					break;
				default:
					state = ASSIETTE1;
					break;
			}
			break;

		case ASSIETTE1:
			sub_action = K_ASSIETE1();

			switch(sub_action){
				case IN_PROGRESS:
					break;
				case NOT_HANDLED:
					state = LANCEUR;
					break;
				case END_OK:
					state = LANCEUR;
					break;
				case END_WITH_TIMEOUT:
					state = LANCEUR;
					break;
				default:
					state = LANCEUR;
					break;
			}
			break;
		case DONE:
			break;
            case LANCEUR:
                sub_action = TEST_Launcher_ball_cadeau();
                switch(sub_action)
                {
                    case END_OK:
                        state=DONE;
                        break;

                    case END_WITH_TIMEOUT:
                        

                        state=DONE;
                        break;
					case FOE_IN_PATH:
                    case NOT_HANDLED:
                        state=DONE;
                        break;

                    case IN_PROGRESS:
                        break;
                }
                    break;
        }

}


void TEST_Launcher_ball(void){
    static enum{
                LANCEUR_CADEAU=0,
                LANCEUR_MID,
                LANCEUR_GATEAU,
                STOP_ACT,
                DONE,
    }state=LANCEUR_CADEAU;

    static error_e sub_action;
    static ACT_function_result_e sub_action_act;

    switch(state){
        case LANCEUR_CADEAU:
            sub_action = TEST_Launcher_ball_cadeau();
                    switch(sub_action)
            {
                case END_OK:
                    state=LANCEUR_MID;
                    break;

                case END_WITH_TIMEOUT:
                    ACT_ball_launcher_stop();
                    
                    state=STOP_ACT;
                    break;
				case FOE_IN_PATH:
                case NOT_HANDLED:
                    state=LANCEUR_GATEAU;
                    break;

                case IN_PROGRESS:
                    break;
            }
            break;
        case LANCEUR_MID:
            sub_action = TEST_Launcher_ball_mid();
                    switch(sub_action)
            {
                case END_OK:
                    state=LANCEUR_GATEAU;
                    break;

                case END_WITH_TIMEOUT:
                    ACT_ball_launcher_stop();
                    state=STOP_ACT;
                    break;
				case FOE_IN_PATH:
                case NOT_HANDLED:
                    state=LANCEUR_GATEAU;
                    break;

                case IN_PROGRESS:
                    break;
            }
            break;
        case LANCEUR_GATEAU:
            sub_action = TEST_Launcher_ball_gateau();
                    switch(sub_action)
            {
                case END_OK:
                    state=DONE;
                    break;

                case END_WITH_TIMEOUT:
                    ACT_ball_launcher_stop();
                    state=STOP_ACT;
                    break;
				case FOE_IN_PATH:
                case NOT_HANDLED:
                    state=LANCEUR_CADEAU;
                    break;

                case IN_PROGRESS:
                    break;
            }
            break;
        case STOP_ACT:
            sub_action_act = ACT_get_last_action_result(ACT_QUEUE_BallLauncher);
            switch(sub_action_act)
            {
                case ACT_FUNCTION_InProgress:
                    break;

                case ACT_FUNCTION_Done:
                    state = DONE;
                    break;

                case ACT_FUNCTION_ActDisabled:
                    state=DONE;
                    break;

                case ACT_FUNCTION_RetryLater:
                    state=DONE;
                    break;
                default:
                    state=DONE;
                    break;
            }
            break;
        case DONE:
            break;
        default:
            break;

    }
}

void TEST_STRAT_verres(void){
	static enum{
		SORTIR = 0,
		GO,
		NEXT,
		DONE
	}state = GO;

	static error_e sub_action;

	switch(state){
		case SORTIR:
			sub_action = goto_pos(1000,COLOR_Y(350),FAST,FORWARD,END_AT_BREAK);
			switch(sub_action){
				case IN_PROGRESS:
					break;
				case END_OK:
					state = GO;
					break;
				case END_WITH_TIMEOUT:
					state = GO;
					break;
				case FOE_IN_PATH:
				case NOT_HANDLED:
					state = GO;
					break;
			}
		case GO:
			sub_action = K_push_half_row_glasses();
			switch(sub_action){
				case IN_PROGRESS:
					break;
				case END_WITH_TIMEOUT:
				case NOT_HANDLED:
				case END_OK:
				default:
					state = NEXT;
					break;
			}
			break;

		case NEXT:
			sub_action = K_push_back_row_glasses();
			switch(sub_action){
				case IN_PROGRESS:
					break;
				case END_WITH_TIMEOUT:
				case NOT_HANDLED:
				case END_OK:
				default:
					state = DONE;
					break;
			}
			break;

		case DONE:
			break;
		default:
			debug_printf("fuyez ca va peter");
			break;
	}
}



/* ----------------------------------------------------------------------------- */
/* 							Tests state_machines multiple              			 */
/* ----------------------------------------------------------------------------- */



void TEST_STRAT_assiettes_evitement(void){

    static error_e sub_action;
    static ACT_function_result_e sub_action_act;
    static enum {
		POS_MOVE = 0,
                POS_MOVE2,
                POS_MOVE3,
                GRABBER_DOWN ,
                GRABBER_DOWN_ATT,
                GRABBER_OPEN,
                GRABBER_OPEN_ATT,
		PUSH,
                GRABBER_CRUSH,
                GRABBER_CRUSH_ATT,
		GRABBER_UP,
                GRABBER_UP_ATT,
                BACK,
                DONE,
    } state = POS_MOVE;

//    static bool_e timeout = FALSE;

    switch (state) {
        case POS_MOVE:

            //sub_action = goto_pos_with_scan_foe((displacement_t[]){{{600,COLOR_Y(800)}}},1,FORWARD,NORMAL_WAIT);
            switch(sub_action)
            {
                case END_OK:
                    state=POS_MOVE2;

                    break;

                case END_WITH_TIMEOUT:
                    state=POS_MOVE2;
                    break;
                case NOT_HANDLED:
                    break;

                case IN_PROGRESS:
                    break;

                default:
                    break;
            }
            break;
         case POS_MOVE2:

            sub_action = goto_pos_with_scan_foe((displacement_t[]){{{1000,COLOR_Y(800)}}},1,FORWARD,NORMAL_WAIT);
            switch(sub_action)
            {
                case END_OK:
                    state=POS_MOVE3;
                    break;

                case END_WITH_TIMEOUT:
                    state=POS_MOVE3;
                    break;
                case NOT_HANDLED:
                    break;

                case IN_PROGRESS:
                    break;

                default:
                    break;
            }
            break;
        case POS_MOVE3:

            sub_action = goto_pos_with_scan_foe((displacement_t[]){{{1000,COLOR_Y(600)}}},1,BACKWARD,NORMAL_WAIT);
            switch(sub_action)
            {
                case END_OK:
                    state=GRABBER_DOWN;
                    break;

                case END_WITH_TIMEOUT:
                    state=GRABBER_DOWN;
                    break;
                case NOT_HANDLED:
                    break;

                case IN_PROGRESS:
                    break;

                default:
                    break;
            }
            break;

        case GRABBER_DOWN:

            ACT_plate_rotate(ACT_PLATE_RotateDown);
            state =GRABBER_DOWN_ATT;
            break;
         case GRABBER_DOWN_ATT:
            sub_action_act = ACT_get_last_action_result(ACT_QUEUE_Plate);
            switch(sub_action_act)
            {
                case ACT_FUNCTION_InProgress:
                    break;

                case ACT_FUNCTION_Done:
                    state = GRABBER_OPEN;
                    break;

                case ACT_FUNCTION_ActDisabled:
                    //state = GRABBER_TIDY_2;

                    break;

                case ACT_FUNCTION_RetryLater:
                     //state = GRABBER_TIDY_2;

                    break;
                default:
                    break;
            }
            break;

       
            
        case GRABBER_OPEN:

            ACT_plate_plier(ACT_PLATE_PlierOpen);
            state =GRABBER_OPEN_ATT;
            break;
         case GRABBER_OPEN_ATT:
            sub_action_act = ACT_get_last_action_result(ACT_QUEUE_Plate);
            switch(sub_action_act)
            {
                case ACT_FUNCTION_InProgress:
                    break;

                case ACT_FUNCTION_Done:
                    state = PUSH;
                    break;

                case ACT_FUNCTION_ActDisabled:
                    state = PUSH;

                    break;

                case ACT_FUNCTION_RetryLater:
                     state = PUSH;

                    break;
                default:
                    break;
            }
            break;

        case PUSH:

            sub_action = goto_pos_with_scan_foe((displacement_t[]){{{1000,COLOR_Y(400)}}},1,BACKWARD,NORMAL_WAIT);
            switch(sub_action)
            {
                case END_OK:
                    state=GRABBER_CRUSH;
                    break;

                case END_WITH_TIMEOUT:
                    state=GRABBER_CRUSH;
                    break;
                case NOT_HANDLED:
                    break;

                case IN_PROGRESS:
                    break;

                default:
                    break;
            }
            break;

        case GRABBER_CRUSH:
            ACT_plate_plier(ACT_PLATE_PlierClose);
            state =GRABBER_CRUSH_ATT;
            break;
         case GRABBER_CRUSH_ATT:
            sub_action_act = ACT_get_last_action_result(ACT_QUEUE_Plate);
            switch(sub_action_act)
            {
                case ACT_FUNCTION_InProgress:
                    break;

                case ACT_FUNCTION_Done:
                    state = GRABBER_UP;
                    break;

                case ACT_FUNCTION_ActDisabled:
                    state = GRABBER_UP;

                    break;

                case ACT_FUNCTION_RetryLater:
                     state = GRABBER_UP;

                    break;
                default:
                    break;
            }
            break;

        case GRABBER_UP:

            ACT_plate_rotate(ACT_PLATE_RotateUp);
            state =GRABBER_UP_ATT;
            break;
         case GRABBER_UP_ATT:
            sub_action_act = ACT_get_last_action_result(ACT_QUEUE_Plate);
            switch(sub_action_act)
            {
                case ACT_FUNCTION_InProgress:
                    break;

                case ACT_FUNCTION_Done:
                    state = BACK;
                    break;

                case ACT_FUNCTION_ActDisabled:
                    //state = GRABBER_TIDY_2;

                    break;

                case ACT_FUNCTION_RetryLater:
                     //state = GRABBER_TIDY_2;

                    break;
                default:
                    break;
            }
            break;
        case BACK:

            sub_action = goto_pos_with_scan_foe((displacement_t[]){{{1000,COLOR_Y(700)}}},1,FORWARD,NORMAL_WAIT);
            switch(sub_action)
            {
                case END_OK:
                    state=DONE;
                    break;

                case END_WITH_TIMEOUT:
                    state=DONE;
                    break;
                case NOT_HANDLED:
                    break;

                case IN_PROGRESS:
                    break;

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

void Test_STRAT_homolo(void){
    static error_e sub_action;

    static enum{
        SORTIE = 0,
        VERRE_ALLER,
        VERRE_RETOUR,
        DONE,
    }state=SORTIE;

    switch(state){
        case SORTIE:
            sub_action = goto_pos_with_scan_foe((displacement_t[]){{{1000,COLOR_Y(300)}}},1,FORWARD,NO_DODGE_AND_NO_WAIT);
            switch(sub_action)
            {
                case END_OK:
                    state=VERRE_ALLER;
                    break;

                case END_WITH_TIMEOUT:
                    state=DONE;
                    break;
                case NOT_HANDLED:
                     state=DONE;
                    break;

                case IN_PROGRESS:
                    break;

                default:
                    break;
            }
            break;

        case VERRE_ALLER:
            sub_action=K_push_half_row_glasses_HOMOLO();
            switch(sub_action)
            {
                case END_OK:
                    state=VERRE_RETOUR;
                    break;

                case END_WITH_TIMEOUT:
                    state=VERRE_RETOUR;
                    break;
                case NOT_HANDLED:
                    state = VERRE_RETOUR;
                    break;

                case IN_PROGRESS:
                    break;

                default:
                    break;
            }
            break;
        case VERRE_RETOUR:
            sub_action=K_push_back_row_glasses_HOMOLO();
            switch(sub_action)
            {
                case END_OK:
                    state= DONE;
                    break;

                case END_WITH_TIMEOUT:
                    state= DONE;
                    break;
                case NOT_HANDLED:
                    state = DONE;
                    break;

                case IN_PROGRESS:
                    break;

                default:
                    break;
            }
            break;
		case DONE:
		default:
			break;
    }
}

void Test_STRAT_COUPE(void){
    static error_e sub_action;

    static enum{
        SORTIE = 0,
        VERRE_ALLER,
        VERRE_RETOUR,
        ASSIETTE_GATEAU_BLEUE,
        ASSIETTE_GATEAU_BLANC,
        ASSIETTE_MILIEU,
        ASSIETTE_CADEAU_BLANC,
        ASSIETTE_CADEAU_BLEUE,
        MOVE_FINAL,
        CADEAU,
        DONE,
    }state=SORTIE;

    switch(state){
        case SORTIE:
            sub_action = goto_pos_with_scan_foe((displacement_t[]){{{1000,COLOR_Y(500)}}},1,FORWARD,NO_AVOIDANCE);
            switch(sub_action)
            {
                case END_OK:
                    state=ASSIETTE_CADEAU_BLANC;
                    break;

                case END_WITH_TIMEOUT:
                    state=ASSIETTE_CADEAU_BLANC;
                    break;
                case NOT_HANDLED:
                     state=ASSIETTE_CADEAU_BLANC;
                    break;

                case IN_PROGRESS:
                    break;

                default:
                    break;
            }
            break;

        case VERRE_ALLER:
            sub_action=K_push_half_row_glasses();
            switch(sub_action)
            {
                case END_OK:
                    state=VERRE_RETOUR;
                    break;

                case END_WITH_TIMEOUT:
                    state=VERRE_RETOUR;
                    break;
                case NOT_HANDLED:
                    state = VERRE_RETOUR;
                    break;

                case IN_PROGRESS:
                    break;

                default:
                    state = VERRE_RETOUR;
                    break;
            }
            break;
        case VERRE_RETOUR:
            sub_action=K_push_back_row_glasses();
            switch(sub_action)
            {
                case END_OK:
                    state= ASSIETTE_CADEAU_BLANC;
                    break;

                case END_WITH_TIMEOUT:
                    state= ASSIETTE_CADEAU_BLANC;
                    break;
                case NOT_HANDLED:
                    state = ASSIETTE_CADEAU_BLANC;
                    break;

                case IN_PROGRESS:
                    break;

                default:
                    state = ASSIETTE_CADEAU_BLANC;
                    break;
            }
            break;

        case ASSIETTE_CADEAU_BLEUE:
            sub_action=Assiete1_lanceur();
            switch(sub_action)
            {
                case END_OK:
                    state=ASSIETTE_GATEAU_BLANC;
                    break;

                case END_WITH_TIMEOUT:
                    state=ASSIETTE_GATEAU_BLANC;
                    break;
                case NOT_HANDLED:
                    state=ASSIETTE_GATEAU_BLANC;
                    break;

                case IN_PROGRESS:
                    break;

                default:
                    break;
            }
            break;

        case ASSIETTE_CADEAU_BLANC:
            sub_action=Assiete2_lanceur();
            switch(sub_action)
            {
                case END_OK:
                    state=ASSIETTE_GATEAU_BLANC;
                    break;

                case END_WITH_TIMEOUT:
                    state=ASSIETTE_GATEAU_BLANC;
                    break;
                case NOT_HANDLED:
                    state=ASSIETTE_GATEAU_BLANC;
                    break;

                case IN_PROGRESS:
                    break;

                default:
                    break;
            }
            break;

        case ASSIETTE_MILIEU:
            sub_action=Assiete3_lanceur();
            switch(sub_action)
            {
                case END_OK:
                    state=ASSIETTE_GATEAU_BLANC;
                    break;

                case END_WITH_TIMEOUT:
                    state=ASSIETTE_GATEAU_BLANC;
                    break;
                case NOT_HANDLED:
                    state=ASSIETTE_GATEAU_BLANC;
                    break;

                case IN_PROGRESS:
                    break;

                default:
                    break;
            }
            break;

        case ASSIETTE_GATEAU_BLANC:
            sub_action=Assiete4_lanceur();
            switch(sub_action)
            {
                case END_OK:
                    state=ASSIETTE_GATEAU_BLEUE;
                    break;

                case END_WITH_TIMEOUT:
                    state=ASSIETTE_GATEAU_BLEUE;
                    break;
                case NOT_HANDLED:
                     state=ASSIETTE_GATEAU_BLEUE;
                    break;

                case IN_PROGRESS:
                    break;

                default:
                    break;
            }
            break;

        case ASSIETTE_GATEAU_BLEUE:
            sub_action=Assiete_5_lanceur();
            switch(sub_action)
            {
                case END_OK:
                    state=VERRE_ALLER;
                    break;

                case END_WITH_TIMEOUT:
                    state=VERRE_ALLER;
                    break;
                case NOT_HANDLED:
                     state=VERRE_ALLER;
                    break;

                case IN_PROGRESS:
                    break;

                default:
                    break;
            }
            break;

        case CADEAU:
            sub_action=Cadeau();
            switch(sub_action)
            {
                case END_OK:
                    state=DONE;
                    break;

                case END_WITH_TIMEOUT:
                    state=CADEAU;
                    break;
                case NOT_HANDLED:
                    break;

                case IN_PROGRESS:
                    break;

                default:
                    break;
            }
            break;
        case MOVE_FINAL :
            sub_action = goto_pos_with_scan_foe((displacement_t[]){{{740, COLOR_Y(555)}}},1,FORWARD,NO_DODGE_AND_NO_WAIT);
            switch(sub_action)
            {
                case END_OK:
                    state=DONE;
                    break;

                case END_WITH_TIMEOUT:
                    state=DONE;
                    break;
                case NOT_HANDLED:
                     state=DONE;
                    break;

                case IN_PROGRESS:
                    break;

                default:
                    break;
            }
            break;
        case DONE:
            break;
    }
    //state=ASSIETTE_CADEAU_BLEUE;
}

void Test_STRAT_COUPE_2(void){
    static error_e sub_action;

    static enum{
        SORTIE = 0,
        VERRE_ALLER,
        VERRE_RETOUR,
        ASSIETTE_GATEAU_BLEUE,
        ASSIETTE_GATEAU_BLANC,
        ASSIETTE_MILIEU,
        ASSIETTE_CADEAU_BLANC,
        ASSIETTE_CADEAU_BLEUE,
        MOVE_FINAL,
        CADEAU,
        DONE,
    }state=VERRE_ALLER;

    switch(state){
        case SORTIE:
            sub_action = goto_pos_with_scan_foe((displacement_t[]){{{1000,COLOR_Y(500)}}},1,FORWARD,NO_AVOIDANCE);
            switch(sub_action)
            {
                case END_OK:
                    state=ASSIETTE_CADEAU_BLANC;
                    break;
                    
                case END_WITH_TIMEOUT:
                    state=ASSIETTE_CADEAU_BLANC;
                    break;
                case NOT_HANDLED:
                     state=ASSIETTE_CADEAU_BLANC;
                    break;

                case IN_PROGRESS:
                    break;

                default:
                    break;
            }
            break;

        case VERRE_ALLER:
            sub_action=K_push_half_row_glasses();
            switch(sub_action)
            {
                case END_OK:
                    debug_printf("VERRE_ALLEREND OK%d\n",__LINE__);
                    state=VERRE_RETOUR;
                    break;

                case END_WITH_TIMEOUT:
                    debug_printf("VERRE_ALLER END_WITH_TIMEOUT%d\n",__LINE__);
                    state=VERRE_RETOUR;
                    break;
                case NOT_HANDLED:
                    debug_printf("VERRE_ALLER NOT_HANDLED%d\n",__LINE__);
                    state = VERRE_RETOUR;
                    break;

                case IN_PROGRESS:
                    break;

                default:
                    debug_printf("VERRE_ALLER DEFAULT%d\n",__LINE__);
                    state = VERRE_RETOUR;
                    break;
            }
            break;
        case VERRE_RETOUR:
            sub_action=K_push_back_row_glasses();
            switch(sub_action)
            {
                case END_OK:
                    state= ASSIETTE_CADEAU_BLANC;
                    debug_printf("VERRE_RETOUR END OK%d\n",__LINE__);
                    break;

                case END_WITH_TIMEOUT:
                    debug_printf("VERRE_RETOUR END_WITH_TIMEOUT%d\n",__LINE__);
                    state= ASSIETTE_CADEAU_BLANC;
                    break;
                case NOT_HANDLED:
                     debug_printf("VERRE_RETOUR NOT_HANDLED%d\n",__LINE__);
                    state = ASSIETTE_CADEAU_BLANC;
                    break;

                case IN_PROGRESS:
                    break;

                default:
                    debug_printf("VERRE_RETOUR DEFAULT%d\n",__LINE__);
                    state = ASSIETTE_CADEAU_BLANC;
                    break;
            }
            break;

        case ASSIETTE_CADEAU_BLEUE:
            sub_action=Assiete1_lanceur();
            switch(sub_action)
            {
                case END_OK:
                    state=ASSIETTE_GATEAU_BLANC;
                    break;

                case END_WITH_TIMEOUT:
                    state=ASSIETTE_GATEAU_BLANC;
                    break;
                case NOT_HANDLED:
                    state=ASSIETTE_GATEAU_BLANC;
                    break;

                case IN_PROGRESS:
                    break;

                default:
                    break;
            }
            break;

        case ASSIETTE_CADEAU_BLANC:
            sub_action=Assiete2_lanceur();
            switch(sub_action)
            {
                case END_OK:
                    state=ASSIETTE_GATEAU_BLANC;
                    break;

                case END_WITH_TIMEOUT:
                    state=ASSIETTE_GATEAU_BLANC;
                    break;
                case NOT_HANDLED:
                    state=ASSIETTE_GATEAU_BLANC;
                    break;

                case IN_PROGRESS:
                    break;

                default:
                    break;
            }
            break;

        case ASSIETTE_MILIEU:
            sub_action=Assiete3_lanceur();
            switch(sub_action)
            {
                case END_OK:
                    state=ASSIETTE_GATEAU_BLANC;
                    break;

                case END_WITH_TIMEOUT:
                    state=ASSIETTE_GATEAU_BLANC;
                    break;
                case NOT_HANDLED:
                    state=ASSIETTE_GATEAU_BLANC;
                    break;

                case IN_PROGRESS:
                    break;

                default:
                    break;
            }
            break;

        case ASSIETTE_GATEAU_BLANC:
            sub_action=Assiete4_lanceur();
            switch(sub_action)
            {
                case END_OK:
                    state=ASSIETTE_GATEAU_BLEUE;
                    break;

                case END_WITH_TIMEOUT:
                    state=ASSIETTE_GATEAU_BLEUE;
                    break;
                case NOT_HANDLED:
                     state=ASSIETTE_GATEAU_BLEUE;
                    break;

                case IN_PROGRESS:
                    break;

                default:
                    break;
            }
            break;

        case ASSIETTE_GATEAU_BLEUE:
            sub_action=Assiete_5_lanceur();
            switch(sub_action)
            {
                case END_OK:
                    state=MOVE_FINAL;
                    break;

                case END_WITH_TIMEOUT:
                    state=MOVE_FINAL;
                    break;
                case NOT_HANDLED:
                     state=MOVE_FINAL;
                    break;

                case IN_PROGRESS:
                    break;

                default:
                    break;
            }
            break;

        case CADEAU:
            sub_action=Cadeau();
            switch(sub_action)
            {
                case END_OK:
                    state=DONE;
                    break;

                case END_WITH_TIMEOUT:
                    state=CADEAU;
                    break;
                case NOT_HANDLED:
                    break;

                case IN_PROGRESS:
                    break;

                default:
                    break;
            }
            break;
        case MOVE_FINAL :
            sub_action = goto_pos_with_scan_foe((displacement_t[]){{{740, COLOR_Y(555)}}},1,FORWARD,NO_DODGE_AND_NO_WAIT);
            switch(sub_action)
            {
                case END_OK:
                    state=DONE;
                    break;

                case END_WITH_TIMEOUT:
                    state=DONE;
                    break;
                case NOT_HANDLED:
                     state=DONE;
                    break;

                case IN_PROGRESS:
                    break;

                default:
                    break;
            }
            break;
        case DONE:
            break;
    }
    //state=ASSIETTE_CADEAU_BLANC;
}

error_e Assiete4_lanceur(void){
    static error_e sub_action;
    static enum{
        ASSIETTE=0,
        LANCEUR_1,
        LANCEUR_2,
        DONE,
    }state=ASSIETTE;

    switch (state)
    {
        case ASSIETTE:
            sub_action=TEST_STRAT_assiettes_evitement_4();
            switch(sub_action)
            {
                case END_OK:
                    state=LANCEUR_1;
                    break;

                case END_WITH_TIMEOUT:
                    state=DONE;
                    return END_WITH_TIMEOUT;
                    break;
                case NOT_HANDLED:
                    state=ASSIETTE;
                    return NOT_HANDLED;
                    break;

                case IN_PROGRESS:
                    return IN_PROGRESS;
                    break;

                default:
                    state=ASSIETTE;
                    return NOT_HANDLED;
                    break;
            }
            break;

        case LANCEUR_1:
            sub_action=TEST_Launcher_ball_gateau();
            switch(sub_action)
            {
                case END_OK:
                    state=DONE;
                    return END_OK;
                    break;

                case END_WITH_TIMEOUT:
                    state=DONE;
                    return END_WITH_TIMEOUT;
                    break;
                case NOT_HANDLED:
                    state=LANCEUR_2;
                    break;

                case IN_PROGRESS:
                    return IN_PROGRESS;
                    break;

                default:
                    state=ASSIETTE;
                    return NOT_HANDLED;
                    break;
            }
            break;
        case LANCEUR_2:
            sub_action=TEST_Launcher_ball_mid();
            switch(sub_action)
            {
                case END_OK:
                    state=DONE;
                    return END_OK;
                    break;

                case END_WITH_TIMEOUT:
                    state=DONE;
                    return END_WITH_TIMEOUT;
                    break;
                case NOT_HANDLED:
                    state=LANCEUR_1;
                    break;

                case IN_PROGRESS:
                    return IN_PROGRESS;
                    break;

                default:
                    state=ASSIETTE;
                    return NOT_HANDLED;
                    break;
            }

            break;
        case DONE:
            return END_OK;
            break;
    }
    return IN_PROGRESS;
}

error_e Assiete3_lanceur(void){
    static error_e sub_action;
    static enum{
        ASSIETTE=0,
        LANCEUR_1,
        LANCEUR_2,
        DONE,
    }state=ASSIETTE;

    switch (state)
    {
        case ASSIETTE:
            sub_action=TEST_STRAT_assiettes_evitement_3();
            switch(sub_action)
            {
                case END_OK:
                    state=LANCEUR_1;
                    break;

                case END_WITH_TIMEOUT:
                    state=DONE;
                    return END_WITH_TIMEOUT;
                    break;
                case NOT_HANDLED:
                    state=ASSIETTE;
                    return NOT_HANDLED;
                    break;

                case IN_PROGRESS:
                    return IN_PROGRESS;
                    break;

                default:
                    state=ASSIETTE;
                    return NOT_HANDLED;
                    break;
            }
            break;
        case LANCEUR_1:
            sub_action=TEST_Launcher_ball_mid();
            switch(sub_action)
            {
                case END_OK:
                    state=DONE;
                    break;

                case END_WITH_TIMEOUT:
                    state=DONE;
                    return END_WITH_TIMEOUT;
                    break;
                case NOT_HANDLED:
                    state=LANCEUR_2;
                    break;

                case IN_PROGRESS:
                    return IN_PROGRESS;
                    break;

                default:
                    state=ASSIETTE;
                    return NOT_HANDLED;
                    break;
            }
            break;
        case LANCEUR_2:
            sub_action=TEST_Launcher_ball_gateau();
            switch(sub_action)
            {
                case END_OK:
                    state=DONE;
                    break;

                case END_WITH_TIMEOUT:
                    state=DONE;
                    return END_WITH_TIMEOUT;
                    break;
                case NOT_HANDLED:
                    state=LANCEUR_1;
                    break;

                case IN_PROGRESS:
                    return IN_PROGRESS;
                    break;

                default:
                    state=ASSIETTE;
                    return NOT_HANDLED;
                    break;
            }

            break;
        case DONE:
            return END_OK;
            break;
    }
    return IN_PROGRESS;
}

error_e Assiete2_lanceur(void){
    static error_e sub_action;
    static enum{
        ASSIETTE=0,
        LANCEUR_1,
        LANCEUR_2,
        DONE,
    }state=ASSIETTE;

    switch (state)
    {
        case ASSIETTE:
            sub_action=TEST_STRAT_assiettes_evitement_2();
            switch(sub_action)
            {
                case END_OK:
                    state=LANCEUR_1;
                    break;

                case END_WITH_TIMEOUT:
                    state=DONE;
                    return END_WITH_TIMEOUT;
                    break;
                case NOT_HANDLED:
                    //state=ASSIETTE;
                    return NOT_HANDLED;
                    break;

                case IN_PROGRESS:
                    return IN_PROGRESS;
                    break;

                default:
                    state=ASSIETTE;
                    return NOT_HANDLED;
                    break;
            }
            break;
        case LANCEUR_1:
            sub_action=TEST_Launcher_ball_mid();
            switch(sub_action)
            {
                case END_OK:
                    state=DONE;
                    break;

                case END_WITH_TIMEOUT:
                    state=DONE;
                    return END_WITH_TIMEOUT;
                    break;
                case NOT_HANDLED:
                    state=LANCEUR_2;
                    break;

                case IN_PROGRESS:
                    return IN_PROGRESS;
                    break;

                default:
                    state=ASSIETTE;
                    return NOT_HANDLED;
                    break;
            }
            break;
        case LANCEUR_2:
            sub_action=TEST_Launcher_ball_cadeau();
            switch(sub_action)
            {
                case END_OK:
                    state=DONE;
                    break;

                case END_WITH_TIMEOUT:
                    state=DONE;
                    return END_WITH_TIMEOUT;
                    break;
                case NOT_HANDLED:
                    state=LANCEUR_1;
                    break;

                case IN_PROGRESS:
                    return IN_PROGRESS;
                    break;

                default:
                    state=ASSIETTE;
                    return NOT_HANDLED;
                    break;
            }

            break;
        case DONE:
            return END_OK;
            break;
    }
    return IN_PROGRESS;
}

error_e Assiete1_lanceur(void){
    static error_e sub_action;
    static enum{
        ASSIETTE=0,
        LANCEUR_1,
        LANCEUR_2,
        DONE,
    }state=ASSIETTE;

    switch (state)
    {
        case ASSIETTE:
            sub_action=TEST_STRAT_assiettes_evitement_1();
            switch(sub_action)
            {
                case END_OK:
                    state=LANCEUR_1;
                    break;

                case END_WITH_TIMEOUT:
                    state=DONE;
                    return END_WITH_TIMEOUT;
                    break;
                case NOT_HANDLED:
                    state=LANCEUR_1;
                    
                    break;

                case IN_PROGRESS:
                    return IN_PROGRESS;
                    break;

                default:
                    state=ASSIETTE;
                    return NOT_HANDLED;
                    break;
            }
            break;
        case LANCEUR_1:
            sub_action=TEST_Launcher_ball_cadeau();
            switch(sub_action)
            {
                case END_OK:
                    state=DONE;
                    break;

                case END_WITH_TIMEOUT:
                    state=DONE;
                    return END_WITH_TIMEOUT;
                    break;
                case NOT_HANDLED:
                    state=LANCEUR_2;
                    break;

                case IN_PROGRESS:
                    return IN_PROGRESS;
                    break;

                default:
                    state=ASSIETTE;
                    return NOT_HANDLED;
                    break;
            }
            break;
        case LANCEUR_2:
            sub_action=TEST_Launcher_ball_mid();
            switch(sub_action)
            {
                case END_OK:
                    state=DONE;
                    break;

                case END_WITH_TIMEOUT:
                    state=DONE;
                    return END_WITH_TIMEOUT;
                case NOT_HANDLED:
                    state=LANCEUR_1;
                    break;

                case IN_PROGRESS:
                    return IN_PROGRESS;
                    break;

                default:
                    state=ASSIETTE;
                    return NOT_HANDLED;
                    break;
            }

            break;
        case DONE:
            return END_OK;
            break;
    }
    return IN_PROGRESS;
}


error_e Cadeau(void){
    static error_e sub_action;
    static enum{
        CADEAU_1=0,
                CADEAU_2,
                CADEAU_3,
                CADEAU_4,
                DONE,
    }state=CADEAU_1;
    switch(state)
    {
        case CADEAU_1:
            sub_action=K_CADEAU1();
            switch(sub_action)
            {
                case END_OK:
                    state=CADEAU_2;
                    break;

                case END_WITH_TIMEOUT:
                    state=DONE;
                    return END_WITH_TIMEOUT;
                case NOT_HANDLED:
                    state=CADEAU_2;
                    break;

                case IN_PROGRESS:
                    return IN_PROGRESS;
                    break;

                default:
                    state=CADEAU_1;
                    return NOT_HANDLED;
                    break;
            }
            break;

        case CADEAU_2:
            sub_action=K_CADEAU2();
            switch(sub_action)
            {
                case END_OK:
                    state=CADEAU_3;
                    break;

                case END_WITH_TIMEOUT:
                    state=DONE;
                    return END_WITH_TIMEOUT;
                case NOT_HANDLED:
                    state=CADEAU_3;
                    break;

                case IN_PROGRESS:
                    return IN_PROGRESS;
                    break;

                default:
                    state=CADEAU_1;
                    return NOT_HANDLED;
                    break;
            }
            break;

        case CADEAU_3:
            sub_action=K_CADEAU3();
            switch(sub_action)
            {
                case END_OK:
                    state=CADEAU_4;
                    break;

                case END_WITH_TIMEOUT:
                    state=DONE;
                    return END_WITH_TIMEOUT;
                case NOT_HANDLED:
                    state=CADEAU_4;
                    break;

                case IN_PROGRESS:
                    return IN_PROGRESS;
                    break;

                default:
                    state=CADEAU_1;
                    return NOT_HANDLED;
                    break;
            }
            break;

        case CADEAU_4:
            sub_action=K_CADEAU4();
            switch(sub_action)
            {
                case END_OK:
                    state=DONE;
                    break;

                case END_WITH_TIMEOUT:
                    state=DONE;
                    return END_WITH_TIMEOUT;
                case NOT_HANDLED:
                    state=CADEAU_1;
                    break;

                case IN_PROGRESS:
                    return IN_PROGRESS;
                    break;

                default:
                    state=CADEAU_1;
                    return NOT_HANDLED;
                    break;
            }
            break;

        case DONE:
            return END_OK;
            break;
        default:
            state=CADEAU_1;
            return NOT_HANDLED;
            break;


    }
    return IN_PROGRESS;
}
