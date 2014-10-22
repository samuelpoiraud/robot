/*
 *	Club Robot ESEO 2008 - 2012
 *	Archi'Tech, CHOMP, CheckNorris, Shark & Fish
 *
 *	Fichier : brain.c
 *	Package : Carte Principale
 *	Description : Fonctions de g�n�ration des ordres
 *	Auteur : Jacen, modifi� par Gonsevi
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

#include "strats_2015/actions_both_2015.h"
#include "strats_2015/actions_wood.h"
#include "strats_2015/actions_holly.h"

#include "high_level_strat.h"


//Strat�gie par d�faut... (modifiable par les codeurs qui auraient la flemme ou l'impossibilit� de configurer leur strat sur le LCD � chaque reset...)
//							Valeur souhaitable pour le commit SVN : high_level_strat
#define DEFAULT_STRAT_BIG	high_level_strat

#define DEFAULT_STRAT_SMALL	high_level_strat


static ia_fun_t strategy;
static char *strategy_name;
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
	time32_t match_duration; // (MATCH_DURATION de base � 90 000 ms et 0 signifit strat�gie avec un temps infini)
	bool_e display_on;
	robot_type_e robot_type;
}strategy_list_s;

static const strategy_list_s list_strategy[] = {

	// Pour les deux robots
	//display name			name function							// match duration	// afficher sur le LCD	// strat�gie pour quel robot BIG/SMALL/BOTH(les deux)
	{"high_level_strat",	high_level_strat,						MATCH_DURATION,		TRUE,					BOTH},
	{"strat_odo_rot",		strat_reglage_odo_rotation,				0,					TRUE,					BOTH},
	{"strat_odo_tra",		strat_reglage_odo_translation,			0,					TRUE,					BOTH},
	{"strat_odo_sym",		strat_reglage_odo_symetrie,				0,					TRUE,					BOTH},
	{"strat_prop",			strat_reglage_prop,						0,					TRUE,					BOTH},

	// Pour holly
	{"strat_inutile",		strat_inutile_holly,					MATCH_DURATION,		TRUE,					BIG},

	// Pour wood
	{"strat_inutile",		strat_inutile_wood,						MATCH_DURATION,		TRUE,					SMALL}
};

static const strategy_list_s *list_displayed_strategy[50];

void BRAIN_init(void){
	Uint8 i;
	bool_e found = FALSE;
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
					|| list_strategy[i].robot_type == BOTH)){
			list_displayed_strategy[number_of_displayed_strategy++] = &list_strategy[i];
			if(list_strategy[i].function == strategy)
				index_strategy = number_of_displayed_strategy-1;
		}

		if(list_strategy[i].function == strategy){
			strategy_name = list_strategy[i].name;
			match_duration = list_strategy[i].match_duration;
			found = TRUE;
		}
	}
	if(!found)
		update_match_duration();
}

/* 	execute un match de match_duration secondes � partir de la
	liberation de la biroute. Arrete le robot � la fin du match.
	Appelle une autre routine pour l'IA pendant le match.
	Une dur�e de 0 indique un match infini
*/
void any_match(void)
{


	if (!global.env.match_started)
	{
		// Initialisation � FALSE des machines � �tat principale pour que les autres strat�gie n'ai pas d'influence
		// Variable mise � TRUE au lancement d'une strat�gie principale
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
			Supervision_send_periodically_pos(20,PI4096/45);	//Demande d'envoi de la position : tout les 20 mm et tout les 4�
			XBEE_ping_pong_enable(FALSE);						//D�sactive le ping/pong... c'est trop tard pour �a...
			return;
		}

		SWITCH_change_color();	//Check the Color switch

		/* accepter et pr�venir des mises � jour de couleur (TOP_COLOR par d�faut) */
		if(global.env.color_updated && !global.env.prop.calibrated && !global.env.ask_prop_calibration)
		{
			ENV_set_color(global.env.wanted_color);
		}

		/* demande de calibration */
		if(global.env.ask_prop_calibration && !global.env.prop.calibrated)
		{
			CAN_msg_t msg;
			msg.sid = PROP_CALIBRATION;
			msg.size = 0;
			CAN_send(&msg);
		}

		if(strategy == high_level_strat)	//Liste ici les strat�gie qui doivent �tre appel�es m�me avant le d�but du match
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

				Supervision_send_periodically_pos(1, PI4096/180); // Tous les milimetres et degr�s: ca flood mais on est pas en match donc pas d�placment
			}
			else
			{
				(*strategy)();	//Strat�gie du match
			}
		}
		else
		{
			/* match is over */
		}
	}
}



//Attention, ne renvoit qu'une fois VRAI lorsque la strategy a �t� mise � jour.... (destination LCD...)
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
		strategy_name = list_displayed_strategy[i]->name;
		index_strategy = i;
		update_match_duration();
		debug_printf("Using strat : %s\n", BRAIN_get_current_strat_name());
		strat_updated = TRUE;
	}else
		debug_printf("Essai de modification de la strat�gie alors que le match est lanc� !\n");
}

//Retourne une chaine de 20 caract�res max
char * BRAIN_get_current_strat_name(void)
{
	if(strategy_name != NULL)
		return strategy_name;
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
