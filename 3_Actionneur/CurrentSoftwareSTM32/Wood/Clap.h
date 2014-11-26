/*  Club Robot ESEO 2014 - 2015
 *	SMALL
 *
 *	Fichier : Clap.h
 *	Package : Carte actionneur
 *	Description : Gestion de l'AX12 du clap
 *  Auteur : Valentin
 *  Version 20130219
 *  Robot : SMALL
 */

#ifndef CLAP_H
#define	CLAP_H

#if 1

#include "../QS/QS_all.h"

#include "../queue.h"

/** Initialisation du gestionnaire de CLAP.
 *
 * Configure l'asservissement de l'AX12
 */
void CLAP_init();

/** Initialisation en position du gestionnaire de CLAP.
 *
 * Initialise la position de l'AX12
 */
void CLAP_init_pos();

/** Stoppe l'actionneur.
 *
 * Cette fonction est appelée en fin de match
 */
void CLAP_stop();

/** Gère les messages CAN liés au CLAP.
 *
 * Cette fonction s'occupe toute seule de gérer la queue et d'agir en conséquence.
 * @param msg le message CAN
 * @return TRUE si le message CAN a été géré par cet actionneur, FALSE sinon
 */
bool_e CLAP_CAN_process_msg(CAN_msg_t* msg);

/** Gère les commandes demandées.
 *
 * Fonction à mettre sur la file pour demander une action.
 */
void CLAP_run_command(queue_id_t queueId, bool_e init);


/**
 * Réinitialise la configuration de l'AX12
 */
void CLAP_reset_config();

#endif

#endif	/* CLAP_H */

