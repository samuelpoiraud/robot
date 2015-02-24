/*  Club Robot ESEO 2014 - 2015
 *	SMALL
 *
 *	Fichier : Pop_drop_right_Wood.h
 *	Package : Carte actionneur
 *	Description : Gestion du bras pour faire tomber les pop corns dans le gobelet droit
 *  Auteur : Valentin
 *  Version 2015
 *  Robot : SMALL
 */

#ifndef POP_DROP_RIGHT_WOOD_H
#define	POP_DROP_RIGHT_WOOD_H



#include "../QS/QS_all.h"

#include "../queue.h"

/** Initialisation du gestionnaire de POP_DROP_RIGHT_WOOD.
 *
 * Configure l'asservissement de l'AX12
 */
void POP_DROP_RIGHT_WOOD_init();

/** Initialisation en position du gestionnaire de POP_DROP_RIGHT_WOOD.
 *
 * Initialise la position de l'AX12
 */
void POP_DROP_RIGHT_WOOD_init_pos();

/** Stoppe l'actionneur.
 *
 * Cette fonction est appelée en fin de match
 */
void POP_DROP_RIGHT_WOOD_stop();

/** Gère les messages CAN liés au POP_DROP_RIGHT_WOOD.
 *
 * Cette fonction s'occupe toute seule de gérer la queue et d'agir en conséquence.
 * @param msg le message CAN
 * @return TRUE si le message CAN a été géré par cet actionneur, FALSE sinon
 */
bool_e POP_DROP_RIGHT_WOOD_CAN_process_msg(CAN_msg_t* msg);

/** Gère les commandes demandées.
 *
 * Fonction à mettre sur la file pour demander une action.
 */
void POP_DROP_RIGHT_WOOD_run_command(queue_id_t queueId, bool_e init);


/**
 * Réinitialise la configuration de l'AX12
 */
void POP_DROP_RIGHT_WOOD_reset_config();



#endif	/* POP_DROP_RIGHT_WOOD_H */

