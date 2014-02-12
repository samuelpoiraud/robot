/*  Club Robot ESEO 2012 - 2013
 *	Krusty
 *
 *	Fichier : Lance_Launcher.h
 *	Package : Carte actionneur
 *	Description : Gestion du lanceur de lance
 *  Auteur : amaury
 *  Version 1
 *  Robot : Krusty
 */

#ifndef LANCE_LAUNCHER_H
#define	LANCE_LAUNCHER_H

#include "../QS/QS_all.h"
#ifdef I_AM_ROBOT_KRUSTY

#include "../queue.h"

/** Initialisation du gestionnaire du bras.
 *
 */
void LANCE_LAUNCHER_init();

/** Stoppe l'actionneur.
 *
 */
void LANCE_LAUNCHER_stop();

/** Gère les messages CAN liés au lancer.
 *
 * Cette fonction s'occupe toute seule de gérer la queue et de lancer la lance.
 * @param msg le message CAN
 * @return TRUE si le message CAN a été géré par cet actionneur, FALSE sinon
 */
bool_e LANCE_LAUNCHER_CAN_process_msg(CAN_msg_t* msg);

/** Gère les commandes demandées.
 *
 * Fonction à mettre sur la file pour demander une action.
 */
void LANCE_LAUNCHER_run_command(queue_id_t queueId, bool_e init);

static bool_e hold_state();

#endif  /* I_AM_ROBOT_KRUSTY */
#endif	/* LANCE_LAUNCHER_H */
