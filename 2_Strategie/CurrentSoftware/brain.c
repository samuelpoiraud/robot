/*
 *	Club Robot ESEO 2008 - 2012
 *	Archi'Tech, CHOMP, CheckNorris, Shark & Fish
 *
 *	Fichier : brain.c
 *	Package : Carte Principale
 *	Description : Fonctions de génération des ordres
 *	Auteur : Jacen, modifié par Gonsevi
 *	Version 2012/01/14
 */

#define BRAIN_C

#include "brain.h"

#include "can_utils.h"
#include "Stacks.h"
#include "actions.h"
#include "clock.h"
#include "QS/QS_CANmsgList.h"
#include "actions_tests_krusty.h"
#include "actions_tests_tiny.h"
#include "actions_tests_tiny_micro.h" // pour T_BALLINFLATER_start
#include "actions.h"
#include "QS/QS_who_am_i.h"
#include "button.h"	//pour SWITCH_change_color

/* 	execute un match de match_duration secondes à partir de la 
	liberation de la biroute. Arrete le robot à la fin du match.
	Appelle une autre routine pour l'IA pendant le match.
	Une durée de 0 indique un match infini
*/
void any_match(time32_t match_duration)
{
	static ia_fun_t strategy;
	static calibration_square_e calibration = ASSER_CALIBRATION_SQUARE_1;
	static way_e calibration_way = ANY_WAY;
	if (!global.env.match_started)
	{
		/* we are before the match */
		/* regarder si le match doit commencer */
		if (global.env.ask_start)
		{
			CLOCK_run_match();
			CAN_send_sid(BROADCAST_START);
			CAN_send_sid(XBEE_START_MATCH);
			
			return;
		}

		SWITCH_change_color();	//Check the Color switch

		/* accepter et prévenir des mises à jour de couleur (BLUE par défaut) */
		if(global.env.color_updated && !global.env.asser.calibrated && !global.env.ask_asser_calibration)
		{
			ENV_set_color(global.env.wanted_color);
		}
		
		ENV_XBEE_ping_process();
		
		/* mise à jour de la configuration de match */
		if(global.env.config_updated)
		{
			global.env.config = global.env.wanted_config;
			ENV_dispatch_config();
		}

		/* demande de calibration */
		if(global.env.ask_asser_calibration && !global.env.asser.calibrated)
		{
			CAN_msg_t msg;
			msg.sid = ASSER_CALIBRATION;
			msg.data[0] = (Uint8)calibration_way;
			msg.data[1] = (Uint8)calibration;
			msg.size = 2;
			CAN_send(&msg);
		}

		/*************************/
		/* Choix de la stratégie */
		/*************************/

		if(QS_WHO_AM_I_get()==TINY)
		{
			calibration_way = (global.env.color == BLUE)?FORWARD:BACKWARD;	//En bleu, TINY se cale en avant pour avoir son bras coté cadeaux.
			calibration = ASSER_CALIBRATION_SQUARE_0;						//Sauf si décision contraire dans certaines stratégies... Tiny est par défaut dans la case 0.
			
			switch(strat_number())
			{
				case 0x01:	//STRAT_1_TINY
					//strategy = STRAT_TINY_all_candles;
					strategy = STRAT_TINY_gifts_cake_and_steal;
				break;
				case 0x02:	//STRAT_2_TINY : STRATEGIE DE TEST HORS MATCH
					//strategy = TEST_STRAT_T_homologation;
					//strategy = STRAT_TINY_all_candles;
					//strategy = STRAT_TINY_test_avoidance_goto_pos_no_dodge_and_wait;
					strategy = STRAT_TINY_test_steals;
				break;
				case 0x03:	//STRAT_3_TINY
					strategy = STRAT_TINY_4_gifts_only;
					
				break;
				case 0x00:	//STRAT_0_TINY (aucun switch)
				//no break;
				default:
					//strategy = TEST_STRAT_T_homologation;
					strategy = STRAT_TINY_gifts_cake_and_steal;
				break;
			}			
		}
		else	//Krusty
		{
			calibration_way = BACKWARD;	//Krusty se cale TOUJOURS en backward (pas de callage contre l'ascenseur à verres)
			calibration = ASSER_CALIBRATION_SQUARE_2; //Et toujours à l'assiette 2 (sachant que Tiny est sur la 0)
			
			switch(strat_number())
			{
				case 0x01:	//STRAT_1_KRUSTY
					//strategy = TEST_STRAT_avoidance;
					strategy = &TEST_STRAT_ALEXIS;
				break;

				case 0x02:	//STRAT_2_KRUSTY
					strategy = K_Strat_Coupe;
				break;

				case 0x03:	//STRAT_3_KRUSTY
					strategy = K_test_strat_unitaire;
				break;

				case 0x00:	//STRAT_0_KRUSTY (aucun switch)
				//no break;

				default:
					strategy = TEST_STRAT_K_homologation;
				break;
			}
		}
	}
	else
	{
		/* we are in match*/
		if(!global.env.match_over)
		{
			if(match_duration != 0 && (global.env.match_time >= (match_duration)))
			{	
				//MATCH QUI SE TERMINE
				CAN_send_sid(BROADCAST_STOP_ALL);
				global.env.match_over = TRUE;
				STACKS_flush_all();
				QUEUE_reset_all();
				CLOCK_stop();
				
				if(QS_WHO_AM_I_get()==TINY)
					T_BALLINFLATER_start();	//Fin du match -> On gonfle le ballon	
			}
			else
			{
				//MATCH EN ATTENTE DE LANCEMENT
				#ifdef USE_SCHEDULED_POSITION_REQUEST
					static bool_e initialized = FALSE;
					CAN_msg_t msg;
					if(!initialized)
					{
						initialized = TRUE;
						msg.sid = ASSER_SEND_PERIODICALLY_POSITION;
						msg.data[0] = 0; 
						msg.data[1] = 0; 					//toutes les XX ms -> si 0, pas de msg en fonction du temps.
						msg.data[2] = HIGHINT(20);
						msg.data[3] = LOWINT(20); 			//tout les 20 mm
						msg.data[4] = HIGHINT(PI4096/45);
						msg.data[5] = LOWINT(PI4096/45);  	//tout les 4°
						msg.size = 6;
						CAN_send(&msg);
					}	
				#endif /* def USE_SCHEDULED_POSITION_REQUEST */
				
				if(global.env.config.balise)
				{
					static bool_e initialized = FALSE;
					if(!initialized)
					{
						initialized = TRUE;
						CAN_send_sid(BEACON_ENABLE_PERIODIC_SENDING);
					}
				}	
							
				//programme du match
				if(strategy)
				{
					(*strategy)();
				}
			}
		}
		else
		{
			/* match is over */
		}	
	}
}

Uint8 strat_number(void)
{
	#ifndef FDP_2013
		return 0x00;
	#else
		if(SWITCH_STRAT_1)
			return 0x01;
		else if(SWITCH_STRAT_2)
			return 0x02;
		else if(SWITCH_STRAT_3)
			return 0x03;
	#endif	
	return 0x00;	//Aucun switch -> stratégie 0.
}

error_e STRAT_TINY_test_moisson_micro(void){

	typedef enum{
		GO_INIT = 0,
		GO_HOME,
		P_1,
		P_2,
		P_3,
		P_4,
		DONE

	}state_e;
	/*
	END_OK=0,
		IN_PROGRESS,
		END_WITH_TIMEOUT,
		NOT_HANDLED,
		FOE_IN_PATH
	 */


	error_e ret = NOT_HANDLED;
	static state_e state = GO_INIT;
	static state_e previousState = GO_INIT;

	static Uint8 count = 0;

	switch(state){
		case GO_INIT:
			count = 0;
			state = P_1;
			previousState = GO_INIT;
			ret = IN_PROGRESS;
			break;
		case P_1:
			//									  in_progress	success	failed
			state = try_going(300, COLOR_Y(2000), P_1, P_2, GO_HOME,FORWARD,NO_DODGE_AND_WAIT);
			previousState = P_1;
			ret = IN_PROGRESS;
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
			ret = IN_PROGRESS;
			break;
		case P_3:
			//risque de boucle entre P_2 P_4
			state = try_going(900, COLOR_Y(2000), P_3, P_4, P_2,FORWARD,NO_DODGE_AND_WAIT);
			previousState = P_3;
			ret = IN_PROGRESS;
			break;

		case P_4:
			//risque de boucle P_1 P_2 P_4
			state = try_going(800, COLOR_Y(1000), P_4, GO_HOME,P_1,FORWARD,NO_DODGE_AND_WAIT);
			previousState = P_4;
			ret = IN_PROGRESS;
			break;

		case GO_HOME:

			switch(previousState){
				case P_1:
					//non
					 ret = NOT_HANDLED;
					state = DONE;
					break;
				case P_2:
					state = try_going(300, COLOR_Y(300), GO_HOME,P_3 ,DONE,FORWARD,NO_DODGE_AND_WAIT);
					ret = IN_PROGRESS;
					break;
				case P_3:
					//non
					ret = NOT_HANDLED;
					state = DONE;
					break;
				case P_4:
					//a la fin
					state = DONE;
					break;
				case GO_INIT:
					// non pas possible
					 ret = NOT_HANDLED;
					state = DONE;
					break;
				case GO_HOME:
					//non pas possible
					ret = NOT_HANDLED;
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
			ret = END_OK;
			ACT_plier_close();
			break;
		default:

			break;
	}
	return ret;

}
