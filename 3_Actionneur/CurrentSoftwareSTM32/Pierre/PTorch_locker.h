/*  Club Robot ESEO 2013 - 2014
 *
 *	Fichier : PTorch_locker.h
 *	Package : Carte actionneur
 *	Description : Gestion du système de vérouillage des torches
 *  Auteur : Arnaud
 *  Version 20130219
 *  Robot : BIG
 */

#ifndef PTORCH_LOCKER_H
#define	PTORCH_LOCKER_H

#include "../QS/QS_all.h"

#include "../queue.h"

/** Initialisation du gestionnaire de TORCH_LOCKER.
 *
 * Configure l'asservissement de l'AX12
 */
void TORCH_LOCKER_init();

/** Initialisation en position du gestionnaire de TORCH_LOCKER.
 *
 * Initialise la position de l'AX12
 */
void TORCH_LOCKER_init_pos();

/** Stoppe l'actionneur.
 *
 * Cette fonction est appelée en fin de match
 */
void TORCH_LOCKER_stop();

/**	Remets les derniéres position des bras
 * de la torche si ils sont partis en erreur
 */
void PTORCH_process_main();

/** Gère les messages CAN liés au TORCH_LOCKER.
 *
 * Cette fonction s'occupe toute seule de gérer la queue et d'agir en conséquence.
 * @param msg le message CAN
 * @return TRUE si le message CAN a été géré par cet actionneur, FALSE sinon
 */
bool_e TORCH_LOCKER_CAN_process_msg(CAN_msg_t* msg);

/** Gère les commandes demandées.
 *
 * Fonction à mettre sur la file pour demander une action.
 */
void TORCH_LOCKER_run_command(queue_id_t queueId, bool_e init);

#endif	/* PTORCH_LOCKER_H */

