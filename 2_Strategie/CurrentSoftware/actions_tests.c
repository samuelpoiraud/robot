/*
 *	Club Robot ESEO 2008 - 2012
 *	Archi'Tech, CHOMP, Shark & Fish, Krusty & Tiny
 *
 *	Fichier : actions_tests.c
 *	Package : Carte Principale
 *	Description : Tests des actions réalisables par le robot
 *	Auteur : Cyril, modifié par Vincent , Antoine , LMG , Herzaeone , Hoobbes
 *	Version 2012/01/14
 */

#define ACTIONS_TEST_C

#include "actions_tests.h"


#define DEFAULT_SPEED	(SLOW)

#define USE_CURVE	0



/* ----------------------------------------------------------------------------- */
/* 							Fonctions d'homologation			                 */
/* ----------------------------------------------------------------------------- */


#ifdef MODE_HOMOLOGATION




/* ----------------------------------------------------------------------------- */
/* 					Sous Strat homologation                     			 */
/* ----------------------------------------------------------------------------- */



#endif
/* ----------------------------------------------------------------------------- */
/* 								Stratégies de test                     			 */
/* ----------------------------------------------------------------------------- */

void TEST_STRAT_avoidance(void){
	static enum{
		SORTIR,
				DEPLACEMENT1,
				DEPLACEMENT2,
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
					break;
				case NOT_HANDLED:
					state = DEPLACEMENT2;
					break;
				case END_OK:
					state = DEPLACEMENT2;
					break;
				case END_WITH_TIMEOUT:
					state = DEPLACEMENT2;
					break;
				default:
					state = DEPLACEMENT2;
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


/* ----------------------------------------------------------------------------- */
/* 						Actions élémentaires de construction                     */
/* ----------------------------------------------------------------------------- */

error_e K_CADEAU1(void){
	static enum{
		MOVE = 0,
				ANGLE,
				OPEN_ACT,
				PUSH,
				EXTRACT,
				DONE,
	}state = MOVE;

	static error_e sub_action;
	switch(state){
		case MOVE:
			sub_action = goto_pos_with_scan_foe((displacement_t[]){{{400,COLOR_Y(470)}}},1,ANY_WAY,NO_DODGE_AND_NO_WAIT);
			switch(sub_action)
            {
                case END_OK:
                    state = ANGLE;
					return IN_PROGRESS;
                    break;

                case END_WITH_TIMEOUT:
					state = DONE;
					return END_WITH_TIMEOUT;
                    break;

                case NOT_HANDLED:
					state = MOVE;
					return NOT_HANDLED;
                    break;

                case IN_PROGRESS:
					return IN_PROGRESS;
                    break;

                default:
					state = MOVE;
					return NOT_HANDLED;
                    break;
            }
			break;

		case ANGLE:
			sub_action = goto_angle(0, FAST);
			switch(sub_action)
            {
                case END_OK:
                    state = OPEN_ACT;
					return IN_PROGRESS;
                    break;

                case END_WITH_TIMEOUT:
					state = DONE;
					return END_WITH_TIMEOUT;
                    break;

                case NOT_HANDLED:
					state = MOVE;
					return NOT_HANDLED;
                    break;

                case IN_PROGRESS:
					return IN_PROGRESS;
                    break;

                default:
					state = MOVE;
					return NOT_HANDLED;
                    break;
            }
			break;

		case OPEN_ACT:
			state = PUSH;
			ACT_push_plate_rotate_prepare(TRUE);
			return IN_PROGRESS;
			break;

		case PUSH:
			sub_action = goto_pos_with_scan_foe((displacement_t[]){{{((global.env.color == BLUE) ? 220 : 250),COLOR_Y(470)}}},1,ANY_WAY,NO_DODGE_AND_NO_WAIT);
			
			switch(sub_action)
            {
                case END_OK:
                    state = EXTRACT;
					ACT_push_plate_rotate_vertically(TRUE);
					return IN_PROGRESS;
                    break;

                case END_WITH_TIMEOUT:
					state = DONE;
					ACT_push_plate_rotate_vertically(TRUE);
					return END_WITH_TIMEOUT;
                    break;

                case NOT_HANDLED:
					state = MOVE;
					ACT_push_plate_rotate_vertically(TRUE);
					return NOT_HANDLED;
                    break;

                case IN_PROGRESS:
					return IN_PROGRESS;
                    break;

                default:
					state = MOVE;
					ACT_push_plate_rotate_vertically(TRUE);
					return NOT_HANDLED;
                    break;
            }
			break;

		case EXTRACT:
			sub_action = goto_pos_with_scan_foe((displacement_t[]){{{600,COLOR_Y(470)}}},1,ANY_WAY,NO_DODGE_AND_NO_WAIT);
			switch(sub_action)
            {
                case END_OK:
                    state = DONE;
					return IN_PROGRESS;
                    break;

                case END_WITH_TIMEOUT:
					state = DONE;
					return END_WITH_TIMEOUT;
                    break;

                case NOT_HANDLED:
					state = DONE;
					return NOT_HANDLED;
                    break;

                case IN_PROGRESS:
					return IN_PROGRESS;
                    break;

                default:
					state = DONE;
					return NOT_HANDLED;
                    break;
            }
			break;
		case DONE:
			return END_OK;
	}
	return NOT_HANDLED;
}

error_e K_CADEAU2(void){
	static enum{
		MOVE = 0,
				ANGLE,
				OPEN_ACT,
				PUSH,
				EXTRACT,
				DONE,
	}state = MOVE;

	static error_e sub_action;
	switch(state){
		case MOVE:
			sub_action = goto_pos_with_scan_foe((displacement_t[]){{{400,COLOR_Y(1070)}}},1,ANY_WAY,NO_DODGE_AND_NO_WAIT);
			switch(sub_action)
            {
                case END_OK:
                    state = ANGLE;
					return IN_PROGRESS;
                    break;

                case END_WITH_TIMEOUT:
					state = DONE;
					return END_WITH_TIMEOUT;
                    break;

                case NOT_HANDLED:
					state = MOVE;
					return NOT_HANDLED;
                    break;

                case IN_PROGRESS:
					return IN_PROGRESS;
                    break;

                default:
					state = MOVE;
					return NOT_HANDLED;
                    break;
            }
			break;

		case ANGLE:
			sub_action = goto_angle(0, FAST);
			switch(sub_action)
            {
                case END_OK:
                    state = OPEN_ACT;
					return IN_PROGRESS;
                    break;

                case END_WITH_TIMEOUT:
					state = DONE;
					return END_WITH_TIMEOUT;
                    break;

                case NOT_HANDLED:
					state = MOVE;
					return NOT_HANDLED;
                    break;

                case IN_PROGRESS:
					return IN_PROGRESS;
                    break;

                default:
					state = MOVE;
					return NOT_HANDLED;
                    break;
            }
			break;

		case OPEN_ACT:
			state = PUSH;
			ACT_push_plate_rotate_prepare(TRUE);
			return IN_PROGRESS;
			break;

		case PUSH:
			sub_action = goto_pos_with_scan_foe((displacement_t[]){{{((global.env.color == BLUE) ? 220 : 250),COLOR_Y(1070)}}},1,ANY_WAY,NO_DODGE_AND_NO_WAIT);

			switch(sub_action)
            {
                case END_OK:
                    state = EXTRACT;
					ACT_push_plate_rotate_vertically(TRUE);
					return IN_PROGRESS;
                    break;

                case END_WITH_TIMEOUT:
					state = DONE;
					ACT_push_plate_rotate_vertically(TRUE);
					return END_WITH_TIMEOUT;
                    break;

                case NOT_HANDLED:
					state = MOVE;
					ACT_push_plate_rotate_vertically(TRUE);
					return NOT_HANDLED;
                    break;

                case IN_PROGRESS:
					return IN_PROGRESS;
                    break;

                default:
					state = MOVE;
					ACT_push_plate_rotate_vertically(TRUE);
					return NOT_HANDLED;
                    break;
            }
			break;

		case EXTRACT:
			sub_action = goto_pos_with_scan_foe((displacement_t[]){{{600,COLOR_Y(1070)}}},1,ANY_WAY,NO_DODGE_AND_NO_WAIT);
			switch(sub_action)
            {
                case END_OK:
                    state = DONE;
					return IN_PROGRESS;
                    break;

                case END_WITH_TIMEOUT:
					state = DONE;
					return END_WITH_TIMEOUT;
                    break;

                case NOT_HANDLED:
					state = DONE;
					return NOT_HANDLED;
                    break;

                case IN_PROGRESS:
					return IN_PROGRESS;
                    break;

                default:
					state = DONE;
					return NOT_HANDLED;
                    break;
            }
			break;
		case DONE:
			return END_OK;
	}
	return NOT_HANDLED;
}

error_e K_CADEAU3(void){
	static enum{
		MOVE = 0,
				ANGLE,
				OPEN_ACT,
				PUSH,
				EXTRACT,
				DONE,
	}state = MOVE;

	static error_e sub_action;
	switch(state){
		case MOVE:
			sub_action = goto_pos_with_scan_foe((displacement_t[]){{{400,COLOR_Y(1670)}}},1,ANY_WAY,NO_DODGE_AND_NO_WAIT);
			switch(sub_action)
            {
                case END_OK:
                    state = ANGLE;
					return IN_PROGRESS;
                    break;

                case END_WITH_TIMEOUT:
					state = DONE;
					return END_WITH_TIMEOUT;
                    break;

                case NOT_HANDLED:
					state = MOVE;
					return NOT_HANDLED;
                    break;

                case IN_PROGRESS:
					return IN_PROGRESS;
                    break;

                default:
					state = MOVE;
					return NOT_HANDLED;
                    break;
            }
			break;

		case ANGLE:
			sub_action = goto_angle(0, FAST);
			switch(sub_action)
            {
                case END_OK:
                    state = OPEN_ACT;
					return IN_PROGRESS;
                    break;

                case END_WITH_TIMEOUT:
					state = DONE;
					return END_WITH_TIMEOUT;
                    break;

                case NOT_HANDLED:
					state = MOVE;
					return NOT_HANDLED;
                    break;

                case IN_PROGRESS:
					return IN_PROGRESS;
                    break;

                default:
					state = MOVE;
					return NOT_HANDLED;
                    break;
            }
			break;

		case OPEN_ACT:
			state = PUSH;
			ACT_push_plate_rotate_prepare(TRUE);
			return IN_PROGRESS;
			break;

		case PUSH:
			sub_action = goto_pos_with_scan_foe((displacement_t[]){{{((global.env.color == BLUE) ? 220 : 250),COLOR_Y(1670)}}},1,ANY_WAY,NO_DODGE_AND_NO_WAIT);

			switch(sub_action)
            {
                case END_OK:
                    state = EXTRACT;
					ACT_push_plate_rotate_vertically(TRUE);
					return IN_PROGRESS;
                    break;

                case END_WITH_TIMEOUT:
					state = DONE;
					ACT_push_plate_rotate_vertically(TRUE);
					return END_WITH_TIMEOUT;
                    break;

                case NOT_HANDLED:
					state = MOVE;
					ACT_push_plate_rotate_vertically(TRUE);
					return NOT_HANDLED;
                    break;

                case IN_PROGRESS:
					return IN_PROGRESS;
                    break;

                default:
					state = MOVE;
					ACT_push_plate_rotate_vertically(TRUE);
					return NOT_HANDLED;
                    break;
            }
			break;

		case EXTRACT:
			sub_action = goto_pos_with_scan_foe((displacement_t[]){{{600,COLOR_Y(1670)}}},1,ANY_WAY,NO_DODGE_AND_NO_WAIT);
			switch(sub_action)
            {
                case END_OK:
                    state = DONE;
					return IN_PROGRESS;
                    break;

                case END_WITH_TIMEOUT:
					state = DONE;
					return END_WITH_TIMEOUT;
                    break;

                case NOT_HANDLED:
					state = DONE;
					return NOT_HANDLED;
                    break;

                case IN_PROGRESS:
					return IN_PROGRESS;
                    break;

                default:
					state = DONE;
					return NOT_HANDLED;
                    break;
            }
			break;
		case DONE:
			return END_OK;
	}
	return NOT_HANDLED;
}

error_e K_CADEAU4(void){
	static enum{
		MOVE = 0,
				ANGLE,
				OPEN_ACT,
				PUSH,
				EXTRACT,
				DONE,
	}state = MOVE;

	static error_e sub_action;
	switch(state){
		case MOVE:
			sub_action = goto_pos_with_scan_foe((displacement_t[]){{{400,COLOR_Y(2270)}}},1,ANY_WAY,NO_DODGE_AND_NO_WAIT);
			switch(sub_action)
            {
                case END_OK:
                    state = ANGLE;
					return IN_PROGRESS;
                    break;

                case END_WITH_TIMEOUT:
					state = DONE;
					return END_WITH_TIMEOUT;
                    break;

                case NOT_HANDLED:
					state = MOVE;
					return NOT_HANDLED;
                    break;

                case IN_PROGRESS:
					return IN_PROGRESS;
                    break;

                default:
					state = MOVE;
					return NOT_HANDLED;
                    break;
            }
			break;

		case ANGLE:
			sub_action = goto_angle(0, FAST);
			switch(sub_action)
            {
                case END_OK:
                    state = OPEN_ACT;
					return IN_PROGRESS;
                    break;

                case END_WITH_TIMEOUT:
					state = DONE;
					return END_WITH_TIMEOUT;
                    break;

                case NOT_HANDLED:
					state = MOVE;
					return NOT_HANDLED;
                    break;

                case IN_PROGRESS:
					return IN_PROGRESS;
                    break;

                default:
					state = MOVE;
					return NOT_HANDLED;
                    break;
            }
			break;

		case OPEN_ACT:
			state = PUSH;
			ACT_push_plate_rotate_prepare(TRUE);
			return IN_PROGRESS;
			break;

		case PUSH:
			sub_action = goto_pos_with_scan_foe((displacement_t[]){{{((global.env.color == BLUE) ? 220 : 250),COLOR_Y(2270)}}},1,ANY_WAY,NO_DODGE_AND_NO_WAIT);
			
			switch(sub_action)
            {
                case END_OK:
                    state = EXTRACT;
					ACT_push_plate_rotate_vertically(TRUE);
					return IN_PROGRESS;
                    break;

                case END_WITH_TIMEOUT:
					state = DONE;
					ACT_push_plate_rotate_vertically(TRUE);
					return END_WITH_TIMEOUT;
                    break;

                case NOT_HANDLED:
					state = MOVE;
					ACT_push_plate_rotate_vertically(TRUE);
					return NOT_HANDLED;
                    break;

                case IN_PROGRESS:
					return IN_PROGRESS;
                    break;

                default:
					state = MOVE;
					ACT_push_plate_rotate_vertically(TRUE);
					return NOT_HANDLED;
                    break;
            }
			break;

		case EXTRACT:
			sub_action = goto_pos_with_scan_foe((displacement_t[]){{{600,COLOR_Y(2270)}}},1,ANY_WAY,NO_DODGE_AND_NO_WAIT);
			switch(sub_action)
            {
                case END_OK:
                    state = DONE;
					return IN_PROGRESS;
                    break;

                case END_WITH_TIMEOUT:
					state = DONE;
					return END_WITH_TIMEOUT;
                    break;

                case NOT_HANDLED:
					state = DONE;
					return NOT_HANDLED;
                    break;

                case IN_PROGRESS:
					return IN_PROGRESS;
                    break;

                default:
					state = DONE;
					return NOT_HANDLED;
                    break;
            }
			break;
		case DONE:
			return END_OK;
	}
	return NOT_HANDLED;
}

/* ----------------------------------------------------------------------------- */
/* 								Fonction diverses                     			 */
/* ----------------------------------------------------------------------------- */


/* ----------------------------------------------------------------------------- */
/* 							Découpe de stratégies                     			 */
/* ----------------------------------------------------------------------------- */



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
            sub_action_act = ACT_get_last_action_result2(ACT_QUEUE_Plate);
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
            sub_action_act = ACT_get_last_action_result2(ACT_QUEUE_Plate);
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
            sub_action_act = ACT_get_last_action_result2(ACT_QUEUE_Plate);
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
            sub_action_act = ACT_get_last_action_result2(ACT_QUEUE_Plate);
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

