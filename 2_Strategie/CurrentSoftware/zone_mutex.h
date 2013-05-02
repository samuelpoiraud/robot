/*
 *	Club Robot ESEO 2012 - 2013
 *	Krusty & Tiny
 *
 *	Fichier : zone_mutex.h
 *	Package : 
 *	Description : 
 *	Auteur : amurzeau
 *	Version 2 mai 2013
 */

#ifndef ZONE_MUTEX_H
#define	ZONE_MUTEX_H

#include "QS/QS_all.h"
#include "avoidance.h"
#include "QS/QS_who_am_i.h"

//Différentes zone ou les 2 robots passent
typedef enum {
	MZ_Center,			//Centre du terrain, Krusty peut y passer pour prendre les verres proches du centre du terrain
	MZ_StartTiny,		//Case de départ de Tiny, Krusty peut y passer pour déposer les verres
	MZ_CakeNearUs,		//Quart du gateau de notre coté, Krusty peut avoir besoin de s'éloigné des assiettes à cause d'un évitement
} map_zone_e;
#define ZONE_NUMBER 3

typedef enum {
	ZIS_Free,		//La zone est initialement libre
	ZIS_Krusty,		//La zone est initialement à Krusty
	ZIS_Tiny		//La zone est initialement à Tiny
} zone_initial_state_e;

typedef struct {
	zone_initial_state_e init_state;
	robot_id_e owner;
} zone_info_t;

//Attention à mettre des backslahes a la fin des lignes
#define ZONE_INITIAL_STATE_DATA {                    \
/*  {init_state, owner }                    */       \
	{ZIS_Free  , KRUSTY},	/*MZ_Center     */       \
	{ZIS_Tiny  , TINY  },	/*MZ_StartTiny  */       \
	{ZIS_Free  , TINY  }	/*MZ_CakeNearUs */       \
};


//Initialise le système de zone. QS_WHO_AM_I doit être initialisé AVANT !!!
void ZONE_init();

//Tente de verrouiller une zone. Cette fonction marche comme try_goto et autre, on donne des états, elle retourne le bon état suivant l'état de l'action.
Uint8 try_lock_zone(map_zone_e zone, Uint8 timeout_msec, Uint8 in_progress_state, Uint8 success_state, Uint8 cant_lock_state, Uint8 no_response_state);

//Tente de prendre une zone.
//Retourne:
// - IN_PROGRESS: la zone n'est pas encore dispo, mais on continue de regarder
// - END_OK: la zone est dispo on peut y aller
// - NOT_HANDLED: on a attendu plus de timeout_ms et la zone n'était toujours pas dispo. Mais l'autre robot répond à nos requetes
// - END_WITH_TIMEOUT: l'autre robot ne répond pas !!
error_e ZONE_try_lock(map_zone_e zone, Uint16 timeout_ms);

//Libère une zone verrouillée
void ZONE_unlock(map_zone_e zone);

//Renvoi TRUE si on a verrouillé la zone pour nous ou si la zone est dispo.
//Il faut quand même faire un try_lock pour aller dans la zone !
bool_e ZONE_is_free(map_zone_e zone);

//Gère les messages CAN liés au zones
void ZONE_CAN_process_msg(CAN_msg_t *msg);


#endif	/* ZONE_MUTEX_H */

