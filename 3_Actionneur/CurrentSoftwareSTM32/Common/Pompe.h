/*  Club Robot ESEO 2013 - 2014
 *	BIG
 *
 *	Fichier : pompe.h
 *	Package : Carte actionneur
 *	Description : Gestion de la pompe
 *  Auteur : Arnaud
 *  Version 20130219
 *  Robot : Les deux
 */

#ifndef PPOMPE_H
#define	PPOMPE_H

#include "../QS/QS_all.h"
#include "../queue.h"

/** Initialisation du gestionnaire de POMPE.
 *
 * Configure du moteur de la pompe
 */
void POMPE_init();

/** Initialisation en position du gestionnaire de POMPE.
 *
 * Initialise de la position de la pompe (stop�)
 */
void POMPE_init_pos();

/** Stoppe l'actionneur.
 *
 * Cette fonction est appel�e en fin de match
 */
void POMPE_stop();

/** G�re les messages CAN li�s au POMPE.
 *
 * Cette fonction s'occupe toute seule de g�rer la queue et d'agir en cons�quence.
 * @param msg le message CAN
 * @return TRUE si le message CAN a �t� g�r� par cet actionneur, FALSE sinon
 */
bool_e POMPE_CAN_process_msg(CAN_msg_t* msg);

/** G�re les commandes demand�es.
 *
 * Fonction � mettre sur la file pour demander une action.
 */
void POMPE_run_command(queue_id_t queueId, bool_e init);

#endif	/* PPOMPE_H */

