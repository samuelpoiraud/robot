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

#include "strats_2014/actions_both_2014.h"
#include "strats_2014/actions_guy.h"
#include "strats_2014/actions_pierre.h"
#include "high_level_strat.h"


/**************************************************************
 *
 * Déclaration des strats pour les deux robots
 *
 **************************************************************/


#define STRAT_0_BIG high_level_strat
#define STRAT_1_BIG high_level_strat
#define STRAT_2_BIG strat_lannion
#define STRAT_3_BIG test_strat_robot_virtuel_with_avoidance

#define STRAT_0_SMALL high_level_strat
#define STRAT_1_SMALL TEST_pathfind
#define STRAT_2_SMALL Strat_Detection_Triangle
#define STRAT_3_SMALL strat_reglage_asser

volatile bool_e strat_updated = FALSE;
static ia_fun_t strategy;
static void update_strategy(void);

/* 	execute un match de match_duration secondes à partir de la
	liberation de la biroute. Arrete le robot à la fin du match.
	Appelle une autre routine pour l'IA pendant le match.
	Une durée de 0 indique un match infini
*/
void any_match(void)
{
	static time32_t match_duration = MATCH_DURATION;
	if (!global.env.match_started)
	{
		// Initialisation à FALSE des machines à état principale pour que les autres stratégie n'ai pas d'influence
		// Variable mise à TRUE au lancement d'une stratégie principale
		main_strategie_used = FALSE;
		stop_request = FALSE;

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
			msg.size = 0;
			CAN_send(&msg);
		}

		/*************************/
		/* Choix de la stratégie */
		/*************************/
		update_strategy();

		if(strategy == strat_reglage_asser)	//Liste ici les stratégie "infinies"...
			match_duration = 0;
		else
			match_duration = MATCH_DURATION;

		if(strategy == high_level_strat)	//Liste ici les stratégie qui doivent être appelées même avant le début du match
			high_level_strat();
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

				//Stratégie du match
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




static void update_strategy(void)
{

	static ia_fun_t previous_strategy = NULL;
	ia_fun_t current_strat;
	current_strat =  (QS_WHO_AM_I_get()==BIG_ROBOT)?STRAT_0_BIG:STRAT_0_SMALL;
	if(SWITCH_STRAT_1)
		current_strat =  (QS_WHO_AM_I_get()==BIG_ROBOT)?STRAT_1_BIG:STRAT_1_SMALL;
	else if(SWITCH_STRAT_2)
		current_strat =  (QS_WHO_AM_I_get()==BIG_ROBOT)?STRAT_2_BIG:STRAT_2_SMALL;
	else if(SWITCH_STRAT_3)
		current_strat =  (QS_WHO_AM_I_get()==BIG_ROBOT)?STRAT_3_BIG:STRAT_3_SMALL;
	strat_updated = (current_strat != previous_strategy)?TRUE:FALSE;	//Flag d'update, pendant un tour de boucle.
	if(strat_updated)
	{
		strategy = current_strat;
		debug_printf("Using strat : %s\n", BRAIN_get_current_strat_name());
	}
	previous_strategy = strategy;
}

bool_e BRAIN_get_strat_updated(void)
{
	return strat_updated;
}

//Retourne une chaine de 20 caractères max
char * BRAIN_get_current_strat_name(void)
{
	if(strategy == high_level_strat)
		return "high_level_strat";
	if(strategy == strat_lannion)
		return "strat_lannion";
	if(strategy == Strat_Detection_Triangle )
		return "Str_Detect_Triangle";
	if(strategy == strat_reglage_asser)
		return "Str_reglage_asser";
	if(strategy == test_strat_robot_virtuel_with_avoidance)
		return "Str_avoidance";
	if(strategy == TEST_pathfind)
		return "TEST_pathfind";

	return "strat not declared !";
}

