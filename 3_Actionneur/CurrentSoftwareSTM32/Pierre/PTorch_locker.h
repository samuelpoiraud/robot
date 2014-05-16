/*  Club Robot ESEO 2013 - 2014
 *
 *	Fichier : PTorch_locker.h
 *	Package : Carte actionneur
 *	Description : Gestion du syst�me de v�rouillage des torches
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
 * Cette fonction est appel�e en fin de match
 */
void TORCH_LOCKER_stop();

/**	Remets les derni�res position des bras
 * de la torche si ils sont partis en erreur
 */
void PTORCH_process_main();

/** G�re les messages CAN li�s au TORCH_LOCKER.
 *
 * Cette fonction s'occupe toute seule de g�rer la queue et d'agir en cons�quence.
 * @param msg le message CAN
 * @return TRUE si le message CAN a �t� g�r� par cet actionneur, FALSE sinon
 */
bool_e TORCH_LOCKER_CAN_process_msg(CAN_msg_t* msg);

/** G�re les commandes demand�es.
 *
 * Fonction � mettre sur la file pour demander une action.
 */
void TORCH_LOCKER_run_command(queue_id_t queueId, bool_e init);

#endif	/* PTORCH_LOCKER_H */

