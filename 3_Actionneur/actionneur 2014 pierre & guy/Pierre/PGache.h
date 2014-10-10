/*  Club Robot ESEO 2013 - 2014
 *	BIG
 *
 *	Fichier : filet.h
 *	Package : Carte actionneur
 *	Description : Gestion du filet
 *  Auteur : Elise FERCHAUD
 *  Version 20130219
 *  Robot : BIG
 */

#ifndef PGACHE_H
#define	PGACHE_H

#include "../QS/QS_all.h"
#ifdef I_AM_ROBOT_BIG

#include "../queue.h"

/** Initialisation du gestionnaire de la GACHE.
 *
 * Configure l'asservissement de l'AX12
 */
void GACHE_init();

/** Initialisation en position du gestionnaire de la GACHE.
 *
 * Initialise la position de l'AX12
 */
void GACHE_init_pos();

/** Stoppe l'actionneur.
 *
 * Cette fonction est appelée en fin de match
 */
void GACHE_stop();

/** Gère les messages CAN liés au GACHE du filet.
 *
 * Cette fonction s'occupe toute seule de gérer la queue et d'agir en conséquence.
 * @param msg le message CAN
 * @return TRUE si le message CAN a été géré par cet actionneur, FALSE sinon
 */
bool_e GACHE_CAN_process_msg(CAN_msg_t* msg);

/** Gère les commandes demandées.
 *
 * Fonction à mettre sur la file pour demander une action.
 */
void GACHE_run_command(queue_id_t queueId, bool_e init);

/**
 * Réinitialise la configuration de l'AX12
 */
void GACHE_reset_config();

#endif  /* I_AM_ROBOT_BIG */
#endif	/* PGACHE_H */

