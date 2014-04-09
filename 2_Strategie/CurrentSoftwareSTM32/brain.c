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


//Stratégie par défaut... (modifiable par les codeurs qui auraient la flemme ou l'impossibilité de configurer leur strat sur le LCD à chaque reset...)
//							Valeur souhaitable pour le commit SVN : high_level_strat
#define DEFAULT_STRAT_BIG	high_level_strat

#define DEFAULT_STRAT_SMALL	high_level_strat


static ia_fun_t strategy;
static time32_t match_duration = MATCH_DURATION;
static Sint8 index_strategy = -1;
static Uint8 number_of_strategy = 0;
static Uint8 number_of_displayed_strategy = 0;
static bool_e strat_updated = TRUE;
static void update_match_duration(void);

typedef enum{
	BIG,
	SMALL,
	BOTH
}robot_type_e;

typedef struct{
	char *name;
	ia_fun_t function;
	time32_t match_duration; // (MATCH_DURATION de base à 90 000 ms et 0 signifit stratégie avec un temps infini)
	bool_e display_on;
	robot_type_e robot_type;
}strategy_list_s;

static const strategy_list_s list_strategy[] = {

	// Pour les deux robots
	//display name			name function							// match duration	// afficher sur le LCD	// stratégie pour quel robot BIG/SMALL/BOTH(les deux)
	{"high_level_strat",	high_level_strat,						MATCH_DURATION,		TRUE,					BOTH},
	{"strat_odo_rot",		strat_reglage_odo_rotation,				0,					TRUE,					BOTH},
	{"strat_odo_tra",		strat_reglage_odo_translation,			0,					TRUE,					BOTH},
	{"strat_odo_sym",		strat_reglage_odo_symetrie,				0,					TRUE,					BOTH},
	{"strat_asser",			strat_reglage_asser,					0,					TRUE,					BOTH},
	{"Str_avoidance",		test_strat_robot_virtuel_with_avoidance,MATCH_DURATION,		FALSE,					BOTH},
	{"TEST_pathfind",		TEST_pathfind,							MATCH_DURATION,		FALSE,					BOTH},
	{"test_evitement",		test_strat_robot_virtuel_with_avoidance,0,					FALSE,					BOTH},

	// Pour pierre
	{"strat_lannion",		strat_lannion,							MATCH_DURATION,		TRUE,					BIG},
	{"strat_pts",			strat_test_point,						MATCH_DURATION,		TRUE,					BIG},
	{"strat_pts_2",			strat_test_point2,						MATCH_DURATION,		TRUE,					BIG},
	{"Str_Detect_Triangle", Strat_Detection_Triangle,				MATCH_DURATION,		FALSE,					BIG}

	// Pour guy

};

static const strategy_list_s *list_displayed_strategy[50];

void BRAIN_init(void){
	Uint8 i;
	if(QS_WHO_AM_I_get() == BIG_ROBOT)
		strategy = DEFAULT_STRAT_BIG;
	else
		strategy = DEFAULT_STRAT_SMALL;

	number_of_strategy = sizeof(list_strategy)/sizeof(strategy_list_s);
	index_strategy = -1;

	number_of_displayed_strategy = 0;
	for(i=0;i<number_of_strategy;i++){
		if(list_strategy[i].display_on == TRUE && (
					(QS_WHO_AM_I_get() == BIG_ROBOT && list_strategy[i].robot_type == BIG)
					|| (QS_WHO_AM_I_get() == SMALL_ROBOT && list_strategy[i].robot_type == SMALL)
					|| list_strategy[i].robot_type == BOTH))
			list_displayed_strategy[number_of_displayed_strategy++] = &list_strategy[i];
		if(list_strategy[i].function == strategy)
			index_strategy = i;
	}
	update_match_duration();
}

/* 	execute un match de match_duration secondes à partir de la
	liberation de la biroute. Arrete le robot à la fin du match.
	Appelle une autre routine pour l'IA pendant le match.
	Une durée de 0 indique un match infini
*/
void any_match(void)
{


	if (!global.env.match_started)
	{
		// Initialisation à FALSE des machines à état principale pour que les autres stratégie n'ai pas d'influence
		// Variable mise à TRUE au lancement d'une stratégie principale
		main_strategie_used = FALSE;
		stop_request = FALSE;

		/* we are before the match */
		/* regarder si le match doit commencer */
		if (global.env.ask_start && strategy != NULL)
		{
			global.env.match_started = TRUE;
			CLOCK_run_match();
			XBEE_send_sid(XBEE_START_MATCH, TRUE);				//URGENT : on lance le match sur notre robot AMI...
			CAN_send_sid(BROADCAST_START);
			Supervision_send_periodically_pos(20,PI4096/45);	//Demande d'envoi de la position : tout les 20 mm et tout les 4°
			XBEE_ping_pong_enable(FALSE);						//Désactive le ping/pong... c'est trop tard pour ça...
			return;
		}

		SWITCH_change_color();	//Check the Color switch

		/* accepter et prévenir des mises à jour de couleur (BLUE par défaut) */
		if(global.env.color_updated && !global.env.asser.calibrated && !global.env.ask_asser_calibration)
		{
			ENV_set_color(global.env.wanted_color);
		}

		/* demande de calibration */
		if(global.env.ask_asser_calibration && !global.env.asser.calibrated)
		{
			CAN_msg_t msg;
			msg.sid = ASSER_CALIBRATION;
			msg.size = 0;
			CAN_send(&msg);
		}

		// mise à jours de match_duration en fonction de la stratégie sélectionnée
		update_match_duration();

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

				Supervision_send_periodically_pos(1, PI4096/180); // Tous les milimetres et degrés: ca flood mais on est pas en match donc pas déplacment
			}
			else
			{
				(*strategy)();	//Stratégie du match
			}
		}
		else
		{
			/* match is over */
		}
	}
}



//Attention, ne renvoit qu'une fois VRAI lorsque la strategy a été mise à jour.... (destination LCD...)
bool_e BRAIN_get_strat_updated(void)
{
	if(strat_updated)
	{
		strat_updated = FALSE;
		return TRUE;
	}
	return FALSE;
}


void BRAIN_set_strategy_index(Uint8 i){
	if(global.env.match_started == FALSE){
		assert(i >= 0 && i < number_of_displayed_strategy);
		strategy = list_displayed_strategy[i]->function;
		index_strategy = i;
		update_match_duration();
		debug_printf("Using strat : %s\n", BRAIN_get_current_strat_name());
		strat_updated = TRUE;
	}else
		debug_printf("Essai de modification de la stratégie alors que le match est lancé !\n");
}

//Retourne une chaine de 20 caractères max
char * BRAIN_get_current_strat_name(void)
{
	if(index_strategy != -1)
		return list_strategy[index_strategy].name;
	else
		return "strat not declared !";
}

ia_fun_t BRAIN_get_displayed_strat_function(Uint8 i){
	assert(i<number_of_displayed_strategy);
	return list_displayed_strategy[i]->function;
}

char * BRAIN_get_displayed_strat_name(Uint8 i){
	assert(i<number_of_displayed_strategy);
	return list_displayed_strategy[i]->name;
}

Uint8 BRAIN_get_number_of_displayed_strategy(){
	return number_of_displayed_strategy;
}

ia_fun_t BRAIN_get_current_strat_function(void){
	return strategy;
}

static void update_match_duration(void)
{
	if(index_strategy != -1)
		match_duration = list_strategy[index_strategy].match_duration;
	else
		match_duration = MATCH_DURATION;
}
