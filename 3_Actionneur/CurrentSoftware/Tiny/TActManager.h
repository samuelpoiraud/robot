/*  Club Robot ESEO 2012 - 2013
 *	Tiny
 *
 *	Fichier : TActManager.h
 *	Package : Carte actionneur
 *	Description : Gestion des actionneurs de Tiny
 *  Auteur : Alexis
 *  Version 20130227
 *  Robot : Tiny
 */

#ifndef TACT_MANAGER_H
#define	TACT_MANAGER_H

#include "../QS/QS_all.h"
#ifdef I_AM_ROBOT_TINY

	//Initialise les actionneurs
	void ACTMGR_init();

	//Met les actionneurs en position de départ
	void ACTMGR_reset_act();

	//Gère les messages CAN des actionneurs. Si le message à été géré, cette fonction renvoie TRUE, sinon FALSE.
	bool_e ACTMGR_process_msg(CAN_msg_t* msg);

	//Stop tous les actionneurs
	void ACTMGR_stop();

#endif  /* I_AM_ROBOT_TINY */


#endif	/* TACT_MANAGER_H */

