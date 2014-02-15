/*  Club Robot ESEO 2012 - 2013
 *	BIG
 *
 *	Fichier : BActManager.h
 *	Package : Carte actionneur
 *	Description : Gestion des actionneurs de BIG_robot
 *  Auteur : Alexis
 *  Version 20130227
 *  Robot : big
 */

#ifndef BACT_MANAGER_H
#define	BACT_MANAGER_H

#include "../QS/QS_all.h"
#ifdef I_AM_ROBOT_BIG

	//Initialise les actionneurs
	void ACTMGR_init();

	//Met les actionneurs en position de départ
	void ACTMGR_reset_act();

	//Gère les messages CAN des actionneurs. Si le message à été géré, cette fonction renvoie TRUE, sinon FALSE.
	bool_e ACTMGR_process_msg(CAN_msg_t* msg);

	//Stop tous les actionneurs
	void ACTMGR_stop();

#endif  /* I_AM_ROBOT_BIG */


#endif	/* BACT_MANAGER_H */

