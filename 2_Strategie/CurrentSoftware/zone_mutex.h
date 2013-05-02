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

//Différentes zone ou les 2 robots passent
//IMPORTANT: les valeurs paires sont à Krusty et les impaires à Tiny
typedef enum {
	MZ_Center = 0,			//Centre du terrain, Krusty peut y passer pour prendre les verres proches du centre du terrain
	MZ_StartTiny = 1,		//Case de départ de Tiny, Krusty peut y passer pour déposer les verres
	MZ_CakeNearUs = 3,		//Quart du gateau de notre coté, Krusty peut avoir besoin de s'éloigné des assiettes à cause d'un évitement
} map_zone_e;
#define ZONE_NUMBER 3

Uint8 try_lock_zone(map_zone_e zone, Uint8 timeout_msec, Uint8 in_progress_state, Uint8 success_state, Uint8 cant_lock_state, Uint8 no_response_state);
error_e ZONE_try_lock(map_zone_e zone, Uint16 timeout_ms);
bool_e ZONE_is_free(map_zone_e zone);
void ZONE_CAN_process_msg(CAN_msg_t *msg);


#endif	/* ZONE_MUTEX_H */

