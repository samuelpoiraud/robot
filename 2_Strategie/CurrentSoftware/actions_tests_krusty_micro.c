/*
 *	Club Robot ESEO 2008 - 2012
 *	Archi'Tech, CHOMP, Shark & Fish, Krusty & Tiny
 *
 *	Fichier : actions_tests_krusty_micro.c
 *	Package : Carte Principale
 *	Description : Tests des actions réalisables par le robot
 *	Auteur : Cyril, modifié par Vincent , Antoine , LMG , Herzaeone , Hoobbes
 *	Version 2012/01/14
 */

#define ACTIONS_TEST_KRUSTY_MICRO_C

#include "actions_tests_krusty_micro.h"


#define DEFAULT_SPEED	(SLOW)

#define USE_CURVE	0





#ifdef MODE_HOMOLOGATION




/* ----------------------------------------------------------------------------- */
/* 					Sous Strat homologation                     			 */
/* ----------------------------------------------------------------------------- */



#endif

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
			ACT_plate_rotate(ACT_PLATE_RotateMid);
			return IN_PROGRESS;
			break;

		case PUSH:
			sub_action = goto_pos_with_scan_foe((displacement_t[]){{{((global.env.color == BLUE) ? 220 : 250),COLOR_Y(470)}}},1,ANY_WAY,NO_DODGE_AND_NO_WAIT);

			switch(sub_action)
            {
                case END_OK:
                    state = EXTRACT;
					ACT_plate_rotate(ACT_PLATE_RotateUp);
					return IN_PROGRESS;
                    break;

                case END_WITH_TIMEOUT:
					state = DONE;
					ACT_plate_rotate(ACT_PLATE_RotateUp);
					return END_WITH_TIMEOUT;
                    break;

                case NOT_HANDLED:
					state = MOVE;
					ACT_plate_rotate(ACT_PLATE_RotateUp);
					return NOT_HANDLED;
                    break;

                case IN_PROGRESS:
					return IN_PROGRESS;
                    break;

                default:
					state = MOVE;
					ACT_plate_rotate(ACT_PLATE_RotateUp);
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
			ACT_plate_rotate(ACT_PLATE_RotateMid);
			return IN_PROGRESS;
			break;

		case PUSH:
			sub_action = goto_pos_with_scan_foe((displacement_t[]){{{((global.env.color == BLUE) ? 220 : 250),COLOR_Y(1070)}}},1,ANY_WAY,NO_DODGE_AND_NO_WAIT);

			switch(sub_action)
            {
                case END_OK:
                    state = EXTRACT;
					ACT_plate_rotate(ACT_PLATE_RotateUp);
					return IN_PROGRESS;
                    break;

                case END_WITH_TIMEOUT:
					state = DONE;
					ACT_plate_rotate(ACT_PLATE_RotateUp);
					return END_WITH_TIMEOUT;
                    break;

                case NOT_HANDLED:
					state = MOVE;
					ACT_plate_rotate(ACT_PLATE_RotateUp);
					return NOT_HANDLED;
                    break;

                case IN_PROGRESS:
					return IN_PROGRESS;
                    break;

                default:
					state = MOVE;
					ACT_plate_rotate(ACT_PLATE_RotateUp);
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
			ACT_plate_rotate(ACT_PLATE_RotateMid);
			return IN_PROGRESS;
			break;

		case PUSH:
			sub_action = goto_pos_with_scan_foe((displacement_t[]){{{((global.env.color == BLUE) ? 220 : 250),COLOR_Y(1670)}}},1,ANY_WAY,NO_DODGE_AND_NO_WAIT);

			switch(sub_action)
            {
                case END_OK:
                    state = EXTRACT;
					ACT_plate_rotate(ACT_PLATE_RotateUp);
					return IN_PROGRESS;
                    break;

                case END_WITH_TIMEOUT:
					state = DONE;
					ACT_plate_rotate(ACT_PLATE_RotateUp);
					return END_WITH_TIMEOUT;
                    break;

                case NOT_HANDLED:
					state = MOVE;
					ACT_plate_rotate(ACT_PLATE_RotateUp);
					return NOT_HANDLED;
                    break;

                case IN_PROGRESS:
					return IN_PROGRESS;
                    break;

                default:
					state = MOVE;
					ACT_plate_rotate(ACT_PLATE_RotateUp);
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
			ACT_plate_rotate(ACT_PLATE_RotateMid);
			return IN_PROGRESS;
			break;

		case PUSH:
			sub_action = goto_pos_with_scan_foe((displacement_t[]){{{((global.env.color == BLUE) ? 220 : 250),COLOR_Y(2270)}}},1,ANY_WAY,NO_DODGE_AND_NO_WAIT);

			switch(sub_action)
            {
                case END_OK:
                    state = EXTRACT;
					ACT_plate_rotate(ACT_PLATE_RotateUp);
					return IN_PROGRESS;
                    break;

                case END_WITH_TIMEOUT:
					state = DONE;
					ACT_plate_rotate(ACT_PLATE_RotateUp);
					return END_WITH_TIMEOUT;
                    break;

                case NOT_HANDLED:
					state = MOVE;
					ACT_plate_rotate(ACT_PLATE_RotateUp);
					return NOT_HANDLED;
                    break;

                case IN_PROGRESS:
					return IN_PROGRESS;
                    break;

                default:
					state = MOVE;
					ACT_plate_rotate(ACT_PLATE_RotateUp);
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


error_e K_ASSIETE1(void){
	static enum{
		MOVE = 0,
				ANGLE,
				GRABER_DOWN,
				WAIT_GRABER_DOWN,
				OPEN_GRABER,
				WAIT_OPEN_GRABER,
				PUSH,
				GRAB,
				WAIT_GRAB,
				EAT_BALLS, //Elles sont bonnes hein???!!!
				WAIT_EAT_BALLS, //Faut bien attendre elles sont grosses quand meme
				DONE,
	}state = MOVE;

	static error_e sub_action;

	switch(state){
		case MOVE:
			sub_action = goto_pos_with_scan_foe((displacement_t[]){{{1000,COLOR_Y(800)}}}, 1, REAR, NO_DODGE_AND_NO_WAIT);
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

			case ANGLE:
			sub_action = goto_angle(PI4096/2, FAST);
			switch(sub_action)
            {
                case END_OK:
                    state = GRABER_DOWN;
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
			
		case GRABER_DOWN:
			state = WAIT_GRABER_DOWN;
			ACT_plate_rotate(ACT_PLATE_RotateDown);
			return IN_PROGRESS;
			break;
			
		case WAIT_GRABER_DOWN:
			sub_action = ACT_get_last_action_result(ACT_QUEUE_Plate);
			
			switch(sub_action)
            {
                case ACT_FUNCTION_Done:
                    state = OPEN_GRABER;
					return IN_PROGRESS;
                    break;

                case ACT_FUNCTION_ActDisabled:
					state = DONE;
					return END_WITH_TIMEOUT;
                    break;

                case ACT_FUNCTION_RetryLater:
					state = MOVE;
					return NOT_HANDLED;
                    break;

                case ACT_FUNCTION_InProgress:
					return IN_PROGRESS;
                    break;

                default:
					state = MOVE;
					return NOT_HANDLED;
                    break;
            }
			break;
			
		case OPEN_GRABER:
			state = WAIT_OPEN_GRABER;
			ACT_plate_plier(ACT_PLATE_PLIER_OPEN);
			return IN_PROGRESS;
			break;

		case WAIT_OPEN_GRABER:
			sub_action = ACT_get_last_action_result(ACT_QUEUE_Plate);

			switch(sub_action)
            {
                case ACT_FUNCTION_Done:
                    state = PUSH;
					return IN_PROGRESS;
                    break;

                case ACT_FUNCTION_ActDisabled:
					state = DONE;
					return END_WITH_TIMEOUT;
                    break;

                case ACT_FUNCTION_RetryLater:
					state = MOVE;
					return NOT_HANDLED;
                    break;

                case ACT_FUNCTION_InProgress:
					return IN_PROGRESS;
                    break;

                default:
					state = MOVE;
					return NOT_HANDLED;
                    break;
            }
			break;

		case PUSH:
			sub_action = goto_pos_with_scan_foe((displacement_t[]){{{1000,COLOR_Y(400)}}},1,REAR,NORMAL_WAIT);
            switch(sub_action)
            {
                case END_OK:
                    state = GRAB;
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

		case GRAB:
			state = WAIT_GRAB;
			ACT_plate_plier(ACT_PLATE_PLIER_CLOSE);
			return IN_PROGRESS;
			break;

		case WAIT_GRAB:
			sub_action = ACT_get_last_action_result(ACT_QUEUE_Plate);

			switch(sub_action)
            {
                case ACT_FUNCTION_Done:
                    state = EAT_BALLS;
					return IN_PROGRESS;
                    break;

                case ACT_FUNCTION_ActDisabled:
					state = DONE;
					return END_WITH_TIMEOUT;
                    break;

                case ACT_FUNCTION_RetryLater:
					state = MOVE;
					return NOT_HANDLED;
                    break;

                case ACT_FUNCTION_InProgress:
					return IN_PROGRESS;
                    break;

                default:
					state = MOVE;
					return NOT_HANDLED;
                    break;
            }
			break;

		case EAT_BALLS:
			state = EAT_BALLS;
			state = WAIT_EAT_BALLS;
			ACT_plate_rotate(ACT_PLATE_RotateLuckyLuke);
			return IN_PROGRESS;
			break;

		case WAIT_EAT_BALLS:
			sub_action = ACT_get_last_action_result(ACT_QUEUE_Plate);
			switch(sub_action)
            {
                case ACT_FUNCTION_Done:
                    state = DONE;
					return IN_PROGRESS;
                    break;

                case ACT_FUNCTION_RetryLater:
					state = DONE;
					return END_WITH_TIMEOUT;
                    break;

                case ACT_FUNCTION_ActDisabled:
					state = MOVE;
					return NOT_HANDLED;
                    break;

                case ACT_FUNCTION_InProgress:
					return IN_PROGRESS;
                    break;

                default:
					state = MOVE;
					return NOT_HANDLED;
                    break;
            }
			break;
		case DONE:
			return END_OK;
			break;
		default:
			return NOT_HANDLED;
			break;

	}
}


error_e K_verres(void){
	static enum{
		SORTIR=0,
		PREMIER,
		SECOND,
		SECONDBIS,
		TROIS,
		QUATRE,
		CINQ,
		SIX,
        SEPT,
		DONE,
	}state = SORTIR;

	static error_e sub_action;


	switch(state){
		case SORTIR:
			sub_action = goto_pos(580,COLOR_Y(380),FAST,FORWARD);
			switch(sub_action){
				case IN_PROGRESS:
					break;
				case END_OK:
					state = PREMIER;
					break;
				case END_WITH_TIMEOUT:
					state = PREMIER;
					break;
				case NOT_HANDLED:
					state = PREMIER;
					break;
			}
			break;
		case PREMIER:
			sub_action = goto_pos_with_scan_foe(
					(displacement_t[]){{{300, COLOR_Y(580)},FAST},
					{{300, COLOR_Y(2300)},FAST},
					//{{550, COLOR_Y(2400)},FAST},
					{{575, COLOR_Y(2405)},
							FAST}},3,REAR,NO_AVOIDANCE);
			//TEST_STRAT_VERRE1();
			switch(sub_action){
				case IN_PROGRESS:
					break;
				case END_OK:
					state = SECOND;
					break;
				case END_WITH_TIMEOUT:
					state = SECOND;
					break;
				case NOT_HANDLED:
					state = SECOND;
					break;
			}
			break;
		case SECOND:
			sub_action = goto_pos_with_scan_foe(
					(displacement_t[]){{{548, COLOR_Y(600)},FAST},
							{{550, COLOR_Y(400)},FAST}},
							2,FORWARD,NO_AVOIDANCE);
					//TEST_STRAT_VERRE2();
			switch(sub_action){
				case IN_PROGRESS:
				{
					static int left=1;
					static int right=1;
					if(left && PORTBbits.RB5){
						left=0;
						debug_printf("Detected left verre");
						ACT_lift_plier(ACT_LIFT_Left,ACT_LIFT_PlierClose);
						ACT_lift_translate(ACT_LIFT_Left,ACT_LIFT_TranslateUp);

					}
					if(right && !PORTBbits.RB3){
						right=0;
						debug_printf("Detected right verre");
						ACT_lift_plier(ACT_LIFT_Right, ACT_LIFT_PlierClose);
						ACT_lift_translate(ACT_LIFT_Right, ACT_LIFT_TranslateUp);
					}
					break;
				}
				case END_OK:
					ACT_lift_plier(ACT_LIFT_Left, ACT_LIFT_PlierOpen);
					ACT_lift_plier(ACT_LIFT_Right, ACT_LIFT_PlierOpen);
					state = TROIS;
					break;
				case END_WITH_TIMEOUT:
					state = TROIS;
					break;
				case NOT_HANDLED:
					state = TROIS;
					break;
			}
			break;
		case SECONDBIS:
			sub_action = goto_pos_with_scan_foe(
					(displacement_t[]){{{555, COLOR_Y(2400)},FAST}},
					1,REAR,NO_AVOIDANCE);

			break;
		case TROIS:
			sub_action = goto_pos_with_scan_foe(
					(displacement_t[]){{{550, COLOR_Y(2200)},FAST},
					{{1000, COLOR_Y(2400)},FAST}},
							2,REAR,NO_AVOIDANCE);
			switch(sub_action){
				case IN_PROGRESS:
					break;
				case END_OK:
					state = QUATRE;
					break;
				case END_WITH_TIMEOUT:
					state = QUATRE;
					break;
				case NOT_HANDLED:
					state = QUATRE;
					break;
			}
			break;
		case QUATRE:
			sub_action = goto_pos_with_scan_foe(
					(displacement_t[]){{{850, COLOR_Y(2200)},FAST},
					{{850, COLOR_Y(1000)},FAST},
					{{850, COLOR_Y(600)},FAST},
					{{805, COLOR_Y(400)},FAST}},
					4,FORWARD,NO_AVOIDANCE);
				switch(sub_action){
					case IN_PROGRESS:
						break;
					case END_OK:
						state = CINQ;
						break;
					case END_WITH_TIMEOUT:
						state = CINQ;
						break;
					case NOT_HANDLED:
						state = CINQ;
						break;
				}
			break;
			case CINQ:
				sub_action = goto_pos_with_scan_foe(
				(displacement_t[]){{{800, COLOR_Y(2400)},FAST},
					{{1200, COLOR_Y(2500)},FAST}},
							2,REAR,NO_AVOIDANCE);
				switch(sub_action){
					case IN_PROGRESS:
						break;
					case END_OK:
						state = SIX;
						break;
					case END_WITH_TIMEOUT:
						state = SIX;
						break;
					case NOT_HANDLED:
						state = SIX;
						break;
				}
			break;
		case SIX:
				sub_action = goto_pos_with_scan_foe(
					(displacement_t[]){{{1075, COLOR_Y(2200)},FAST},
					{{1100, COLOR_Y(1000)},FAST},
					{{1075, COLOR_Y(600)},FAST},
					{{1050, COLOR_Y(400)},FAST}},
					4,FORWARD,NO_AVOIDANCE);
				switch(sub_action){
					case IN_PROGRESS:
						break;
					case END_OK:
						state = SEPT;
						break;
					case END_WITH_TIMEOUT:
						state = SEPT;
						break;
					case NOT_HANDLED:
						state = SEPT;
						break;
				}
				break;
                    case SEPT:
				sub_action = goto_pos_with_scan_foe(
					(displacement_t[]){{{1050, COLOR_Y(700)},FAST}},
					1,REAR,NO_AVOIDANCE);
				switch(sub_action){
					case IN_PROGRESS:
						break;
					case END_OK:
						state = DONE;
						break;
					case END_WITH_TIMEOUT:
						state = DONE;
						break;
					case NOT_HANDLED:
						state = DONE;
						break;
				}
				break;
		case DONE:
			return END_OK;
			break;
	}
	return IN_PROGRESS;

}


error_e K_push_half_row_glasses(void){
	static enum{
		PUSH_ROW,
		DONE
	}state = PUSH_ROW;

	static error_e sub_action;

	switch(state){
		case PUSH_ROW:
			sub_action = goto_pos_with_scan_foe(
					(displacement_t[]){{{1110, COLOR_Y(2150)},FAST},
					{{1010, COLOR_Y(1855)},FAST}}
					//{{550, COLOR_Y(2400)},FAST},
					,2,FORWARD,NO_AVOIDANCE);
			switch(sub_action){
				case IN_PROGRESS:
				{
					static int left=1;
					static int right=1;
					if(left && PORTBbits.RB5){
						left=0;
						debug_printf("Detected left verre");
						ACT_lift_plier(ACT_LIFT_Left,ACT_LIFT_PlierClose);
						ACT_lift_translate(ACT_LIFT_Left,ACT_LIFT_TranslateUp);

					}
					if(right && !PORTBbits.RB3){
						right=0;
						debug_printf("Detected right verre");
						ACT_lift_plier(ACT_LIFT_Right, ACT_LIFT_PlierClose);
						ACT_lift_translate(ACT_LIFT_Right, ACT_LIFT_TranslateUp);
					}
					break;
				}
				case END_OK:
					state = DONE;
					break;
				case NOT_HANDLED:
					state = PUSH_ROW;
					return NOT_HANDLED;
					break;
				case END_WITH_TIMEOUT:
					state = DONE;
					return END_WITH_TIMEOUT;
					break;
				default:
					state = PUSH_ROW;
					return NOT_HANDLED;

			}
			break;

		case DONE:
			return END_OK;
			break;
	}
	return IN_PROGRESS;
}


error_e TEST_Launcher_ball_mid(void){
    static Uint8 nb_ball=0;
    static error_e sub_action;
    static ACT_function_result_e sub_action_act;
    static enum {
        MOVE = 0,
        ANGLE,
        LAUNCH_BALL_NORMAL,
        LAUNCH_BALL_SLOW,
        LAUNCH_BALL_ATT,
        WAIT_BALL,
        WAIT_BALL_ATT,
        STOP_LAUNCH,
        STOP_LAUNCH_ATT,
        DONE,
    } state = MOVE;

    switch(state){
        case MOVE :
            sub_action = goto_pos_with_scan_foe((displacement_t[]){{{1000,COLOR_Y(700)}}},1,FORWARD,NORMAL_WAIT);
            switch(sub_action)
            {
                case END_OK:
                    state=ANGLE;
                    break;

                case END_WITH_TIMEOUT:
                    state=DONE;
                    return END_WITH_TIMEOUT;
                    break;
                case NOT_HANDLED:
                    state=MOVE;
                    return NOT_HANDLED;
                    break;

                case IN_PROGRESS:
                    return IN_PROGRESS;
                    break;

                default:
                    state=MOVE;
                    return NOT_HANDLED;
                    break;
            }
            break;

        case ANGLE:
            sub_action = goto_angle(((global.env.color == BLUE) ? -9646 : -3215), FAST);
            switch(sub_action)
            {
                case END_OK:
                    state = LAUNCH_BALL_NORMAL;
                    break;

                case END_WITH_TIMEOUT:
                    state = DONE;
                    return END_WITH_TIMEOUT;
                    break;

                case NOT_HANDLED:
                    state=MOVE;
                    return NOT_HANDLED;
                    break;

                case IN_PROGRESS:
                    return IN_PROGRESS;
                    break;

                default:
                    state=MOVE;
                    return NOT_HANDLED;
                    break;
            }
            break;

        case LAUNCH_BALL_NORMAL:
           ACT_ball_launcher_run(6400);  // a 66cm du bord du gateau
            state=LAUNCH_BALL_ATT;
            break;

        case LAUNCH_BALL_SLOW:
            ACT_ball_launcher_run(4000);
            state=LAUNCH_BALL_ATT;
            break;

        case LAUNCH_BALL_ATT:
            sub_action_act = ACT_get_last_action_result(ACT_QUEUE_BallLauncher);
            switch(sub_action_act)
            {
                case ACT_FUNCTION_InProgress:
                    return IN_PROGRESS;
                    break;

                case ACT_FUNCTION_Done:
                    state = WAIT_BALL;
                    break;

                case ACT_FUNCTION_ActDisabled:
                    state=MOVE;
                    return NOT_HANDLED;
                    break;

                case ACT_FUNCTION_RetryLater:
                    state=DONE;
                    return END_WITH_TIMEOUT;;
                    break;
                default:
                    state=MOVE;
                    return NOT_HANDLED;
                    break;
            }
            break;

        case WAIT_BALL:
            nb_ball=nb_ball+1;


            ACT_ball_sorter_next();
            state=WAIT_BALL_ATT;
            break;

        case WAIT_BALL_ATT:

            sub_action_act = ACT_get_last_action_result(ACT_QUEUE_BallSorter);
            switch(sub_action_act)
            {
                case ACT_FUNCTION_InProgress:
                    return IN_PROGRESS;
                    break;

                case ACT_FUNCTION_Done:
                    if (nb_ball>=8) state=STOP_LAUNCH;
                    else if (global.env.color_ball==0)state = LAUNCH_BALL_NORMAL;
                    else if (global.env.color_ball==1)state = LAUNCH_BALL_SLOW;
                    break;

                case ACT_FUNCTION_ActDisabled:
                    state=MOVE;
                    return NOT_HANDLED;
                    break;

                case ACT_FUNCTION_RetryLater:
                    state=DONE;
                    return END_WITH_TIMEOUT;
                    break;
                default:
                    state=MOVE;
                    return NOT_HANDLED;
                    break;
            }
            break;

        case STOP_LAUNCH:
            ACT_ball_launcher_stop();
            state=STOP_LAUNCH_ATT;
            break;

        case STOP_LAUNCH_ATT:
            sub_action_act = ACT_get_last_action_result(ACT_QUEUE_BallLauncher);
            switch(sub_action_act)
            {
                case ACT_FUNCTION_InProgress:
                    return IN_PROGRESS;
                    break;

                case ACT_FUNCTION_Done:
                    state = DONE;
                    break;

                case ACT_FUNCTION_ActDisabled:
                    state=MOVE;
                    return NOT_HANDLED;
                    break;

                case ACT_FUNCTION_RetryLater:
                    state=DONE;
                    return END_WITH_TIMEOUT;
                    break;
                default:
                    state=MOVE;
                    return NOT_HANDLED;
                    break;
            }
            break;

        case DONE:
            return END_OK;
            break;
        default :
            state=MOVE;
            return NOT_HANDLED;

    }
    return IN_PROGRESS;
}


error_e TEST_Launcher_ball_cadeau(void){
    static Uint8 nb_ball=0;
    static error_e sub_action;
    static ACT_function_result_e sub_action_act;
    static enum {
        MOVE = 0,
        ANGLE,
        LAUNCH_BALL_NORMAL,
        LAUNCH_BALL_SLOW,
        LAUNCH_BALL_ATT,
        WAIT_BALL,
        WAIT_BALL_ATT,
        STOP_LAUNCH,
        STOP_LAUNCH_ATT,
        DONE,
    } state = MOVE;

    switch(state){
        case MOVE :
            sub_action = goto_pos_with_scan_foe((displacement_t[]){{{600,COLOR_Y(700)}}},1,FORWARD,NORMAL_WAIT);
            switch(sub_action)
            {
                case END_OK:
                    state=ANGLE;
                    break;

                case END_WITH_TIMEOUT:
                    state=DONE;
                    return END_WITH_TIMEOUT;
                    break;
                case NOT_HANDLED:
                    state=MOVE;
                    return NOT_HANDLED;
                    break;

                case IN_PROGRESS:
                    return IN_PROGRESS;
                    break;

                default:
                    state=MOVE;
                    return NOT_HANDLED;
                    break;
            }
            break;

        case ANGLE:
            sub_action = goto_angle(((global.env.color == BLUE) ? -9145 : -2714), FAST);
            switch(sub_action)
            {
                case END_OK:
                    state = LAUNCH_BALL_NORMAL;
                    break;

                case END_WITH_TIMEOUT:
                    state = MOVE;
                    return END_WITH_TIMEOUT;
                    break;

                case NOT_HANDLED:
                    state=DONE;
                    return NOT_HANDLED;
                    
                    break;

                case IN_PROGRESS:
                    return IN_PROGRESS;
                    break;

                default:
                    state=DONE;
                    return NOT_HANDLED;
                    break;
            }
            break;

        case LAUNCH_BALL_NORMAL:
           ACT_ball_launcher_run(6900);
            state=LAUNCH_BALL_ATT;
            break;

        case LAUNCH_BALL_SLOW:
            ACT_ball_launcher_run(4000);
            state=LAUNCH_BALL_ATT;
            break;

        case LAUNCH_BALL_ATT:
            sub_action_act = ACT_get_last_action_result(ACT_QUEUE_BallLauncher);
            switch(sub_action_act)
            {
                case ACT_FUNCTION_InProgress:
                    return IN_PROGRESS;
                    break;

                case ACT_FUNCTION_Done:
                    state = WAIT_BALL;
                    break;

                case ACT_FUNCTION_ActDisabled:
                    state=MOVE;
                    return NOT_HANDLED;
                    break;

                case ACT_FUNCTION_RetryLater:
                    state=DONE;
                    return END_WITH_TIMEOUT;
                    break;
                default:
                    state=MOVE;
                    return NOT_HANDLED;
                    break;
            }
            break;

        case WAIT_BALL:
            nb_ball=nb_ball+1;


            ACT_ball_sorter_next();
            state=WAIT_BALL_ATT;
            break;

        case WAIT_BALL_ATT:

            sub_action_act = ACT_get_last_action_result(ACT_QUEUE_BallSorter);
            switch(sub_action_act)
            {
                case ACT_FUNCTION_InProgress:
                    return IN_PROGRESS;
                    break;

                case ACT_FUNCTION_Done:
                    if (nb_ball>=8) state=STOP_LAUNCH;
                    else if (global.env.color_ball==0)state = LAUNCH_BALL_NORMAL;
                    else if (global.env.color_ball==1)state = LAUNCH_BALL_SLOW;
                    break;

                case ACT_FUNCTION_ActDisabled:
                    state=MOVE;
                    return NOT_HANDLED;
                    break;

                case ACT_FUNCTION_RetryLater:
                    state=DONE;
                    return END_WITH_TIMEOUT;

                    break;
                default:
                    state=MOVE;
                    return NOT_HANDLED;
                    break;
            }
            break;

        case STOP_LAUNCH:
            ACT_ball_launcher_stop();
            state=STOP_LAUNCH_ATT;
            break;

        case STOP_LAUNCH_ATT:
            sub_action_act = ACT_get_last_action_result(ACT_QUEUE_BallLauncher);
            switch(sub_action_act)
            {
                case ACT_FUNCTION_InProgress:
                    return IN_PROGRESS;
                    break;

                case ACT_FUNCTION_Done:
                    state = DONE;
                    break;

                case ACT_FUNCTION_ActDisabled:
                    state=MOVE;
                    return NOT_HANDLED;
                    break;

                case ACT_FUNCTION_RetryLater:
                    state=DONE;
                    return END_WITH_TIMEOUT;
                    break;
                default:
                    break;
            }
            break;

        case DONE:
            return END_OK;
            break;
        default :
            state=MOVE;
            return NOT_HANDLED;
            break;
    }
    return IN_PROGRESS;

}

error_e TEST_Launcher_ball_gateau(void){
    static Uint8 nb_ball=0;
    static error_e sub_action;
    static ACT_function_result_e sub_action_act;
    static enum {
        MOVE = 0,
        ANGLE,
        LAUNCH_BALL_NORMAL,
        LAUNCH_BALL_SLOW,
        LAUNCH_BALL_ATT,
        WAIT_BALL,
        WAIT_BALL_ATT,
        STOP_LAUNCH,
        STOP_LAUNCH_ATT,
        DONE,
    } state = MOVE;

    switch(state){
        case MOVE :
            sub_action = goto_pos_with_scan_foe((displacement_t[]){{{1350,COLOR_Y(700)}}},1,FORWARD,NORMAL_WAIT);
            switch(sub_action)
            {
                case END_OK:
                    state=ANGLE;
                    break;

                case END_WITH_TIMEOUT:
                    state=DONE;
                    return END_WITH_TIMEOUT;
                    break;
                case NOT_HANDLED:
                    state=MOVE;
                    return NOT_HANDLED;
                    break;

                case IN_PROGRESS:
                    return IN_PROGRESS;
                    break;

                default:
                    state=MOVE;
                    return NOT_HANDLED;
                    break;
            }
            break;

        case ANGLE:
            sub_action = goto_angle(((global.env.color == BLUE) ? -9750 : -3320), FAST);
            switch(sub_action)
            {
                case END_OK:
                    state = LAUNCH_BALL_NORMAL;
                    break;

                case END_WITH_TIMEOUT:
                    state = MOVE;
                    return END_WITH_TIMEOUT;
                    break;

                case NOT_HANDLED:
                    state=DONE;
                    return NOT_HANDLED;
                    break;

                case IN_PROGRESS:
                    return IN_PROGRESS;
                    break;

                default:
                    state=DONE;
                    return NOT_HANDLED;
                    break;
            }
            break;

        case LAUNCH_BALL_NORMAL:
           ACT_ball_launcher_run(6050);
            state=LAUNCH_BALL_ATT;
            break;

        case LAUNCH_BALL_SLOW:
            ACT_ball_launcher_run(4000);
            state=LAUNCH_BALL_ATT;
            break;

        case LAUNCH_BALL_ATT:
            sub_action_act = ACT_get_last_action_result(ACT_QUEUE_BallLauncher);
            switch(sub_action_act)
            {
                case ACT_FUNCTION_InProgress:
                    return IN_PROGRESS;
                    break;

                case ACT_FUNCTION_Done:
                    state = WAIT_BALL;
                    break;

                case ACT_FUNCTION_ActDisabled:
                    state=MOVE;
                    return NOT_HANDLED;
                    break;

                case ACT_FUNCTION_RetryLater:
                    state=DONE;
                    return END_WITH_TIMEOUT;
                    break;
                default:
                    state=MOVE;
                    return NOT_HANDLED;
                    break;
            }
            break;

        case WAIT_BALL:
            nb_ball=nb_ball+1;


            ACT_ball_sorter_next();
            state=WAIT_BALL_ATT;
            break;

        case WAIT_BALL_ATT:

            sub_action_act = ACT_get_last_action_result(ACT_QUEUE_BallSorter);
            switch(sub_action_act)
            {
                case ACT_FUNCTION_InProgress:
                    return IN_PROGRESS;
                    break;

                case ACT_FUNCTION_Done:
                    if (nb_ball>=8) state=STOP_LAUNCH;
                    else if (global.env.color_ball==0)state = LAUNCH_BALL_NORMAL;
                    else if (global.env.color_ball==1)state = LAUNCH_BALL_SLOW;
                    break;

                case ACT_FUNCTION_ActDisabled:
                    state=MOVE;
                    return NOT_HANDLED;
                    break;

                case ACT_FUNCTION_RetryLater:
                    state=DONE;
                    return END_WITH_TIMEOUT;

                    break;
                default:
                    state=MOVE;
                    return NOT_HANDLED;
                    break;
            }
            break;

        case STOP_LAUNCH:
            ACT_ball_launcher_stop();
            state=STOP_LAUNCH_ATT;
            break;

        case STOP_LAUNCH_ATT:
            sub_action_act = ACT_get_last_action_result(ACT_QUEUE_BallLauncher);
            switch(sub_action_act)
            {
                case ACT_FUNCTION_InProgress:
                    return IN_PROGRESS;
                    break;

                case ACT_FUNCTION_Done:
                    state = DONE;
                    break;

                case ACT_FUNCTION_ActDisabled:
                    state=MOVE;
                    return NOT_HANDLED;
                    break;

                case ACT_FUNCTION_RetryLater:
                    state=DONE;
                    return END_WITH_TIMEOUT;
                    break;
                default:
                    break;
            }
            break;

        case DONE:
            return END_OK;
            break;
        default :
            state=MOVE;
            return NOT_HANDLED;
    }
    return IN_PROGRESS;

}
/* ----------------------------------------------------------------------------- */
/* 								Fonction diverses                     			 */
/* ----------------------------------------------------------------------------- */


/* ----------------------------------------------------------------------------- */
/* 							Découpe de stratégies                     			 */
/* ----------------------------------------------------------------------------- */

