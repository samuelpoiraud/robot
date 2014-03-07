/*
 *	Club Robot ESEO 2013 - 2014
 *	Pierre & Guy
 *
 *	Fichier : actions_pierre.h
 *	Package : Carte S²/strats2013
 *	Description : Tests des actions réalisables par le robot
 *	Auteur : Herzaeone, modifié par .
 *	Version 2013/10/03
 */

#include "../QS/QS_all.h"

#ifndef STRAT_PIERRE_H
#define STRAT_PIERRE_H


#include "../asser_functions.h"
#include "../act_functions.h"
#include "../QS/QS_types.h"
#include "../config/config_pin.h"

typedef enum{
	SUB_INIT = 0,
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
}subaction_id_e;

void set_sub_act(subaction_id_e sub_action, Uint8 priority, bool_e enable, Uint32 t_begin, Uint32 t_end, char * chaine);
void set_sub_act_priority(subaction_id_e sub_action, Uint8 priority);
void set_sub_act_enable(subaction_id_e sub_action, bool_e enable);
void set_sub_act_t_begin(subaction_id_e sub_action, Uint32 t_begin);
void set_sub_act_t_end(subaction_id_e sub_action, Uint32 t_end);
void set_sub_act_chaine(subaction_id_e sub_action, char* chaine);
Uint8 get_sub_act_priority(subaction_id_e sub_action);
bool_e get_sub_act_enable(subaction_id_e sub_action);
Uint32 get_sub_act_t_begin(subaction_id_e sub_action);
Uint32 get_sub_act_t_end(subaction_id_e sub_action);
char * get_sub_act_chaine(subaction_id_e sub_action);


#endif /* STRAT_PIERRE_H_ */
