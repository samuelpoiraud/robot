/*  Club Robot ESEO 2012 - 2013
 *	Tiny
 *
 *	Fichier : Hammer.h
 *	Package : Carte actionneur
 *	Description : Gestion du marteau appuyant sur les bougies du bas
 *  Auteur : Alexis
 *  Version 20130312
 *  Robot : Tiny
 */

#ifndef HAMMER_H
#define	HAMMER_H

#include "../QS/QS_all.h"
#ifdef I_AM_ROBOT_TINY

#include "../queue.h"

/** Initialisation du gestionnaire du bras.
 *
 * Configure DCMotor pour l'asservissement du bras.
 */
void HAMMER_init();

/** Stoppe l'actionneur.
 *
 * Cette fonction est appel�e en fin de match
 */
void HAMMER_stop();

/** R�cup�re la position courant du bras en degr�s.
 */
Uint16 HAMMER_get_pos();

/** G�re les messages CAN li�s au bras.
 *
 * Cette fonction s'occupe toute seule de g�rer la queue et d'asservir le bras � la position voulue.
 * @param msg le message CAN
 * @return TRUE si le message CAN a �t� g�r� par cet actionneur, FALSE sinon
 */
bool_e HAMMER_CAN_process_msg(CAN_msg_t* msg);

#endif  /* I_AM_ROBOT_TINY */
#endif	/* HAMMER_H */
