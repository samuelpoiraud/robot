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

#include "actions_tests_krusty_micro.h"


#define DEFAULT_SPEED	(SLOW)

#define USE_CURVE	0




/* ----------------------------------------------------------------------------- */
/* 						Actions élémentaires de construction                     */
/* ----------------------------------------------------------------------------- */

/* ----------------------------------------------------------------------------- */
/* 							Pour France											*/
/* ----------------------------------------------------------------------------- */

bool_e act_go_on(queue_id_e act_id){
	error_e sub_actionner = ACT_get_last_action_result(act_id);
	bool_e ret = FALSE;
		switch(sub_actionner){
		case IN_PROGRESS:
			break;
		case END_OK:
		case END_WITH_TIMEOUT:
		case NOT_HANDLED:
		default:
		ret = TRUE;
			break;
	}
	return ret;
}

error_e two_first_rows(void){
	static enum{
		INIT = 0,
		FIRST_ROW,
		SECOND_ROW,
		DONE
	}state = INIT;

	enum actionneur_state{
		GRAB = 0,
		WAIT_GRAB,
		LIFT_DOWN,
		WAIT_LIFT_DOWN,
		LIFT_UP,
		ACT_DONE
	};


	static error_e act_prioritaire = GRAB;
	/*static error_e act_previous_left = GRAB;
	static error_e act_entrance_left;
	//Mise a jour
	act_entrance_left = (act_previous_left == act_left)? FALSE : TRUE;
	act_previous_left = act_left;
	*/
	
	static error_e act_secondaire = GRAB;
	/*static error_e act_previous_right = GRAB;
	static error_e act_entrance_right;
	//Mise a jour
	act_entrance_right = (act_previous_right == act_right)? FALSE : TRUE;
	act_previous_right = act_right;
	*/

	static ACT_lift_pos_t prioritaire;
	static ACT_lift_pos_t secondaire;
	static queue_id_e ret_prioritaire;
	static queue_id_e ret_secondaire;

	static bool_e right_ok = FALSE; //Flag qui sert à  faire patienter l'ascenseur de droite pour ne pas réagir au premier reach_y
	
	static error_e sub_action;
	
	error_e ret = IN_PROGRESS;

 // MAE de déplacements
	switch(state){
		case INIT:
			//debug_printf("state = ASK_WARNER");
			prioritaire = (global.env.color == BLUE)? ACT_LIFT_LEFT : ACT_LIFT_RIGHT;
			secondaire = (global.env.color == RED)? ACT_LIFT_LEFT : ACT_LIFT_RIGHT;
			ret_prioritaire = (global.env.color == BLUE)? ACT_QUEUE_LiftLeft : ACT_QUEUE_LiftRight;
			ret_secondaire = (global.env.color == RED)? ACT_QUEUE_LiftRight : ACT_QUEUE_LiftLeft;

			ASSER_WARNER_arm_y(COLOR_Y(900));
			state =FIRST_ROW;

		case FIRST_ROW:
			sub_action = goto_pos_with_scan_foe((displacement_t[]){{{1000,COLOR_Y(950)},FAST},
				{{1050,COLOR_Y(1500)},FAST}},
				2,FORWARD,NO_AVOIDANCE);
			if(sub_action != IN_PROGRESS){
				state = SECOND_ROW;
				ASSER_WARNER_arm_y(COLOR_Y(1350));
				right_ok = FALSE;
			}
			break;

		case SECOND_ROW:
			sub_action = goto_pos_with_scan_foe((displacement_t[]){{{820,COLOR_Y(1330)},FAST},
				{{750,COLOR_Y(1130)},FAST},
				{{750,COLOR_Y(750)},FAST}},
				3,FORWARD,NO_AVOIDANCE);
			if(sub_action != IN_PROGRESS)
				state = DONE;
			//	ret = END_OK;
			break;
		case DONE:
			break;
		default:
			break;
	}

 // MAE de l'ascenseur principale
	switch(act_prioritaire){
		case GRAB:
			if(global.env.asser.reach_y){
				//debug_printf("fst warner recieved");
				ACT_lift_plier(prioritaire, ACT_LIFT_PlierClose);
				act_prioritaire = WAIT_GRAB;
				ASSER_WARNER_arm_y(COLOR_Y(1200));
			}
			break;
		case WAIT_GRAB:
			right_ok = TRUE;
			if(act_go_on(ret_prioritaire) == TRUE){
				//debug_printf("retour lift left:ok");
				ACT_lift_translate(prioritaire,ACT_LIFT_TranslateUp);
				act_prioritaire = LIFT_DOWN;
			}
			break;
		case LIFT_DOWN:
			if(state == SECOND_ROW){
				if(global.env.asser.reach_y){
					ACT_lift_plier(prioritaire, ACT_LIFT_PlierOpen);
					ACT_lift_translate(prioritaire, ACT_LIFT_TranslateDown);
					ASSER_WARNER_arm_y(COLOR_Y(1050));
					act_prioritaire = WAIT_LIFT_DOWN;
				}
			}
			break;
		case WAIT_LIFT_DOWN:
			right_ok = TRUE;
			if(act_go_on(ret_prioritaire) == TRUE){
				ACT_lift_plier(prioritaire, ACT_LIFT_PlierClose);
				act_prioritaire = LIFT_UP;
			}
			break;
		case LIFT_UP:
			if(act_go_on(ret_prioritaire) == TRUE){
				ACT_lift_translate(prioritaire, ACT_LIFT_TranslateUp);
				act_prioritaire = ACT_DONE;
			}
			break;
		case ACT_DONE:
			break;
		default:
			break;
	}

 // MAE du second ascenseur
	switch(act_secondaire){
		case GRAB:
			if(right_ok){
				if(global.env.asser.reach_y){
					//debug_printf("snd warner recieved");
					ACT_lift_plier(secondaire, ACT_LIFT_PlierClose);
					act_secondaire = WAIT_GRAB;
				}
			}
			break;
		case WAIT_GRAB:
			if(act_go_on(ret_secondaire) == TRUE){
				//debug_printf("retour lift right:ok");
				ACT_lift_translate(secondaire, ACT_LIFT_TranslateUp);
				act_secondaire = LIFT_DOWN;
			}
			break;
		case LIFT_DOWN:
			if(state == SECOND_ROW && right_ok){
				if(global.env.asser.reach_y){
					ACT_lift_plier(secondaire, ACT_LIFT_PlierOpen);
					ACT_lift_translate(secondaire, ACT_LIFT_TranslateDown);
					ASSER_WARNER_arm_y(COLOR_Y(1050));
					act_secondaire = WAIT_LIFT_DOWN;
				}
			}
			break;
		case WAIT_LIFT_DOWN:
			if(act_go_on(ret_secondaire) == TRUE){
				ACT_lift_plier(secondaire, ACT_LIFT_PlierClose);
				act_secondaire = LIFT_UP;
			}
			break;
		case LIFT_UP:
			if(act_go_on(ret_secondaire) == TRUE){
				ACT_lift_translate(secondaire, ACT_LIFT_TranslateUp);
				act_secondaire = ACT_DONE;
			}
			break;
		case ACT_DONE:
			ret = END_OK;
			break;
		default:
			break;
	}

	return ret;
}


//Fonction qui va chercher la dernière rangée de verres du côté cadeau jusqu'à évitement dans le sens avant vers l'adversaire
error_e try_last_row(void){
	static enum{
		PUSH = 0,
		STOP_AND_WAIT
	}asser_state = PUSH;

	enum actionneur_state{
		WAIT_CAPTOR,
		LIFT_DOWN,
		GRAB
	};

	static enum actionneur_state act_left = WAIT_CAPTOR;
	static enum actionneur_state act_right = WAIT_CAPTOR;
}



Uint8 wait_plier(Uint8 in_progress, Uint8 success_state, Uint8 fail_state){
	static error_e sub_action;
	sub_action = ACT_get_last_action_result(ACT_QUEUE_Plate);
	switch(sub_action)
		{
                case ACT_FUNCTION_Done:
					return success_state;
                    break;
                case ACT_FUNCTION_ActDisabled:
                case ACT_FUNCTION_RetryLater:
					return fail_state;
                    break;
                case ACT_FUNCTION_InProgress:
					return in_progress;
                    break;
                default:
					return fail_state;
                    break;
            }
	return in_progress;
}

error_e Lacher_verres(Uint8 type){
	static enum{
		INIT,
		ANGLE_TYPE1,
		ANGLE_TYPE2,
		OPEN_PLIER,
		WAIT_PLIER,
		EXTRACT_TYPE1,
		EXTRACT_TYPE2
	}state = INIT;

	static error_e sub_action;
	error_e ret = IN_PROGRESS;

	switch(state){
		case INIT:
			if(type == 0)
				state = ANGLE_TYPE1;
			else
				state = ANGLE_TYPE2;
			break;
		case ANGLE_TYPE1:
			state = try_go_angle(COLOR_ANGLE(-PI4096), ANGLE_TYPE1, OPEN_PLIER, OPEN_PLIER, FAST);
			break;
		case ANGLE_TYPE2:
			state = try_go_angle(COLOR_ANGLE(-PI4096/2), ANGLE_TYPE2, OPEN_PLIER, OPEN_PLIER, FAST);
			break;
		case OPEN_PLIER:
			ACT_plate_plier(ACT_PLATE_PLIER_OPEN);
			state = WAIT_PLIER;
			break;
		case WAIT_PLIER:
			//Si l'action s'est bien déroulée ou non on se dirige versl'extraction correspondante à l'argument type
			state = wait_plier(WAIT_PLIER, (!type)? EXTRACT_TYPE1 : EXTRACT_TYPE2, (!type)? EXTRACT_TYPE1 : EXTRACT_TYPE2);
			break;
		case EXTRACT_TYPE1:
			sub_action = goto_pos(1005, COLOR_Y(323), SLOW, BACKWARD, END_AT_LAST_POINT);
			if(sub_action != IN_PROGRESS)
				state = INIT;
				ret = END_OK;
			break;
		case EXTRACT_TYPE2:
			sub_action = goto_pos(955, COLOR_Y(340), SLOW, BACKWARD, END_AT_LAST_POINT);
			if(sub_action != IN_PROGRESS)
				state = INIT;
				ret = END_OK;
			break;
	}
	return ret;
}


error_e K_Sortie(void){
    static error_e sub_action;
    sub_action = goto_pos_with_scan_foe((displacement_t[]){{{1000,COLOR_Y(400)}}},1,FORWARD,NO_AVOIDANCE);
    switch(sub_action){
        case END_OK:
             return END_OK;
             break;
        case END_WITH_TIMEOUT:
             return END_WITH_TIMEOUT;
             break;
        case NOT_HANDLED:
              return NOT_HANDLED;
              break;
        case IN_PROGRESS:
            return IN_PROGRESS;
              break;
        default:
              break;

    }
    return IN_PROGRESS;
}

error_e K_push_half_row_glasses(void){
	static enum{
		ASK_WARNER = 0,
		PUSH_ROW,
		BACK_ROW,
		DONE
	}state = ASK_WARNER;

	enum action_verre{
		WAIT_EVENT = 0,
		ACT_UP,
		WAIT_ACT_UP,
		WAIT_NEXT_EVENT,
		ACT_DOWN,
		WAIT_ACT_DOWN,
		ACT_UP_BIS,
		ACT_DONE
	};
	static enum action_verre lift_left = WAIT_EVENT;
	static enum action_verre lift_right = WAIT_EVENT;

	static error_e sub_action;
	static error_e act_left;
	static error_e act_right;
	static bool_e captor_ok;
	static avoidance_type_e avoidance=NO_DODGE_AND_NO_WAIT;
	if(global.env.asser.reach_y){
		captor_ok = TRUE;
	}


	switch(state){
		case ASK_WARNER:
			ASSER_WARNER_arm_y(COLOR_Y(690));
			state = PUSH_ROW;
			break;

		case PUSH_ROW:
			sub_action = goto_pos_with_scan_foe(
					(displacement_t[]){{{975, COLOR_Y(500)},FAST},
					{{1000, COLOR_Y(790)},FAST},
					{{1070, COLOR_Y(1070)},FAST},
					{{1112, COLOR_Y(1310)},FAST},
					{{1040, COLOR_Y(1510)},FAST}},
					5,FORWARD,avoidance);

			switch(sub_action){
				case IN_PROGRESS:
					break;
				case END_OK:
					state = BACK_ROW;
					break;
				case NOT_HANDLED:
					state = DONE;
					return NOT_HANDLED;
					break;
				case END_WITH_TIMEOUT:
					state = DONE;
					return END_WITH_TIMEOUT;
					break;
				default:
					state = PUSH_ROW;
					return NOT_HANDLED;
					break;
			}
			break;

		case BACK_ROW:
			sub_action = goto_pos_with_scan_foe(
					(displacement_t[]){{{882, COLOR_Y(1370)},FAST},
					{{782, COLOR_Y(1145)},FAST},
					{{760, COLOR_Y(990)},SLOW}},
					3,FORWARD,avoidance);

					switch(sub_action){
				case IN_PROGRESS:
					break;
				case END_OK:
					state = DONE;
					return END_OK;
                                        break;
				case NOT_HANDLED:
					state = DONE;
					return NOT_HANDLED;
					break;
				case END_WITH_TIMEOUT:
					state = DONE;
					return END_WITH_TIMEOUT;
					break;
				default:
					state = DONE;
					return NOT_HANDLED;
					break;
			}
			break;

		case DONE:
			return END_OK;
			break;
	}

	//Ascenceur gauche
	switch(lift_left){
		case WAIT_EVENT:
			if(captor_ok)
			{
				if(PORTBbits.RB5 && state == PUSH_ROW){
					lift_left = ACT_UP;
				}
			}
			break;

		case ACT_UP:
			ACT_lift_plier(ACT_LIFT_Left,ACT_LIFT_PlierClose);
			ACT_lift_translate(ACT_LIFT_Left,ACT_LIFT_TranslateUp);
			lift_left = WAIT_ACT_UP;
			break;

		case WAIT_ACT_UP:
			act_left = ACT_get_last_action_result(ACT_QUEUE_LiftLeft);
			switch(act_left){
				case IN_PROGRESS:
					break;
				case END_OK:
					lift_left = WAIT_NEXT_EVENT;
					break;
				case END_WITH_TIMEOUT:
					lift_left = ACT_DONE;
					break;
				case NOT_HANDLED:
					lift_left = WAIT_EVENT;
					break;
				default:
					break;
			}

		case WAIT_NEXT_EVENT:
			if(PORTBbits.RB5 && state == BACK_ROW){
				lift_left = ACT_DOWN;
			}
			break;

		case ACT_DOWN:
			ACT_lift_plier(ACT_LIFT_Left,ACT_LIFT_PLIER_OPEN);
			ACT_lift_translate(ACT_LIFT_Left,ACT_LIFT_TranslateDown);
			lift_left = WAIT_ACT_DOWN;
			break;

		case WAIT_ACT_DOWN:
			act_left = ACT_get_last_action_result(ACT_QUEUE_LiftLeft);
			switch(act_left){
				case IN_PROGRESS:
					break;
				case END_OK:
					lift_left = ACT_UP_BIS;
					break;
				case END_WITH_TIMEOUT:
					lift_left = ACT_DONE;
					break;
				case NOT_HANDLED:
					lift_left = ACT_DOWN;
					break;
				default:
					debug_printf("Fuyez cava peter!!");
					break;
			}
			break;

		case ACT_UP_BIS:
			ACT_lift_plier(ACT_LIFT_Left,ACT_LIFT_PlierClose);
			ACT_lift_translate(ACT_LIFT_Left,ACT_LIFT_TranslateUp);
			lift_left = ACT_DONE;
			break;

		case ACT_DONE:
			break;
	}

	//Ascenceur droite
	switch(lift_right){
		case WAIT_EVENT:
			if(captor_ok)
			{
				if(!PORTBbits.RB3 && state == PUSH_ROW){
					lift_right = ACT_UP;
				}
			}
			break;

		case ACT_UP:
			ACT_lift_plier(ACT_LIFT_Right,ACT_LIFT_PlierClose);
			ACT_lift_translate(ACT_LIFT_Right,ACT_LIFT_TranslateUp);
			lift_right = WAIT_ACT_UP;
			break;

		case WAIT_ACT_UP:
			act_right = ACT_get_last_action_result(ACT_QUEUE_LiftRight);
			switch(act_right){
				case IN_PROGRESS:
					break;
				case END_OK:
					lift_right = WAIT_NEXT_EVENT;
					break;
				case END_WITH_TIMEOUT:
					lift_right = ACT_DONE;
					break;
				case NOT_HANDLED:
					lift_right = WAIT_EVENT;
					break;
				default:
					debug_printf("Fuyez cava peter!!");
					break;
			}

		case WAIT_NEXT_EVENT:
			if(!PORTBbits.RB3 && state == BACK_ROW){
				lift_right = ACT_DOWN;
			}
			break;

		case ACT_DOWN:
			ACT_lift_plier(ACT_LIFT_Right,ACT_LIFT_PLIER_OPEN);
			ACT_lift_translate(ACT_LIFT_Right,ACT_LIFT_TranslateDown);
			lift_right = WAIT_ACT_DOWN;
			break;

		case WAIT_ACT_DOWN:
			act_right = ACT_get_last_action_result(ACT_QUEUE_LiftRight);
			switch(act_right){
				case IN_PROGRESS:
					break;
				case END_OK:
					lift_right = ACT_UP_BIS;
					break;
				case END_WITH_TIMEOUT:
					lift_right = ACT_DONE;
					break;
				case NOT_HANDLED:
					lift_right = ACT_DOWN;
					break;
				default:
					debug_printf("Fuyez cava peter!!");
					break;
			}
			break;

		case ACT_UP_BIS:
			ACT_lift_plier(ACT_LIFT_Right,ACT_LIFT_PlierClose);
			ACT_lift_translate(ACT_LIFT_Right,ACT_LIFT_TranslateUp);
			lift_right = ACT_DONE;
			break;

		case ACT_DONE:
			break;
	}


	return IN_PROGRESS;
}

error_e K_push_back_row_glasses(void){
	static enum{
		PUSH_ROW = 0,
		BACK_ROW,
		LUCKY_LUKE,
        LUCKY_LUKE2,
		DONE_AND_WAIT,
		EXTRACT,
		DONE
	}state = PUSH_ROW;

	enum action_verre{
		WAIT_EVENT = 0,
		ACT_DOWN,
		WAIT_ACT_DOWN,
		ACT_GRAB_ROW,
		ACT_OPEN,
		ACT_DONE
	};

	static enum action_verre lift_left = ACT_OPEN;
	static enum action_verre lift_right = ACT_OPEN;
	static error_e act_left;
	static error_e act_right;

	static error_e sub_action;

	switch(state){
		case PUSH_ROW:

			sub_action = goto_pos_with_scan_foe(
					(displacement_t[]){{{750, COLOR_Y(1280)},FAST}},
					1,BACKWARD,NO_DODGE_AND_NO_WAIT);

			switch(sub_action){
				case IN_PROGRESS:
					break;
				case END_OK:
					state = BACK_ROW;
					break;
				case NOT_HANDLED:

					state = LUCKY_LUKE2;
					break;
				case END_WITH_TIMEOUT:
					state = LUCKY_LUKE2;
					break;
				default:
					state = LUCKY_LUKE2;
					return NOT_HANDLED;
					break;
			}
			break;

		case BACK_ROW:
			sub_action = goto_pos_with_scan_foe(
					(displacement_t[]){{{615, COLOR_Y(1170)},FAST},
					{{505, COLOR_Y(910)},FAST}},
					2,FORWARD,NO_DODGE_AND_NO_WAIT);
			switch(sub_action){
				case IN_PROGRESS:
					break;
				case END_OK:
					state = LUCKY_LUKE;
					break;
				case NOT_HANDLED:
					state = LUCKY_LUKE2;
					break;
				case END_WITH_TIMEOUT:
					state = LUCKY_LUKE2;
					break;
				default:
					state = PUSH_ROW;
					return NOT_HANDLED;
					break;
			}
			break;

		case LUCKY_LUKE:
				sub_action = goto_pos_with_scan_foe(
					(displacement_t[]){{{740, COLOR_Y(555)}}},
					1,FORWARD,NO_AVOIDANCE);
				switch(sub_action){
					case IN_PROGRESS:
						break;
					case END_OK:
						state = LUCKY_LUKE2;
						break;
					case NOT_HANDLED:
						state = LUCKY_LUKE2;
						break;
					case END_WITH_TIMEOUT:
						state = LUCKY_LUKE2;
						break;
					default:
						state = LUCKY_LUKE2;
						//return NOT_HANDLED;
						break;
				}

			break;
                        case LUCKY_LUKE2:
                            sub_action = goto_pos_with_scan_foe(
					(displacement_t[]){{{955, COLOR_Y(323)},FAST}},
					1,FORWARD,NO_AVOIDANCE);
				switch(sub_action){
					case IN_PROGRESS:
						break;
					case END_OK:
						state = EXTRACT;
						break;
					case NOT_HANDLED:
						state = EXTRACT;
						break;
					case END_WITH_TIMEOUT:
						state = EXTRACT;
						break;
					default:
						state = PUSH_ROW;
                                            return NOT_HANDLED;
						break;
				}

			break;

		case DONE_AND_WAIT:
			if(lift_left == ACT_DONE && lift_right == ACT_DONE )
				state = EXTRACT;
			break;

		case EXTRACT:
			sub_action = goto_pos_with_scan_foe(
					(displacement_t[]){{{800, COLOR_Y(500)},FAST}},
					1,BACKWARD,NO_AVOIDANCE);

			switch(sub_action){
				case IN_PROGRESS:
					break;
				case END_OK:
				case NOT_HANDLED:
				case END_WITH_TIMEOUT:
					state = DONE;
                    return END_OK;
					break;
				default:
					state = DONE;
					return NOT_HANDLED;
					break;
			}
			break;

		case DONE:
			return END_OK;
			break;

		default:
			break;
	}


/* ASCENCEUR gauche */
	switch(lift_left){
		case WAIT_EVENT:
			if(state == LUCKY_LUKE2)
				lift_left = ACT_OPEN;
			if(PORTBbits.RB5 && state == BACK_ROW){
				lift_left = ACT_DOWN;
			}
			break;

		case ACT_DOWN:
			ACT_lift_plier(ACT_LIFT_Left,ACT_LIFT_PLIER_OPEN);
			ACT_lift_translate(ACT_LIFT_Left,ACT_LIFT_TranslateDown);
			lift_left = WAIT_ACT_DOWN;

		case WAIT_ACT_DOWN:
			act_left = ACT_get_last_action_result(ACT_QUEUE_LiftLeft);
			switch(act_left){
				case IN_PROGRESS:
					break;
				case END_OK:
					lift_left = ACT_GRAB_ROW;
					ACT_lift_plier(ACT_LIFT_Left,ACT_LIFT_PlierClose);
					break;
				case END_WITH_TIMEOUT:
					lift_right = ACT_DONE;
					break;
				case NOT_HANDLED:
					lift_right = ACT_DOWN;
					break;
				default:
					debug_printf("Fuyez cava peter!!");
					break;
			}
			break;


		case ACT_GRAB_ROW:
			act_left = ACT_get_last_action_result(ACT_QUEUE_LiftLeft);
				switch(act_left){
				case IN_PROGRESS:
					break;
				case END_OK:
				case END_WITH_TIMEOUT:
				case NOT_HANDLED:
				default:
					lift_left = ACT_OPEN;
					break;
			}
			break;



		case ACT_OPEN:
				if(state == EXTRACT){
				ACT_lift_plier(ACT_LIFT_Left,ACT_LIFT_PlierOpen);
				lift_left = ACT_DONE;
			}

		case ACT_DONE:
			break;

		default:
			break;
	}




	switch(lift_right){
		case WAIT_EVENT:
			if(state == LUCKY_LUKE)
				lift_right = ACT_OPEN;
			if(!PORTBbits.RB3 && state == BACK_ROW){
				lift_right = ACT_DOWN;
			}
			break;

		case ACT_DOWN:
			ACT_lift_plier(ACT_LIFT_Right,ACT_LIFT_PLIER_OPEN);
			ACT_lift_translate(ACT_LIFT_Right,ACT_LIFT_TranslateDown);
			lift_right = WAIT_ACT_DOWN;

		case WAIT_ACT_DOWN:
			act_right = ACT_get_last_action_result(ACT_QUEUE_LiftRight);
			switch(act_right){
				case IN_PROGRESS:
					break;
				case END_OK:
					ACT_lift_plier(ACT_LIFT_Right,ACT_LIFT_PlierClose);
					lift_right = ACT_GRAB_ROW;
					break;
				case END_WITH_TIMEOUT:
					lift_right = ACT_DONE;
					break;
				case NOT_HANDLED:
					lift_right = ACT_DOWN;
					break;
				default:
					debug_printf("Fuyez cava peter!!");
					break;
			}
			break;


		case ACT_GRAB_ROW:
			act_right = ACT_get_last_action_result(ACT_QUEUE_LiftRight);
			switch(act_right){
				case IN_PROGRESS:
					break;
				case END_OK:
				case END_WITH_TIMEOUT:
				case NOT_HANDLED:
				default:
					lift_right = ACT_OPEN;
					break;
			}
			break;

		case ACT_OPEN:
			if(state == EXTRACT){
				ACT_lift_plier(ACT_LIFT_Right,ACT_LIFT_PlierOpen);
				lift_right = ACT_DONE;
			}
		case ACT_DONE:
			break;

		default:
			break;
	}

	return IN_PROGRESS;
}

error_e TEST_STRAT_assiettes_evitement_2(void){

    static error_e sub_action;
    static ACT_function_result_e sub_action_act;
    static enum {
		POS_MOVE1 = 0,
                POS_MOVE2,
                GRABBER_DOWN_ATT,
                GRABBER_OPEN_ATT,
		PUSH,
                GRABBER_UP_ATT,
                BACK,
                GRABBER_MID_ATT,
                GRABBER_OPEN2_ATT,
                GRABBER_CLOSE2_ATT,
                GRABBER_UP2_ATT,
                DONE,
    } state = POS_MOVE1;


    switch (state) {
         case POS_MOVE1:

            sub_action = goto_pos_with_scan_foe((displacement_t[]){{{(global.env.color == BLUE) ? 625 : 620,COLOR_Y(620)}}},1,FORWARD,NO_AVOIDANCE);
            switch(sub_action)
            {
                case END_OK:
                    state=POS_MOVE2;
                    break;

                case END_WITH_TIMEOUT:
                    state=POS_MOVE2;
                    break;
                case NOT_HANDLED:
                    state=POS_MOVE2;
                    break;

                case IN_PROGRESS:
                    return IN_PROGRESS;
                    break;

                default:
                    state=POS_MOVE2;
                    break;
            }
            break;
        case POS_MOVE2:

            sub_action = goto_angle(((global.env.color == BLUE) ? -PI4096/2 : PI4096/2), FAST);
            switch(sub_action)
            {
                case END_OK:
                    ACT_plate_rotate(ACT_PLATE_RotateDown);
                    state=GRABBER_DOWN_ATT;
                    break;

                case END_WITH_TIMEOUT:
                    state=DONE;
                    return END_WITH_TIMEOUT;
                    break;
                case NOT_HANDLED:
                    state=POS_MOVE1;
                    return NOT_HANDLED;
                    break;

                case IN_PROGRESS:
                    break;

                default:
                    break;
            }
            break;

         case GRABBER_DOWN_ATT:
            sub_action_act = ACT_get_last_action_result(ACT_QUEUE_Plate);
            switch(sub_action_act)
            {
                case ACT_FUNCTION_InProgress:
                    return IN_PROGRESS;
                    break;

                case ACT_FUNCTION_Done:
                    ACT_plate_plier(ACT_PLATE_PlierOpen);
                    state =GRABBER_OPEN_ATT;
                    break;

                case ACT_FUNCTION_ActDisabled:
                    ACT_plate_plier(ACT_PLATE_PlierOpen);
                    state=GRABBER_OPEN_ATT;
                    break;

                case ACT_FUNCTION_RetryLater:
                    ACT_plate_plier(ACT_PLATE_PlierOpen);
                   state=GRABBER_OPEN_ATT;
                    break;
                default:
                    state=POS_MOVE1;
                    return NOT_HANDLED;
                    break;
            }
            break;

         case GRABBER_OPEN_ATT:
            sub_action_act = ACT_get_last_action_result(ACT_QUEUE_Plate);
            switch(sub_action_act)
           {
                case ACT_FUNCTION_InProgress:
                    return IN_PROGRESS;
                    break;

                case ACT_FUNCTION_Done:
                    state =PUSH;
                    break;

                case ACT_FUNCTION_ActDisabled:
                    state=POS_MOVE1;
                    return NOT_HANDLED;
                    break;

                case ACT_FUNCTION_RetryLater:
                    state=DONE;
                    return END_WITH_TIMEOUT;
                    break;
                default:
                    state=POS_MOVE1;
                    return NOT_HANDLED;
                    break;
            }
            break;

        case PUSH:
            sub_action = goto_pos_with_scan_foe((displacement_t[]){{{(global.env.color == BLUE) ? 620 : 620,COLOR_Y(390)}}},1,BACKWARD,NO_AVOIDANCE);
            switch(sub_action)
            {
                case END_OK:
                    ACT_plate_rotate(ACT_PLATE_RotateUp);
                    state=GRABBER_UP_ATT;
                    break;

                case END_WITH_TIMEOUT:
                    state=BACK;
                    break;
                case NOT_HANDLED:
                     state=BACK;
                    break;

                case IN_PROGRESS:

                    if(COLOR_Y(global.env.pos.y)<500){
                        static Uint8 crush=0;
                        if(crush==0){
                           ACT_plate_plier(ACT_PLATE_PlierClose);
                           crush=1;
                        }

                     }
                    return IN_PROGRESS;
                     break;

                default:
                    return NOT_HANDLED;
                     break;
                    }
                  break;

            break;
         case GRABBER_UP_ATT:
            sub_action_act = ACT_get_last_action_result(ACT_QUEUE_Plate);
            switch(sub_action_act)
            {
                case ACT_FUNCTION_InProgress:
                    return IN_PROGRESS;
                    break;

                case ACT_FUNCTION_Done:
                    state=BACK;
                    break;

                case ACT_FUNCTION_ActDisabled:
                    state=POS_MOVE1;
                    return NOT_HANDLED;
                    break;

                case ACT_FUNCTION_RetryLater:
                    state=DONE;
                    return END_WITH_TIMEOUT;
                    break;
                default:
                    state=POS_MOVE1;
                    return NOT_HANDLED;
                    break;
            }
            break;

        case BACK:

            sub_action = goto_pos_with_scan_foe((displacement_t[]){{{(global.env.color == BLUE) ? 620 : 620,COLOR_Y(620)}}},1,FORWARD,NO_DODGE_AND_NO_WAIT);
            switch(sub_action)
            {
                case END_OK:

                     ACT_plate_rotate(ACT_PLATE_RotateMid);
                    state=GRABBER_MID_ATT;
                    break;

                case END_WITH_TIMEOUT:
                    state=DONE;
                    return END_WITH_TIMEOUT;
                    break;
                case NOT_HANDLED:
                    state=POS_MOVE1;
                    return NOT_HANDLED;
                    break;

                case IN_PROGRESS:
                    return IN_PROGRESS;
                    break;

                default:
                    state=POS_MOVE1;
                    return NOT_HANDLED;
                    break;
            }
            break;

         case GRABBER_MID_ATT:
            sub_action_act = ACT_get_last_action_result(ACT_QUEUE_Plate);
            switch(sub_action_act)
            {
                case ACT_FUNCTION_InProgress:
                    return IN_PROGRESS;
                    break;

                case ACT_FUNCTION_Done:
                    ACT_plate_plier(ACT_PLATE_PlierOpen);
                    state =GRABBER_OPEN2_ATT;
                    break;

                case ACT_FUNCTION_ActDisabled:
                    state=POS_MOVE1;
                    return NOT_HANDLED;
                    break;

                case ACT_FUNCTION_RetryLater:
                    state=DONE;
                    return END_WITH_TIMEOUT;
                    break;
                default:
                    state=POS_MOVE1;
                    return NOT_HANDLED;
                    break;
            }
            break;

         case GRABBER_OPEN2_ATT:
            sub_action_act = ACT_get_last_action_result(ACT_QUEUE_Plate);
            switch(sub_action_act)
           {
                case ACT_FUNCTION_InProgress:
                    return IN_PROGRESS;
                    break;

                case ACT_FUNCTION_Done:
                    ACT_plate_plier(ACT_PLATE_PlierClose);
                    state =GRABBER_CLOSE2_ATT;
                    break;

                case ACT_FUNCTION_ActDisabled:
                    state=POS_MOVE1;
                    return NOT_HANDLED;
                    break;

                case ACT_FUNCTION_RetryLater:
                    state=DONE;
                    return END_WITH_TIMEOUT;
                    break;
                default:
                    state=POS_MOVE1;
                    return NOT_HANDLED;
                    break;
            }
            break;

         case GRABBER_CLOSE2_ATT:
            sub_action_act = ACT_get_last_action_result(ACT_QUEUE_Plate);
            switch(sub_action_act)
            {
                case ACT_FUNCTION_InProgress:
                    return IN_PROGRESS;
                    break;

                case ACT_FUNCTION_Done:
                    ACT_plate_rotate(ACT_PLATE_RotateUp);
                    state =GRABBER_UP2_ATT;
                    break;

                case ACT_FUNCTION_ActDisabled:
                    state=POS_MOVE1;
                    return NOT_HANDLED;
                    break;

                case ACT_FUNCTION_RetryLater:
                    state=DONE;
                    return END_WITH_TIMEOUT;
                    break;
                default:
                    state=POS_MOVE1;
                    return NOT_HANDLED;
                    break;
            }
            break;

         case GRABBER_UP2_ATT:
            sub_action_act = ACT_get_last_action_result(ACT_QUEUE_Plate);
            switch(sub_action_act)
            {
                case ACT_FUNCTION_InProgress:
                    return IN_PROGRESS;
                    break;

                case ACT_FUNCTION_Done:
                    state =DONE;
                    return END_OK;
                    break;

                case ACT_FUNCTION_ActDisabled:
                    state=POS_MOVE1;
                    return NOT_HANDLED;
                    break;

                case ACT_FUNCTION_RetryLater:
                    state=DONE;
                    return END_WITH_TIMEOUT;
                    break;
                default:
                    state=POS_MOVE1;
                    return NOT_HANDLED;
                    break;
            }
            break;


        case DONE:
            return END_OK;
            break;
        default:
            state=POS_MOVE1;
            return NOT_HANDLED;
            break;
    }
    return IN_PROGRESS;
}

error_e TEST_STRAT_assiettes_evitement_4(void){

    static error_e sub_action;
    static ACT_function_result_e sub_action_act;
    static enum {
                POS_MOVE1=0,
                POS_MOVE2,
                GRABBER_DOWN_ATT,
                GRABBER_OPEN_ATT,
		PUSH,
                GRABBER_UP_ATT,
                BACK,
                GRABBER_MID_ATT,
                GRABBER_OPEN2_ATT,
                GRABBER_CLOSE2_ATT,
                GRABBER_UP2_ATT,
                DONE,
    } state = POS_MOVE1;


    switch (state) {
        case POS_MOVE1:

            sub_action = goto_pos_with_scan_foe((displacement_t[]){{{ (global.env.color == BLUE) ? 1430 : 1430 ,COLOR_Y(620)}}},1,BACKWARD,NO_AVOIDANCE);
            switch(sub_action)
            {
                case END_OK:
                    state=POS_MOVE2;
                    break;

                case END_WITH_TIMEOUT:
                    state=DONE;
                    return END_WITH_TIMEOUT;
                    break;
                case NOT_HANDLED:
                    state=POS_MOVE1;
                    return NOT_HANDLED;
                    break;

                case IN_PROGRESS:
                    break;

                default:
                    break;
            }
            break;
            case POS_MOVE2:

            sub_action = goto_angle(((global.env.color == BLUE) ? -PI4096/2 : PI4096/2), FAST);
            switch(sub_action)
            {
                case END_OK:
                    ACT_plate_rotate(ACT_PLATE_RotateDown);
                    state=GRABBER_DOWN_ATT;
                    break;

                case END_WITH_TIMEOUT:
                    state=DONE;
                    return END_WITH_TIMEOUT;
                    break;
                case NOT_HANDLED:
                    state=POS_MOVE1;
                    return NOT_HANDLED;
                    break;

                case IN_PROGRESS:
                    break;

                default:
                    break;
            }
            break;

         case GRABBER_DOWN_ATT:
            sub_action_act = ACT_get_last_action_result(ACT_QUEUE_Plate);
            switch(sub_action_act)
            {
                case ACT_FUNCTION_InProgress:
                    return IN_PROGRESS;
                    break;

                case ACT_FUNCTION_Done:
                    ACT_plate_plier(ACT_PLATE_PlierOpen);
                    state =GRABBER_OPEN_ATT;
                    break;

                case ACT_FUNCTION_ActDisabled:
                    ACT_plate_plier(ACT_PLATE_PlierOpen);
                    state=GRABBER_OPEN_ATT;
                    break;

                case ACT_FUNCTION_RetryLater:
                    ACT_plate_plier(ACT_PLATE_PlierOpen);
                   state=GRABBER_OPEN_ATT;
                    break;
                default:
                    state=POS_MOVE1;
                    return NOT_HANDLED;
                    break;
            }
            break;

         case GRABBER_OPEN_ATT:
            sub_action_act = ACT_get_last_action_result(ACT_QUEUE_Plate);
            switch(sub_action_act)
           {
                case ACT_FUNCTION_InProgress:
                    return IN_PROGRESS;
                    break;

                case ACT_FUNCTION_Done:
                    state =PUSH;
                    break;

                case ACT_FUNCTION_ActDisabled:
                    state=POS_MOVE1;
                    return NOT_HANDLED;
                    break;

                case ACT_FUNCTION_RetryLater:
                    state=DONE;
                    return END_WITH_TIMEOUT;
                    break;
                default:
                    state=POS_MOVE1;
                    return NOT_HANDLED;
                    break;
            }
            break;

        case PUSH:
            sub_action = goto_pos_with_scan_foe((displacement_t[]){{{(global.env.color == BLUE) ? 1430 : 1430,COLOR_Y(390)}}},1,BACKWARD,NO_AVOIDANCE);
            switch(sub_action)
            {
                case END_OK:
                    ACT_plate_rotate(ACT_PLATE_RotateUp);
                    state=GRABBER_UP_ATT;
                    break;

                case END_WITH_TIMEOUT:
                    ACT_plate_rotate(ACT_PLATE_RotateUp);
                    state=GRABBER_UP_ATT;
                    break;
                case NOT_HANDLED:
                    ACT_plate_rotate(ACT_PLATE_RotateUp);
                    state=GRABBER_UP_ATT;
                    break;

                case IN_PROGRESS:

                    if(COLOR_Y(global.env.pos.y)<500){
                        static Uint8 crush=0;
                        if(crush==0){
                           ACT_plate_plier(ACT_PLATE_PlierClose);
                           crush=1;
                        }

                     }
                    return IN_PROGRESS;
                     break;

                default:
                    return NOT_HANDLED;
                     break;
                    }
                  break;

            break;
         case GRABBER_UP_ATT:
            sub_action_act = ACT_get_last_action_result(ACT_QUEUE_Plate);
            switch(sub_action_act)
            {
                case ACT_FUNCTION_InProgress:
                    return IN_PROGRESS;
                    break;

                case ACT_FUNCTION_Done:
                    state=DONE;
                    return END_OK;
                    break;

                case ACT_FUNCTION_ActDisabled:
                    state=POS_MOVE1;
                    return NOT_HANDLED;
                    break;

                case ACT_FUNCTION_RetryLater:
                    state=DONE;
                    return END_WITH_TIMEOUT;
                    break;
                default:
                    state=POS_MOVE1;
                    return NOT_HANDLED;
                    break;
            }
            break;

        case DONE:
            return END_OK;
            break;
        default:
            state=POS_MOVE1;
            return NOT_HANDLED;
            break;
    }
    return IN_PROGRESS;
}

error_e TEST_STRAT_assiettes_evitement_5(void){

    static error_e sub_action;
    static ACT_function_result_e sub_action_act;
    static enum {
		POS_MOVE1 = 0,
                POS_MOVE2,
                GRABBER_MID_1_ATT,
                GRABBER_OPEN_1_ATT,
                GRABBER_DOWN_ATT,
		PUSH,
                GRABBER_UP_ATT,
                BACK,
                GRABBER_MID_ATT,
                GRABBER_OPEN2_ATT,
                GRABBER_CLOSE2_ATT,
                GRABBER_UP2_ATT,
                DONE,
    } state = POS_MOVE1;


    switch (state) {
         case POS_MOVE1:

            sub_action = goto_pos_with_scan_foe((displacement_t[]){{{(global.env.color == BLUE) ? 1768 : 1770,COLOR_Y(620)}}},1,FORWARD,NO_AVOIDANCE);
            switch(sub_action)
            {
                case END_OK:
                    state=POS_MOVE2;
                    break;

                case END_WITH_TIMEOUT:
                    state=POS_MOVE2;
                    break;
                case NOT_HANDLED:
                    state=POS_MOVE2;
                    break;

                case IN_PROGRESS:
                    return IN_PROGRESS;
                    break;

                default:
                    state=POS_MOVE2;
                    break;
            }
            break;
        case POS_MOVE2:

            sub_action = goto_angle(((global.env.color == BLUE) ? -PI4096/2 : PI4096/2), FAST);
            switch(sub_action)
            {
                case END_OK:
                    ACT_plate_rotate(ACT_PLATE_RotateMid);
                    state=GRABBER_MID_1_ATT;
                    break;

                case END_WITH_TIMEOUT:
                    state=DONE;
                    return END_WITH_TIMEOUT;
                    break;
                case NOT_HANDLED:
                    state=POS_MOVE1;
                    return NOT_HANDLED;
                    break;

                case IN_PROGRESS:
                    break;

                default:
                    break;
            }
            break;

        case GRABBER_MID_1_ATT:
            sub_action_act = ACT_get_last_action_result(ACT_QUEUE_Plate);
            switch(sub_action_act)
            {
                case ACT_FUNCTION_InProgress:
                    return IN_PROGRESS;
                    break;

                case ACT_FUNCTION_Done:
                    ACT_plate_plier(ACT_PLATE_PlierOpen);
                    state =GRABBER_OPEN_1_ATT;
                    break;

                case ACT_FUNCTION_ActDisabled:
                    state=POS_MOVE1;
                    return NOT_HANDLED;
                    break;

                case ACT_FUNCTION_RetryLater:
                    state=DONE;
                    return END_WITH_TIMEOUT;
                    break;
                default:
                    state=POS_MOVE1;
                    return NOT_HANDLED;
                    break;
            }
            break;

        case GRABBER_OPEN_1_ATT:
            sub_action_act = ACT_get_last_action_result(ACT_QUEUE_Plate);
            switch(sub_action_act)
           {
                case ACT_FUNCTION_InProgress:
                    return IN_PROGRESS;
                    break;

                case ACT_FUNCTION_Done:
                     ACT_plate_rotate(ACT_PLATE_RotateDown);
                    state = GRABBER_DOWN_ATT;
                    break;

                case ACT_FUNCTION_ActDisabled:
                    state=POS_MOVE1;
                    return NOT_HANDLED;
                    break;

                case ACT_FUNCTION_RetryLater:
                    state=DONE;
                    return END_WITH_TIMEOUT;
                    break;
                default:
                    state=POS_MOVE1;
                    return NOT_HANDLED;
                    break;
            }
            break;

         case GRABBER_DOWN_ATT:
            sub_action_act = ACT_get_last_action_result(ACT_QUEUE_Plate);
            switch(sub_action_act)
            {
                case ACT_FUNCTION_InProgress:
                    return IN_PROGRESS;
                    break;

                case ACT_FUNCTION_Done:
                    state =PUSH;
                    break;

                case ACT_FUNCTION_ActDisabled:
                    state=PUSH;
                    break;

                case ACT_FUNCTION_RetryLater:
                    state=PUSH;
                    break;
                default:
                    state=POS_MOVE1;
                    return NOT_HANDLED;
                    break;
            }
            break;

        case PUSH:
            sub_action = goto_pos_with_scan_foe((displacement_t[]){{{(global.env.color == BLUE) ? 1769 : 1770 ,COLOR_Y(390)}}},1,BACKWARD,NO_AVOIDANCE);
            switch(sub_action)
            {
                case END_OK:
                    ACT_plate_rotate(ACT_PLATE_RotateUp);
                    state=GRABBER_UP_ATT;
                    break;

                case END_WITH_TIMEOUT:
                    ACT_plate_rotate(ACT_PLATE_RotateUp);
                    state=GRABBER_UP_ATT;
                    break;
                case NOT_HANDLED:
                    ACT_plate_rotate(ACT_PLATE_RotateUp);
                    state=GRABBER_UP_ATT;
                    break;

                case IN_PROGRESS:

                    if(COLOR_Y(global.env.pos.y)<500){
                        static Uint8 crush=0;
                        if(crush==0){
                           ACT_plate_plier(ACT_PLATE_PlierClose);
                           crush=1;
                        }

                     }
                    return IN_PROGRESS;
                     break;

                default:
                    return NOT_HANDLED;
                     break;
                    }
                  break;

            break;
         case GRABBER_UP_ATT:
            sub_action_act = ACT_get_last_action_result(ACT_QUEUE_Plate);
            switch(sub_action_act)
            {
                case ACT_FUNCTION_InProgress:
                    return IN_PROGRESS;
                    break;

                case ACT_FUNCTION_Done:
                    state =BACK;
                    break;

                case ACT_FUNCTION_ActDisabled:
                    state=POS_MOVE1;
                    return NOT_HANDLED;
                    break;

                case ACT_FUNCTION_RetryLater:
                    state=DONE;
                    return END_WITH_TIMEOUT;
                    break;
                default:
                    state=POS_MOVE1;
                    return NOT_HANDLED;
                    break;
            }
            break;

        case BACK:

            sub_action = goto_pos_with_scan_foe((displacement_t[]){{{(global.env.color == BLUE) ? 1770 : 1770 ,COLOR_Y(620)}}},1,FORWARD,NO_DODGE_AND_NO_WAIT);
            switch(sub_action)
            {
                case END_OK:
                    ACT_plate_rotate(ACT_PLATE_RotateMid);
                    state=GRABBER_MID_ATT;
                    break;

                case END_WITH_TIMEOUT:
                    state=DONE;
                    return END_WITH_TIMEOUT;
                    break;
                case NOT_HANDLED:
                    state=POS_MOVE1;
                    return NOT_HANDLED;
                    break;

                case IN_PROGRESS:
                    return IN_PROGRESS;
                    break;

                default:
                    state=POS_MOVE1;
                    return NOT_HANDLED;
                    break;
            }
            break;

         case GRABBER_MID_ATT:
            sub_action_act = ACT_get_last_action_result(ACT_QUEUE_Plate);
            switch(sub_action_act)
            {
                case ACT_FUNCTION_InProgress:
                    return IN_PROGRESS;
                    break;

                case ACT_FUNCTION_Done:
                    ACT_plate_plier(ACT_PLATE_PlierOpen);
                    state =GRABBER_OPEN2_ATT;
                    break;

                case ACT_FUNCTION_ActDisabled:
                    state=POS_MOVE1;
                    return NOT_HANDLED;
                    break;

                case ACT_FUNCTION_RetryLater:
                    state=DONE;
                    return END_WITH_TIMEOUT;
                    break;
                default:
                    state=POS_MOVE1;
                    return NOT_HANDLED;
                    break;
            }
            break;

         case GRABBER_OPEN2_ATT:
            sub_action_act = ACT_get_last_action_result(ACT_QUEUE_Plate);
            switch(sub_action_act)
           {
                case ACT_FUNCTION_InProgress:
                    return IN_PROGRESS;
                    break;

                case ACT_FUNCTION_Done:
                    ACT_plate_plier(ACT_PLATE_PlierClose);
                    state =GRABBER_CLOSE2_ATT;
                    break;

                case ACT_FUNCTION_ActDisabled:
                    state=POS_MOVE1;
                    return NOT_HANDLED;
                    break;

                case ACT_FUNCTION_RetryLater:
                    state=DONE;
                    return END_WITH_TIMEOUT;
                    break;
                default:
                    state=POS_MOVE1;
                    return NOT_HANDLED;
                    break;
            }
            break;

         case GRABBER_CLOSE2_ATT:
            sub_action_act = ACT_get_last_action_result(ACT_QUEUE_Plate);
            switch(sub_action_act)
            {
                case ACT_FUNCTION_InProgress:
                    return IN_PROGRESS;
                    break;

                case ACT_FUNCTION_Done:
                    ACT_plate_rotate(ACT_PLATE_RotateUp);
                    state =GRABBER_UP2_ATT;
                    break;

                case ACT_FUNCTION_ActDisabled:
                    state=POS_MOVE1;
                    return NOT_HANDLED;
                    break;

                case ACT_FUNCTION_RetryLater:
                    state=DONE;
                    return END_WITH_TIMEOUT;
                    break;
                default:
                    state=POS_MOVE1;
                    return NOT_HANDLED;
                    break;
            }
            break;

         case GRABBER_UP2_ATT:
            sub_action_act = ACT_get_last_action_result(ACT_QUEUE_Plate);
            switch(sub_action_act)
            {
                case ACT_FUNCTION_InProgress:
                    return IN_PROGRESS;
                    break;

                case ACT_FUNCTION_Done:
                    state =DONE;
                    return END_OK;
                    break;

                case ACT_FUNCTION_ActDisabled:
                    state=POS_MOVE1;
                    return NOT_HANDLED;
                    break;

                case ACT_FUNCTION_RetryLater:
                    state=DONE;
                    return END_WITH_TIMEOUT;
                    break;
                default:
                    state=POS_MOVE1;
                    return NOT_HANDLED;
                    break;
            }
            break;


        case DONE:
            return END_OK;
            break;
        default:
            state=POS_MOVE1;
            return NOT_HANDLED;
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
        WAIT_BALL,
        WAIT_BALL_ATT,
        STOP_LAUNCH,
        STOP_LAUNCH_ATT,
        DONE
    } state = MOVE;

    switch(state){
        case MOVE :
            nb_ball=0;
            sub_action = goto_pos_with_scan_foe((displacement_t[]){{{(global.env.color == BLUE) ? 1015 : 990,COLOR_Y(700)}}},1,FORWARD,NO_DODGE_AND_NO_WAIT);
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
            sub_action = goto_angle(((global.env.color == BLUE) ? -9400 : -4150), FAST);
            switch(sub_action)
            {
                case END_OK:
                    state=WAIT_BALL;
                    break;

                case END_WITH_TIMEOUT:
                    state=DONE;
                    return END_WITH_TIMEOUT;
                    break;

                case NOT_HANDLED:
                    state=MOVE;
                    return NOT_HANDLED;

                case IN_PROGRESS:
                    return IN_PROGRESS;
                    break;

                default:
                    state=MOVE;
                    return NOT_HANDLED;
                    break;
            }
            break;

        case WAIT_BALL:
            nb_ball=nb_ball+1;


            ACT_ball_sorter_next_autoset_speed(6300);
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
                    if (nb_ball>=9) state=STOP_LAUNCH;
                    else state=WAIT_BALL;
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
            state = MOVE;
            return END_OK;
            break;
        default :
            state=MOVE;
            return NOT_HANDLED;

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
        WAIT_BALL,
        WAIT_BALL_ATT,
        STOP_LAUNCH,
        STOP_LAUNCH_ATT,
        DONE,
    } state = MOVE;

    switch(state){
        case MOVE :
            nb_ball=0;
            sub_action = goto_pos_with_scan_foe((displacement_t[]){{{1400 ,COLOR_Y(620)}}},1,FORWARD,NO_DODGE_AND_NO_WAIT);
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
            sub_action = goto_angle(((global.env.color == BLUE) ? -10549 : -3100), FAST);
            switch(sub_action)
            {
                case END_OK:
                    state = WAIT_BALL;
                    break;

                case END_WITH_TIMEOUT:
                    state=DONE;
                    return END_WITH_TIMEOUT;
                    break;

                case NOT_HANDLED:
                    state=MOVE;
                    return NOT_HANDLED;

                case IN_PROGRESS:
                    return IN_PROGRESS;
                    break;

                default:
                    state=DONE;
                    return NOT_HANDLED;
                    break;
            }
            break;


        case WAIT_BALL:
            nb_ball=nb_ball+1;


            ACT_ball_sorter_next_autoset_speed(5850);
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
                    if (nb_ball>=9) state=STOP_LAUNCH;
                    else state=WAIT_BALL;

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
            state = MOVE;
            return END_OK;
            break;
        default :
            state=MOVE;
            return NOT_HANDLED;
    }
    return IN_PROGRESS;

}

error_e TEST_Launcher_ball_mid_double(void){
    static Uint8 nb_ball=0;
    static error_e sub_action;
    static ACT_function_result_e sub_action_act;
    static enum {
        MOVE = 0,
        ANGLE,
        WAIT_BALL,
        WAIT_BALL_ATT,
        STOP_LAUNCH,
        STOP_LAUNCH_ATT,
        GRABBER_MID_ATT,
        GRABBER_OPEN2_ATT,
        GRABBER_CLOSE2_ATT,
        GRABBER_UP2_ATT,
        DONE
    } state = MOVE;

    switch(state){
        case MOVE :
            nb_ball=0;
            sub_action = goto_pos_with_scan_foe((displacement_t[]){{{(global.env.color == BLUE) ? 1015 : 990,COLOR_Y(700)}}},1,FORWARD,NO_DODGE_AND_NO_WAIT);
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
            sub_action = goto_angle(((global.env.color == BLUE) ? -9400 : -4150), FAST);
            switch(sub_action)
            {
                case END_OK:
                    state=WAIT_BALL;
                    break;

                case END_WITH_TIMEOUT:
                    state=DONE;
                    return END_WITH_TIMEOUT;
                    break;

                case NOT_HANDLED:
                    state=MOVE;
                    return NOT_HANDLED;

                case IN_PROGRESS:
                    return IN_PROGRESS;
                    break;

                default:
                    state=MOVE;
                    return NOT_HANDLED;
                    break;
            }
            break;

        case WAIT_BALL:
            nb_ball=nb_ball+1;


            ACT_ball_sorter_next_autoset_speed(6300);
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
                    if (nb_ball>=18) state=STOP_LAUNCH;
                    else state=WAIT_BALL;
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
                    ACT_plate_rotate(ACT_PLATE_RotateMid);
                    state = GRABBER_MID_ATT;
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

         case GRABBER_MID_ATT:
            sub_action_act = ACT_get_last_action_result(ACT_QUEUE_Plate);
            switch(sub_action_act)
            {
                case ACT_FUNCTION_InProgress:
                    return IN_PROGRESS;
                    break;

                case ACT_FUNCTION_Done:
                    ACT_plate_plier(ACT_PLATE_PlierOpen);
                    state =GRABBER_OPEN2_ATT;
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

         case GRABBER_OPEN2_ATT:
            sub_action_act = ACT_get_last_action_result(ACT_QUEUE_Plate);
            switch(sub_action_act)
           {
                case ACT_FUNCTION_InProgress:
                    return IN_PROGRESS;
                    break;

                case ACT_FUNCTION_Done:
                    ACT_plate_plier(ACT_PLATE_PlierClose);
                    state =GRABBER_CLOSE2_ATT;
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

         case GRABBER_CLOSE2_ATT:
            sub_action_act = ACT_get_last_action_result(ACT_QUEUE_Plate);
            switch(sub_action_act)
            {
                case ACT_FUNCTION_InProgress:
                    return IN_PROGRESS;
                    break;

                case ACT_FUNCTION_Done:
                    ACT_plate_rotate(ACT_PLATE_RotateUp);
                    state =GRABBER_UP2_ATT;
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

         case GRABBER_UP2_ATT:
            sub_action_act = ACT_get_last_action_result(ACT_QUEUE_Plate);
            switch(sub_action_act)
            {
                case ACT_FUNCTION_InProgress:
                    return IN_PROGRESS;
                    break;

                case ACT_FUNCTION_Done:
                    state =DONE;
                    return END_OK;
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
            state = MOVE;
            return END_OK;
            break;
        default :
            state=MOVE;
            return NOT_HANDLED;

    }
    return IN_PROGRESS;
}

error_e TEST_Launcher_ball_gateau_double(void){
    static Uint8 nb_ball=0;
    static error_e sub_action;
    static ACT_function_result_e sub_action_act;
    static enum {
       MOVE = 0,
        ANGLE,
        WAIT_BALL,
        WAIT_BALL_ATT,
        STOP_LAUNCH,
        STOP_LAUNCH_ATT,
               ANGLE_GRABBER,
        GRABBER_MID_ATT,
        GRABBER_OPEN2_ATT,
        GRABBER_CLOSE2_ATT,
        GRABBER_UP2_ATT,
        DONE
    } state = MOVE;

    switch(state){
        case MOVE :
            nb_ball=0;
            sub_action = goto_pos_with_scan_foe((displacement_t[]){{{1400 ,COLOR_Y(620)}}},1,FORWARD,NO_DODGE_AND_NO_WAIT);
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
            sub_action = goto_angle(((global.env.color == BLUE) ? -10549 : -3100), FAST);
            switch(sub_action)
            {
                case END_OK:
                    state = WAIT_BALL;
                    break;

                case END_WITH_TIMEOUT:
                    state=DONE;
                    return END_WITH_TIMEOUT;
                    break;

                case NOT_HANDLED:
                    state=MOVE;
                    return NOT_HANDLED;

                case IN_PROGRESS:
                    return IN_PROGRESS;
                    break;

                default:
                    state=DONE;
                    return NOT_HANDLED;
                    break;
            }
            break;


        case WAIT_BALL:
            nb_ball=nb_ball+1;


            ACT_ball_sorter_next_autoset_speed(5850);
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
                    if (nb_ball>=18) state=STOP_LAUNCH;
                    else state=WAIT_BALL;

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
                    state = ANGLE_GRABBER;
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
        case ANGLE_GRABBER:
                sub_action = goto_angle(((global.env.color == BLUE) ? -PI4096/2 : PI4096/2), FAST);
            switch(sub_action)
            {
                case END_OK:
                    ACT_plate_rotate(ACT_PLATE_RotateMid);
                    state =GRABBER_MID_ATT;
                    break;

                case END_WITH_TIMEOUT:
                    state=DONE;
                    return END_WITH_TIMEOUT;
                    break;

                case NOT_HANDLED:
                    state=MOVE;
                    return NOT_HANDLED;

                case IN_PROGRESS:
                    return IN_PROGRESS;
                    break;

                default:
                    state=DONE;
                    return NOT_HANDLED;
                    break;
            }
            break;
        case GRABBER_MID_ATT:
            sub_action_act = ACT_get_last_action_result(ACT_QUEUE_Plate);
            switch(sub_action_act)
            {
                case ACT_FUNCTION_InProgress:
                    return IN_PROGRESS;
                    break;

                case ACT_FUNCTION_Done:
                    ACT_plate_plier(ACT_PLATE_PlierOpen);
                    state =GRABBER_OPEN2_ATT;
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

         case GRABBER_OPEN2_ATT:
            sub_action_act = ACT_get_last_action_result(ACT_QUEUE_Plate);
            switch(sub_action_act)
           {
                case ACT_FUNCTION_InProgress:
                    return IN_PROGRESS;
                    break;

                case ACT_FUNCTION_Done:
                    ACT_plate_plier(ACT_PLATE_PlierClose);
                    state =GRABBER_CLOSE2_ATT;
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

         case GRABBER_CLOSE2_ATT:
            sub_action_act = ACT_get_last_action_result(ACT_QUEUE_Plate);
            switch(sub_action_act)
            {
                case ACT_FUNCTION_InProgress:
                    return IN_PROGRESS;
                    break;

                case ACT_FUNCTION_Done:
                    ACT_plate_rotate(ACT_PLATE_RotateUp);
                    state =GRABBER_UP2_ATT;
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

         case GRABBER_UP2_ATT:
            sub_action_act = ACT_get_last_action_result(ACT_QUEUE_Plate);
            switch(sub_action_act)
            {
                case ACT_FUNCTION_InProgress:
                    return IN_PROGRESS;
                    break;

                case ACT_FUNCTION_Done:
                    state =DONE;
                    return END_OK;
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
            state = MOVE;
            return END_OK;
            break;
        default :
            state=MOVE;
            return NOT_HANDLED;
    }
    return IN_PROGRESS;

}

/* ----------------------------------------------------------------------------- */
/* 							Before Belgique                    			 */
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
			sub_action = goto_pos_with_scan_foe((displacement_t[]){{{1000,COLOR_Y(800)}}}, 1, BACKWARD, NO_DODGE_AND_NO_WAIT);
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
			sub_action = goto_pos_with_scan_foe((displacement_t[]){{{1000,COLOR_Y(400)}}},1,BACKWARD,NORMAL_WAIT);
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
			sub_action = goto_pos(580,COLOR_Y(380),FAST,FORWARD,END_AT_BREAK);
			switch(sub_action){
				case IN_PROGRESS:
					break;
				case END_OK:
					state = PREMIER;
					break;
				case END_WITH_TIMEOUT:
					state = PREMIER;
					break;
				case FOE_IN_PATH:
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
							FAST}},3,BACKWARD,NO_AVOIDANCE);
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
				case FOE_IN_PATH:
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
				case FOE_IN_PATH:
				case NOT_HANDLED:
					state = TROIS;
					break;
			}
			break;
		case SECONDBIS:
			sub_action = goto_pos_with_scan_foe(
					(displacement_t[]){{{555, COLOR_Y(2400)},FAST}},
					1,BACKWARD,NO_AVOIDANCE);

			break;
		case TROIS:
			sub_action = goto_pos_with_scan_foe(
					(displacement_t[]){{{550, COLOR_Y(2200)},FAST},
					{{1000, COLOR_Y(2400)},FAST}},
							2,BACKWARD,NO_AVOIDANCE);
			switch(sub_action){
				case IN_PROGRESS:
					break;
				case END_OK:
					state = QUATRE;
					break;
				case END_WITH_TIMEOUT:
					state = QUATRE;
					break;
				case FOE_IN_PATH:
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
					case FOE_IN_PATH:
					case NOT_HANDLED:
						state = CINQ;
						break;
				}
			break;
			case CINQ:
				sub_action = goto_pos_with_scan_foe(
				(displacement_t[]){{{800, COLOR_Y(2400)},FAST},
					{{1200, COLOR_Y(2500)},FAST}},
							2,BACKWARD,NO_AVOIDANCE);
				switch(sub_action){
					case IN_PROGRESS:
						break;
					case END_OK:
						state = SIX;
						break;
					case END_WITH_TIMEOUT:
						state = SIX;
						break;
					case FOE_IN_PATH:
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
					case FOE_IN_PATH:
					case NOT_HANDLED:
						state = SEPT;
						break;
				}
				break;
                    case SEPT:
				sub_action = goto_pos_with_scan_foe(
					(displacement_t[]){{{1050, COLOR_Y(700)},FAST}},
					1,BACKWARD,NO_AVOIDANCE);
				switch(sub_action){
					case IN_PROGRESS:
						break;
					case END_OK:
						state = DONE;
						break;
					case END_WITH_TIMEOUT:
						state = DONE;
						break;
					case FOE_IN_PATH:
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


error_e K_push_half_row_glasses_HOMOLO(void){
	static enum{
		ASK_WARNER = 0,
		PUSH_ROW,
		BACK_ROW,
		DONE
	}state = ASK_WARNER;

	enum action_verre{
		WAIT_EVENT = 0,
		ACT_UP,
		WAIT_ACT_UP,
		WAIT_NEXT_EVENT,
		ACT_DOWN,
		WAIT_ACT_DOWN,
		ACT_UP_BIS,
		ACT_DONE
	};
	static enum action_verre lift_left = WAIT_EVENT;
	static enum action_verre lift_right = WAIT_EVENT;

	static error_e sub_action;
	static error_e act_left;
	static error_e act_right;
	static bool_e captor_ok;

	if(global.env.asser.reach_y){
		captor_ok = TRUE;
	}


	switch(state){
		case ASK_WARNER:
			ASSER_WARNER_arm_y(COLOR_Y(690));
			state = PUSH_ROW;
			break;

		case PUSH_ROW:
			sub_action = goto_pos_with_scan_foe(
					(displacement_t[]){{{975, COLOR_Y(500)},FAST},
					{{1000, COLOR_Y(790)},FAST},
					{{1070, COLOR_Y(1070)},FAST},
					{{1112, COLOR_Y(1310)},FAST},
					{{1040, COLOR_Y(1510)},FAST}},
					5,FORWARD,NO_DODGE_AND_NO_WAIT);

			switch(sub_action){
				case IN_PROGRESS:
					break;
				case END_OK:
					state = BACK_ROW;
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
					break;
			}
			break;

		case BACK_ROW:
			sub_action = goto_pos_with_scan_foe(
					(displacement_t[]){{{882, COLOR_Y(1370)},FAST},
					{{782, COLOR_Y(1145)},FAST},
					{{760, COLOR_Y(990)},SLOW}},
					3,FORWARD,NO_DODGE_AND_NO_WAIT);

					switch(sub_action){
				case IN_PROGRESS:
					break;
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
					break;
			}
			break;

		case DONE:
			return END_OK;
			break;
	}

	//Ascenceur gauche
	switch(lift_left){
		case WAIT_EVENT:
			if(captor_ok)
			{
				if(PORTBbits.RB5 && state == PUSH_ROW){
					lift_left = ACT_UP;
				}
			}
			break;

		case ACT_UP:
			ACT_lift_plier(ACT_LIFT_Left,ACT_LIFT_PlierClose);
			ACT_lift_translate(ACT_LIFT_Left,ACT_LIFT_TranslateUp);
			lift_left = WAIT_ACT_UP;
			break;

		case WAIT_ACT_UP:
			act_left = ACT_get_last_action_result(ACT_QUEUE_LiftLeft);
			switch(act_left){
				case IN_PROGRESS:
					break;
				case END_OK:
					lift_left = WAIT_NEXT_EVENT;
					break;
				case END_WITH_TIMEOUT:
					lift_left = ACT_DONE;
					break;
				case NOT_HANDLED:
					lift_left = WAIT_EVENT;
					break;
				default:
					break;
			}

		case WAIT_NEXT_EVENT:
			if(PORTBbits.RB5 && state == BACK_ROW){
				lift_left = ACT_DOWN;
			}
			break;

		case ACT_DOWN:
			ACT_lift_plier(ACT_LIFT_Left,ACT_LIFT_PLIER_OPEN);
			ACT_lift_translate(ACT_LIFT_Left,ACT_LIFT_TranslateDown);
			lift_left = WAIT_ACT_DOWN;
			break;

		case WAIT_ACT_DOWN:
			act_left = ACT_get_last_action_result(ACT_QUEUE_LiftLeft);
			switch(act_left){
				case IN_PROGRESS:
					break;
				case END_OK:
					lift_left = ACT_UP_BIS;
					break;
				case END_WITH_TIMEOUT:
					lift_left = ACT_DONE;
					break;
				case NOT_HANDLED:
					lift_left = ACT_DOWN;
					break;
				default:
					debug_printf("Fuyez cava peter!!");
					break;
			}
			break;

		case ACT_UP_BIS:
			ACT_lift_plier(ACT_LIFT_Left,ACT_LIFT_PlierClose);
			ACT_lift_translate(ACT_LIFT_Left,ACT_LIFT_TranslateUp);
			lift_left = ACT_DONE;
			break;

		case ACT_DONE:
			break;
	}

	//Ascenceur droite
	switch(lift_right){
		case WAIT_EVENT:
			if(captor_ok)
			{
				if(!PORTBbits.RB3 && state == PUSH_ROW){
					lift_right = ACT_UP;
				}
			}
			break;

		case ACT_UP:
			ACT_lift_plier(ACT_LIFT_Right,ACT_LIFT_PlierClose);
			ACT_lift_translate(ACT_LIFT_Right,ACT_LIFT_TranslateUp);
			lift_right = WAIT_ACT_UP;
			break;

		case WAIT_ACT_UP:
			act_right = ACT_get_last_action_result(ACT_QUEUE_LiftRight);
			switch(act_right){
				case IN_PROGRESS:
					break;
				case END_OK:
					lift_right = WAIT_NEXT_EVENT;
					break;
				case END_WITH_TIMEOUT:
					lift_right = ACT_DONE;
					break;
				case NOT_HANDLED:
					lift_right = WAIT_EVENT;
					break;
				default:
					debug_printf("Fuyez cava peter!!");
					break;
			}

		case WAIT_NEXT_EVENT:
			if(!PORTBbits.RB3 && state == BACK_ROW){
				lift_right = ACT_DOWN;
			}
			break;

		case ACT_DOWN:
			ACT_lift_plier(ACT_LIFT_Right,ACT_LIFT_PLIER_OPEN);
			ACT_lift_translate(ACT_LIFT_Right,ACT_LIFT_TranslateDown);
			lift_right = WAIT_ACT_DOWN;
			break;

		case WAIT_ACT_DOWN:
			act_right = ACT_get_last_action_result(ACT_QUEUE_LiftRight);
			switch(act_right){
				case IN_PROGRESS:
					break;
				case END_OK:
					lift_right = ACT_UP_BIS;
					break;
				case END_WITH_TIMEOUT:
					lift_right = ACT_DONE;
					break;
				case NOT_HANDLED:
					lift_right = ACT_DOWN;
					break;
				default:
					debug_printf("Fuyez cava peter!!");
					break;
			}
			break;

		case ACT_UP_BIS:
			ACT_lift_plier(ACT_LIFT_Right,ACT_LIFT_PlierClose);
			ACT_lift_translate(ACT_LIFT_Right,ACT_LIFT_TranslateUp);
			lift_right = ACT_DONE;
			break;

		case ACT_DONE:
			break;
	}


	return IN_PROGRESS;
}


error_e K_push_back_row_glasses_HOMOLO(void){
	static enum{
		PUSH_ROW = 0,
		BACK_ROW,
		LUCKY_LUKE,
                        LUCKY_LUKE2,
		DONE_AND_WAIT,
		EXTRACT,
		DONE
	}state = PUSH_ROW;

	enum action_verre{
		WAIT_EVENT = 0,
		ACT_DOWN,
		WAIT_ACT_DOWN,
		ACT_GRAB_ROW,
		ACT_OPEN,
		ACT_DONE
	};

	static enum action_verre lift_left = WAIT_EVENT;
	static enum action_verre lift_right = WAIT_EVENT;
	static error_e act_left;
	static error_e act_right;

	static error_e sub_action;

	switch(state){
		case PUSH_ROW:

			sub_action = goto_pos_with_scan_foe(
					(displacement_t[]){{{750, COLOR_Y(1280)},FAST}},
					1,BACKWARD,NO_DODGE_AND_NO_WAIT);

			switch(sub_action){
				case IN_PROGRESS:
					break;
				case END_OK:
					state = BACK_ROW;
					break;
				case NOT_HANDLED:

					state = LUCKY_LUKE2;
					break;
				case END_WITH_TIMEOUT:
					state = LUCKY_LUKE2;
					break;
				default:
					state = LUCKY_LUKE2;
					return NOT_HANDLED;
					break;
			}
			break;

		case BACK_ROW:
			sub_action = goto_pos_with_scan_foe(
					(displacement_t[]){{{615, COLOR_Y(1170)},FAST},
					{{505, COLOR_Y(910)},FAST}},
					2,FORWARD,NO_DODGE_AND_NO_WAIT);
			switch(sub_action){
				case IN_PROGRESS:
					break;
				case END_OK:
					state = LUCKY_LUKE;
					break;
				case NOT_HANDLED:
					state = LUCKY_LUKE2;
					break;
				case END_WITH_TIMEOUT:
					state = LUCKY_LUKE2;
					break;
				default:
					state = PUSH_ROW;
					return NOT_HANDLED;
					break;
			}
			break;

		case LUCKY_LUKE:
				sub_action = goto_pos_with_scan_foe(
					(displacement_t[]){{{740, COLOR_Y(555)}}},
					1,FORWARD,NO_DODGE_AND_NO_WAIT);
				switch(sub_action){
					case IN_PROGRESS:
						break;
					case END_OK:
						state = LUCKY_LUKE2;
						break;
					case NOT_HANDLED:
						state = LUCKY_LUKE2;
						break;
					case END_WITH_TIMEOUT:
						state = LUCKY_LUKE2;
						break;
					default:
						state = LUCKY_LUKE2;
						//return NOT_HANDLED;
						break;
				}

			break;
                        case LUCKY_LUKE2:
                            sub_action = goto_pos_with_scan_foe(
					(displacement_t[]){{{955, COLOR_Y(323)},FAST}},
					1,BACKWARD,NO_DODGE_AND_NO_WAIT);
				switch(sub_action){
					case IN_PROGRESS:
						break;
					case END_OK:
						state = DONE_AND_WAIT;
						break;
					case NOT_HANDLED:
						state = EXTRACT;
						break;
					case END_WITH_TIMEOUT:
						state = EXTRACT;
						break;
					default:
						state = PUSH_ROW;
						//return NOT_HANDLED;
						break;
				}

			break;

		case DONE_AND_WAIT:
			if(lift_left == ACT_DONE && lift_right == ACT_DONE )
				state = EXTRACT;
			break;

		case EXTRACT:
			sub_action = goto_pos_with_scan_foe(
					(displacement_t[]){{{800, COLOR_Y(500)},FAST}},
					1,BACKWARD,NO_DODGE_AND_NO_WAIT);

			switch(sub_action){
				case IN_PROGRESS:
					break;
				case END_OK:
				case NOT_HANDLED:
				case END_WITH_TIMEOUT:
					state = DONE;
					break;
				default:
					state = DONE;
					return NOT_HANDLED;
					break;
			}
			break;

		case DONE:
			return END_OK;
			break;

		default:
			break;
	}


/* ASCENCEUR gauche */
	switch(lift_left){
		case WAIT_EVENT:
			if(state == LUCKY_LUKE/*2*/)
				lift_left = ACT_OPEN;
			if(PORTBbits.RB5 && state == BACK_ROW){
				lift_left = ACT_DOWN;
			}
			break;

		case ACT_DOWN:
			ACT_lift_plier(ACT_LIFT_Left,ACT_LIFT_PLIER_OPEN);
			ACT_lift_translate(ACT_LIFT_Left,ACT_LIFT_TranslateDown);
			lift_left = WAIT_ACT_DOWN;

		case WAIT_ACT_DOWN:
			act_left = ACT_get_last_action_result(ACT_QUEUE_LiftLeft);
			switch(act_left){
				case IN_PROGRESS:
					break;
				case END_OK:
					lift_left = ACT_GRAB_ROW;
					ACT_lift_plier(ACT_LIFT_Left,ACT_LIFT_PlierClose);
					break;
				case END_WITH_TIMEOUT:
					lift_right = ACT_DONE;
					break;
				case NOT_HANDLED:
					lift_right = ACT_DOWN;
					break;
				default:
					debug_printf("Fuyez cava peter!!");
					break;
			}
			break;


		case ACT_GRAB_ROW:
			act_left = ACT_get_last_action_result(ACT_QUEUE_LiftLeft);
				switch(act_left){
				case IN_PROGRESS:
					break;
				case END_OK:
				case END_WITH_TIMEOUT:
				case NOT_HANDLED:
				default:
					lift_left = ACT_OPEN;
					break;
			}
			break;



		case ACT_OPEN:
			if(state == DONE_AND_WAIT){
				ACT_lift_plier(ACT_LIFT_Left,ACT_LIFT_PlierOpen);
				lift_left = ACT_DONE;
			}

		case ACT_DONE:
			break;

		default:
			break;
	}




	switch(lift_right){
		case WAIT_EVENT:
			if(state == LUCKY_LUKE)
				lift_right = ACT_OPEN;
			if(!PORTBbits.RB3 && state == BACK_ROW){
				lift_right = ACT_DOWN;
			}
			break;

		case ACT_DOWN:
			ACT_lift_plier(ACT_LIFT_Right,ACT_LIFT_PLIER_OPEN);
			ACT_lift_translate(ACT_LIFT_Right,ACT_LIFT_TranslateDown);
			lift_right = WAIT_ACT_DOWN;

		case WAIT_ACT_DOWN:
			act_right = ACT_get_last_action_result(ACT_QUEUE_LiftRight);
			switch(act_right){
				case IN_PROGRESS:
					break;
				case END_OK:
					ACT_lift_plier(ACT_LIFT_Right,ACT_LIFT_PlierClose);
					lift_right = ACT_GRAB_ROW;
					break;
				case END_WITH_TIMEOUT:
					lift_right = ACT_DONE;
					break;
				case NOT_HANDLED:
					lift_right = ACT_DOWN;
					break;
				default:
					debug_printf("Fuyez cava peter!!");
					break;
			}
			break;


		case ACT_GRAB_ROW:
			act_right = ACT_get_last_action_result(ACT_QUEUE_LiftRight);
			switch(act_right){
				case IN_PROGRESS:
					break;
				case END_OK:
				case END_WITH_TIMEOUT:
				case NOT_HANDLED:
				default:
					lift_right = ACT_OPEN;
					break;
			}
			break;

		case ACT_OPEN:
			if(state == DONE_AND_WAIT){
				ACT_lift_plier(ACT_LIFT_Right,ACT_LIFT_PlierOpen);
				lift_right = ACT_DONE;
			}
		case ACT_DONE:
			break;

		default:
			break;
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
            nb_ball=0;
            sub_action = goto_pos_with_scan_foe((displacement_t[]){{{(global.env.color == BLUE) ? 615 : 590,COLOR_Y(700)}}},1,FORWARD,NO_DODGE_AND_NO_WAIT);
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
            sub_action = goto_angle(((global.env.color == BLUE) ? -8505 : -4600), FAST);
            switch(sub_action)
            {
                case END_OK:
                    if (global.env.color_ball==0)state = LAUNCH_BALL_NORMAL;
                    else if (global.env.color_ball==1)state = LAUNCH_BALL_SLOW;
                    
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
                    state=DONE;
                    return NOT_HANDLED;
                    break;
            }
            break;

        case LAUNCH_BALL_NORMAL:
           ACT_ball_launcher_run((global.env.color == BLUE) ? 7100 : 7200);
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
                    if (nb_ball>=9) state=STOP_LAUNCH;
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
            state = MOVE;
            return END_OK;
            break;
        default :
            state=MOVE;
            return NOT_HANDLED;
            break;
    }
    return IN_PROGRESS;

}

error_e TEST_Launcher_ball_gateau2(void){
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
            nb_ball=0;
            sub_action = goto_pos_with_scan_foe((displacement_t[]){{{(global.env.color == BLUE) ? 1450 : 1340,COLOR_Y(700)}}},1,BACKWARD,NO_DODGE_AND_NO_WAIT);
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
            sub_action = goto_angle(((global.env.color == BLUE) ? -10000 : -3100), FAST);
            switch(sub_action)
            {
                case END_OK:
                   if (global.env.color_ball==0)state = LAUNCH_BALL_NORMAL;
                    else if (global.env.color_ball==1)state = LAUNCH_BALL_SLOW;
                    
                    break;

                case END_WITH_TIMEOUT:
                    state=DONE;
                    return END_WITH_TIMEOUT;
                    break;

                case NOT_HANDLED:
                    state=MOVE;
                    return NOT_HANDLED;

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
           ACT_ball_launcher_run((global.env.color == BLUE) ? 5850:5950);
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

                    if (nb_ball>=13) state=STOP_LAUNCH;
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
            state = MOVE;
            return END_OK;
            break;
        default :
            state=MOVE;
            return NOT_HANDLED;
    }
    return IN_PROGRESS;

}

error_e TEST_STRAT_assiettes_evitement_1(void){

    static error_e sub_action;
    static ACT_function_result_e sub_action_act;
    static enum {
				POS_MOVE1 = 0,
                POS_MOVE2,
                GRABBER_DOWN ,
                GRABBER_DOWN_ATT,
                GRABBER_OPEN,
                GRABBER_OPEN_ATT,
				PUSH,
				GRABBER_UP,
                GRABBER_UP_ATT,
                BACK,
                GRABBER_MID,
                GRABBER_MID_ATT,
                GRABBER_OPEN2,
                GRABBER_OPEN2_ATT,
                GRABBER_CLOSE2,
                GRABBER_CLOSE2_ATT,
                GRABBER_UP2,
                GRABBER_UP2_ATT,
                DONE,
    } state = POS_MOVE1;

//    static bool_e timeout = FALSE;

    switch (state) {
         case POS_MOVE1:

            sub_action = goto_pos_with_scan_foe((displacement_t[]){{{(global.env.color == BLUE) ?  265: 240,COLOR_Y(800)}}},1,FORWARD,NO_DODGE_AND_NO_WAIT);
            switch(sub_action)
            {
                case END_OK:
                    state=POS_MOVE2;
                    break;

                case END_WITH_TIMEOUT:
                    state=DONE;
                    return END_WITH_TIMEOUT;
                    break;
                case NOT_HANDLED:
                    state=POS_MOVE1;
                    return NOT_HANDLED;
                    break;

                case IN_PROGRESS:
                    return IN_PROGRESS;
                    break;

                default:
                    return NOT_HANDLED;
                    state=POS_MOVE1;
                    break;
            }
            break;
        case POS_MOVE2:

            sub_action = goto_pos_with_scan_foe((displacement_t[]){{{(global.env.color == BLUE) ? 265: 240,COLOR_Y(700)}}},1,BACKWARD,NO_AVOIDANCE);
            switch(sub_action)
            {
                case END_OK:
                    ACT_plate_rotate(ACT_PLATE_RotateDown);
                    state=GRABBER_DOWN;
                    break;

                case END_WITH_TIMEOUT:
                    state=DONE;
                    return END_WITH_TIMEOUT;
                    break;
                case NOT_HANDLED:
                    ACT_plate_rotate(ACT_PLATE_RotateDown);
                    state=GRABBER_DOWN;
                    return NOT_HANDLED;
                    break;

                case IN_PROGRESS:
                    break;

                default:
                    break;
            }
            break;

        case GRABBER_DOWN:

            
            state =GRABBER_DOWN_ATT;
            break;
         case GRABBER_DOWN_ATT:
            sub_action_act = ACT_get_last_action_result(ACT_QUEUE_Plate);
            switch(sub_action_act)
            {
                case ACT_FUNCTION_InProgress:
                    return IN_PROGRESS;
                    break;

                case ACT_FUNCTION_Done:
                    ACT_plate_plier(ACT_PLATE_PlierOpen);
                    state =GRABBER_OPEN;
                    break;

                case ACT_FUNCTION_ActDisabled:
                    state=POS_MOVE1;
                    return NOT_HANDLED;
                    break;

                case ACT_FUNCTION_RetryLater:
                    state=DONE;
                    return END_WITH_TIMEOUT;
                    break;
                default:
                    state=POS_MOVE1;
                    return NOT_HANDLED;
                    break;
            }
            break;



        case GRABBER_OPEN:

            state =GRABBER_OPEN_ATT;
            break;
         case GRABBER_OPEN_ATT:
            sub_action_act = ACT_get_last_action_result(ACT_QUEUE_Plate);
            switch(sub_action_act)
           {
                case ACT_FUNCTION_InProgress:
                    return IN_PROGRESS;
                    break;

                case ACT_FUNCTION_Done:
                    state =PUSH;
                    break;

                case ACT_FUNCTION_ActDisabled:
                    state=POS_MOVE1;
                    return NOT_HANDLED;
                    break;

                case ACT_FUNCTION_RetryLater:
                    state=DONE;
                    return END_WITH_TIMEOUT;
                    break;
                default:
                    state=POS_MOVE1;
                    return NOT_HANDLED;
                    break;
            }
            break;






        case PUSH:

            sub_action = goto_pos_with_scan_foe((displacement_t[]){{{(global.env.color == BLUE) ? 265: 240,COLOR_Y(390)}}},1,BACKWARD,NO_AVOIDANCE);
            //sub_action= TEST_STRAT_in_da_wall();
            switch(sub_action)
            {
                case END_OK:
                    ACT_plate_rotate(ACT_PLATE_RotateUp);
                    state=GRABBER_UP;
                    break;

                case END_WITH_TIMEOUT:
                    state=BACK;
                    break;
                case NOT_HANDLED:
                    state=POS_MOVE1;
                    return NOT_HANDLED;
                    break;

                case IN_PROGRESS:

                    if(global.env.pos.y<580){
                        static Uint8 crush=0;
                        if(crush==0){
                           ACT_plate_plier(ACT_PLATE_PlierClose);
                           crush=1;
                        }

                     }
                    return IN_PROGRESS;
                     break;

                default:
                    return NOT_HANDLED;
                     break;
                    }
                  break;

        case GRABBER_UP:

            
            state =GRABBER_UP_ATT;
            break;
         case GRABBER_UP_ATT:
            sub_action_act = ACT_get_last_action_result(ACT_QUEUE_Plate);
            switch(sub_action_act)
            {
                case ACT_FUNCTION_InProgress:
                    return IN_PROGRESS;
                    break;

                case ACT_FUNCTION_Done:
                    state =BACK;
                    break;

                case ACT_FUNCTION_ActDisabled:
                    state=POS_MOVE1;
                    return NOT_HANDLED;
                    break;

                case ACT_FUNCTION_RetryLater:
                    state=DONE;
                    return END_WITH_TIMEOUT;
                    break;
                default:
                    state=POS_MOVE1;
                    return NOT_HANDLED;
                    break;
            }
            break;

        case BACK:

            sub_action = goto_pos_with_scan_foe((displacement_t[]){{{(global.env.color == BLUE) ? 265 : 240,COLOR_Y(700)}}},1,FORWARD,NO_DODGE_AND_NO_WAIT);
            switch(sub_action)
            {
                case END_OK:
                    ACT_plate_rotate(ACT_PLATE_RotateMid);
                    state=GRABBER_MID;
                    break;

                case END_WITH_TIMEOUT:
                    state=DONE;
                    return END_WITH_TIMEOUT;
                    break;
                case NOT_HANDLED:
                    state=POS_MOVE1;
                    return NOT_HANDLED;
                    break;

                case IN_PROGRESS:
                    return IN_PROGRESS;
                    break;

                default:
                    state=POS_MOVE1;
                    return NOT_HANDLED;
                    break;
            }
            break;

        case GRABBER_MID:

            
            state =GRABBER_MID_ATT;
            break;
         case GRABBER_MID_ATT:
            sub_action_act = ACT_get_last_action_result(ACT_QUEUE_Plate);
            switch(sub_action_act)
            {
                case ACT_FUNCTION_InProgress:
                    return IN_PROGRESS;
                    break;

                case ACT_FUNCTION_Done:
                    state =GRABBER_OPEN2;
                    break;

                case ACT_FUNCTION_ActDisabled:
                    state=POS_MOVE1;
                    return NOT_HANDLED;
                    break;

                case ACT_FUNCTION_RetryLater:
                    state=DONE;
                    return END_WITH_TIMEOUT;
                    break;
                default:
                    state=POS_MOVE1;
                    return NOT_HANDLED;
                    break;
            }
            break;


        case GRABBER_OPEN2:

            ACT_plate_plier(ACT_PLATE_PlierOpen);
            state =GRABBER_OPEN2_ATT;
            break;
         case GRABBER_OPEN2_ATT:
            sub_action_act = ACT_get_last_action_result(ACT_QUEUE_Plate);
            switch(sub_action_act)
           {
                case ACT_FUNCTION_InProgress:
                    return IN_PROGRESS;
                    break;

                case ACT_FUNCTION_Done:
                    state =GRABBER_CLOSE2;
                    break;

                case ACT_FUNCTION_ActDisabled:
                    state=POS_MOVE1;
                    return NOT_HANDLED;
                    break;

                case ACT_FUNCTION_RetryLater:
                    state=DONE;
                    return END_WITH_TIMEOUT;
                    break;
                default:
                    state=POS_MOVE1;
                    return NOT_HANDLED;
                    break;
            }
            break;

         case GRABBER_CLOSE2:

            ACT_plate_plier(ACT_PLATE_PlierClose);
            state =GRABBER_CLOSE2_ATT;
            break;
         case GRABBER_CLOSE2_ATT:
            sub_action_act = ACT_get_last_action_result(ACT_QUEUE_Plate);
            switch(sub_action_act)
            {
                case ACT_FUNCTION_InProgress:
                    return IN_PROGRESS;
                    break;

                case ACT_FUNCTION_Done:
                    state =GRABBER_UP2;
                    break;

                case ACT_FUNCTION_ActDisabled:
                    state=POS_MOVE1;
                    return NOT_HANDLED;
                    break;

                case ACT_FUNCTION_RetryLater:
                    state=DONE;
                    return END_WITH_TIMEOUT;
                    break;
                default:
                    state=POS_MOVE1;
                    return NOT_HANDLED;
                    break;
            }
            break;

        case GRABBER_UP2:

            ACT_plate_rotate(ACT_PLATE_RotateUp);
            state =GRABBER_UP2_ATT;
            break;
         case GRABBER_UP2_ATT:
            sub_action_act = ACT_get_last_action_result(ACT_QUEUE_Plate);
            switch(sub_action_act)
            {
                case ACT_FUNCTION_InProgress:
                    return IN_PROGRESS;
                    break;

                case ACT_FUNCTION_Done:
                    state =DONE;
                    break;

                case ACT_FUNCTION_ActDisabled:
                    state=POS_MOVE1;
                    return NOT_HANDLED;
                    break;

                case ACT_FUNCTION_RetryLater:
                    state=DONE;
                    return END_WITH_TIMEOUT;
                    break;
                default:
                    state=POS_MOVE1;
                    return NOT_HANDLED;
                    break;
            }
            break;


        case DONE:
            return END_OK;
            break;
        default:
            state=POS_MOVE1;
            return NOT_HANDLED;
            break;
    }
    return IN_PROGRESS;
}

error_e TEST_STRAT_assiettes_evitement_3(void){

    static error_e sub_action;
    static ACT_function_result_e sub_action_act;
    static enum {
				POS_MOVE1 = 0,
                POS_MOVE2,
                GRABBER_DOWN ,
                GRABBER_DOWN_ATT,
                GRABBER_OPEN,
                GRABBER_OPEN_ATT,
				PUSH,
				GRABBER_UP,
                GRABBER_UP_ATT,
                BACK,
                GRABBER_MID,
                GRABBER_MID_ATT,
                GRABBER_OPEN2,
                GRABBER_OPEN2_ATT,
                GRABBER_CLOSE2,
                GRABBER_CLOSE2_ATT,
                GRABBER_UP2,
                GRABBER_UP2_ATT,
                DONE,
    } state = POS_MOVE1;

//    static bool_e timeout = FALSE;

    switch (state) {
         case POS_MOVE1:

            sub_action = goto_pos_with_scan_foe((displacement_t[]){{{(global.env.color == BLUE) ? 1015 : 990 ,COLOR_Y(800)}}},1,FORWARD,NO_DODGE_AND_NO_WAIT);
            switch(sub_action)
            {
                case END_OK:
                    state=POS_MOVE2;
                    break;

                case END_WITH_TIMEOUT:
                    state=DONE;
                    return END_WITH_TIMEOUT;
                    break;
                case NOT_HANDLED:
                    state=POS_MOVE1;
                    return NOT_HANDLED;
                    break;

                case IN_PROGRESS:
                    return IN_PROGRESS;
                    break;

                default:
                    return NOT_HANDLED;
                    state=POS_MOVE1;
                    break;
            }
            break;
        case POS_MOVE2:

            sub_action = goto_pos_with_scan_foe((displacement_t[]){{{(global.env.color == BLUE) ? 1015 : 990,COLOR_Y(700)}}},1,BACKWARD,NO_AVOIDANCE);
            switch(sub_action)
            {
                case END_OK:
                    ACT_plate_rotate(ACT_PLATE_RotateDown);
                    state=GRABBER_DOWN;
                    break;

                case END_WITH_TIMEOUT:
                    state=DONE;
                    return END_WITH_TIMEOUT;
                    break;
                case NOT_HANDLED:
                    state=POS_MOVE1;
                    return NOT_HANDLED;
                    break;

                case IN_PROGRESS:
                    break;

                default:
                    break;
            }
            break;

        case GRABBER_DOWN:

            
            state =GRABBER_DOWN_ATT;
            break;
         case GRABBER_DOWN_ATT:
            sub_action_act = ACT_get_last_action_result(ACT_QUEUE_Plate);
            switch(sub_action_act)
            {
                case ACT_FUNCTION_InProgress:
                    return IN_PROGRESS;
                    break;

                case ACT_FUNCTION_Done:
                    state =GRABBER_OPEN;
                    break;

                case ACT_FUNCTION_ActDisabled:
                    state=POS_MOVE1;
                    return NOT_HANDLED;
                    break;

                case ACT_FUNCTION_RetryLater:
                    state=DONE;
                    return END_WITH_TIMEOUT;
                    break;
                default:
                    state=POS_MOVE1;
                    return NOT_HANDLED;
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
                    return IN_PROGRESS;
                    break;

                case ACT_FUNCTION_Done:
                    state =PUSH;
                    break;

                case ACT_FUNCTION_ActDisabled:
                    state=POS_MOVE1;
                    return NOT_HANDLED;
                    break;

                case ACT_FUNCTION_RetryLater:
                    state=DONE;
                    return END_WITH_TIMEOUT;
                    break;
                default:
                    state=POS_MOVE1;
                    return NOT_HANDLED;
                    break;
            }
            break;






        case PUSH:

            sub_action = goto_pos_with_scan_foe((displacement_t[]){{{(global.env.color == BLUE) ? 1015 : 990,COLOR_Y(390)}}},1,BACKWARD,NO_AVOIDANCE);
            //sub_action= TEST_STRAT_in_da_wall();
            switch(sub_action)
            {
                case END_OK:
                    ACT_plate_rotate(ACT_PLATE_RotateUp);
                    state=GRABBER_UP;
                    break;

                case END_WITH_TIMEOUT:
                    state=BACK;
                    break;
                case NOT_HANDLED:
                    state=POS_MOVE1;
                    return NOT_HANDLED;
                    break;

                case IN_PROGRESS:

                    if(global.env.pos.y<580){
                        static Uint8 crush=0;
                        if(crush==0){
                           ACT_plate_plier(ACT_PLATE_PlierClose);
                           crush=1;
                        }

                     }
                    return IN_PROGRESS;
                     break;

                default:
                    return NOT_HANDLED;
                     break;
                    }
                  break;

        case GRABBER_UP:

            
            state =GRABBER_UP_ATT;
            break;
         case GRABBER_UP_ATT:
            sub_action_act = ACT_get_last_action_result(ACT_QUEUE_Plate);
            switch(sub_action_act)
            {
                case ACT_FUNCTION_InProgress:
                    return IN_PROGRESS;
                    break;

                case ACT_FUNCTION_Done:
                    state =BACK;
                    break;

                case ACT_FUNCTION_ActDisabled:
                    state=POS_MOVE1;
                    return NOT_HANDLED;
                    break;

                case ACT_FUNCTION_RetryLater:
                    state=DONE;
                    return END_WITH_TIMEOUT;
                    break;
                default:
                    state=POS_MOVE1;
                    return NOT_HANDLED;
                    break;
            }
            break;

        case BACK:

            sub_action = goto_pos_with_scan_foe((displacement_t[]){{{(global.env.color == BLUE) ? 1015 : 990,COLOR_Y(700)}}},1,FORWARD,NO_DODGE_AND_NO_WAIT);
            switch(sub_action)
            {
                case END_OK:
                    ACT_plate_rotate(ACT_PLATE_RotateMid);
                    state=GRABBER_MID;
                    break;

                case END_WITH_TIMEOUT:
                    state=DONE;
                    return END_WITH_TIMEOUT;
                    break;
                case NOT_HANDLED:
                    state=POS_MOVE1;
                    return NOT_HANDLED;
                    break;

                case IN_PROGRESS:
                    return IN_PROGRESS;
                    break;

                default:
                    state=POS_MOVE1;
                    return NOT_HANDLED;
                    break;
            }
            break;

        case GRABBER_MID:

            
            state =GRABBER_MID_ATT;
            break;
         case GRABBER_MID_ATT:
            sub_action_act = ACT_get_last_action_result(ACT_QUEUE_Plate);
            switch(sub_action_act)
            {
                case ACT_FUNCTION_InProgress:
                    return IN_PROGRESS;
                    break;

                case ACT_FUNCTION_Done:
                    state =GRABBER_OPEN2;
                    break;

                case ACT_FUNCTION_ActDisabled:
                    state=POS_MOVE1;
                    return NOT_HANDLED;
                    break;

                case ACT_FUNCTION_RetryLater:
                    state=DONE;
                    return END_WITH_TIMEOUT;
                    break;
                default:
                    state=POS_MOVE1;
                    return NOT_HANDLED;
                    break;
            }
            break;


        case GRABBER_OPEN2:

            ACT_plate_plier(ACT_PLATE_PlierOpen);
            state =GRABBER_OPEN2_ATT;
            break;
         case GRABBER_OPEN2_ATT:
            sub_action_act = ACT_get_last_action_result(ACT_QUEUE_Plate);
            switch(sub_action_act)
           {
                case ACT_FUNCTION_InProgress:
                    return IN_PROGRESS;
                    break;

                case ACT_FUNCTION_Done:
                    state =GRABBER_CLOSE2;
                    break;

                case ACT_FUNCTION_ActDisabled:
                    state=POS_MOVE1;
                    return NOT_HANDLED;
                    break;

                case ACT_FUNCTION_RetryLater:
                    state=DONE;
                    return END_WITH_TIMEOUT;
                    break;
                default:
                    state=POS_MOVE1;
                    return NOT_HANDLED;
                    break;
            }
            break;

         case GRABBER_CLOSE2:

            ACT_plate_plier(ACT_PLATE_PlierClose);
            state =GRABBER_CLOSE2_ATT;
            break;
         case GRABBER_CLOSE2_ATT:
            sub_action_act = ACT_get_last_action_result(ACT_QUEUE_Plate);
            switch(sub_action_act)
            {
                case ACT_FUNCTION_InProgress:
                    return IN_PROGRESS;
                    break;

                case ACT_FUNCTION_Done:
                    state =GRABBER_UP2;
                    break;

                case ACT_FUNCTION_ActDisabled:
                    state=POS_MOVE1;
                    return NOT_HANDLED;
                    break;

                case ACT_FUNCTION_RetryLater:
                    state=DONE;
                    return END_WITH_TIMEOUT;
                    break;
                default:
                    state=POS_MOVE1;
                    return NOT_HANDLED;
                    break;
            }
            break;

        case GRABBER_UP2:

            ACT_plate_rotate(ACT_PLATE_RotateUp);
            state =GRABBER_UP2_ATT;
            break;
         case GRABBER_UP2_ATT:
            sub_action_act = ACT_get_last_action_result(ACT_QUEUE_Plate);
            switch(sub_action_act)
            {
                case ACT_FUNCTION_InProgress:
                    return IN_PROGRESS;
                    break;

                case ACT_FUNCTION_Done:
                    state =DONE;
                    break;

                case ACT_FUNCTION_ActDisabled:
                    state=POS_MOVE1;
                    return NOT_HANDLED;
                    break;

                case ACT_FUNCTION_RetryLater:
                    state=DONE;
                    return END_WITH_TIMEOUT;
                    break;
                default:
                    state=POS_MOVE1;
                    return NOT_HANDLED;
                    break;
            }
            break;


        case DONE:
            return END_OK;
            break;
        default:
            state=POS_MOVE1;
            return NOT_HANDLED;
            break;
    }
    return IN_PROGRESS;
}
/* ----------------------------------------------------------------------------- */
/* 								Fonction diverses                     			 */
/* ----------------------------------------------------------------------------- */


/* ----------------------------------------------------------------------------- */
/* 							Découpe de stratégies                     			 */
/* ----------------------------------------------------------------------------- */
