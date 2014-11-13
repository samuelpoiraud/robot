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

#ifndef PINCE_GAUCHE_H
#define	PINCE_GAUCHE_H

#if 1

#include "../QS/QS_all.h"

#include "../queue.h"

/** Initialisation du gestionnaire de EXEMPLE.
 *
 * Configure l'asservissement de l'AX12
 */
void PINCE_GAUCHE_init();

/** Initialisation en position du gestionnaire de EXEMPLE.
 *
 * Initialise la position de l'AX12
 */
void PINCE_GAUCHE_init_pos();

/** Stoppe l'actionneur.
 *
 * Cette fonction est appelée en fin de match
 */
void PINCE_GAUCHE_stop();

/** Gère les messages CAN liés au EXEMPLE.
 *
 * Cette fonction s'occupe toute seule de gérer la queue et d'agir en conséquence.
 * @param msg le message CAN
 * @return TRUE si le message CAN a été géré par cet actionneur, FALSE sinon
 */
bool_e PINCE_GAUCHE_CAN_process_msg(CAN_msg_t* msg);

/** Gère les commandes demandées.
 *
 * Fonction à mettre sur la file pour demander une action.
 */
void PINCE_GAUCHE_run_command(queue_id_t queueId, bool_e init);


/**
 * Réinitialise la configuration de l'AX12
 */
void PINCE_GAUCHE_reset_config();

#endif

#endif	/* PEXEMPLE_H */

