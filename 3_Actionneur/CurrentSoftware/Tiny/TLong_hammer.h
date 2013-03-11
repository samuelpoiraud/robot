/*  Club Robot ESEO 2012 - 2013
 *	Tiny
 *
 *	Fichier : Long_hammer.h
 *	Package : Carte actionneur
 *	Description : Gestion du marteau appuyant sur les bougies du haut
 *  Auteur : Alexis
 *  Version 20130207
 *  Robot : Tiny
 */

#ifndef LONG_HAMMER_H
#define	LONG_HAMMER_H

#include "../QS/QS_all.h"
#ifdef I_AM_ROBOT_TINY

#include "../queue.h"

/** Initialisation du gestionnaire du bras.
 *
 * Configure DCMotor pour l'asservissement du bras.
 */
void LONGHAMMER_init();

/** G�re les messages CAN li�s au bras.
 *
 * Cette fonction s'occupe toute seule de g�rer la queue et d'asservir le bras � la position voulue.
 * @param msg le message CAN
 * @return TRUE si le message CAN a �t� g�r� par cet actionneur, FALSE sinon
 */
bool_e LONGHAMMER_CAN_process_msg(CAN_msg_t* msg);

#endif  /* I_AM_ROBOT_TINY */
#endif	/* LONG_HAMMER_H */
