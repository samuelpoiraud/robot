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
	time32_t t_begin;			// Temps match à partir duquel l'action peut être appelée (en ms)
	time32_t t_end;				// Temps match à partir duquel l'action ne peut plus être appelée (en ms)
	bool_e updated_for_lcd;		// Doit être mis à 1 à chaque mise à jours
	char chaine[10];			// Chaine de 9 caractères + '\0' pour décrir l'action sur le lcd
}subaction_t;

static subaction_t subaction[SUB_NB];



void strat_lannion_cerveau(void){
	static subaction_id_e previous = SUB_INIT, state = SUB_INIT;
	static bool_e init = FALSE;
	static bool_e previous_switch[3];

	bool_e current_switch[3];
	error_e sub_action;
	subaction_id_e sub;
	Uint8 min_failed, priority;

	if(init == FALSE){
		// Initialisation de la structure
		for(sub=0;sub<SUB_NB;sub++){
			subaction[sub].priority = 0xFF;
			subaction[sub].failed = 0;
			subaction[sub].done = FALSE;
			subaction[sub].enable = FALSE;
			subaction[sub].t_begin = 0;
			subaction[sub].t_end = 90000;
			subaction[sub].updated_for_lcd = FALSE;
		}

		//			sub action		priority	enable		t_begin		t_end		 chaine
		set_sub_act(SUB_GETOUT,		0,			TRUE,		0,			90000,		 "GETOUT");
		set_sub_act(SUB_LANCE,		1,			TRUE,		0,			90000,		 "LANCE");
		set_sub_act(SUB_FRUITS,		2,			TRUE,		0,			90000,		 "FRUITS");
		set_sub_act(SUB_LANCE_ADV,	3,			TRUE,		0,			90000,		 "LANCE ADV");
		set_sub_act(SUB_FRUITS_ADV,	4,			TRUE,		60000,		90000,		 "FRUITS ADV");
		set_sub_act(SUB_FRESCO,		5,			TRUE,		0,			90000,		 "FRESCO");
		set_sub_act(SUB_FILET,		0,			TRUE,		85000,		90000,		 "FILET");

		// Message d'avertissement dans le cadre d'un oubli d'initialisation de subaction
		for(sub=0;sub<SUB_NB;sub++)
			if(subaction[sub].updated_for_lcd == FALSE)
				debug_printf("La subaction %d n'a pas été initialisée !\n", sub);

	}

	// Sauvegarde de l'état des switchs
	current_switch[0] = SWITCH_STRAT_1;
	current_switch[1] = SWITCH_STRAT_2;
	current_switch[2] = SWITCH_STRAT_3;
	if(current_switch[0] != previous_switch[0] || current_switch[1] != previous_switch[1] ||  current_switch[2] != previous_switch[2] || init == FALSE){
		// Si les switchs ont changés d'état ou si les subs actions n'ont pas été initialisées

		// Sub action à régler en fonction des switchs

		if(SWITCH_STRAT_1){ // Exemple...
			set_sub_act_priority(SUB_FRESCO, 2);
		}

		previous_switch[0] = current_switch[0];
		previous_switch[1] = current_switch[1];
		previous_switch[2] = current_switch[2];
	}

	// Fonction de mise à jours de l'écran LCD
	// ...


	switch(state){

		case SUB_INIT :
			subaction[SUB_INIT].done = TRUE;
			state = TAKE_DECISION;
			break;

		case SUB_GETOUT :
			break;

		case SUB_LANCE :
			sub_action = strat_lance_launcher(FALSE);
			switch(sub_action){
				case END_OK :
					subaction[SUB_LANCE].done = TRUE;
					state = TAKE_DECISION;
					break;

				case IN_PROGRESS :
					break;

				case END_WITH_TIMEOUT :
				case NOT_HANDLED :
				case FOE_IN_PATH :
					subaction[SUB_LANCE].failed++;
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

			// Détermination de la sub action la plus important à faire
			for(sub=0;sub<SUB_NB;sub++){
				if(subaction[sub].enable == TRUE
						&& subaction[sub].done == FALSE
						&& previous != sub
						&& subaction[sub].priority < priority
						&& subaction[sub].failed < min_failed
						&& subaction[sub].t_begin <= global.env.match_time
						&& subaction[sub].t_end >= global.env.match_time){
					priority = subaction[sub].priority;
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
	assert(sub_action < SUB_NB);
	subaction[sub_action].priority = priority;
	subaction[sub_action].updated_for_lcd = TRUE;
}

void set_sub_act_enable(subaction_id_e sub_action, bool_e enable){
	assert(sub_action < SUB_NB);
	subaction[sub_action].enable = enable;
	subaction[sub_action].updated_for_lcd = TRUE;
}

void set_sub_act_t_begin(subaction_id_e sub_action, time32_t t_begin){
	assert(sub_action < SUB_NB);
	subaction[sub_action].t_begin = t_begin;
	subaction[sub_action].updated_for_lcd = TRUE;
}

void set_sub_act_t_end(subaction_id_e sub_action, time32_t t_end){
	assert(sub_action < SUB_NB);
	subaction[sub_action].t_end = t_end;
	subaction[sub_action].updated_for_lcd = TRUE;
}

void set_sub_act_chaine(subaction_id_e sub_action, char* chaine){
	assert(sub_action < SUB_NB);
	strncpy(subaction[sub_action].chaine, chaine, 9); \
	subaction[sub_action].chaine[strlen(chaine)] = '\0';
	subaction[sub_action].updated_for_lcd = TRUE;
}

void inc_sub_act_priority(subaction_id_e sub_action){
	assert(sub_action < SUB_NB);
	if(subaction[sub_action].priority == 0xFF)
		subaction[sub_action].priority = 0;
	else
		subaction[sub_action].priority++;
	subaction[sub_action].updated_for_lcd = TRUE;
}

void dec_sub_act_priority(subaction_id_e sub_action){
	assert(sub_action < SUB_NB);
	if(subaction[sub_action].priority == 0x00)
		subaction[sub_action].priority = 0xFF;
	else
		subaction[sub_action].priority--;
	subaction[sub_action].updated_for_lcd = TRUE;
}

void inc_sub_act_t_begin(subaction_id_e sub_action){
	assert(sub_action < SUB_NB);
	if(subaction[sub_action].t_begin >= 89000)
		subaction[sub_action].t_begin = 0;
	else
		subaction[sub_action].t_begin+=1000;
	subaction[sub_action].updated_for_lcd = TRUE;
}

void dec_sub_act_t_begin(subaction_id_e sub_action){
	assert(sub_action < SUB_NB);
	if(subaction[sub_action].t_begin <= 1000)
		subaction[sub_action].t_begin = 90000;
	else
		subaction[sub_action].t_begin-=1000;
	subaction[sub_action].updated_for_lcd = TRUE;
}

void inc_sub_act_t_end(subaction_id_e sub_action){
	assert(sub_action < SUB_NB);
	if(subaction[sub_action].t_end >= 80000)
		subaction[sub_action].t_end = 0;
	else
		subaction[sub_action].t_end+=1000;
	subaction[sub_action].updated_for_lcd = TRUE;
}

void dec_sub_act_t_end(subaction_id_e sub_action){
	assert(sub_action < SUB_NB);
	if(subaction[sub_action].t_end <= 1000)
		subaction[sub_action].t_end = 90000;
	else
		subaction[sub_action].t_end-=1000;
	subaction[sub_action].updated_for_lcd = TRUE;
}

Uint8 get_sub_act_priority(subaction_id_e sub_action){
	assert(sub_action < SUB_NB);
	return subaction[sub_action].priority;
}

bool_e get_sub_act_enable(subaction_id_e sub_action){
	assert(sub_action < SUB_NB);
	return subaction[sub_action].enable;
}

Uint32 get_sub_act_t_begin(subaction_id_e sub_action){
	assert(sub_action < SUB_NB);
	return subaction[sub_action].t_begin;
}

Uint32 get_sub_act_t_end(subaction_id_e sub_action){
	assert(sub_action < SUB_NB);
	return subaction[sub_action].t_end;
}

char * get_sub_act_chaine(subaction_id_e sub_action){
	assert(sub_action < SUB_NB);
	return subaction[sub_action].chaine;
}

void set_sub_act(subaction_id_e sub_action, Uint8 priority, bool_e enable, time32_t t_begin, time32_t t_end, char * chaine){
	assert(sub_action < SUB_NB);
	subaction[sub_action].priority = priority;
	subaction[sub_action].enable = enable;
	subaction[sub_action].t_begin = t_begin;
	subaction[sub_action].t_end = t_end;
	strncpy(subaction[sub_action].chaine, chaine, 9);
	subaction[sub_action].chaine[strlen(chaine)] = '\0';
	subaction[sub_action].updated_for_lcd = TRUE;
}

