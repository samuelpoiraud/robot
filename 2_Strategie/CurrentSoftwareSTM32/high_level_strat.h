/*
 *	Club Robot ESEO 2013 - 2015
 *	Pierre & Guy, Holly & Wood
 *
 *	Fichier : high_level_strat.h
 *	Package : Carte S�/strats2014
 *	Description : Strat�gie g�n�rale
 *	Auteur : Arnaud
 *	Version 2014/03/08
 */

#include "../QS/QS_all.h"

#ifndef HIGH_STRAT_LEVEL_H
#define HIGH_STRAT_LEVEL_H


#include "../prop_functions.h"
#include "../act_functions.h"
#include "../state_machine_helper.h"



// Cette variable est mise � TRUE quand une sub action prioritaire urgente doit �tre faite
bool_e stop_request; // Initialis� � FALSE dans le brain, si on n'utilise pas la strat�gie principale que ce flag n'ai pas d'incidence

// Cette variable est mise � TRUE quand la subaction a �t� cass�
bool_e sub_action_broken;

// Variable � TRUE quand la strat�gie principal est utilis�e et FALSE pour les autres strat�gie (utilisation : script � d�clencher que quand c'est la strat�gie principale)
bool_e main_strategie_used;

// L'utilisation de cette fonction est pr�vue de base sur un changement d'�tat (entrance)
	// condition	-> condition qui va d�clencher l'arr�te de l'action
	// *state		-> l'�tat de la machine � �tat que l'ont doit tester ou modifier
	// taille		-> taille du tableau d'�tat passer en argument
	// state_tab	-> Tableau d'�tat en commen�ant par l'�tat qui va introduire la sortie de l'action
	//					en continuant par les �tats par lequels on peut passer jusqu'� la sortie final
// Fonction � mettre dans toutes les sous machines � �tats
// Traiter les cas particuli�s d'attente, ... dans les �tats en ayant besoin, en utilisant la variable global stop_request comme d�clencheur
void STOP_REQUEST_IF_CHANGE(bool_e condition, Uint8 *state, Uint8 taille, Uint8 state_tab[]);
// Exemple
// STOP_REQUEST_IF_CHANGE(entrance, &state, 4, (Uint8 []){END, END_IMPOSSIBLE, DONE, ERROR});
// Quand la machine � �tat change d'�tat et qu'une stoqu_request est faite on imposse state � END
// On ne fait rien si l'�tat change entre END, END_IMPOSSIBLE, DONE, ERROR

TYPEDEF_ENUM_WITH_STRING(subaction_id_e, subaction_name,
	SUB_INIT,
	SUB_GETOUT,

	//Peu importe l'ordre des �tats... (sauf en cas de priorit� �gale)

	//--------------Sub action de holly
	HOLLY_SUB_INITIAL,
	HOLLY_SUB_DISPOSE_IN_START_ZONE,
	HOLLY_SUB_DISPOSE_CARPETS,
	HOLLY_SUB_SCAN_AND_DISPOSE_ON_ESTRAD,
	HOLLY_SUB_DO_OUR_CLAP,
	HOLLY_SUB_CUP_SCAN_AND_STEAL,
	HOLLY_SUB_COLLECT_POPCORN_ADV,
	HOLLY_SUB_TAKE_REAR_FEET_IN_LIFT,
	HOLLY_SUB_END_OF_MATCH,

	//--------------Sub action de wood
	WOOD_SUB_INITIAL,
	WOOD_SUB_ADV_CLAP,
	WOOD_SUB_OUR_CLAP,
	WOOD_SUB_ADV_POPCORN,
	WOOD_SUB_CUP_DISPOSE,
	WOOD_SUB_PROTECT_POPCORN,
	WOOD_SUB_DISPOSE_CUP_END_MATCH,
	WOOD_SUB_PROTECT_NORTH,
	WOOD_SUB_PROTECT_SOUTH,
	WOOD_SUB_PROTECT_SOUTH_AND_NORTH,
	WOOD_SUB_OUR_POPCORN,
	WOOD_SUB_TAKE_FEET,

	//------------------------------------
	SUB_NB,			//Ceci n'est pas un �tat atteignable, mais permet de connaitre le nombre d'action !

	TAKE_DECISION,

	//-----------------Autre �tats
	GET_OUT_IF_NO_CALIBRATION,
	TURN_IF_NO_CALIBRATION
);


// Contient la liste des subactions initialis�es et se termine par -1
Sint16 sub_action_order[SUB_NB];

// Contient le nombre de subactions initialis�es
Uint8 number_of_sub_action;

void high_level_strat();

void set_update_subaction_order(bool_e update);
void set_sub_act(subaction_id_e sub_action, Uint8 priority, bool_e enable, time32_t t_begin, time32_t t_end, bool_e ask_stop_request, bool_e call_again, char * chaine);
void set_sub_act_priority(subaction_id_e sub_action, Uint8 priority);
void set_sub_act_enable(subaction_id_e sub_action, bool_e enable);
void set_sub_act_done(subaction_id_e sub_action, bool_e done);
void set_sub_act_call_again(subaction_id_e sub_action, bool_e call_again);
void set_sub_act_t_begin(subaction_id_e sub_action, time32_t t_begin);
void set_sub_act_t_end(subaction_id_e sub_action, time32_t t_end);
void set_sub_act_chaine(subaction_id_e sub_action, char* chaine);
void set_sub_act_ask_stop_request(subaction_id_e sub_action, bool_e ask_stop_request);
void inc_sub_act_failed(subaction_id_e sub_action);
void inc_sub_act_priority(subaction_id_e sub_action);
void dec_sub_act_priority(subaction_id_e sub_action);
void inc_sub_act_t_begin(subaction_id_e sub_action);
void dec_sub_act_t_begin(subaction_id_e sub_action);
void inc_sub_act_t_end(subaction_id_e sub_action);
void dec_sub_act_t_end(subaction_id_e sub_action);

Uint8 get_sub_act_priority(subaction_id_e sub_action);
bool_e get_sub_act_enable(subaction_id_e sub_action);
bool_e get_sub_act_call_again(subaction_id_e sub_action);
bool_e get_sub_act_done(subaction_id_e sub_action);
Uint32 get_sub_act_t_begin(subaction_id_e sub_action);
Uint32 get_sub_act_t_end(subaction_id_e sub_action);
char * get_sub_act_chaine(subaction_id_e sub_action);
bool_e get_sub_act_updated_for_lcd(subaction_id_e sub_action);
void reset_sub_act_updated_for_lcd(subaction_id_e sub_action);
bool_e get_sub_act_ask_stop_request(subaction_id_e sub_action);


#endif /* HIGH_STRAT_LEVEL_H */
