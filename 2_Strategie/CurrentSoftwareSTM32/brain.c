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
#include "clock.h"
#include "button.h"	//pour SWITCH_change_color
#include "environment.h"
#include "QS/QS_CANmsgList.h"
#include "QS/QS_can_over_xbee.h"
#include "QS/QS_who_am_i.h"
#include "QS/QS_outputlog.h"
#include "config_use.h"
#include "Supervision/Supervision.h"

#include "strats_2013/actions.h"
#include "strats_2013/actions_tests_krusty.h"
#include "strats_2013/actions_tests_tiny.h"
#include "strats_2013/actions_tests_tiny_micro.h" // pour T_BALLINFLATER_start
#include "strats_2014/actions_both_2014.h"
#include "strats_2014/actions_guy.h"
#include "strats_2014/actions_pierre.h"


/**************************************************************
 *
 * Déclaration des strats pour les deux robots
 *
 **************************************************************/

// GROS = PIERRE (pour la cohérence pour les années suivantes
#define STRAT_0_GROS strat_lannion
#define STRAT_1_GROS test_strat_robot_virtuel_with_avoidance
#define STRAT_2_GROS Strat_Detection_Triangle
#define STRAT_3_GROS test_strat_robot_virtuel_with_avoidance

// PETIT = GUY
#define STRAT_0_PETIT test_strat_robot_virtuel_with_avoidance
#define STRAT_1_PETIT TEST_pathfind
#define STRAT_2_PETIT Strat_Detection_Triangle
#define STRAT_3_PETIT strat_reglage_asser


/* 	execute un match de match_duration secondes à partir de la
	liberation de la biroute. Arrete le robot à la fin du match.
	Appelle une autre routine pour l'IA pendant le match.
	Une durée de 0 indique un match infini
*/
void any_match(void)
{
	static ia_fun_t strategy;
	static calibration_square_e calibration = ASSER_CALIBRATION_SQUARE_1;
	static way_e calibration_way = ANY_WAY;
	static time32_t match_duration = MATCH_DURATION;
	if (!global.env.match_started)
	{
		/* we are before the match */
		/* regarder si le match doit commencer */
		if (global.env.ask_start)
		{
			CLOCK_run_match();
			CAN_send_sid(BROADCAST_START);
			XBEE_send_sid(XBEE_START_MATCH, TRUE);
			return;
		}

		SWITCH_change_color();	//Check the Color switch

		/* accepter et prévenir des mises à jour de couleur (BLUE par défaut) */
		if(global.env.color_updated && !global.env.asser.calibrated && !global.env.ask_asser_calibration)
		{
			ENV_set_color(global.env.wanted_color);
		}


		/* mise à jour de la configuration de match */
		if(global.env.config_updated)
			global.env.config = global.env.wanted_config;

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

		if(QS_WHO_AM_I_get()==GUY)
		{
			calibration_way = (global.env.color == BLUE)?FORWARD:BACKWARD;	//En bleu, TINY se cale en avant pour avoir son bras coté cadeaux.
			calibration = ASSER_CALIBRATION_SQUARE_0;						//Sauf si décision contraire dans certaines stratégies... Tiny est par défaut dans la case 0.

			switch(strat_number())
			{
				case 0x01:	//STRAT_1_GUY
					strategy = STRAT_1_PETIT;
				break;
				case 0x02:	//STRAT_2_GUY
					strategy = STRAT_2_PETIT;
				break;
				case 0x03:	//STRAT_3_GUY
					strategy = STRAT_3_PETIT;

				break;
				case 0x00:	//STRAT_0_TINY (aucun switch)
				//no break;
				default:
					strategy = STRAT_0_PETIT;
				break;
			}
		}
		else	//Pierre
		{
			calibration_way = BACKWARD;	//Krusty se cale TOUJOURS en backward (pas de callage contre l'ascenseur à verres)
			calibration = ASSER_CALIBRATION_SQUARE_2; //Et toujours à l'assiette 2 (sachant que Tiny est sur la 0)

			static int debug_strat = -1;

			if(debug_strat != strat_number()) {
				debug_strat = strat_number();
				debug_printf("Using strat %d\n", debug_strat);
			}

			switch(debug_strat)
			{
				case 0x01:	//STRAT_1_PIERRE
					strategy = STRAT_1_GROS;
				break;

				case 0x02:	//STRAT_2_PIERRE
					strategy = STRAT_2_GROS;
				break;

				case 0x03:	//STRAT_3_PIERRE
					strategy = STRAT_3_GROS;
				break;

				case 0x00:	//STRAT_0_PIERRE (aucun switch)
				//no break;
				default:
					strategy = STRAT_0_GROS;
				break;
			}
		}

		if(strategy == strat_reglage_asser)	//Liste ici les stratégie "infinies"...
			match_duration = 0;
		else
			match_duration = MATCH_DURATION;
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


				if(QS_WHO_AM_I_get()==PIERRE)
					ACT_filet_launch(ACT_FILET_LAUNCHED);
			}
			else
			{
				//MATCH EN ATTENTE DE LANCEMENT
				#ifdef USE_SCHEDULED_POSITION_REQUEST
					static bool_e initialized = FALSE;
					if(!initialized)
					{
						initialized = TRUE;

						Supervision_send_periodically_pos(20,PI4096/45);	// Envoyer tous les 20 mm la position du robot, tous les 4°
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
	//#ifndef FDP_2013
	//	return 0x00;
	//#else
		if(SWITCH_STRAT_1)
			return 0x01;
		else if(SWITCH_STRAT_2)
			return 0x02;
		else if(SWITCH_STRAT_3)
			return 0x03;
	//#endif
	return 0x00;	//Aucun switch -> stratégie 0.
}


