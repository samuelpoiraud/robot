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

#include "TLong_hammer.h"

//Initialise les actionneurs
void ACTMGR_init() {
	LONGHAMMER_init();
}

//Gère les messages CAN des actionneurs. Si le message à été géré, cette fonction renvoie TRUE, sinon FALSE.
bool_e ACTMGR_process_msg(CAN_msg_t* msg) {
	if(LONGHAMMER_CAN_process_msg(msg))
		return TRUE;

	return FALSE;
}

#endif  /* I_AM_ROBOT_TINY */
