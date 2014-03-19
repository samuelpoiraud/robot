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

//Diff�rentes zone ou les 2 robots passent
typedef enum {
	MZ_Center,			//Centre du terrain, Krusty peut y passer pour prendre les verres proches du centre du terrain
	MZ_StartTiny,		//Case de d�part de Tiny, Krusty peut y passer pour d�poser les verres
	MZ_CakeNearUs,		//Quart du gateau de notre cot�, Krusty peut avoir besoin de s'�loign� des assiettes � cause d'un �vitement
} map_zone_e;
#define ZONE_MUTEX_NUMBER 3

typedef enum {
	ZIS_Free,		//La zone est initialement libre
	ZIS_Krusty,		//La zone est initialement � Krusty
	ZIS_Tiny		//La zone est initialement � Tiny
} zone_initial_state_e;

typedef enum {
	ZS_Free,			//La zone est libre, on ne se base pas sur cette info, on demandera toujours � l'autre robot si on peut y aller dans la zone
	ZS_OwnedByMe,		//La zone est occup�e par nous-m�me
	ZS_OwnedByOther,	//La zone est occup�e par l'autre robot
	ZS_Acquiring		//On a demand� le verrouillage de la zone, on attend une r�ponse de l'autre robot
} zone_state_e;

typedef struct {
	zone_initial_state_e init_state;
	robot_id_e owner;
} zone_info_t;

//Attention � mettre des backslahes a la fin des lignes
#define ZONE_INITIAL_STATE_DATA {                    \
/*  {init_state, owner�}                    */       \
	{ZIS_Free  , KRUSTY},	/*MZ_Center     */       \
	{ZIS_Tiny  , TINY  },	/*MZ_StartTiny  */       \
	{ZIS_Free  , TINY  }	/*MZ_CakeNearUs */       \
};


//Initialise le syst�me de zone. QS_WHO_AM_I doit �tre initialis� AVANT !!!
void ZONE_init();

//Tente de verrouiller une zone. Cette fonction marche comme try_goto et autre, on donne des �tats, elle retourne le bon �tat suivant l'�tat de l'action.
Uint8 try_lock_zone(map_zone_e zone, Uint16 timeout_msec, Uint8 in_progress_state, Uint8 success_state, Uint8 cant_lock_state, Uint8 no_response_state);

//Tente de prendre une zone.
//Retourne:
// - IN_PROGRESS: la zone n'est pas encore dispo, mais on continue de regarder
// - END_OK: la zone est dispo on peut y aller
// - NOT_HANDLED: on a attendu plus de timeout_ms et la zone n'�tait toujours pas dispo. Mais l'autre robot r�pond � nos requetes
// - END_WITH_TIMEOUT: l'autre robot ne r�pond pas !!
error_e ZONE_try_lock(map_zone_e zone, Uint16 timeout_ms);

//Lib�re une zone verrouill�e
void ZONE_unlock(map_zone_e zone);

//Renvoi TRUE si on a verrouill� la zone pour nous ou si la zone est dispo.
//Il faut quand m�me faire un try_lock pour aller dans la zone !
bool_e ZONE_is_free(map_zone_e zone);

//R�cup�re l'�tat d'une zone
zone_state_e ZONE_get_status(map_zone_e zone);

//G�re les messages CAN li�s au zones
void ZONE_CAN_process_msg(CAN_msg_t *msg);


#endif	/* ZONE_MUTEX_H */

