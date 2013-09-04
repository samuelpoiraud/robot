/*
 *	Club Robot ESEO 2012 - 2013
 *	Krusty & Tiny
 *
 *	Fichier : actions_utils.h
 *	Package : Carte Stratégie
 *	Description : Offre des fonctions simplifiant le code de strategie
 *	Auteur : amurzeau
 *	Version 27 avril 2013
 */

#ifndef ACTIONS_UTILS_H
#define	ACTIONS_UTILS_H

#include "queue.h"
#include "avoidance.h"
#include "Supervision/SD/SD.h"

//Défini un nom d'état dans un tableau de string. Utilisé pour afficher le nom d'une valeur d'un enum, par exemple:
//Pour que tableau[UN_ETAT_OU_ENUM] = "UN_ETAT_OU_ENUM", il faut faire STATE_STR_DECLARE(tableau, UN_ETAT_OU_ENUM)
#define STATE_STR_DECLARE(tableau, state)  tableau[state] = #state;

//Le bloc de code est dans une do {} while(0) pour pouvoir être mis dans un if sans accolade du genre if(condition) STATE_STR_INIT_UNDECLARED(tableau, nbstate);
//Initialise les états non défini d'un tableau de string.
//Les noms des états non définis est mis à "Unknown"
//Pour définir un état dans un tableau de nom d'état, veuillez utiliser STATE_STR_DECLARE(tableau, etat)
//Un exemple est dispo dans K_STRAT_sub_glasses_alexis
#define STATE_STR_INIT_UNDECLARED(tableau, nbstate) \
	do {\
		Uint8 i; \
		for(i=0; i<(nbstate); i++) {\
			if(tableau[i] == 0) \
				tableau[i] = "Unknown"; \
		} \
	}while(0)

#define STATECHANGE_log(state_define, old_state_name, old_state_id, new_state_name, new_state_id) \
	UTILS_LOG_state_changed(#state_define, state_define, old_state_name, old_state_id, new_state_name, new_state_id)

void UTILS_LOG_state_changed(const char* sm_name, Uint16 sm_id, const char* old_state_name, Uint8 old_state_id, const char* new_state_name, Uint8 new_state_id);
void UTILS_CAN_send_state_changed(Uint16 state_machine_id, Uint8 old_state, Uint8 new_state, Uint8 nb_params, ...);

//ID des machines à états, sur 16 bits.
//0xRSMM: R: numéro de robot, S: numéro de strat (strat globale comme faire les cadeaux), MM: un octet pour un numéro de machine à état d'un truc précis...
//R = 0 Pour Krusty, 1 pour Tiny, numéro suivant pour autre chose ...
#define SM_ID_KRUSTY_STRAT_ALEXIS	0x0000
#define SM_ID_KRUSTY_STRAT_ALEXIS_FINALE	0x0001
#define SM_ID_GLASSES_DO		0x0101
#define SM_ID_GLASSES_GRAB		0x0102
#define SM_ID_GLASSES_PUT_DOWN	0x0103
#define SM_ID_CHERRIES_MAIN		0x0200
#define SM_ID_CHERRIES_MOVE		0x0201
#define SM_ID_CHERRIES_GRAB		0x0202
#define SM_ID_CHERRIES_LAUNCH	0x0203
#define SM_ID_CHERRIES_DROP		0x0204
#define SM_ID_ZONE_TRY_LOCK		0x0301

Uint8 check_act_status(queue_id_e act_queue_id, Uint8 in_progress_state, Uint8 success_state, Uint8 failed_state);
Uint8 check_sub_action_result(error_e sub_action, Uint8 in_progress_state, Uint8 success_state, Uint8 failed_state);





#define HAMMER_POSITION_UP		(0)
#define HAMMER_POSITION_CANDLE	(35)
#define HAMMER_POSITION_DOWN	(85)
#define HAMMER_POSITION_HOME	(90)


Uint16 wait_hammer(Uint16 progress, Uint16 success, Uint16 fail);


bool_e all_gifts_done(void);

#endif	/* ACTIONS_UTILS_H */

