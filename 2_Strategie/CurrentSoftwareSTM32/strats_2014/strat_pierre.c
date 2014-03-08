/*
 *	Club Robot ESEO 2013 - 2014
 *	Pierre & Guy
 *
 *	Fichier : strat_pierre.c
 *	Package : Carte S²/strats2014
 *	Description : Stratégie de base de Pierre
 *	Auteur : Arnaud.
 *	Version 2014/03/08
 */


#include "strat_pierre.h"
#include "actions_pierre.h"
#include "../QS/QS_outputlog.h"
#include "../state_machine_helper.h"
#include "../act_can.h"
#include "../Pathfind.h"
#include <string.h>


/**********************************************************************************************************************************
 *
 *		Robot prototype devra modifier le sens ( BACKWARD / FORWARD ) sur le lanceLauncher, depose de fruit et ramassage de fruit
 *				sur la verison finale
 *
 **********************************************************************************************************************************/


typedef struct{
	Uint8 priority;				// Priorité de l'action (faible = forte priorité)
	Uint8 failed;				// Nombre de fois ou l'action à raté
	bool_e done;				// Action terminée ou non
	bool_e enable;				// Action activée ou non
	Uint32 t_begin;				// Temps match à partir duquel l'action peut être appelée (en ms)
	Uint32 t_end;				// Temps match à partir duquel l'action ne peut plus être appelée (en ms)
	bool_e updated_for_lcd;		// Doit être mis à 1 à chaque mise à jours
	char chaine[10];			// Chaine de 9 caractères + '\0' pour décrir l'action sur le lcd
}action_t;

static action_t action[SUB_NB];



void strat_lannion_cerveau(void){
	static subaction_id_e previous, state;
	static bool_e init = FALSE;
	bool_e previous_switch[3];

	error_e sub_action;
	subaction_id_e sub;
	Uint8 min_failed, priority;

	if(init == FALSE){
		//			sub action		priority	enable		t_begin		t_end		 chaine
		set_sub_act(SUB_GETOUT,		0,			TRUE,		0,			0,			 "GETOUT");
		set_sub_act(SUB_LANCE,		1,			TRUE,		0,			0,			 "LANCE");
		set_sub_act(SUB_FRUITS,		2,			TRUE,		0,			0,			 "FRUITS");
		set_sub_act(SUB_LANCE_ADV,	3,			TRUE,		0,			0,			 "LANCE ADV");
		set_sub_act(SUB_FRUITS_ADV,	4,			FALSE,		60000,		0,			 "FRUITS ADV");
		set_sub_act(SUB_FRESCO,		5,			TRUE,		0,			0,			 "FRESCO");
		set_sub_act(SUB_FILET,		0xFF,		TRUE,		0,			0,			 "FILET");

	}

	if(SWITCH_STRAT_1 != previous_switch[0] || SWITCH_STRAT_2 != previous_switch[1] ||  SWITCH_STRAT_3 != previous_switch[2] || init == FALSE){
		// Si les switchs ont changés d'état ou si les subs actions n'ont pas été initialisées

		// Sub action à régler en fonction des switchs

		if(SWITCH_STRAT_1){ // Exemple...
			set_sub_act_priority(SUB_FRESCO, 2);
		}

		previous_switch[0] = SWITCH_STRAT_1;
		previous_switch[1] = SWITCH_STRAT_2;
		previous_switch[2] = SWITCH_STRAT_3;
	}

	// Fonction de mise à jours de l'écran LCD
	// ...


	switch(state){

		case SUB_INIT :
			for(sub=0;sub<SUB_NB;sub++){
				action[sub].failed = 0;
				action[sub].done = 0;
				action[sub].updated_for_lcd = 1;
			}
			action[SUB_INIT].enable = 0;
			action[SUB_INIT].done = 1;
			state = TAKE_DECISION;
			break;

		case SUB_GETOUT :
			break;

		case SUB_LANCE :
			sub_action = strat_lance_launcher(FALSE);
			switch(sub_action){
				case END_OK :
					action[SUB_LANCE].done = TRUE;
					state = TAKE_DECISION;
					break;

				case IN_PROGRESS :
					break;

				case END_WITH_TIMEOUT :
				case NOT_HANDLED :
				case FOE_IN_PATH :
					action[SUB_LANCE].failed++;
					state = TAKE_DECISION;
					break;
			}

			break;

		case SUB_LANCE_ADV :
			break;

		case SUB_FRUITS :
			break;

		case SUB_FRUITS_ADV :
			break;

		case SUB_FILET :
			break;

		case TAKE_DECISION :
			// Init des variables
			min_failed = 0xFF;
			priority = 0xFF;

			// Update des actions en fonction du temps
			for(sub=0;sub<SUB_NB;sub++){
				if(action[sub].t_begin !=0 && action[sub].enable == FALSE){
					if(action[sub].t_begin >= global.env.match_time)
						action[sub].enable = TRUE;
				}else if(action[sub].t_end !=0 && action[sub].enable == TRUE){
					if(action[sub].t_end >= global.env.match_time)
						action[sub].enable = FALSE;
				}
			}

			// Détermination de la sub action la plus important à faire
			for(sub=0;sub<SUB_NB;sub++){
				if(action[sub].enable == TRUE
						&& action[sub].done == FALSE
						&& previous != sub
						&& action[sub].priority < priority
						&& action[sub].failed < min_failed){
					priority = action[sub].priority;
					state = sub;
				}
			}
			break;

		case GET_OUT_IF_NO_CALIBRATION :

			break;

		case TURN_IF_NO_CALIBRATION :

			break;

		default :
			break;
	}
	init = TRUE;
}

void set_sub_act_priority(subaction_id_e sub_action, Uint8 priority){
	action[sub_action].priority = priority;
	action[sub_action].updated_for_lcd = TRUE;
}

void set_sub_act_enable(subaction_id_e sub_action, bool_e enable){
	action[sub_action].enable = enable;
	action[sub_action].updated_for_lcd = TRUE;
}

void set_sub_act_t_begin(subaction_id_e sub_action, Uint32 t_begin){
	action[sub_action].t_begin = t_begin;
	action[sub_action].updated_for_lcd = TRUE;
}

void set_sub_act_t_end(subaction_id_e sub_action, Uint32 t_end){
	action[sub_action].t_end = t_end;
	action[sub_action].updated_for_lcd = TRUE;
}

void set_sub_act_chaine(subaction_id_e sub_action, char* chaine){
	strncpy(action[sub_action].chaine, chaine, 9); \
	action[sub_action].chaine[strlen(chaine)] = '\0';
	action[sub_action].updated_for_lcd = TRUE;
}

void inc_sub_act_priority(subaction_id_e sub_action){
	if(action[sub_action].priority == 0xFF)
		action[sub_action].priority = 0;
	else
		action[sub_action].priority++;
	action[sub_action].updated_for_lcd = TRUE;
}

void dec_sub_act_priority(subaction_id_e sub_action){
	if(action[sub_action].priority == 0x00)
		action[sub_action].priority = 0xFF;
	else
		action[sub_action].priority--;
	action[sub_action].updated_for_lcd = TRUE;
}

void inc_sub_act_t_begin(subaction_id_e sub_action){
	if(action[sub_action].t_begin >= 89000)
		action[sub_action].t_begin = 0;
	else
		action[sub_action].t_begin+=1000;
	action[sub_action].updated_for_lcd = TRUE;
}

void dec_sub_act_t_begin(subaction_id_e sub_action){
	if(action[sub_action].t_begin <= 1000)
		action[sub_action].t_begin = 90000;
	else
		action[sub_action].t_begin-=1000;
	action[sub_action].updated_for_lcd = TRUE;
}

void inc_sub_act_t_end(subaction_id_e sub_action){
	if(action[sub_action].t_end >= 80000)
		action[sub_action].t_end = 0;
	else
		action[sub_action].t_end+=1000;
	action[sub_action].updated_for_lcd = TRUE;
}

void dec_sub_act_t_end(subaction_id_e sub_action){
	if(action[sub_action].t_end <= 1000)
		action[sub_action].t_end = 90000;
	else
		action[sub_action].t_end-=1000;
	action[sub_action].updated_for_lcd = TRUE;
}

Uint8 get_sub_act_priority(subaction_id_e sub_action){
	return action[sub_action].priority;
}

bool_e get_sub_act_enable(subaction_id_e sub_action){
	return action[sub_action].enable;
}

Uint32 get_sub_act_t_begin(subaction_id_e sub_action){
	return action[sub_action].t_begin;
}

Uint32 get_sub_act_t_end(subaction_id_e sub_action){
	return action[sub_action].t_end;
}

char * get_sub_act_chaine(subaction_id_e sub_action){
	return action[sub_action].chaine;
}

void set_sub_act(subaction_id_e sub_action, Uint8 priority, bool_e enable, Uint32 t_begin, Uint32 t_end, char * chaine){
	action[sub_action].priority = priority;
	action[sub_action].enable = enable;
	action[sub_action].t_begin = t_begin;
	action[sub_action].t_end = t_end;
	strncpy(action[sub_action].chaine, chaine, 9); \
	action[sub_action].chaine[strlen(chaine)] = '\0';
	action[sub_action].updated_for_lcd = TRUE;
}

