/*  Club Robot ESEO 2012 - 2013
 *	Tiny
 *
 *	Fichier : Ball_inflater.h
 *	Package : Carte actionneur
 *	Description : Gestion du gonfleur du ballon
 *  Auteur : Alexis
 *  Version 20130207
 *  Robot : Tiny
 */

#ifndef BALL_INFLATER_H
#define	BALL_INFLATER_H

#include "../QS/QS_all.h"
#ifdef I_AM_ROBOT_TINY

#include "../queue.h"

/** Initialisation du gestionnaire du bras.
 *
 */
void BALLINFLATER_init();

/** Stoppe l'actionneur.
 *
 * Cette fonction est appelée en fin de match
 */
void BALLINFLATER_stop();

/** Gère les messages CAN liés au bras.
 *
 * Cette fonction s'occupe toute seule de gérer la queue et de lancer le gonflage.
 * @param msg le message CAN
 * @return TRUE si le message CAN a été géré par cet actionneur, FALSE sinon
 */
bool_e BALLINFLATER_CAN_process_msg(CAN_msg_t* msg);

/** Gère les commandes demandées.
 *
 * Fonction à mettre sur la file pour demander une action.
 */
void BALLINFLATER_run_command(queue_id_t queueId, bool_e init);

#endif  /* I_AM_ROBOT_TINY */
#endif	/* BALL_INFLATER_H */
