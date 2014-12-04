/*  Club Robot ESEO 2014 - 2015
 *	SMALL
 *
 *	Fichier : Pince_devant.h
 *	Package : Carte actionneur
 *	Description : Gestion de la pince de devant
 *  Auteur : Valentin
 *  Version 20130219
 *  Robot : SMALL
 */

#ifndef PINCE_DEVANT_H
#define	PINCE_DEVANT_H

#include "../QS/QS_all.h"

#include "../queue.h"

/** Initialisation du gestionnaire de PINCE_DEVANT.
 *
 * Configure l'asservissement de l'AX12
 */
void PINCE_DEVANT_init();

/** Initialisation en position du gestionnaire de PINCE_DEVANT.
 *
 * Initialise la position de l'AX12
 */
void PINCE_DEVANT_init_pos();

/** Stoppe l'actionneur.
 *
 * Cette fonction est appelée en fin de match
 */
void PINCE_DEVANT_stop();

/** Gère les messages CAN liés au PINCE_DEVANT.
 *
 * Cette fonction s'occupe toute seule de gérer la queue et d'agir en conséquence.
 * @param msg le message CAN
 * @return TRUE si le message CAN a été géré par cet actionneur, FALSE sinon
 */
bool_e PINCE_DEVANT_CAN_process_msg(CAN_msg_t* msg);

/** Gère les commandes demandées.
 *
 * Fonction à mettre sur la file pour demander une action.
 */
void PINCE_DEVANT_run_command(queue_id_t queueId, bool_e init);


/**
 * Réinitialise la configuration de l'AX12
 */
void PINCE_DEVANT_reset_config();

#endif	/* PINCE_DEVANT_H */

