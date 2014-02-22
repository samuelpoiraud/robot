/*  Club Robot ESEO 2013 - 2014
 *	BIG
 *
 *	Fichier : Fruit.h
 *	Package : Carte actionneur
 *	Description : Gestion des fruit_mouths
 *  Auteur : Amaury
 *  Version 20130219
 *  Robot : BIG
 */

#ifndef PFRUIT_H
#define	PFRUIT_H

#include "../QS/QS_all.h"
#ifdef I_AM_ROBOT_BIG

#include "../queue.h"

/** Initialisation du gestionnaire de FRUIT.
 *
 * Configure l'asservissement de l'AX12
 */
void FRUIT_init();

/** Initialisation en position du gestionnaire de FRUIT.
 *
 * Initialise la position de l'AX12
 */
void FRUIT_init_pos();

/** Stoppe l'actionneur.
 *
 * Cette fonction est appelée en fin de match
 */
void FRUIT_stop();

/** Gère les messages CAN liés au fruit.
 *
 * Cette fonction s'occupe toute seule de gérer la queue et d'agir en conséquence.
 * @param msg le message CAN
 * @return TRUE si le message CAN a été géré par cet actionneur, FALSE sinon
 */
bool_e FRUIT_CAN_process_msg(CAN_msg_t* msg);

/** Gère les commandes demandées.
 *
 * Fonction à mettre sur la file pour demander une action.
 */
void FRUIT_run_command(queue_id_t queueId, bool_e init);

#endif  /* I_AM_ROBOT_BIG */
#endif	/* PFRUIT_H */

