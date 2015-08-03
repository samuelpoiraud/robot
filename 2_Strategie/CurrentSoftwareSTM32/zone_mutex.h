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

typedef enum {
	ZIS_Free,		//La zone est initialement libre
	ZIS_BIG,		//La zone appartient au BIG_ROBOT
	ZIS_SMALL		//La zone appartient au SMALL_ROBOT
} zone_owner_e;

typedef enum {

	ZS_Free,			//La zone est libre, on ne se base pas sur cette info, on demandera toujours � l'autre robot si on peut y aller dans la zone
	ZS_OwnedByMe,		//La zone est occup�e par nous-m�me quand j'occupe une zone libre ou qui ne apprtient pas
	ZS_OwnedByOther,	//La zone est occup�e par l'autre robot
	ZS_Acquiring,		//On a demand� le verrouillage de la zone, on attend une r�ponse de l'autre robot
	ZS_OtherTryLock		//L'autre robot a fait une demande pour aller dans une de nos zones

} zone_state_e;

typedef struct {
	zone_owner_e owner;
	zone_state_e state;
	bool_e accept_donate;

} zone_info_t;

//Attention � mettre des backslahes a la fin des lignes
#define ZONE_INITIAL_STATE_DATA {                    \
/*  {owner, state }                    */       \
	{ZIS_BIG  , ZS_Free , FALSE },		/*MZ_MAMMOUTH_OUR*/			\
	{ZIS_BIG  , ZS_Free , FALSE },		/*MZ_FRUIT_TRAY */			\
	{ZIS_Free  , ZS_Free ,FALSE }		/*MZ_ZONE_LIBRE */			\
};


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

bool_e ZONE_validate(map_zone_e zone);

// Si a un moment du match, nous savons que nous passerons plus par l�, nous pouvons donner une zone
void ZONE_donate(map_zone_e zone);

#endif	/* ZONE_MUTEX_H */

