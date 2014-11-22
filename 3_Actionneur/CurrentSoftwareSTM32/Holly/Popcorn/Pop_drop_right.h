/*  Club Robot ESEO 2014 - 2015
 *	HOLLY - WOOD
 *
 *	Fichier : Pop_drop_right.h
 *	Package : Carte actionneur
 *	Description : Gestion de l'ax12 de la depose des popcorns gauche
 *  Auteur : Anthony
 *  Version 20130219
 *  Robot : HOLLY
 */

#ifndef POP_DROP_RIGHT_H
#define	POP_DROP_RIGHT_H

#include "../QS/QS_all.h"

#ifdef I_AM_ROBOT_BIG

#include "../queue.h"

/** Initialisation du gestionnaire de POP_DROP_RIGHT.
 *
 * Configure l'asservissement de l'AX12
 */
void POP_DROP_RIGHT_init();

/** Initialisation en position du gestionnaire de POP_DROP_RIGHT.
 *
 * Initialise la position de l'AX12
 */
void POP_DROP_RIGHT_init_pos();

/** Stoppe l'actionneur.
 *
 * Cette fonction est appelée en fin de match
 */
void POP_DROP_RIGHT_stop();

/** Gère les messages CAN liés à la POP_DROP_RIGHT.
 *
 * Cette fonction s'occupe toute seule de gérer la queue et d'agir en conséquence.
 * @param msg le message CAN
 * @return TRUE si le message CAN a été géré par cet actionneur, FALSE sinon
 */
bool_e POP_DROP_RIGHT_CAN_process_msg(CAN_msg_t* msg);

/** Gère les commandes demandées.
 *
 * Fonction à mettre sur la file pour demander une action.
 */
void POP_DROP_RIGHT_run_command(queue_id_t queueId, bool_e init);


/**
 * Réinitialise la configuration de l'AX12
 */
void POP_DROP_RIGHT_reset_config();

#endif

#endif	/* POP_DROP_RIGHT_H */

