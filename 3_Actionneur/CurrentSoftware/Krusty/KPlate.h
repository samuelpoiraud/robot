/*  Club Robot ESEO 2012 - 2013
 *	Krusty
 *
 *	Fichier : Plate.h
 *	Package : Carte actionneur
 *	Description : Gestion de l'assiette contenant les cerises
 *  Auteur : Alexis
 *  Version 20130219
 *  Robot : Krusty
 */

#ifndef KPLATE_H
#define	KPLATE_H

#include "../QS/QS_all.h"
#ifdef I_AM_ROBOT_KRUSTY

#include "../queue.h"

/** Initialisation du gestionnaire de l'assiette.
 *
 * Configure l'asservissement du moteur pour la rotation de l'assiette et l'AX12 qui sert la pince
 */
void PLATE_init();

/** Gère les messages CAN liés à l'assiette.
 *
 * Cette fonction s'occupe toute seule de gérer la queue et d'agir en conséquence.
 * @param msg le message CAN
 * @return TRUE si le message CAN a été géré par cet actionneur, FALSE sinon
 */
bool_e PLATE_CAN_process_msg(CAN_msg_t* msg);

/**
 * Exécute une action indiqué par un message CAN par l'intermédiaire de la queue.
 * @param queueId le numéro de la queue
 * @param init TRUE si la commande doit être initialisée, FALSE sinon
 * @param QUEUE_get_arg la commande (data[0] dans le message CAN)
 */
void PLATE_run_command(queue_id_t queueId, bool_e init);

#endif  /* I_AM_ROBOT_KRUSTY */
#endif	/* KPLATE_H */

