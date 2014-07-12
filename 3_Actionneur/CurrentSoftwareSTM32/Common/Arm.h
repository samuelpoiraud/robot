/*
 *  Club Robot ESEO 2013 - 2014
 *
 *  $Id$
 *
 *  Package : Carte Actionneur
 *  Description : Gestion du bras
 *  Auteur : Alexis
 *  Robot : Pierre & Guy
 */

#ifndef ARM_H
#define	ARM_H

#include "../QS/QS_all.h"

#include "../queue.h"

/** Initialisation du gestionnaire des ascenseurs.
 *
 * Configure l'asservissement du moteur pour la translation d'un ascenseurs et l'AX12 qui sert la pince
 */
void ARM_init();

/** Initialisation en position du gestionnaire de ARM.
 *
 * Initialise la position de l'AX12
 */
void ARM_init_pos();

/** Stoppe l'actionneur.
 *
 * Cette fonction est appelée en fin de match
 */
void ARM_stop();

/** Gère les messages CAN liés au bras.
 *
 * Cette fonction s'occupe toute seule de gérer la queue et d'agir en conséquence.
 * @param msg le message CAN
 * @return TRUE si le message CAN a été géré par cet actionneur, FALSE sinon
 */
bool_e ARM_CAN_process_msg(CAN_msg_t* msg);

/** Gère les commandes demandées.
 *
 * Fonction à mettre sur la file pour demander une action.
 */
void ARM_run_command(queue_id_t queueId, bool_e init);

/**
 * Réinitialise la configuration de l'AX12
 */
void ARM_reset_config();

#endif	/* ARM_H */

