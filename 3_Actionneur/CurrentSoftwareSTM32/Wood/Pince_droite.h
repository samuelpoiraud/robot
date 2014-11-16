/*  Club Robot ESEO 2014 - 2015
 *	SMALL
 *
 *	Fichier : PSmallArm.h
 *	Package : Carte actionneur
 *	Description : Gestion de la pince droite
 *  Auteur : Valentin
 *  Version 20130219
 *  Robot : SMALL
 */

#ifndef PINCE_DROITE_H
#define	PINCE_DROITE_H

#if 1

#include "../QS/QS_all.h"

#include "../queue.h"

/** Initialisation du gestionnaire de PINCE_DROITE.
 *
 * Configure l'asservissement de l'AX12
 */
void PINCE_DROITE_init();

/** Initialisation en position du gestionnaire de PINCE_DROITE.
 *
 * Initialise la position de l'AX12
 */
void PINCE_DROITE_init_pos();

/** Stoppe l'actionneur.
 *
 * Cette fonction est appelée en fin de match
 */
void PINCE_DROITE_stop();

/** Gère les messages CAN liés à la PINCE_DROITE.
 *
 * Cette fonction s'occupe toute seule de gérer la queue et d'agir en conséquence.
 * @param msg le message CAN
 * @return TRUE si le message CAN a été géré par cet actionneur, FALSE sinon
 */
bool_e PINCE_DROITE_CAN_process_msg(CAN_msg_t* msg);

/** Gère les commandes demandées.
 *
 * Fonction à mettre sur la file pour demander une action.
 */
void PINCE_DROITE_run_command(queue_id_t queueId, bool_e init);


/**
 * Réinitialise la configuration de l'AX12
 */
void PINCE_DROITE_reset_config();

#endif

#endif	/* PINCE_DROITE_H */

