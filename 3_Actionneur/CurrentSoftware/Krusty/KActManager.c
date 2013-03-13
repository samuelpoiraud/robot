/*  Club Robot ESEO 2012 - 2013
 *	Krusty
 *
 *	Fichier : KActManager.c
 *	Package : Carte actionneur
 *	Description : Gestion des actionneurs de Krusty
 *  Auteur : Alexis
 *  Version 20130227
 *  Robot : Krusty
 */

#include "KActManager.h"
#ifdef I_AM_ROBOT_KRUSTY

#include "KBall_launcher.h"
#include "KPlate.h"
#include "KLift.h"

void ACTMGR_init() {
	BALLLAUNCHER_init();
	PLATE_init();
	LIFT_init();
}

//G�re les messages CAN des actionneurs. Si le message � �t� g�r�, cette fonction renvoie TRUE, sinon FALSE.
bool_e ACTMGR_process_msg(CAN_msg_t* msg) {
	//Traitement des messages, si la fonction de traitement d'un actionneur retourne TRUE, c'est que le message a �t� trait� donc arr�te le passage du message dans chaque fonction de traitement. (les fonctions de traitement des actionneurs sont responsable de retourner FALSE lorsque le message n'est pas destin� qu'a un actionneur)
	if(BALLLAUNCHER_CAN_process_msg(msg))
		return TRUE;
	if(PLATE_CAN_process_msg(msg))
		return TRUE;
	if(LIFT_CAN_process_msg(msg))
		return TRUE;

	return FALSE;
}

#endif  /* I_AM_ROBOT_KRUSTY */
