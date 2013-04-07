/*  Club Robot ESEO 2012 - 2013
 *	Krusty
 *
 *	Fichier : KActManager.h
 *	Package : Carte actionneur
 *	Description : Gestion des actionneurs de Krusty
 *  Auteur : Alexis
 *  Version 20130227
 *  Robot : Krusty
 */

#ifndef KACT_MANAGER_H
#define	KACT_MANAGER_H

#include "../QS/QS_all.h"
#ifdef I_AM_ROBOT_KRUSTY

	//Initialise les actionneurs
	void ACTMGR_init();

	//Gère les messages CAN des actionneurs. Si le message à été géré, cette fonction renvoie TRUE, sinon FALSE.
	bool_e ACTMGR_process_msg(CAN_msg_t* msg);

	//Stop tous les actionneurs
	void ACTMGR_stop();

#endif  /* I_AM_ROBOT_KRUSTY */


#endif	/* KACT_MANAGER_H */

