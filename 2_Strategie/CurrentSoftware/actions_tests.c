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

void TEST_STRAT_HOMOLOGATION(void){  //by micky
    static enum{
                GLASSES = 0,
                FIRST_GIFT,
                SECOND_GIFT,
                THIRD_GIFT,
                FOURTH_GIFT,
    }state = GLASSES;

   // bool_e timeout;
    error_e sub_action;

    switch(state){
        case GLASSES:
            sub_action = TEST_STRAT_verres();

            switch(sub_action){
                case END_OK:
                case END_WITH_TIMEOUT:
                case NOT_HANDLED:
                    state = FIRST_GIFT;
                    break;
                case IN_PROGRESS:
                    break;
            }
            break;

        case FIRST_GIFT:
            //Entrer l'argument dans le tableau
            sub_action = TEST_STRAT_gift1();

            switch(sub_action){
                case END_OK:
                case END_WITH_TIMEOUT:
                case NOT_HANDLED:
                    state = SECOND_GIFT;
                    break;
                case IN_PROGRESS:
                    break;
            }
            break;

            case SECOND_GIFT:
            //Entrer l'argument dans le tableau
            sub_action = TEST_STRAT_gift2();

            switch(sub_action){
                case END_OK:
                case END_WITH_TIMEOUT:
                case NOT_HANDLED:
                    state = THIRD_GIFT;
                    break;
                case IN_PROGRESS:
                    break;
            }
            break;

            case THIRD_GIFT:
            //Entrer l'argument dans le tableau
            sub_action = TEST_STRAT_gift3();

            switch(sub_action){
                case END_OK:
                case END_WITH_TIMEOUT:
                case NOT_HANDLED:
                    state = FOURTH_GIFT;
                    break;
                case IN_PROGRESS:
                    break;
            }

            case FOURTH_GIFT:
            //Entrer l'argument dans le tableau
            sub_action = TEST_STRAT_gift4();

            switch(sub_action){
                case END_OK:
                case END_WITH_TIMEOUT:
                case NOT_HANDLED:
                    state = GLASSES;
                    break;
                case IN_PROGRESS:
                    break;
            }
            break;
            break;
    }
}

void TEST_STRAT_homolagation_police(void)
{
	static enum
	{
		START,
		DO_POINTS,
		AVOIDANCE
	}state = START;

	static error_e sub_action;
	static bool_e timeout= FALSE;

	switch(state)
	{
		case START:
			sub_action = Test_Homologation_Sortie_Base();
			switch (sub_action)
			{
				case END_OK:
					state = DO_POINTS;
					break;
				case END_WITH_TIMEOUT:
					state = DO_POINTS;
					break;
				case NOT_HANDLED:
					state = DO_POINTS;
					break;
				case IN_PROGRESS:
					break;
				default:
					state = 0;
					break;
			}
			break;

		case DO_POINTS:
			sub_action =  Test_Homologation_Point();
			switch (sub_action)
			{
				case END_OK:
					state = AVOIDANCE;
					break;
				case END_WITH_TIMEOUT:
					state = AVOIDANCE;
					break;
				case NOT_HANDLED:
					state = AVOIDANCE;
					break;
				case IN_PROGRESS:
					break;
				default:
					state = 0;
					break;
			}
			break;


		case AVOIDANCE:
			sub_action = goto_pos_with_scan_foe
			((displacement_t[]){{{350,COLOR_Y(800)},SLOW},{{1575,COLOR_Y(800)},SLOW}},2,FORWARD,NO_DODGE_AND_WAIT);
			switch(sub_action)
			{
				case END_OK:
					state = AVOIDANCE;
					break;
				case END_WITH_TIMEOUT:
					timeout = TRUE;
					state = AVOIDANCE;
					break;
				case NOT_HANDLED:
					state = AVOIDANCE;
					break;
				case IN_PROGRESS:
					break;
				default:
					break;
			}
			break;
	}
}


/* ----------------------------------------------------------------------------- */
/* 					Sous Strat homologation                     			 */
/* ----------------------------------------------------------------------------- */

error_e Test_Homologation_Point(void) //by Amaury
{
	static enum{
		APPROACH = 0,
		POINT_CADEAU,
		DONE,
		}state = APPROACH;

		static error_e sub_action;
		static bool_e timeout=FALSE;

		switch(state){
			case APPROACH :
				sub_action = goto_pos(60,COLOR_Y(600),FAST,REAR);

				switch(sub_action){
					case END_OK:
						state = POINT_CADEAU;
						break;
					case END_WITH_TIMEOUT:
						state = DONE;
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

			case POINT_CADEAU :
				//sub_action = actionneur cadeau;

				switch(sub_action){
					case END_OK:
						state = APPROACH;
						break;
					case END_WITH_TIMEOUT:
						state = DONE;
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

			case DONE :
				return (timeout)?END_WITH_TIMEOUT:END_OK;
				break;

			default :
				state = 0;
				break;
		}
		return IN_PROGRESS;
}


error_e Test_Homologation_Sortie_Base(void)
{
    static enum{
        EMPILE = 0,
		WAIT,
		DONE,
    }state = EMPILE;

    static bool_e timeout=FALSE;

    switch(state){
        case EMPILE:
            ASSER_push_goto(600, COLOR_Y(600),FAST,REAR,0,TRUE);
            state = WAIT;
            break;

        case WAIT:
            if(STACKS_wait_end_auto_pull(ASSER, &timeout)){
                state = DONE;
            }
            break;

        case DONE:
            state = EMPILE;
            return (timeout)?END_WITH_TIMEOUT:END_OK;
            break;

        default:
            state = 0;
            return NOT_HANDLED;
            break;
    }
    return IN_PROGRESS;
}

#endif
/* ----------------------------------------------------------------------------- */
/* 								Stratégies de test                     			 */
/* ----------------------------------------------------------------------------- */
void test_qui_sert_pas(){ //Ceci n'a rien a voir avec le robot
	Uint8 x,y;
	static bool_e s=1;
	
	if(s){
		x = 0x00;
		y = 0x03;

		x = y+3;
		debug_printf("x = 0x%x	y = 0x%x\n",x,y);
		x |= 0xC3;
		debug_printf("x = 0x%x	y = 0x%x\n",x,y);
		x = x&0b10001010;
		debug_printf("x = 0x%x	y = 0x%x\n",x,y);
		x <<= y;
		debug_printf("x = 0x%x	y = 0x%x\n",x,y);
		y ^= 0x1;
		debug_printf("x = 0x%x	y = 0x%x\n",x,y);
		x >>= y;
		debug_printf("x = 0x%x	y = 0x%x\n",x,y);
		y =~ x;
		debug_printf("x = 0x%x	y = 0x%x\n",x,y);
		x = !y;
		debug_printf("x = 0x%x	y = 0x%x\n",x,y);
		s=0;
	}
}
void TEST_STRAT_strat_selector_1(void){
	LED_USER2 = 1;
	LED_ERROR = 0;
}
void TEST_STRAT_strat_selector_2(void){
	LED_USER2 = 1;
	LED_ERROR = 1;
}
void TEST_STRAT_strat_selector_3(void){
	LED_USER2 = 0;
	LED_ERROR = 1;
}

void TEST_STRAT_lever_le_kiki(void){
    static int a=1;
    if(a==1){
        //TEST_STRAT_lever_le_kiki: si vous voulez utiliser cette strat, il va faloir changer la position a quelque chose de correct
        //ACT_push_hammer_goto(0, TRUE);
	#warning "Todo: Il faut changer la position du hammer pour l'utiliser"
        a=0;
    }
}

void TEST_STRAT_premier_deplacement(void){
	static enum{
		SORTIR,
		PREMIER,
		SECOND,
		SECONDBIS,
		TROIS,
		QUATRE,
		
				DONE,
	}state = SORTIR;

	//static bool_e timeout;
	static error_e sub_action;
	
	
	switch(state){
		case SORTIR:
			sub_action = goto_pos(580,COLOR_Y(450),FAST,FORWARD);
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
					{{550, COLOR_Y(2400)},FAST},
					{{600, COLOR_Y(2405)},
							FAST}},4,REAR,NO_AVOIDANCE);
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
					(displacement_t[]){{{550, COLOR_Y(400)},FAST}},
							1,FORWARD,NO_AVOIDANCE);
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
					(displacement_t[]){{{1200, COLOR_Y(2400)},FAST}},
							1,REAR,NO_AVOIDANCE);

			break;
		case TROIS:
			sub_action = goto_pos_with_scan_foe(
					(displacement_t[]){{{550, COLOR_Y(2200)},FAST},
					{{1200, COLOR_Y(2400)},FAST}},
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
					(displacement_t[]){{{800, COLOR_Y(2200)},FAST},
					{{800, COLOR_Y(1000)},FAST},
					{{700, COLOR_Y(420)},FAST}},
					3,FORWARD,NO_AVOIDANCE);
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
			break;
	}
}

void levi(void){
    goto_pos_with_scan_foe((displacement_t[]){{{1000,COLOR_Y(1500)},VERY_SLOW}}, 1,FORWARD,NO_DODGE_AND_WAIT);

}


//By Julien j. si vous avez un probleme verifiez qu'il n'ait pas fumé les dernières 24 heures et demandez lui avec des mots doux XD
void strat_verre_assiettes_et_gateau()
{
    static enum
	{
                VERRE,
                VERRE2,
		START_A,
		PREMIERE_A,
                SECONDE_A,
                TROISIEME_A,
                DERNIERE_A,
                REPOSITIONNEMENT,
                PUSH,
                FIRST_HALF_CAKE,
                SECOND_HALF_CAKE,
                TIMEOUT,
                DONE
	}state = START_A;

    static error_e sub_action;

    switch(state)
    {
        case VERRE :

                        sub_action = TEST_STRAT_VERRE1();
			switch (sub_action)
			{
				case END_OK:
					state = VERRE2;
					break;
				case END_WITH_TIMEOUT:
					state = VERRE2;
					break;
				case NOT_HANDLED:
					state = VERRE2;
					break;
				case IN_PROGRESS:
					break;
				default:
					state = 0;
					break;
			}
			break;
        case VERRE2 :

                        sub_action = TEST_STRAT_VERRE2();
			switch (sub_action)
			{
				case END_OK:
					state = START_A;
					break;
				case END_WITH_TIMEOUT:
					state = START_A;
					break;
				case NOT_HANDLED:
					state = START_A;
					break;
				case IN_PROGRESS:
					break;
				default:
					state = 0;
					break;
			}
			break;

        case START_A :

                        sub_action = STRAT_DEBUT_ASSIETTE();
			switch (sub_action)
			{
				case END_OK:
					state = PREMIERE_A;
					break;
				case END_WITH_TIMEOUT:
					state = PREMIERE_A;
					break;
				case NOT_HANDLED:
					state = PREMIERE_A;
					break;
				case IN_PROGRESS:
					break;
				default:
					state = 0;
					break;
			}
			break;

            case PREMIERE_A:
			sub_action = STRAT_PREMIERE_ASSIETTE();
			switch (sub_action)
			{
				case END_OK:
					state = SECONDE_A;
					break;
				case END_WITH_TIMEOUT:
					state = SECONDE_A;
					break;
				case NOT_HANDLED:
					state = SECONDE_A;
					break;
				case IN_PROGRESS:
					break;
				default:
					state = 0;
					break;
			}
			break;

            case SECONDE_A:
			sub_action = STRAT_SECONDE_ASSIETTE();
			switch (sub_action)
			{
				case END_OK:
					state = TROISIEME_A;
					break;
				case END_WITH_TIMEOUT:
					state = TROISIEME_A;
					break;
				case NOT_HANDLED:
					state = TROISIEME_A;
					break;
				case IN_PROGRESS:
					break;
				default:
					state = 0;
					break;
			}
			break;

            case TROISIEME_A:
			sub_action = STRAT_TROISIEME_ASSIETTE();
			switch (sub_action)
			{
				case END_OK:
					state = DERNIERE_A;
					break;
				case END_WITH_TIMEOUT:
					state = DERNIERE_A;
					break;
				case NOT_HANDLED:
					state = DERNIERE_A;
					break;
				case IN_PROGRESS:
					break;
				default:
					state = 0;
					break;
			}
			break;

            case DERNIERE_A:
			sub_action = STRAT_DERNIERE_ASSIETTE();
			switch (sub_action)
			{
				case END_OK:
					state = REPOSITIONNEMENT;
					break;
				case END_WITH_TIMEOUT:
					state = REPOSITIONNEMENT;
					break;
				case NOT_HANDLED:
					state = REPOSITIONNEMENT;
					break;
				case IN_PROGRESS:
					break;
				default:
					state = 0;
					break;
			}
			break;

            case REPOSITIONNEMENT:
                        sub_action = STRAT_FIN_ASSIETTE();
                        switch (sub_action)
			{
				case END_OK:
                                    state = PUSH;
					break;
				case END_WITH_TIMEOUT:
                                    state = PUSH;

					break;
				case NOT_HANDLED:
                                    state = PUSH;
					break;
				case IN_PROGRESS:
					break;
				default:

					break;
			}
			break;
            case PUSH:
                                sub_action = TEST_STRAT_in_da_wall();

                                switch(sub_action){
                                case END_OK:

                                    state = FIRST_HALF_CAKE;
                                    break;
                                case END_WITH_TIMEOUT:
                                    state = FIRST_HALF_CAKE;
                                    break;
                                case NOT_HANDLED:
                                    state = FIRST_HALF_CAKE;
                                    break;
                                case IN_PROGRESS:
                                    break;
                            }
                            break;

            case FIRST_HALF_CAKE:
                            sub_action = TEST_STRAT_first_half_cake();

                            switch(sub_action)
                            {
                                case END_OK:
                                    state = SECOND_HALF_CAKE;
                                    break;
                                case END_WITH_TIMEOUT:
                                    state = SECOND_HALF_CAKE;
                                    break;
                                case NOT_HANDLED:
                                    state = SECOND_HALF_CAKE;
                                    break;
                                case IN_PROGRESS:
                                    break;
                                default:
                                    break;
                            }
                            break;

            case SECOND_HALF_CAKE:
                            sub_action = TEST_STRAT_second_half_cake();

                            switch(sub_action)
                            {
                                case END_OK:
                                    state = DONE;
                                    break;
                                case END_WITH_TIMEOUT:
                                    state = DONE;
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

            case TIMEOUT:
                        sub_action = STRAT_TIMEOUT_ASSIETTE();
                        switch (sub_action)
			{
				case END_OK:
                                    state = START_A;
					break;
				case END_WITH_TIMEOUT:
                                    state = START_A;

					break;
				case NOT_HANDLED:
					state = START_A;
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


/* ----------------------------------------------------------------------------- */
/* 						Actions élémentaires de construction                     */
/* ----------------------------------------------------------------------------- */


error_e TEST_STRAT_gift1(void){
    static enum{
        APPROACH,
        DONE,
    }state = APPROACH;

    static bool_e timeout;
    error_e sub_action;

    switch(state){
        case APPROACH:
            sub_action = goto_pos_with_scan_foe((displacement_t[]){{{450,COLOR_Y(550)},DEFAULT_SPEED}, {{250,COLOR_Y(1000)},DEFAULT_SPEED}},2,FORWARD,NO_DODGE_AND_WAIT);

            switch(sub_action){
                case END_OK:
                    state = DONE;
                    break;
                case END_WITH_TIMEOUT:
                    state = DONE;
                    break;
                case NOT_HANDLED:
                    state = DONE;
                    break;
                case IN_PROGRESS:
                    break;
            }

            break;
        case DONE:
            return (timeout)? END_WITH_TIMEOUT:END_OK;
            break;
        default:
            return NOT_HANDLED;
            break;
    }
    return IN_PROGRESS;
}

error_e TEST_STRAT_gift2(void){
    return END_OK;
}

error_e TEST_STRAT_gift3(void){
    return END_OK;
}

error_e TEST_STRAT_gift4(void){
    return END_OK;
}


void test_strat_verrre(){
    static error_e sub_action;
    static enum
    {
        VERRES = 0,
        DONE,
    }state=DONE;

    switch (state)
    {
        case VERRES:
            sub_action = TEST_STRAT_verres();
            switch(sub_action)
            {
                case END_OK:
                    state=DONE;
                    break;

                case END_WITH_TIMEOUT:
                    state=DONE;
					break;
                case NOT_HANDLED:
                    state = 0;
                    break;

                case IN_PROGRESS:
                    break;

                default:
                    state = 0;
                    break;
            }
            break;
        case DONE:
            break;
        default:
            break;
    }
}

error_e TEST_ACT(void){

        static ACT_function_result_e sub_action_act;
        static bool_e timeout = FALSE;
	static enum {
            ACT_HAMMER_DOWN = 0,
            ACT_HAMMER_TIDY,
            ACT_HAMMER_UP,
            ACT_BALL_GRABBER_DOWN,
            ACT_BALL_GRABBER_TIDY,
            ACT_BALL_GRABBER_UP,
            ACT_BALL_LAUNCHER_RUN,
            ACT_BALL_LAUNCHER_STOP,
            DONE,
	} state = ACT_HAMMER_DOWN;

        switch (state)
        {
            case ACT_HAMMER_DOWN:
                //ACT_hammer_down();
                #warning "Todo: Il faut changer la position du hammer pour l'utiliser"
                return NOT_HANDLED;
                //ACT_push_hammer_goto(0, TRUE);

                sub_action_act = ACT_get_last_action_result(ACT_STACK_Hammer);
                switch(sub_action_act)
                {
                        case ACT_FUNCTION_InProgress:
                            break;

                        case ACT_FUNCTION_Done:
                            state=ACT_HAMMER_TIDY;
                            break;

                        case ACT_FUNCTION_ActDisabled:
                            state = 0;
                            return NOT_HANDLED;
                            break;

                        case ACT_FUNCTION_RetryLater:
                            state = 0;
                            return NOT_HANDLED;
                            break;

                        default:
                            state = 0;
                            return NOT_HANDLED;
                            break;

                }

                break;

            case ACT_HAMMER_TIDY:
                //ACT_hammer_tidy();
                #warning "Todo: Il faut changer la position du hammer pour l'utiliser"
                return NOT_HANDLED;
                //ACT_push_hammer_goto(0, TRUE);
                sub_action_act = ACT_get_last_action_result(ACT_STACK_Hammer);
                switch(sub_action_act)
                {
                        case ACT_FUNCTION_InProgress:
                            break;

                        case ACT_FUNCTION_Done:
                            state=ACT_HAMMER_UP;
                            break;

                        case ACT_FUNCTION_ActDisabled:
                            state = 0;
                            return NOT_HANDLED;
                            break;

                        case ACT_FUNCTION_RetryLater:
                            state = 0;
                            return NOT_HANDLED;
                            break;

                        default:
                            state = 0;
                            return NOT_HANDLED;
                            break;

                }
                break;

            case ACT_HAMMER_UP:
                //ACT_hammer_up();
                #warning "Todo: Il faut changer la position du hammer pour l'utiliser"
                return NOT_HANDLED;
                //ACT_push_hammer_goto(0, TRUE);
                sub_action_act = ACT_get_last_action_result(ACT_STACK_Hammer);
                switch(sub_action_act)
                {
                        case ACT_FUNCTION_InProgress:
                            break;

                        case ACT_FUNCTION_Done:
                            state=ACT_BALL_GRABBER_DOWN;
                            break;

                        case ACT_FUNCTION_ActDisabled:
                            state = 0;
                            return NOT_HANDLED;
                            break;

                        case ACT_FUNCTION_RetryLater:
                            state = 0;
                            return NOT_HANDLED;
                            break;

                        default:
                            state = 0;
                            return NOT_HANDLED;
                            break;

                }
                break;

            case ACT_BALL_GRABBER_DOWN:
                //ACT_ball_grabber_down);
		ACT_push_plate_rotate_horizontally(TRUE);
                sub_action_act = ACT_get_last_action_result(ACT_STACK_Plate);
                switch(sub_action_act)
                {
                        case ACT_FUNCTION_InProgress:
                            break;

                        case ACT_FUNCTION_Done:
                            state=ACT_BALL_GRABBER_TIDY;
                            break;

                        case ACT_FUNCTION_ActDisabled:
                            state = 0;
                            return NOT_HANDLED;
                            break;

                        case ACT_FUNCTION_RetryLater:
                            state = 0;
                            return NOT_HANDLED;
                            break;

                        default:
                            state = 0;
                            return NOT_HANDLED;
                            break;

                }
                break;

            case ACT_BALL_GRABBER_TIDY:
                //ACT_ball_grabber_tidy();
                ACT_push_plate_rotate_prepare(TRUE);
                sub_action_act = ACT_get_last_action_result(ACT_STACK_Plate);
                switch(sub_action_act)
                {
                        case ACT_FUNCTION_InProgress:
                            break;

                        case ACT_FUNCTION_Done:
                            state=ACT_BALL_GRABBER_UP;
                            break;

                        case ACT_FUNCTION_ActDisabled:
                            state = 0;
                            return NOT_HANDLED;
                            break;

                        case ACT_FUNCTION_RetryLater:
                            state = 0;
                            return NOT_HANDLED;
                            break;

                        default:
                            state = 0;
                            return NOT_HANDLED;
                            break;

                }
                break;

            case ACT_BALL_GRABBER_UP:
                //ACT_ball_grabber_up();
                ACT_push_plate_rotate_vertically(TRUE);
                sub_action_act = ACT_get_last_action_result(ACT_STACK_Plate);
                switch(sub_action_act)
                {
                        case ACT_FUNCTION_InProgress:
                            break;

                        case ACT_FUNCTION_Done:
                            state=ACT_BALL_LAUNCHER_RUN;
                            break;

                        case ACT_FUNCTION_ActDisabled:
                            state = 0;
                            return NOT_HANDLED;
                            break;

                        case ACT_FUNCTION_RetryLater:
                            state = 0;
                            return NOT_HANDLED;
                            break;

                        default:
                            state = 0;
                            return NOT_HANDLED;
                            break;

                }
                break;

            case ACT_BALL_LAUNCHER_RUN:
                ACT_push_ball_launcher_run(6000,TRUE);


                sub_action_act = ACT_get_last_action_result(ACT_STACK_BallLauncher);
                switch(sub_action_act)
                {
                        case ACT_FUNCTION_InProgress:
                            break;

                        case ACT_FUNCTION_Done:
                            state=ACT_BALL_LAUNCHER_STOP;
                            break;

                        case ACT_FUNCTION_ActDisabled:
                            state = 0;
                            return NOT_HANDLED;
                            break;

                        case ACT_FUNCTION_RetryLater:
                            state = 0;
                            return NOT_HANDLED;
                            break;

                        default:
                            state = 0;
                            return NOT_HANDLED;
                            break;

                }
                break;

            case ACT_BALL_LAUNCHER_STOP:
                ACT_push_ball_launcher_stop(TRUE);

                sub_action_act = ACT_get_last_action_result(ACT_STACK_BallLauncher);
                switch(sub_action_act)
                {
                        case ACT_FUNCTION_InProgress:
                         break;

                         case ACT_FUNCTION_Done:
                            state=DONE;
                            break;

                        case ACT_FUNCTION_ActDisabled:
                            state = 0;
                            return NOT_HANDLED;
                            break;

                        case ACT_FUNCTION_RetryLater:
                            state = 0;
                            return NOT_HANDLED;
                            break;

                        default:
                            state = 0;
                            return NOT_HANDLED;
                            break;
                }
                break;
                
            case DONE:
                return (timeout) ? END_WITH_TIMEOUT : END_OK;
                break;
            default :
                state=0;
                return NOT_HANDLED;
                break;

        }
		return NOT_HANDLED;
}








void TEST_STRAT_assiettes_evitement(void) {

    static enum {
        ASSIETTE_1,
        ASSIETTE_2,
        ASSIETTE_3,
        ASSIETTE_4,
        ASSIETTE_5,
        DONE
    } state = ASSIETTE_1;

    static error_e sub_action;

    switch (state) {
        case ASSIETTE_1:
            sub_action = TEST_STRAT_PREMIERE_ASSIETTE_EVITEMENT() ;
            switch (sub_action) {
                case END_OK:
                    state = ASSIETTE_2;
                    break;
                case END_WITH_TIMEOUT:
                    state = ASSIETTE_2;
                    break;
                case NOT_HANDLED:
                    state = ASSIETTE_2;
                    break;
                case IN_PROGRESS:
                    break;
                default:
                    state = 0;
                    break;
            }
            break;

        case ASSIETTE_2:

            sub_action = TEST_STRAT_SECONDE_ASSIETTE_EVITEMENT() ;
            switch (sub_action) {
                case END_OK:
                    state = ASSIETTE_3;
                    break;
                case END_WITH_TIMEOUT:
                    state = ASSIETTE_3;
                    break;
                case NOT_HANDLED:
                    state = ASSIETTE_3;
                    break;
                case IN_PROGRESS:
                    break;
                default:
                    state = 0;
                    break;
            }
            break;

        case ASSIETTE_3:
            sub_action = TEST_STRAT_TROISIEME_ASSIETTE_EVITEMENT() ;
            switch (sub_action) {
                case END_OK:
                    state = ASSIETTE_4;
                    break;
                case END_WITH_TIMEOUT:
                    state = ASSIETTE_4;
                    break;
                case NOT_HANDLED:
                    state = ASSIETTE_4;
                    break;
                case IN_PROGRESS:
                    break;
                default:
                    state = 0;
                    break;
            }
            break;

        case ASSIETTE_4:
            sub_action = TEST_STRAT_QUATRIEME_ASSIETTE_EVITEMENT() ;
            switch (sub_action) {
                case END_OK:
                    state = ASSIETTE_5;
                    break;
                case END_WITH_TIMEOUT:
                    state = ASSIETTE_5;
                    break;
                case NOT_HANDLED:
                    state = ASSIETTE_5;
                    break;
                case IN_PROGRESS:
                    break;
                default:
                    state = 0;
                    break;
            }
            break;

        case ASSIETTE_5:
            sub_action = TEST_STRAT_CINQUIEME_ASSIETTE_EVITEMENT() ;
            switch (sub_action) {
                case END_OK:
                    state = DONE;
                    break;
                case END_WITH_TIMEOUT:
                    state = DONE;
                    break;
                case NOT_HANDLED:
                    state = DONE;
                    break;
                case IN_PROGRESS:
                    break;
                default:
                    state = 0;
                    break;
            }
            break;

        case DONE:
            break;

        default:
            break;


    }
}



//assiettes avec evitement
error_e TEST_STRAT_PREMIERE_ASSIETTE_EVITEMENT(void){

    static error_e sub_action;
	static ACT_function_result_e sub_action_act;
    static enum {
		POS_MOVE = 0,
        PUSH_MOVE ,
		WAIT_END_OF_MOVE,
		ACT_DOWN,
		ACT_UP,
        MOVE_REAR,
        DONE,
    } state = PUSH_MOVE;

    static bool_e timeout = FALSE;

    switch (state) {
        case POS_MOVE:

            //pour les test uniquement
            sub_action = goto_pos_with_scan_foe((displacement_t[]){{{500,COLOR_Y(1000)}}, {{250,COLOR_Y(600)}}},2,FORWARD,NORMAL_WAIT);
            //pour la vrai strat : sub_action = goto_pos_with_scan_foe((displacement_t[]){ {{250,COLOR_Y(600)}},{{250,COLOR_Y(380)}}},2,FORWARD,NORMAL_WAIT);
			switch(sub_action)
            {
                case END_OK:
                    state=PUSH_MOVE;
                    break;

                case END_WITH_TIMEOUT:
                    state=PUSH_MOVE;
					break;
                case NOT_HANDLED:
                    state = 0;
                    return NOT_HANDLED;
                    break;

                case IN_PROGRESS:
                    break;

                default:
                    state = 0;
                    return NOT_HANDLED;
                    break;
            }
            break;

        case PUSH_MOVE:

            // ATTENTION : PAS D'EVITEMENT SUR CETTE FONCTION
            ASSER_push_rush_in_the_wall(FORWARD,TRUE,0,FALSE);
            state=WAIT_END_OF_MOVE;
            break;

        case WAIT_END_OF_MOVE:
            if (STACKS_wait_end_auto_pull(ASSER, &timeout)) {
                state = ACT_DOWN;
            }
            break;
		case ACT_DOWN:

			//ACT_ball_grabber_down();
			ACT_push_plate_rotate_horizontally(TRUE);
			sub_action_act = ACT_get_last_action_result(ACT_STACK_Plate);
			switch(sub_action_act)
            {
                case ACT_FUNCTION_InProgress:
                    break;

                case ACT_FUNCTION_Done:
                    state=ACT_UP;
					break;

                case ACT_FUNCTION_ActDisabled:
                    state = 0;
                    return NOT_HANDLED;
                    break;

                case ACT_FUNCTION_RetryLater:
                     state = 0;
                    return NOT_HANDLED;
					break;

                default:
                    state = 0;
                    return NOT_HANDLED;
                    break;
            }

			break;

		case ACT_UP:

			//ACT_ball_grabber_up();
			ACT_push_plate_rotate_vertically(TRUE);
			sub_action_act = ACT_get_last_action_result(ACT_STACK_Plate);
			switch(sub_action_act)
            {
                case ACT_FUNCTION_InProgress:
                    break;

                case ACT_FUNCTION_Done:
                    state=MOVE_REAR;
					break;

                case ACT_FUNCTION_ActDisabled:
                    state = 0;
                    return NOT_HANDLED;
                    break;

                case ACT_FUNCTION_RetryLater:
                     state = 0;
                    return NOT_HANDLED;
					break;

                default:
                    state = 0;
                    return NOT_HANDLED;
                    break;
            }
			break;

        case MOVE_REAR:
            sub_action = goto_pos_with_scan_foe((displacement_t[]){{{250,COLOR_Y(600)}}},1,REAR,NORMAL_WAIT);

            switch(sub_action)
            {
                case END_OK:
                    state=DONE;
                    break;

                case END_WITH_TIMEOUT:
                    state=DONE;
					break;
                case NOT_HANDLED:
                    state = 0;
                    return NOT_HANDLED;
                    break;

                case IN_PROGRESS:
                    break;

                default:
                    state = 0;
                    return NOT_HANDLED;
                    break;
            }
            break;
        case DONE:
            return (timeout) ? END_WITH_TIMEOUT : END_OK;
            break;
        default:
            state = 0;
            return NOT_HANDLED;
            break;
    }
	return NOT_HANDLED;
}

error_e TEST_STRAT_SECONDE_ASSIETTE_EVITEMENT(void){

    static error_e sub_action;
	static ACT_function_result_e sub_action_act;
    static enum {
		POS_MOVE = 0,
        PUSH_MOVE ,
		WAIT_END_OF_MOVE,
		ACT_DOWN,
		ACT_UP,
        MOVE_REAR,
        DONE,
    } state = POS_MOVE;

    static bool_e timeout = FALSE;

    switch (state) {
		case POS_MOVE:
			sub_action = goto_pos_with_scan_foe((displacement_t[]){ {{600,COLOR_Y(600)}}},1,FORWARD,NORMAL_WAIT);
			switch(sub_action)
            {
                case END_OK:
                    state=PUSH_MOVE;
                    break;

                case END_WITH_TIMEOUT:
                    state=PUSH_MOVE;
					break;
                case NOT_HANDLED:
                    state = 0;
                    return NOT_HANDLED;
                    break;

                case IN_PROGRESS:
                    break;

                default:
                    state = 0;
                    return NOT_HANDLED;
                    break;
            }
            break;

        case PUSH_MOVE:

            // ATTENTION : PAS D'EVITEMENT SUR CETTE FONCTION
            ASSER_push_rush_in_the_wall(FORWARD,TRUE,0,FALSE);
            state=WAIT_END_OF_MOVE;
            break;

        case WAIT_END_OF_MOVE:
            if (STACKS_wait_end_auto_pull(ASSER, &timeout)) {
                state = ACT_DOWN;
            }
            break;
		case ACT_DOWN:

			//ACT_ball_grabber_down();
			ACT_push_plate_rotate_horizontally(TRUE);
			sub_action_act = ACT_get_last_action_result(ACT_STACK_Plate);
			switch(sub_action_act)
            {
                case ACT_FUNCTION_InProgress:
                    break;

                case ACT_FUNCTION_Done:
                    state=ACT_UP;
					break;

                case ACT_FUNCTION_ActDisabled:
                    state = 0;
                    return NOT_HANDLED;
                    break;

                case ACT_FUNCTION_RetryLater:
                     state = 0;
                    return NOT_HANDLED;
					break;

                default:
                    state = 0;
                    return NOT_HANDLED;
                    break;
            }
			break;

		case ACT_UP:

			//ACT_ball_grabber_up();
			ACT_push_plate_rotate_vertically(TRUE);
			sub_action_act = ACT_get_last_action_result(ACT_STACK_Plate);
			switch(sub_action_act)
            {
                case ACT_FUNCTION_InProgress:
                    break;

                case ACT_FUNCTION_Done:
                    state=MOVE_REAR;
					break;

                case ACT_FUNCTION_ActDisabled:
                    state = 0;
                    return NOT_HANDLED;
                    break;

                case ACT_FUNCTION_RetryLater:
                     state = 0;
                    return NOT_HANDLED;
					break;

                default:
                    state = 0;
                    return NOT_HANDLED;
                    break;
            }
			break;

        case MOVE_REAR:
            sub_action = goto_pos_with_scan_foe((displacement_t[]){{{600,COLOR_Y(600)}}},1,REAR,NORMAL_WAIT);

            switch(sub_action)
            {
                case END_OK:
                    state=DONE;
                    break;

                case END_WITH_TIMEOUT:
                    state=DONE;
					break;
                case NOT_HANDLED:
                    state = 0;
                    return NOT_HANDLED;
                    break;

                case IN_PROGRESS:
                    break;

                default:
                    state = 0;
                    return NOT_HANDLED;
                    break;
            }
            break;
        case DONE:
            return (timeout) ? END_WITH_TIMEOUT : END_OK;
            break;
        default:
            state = 0;
            return NOT_HANDLED;
            break;
    }
	return NOT_HANDLED;
}

error_e TEST_STRAT_TROISIEME_ASSIETTE_EVITEMENT(void){

    static error_e sub_action;
	static ACT_function_result_e sub_action_act;
    static enum {
		POS_MOVE = 0,
        PUSH_MOVE ,
		WAIT_END_OF_MOVE,
		ACT_DOWN,
		ACT_UP,
        MOVE_REAR,
        DONE,
    } state = PUSH_MOVE;

    static bool_e timeout = FALSE;

    switch (state) {
        case POS_MOVE:


            sub_action = goto_pos_with_scan_foe((displacement_t[]){ {{1000,COLOR_Y(600)}}},1,FORWARD,NORMAL_WAIT);
 			switch(sub_action)
            {
                case END_OK:
                    state=PUSH_MOVE;
                    break;

                case END_WITH_TIMEOUT:
                    state=PUSH_MOVE;
					break;
                case NOT_HANDLED:
                    state = 0;
                    return NOT_HANDLED;
                    break;

                case IN_PROGRESS:
                    break;

                default:
                    state = 0;
                    return NOT_HANDLED;
                    break;
            }
            break;

        case PUSH_MOVE:

            // ATTENTION : PAS D'EVITEMENT SUR CETTE FONCTION
            ASSER_push_rush_in_the_wall(FORWARD,TRUE,0,FALSE);
            state=WAIT_END_OF_MOVE;
            break;

        case WAIT_END_OF_MOVE:
            if (STACKS_wait_end_auto_pull(ASSER, &timeout)) {
                state = ACT_DOWN;
            }
            break;
		case ACT_DOWN:

			//ACT_ball_grabber_down();
			ACT_push_plate_rotate_horizontally(TRUE);
			sub_action_act = ACT_get_last_action_result(ACT_STACK_Plate);
			switch(sub_action_act)
            {
                case ACT_FUNCTION_InProgress:
                    break;

                case ACT_FUNCTION_Done:
                    state=ACT_UP;
					break;

                case ACT_FUNCTION_ActDisabled:
                    state = 0;
                    return NOT_HANDLED;
                    break;

                case ACT_FUNCTION_RetryLater:
                     state = 0;
                    return NOT_HANDLED;
					break;

                default:
                    state = 0;
                    return NOT_HANDLED;
                    break;
            }
			break;

		case ACT_UP:

			//ACT_ball_grabber_up();
			ACT_push_plate_rotate_vertically(TRUE);
			sub_action_act = ACT_get_last_action_result(ACT_STACK_Plate);
			switch(sub_action_act)
            {
                case ACT_FUNCTION_InProgress:
                    break;

                case ACT_FUNCTION_Done:
                    state=MOVE_REAR;
					break;

                case ACT_FUNCTION_ActDisabled:
                    state = 0;
                    return NOT_HANDLED;
                    break;

                case ACT_FUNCTION_RetryLater:
                     state = 0;
                    return NOT_HANDLED;
					break;

                default:
                    state = 0;
                    return NOT_HANDLED;
                    break;
            }
			break;

        case MOVE_REAR:
            sub_action = goto_pos_with_scan_foe((displacement_t[]){{{1000,COLOR_Y(600)}}},1,REAR,NORMAL_WAIT);

            switch(sub_action)
            {
                case END_OK:
                    state=DONE;
                    break;

                case END_WITH_TIMEOUT:
                    state=DONE;
                    break;
                case NOT_HANDLED:
                    state = 0;
                    return NOT_HANDLED;
                    break;

                case IN_PROGRESS:
                    break;

                default:
                    state = 0;
                    return NOT_HANDLED;
                    break;
            }
            break;
        case DONE:
            return (timeout) ? END_WITH_TIMEOUT : END_OK;
            break;
        default:
            state = 0;
            return NOT_HANDLED;
            break;
    }
	return NOT_HANDLED;
}

error_e TEST_STRAT_QUATRIEME_ASSIETTE_EVITEMENT(void){

    static error_e sub_action;
	static ACT_function_result_e sub_action_act;
    static enum {
		POS_MOVE = 0,
        PUSH_MOVE ,
		WAIT_END_OF_MOVE,
		ACT_DOWN,
		ACT_UP,
        MOVE_REAR,
        DONE,
    } state = PUSH_MOVE;

    static bool_e timeout = FALSE;

    switch (state) {
        case POS_MOVE:


            sub_action = goto_pos_with_scan_foe((displacement_t[]){ {{1400,COLOR_Y(600)}}},1,FORWARD,NORMAL_WAIT);
			switch(sub_action)
            {
                case END_OK:
                    state=PUSH_MOVE;
                    break;

                case END_WITH_TIMEOUT:
                    state=PUSH_MOVE;
                    break;
                case NOT_HANDLED:
                    state = 0;
                    return NOT_HANDLED;
                    break;

                case IN_PROGRESS:
                    break;

                default:
                    state = 0;
                    return NOT_HANDLED;
                    break;
            }
            break;

        case PUSH_MOVE:

            // ATTENTION : PAS D'EVITEMENT SUR CETTE FONCTION
            ASSER_push_rush_in_the_wall(FORWARD,TRUE,0,FALSE);
            state=WAIT_END_OF_MOVE;
            break;

        case WAIT_END_OF_MOVE:
            if (STACKS_wait_end_auto_pull(ASSER, &timeout)) {
                state = ACT_DOWN;
            }
            break;
		case ACT_DOWN:

			//ACT_ball_grabber_down();
			ACT_push_plate_rotate_horizontally(TRUE);
			sub_action_act = ACT_get_last_action_result(ACT_STACK_Plate);
			switch(sub_action_act)
            {
                case ACT_FUNCTION_InProgress:
                    break;

                case ACT_FUNCTION_Done:
                    state=ACT_UP;
                    break;

                case ACT_FUNCTION_ActDisabled:
                    state = 0;
                    return NOT_HANDLED;
                    break;

                case ACT_FUNCTION_RetryLater:
                     state = 0;
                    return NOT_HANDLED;
                    break;

                default:
                    state = 0;
                    return NOT_HANDLED;
                    break;
            }
			break;

		case ACT_UP:

			//ACT_ball_grabber_up();
			ACT_push_plate_rotate_vertically(TRUE);
			sub_action_act = ACT_get_last_action_result(ACT_STACK_Plate);
			switch(sub_action_act)
            {
                case ACT_FUNCTION_InProgress:
                    break;

                case ACT_FUNCTION_Done:
                    state=MOVE_REAR;
					break;

                case ACT_FUNCTION_ActDisabled:
                    state = 0;
                    return NOT_HANDLED;
                    break;

                case ACT_FUNCTION_RetryLater:
                     state = 0;
                    return NOT_HANDLED;
					break;

                default:
                    state = 0;
                    return NOT_HANDLED;
                    break;
            }
			break;

        case MOVE_REAR:
            sub_action = goto_pos_with_scan_foe((displacement_t[]){{{1400,COLOR_Y(600)}}},1,REAR,NORMAL_WAIT);

            switch(sub_action)
            {
                case END_OK:
                    state=DONE;
                    break;

                case END_WITH_TIMEOUT:
                    state=DONE;
					break;
                case NOT_HANDLED:
                    state = 0;
                    return NOT_HANDLED;
                    break;

                case IN_PROGRESS:
                    break;

                default:
                    state = 0;
                    return NOT_HANDLED;
                    break;
            }
            break;
        case DONE:
            return (timeout) ? END_WITH_TIMEOUT : END_OK;
            break;
        default:
            state = 0;
            return NOT_HANDLED;
            break;
    }
	return NOT_HANDLED;
}

error_e TEST_STRAT_CINQUIEME_ASSIETTE_EVITEMENT(void){

    static error_e sub_action;
	static ACT_function_result_e sub_action_act;
    static enum {
		POS_MOVE = 0,
        PUSH_MOVE ,
		WAIT_END_OF_MOVE,
		ACT_DOWN,
		ACT_UP,
        MOVE_REAR,
        DONE,
    } state = PUSH_MOVE;

    static bool_e timeout = FALSE;

    switch (state) {
        case POS_MOVE:


            sub_action = goto_pos_with_scan_foe((displacement_t[]){ {{1730,COLOR_Y(600)}}},1,FORWARD,NORMAL_WAIT);
 			switch(sub_action)
            {
                case END_OK:
                    state=PUSH_MOVE;
                    break;

                case END_WITH_TIMEOUT:
                    state=PUSH_MOVE;
					break;
                case NOT_HANDLED:
                    state = 0;
                    return NOT_HANDLED;
                    break;

                case IN_PROGRESS:
                    break;

                default:
                    state = 0;
                    return NOT_HANDLED;
                    break;
            }
            break;

        case PUSH_MOVE:

            // ATTENTION : PAS D'EVITEMENT SUR CETTE FONCTION
            ASSER_push_rush_in_the_wall(FORWARD,TRUE,0,FALSE);
            state=WAIT_END_OF_MOVE;
            break;

        case WAIT_END_OF_MOVE:
            if (STACKS_wait_end_auto_pull(ASSER, &timeout)) {
                state = ACT_DOWN;
            }
            break;
		case ACT_DOWN:

			//ACT_ball_grabber_down();
			ACT_push_plate_rotate_horizontally(TRUE);
			sub_action_act = ACT_get_last_action_result(ACT_STACK_Plate);
			switch(sub_action_act)
            {
                case ACT_FUNCTION_InProgress:
                    break;

                case ACT_FUNCTION_Done:
                    state=ACT_UP;
					break;

                case ACT_FUNCTION_ActDisabled:
                    state = 0;
                    return NOT_HANDLED;
                    break;

                case ACT_FUNCTION_RetryLater:
                     state = 0;
                    return NOT_HANDLED;
					break;

                default:
                    state = 0;
                    return NOT_HANDLED;
                    break;
            }
			break;

		case ACT_UP:

			//ACT_ball_grabber_up();
			ACT_push_plate_rotate_vertically(TRUE);
			sub_action_act = ACT_get_last_action_result(ACT_STACK_Plate);
			switch(sub_action_act)
            {
                case ACT_FUNCTION_InProgress:
                    break;

                case ACT_FUNCTION_Done:
                    state=MOVE_REAR;
					break;

                case ACT_FUNCTION_ActDisabled:
                    state = 0;
                    return NOT_HANDLED;
                    break;

                case ACT_FUNCTION_RetryLater:
                     state = 0;
                    return NOT_HANDLED;
					break;

                default:
                    state = 0;
                    return NOT_HANDLED;
                    break;
            }
			break;

        case MOVE_REAR:
            sub_action = goto_pos_with_scan_foe((displacement_t[]){{{1730,COLOR_Y(600)}}},1,REAR,NORMAL_WAIT);

            switch(sub_action)
            {
                case END_OK:
                    state=DONE;
                    break;

                case END_WITH_TIMEOUT:
                    state=DONE;
					break;
                case NOT_HANDLED:
                    state = 0;
                    return NOT_HANDLED;
                    break;

                case IN_PROGRESS:
                    break;

                default:
                    state = 0;
                    return NOT_HANDLED;
                    break;
            }
            break;
        case DONE:
            return (timeout) ? END_WITH_TIMEOUT : END_OK;
            break;
        default:
            state = 0;
            return NOT_HANDLED;
            break;
    }
	return NOT_HANDLED;
}



error_e TEST_LAUNCH_BALL(void){

	static error_e sub_action;
	static ACT_function_result_e sub_action_act;
	static enum {
		GO_POS = 0,
		GO_ANGLE,
		LAUNCH_RUN,
		WAIT,
		LAUNCH_STOP,
		DONE,
	} state = GO_POS;

	static bool_e timeout = FALSE;

	switch(state){
		case GO_POS:
			sub_action = goto_pos_with_scan_foe((displacement_t[]){{{1730,COLOR_Y(600)}}},1,REAR,NORMAL_WAIT);

            switch(sub_action)
            {
                case END_OK:
                    state=GO_ANGLE;
                    break;

                case END_WITH_TIMEOUT:
                    state=GO_ANGLE;
                    break;

                case NOT_HANDLED:
                    state = 0;
                    return NOT_HANDLED;
                    break;

                case IN_PROGRESS:
                    break;

                default:
                    state = 0;
                    return NOT_HANDLED;
                    break;
            }
            break;
		case GO_ANGLE:
		sub_action = goto_pos_with_scan_foe((displacement_t[]){{{1800,COLOR_Y(650)}}},1,REAR,NORMAL_WAIT);

            switch(sub_action)
            {
                case END_OK:
                    state=GO_ANGLE;
                    break;

                case END_WITH_TIMEOUT:
                    state=GO_ANGLE;
                    break;

                case NOT_HANDLED:
                    state = 0;
                    return NOT_HANDLED;
                    break;

                case IN_PROGRESS:
                    break;

                default:
                    state = 0;
                    return NOT_HANDLED;
                    break;
            }
            break;
		case LAUNCH_RUN:

			ACT_push_ball_launcher_run(6000,TRUE);
			sub_action_act = ACT_get_last_action_result(ACT_STACK_BallLauncher);
			switch(sub_action_act)
            {
                case ACT_FUNCTION_InProgress:
                    break;

                case ACT_FUNCTION_Done:
                    state=WAIT;
					break;

                case ACT_FUNCTION_ActDisabled:
                    state = 0;
                    return NOT_HANDLED;
                    break;

                case ACT_FUNCTION_RetryLater:
                     state = 0;
                    return NOT_HANDLED;
					break;

                default:
                    state = 0;
                    return NOT_HANDLED;
                    break;
            }
			break;
		case WAIT:



			//si plus de balle
			//state=LAUNCH_STOP;

			break;
		case LAUNCH_STOP:

			ACT_push_ball_launcher_stop(TRUE);
			sub_action_act = ACT_get_last_action_result(ACT_STACK_BallLauncher);
			            switch(sub_action_act)
            {
                case ACT_FUNCTION_InProgress:
                    break;

                case ACT_FUNCTION_Done:
                    state=DONE;
					break;

                case ACT_FUNCTION_ActDisabled:
                    state = 0;
                    return NOT_HANDLED;
                    break;

                case ACT_FUNCTION_RetryLater:
                     state = 0;
                    return NOT_HANDLED;
					break;

                default:
                    state = 0;
                    return NOT_HANDLED;
                    break;
            }
			 break;

		case DONE:
			return (timeout) ? END_WITH_TIMEOUT : END_OK;
			break;

		default :
			state = 0;
			return NOT_HANDLED;
			break;
	}
	return NOT_HANDLED;
}



//micro strat cadeau par amaury
error_e TEST_STRAT_CADEAU_1(void){
        static bool_e timeout = FALSE;
        static error_e sub_action;
    static ACT_function_result_e sub_action_act;
    static enum {
        GO_POS = 0,
        GRABBER_TIDY,
        MOVE_PUSH,
        MOVE_BACK,
        DONE,
    }state = GO_POS;

    switch(state)
    {
        case GO_POS:
            sub_action = goto_pos_with_scan_foe((displacement_t[]){{{300,COLOR_Y(490)}}},1,REAR,NORMAL_WAIT);

            switch(sub_action)
            {
                case END_OK:
                    state=MOVE_PUSH;
                    break;

                case END_WITH_TIMEOUT:
                    state=MOVE_PUSH;
                    break;

                case NOT_HANDLED:
                    state = 0;
                    return NOT_HANDLED;
                    break;

                case IN_PROGRESS:
                    break;

                default:
                    state = 0;
                    return NOT_HANDLED;
                    break;
            }
            break;
        case MOVE_PUSH:
            sub_action = goto_pos_with_scan_foe((displacement_t[]){{{260,COLOR_Y(490)}}},1,REAR,NORMAL_WAIT);

            switch(sub_action)
            {
                case END_OK:
                    state=GRABBER_TIDY;
                    break;

                case END_WITH_TIMEOUT:
                    state=GRABBER_TIDY;
                    break;

                case NOT_HANDLED:
                    state = 0;
                    return NOT_HANDLED;
                    break;

                case IN_PROGRESS:
                    break;

                default:
                    state = 0;
                    return NOT_HANDLED;
                    break;
            }
            break;
        case GRABBER_TIDY:
            //ACT_ball_grabber_tidy();
            ACT_push_plate_rotate_prepare(TRUE);
            sub_action_act = ACT_get_last_action_result(ACT_STACK_Plate);
            switch(sub_action_act)
            {
                case ACT_FUNCTION_InProgress:
                    break;

                case ACT_FUNCTION_Done:
                    state=MOVE_BACK;
                    break;

                case ACT_FUNCTION_ActDisabled:
                    state = 0;
                    return NOT_HANDLED;
                    break;

                case ACT_FUNCTION_RetryLater:
                     state = 0;
                    return NOT_HANDLED;
                    break;

                default:
                    state = 0;
                    return NOT_HANDLED;
                    break;
            }


            break;
        case MOVE_BACK:
            sub_action = goto_pos_with_scan_foe((displacement_t[]){{{300,COLOR_Y(489)}}},1,REAR,NORMAL_WAIT);

            switch(sub_action)
            {
                case END_OK:
                    state=DONE;
                    break;

                case END_WITH_TIMEOUT:
                    state=DONE;
                    break;

                case NOT_HANDLED:
                    state = 0;
                    return NOT_HANDLED;
                    break;

                case IN_PROGRESS:
                    break;

                default:
                    state = 0;
                    return NOT_HANDLED;
                    break;
            }
            break;

        case DONE:
            return (timeout) ? END_WITH_TIMEOUT : END_OK;
            break;

	default :
            state = 0;
            return NOT_HANDLED;
            break;
    }
	return NOT_HANDLED;
}

error_e TEST_STRAT_CADEAU_2(void){
        static bool_e timeout = FALSE;
        static error_e sub_action;
    static ACT_function_result_e sub_action_act;
    static enum {
        GO_POS = 0,
        GRABBER_TIDY,
        MOVE_PUSH,
        MOVE_BACK,
        DONE,
    }state = GO_POS;

    switch(state)
    {
        case GO_POS:
            sub_action = goto_pos_with_scan_foe((displacement_t[]){{{300,COLOR_Y(650)}}},1,REAR,NORMAL_WAIT);

            switch(sub_action)
            {
                case END_OK:
                    state=MOVE_PUSH;
                    break;

                case END_WITH_TIMEOUT:
                    state=MOVE_PUSH;
                    break;

                case NOT_HANDLED:
                    state = 0;
                    return NOT_HANDLED;
                    break;

                case IN_PROGRESS:
                    break;

                default:
                    state = 0;
                    return NOT_HANDLED;
                    break;
            }
            break;
        case MOVE_PUSH:
            sub_action = goto_pos_with_scan_foe((displacement_t[]){{{250,COLOR_Y(650)}}},1,REAR,NORMAL_WAIT);

            switch(sub_action)
            {
                case END_OK:
                    state=GRABBER_TIDY;
                    break;

                case END_WITH_TIMEOUT:
                    state=GRABBER_TIDY;
                    break;

                case NOT_HANDLED:
                    state = 0;
                    return NOT_HANDLED;
                    break;

                case IN_PROGRESS:
                    break;

                default:
                    state = 0;
                    return NOT_HANDLED;
                    break;
            }
            break;
        case GRABBER_TIDY:
            //ACT_ball_grabber_tidy();
            ACT_push_plate_rotate_prepare(TRUE);
            sub_action_act = ACT_get_last_action_result(ACT_STACK_Plate);
            switch(sub_action_act)
            {
                case ACT_FUNCTION_InProgress:
                    break;

                case ACT_FUNCTION_Done:
                    state=MOVE_BACK;
                    break;

                case ACT_FUNCTION_ActDisabled:
                    state = 0;
                    return NOT_HANDLED;
                    break;

                case ACT_FUNCTION_RetryLater:
                     state = 0;
                    return NOT_HANDLED;
                    break;

                default:
                    state = 0;
                    return NOT_HANDLED;
                    break;
            }


            break;
        case MOVE_BACK:
            sub_action = goto_pos_with_scan_foe((displacement_t[]){{{300,COLOR_Y(650)}}},1,REAR,NORMAL_WAIT);

            switch(sub_action)
            {
                case END_OK:
                    state=DONE;
                    break;

                case END_WITH_TIMEOUT:
                    state=DONE;
                    break;

                case NOT_HANDLED:
                    state = 0;
                    return NOT_HANDLED;
                    break;

                case IN_PROGRESS:
                    break;

                default:
                    state = 0;
                    return NOT_HANDLED;
                    break;
            }
            break;

        case DONE:
            return (timeout) ? END_WITH_TIMEOUT : END_OK;
            break;

	default :
            state = 0;
            return NOT_HANDLED;
            break;
    }
	return NOT_HANDLED;
}

error_e TEST_STRAT_CADEAU_3(void){
        static bool_e timeout = FALSE;
        static error_e sub_action;
    static ACT_function_result_e sub_action_act;
    static enum {
        GO_POS = 0,
        GRABBER_TIDY,
        MOVE_PUSH,
        MOVE_BACK,
        DONE,
    }state = GO_POS;

    switch(state)
    {
        case GO_POS:
            sub_action = goto_pos_with_scan_foe((displacement_t[]){{{300,COLOR_Y(650)}}},1,REAR,NORMAL_WAIT);

            switch(sub_action)
            {
                case END_OK:
                    state=MOVE_PUSH;
                    break;

                case END_WITH_TIMEOUT:
                    state=MOVE_PUSH;
                    break;

                case NOT_HANDLED:
                    state = 0;
                    return NOT_HANDLED;
                    break;

                case IN_PROGRESS:
                    break;

                default:
                    state = 0;
                    return NOT_HANDLED;
                    break;
            }
            break;
        case MOVE_PUSH:
            sub_action = goto_pos_with_scan_foe((displacement_t[]){{{250,COLOR_Y(650)}}},1,REAR,NORMAL_WAIT);

            switch(sub_action)
            {
                case END_OK:
                    state=GRABBER_TIDY;
                    break;

                case END_WITH_TIMEOUT:
                    state=GRABBER_TIDY;
                    break;

                case NOT_HANDLED:
                    state = 0;
                    return NOT_HANDLED;
                    break;

                case IN_PROGRESS:
                    break;

                default:
                    state = 0;
                    return NOT_HANDLED;
                    break;
            }
            break;
        case GRABBER_TIDY:
            //ACT_ball_grabber_tidy();
            ACT_push_plate_rotate_prepare(TRUE);
            sub_action_act = ACT_get_last_action_result(ACT_STACK_Plate);
            switch(sub_action_act)
            {
                case ACT_FUNCTION_InProgress:
                    break;

                case ACT_FUNCTION_Done:
                    state=MOVE_BACK;
                    break;

                case ACT_FUNCTION_ActDisabled:
                    state = 0;
                    return NOT_HANDLED;
                    break;

                case ACT_FUNCTION_RetryLater:
                     state = 0;
                    return NOT_HANDLED;
                    break;

                default:
                    state = 0;
                    return NOT_HANDLED;
                    break;
            }


            break;
        case MOVE_BACK:
            sub_action = goto_pos_with_scan_foe((displacement_t[]){{{300,COLOR_Y(650)}}},1,REAR,NORMAL_WAIT);

            switch(sub_action)
            {
                case END_OK:
                    state=DONE;
                    break;

                case END_WITH_TIMEOUT:
                    state=DONE;
                    break;

                case NOT_HANDLED:
                    state = 0;
                    return NOT_HANDLED;
                    break;

                case IN_PROGRESS:
                    break;

                default:
                    state = 0;
                    return NOT_HANDLED;
                    break;
            }
            break;

        case DONE:
            return (timeout) ? END_WITH_TIMEOUT : END_OK;
            break;

	default :
            state = 0;
            return NOT_HANDLED;
            break;
    }
	return NOT_HANDLED;
}

error_e TEST_STRAT_CADEAU_4(void){
        static bool_e timeout = FALSE;
        static error_e sub_action;
    static ACT_function_result_e sub_action_act;
    static enum {
        GO_POS = 0,
        GRABBER_TIDY,
        MOVE_PUSH,
        MOVE_BACK,
        DONE,
    }state = GO_POS;

    switch(state)
    {
        case GO_POS:
            sub_action = goto_pos_with_scan_foe((displacement_t[]){{{300,COLOR_Y(650)}}},1,REAR,NORMAL_WAIT);

            switch(sub_action)
            {
                case END_OK:
                    state=MOVE_PUSH;
                    break;

                case END_WITH_TIMEOUT:
                    state=MOVE_PUSH;
                    break;

                case NOT_HANDLED:
                    state = 0;
                    return NOT_HANDLED;
                    break;

                case IN_PROGRESS:
                    break;

                default:
                    state = 0;
                    return NOT_HANDLED;
                    break;
            }
            break;
        case MOVE_PUSH:
            sub_action = goto_pos_with_scan_foe((displacement_t[]){{{250,COLOR_Y(650)}}},1,REAR,NORMAL_WAIT);

            switch(sub_action)
            {
                case END_OK:
                    state=GRABBER_TIDY;
                    break;

                case END_WITH_TIMEOUT:
                    state=GRABBER_TIDY;
                    break;

                case NOT_HANDLED:
                    state = 0;
                    return NOT_HANDLED;
                    break;

                case IN_PROGRESS:
                    break;

                default:
                    state = 0;
                    return NOT_HANDLED;
                    break;
            }
            break;
        case GRABBER_TIDY:
            //ACT_ball_grabber_tidy();
            ACT_push_plate_rotate_prepare(TRUE);
            sub_action_act = ACT_get_last_action_result(ACT_STACK_Plate);
            switch(sub_action_act)
            {
                case ACT_FUNCTION_InProgress:
                    break;

                case ACT_FUNCTION_Done:
                    state=MOVE_BACK;
                    break;

                case ACT_FUNCTION_ActDisabled:
                    state = 0;
                    return NOT_HANDLED;
                    break;

                case ACT_FUNCTION_RetryLater:
                     state = 0;
                    return NOT_HANDLED;
                    break;

                default:
                    state = 0;
                    return NOT_HANDLED;
                    break;
            }


            break;
        case MOVE_BACK:
            sub_action = goto_pos_with_scan_foe((displacement_t[]){{{300,COLOR_Y(650)}}},1,REAR,NORMAL_WAIT);

            switch(sub_action)
            {
                case END_OK:
                    state=DONE;
                    break;

                case END_WITH_TIMEOUT:
                    state=DONE;
                    break;

                case NOT_HANDLED:
                    state = 0;
                    return NOT_HANDLED;
                    break;

                case IN_PROGRESS:
                    break;

                default:
                    state = 0;
                    return NOT_HANDLED;
                    break;
            }
            break;

        case DONE:
            return (timeout) ? END_WITH_TIMEOUT : END_OK;
            break;

	default :
            state = 0;
            return NOT_HANDLED;
            break;
    }
	return NOT_HANDLED;
}


//micro strat verre refait par amaury

error_e TEST_STRAT_verres(void){
    static error_e sub_action;
    static enum{
        PUSH_MOVE1 = 0,
        PUSH_MOVE2,
	DONE,
    }state = PUSH_MOVE1;

    static bool_e timeout=FALSE;

    switch(state){
        case PUSH_MOVE1:
            //ASSER_push_goto_multi_point(680,COLOR_Y(2405),FAST,FORWARD,ASSER_CURVES,END_OF_BUFFER,FALSE);
            //ASSER_push_goto_multi_point(675,COLOR_Y(2400),FAST,FORWARD,ASSER_CURVES,END_OF_BUFFER,FALSE);
            //ASSER_push_goto_multi_point(300,COLOR_Y(2300),FAST,FORWARD,ASSER_CURVES,END_OF_BUFFER,FALSE);
            //ASSER_push_goto_multi_point(300,COLOR_Y(580),FAST,FORWARD,ASSER_CURVES,NOW,FALSE);

            sub_action = goto_pos_with_scan_foe((displacement_t[]){{{300,COLOR_Y(580)},FAST},{{300,COLOR_Y(2300)},FAST},{{675,COLOR_Y(2400)},FAST},{{680,COLOR_Y(2405)},FAST}},4,FORWARD,NO_AVOIDANCE);

            switch(sub_action)
            {
                case END_OK:
					debug_printf("endok\n");
                    state=PUSH_MOVE2;
                    break;

                case END_WITH_TIMEOUT:
					debug_printf("TIMOUT\n");
                    state=PUSH_MOVE2;
                    break;

                case NOT_HANDLED:
                    state = 0;
					debug_printf("NH\n");
                    return NOT_HANDLED;
                    break;

                case IN_PROGRESS:
					debug_printf("PROGRESS\n");
                    break;

                default:
                    state = 0;
                    return NOT_HANDLED;
                    break;
            }
            break;
        case PUSH_MOVE2:
            //ASSER_push_goto_multi_point(700,COLOR_Y(420),FAST,REAR,ASSER_CURVES,END_OF_BUFFER,FALSE);
            //ASSER_push_goto_multi_point(1000,COLOR_Y(1000),FAST,REAR,ASSER_CURVES,END_OF_BUFFER,FALSE);
            //ASSER_push_goto_multi_point(1000,COLOR_Y(2200),FAST,REAR,ASSER_CURVES,END_OF_BUFFER,FALSE);
            //ASSER_push_goto_multi_point(1200,COLOR_Y(2400),FAST,FORWARD,ASSER_CURVES,END_OF_BUFFER,FALSE);
            //ASSER_push_goto_multi_point(675,COLOR_Y(2200),FAST,FORWARD,ASSER_CURVES,END_OF_BUFFER,FALSE);
            //ASSER_push_goto_multi_point(675,COLOR_Y(400),FAST,REAR,ASSER_CURVES,NOW,TRUE);

            sub_action = goto_pos_with_scan_foe((displacement_t[]){{{675,COLOR_Y(400)}},{{675,COLOR_Y(2200)}},{{1200,COLOR_Y(2400)}},{{1000,COLOR_Y(2200)}},{{1000,COLOR_Y(1000)}},{{700,COLOR_Y(420)}}},6,REAR,NORMAL_WAIT);

            switch(sub_action)
            {
                case END_OK:
                    state=DONE;
                    break;

                case END_WITH_TIMEOUT:
                    state=DONE;
                    break;

                case NOT_HANDLED:
                    state = 0;
                    return NOT_HANDLED;
                    break;

                case IN_PROGRESS:
                    break;

                default:
                    state = 0;
                    return NOT_HANDLED;
                    break;
            }
            break;

        case DONE:
            state = PUSH_MOVE1;
            return (timeout)?END_WITH_TIMEOUT:END_OK;
            break;
        default:
            state = 0;
            return NOT_HANDLED;
            break;
    }
    return IN_PROGRESS;
}

error_e TEST_STRAT_in_da_wall(void)
{
    static enum{
        PUSH_MOVE = 0,
	WAIT_END_OF_MOVE,
	DONE,
    }state = PUSH_MOVE;

    static bool_e timeout=FALSE;

    switch(state){
        case PUSH_MOVE:
            debug_printf("in da wall");
            //Utiliser rush in da wall de cette forme la
           //ASSER_push_rush_in_the_wall (ASSER_way_e way, bool_e asser_rotate,Sint16 angle, bool_e run)
            ASSER_push_rush_in_the_wall(REAR,TRUE,0,TRUE);
            state = WAIT_END_OF_MOVE;
            break;
        case WAIT_END_OF_MOVE:
            if(STACKS_wait_end_auto_pull(ASSER, &timeout)){
                state = DONE;
            }
            break;
        case DONE:
            state = PUSH_MOVE;
            debug_printf("%s",(timeout)?"timeout\n":"c'estbon\n");
            return (timeout)?END_WITH_TIMEOUT:END_OK;
            break;
        default:
            state = 0;
            return NOT_HANDLED;
            break;
    }
    return IN_PROGRESS;
}
/*
error_e TEST_STRAT_first_half_cake(void)
{
    static enum{
        PUSH_MOVE = 0,
	WAIT_END_OF_MOVE,
	DONE,
    }state = PUSH_MOVE;

    static bool_e timeout=FALSE;
    
    switch(state){
        case PUSH_MOVE:
            ASSER_push_goto_multi_point(1453,COLOR_Y(3000-1447),DEFAULT_SPEED,FORWARD,ASSER_CURVES,END_OF_BUFFER,FALSE);
            ASSER_push_goto_multi_point(1463,COLOR_Y(3000-1381),DEFAULT_SPEED,FORWARD,ASSER_CURVES,END_OF_BUFFER,FALSE);
            ASSER_push_goto_multi_point(1482,COLOR_Y(3000-1316),DEFAULT_SPEED,FORWARD,ASSER_CURVES,END_OF_BUFFER,FALSE);
            ASSER_push_goto_multi_point(1508,COLOR_Y(3000-1255),DEFAULT_SPEED,FORWARD,ASSER_CURVES,END_OF_BUFFER,FALSE);
            ASSER_push_goto_multi_point(1541,COLOR_Y(3000-1196),DEFAULT_SPEED,FORWARD,ASSER_CURVES,END_OF_BUFFER,FALSE);
            ASSER_push_goto_multi_point(1582,COLOR_Y(3000-1143),DEFAULT_SPEED,FORWARD,ASSER_CURVES,END_OF_BUFFER,FALSE);
            ASSER_push_goto_multi_point(1628,COLOR_Y(3000-1094),DEFAULT_SPEED,FORWARD,ASSER_CURVES,END_OF_BUFFER,FALSE);
            ASSER_push_goto_multi_point(1681,COLOR_Y(3000-1052),DEFAULT_SPEED,FORWARD,ASSER_CURVES,END_OF_BUFFER,FALSE);
            ASSER_push_goto_multi_point(1738,COLOR_Y(3000-1017),DEFAULT_SPEED,FORWARD,ASSER_CURVES,END_OF_BUFFER,FALSE);
            ASSER_push_goto_multi_point(1798,COLOR_Y(3000-988),DEFAULT_SPEED,FORWARD,ASSER_CURVES,END_OF_BUFFER,FALSE);
            ASSER_push_goto_multi_point(1862,COLOR_Y(3000-968),DEFAULT_SPEED,FORWARD,ASSER_CURVES,END_OF_BUFFER,FALSE);
            ASSER_push_goto_multi_point(1928,COLOR_Y(3000-955),DEFAULT_SPEED,FORWARD,ASSER_CURVES,END_OF_BUFFER,TRUE);
            
//            ASSER_push_goto_multi_point(1862,COLOR_Y(3000-968),DEFAULT_SPEED,FORWARD,ASSER_CURVES,END_OF_BUFFER,FALSE);
//            ASSER_push_goto_multi_point(1928,COLOR_Y(3000-800),DEFAULT_SPEED,FORWARD,ASSER_CURVES,END_OF_BUFFER,TRUE);
            ASSER_push_goto(1500,COLOR_Y(3000-800),SLOW,FORWARD,0,TRUE);
            state = WAIT_END_OF_MOVE;
            break;
        case WAIT_END_OF_MOVE:
            if(STACKS_wait_end_auto_pull(ASSER, &timeout)){
                state = DONE;
            }
            break;
        case DONE:
            state = PUSH_MOVE;
            return (timeout)?END_WITH_TIMEOUT:END_OK;
            break;
        default:
            state = 0;
            return NOT_HANDLED;
            break;
    }
    return IN_PROGRESS;
}
*/

error_e TEST_STRAT_first_half_cake(void)
{
    static enum{
        PUSH_MOVE = 0,
	WAIT_END_OF_MOVE,
	DONE,
    }state = PUSH_MOVE;

    static bool_e timeout=FALSE;

//    Uint16 i;

//    static GEOMETRY_point_t points_gateau[25]={{1909,800},{1825,823},{1744,849},{1666,885},{1594,931},{1528,984},{1468,1046},{1417,1114},{1374,1188},{1341,1267},{1317,1349},{1304,1433},{1301,1519},{1308,1604},{1326,1688},{1354,1768},{1391,1845},{1438,1917},{1493,1982},{1555,2041},{1624,2091},{1699,2132},{1778,2164},{1861,2186},{1946,2198}};

    switch(state){
        case PUSH_MOVE:
//            for (i=20;i>0;i--) {
//                ASSER_push_goto_multi_point(points_gateau[i].x,COLOR_Y(points_gateau[i].y),SLOW,FORWARD,ASSER_CURVES,END_OF_BUFFER,FALSE);
//            }
            ASSER_push_goto(600,COLOR_Y(300),SLOW,FORWARD,0,FALSE);
            ASSER_push_goto(1775,COLOR_Y(800),SLOW,FORWARD,0,TRUE);
 //           ASSER_push_goto(1500,COLOR_Y(3000-800),SLOW,FORWARD,0,TRUE);
            state = WAIT_END_OF_MOVE;
            break;
        case WAIT_END_OF_MOVE:
            if(STACKS_wait_end_auto_pull(ASSER, &timeout)){
                state = DONE;
            }
            break;
        case DONE:
            state = PUSH_MOVE;
            return (timeout)?END_WITH_TIMEOUT:END_OK;
            break;
        default:
            state = 0;
            return NOT_HANDLED;
            break;
    }
    return IN_PROGRESS;
}


error_e TEST_STRAT_second_half_cake(void)
{
    static enum{
        PUSH_MOVE = 0,
	WAIT_END_OF_MOVE,
	DONE,
    }state = PUSH_MOVE;

    static bool_e timeout=FALSE;

    switch(state){
        case PUSH_MOVE:
            ASSER_push_goto_multi_point(1957,COLOR_Y(3000-2048),DEFAULT_SPEED,FORWARD,ASSER_CURVES,END_OF_BUFFER,FALSE);
            ASSER_push_goto_multi_point(1890,COLOR_Y(3000-2039),DEFAULT_SPEED,FORWARD,ASSER_CURVES,END_OF_BUFFER,FALSE);
            ASSER_push_goto_multi_point(1825,COLOR_Y(3000-2022),DEFAULT_SPEED,FORWARD,ASSER_CURVES,END_OF_BUFFER,FALSE);
            ASSER_push_goto_multi_point(1763,COLOR_Y(3000-1996),DEFAULT_SPEED,FORWARD,ASSER_CURVES,END_OF_BUFFER,FALSE);
            ASSER_push_goto_multi_point(1704,COLOR_Y(3000-1964),DEFAULT_SPEED,FORWARD,ASSER_CURVES,END_OF_BUFFER,FALSE);
            ASSER_push_goto_multi_point(1650,COLOR_Y(3000-1924),DEFAULT_SPEED,FORWARD,ASSER_CURVES,END_OF_BUFFER,FALSE);
            ASSER_push_goto_multi_point(1601,COLOR_Y(3000-1879),DEFAULT_SPEED,FORWARD,ASSER_CURVES,END_OF_BUFFER,FALSE);
            ASSER_push_goto_multi_point(1558,COLOR_Y(3000-1827),DEFAULT_SPEED,FORWARD,ASSER_CURVES,END_OF_BUFFER,FALSE);
            ASSER_push_goto_multi_point(1521,COLOR_Y(3000-1771),DEFAULT_SPEED,FORWARD,ASSER_CURVES,END_OF_BUFFER,FALSE);
            ASSER_push_goto_multi_point(1492,COLOR_Y(3000-1710),DEFAULT_SPEED,FORWARD,ASSER_CURVES,END_OF_BUFFER,FALSE);
            ASSER_push_goto_multi_point(1470,COLOR_Y(3000-1647),DEFAULT_SPEED,FORWARD,ASSER_CURVES,END_OF_BUFFER,FALSE);
            ASSER_push_goto_multi_point(1456,COLOR_Y(3000-1581),DEFAULT_SPEED,FORWARD,ASSER_CURVES,END_OF_BUFFER,FALSE);
            ASSER_push_goto_multi_point(1450,COLOR_Y(3000-1514),DEFAULT_SPEED,FORWARD,ASSER_CURVES,END_OF_BUFFER,TRUE);
            state = WAIT_END_OF_MOVE;
            break;
        case WAIT_END_OF_MOVE:
            if(STACKS_wait_end_auto_pull(ASSER, &timeout)){
                state = DONE;
            }
            break;
        case DONE:
            state = PUSH_MOVE;
            return (timeout)?END_WITH_TIMEOUT:END_OK;
            break;
        default:
            state = 0;
            return NOT_HANDLED;
            break;
    }
    return IN_PROGRESS;
}

//Alexis; test des fonctions act_function
void TEST_STRAT_balllauncher_run(void){
    static int pushed = 0;
    if(pushed == 0){
		debug_printf("Send ball launcher request to run\n");
        ACT_push_ball_launcher_run(6000, TRUE);
        pushed = 1;
    }

	if(pushed == 1 && ACT_get_last_action_result(ACT_STACK_BallLauncher) != ACT_FUNCTION_InProgress) {
		ACT_function_result_e result = ACT_get_last_action_result(ACT_STACK_BallLauncher);
		switch(result) {
			case ACT_FUNCTION_Done:
				debug_printf("Operation done\n");
				break;
			case ACT_FUNCTION_ActDisabled:
				debug_printf("Operation failed, act is disabled\n");
				break;
			case ACT_FUNCTION_RetryLater:
				debug_printf("Operation failed, strat should try later\n");
			case ACT_FUNCTION_InProgress:
				break;
		}
		pushed = 2;
	}

}

/* ----------------------------------------------------------------------------- */
/* 								Fonction diverses                     			 */
/* ----------------------------------------------------------------------------- */

void set_calage(void)
{
    CAN_msg_t msg;
    msg.sid = ASSER_SET_POSITION;
    msg.size = 0x06;
    msg.data[0] = 0x02;
    msg.data[1] = 0x5F;
    msg.data[2] = 0x0A;
    msg.data[3] = 0xBE;
    msg.data[4] = 0x19;
    msg.data[5] = 0x66;
    CAN_send(&msg);
}

/* ----------------------------------------------------------------------------- */
/* 							Découpe de stratégies                     			 */
/* ----------------------------------------------------------------------------- */

void TEST_STRAT_gateau(void)
{
    static enum{
                APPROACH = 0,
                WAIT_APPROACH,
                PUSH,
                WAIT_PUSH,
                FIRST_HALF_CAKE,
                WAIT_FIRST_HALF_CAKE,
                SECOND_HALF_CAKE,
                WAIT_SECOND_HALF_CAKE,
                DONE,
    }state = APPROACH;

    bool_e timeout;

    switch(state){
        case APPROACH:
            ASSER_push_goto(1600,COLOR_Y(3000-800),FAST,REAR,0,TRUE);
            state = WAIT_APPROACH;
            break;

        case WAIT_APPROACH:
            if(STACKS_wait_end_auto_pull(ASSER, &timeout))
                state = PUSH;
            break;

        case PUSH:
            ASSER_push_rush_in_the_wall(REAR,TRUE,PI4096,TRUE);
            state = WAIT_PUSH;
            break;

        case WAIT_PUSH:
            if(STACKS_wait_end_auto_pull(ASSER, &timeout))
                state = FIRST_HALF_CAKE;
            break;

        case FIRST_HALF_CAKE:
            ASSER_push_goto(1000,COLOR_Y(3000-800),FAST,FORWARD,0,TRUE);
            state = WAIT_FIRST_HALF_CAKE;
            break;

        case WAIT_FIRST_HALF_CAKE:
            if(STACKS_wait_end_auto_pull(ASSER, &timeout))
                state = DONE;
            break;

        case SECOND_HALF_CAKE:
            state = WAIT_SECOND_HALF_CAKE;
            break;

        case WAIT_SECOND_HALF_CAKE:
            if(STACKS_wait_end_auto_pull(ASSER, &timeout))
                state = DONE;
            break;
        case DONE:
            break;
        default:
            state = 0;
            break;
    }
}



void TEST_STRAT_gatoacsuba(void)
{
    static enum{
                APPROACH = 0,
                PUSH,
                FIRST_HALF_CAKE,
                SECOND_HALF_CAKE,
                DONE,
                TIMEOUT,
    }state = APPROACH;
    
    static error_e sub_action;

    switch(state){
        case APPROACH: //Approche de la zone du gateau pour engager le rush in the wall
            sub_action = goto_pos(1600,COLOR_Y(800),FAST,REAR);
            
            debug_printf("jy vais\n");

            switch(sub_action)
            {
                case END_OK:

                    debug_printf("fini\n");

                    state = PUSH;
                    break;
                case END_WITH_TIMEOUT:
                    debug_printf("bug TO\n");
                    state = TIMEOUT;
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

        case PUSH:
            debug_printf("wall\n");
            sub_action = TEST_STRAT_in_da_wall();
            switch(sub_action){
                case END_OK:

                    //debug_printf("dan le mur");

                    state = FIRST_HALF_CAKE;
                    break;
                case END_WITH_TIMEOUT:
                    debug_printf("TO Wall\n");
                    state = FIRST_HALF_CAKE;
                    break;
                case NOT_HANDLED:
                    state = DONE;
                    break;
                case IN_PROGRESS:
                    break;
            }
            break;

        case FIRST_HALF_CAKE:
            debug_printf("FHC\n");
            sub_action = TEST_STRAT_first_half_cake();

            switch(sub_action)
            {
                case END_OK:
                    state = SECOND_HALF_CAKE;
                    break;
                case END_WITH_TIMEOUT:
                    state = DONE;
                    break;
                case NOT_HANDLED:
                    state = DONE;
                    break;
                case IN_PROGRESS:
                    break;
                default:
                    break;
            }
            state = DONE;
            break;

        case SECOND_HALF_CAKE:
            state = DONE;
            break;

        case DONE:
            break;
        case TIMEOUT:
            debug_printf("GOT TO\n");
            sub_action = goto_pos(1000,COLOR_Y(800),FAST,ANY_WAY);

            switch(sub_action)
            {
                case END_OK:
                    state = DONE;
                    break;
                case END_WITH_TIMEOUT:
                    state = TIMEOUT;
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
        default:
            state = 0;
            break;
    }
}

//By Julien alias jaune

error_e TEST_STRAT_VERRE1(void) {

    static enum {
        EMPILE,
        WAIT,
        DONE,
    } state = EMPILE;

    static bool_e timeout = FALSE;

    switch (state) {
        case EMPILE:

            ASSER_push_goto_multi_point(680, COLOR_Y(2405), FAST, REAR, ASSER_CURVES, END_OF_BUFFER, FALSE);
            ASSER_push_goto_multi_point(675, COLOR_Y(2400), FAST, REAR, ASSER_CURVES, END_OF_BUFFER, FALSE);
            ASSER_push_goto_multi_point(300, COLOR_Y(2300), FAST, REAR, ASSER_CURVES, END_OF_BUFFER, FALSE);
            ASSER_push_goto_multi_point(300, COLOR_Y(580), FAST, REAR, ASSER_CURVES, NOW, TRUE);

           //ASSER_push_goangle(PI4096 / 2, SLOW, TRUE);
            state = WAIT;
            break;
        case WAIT:
            if (STACKS_wait_end_auto_pull(ASSER, &timeout)) {
                state = DONE;
            }
            break;
        case DONE:
            state = EMPILE;
            return (timeout) ? END_WITH_TIMEOUT : END_OK;
            break;
        default:
            state = 0;
            return NOT_HANDLED;
            break;
    }
    return IN_PROGRESS;
}

//rammasse des lignes top et milieu des verres

error_e TEST_STRAT_VERRE2(void) {

    static enum {
        EMPILE,
        WAIT,
        DONE,
    } state = EMPILE;

    static bool_e timeout = FALSE;

    switch (state) {
        case EMPILE:

            ASSER_push_goto_multi_point(700, COLOR_Y(420), FAST, FORWARD, ASSER_CURVES, END_OF_BUFFER, FALSE);
            ASSER_push_goto_multi_point(1000, COLOR_Y(1000), FAST, FORWARD, ASSER_CURVES, END_OF_BUFFER, FALSE);
            ASSER_push_goto_multi_point(1000, COLOR_Y(2200), FAST, FORWARD, ASSER_CURVES, END_OF_BUFFER, FALSE);
            ASSER_push_goto_multi_point(1200, COLOR_Y(2400), FAST, REAR, ASSER_CURVES, END_OF_BUFFER, FALSE);
            ASSER_push_goto_multi_point(675, COLOR_Y(2200), FAST, REAR, ASSER_CURVES, END_OF_BUFFER, FALSE);
            ASSER_push_goto_multi_point(675, COLOR_Y(400), FAST, FORWARD, ASSER_CURVES, NOW, TRUE);
            state = WAIT;




            break;
        case WAIT:
            if (STACKS_wait_end_auto_pull(ASSER, &timeout)) {
                state = DONE;
            }
            break;
        case DONE:
            state = EMPILE;
            return (timeout) ? END_WITH_TIMEOUT : END_OK;
            break;
        default:
            state = 0;
            return NOT_HANDLED;
            break;
    }
    return IN_PROGRESS;
}

//assiettes

//le robot se place en (600, COLOR_Y(600)) pour débuter la stratégie des assiettes

error_e STRAT_DEBUT_ASSIETTE(void) {

    static enum {
        EMPILE,
        WAIT,
        DONE,
    } state = EMPILE;

    static bool_e timeout = FALSE;

    switch (state) {
        case EMPILE:
            ASSER_push_goto(600, COLOR_Y(600), FAST, REAR, 0, TRUE);
            state = WAIT;
            break;
        case WAIT:
            if (STACKS_wait_end_auto_pull(ASSER, &timeout)) {
                state = DONE;
            }
            break;
        case DONE:
            state = EMPILE;
            return (timeout) ? END_WITH_TIMEOUT : END_OK;
            break;
        default:
            state = 0;
            return NOT_HANDLED;
            break;
    }
    return IN_PROGRESS;
}

// on écarte un peu le robot des assiettes en (1730, COLOR_Y(807)) une fois qu'elles sont toutes faites

error_e STRAT_FIN_ASSIETTE(void) {

    static enum {
        EMPILE,
        WAIT,
        DONE,
    } state = EMPILE;

    static bool_e timeout = FALSE;

    switch (state) {
        case EMPILE:
            ASSER_push_goto(1730, COLOR_Y(807), FAST, REAR, 0, TRUE);
            state = WAIT;
            break;
        case WAIT:
            if (STACKS_wait_end_auto_pull(ASSER, &timeout)) {
                state = DONE;
            }
            break;
        case DONE:
            state = EMPILE;
            return (timeout) ? END_WITH_TIMEOUT : END_OK;
            break;
        default:
            state = 0;
            return NOT_HANDLED;
            break;
    }
    return IN_PROGRESS;
}

//assiette de notre coté en bas

error_e STRAT_PREMIERE_ASSIETTE(void) {

    static enum {
        EMPILE,
        WAIT,
        DONE,
    } state = EMPILE;

    static bool_e timeout = FALSE;

    switch (state) {
        case EMPILE:
            ASSER_push_goto(250, COLOR_Y(600), FAST, REAR, 0, FALSE);
            ASSER_push_goto(250, COLOR_Y(380), FAST, FORWARD, 0, FALSE);
            ASSER_push_goto(250, COLOR_Y(600), FAST, FORWARD, 0, TRUE);
            state = WAIT;
            break;
        case WAIT:
            if (STACKS_wait_end_auto_pull(ASSER, &timeout)) {
                state = DONE;
            }
            break;
        case DONE:
            state = EMPILE;
            return (timeout) ? END_WITH_TIMEOUT : END_OK;
            break;
        default:
            state = 0;
            return NOT_HANDLED;
            break;
    }
    return IN_PROGRESS;
}

//assiette de notre coté sur la case bleue du milieu

error_e STRAT_SECONDE_ASSIETTE(void) {

    static enum {
        EMPILE,
        WAIT,
        DONE,
    } state = EMPILE;

    static bool_e timeout = FALSE;

    switch (state) {
        case EMPILE:
            ASSER_push_goto(1000, COLOR_Y(600), FAST, REAR, 0, FALSE);
            ASSER_push_goto(1000, COLOR_Y(380), FAST, FORWARD, 0, FALSE);
            ASSER_push_goto(1000, COLOR_Y(600), FAST, FORWARD, 0, TRUE);
            state = WAIT;
            break;
        case WAIT:
            if (STACKS_wait_end_auto_pull(ASSER, &timeout)) {
                state = DONE;
            }
            break;
        case DONE:
            state = EMPILE;
            return (timeout) ? END_WITH_TIMEOUT : END_OK;
            break;
        default:
            state = 0;
            return NOT_HANDLED;
            break;
    }
    return IN_PROGRESS;
}

//assiette sur case blanche du haut

error_e STRAT_TROISIEME_ASSIETTE(void) {

    static enum {
        EMPILE,
        WAIT,
        DONE,
    } state = EMPILE;

    static bool_e timeout = FALSE;

    switch (state) {
        case EMPILE:
            ASSER_push_goto(1400, COLOR_Y(600), FAST, REAR, 0, FALSE);
            ASSER_push_goto(1400, COLOR_Y(380), FAST, FORWARD, 0, FALSE);
            ASSER_push_goto(1400, COLOR_Y(600), FAST, FORWARD, 0, TRUE);
            state = WAIT;
            break;
        case WAIT:
            if (STACKS_wait_end_auto_pull(ASSER, &timeout)) {
                state = DONE;
            }
            break;
        case DONE:
            state = EMPILE;
            return (timeout) ? END_WITH_TIMEOUT : END_OK;
            break;
        default:
            state = 0;
            return NOT_HANDLED;
            break;
    }
    return IN_PROGRESS;
}

//assiette sur la case bleue du haut

error_e STRAT_DERNIERE_ASSIETTE(void) {

    static enum {
        EMPILE,
        WAIT,
        DONE,
    } state = EMPILE;

    static bool_e timeout = FALSE;

    switch (state) {
        case EMPILE:
            ASSER_push_goto(1730, COLOR_Y(380), FAST, FORWARD, 0, FALSE);
            ASSER_push_goto(1730, COLOR_Y(600), FAST, FORWARD, 0, TRUE);
            state = WAIT;
            break;
        case WAIT:
            if (STACKS_wait_end_auto_pull(ASSER, &timeout)) {
                state = DONE;
            }
            break;
        case DONE:
            state = EMPILE;
            return (timeout) ? END_WITH_TIMEOUT : END_OK;
            break;
        default:
            state = 0;
            return NOT_HANDLED;
            break;
    }
    return IN_PROGRESS;
}

//si le robot ne peut finir de faire les assiettes de notre coté il s'écarte pour allez en (600,color_Y(900))

error_e STRAT_TIMEOUT_ASSIETTE(void) {

    static enum {
        EMPILE,
        WAIT,
        DONE,
    } state = EMPILE;

    static bool_e timeout = FALSE;

    switch (state) {
        case EMPILE:
            ASSER_push_goto(600, COLOR_Y(900), FAST, FORWARD, 0, FALSE);
            ASSER_push_goto(600, COLOR_Y(600), FAST, FORWARD, 0, TRUE);
            state = WAIT;
            break;
        case WAIT:
            if (STACKS_wait_end_auto_pull(ASSER, &timeout)) {
                state = DONE;
            }
            break;
        case DONE:
            state = EMPILE;
            return (timeout) ? END_WITH_TIMEOUT : END_OK;
            break;
        default:
            state = 0;
            return NOT_HANDLED;
            break;
    }
    return IN_PROGRESS;
}

/* ----------------------------------------------------------------------------- */
/* 							Tests state_machines multiple              			 */
/* ----------------------------------------------------------------------------- */
