/*
 *	Club Robot ESEO 2013 - 2014
 *	Pierre & Guy
 *
 *	Fichier : strat_pierre.h
 *	Package : Carte S²/strats2014
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

// Cette variable est mise à TRUE quand une sub action prioritaire urgente doit être faite
bool_e stop_request;

TYPEDEF_ENUM_WITH_STRING(subaction_id_e, subaction_name,
	SUB_INIT,
	SUB_GETOUT,
	SUB_LANCE,
	SUB_LANCE_ADV,
	SUB_FRUITS,
	SUB_FRUITS_ADV,
	SUB_FRESCO,
	SUB_FILET,		//Peu importe l'ordre des états... (sauf en cas de priorité égale)

	SUB_NB,			//Ceci n'est pas un état atteignable, mais permet de connaitre le nombre d'action !

	TAKE_DECISION,

	// Autre états
	GET_OUT_IF_NO_CALIBRATION,
	TURN_IF_NO_CALIBRATION
);

void set_sub_act(subaction_id_e sub_action, Uint8 priority, bool_e enable, time32_t t_begin, time32_t t_end, bool_e ask_stop_request, char * chaine);
void set_sub_act_priority(subaction_id_e sub_action, Uint8 priority);
void set_sub_act_enable(subaction_id_e sub_action, bool_e enable);
void set_sub_act_t_begin(subaction_id_e sub_action, time32_t t_begin);
void set_sub_act_t_end(subaction_id_e sub_action, time32_t t_end);
void set_sub_act_chaine(subaction_id_e sub_action, char* chaine);
void set_sub_act_ask_stop_request(subaction_id_e sub_action, bool_e ask_stop_request);
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
