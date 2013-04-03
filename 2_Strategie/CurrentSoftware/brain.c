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
	static Uint8 calibration = 0x01;
	if (!global.env.match_started)
	{
		/* we are before the match */
		/* regarder si le match doit commencer */
		if (global.env.ask_start)
		{
			CLOCK_run_match();
			CAN_send_sid(BROADCAST_START);
			CAN_send_sid(XBEE_START_MATCH);
			LED_RUN = 1;
			return;
		}

		#ifdef FDP_2013
			SWITCH_change_color();	//Check the Color switch
		#endif

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
			msg.data[0] = global.env.color == BLUE?REAR:FORWARD;
			msg.data[1] = calibration;
			msg.size = 2;
			CAN_send(&msg);
		}

		/*************************/
		/* Choix de la stratégie */
		/*************************/
		#ifdef FDP_2013	//Pour rétrocompatibilité...
	#warning "Pensez à créer des stratégies différentes pour Tiny et Krusty... et à les inclure ci-dessous avant de virer ce warning."

		if(QS_WHO_AM_I_get()==TINY){
			if(strat_number()==0x00){
					//STRAT_1_TINY
					calibration = 0x00;
					strategy = TEST_Launcher_ball;
			}
			if(strat_number()==0x01){
					//STRAT_2_TINY
					calibration = 0x01;
					strategy = TEST_Launcher_ball;
			}
			if(strat_number()==0x02){
					//STRAT_3_TINY
					calibration = 0x00;
					strategy = TEST_Launcher_ball;
			}


		}else{
		#endif
			if(strat_number()==0x00){
					//STRAT_1_KRUSTY
				calibration = 0x00;
					strategy = TEST_STRAT_avoidance;
			}
			if(strat_number()==0x01){
					//STRAT_2_KRUSTY
					calibration = 0x00;
					strategy = TEST_Launcher_ball;
			}
			if(strat_number()==0x02){
					//STRAT_3_KRUSTY
					calibration = 0x00;
					strategy = TEST_STRAT_verres;
			}


		#ifdef FDP_2013 //Pas très propre mais pas trop le choix
		}
		#endif

	}
	else
	{
		/* we are in match*/
		if(!global.env.match_over)
		{
			if(match_duration != 0 && (global.env.match_time >= (match_duration)))
			{
				CAN_send_sid(BROADCAST_STOP_ALL);
				global.env.match_over = TRUE;
				STACKS_flush_all();
				CLOCK_stop();
			}
			else
			{
				#ifdef USE_SCHEDULED_POSITION_REQUEST
					static bool_e initialized = FALSE;
					CAN_msg_t msg;
					if(!initialized)
					{
						initialized = TRUE;
						msg.sid = ASSER_SEND_PERIODICALLY_POSITION;
						msg.data[0] = 0; 
						msg.data[1] = 20; 					//toutes les 20ms
						msg.data[2] = HIGHINT(20);
						msg.data[3] = LOWINT(20); 			//tout les 20 mm
						msg.data[4] = HIGHINT(PI4096/180);
						msg.data[5] = LOWINT(PI4096/180);  	//tout les 2°
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

Uint8 strat_number(){
	if(SWITCH_STRAT_1)
		return 0x00;
	else{
		if(SWITCH_STRAT_2)
			return 0x01;
		else{
			if(SWITCH_STRAT_3)
				return 0x02;
		}
	}
	return 0x00;
}
