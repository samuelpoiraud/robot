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

#define STRAT_BIG	high_level_strat

#define STRAT_SMALL	high_level_strat
/*
#define STRAT_0_BIG high_level_strat
#define STRAT_1_BIG high_level_strat
#define STRAT_2_BIG strat_lannion
#define STRAT_3_BIG test_strat_robot_virtuel_with_avoidance

#define STRAT_0_SMALL high_level_strat
#define STRAT_1_SMALL TEST_pathfind
#define STRAT_2_SMALL Strat_Detection_Triangle
#define STRAT_3_SMALL strat_reglage_asser*/

static void update_strategy(void);
static void update_match_duration(time32_t *match_duration);

volatile bool_e strat_updated = FALSE;
static ia_fun_t strategy;
static Sint8 index_strategy = -1;
static Uint8 number_of_strategy = 0;
static Uint8 number_of_displayed_strategy = 0;


typedef struct{
	char *name;
	ia_fun_t function;
	time32_t match_duration; // (MATCH_DURATION de base à 90 000 ms et 0 signifit stratégie avec un temps infini)
	bool_e display_on;
}strategy_list_s;

static const strategy_list_s list_strategy_pierre[] = {
	//display name			name function							// match duration	// afficher sur le LCD
	{"high_level_strat",	high_level_strat,						MATCH_DURATION,		TRUE},
	{"strat_lannion",		strat_lannion,							MATCH_DURATION,		TRUE},
	{"strat_odo_rot",		strat_reglage_odo_rotation,				0,					TRUE},
	{"strat_odo_tra",		strat_reglage_odo_translation,			0,					TRUE},
	{"strat_odo_sym",		strat_reglage_odo_symetrie,				0,					TRUE},
	{"strat_asser",			strat_reglage_asser,					0,					TRUE},
	{"strat_pts",			strat_test_point,						MATCH_DURATION,		TRUE},
	{"strat_pts_2",			strat_test_point2,						MATCH_DURATION,		TRUE},
	{"Str_Detect_Triangle", Strat_Detection_Triangle,				MATCH_DURATION,		FALSE},
	{"Str_avoidance",		test_strat_robot_virtuel_with_avoidance,MATCH_DURATION,		FALSE},
	{"TEST_pathfind",		TEST_pathfind,							MATCH_DURATION,		FALSE}
};

static const strategy_list_s list_strategy_guy[] = {
	//display name			name function							// match duration	// afficher sur le LCD
	{"high_level_strat",	high_level_strat,						MATCH_DURATION,		TRUE},
	{"strat_odo_rot",		strat_reglage_odo_rotation,				0,					TRUE},
	{"strat_odo_tra",		strat_reglage_odo_translation,			0,					TRUE},
	{"strat_odo_sym",		strat_reglage_odo_symetrie,				0,					TRUE},
	{"strat_asser",			strat_reglage_asser,					0,					TRUE},
	{"Str_Detect_Triangle", Strat_Detection_Triangle,				MATCH_DURATION,		FALSE},
	{"Str_avoidance",		test_strat_robot_virtuel_with_avoidance,MATCH_DURATION,		FALSE},
	{"TEST_pathfind",		TEST_pathfind,							MATCH_DURATION,		FALSE}
};

static const strategy_list_s *list_displayed_strategy[50];

void BRAIN_init(void){
	Uint8 i;
	if(QS_WHO_AM_I_get() == BIG_ROBOT){
		strategy = STRAT_BIG;
		number_of_strategy = sizeof(list_strategy_pierre)/sizeof(strategy_list_s);
	}else{
		strategy = STRAT_SMALL;
		number_of_strategy = sizeof(list_strategy_guy)/sizeof(strategy_list_s);
	}
	index_strategy = -1;

	number_of_displayed_strategy = 0;
	for(i=0;i<number_of_strategy;i++){
		if(QS_WHO_AM_I_get() == BIG_ROBOT){
			if(list_strategy_pierre[i].display_on == TRUE)
				list_displayed_strategy[number_of_displayed_strategy++] = &list_strategy_pierre[i];
			if(list_strategy_pierre[i].function == strategy)
				index_strategy = i;
		}else{
			if(list_strategy_guy[i].display_on == TRUE)
				list_displayed_strategy[number_of_displayed_strategy++] = &list_strategy_guy[i];
			if(list_strategy_guy[i].function == strategy)
				index_strategy = i;
		}
	}
}

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
		update_match_duration(&match_duration);
		/*
		if(strategy == strat_reglage_asser)	//Liste ici les stratégie "infinies"...
			match_duration = 0;
		else
			match_duration = MATCH_DURATION;*/

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
	/*
	ia_fun_t current_strat;
	current_strat =  (QS_WHO_AM_I_get()==BIG_ROBOT)?STRAT_0_BIG:STRAT_0_SMALL;
	if(SWITCH_STRAT_1)
		current_strat =  (QS_WHO_AM_I_get()==BIG_ROBOT)?STRAT_1_BIG:STRAT_1_SMALL;
	else if(SWITCH_STRAT_2)
		current_strat =  (QS_WHO_AM_I_get()==BIG_ROBOT)?STRAT_2_BIG:STRAT_2_SMALL;
	else if(SWITCH_STRAT_3)
		current_strat =  (QS_WHO_AM_I_get()==BIG_ROBOT)?STRAT_3_BIG:STRAT_3_SMALL;
	strat_updated = (current_strat != previous_strategy)?TRUE:FALSE;	//Flag d'update, pendant un tour de boucle.
	*/
	if(strat_updated)
	{
		//strategy = current_strat;
		debug_printf("Using strat : %s\n", BRAIN_get_current_strat_name());
	}
	//previous_strategy = strategy;
}

bool_e BRAIN_get_strat_updated(void)
{
	return strat_updated;
}

void BRAIN_set_strategy_index(Uint8 i){
	if(global.env.match_started == FALSE){
		assert(i >= 0 && i < number_of_displayed_strategy);
		strategy = list_displayed_strategy[i]->function;
		index_strategy = i;
		strat_updated = TRUE;
	}else
		debug_printf("Essai de modification de la stratégie alors que le match est lancé !\n");
}

//Retourne une chaine de 20 caractères max
char * BRAIN_get_current_strat_name(void)
{
	if(index_strategy != -1 && QS_WHO_AM_I_get() == BIG_ROBOT)
		return list_strategy_pierre[index_strategy].name;
	else if(index_strategy != -1 && QS_WHO_AM_I_get() == SMALL_ROBOT)
		return list_strategy_guy[index_strategy].name;
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

static void update_match_duration(time32_t *match_duration){
	if(index_strategy != -1 && QS_WHO_AM_I_get() == BIG_ROBOT)
		*match_duration = list_strategy_pierre[index_strategy].match_duration;
	else if(index_strategy != -1 && QS_WHO_AM_I_get() == SMALL_ROBOT)
		*match_duration = list_strategy_guy[index_strategy].match_duration;
	else
		*match_duration = MATCH_DURATION;
}
