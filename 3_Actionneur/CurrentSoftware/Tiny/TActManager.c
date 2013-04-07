/*  Club Robot ESEO 2012 - 2013
 *	Tiny
 *
 *	Fichier : TActManager.c
 *	Package : Carte actionneur
 *	Description : Gestion des actionneurs de Tiny
 *  Auteur : Alexis
 *  Version 20130227
 *  Robot : Tiny
 */

#include "TActManager.h"
#ifdef I_AM_ROBOT_TINY

#include "THammer.h"
#include "TBall_inflater.h"
#include "TCandle_color_sensor.h"

//Initialise les actionneurs
void ACTMGR_init() {
	HAMMER_init();
	BALLINFLATER_init();
	CANDLECOLOR_init();
}

//Gère les messages CAN des actionneurs. Si le message à été géré, cette fonction renvoie TRUE, sinon FALSE.
bool_e ACTMGR_process_msg(CAN_msg_t* msg) {
	if(HAMMER_CAN_process_msg(msg))
		return TRUE;
	if(BALLINFLATER_CAN_process_msg(msg))
		return TRUE;
	if(CANDLECOLOR_CAN_process_msg(msg))
		return TRUE;

	return FALSE;
}

//Stop tous les actionneurs
void ACTMGR_stop() {
	HAMMER_stop();
	BALLINFLATER_stop();
	CANDLECOLOR_stop();
}

#endif  /* I_AM_ROBOT_TINY */
