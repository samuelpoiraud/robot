/*  Club Robot ESEO 2013 - 2014
 *	SMALL
 *
 *	Fichier : PSmallArm.h
 *	Package : Carte actionneur
 *	Description : Gestion du petit bras
 *  Auteur : Arnaud
 *  Version 20130219
 *  Robot : SMALL
 */

#ifndef PEXEMPLE_H
#define	PEXEMPLE_H

#if 0

#include "../QS/QS_all.h"

#include "../queue.h"

/** Initialisation du gestionnaire de EXEMPLE.
 *
 * Configure l'asservissement de l'AX12
 */
void EXEMPLE_init();

/** Initialisation en position du gestionnaire de EXEMPLE.
 *
 * Initialise la position de l'AX12
 */
void EXEMPLE_init_pos();

/** Stoppe l'actionneur.
 *
 * Cette fonction est appelée en fin de match
 */
void EXEMPLE_stop();

/** Gère les messages CAN liés au EXEMPLE.
 *
 * Cette fonction s'occupe toute seule de gérer la queue et d'agir en conséquence.
 * @param msg le message CAN
 * @return TRUE si le message CAN a été géré par cet actionneur, FALSE sinon
 */
bool_e EXEMPLE_CAN_process_msg(CAN_msg_t* msg);

/** Gère les commandes demandées.
 *
 * Fonction à mettre sur la file pour demander une action.
 */
void EXEMPLE_run_command(queue_id_t queueId, bool_e init);


/**
 * Réinitialise la configuration de l'AX12
 */
void EXEMPLE_reset_config();

#endif

#endif	/* PEXEMPLE_H */

