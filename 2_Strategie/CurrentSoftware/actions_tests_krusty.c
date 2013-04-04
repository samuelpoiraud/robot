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

#define ACTIONS_TEST_KRUSTY_C

#include "actions_tests_krusty.h"


#define DEFAULT_SPEED	(SLOW)

#define USE_CURVE	0



/* ----------------------------------------------------------------------------- */
/* 							Fonctions d'homologation			                 */
/* ----------------------------------------------------------------------------- */


#ifdef MODE_HOMOLOGATION





#endif
/* ----------------------------------------------------------------------------- */
/* 								Stratégies de test                     			 */
/* ----------------------------------------------------------------------------- */

void TEST_STRAT_avoidance(void){
	static enum{
		SORTIR,
				DEPLACEMENT1,
				DEPLACEMENT2,
				DONE,
	}state = SORTIR;

	static error_e sub_action;

	switch(state){
		case SORTIR:
			sub_action = goto_pos(580,COLOR_Y(380),FAST,FORWARD);
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
					{{500, COLOR_Y(2500)},SLOW}},2,ANY_WAY,NO_DODGE_AND_NO_WAIT);
			switch(sub_action){
				case IN_PROGRESS:
                                    debug_printf("INPROGRESS");
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
			sub_action = goto_pos_with_scan_foe((displacement_t[]){{{1000, COLOR_Y(700)},SLOW},
					{{1000, COLOR_Y(2500)},SLOW}},2,ANY_WAY,NO_DODGE_AND_NO_WAIT);
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
			sub_action = goto_pos(580,COLOR_Y(380),FAST,FORWARD);
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
			sub_action = goto_pos(580,COLOR_Y(380),FAST,FORWARD);
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
			sub_action = goto_pos(580,COLOR_Y(380),FAST,FORWARD);
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
                case NOT_HANDLED:
                    state=LANCEUR_MID;
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
                case NOT_HANDLED:
                    state=DONE;
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
		DONE
	}state = GO;

	static error_e sub_action;

	switch(state){
		case SORTIR:
			sub_action = goto_pos(1000,COLOR_Y(350),FAST,FORWARD);
			switch(sub_action){
				case IN_PROGRESS:
					break;
				case END_OK:
					state = GO;
					break;
				case END_WITH_TIMEOUT:
					state = GO;
					break;
				case NOT_HANDLED:
					state = GO;
					break;
			}
		case GO:
			sub_action = K_verres();
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

            sub_action = goto_pos_with_scan_foe((displacement_t[]){{{600,COLOR_Y(800)}}},1,FORWARD,NORMAL_WAIT);
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

            sub_action = goto_pos_with_scan_foe((displacement_t[]){{{1000,COLOR_Y(600)}}},1,REAR,NORMAL_WAIT);
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

            sub_action = goto_pos_with_scan_foe((displacement_t[]){{{1000,COLOR_Y(400)}}},1,REAR,NORMAL_WAIT);
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

void Test_STRAT_COUPE(void){
    static error_e sub_action;

    static enum{
        SORTIE = 0,
        VERRE_ALLER,
        ASSIETTE_GATEAU_BLEUE,
        ASSIETTE_GATEAU_BLANC,
        ASSIETTE_MILIEU,
        ASSIETTE_BLANC_CADEAU,
        CADEAU,
        DONE,
    }state=SORTIE;

    switch(state){
        case SORTIE:
            sub_action = goto_pos_with_scan_foe((displacement_t[]){{{600,COLOR_Y(600)}}},1,REAR,NORMAL_WAIT);
            switch(sub_action)
            {
                case END_OK:
                    state=VERRE_ALLER;

                    break;

                case END_WITH_TIMEOUT:
                    state=VERRE_ALLER;
                    break;
                case NOT_HANDLED:
                    break;

                case IN_PROGRESS:
                    break;

                default:
                    break;
            }
            break;
            
        case VERRE_ALLER:
            break;

        case ASSIETTE_GATEAU_BLEUE:
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
                    break;

                case IN_PROGRESS:
                    break;

                default:
                    break;
            }
            break;

        case ASSIETTE_GATEAU_BLANC:
            sub_action=Assiete2_lanceur();
            switch(sub_action)
            {
                case END_OK:
                    state=ASSIETTE_MILIEU;
                    break;

                case END_WITH_TIMEOUT:
                    state=ASSIETTE_MILIEU;
                    break;
                case NOT_HANDLED:
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
                    state=ASSIETTE_BLANC_CADEAU;
                    break;

                case END_WITH_TIMEOUT:
                    state=ASSIETTE_BLANC_CADEAU;
                    break;
                case NOT_HANDLED:
                    break;

                case IN_PROGRESS:
                    break;

                default:
                    break;
            }
            break;

        case ASSIETTE_BLANC_CADEAU:
            sub_action=Assiete4_lanceur();
            switch(sub_action)
            {
                case END_OK:
                    state=CADEAU;
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

        case DONE:
            break;
    }
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
            //sub_action=TA_MAMAN
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
            //sub_action=TA_MAMAN
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
            //sub_action=TA_MAMAN
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
            break;
    }
    return IN_PROGRESS;
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
            //sub_action=TA_MAMAN
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
            break;
        default:
            state=CADEAU_1;
            return NOT_HANDLED;
            break;


    }
    return IN_PROGRESS;
}
