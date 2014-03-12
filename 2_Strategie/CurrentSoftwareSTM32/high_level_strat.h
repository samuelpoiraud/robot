/*
 *	Club Robot ESEO 2013 - 2014
 *	Pierre & Guy
 *
 *	Fichier : strat_pierre.h
 *	Package : Carte S�/strats2014
 *	Description :
 *	Auteur : Arnaud
 *	Version 2014/03/08
 */

#include "../QS/QS_all.h"

#ifndef STRAT_PIERRE_H
#define STRAT_PIERRE_H


#include "../asser_functions.h"
#include "../act_functions.h"
#include "../QS/QS_types.h"
#include "../config/config_pin.h"
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

	//--------------Sub action de pierre

	SUB_LANCE,
	SUB_LANCE_ADV,
	SUB_FRUITS,
	SUB_DROP_FRUITS,
	SUB_FRUITS_ADV,
	SUB_FRESCO,
	SUB_FILET,		//Peu importe l'ordre des �tats... (sauf en cas de priorit� �gale)

	//--------------Sub action de guy

	SUB_SCAN,


	//------------------------------------
	SUB_NB,			//Ceci n'est pas un �tat atteignable, mais permet de connaitre le nombre d'action !

	TAKE_DECISION,

	//-----------------Autre �tats
	GET_OUT_IF_NO_CALIBRATION,
	TURN_IF_NO_CALIBRATION
);

void high_level_strat();

void set_sub_act(subaction_id_e sub_action, Uint8 priority, bool_e enable, time32_t t_begin, time32_t t_end, bool_e ask_stop_request, char * chaine);
void set_sub_act_priority(subaction_id_e sub_action, Uint8 priority);
void set_sub_act_enable(subaction_id_e sub_action, bool_e enable);
void set_sub_act_done(subaction_id_e sub_action, bool_e done);
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
Uint32 get_sub_act_t_begin(subaction_id_e sub_action);
Uint32 get_sub_act_t_end(subaction_id_e sub_action);
char * get_sub_act_chaine(subaction_id_e sub_action);
bool_e get_sub_act_ask_stop_request(subaction_id_e sub_action);


#endif /* STRAT_PIERRE_H_ */
