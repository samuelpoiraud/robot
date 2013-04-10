/*
 *	Club Robot ESEO 2008 - 2010
 *	Archi'Tech, CHOMP, CheckNorris, 
 *
 *	Fichier : actions.c
 *	Package : Carte Principale
 *	Description : Actions réalisables par le robot sur commande de brain.c
 *	Auteur : Julien
 *	Version 20110313
 */

#define ACTIONS_C
#include "actions.h"

#define DEFAULT_SPEED FAST


//ATTENTION: Fonction codé dégueu pour la coupe de Lannion Totalement linéaire et sans évitement.
void STRAT_Lannion(void)
	{
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
                        GO_POS,
        GRABBER_TIDY,
        MOVE_PUSH,
        MOVE_BACK,
                        GO_POS2,
                        MOVE_PUSH2,
                        MOVE_BACK2,
                        GO_POS3,
                        MOVE_PUSH3,
                        MOVE_BACK3,
                        GO_POS4,
                        GO_POS4bis,
                        MOVE_PUSH4,
                        MOVE_BACK4,
                        GRABBER_UP,
                        GO_POS5,
                        GO_POS5bis,
                        GO_POS5final,
				DONE,
	}state = SORTIR;

	//static bool_e timeout;
	static error_e sub_action;


	switch(state){
		case SORTIR:
			sub_action = goto_pos(580,COLOR_Y(380),FAST,FORWARD,END_AT_LAST_POINT);
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
					break;
				case END_OK:
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
					1,BACKWARD,NO_AVOIDANCE);
				switch(sub_action){
					case IN_PROGRESS:
						break;
					case END_OK:
						state = GO_POS;
						break;
					case END_WITH_TIMEOUT:
						state = GO_POS;
						break;
					case NOT_HANDLED:
						state = GO_POS;
						break;
				}
				break;
                    case GO_POS:
                        sub_action = goto_pos_with_scan_foe((displacement_t[]){{{600,COLOR_Y(1080)}}},1,FORWARD,NORMAL_WAIT);
                        switch(sub_action)
            {
                case END_OK:
                    state=GRABBER_TIDY;
                    break;

                case END_WITH_TIMEOUT:
                    state=GRABBER_TIDY;
                    break;

                case NOT_HANDLED:
                    state=GRABBER_TIDY;

                    break;

                case IN_PROGRESS:
                    break;

                default:

                    break;
            }
            break;

        case GRABBER_TIDY:
            //ACT_push_plate_rotate_prepare(TRUE);
			

            state = MOVE_PUSH;

        case MOVE_PUSH:
            sub_action = goto_pos_with_scan_foe((displacement_t[]){{{260,COLOR_Y(1080)}}},1,BACKWARD,NORMAL_WAIT);

            switch(sub_action)
            {
                case END_OK:
                    state=MOVE_BACK;
                    break;

                case END_WITH_TIMEOUT:
                    state=MOVE_BACK;
                    break;

                case NOT_HANDLED:
                    state = MOVE_BACK;

                    break;

                case IN_PROGRESS:
                    break;

                default:

                    break;
            }
            break;
        case MOVE_BACK:
            sub_action = goto_pos_with_scan_foe((displacement_t[]){{{600,COLOR_Y(1080)}}},1,FORWARD,NORMAL_WAIT);

            switch(sub_action)
            {
                case END_OK:
                    state=GO_POS2;
                    break;

                case END_WITH_TIMEOUT:
                    state=GO_POS2;
                    break;

                case NOT_HANDLED:
                    state = GO_POS2;

                    break;

                case IN_PROGRESS:
                    break;

                default:
                    break;
            }
            break;
        case GO_POS2:
            sub_action = goto_pos_with_scan_foe((displacement_t[]){{{600,COLOR_Y(1680)}}},1,FORWARD,NORMAL_WAIT);
            switch(sub_action)
            {
                case END_OK:
                    state=MOVE_PUSH2;
                    break;

                case END_WITH_TIMEOUT:
                    state=MOVE_PUSH2;
                    break;

                case NOT_HANDLED:
                    state=MOVE_PUSH2;

                    break;

                case IN_PROGRESS:
                    break;

                default:

                    break;
            }
            break;
         case MOVE_PUSH2:
            sub_action = goto_pos_with_scan_foe((displacement_t[]){{{260,COLOR_Y(1680)}}},1,BACKWARD,NORMAL_WAIT);

            switch(sub_action)
            {
                case END_OK:
                    state=MOVE_BACK2;
                    break;

                case END_WITH_TIMEOUT:
                    state=MOVE_BACK2;
                    break;

                case NOT_HANDLED:
                    state = MOVE_BACK2;

                    break;

                case IN_PROGRESS:
                    break;

                default:

                    break;
            }
            break;
        case MOVE_BACK2:
            sub_action = goto_pos_with_scan_foe((displacement_t[]){{{600,COLOR_Y(1680)}}},1,FORWARD,NORMAL_WAIT);

            switch(sub_action)
            {
                case END_OK:
                    state=GO_POS3;
                    break;

                case END_WITH_TIMEOUT:
                    state=GO_POS3;
                    break;

                case NOT_HANDLED:
                    state = GO_POS3;

                    break;

                case IN_PROGRESS:
                    break;

                default:
                    break;
            }
            break;
        case GO_POS3:
            sub_action = goto_pos_with_scan_foe((displacement_t[]){{{600,COLOR_Y(2280)}}},1,FORWARD,NORMAL_WAIT);
            switch(sub_action)
            {
                case END_OK:
                    state=MOVE_PUSH3;
                    break;

                case END_WITH_TIMEOUT:
                    state=MOVE_PUSH3;
                    break;

                case NOT_HANDLED:
                    state=MOVE_PUSH3;

                    break;

                case IN_PROGRESS:
                    break;

                default:

                    break;
            }
            break;
         case MOVE_PUSH3:
            sub_action = goto_pos_with_scan_foe((displacement_t[]){{{260,COLOR_Y(2280)}}},1,BACKWARD,NORMAL_WAIT);

            switch(sub_action)
            {
                case END_OK:
                    state=MOVE_BACK3;
                    break;

                case END_WITH_TIMEOUT:
                    state=MOVE_BACK3;
                    break;

                case NOT_HANDLED:
                    state = MOVE_BACK3;

                    break;

                case IN_PROGRESS:
                    break;

                default:

                    break;
            }
            break;
        case MOVE_BACK3:
            sub_action = goto_pos_with_scan_foe((displacement_t[]){{{600,COLOR_Y(2280)}}},1,FORWARD,NORMAL_WAIT);

            switch(sub_action)
            {
                case END_OK:
                    state=GO_POS4;
                    break;

                case END_WITH_TIMEOUT:
                    state=GO_POS4;
                    break;

                case NOT_HANDLED:
                    state = GO_POS4;

                    break;

                case IN_PROGRESS:
                    break;

                default:
                    break;
            }
            break;
        case GO_POS4:
            sub_action = goto_pos_with_scan_foe((displacement_t[]){{{600,COLOR_Y(400)}}},1,FORWARD,NORMAL_WAIT);
            switch(sub_action)
            {
                case END_OK:
                    state=GO_POS4bis;
                    break;

                case END_WITH_TIMEOUT:
                    state=GO_POS4bis;
                    break;

                case NOT_HANDLED:
                    state=GO_POS4bis;

                    break;

                case IN_PROGRESS:
                    break;

                default:

                    break;
            }
            break;
         case GO_POS4bis:
            sub_action = goto_pos_with_scan_foe((displacement_t[]){{{600,COLOR_Y(490)}}},1,BACKWARD,NORMAL_WAIT);
            switch(sub_action)
            {
                case END_OK:
                    state=MOVE_PUSH4;
                    break;

                case END_WITH_TIMEOUT:
                    state=MOVE_PUSH4;
                    break;

                case NOT_HANDLED:
                    state=MOVE_PUSH4;

                    break;

                case IN_PROGRESS:
                    break;

                default:

                    break;
            }
            break;
         case MOVE_PUSH4:
            sub_action = goto_pos_with_scan_foe((displacement_t[]){{{260,COLOR_Y(490)}}},1,BACKWARD,NORMAL_WAIT);

            switch(sub_action)
            {
                case END_OK:
                    state=MOVE_BACK4;
                    break;

                case END_WITH_TIMEOUT:
                    state=MOVE_BACK4;
                    break;

                case NOT_HANDLED:
                    state = MOVE_BACK4;

                    break;

                case IN_PROGRESS:
                    break;

                default:

                    break;
            }
            break;
        case MOVE_BACK4:
            sub_action = goto_pos_with_scan_foe((displacement_t[]){{{600,COLOR_Y(490)}}},1,FORWARD,NORMAL_WAIT);

            switch(sub_action)
            {
                case END_OK:
                    state=GRABBER_UP;
                    break;

                case END_WITH_TIMEOUT:
                    state=GRABBER_UP;
                    break;

                case NOT_HANDLED:
                    state = GRABBER_UP;

                    break;

                case IN_PROGRESS:
                    break;

                default:
                    break;
            }
            break;
        case GRABBER_UP:
            //ACT_push_plate_rotate_vertically(TRUE);
            state = GO_POS5;
        case GO_POS5:
            sub_action = goto_pos_with_scan_foe((displacement_t[]){{{600,COLOR_Y(2000)}}},1,FORWARD,NORMAL_WAIT);
            switch(sub_action)
            {
                case END_OK:
                    state=GO_POS5bis;
                    break;

                case END_WITH_TIMEOUT:
                    state=GO_POS5bis;
                    break;

                case NOT_HANDLED:
                    state=GO_POS5bis;

                    break;

                case IN_PROGRESS:
                    break;

                default:

                    break;
            }
            break;
        case GO_POS5bis:
            sub_action = goto_pos_with_scan_foe((displacement_t[]){{{800,COLOR_Y(2000)}}},1,FORWARD,NORMAL_WAIT);
            switch(sub_action)
            {
                case END_OK:
                    state=GO_POS5final;
                    break;

                case END_WITH_TIMEOUT:
                    state=GO_POS5final;
                    break;

                case NOT_HANDLED:
                    state=GO_POS5final;

                    break;

                case IN_PROGRESS:
                    break;

                default:

                    break;
            }
            break;
        case GO_POS5final:
            sub_action = goto_pos_with_scan_foe((displacement_t[]){{{800,COLOR_Y(400)}}},1,FORWARD,NORMAL_WAIT);
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
}

