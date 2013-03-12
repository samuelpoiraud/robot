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

/** Gère les messages CAN liés au bras.
 *
 * Cette fonction s'occupe toute seule de gérer la queue et de lancer le gonflage.
 * @param msg le message CAN
 * @return TRUE si le message CAN a été géré par cet actionneur, FALSE sinon
 */
bool_e BALLINFLATER_CAN_process_msg(CAN_msg_t* msg);

#endif  /* I_AM_ROBOT_TINY */
#endif	/* BALL_INFLATER_H */
