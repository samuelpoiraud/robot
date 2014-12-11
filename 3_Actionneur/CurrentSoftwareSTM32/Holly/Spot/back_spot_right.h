/*  Club Robot ESEO 2014 - 2015
 *	HOLLY - WOOD
 *
 *	Fichier : back_spot_right.h
 *	Package : Carte actionneur
 *	Description : Gestion de l'ax12 de la ventouse de la prise de spot arrière droite
 *  Auteur : Arnaud
 *  Version 20130219
 *  Robot : HOLLY
 */

#ifndef BACK_SPOT_RIGHT_H
#define	BACK_SPOT_RIGHT_H

#include "../QS/QS_all.h"

#ifdef I_AM_ROBOT_BIG
#include "../queue.h"

/** Initialisation du gestionnaire de la BACK_SPOT_RIGHT.
 *
 * Configure l'asservissement de l'AX12
 */
void BACK_SPOT_RIGHT_init();

/** Initialisation en position du gestionnaire de la BACK_SPOT_RIGHT.
 *
 * Initialise la position de l'AX12
 */
void BACK_SPOT_RIGHT_init_pos();

/** Stoppe l'actionneur.
 *
 * Cette fonction est appelée en fin de match
 */
void BACK_SPOT_RIGHT_stop();

/** Gère les messages CAN liés a la BACK_SPOT_RIGHT.
 *
 * Cette fonction s'occupe toute seule de gérer la queue et d'agir en conséquence.
 * @param msg le message CAN
 * @return TRUE si le message CAN a été géré par cet actionneur, FALSE sinon
 */
bool_e BACK_SPOT_RIGHT_CAN_process_msg(CAN_msg_t* msg);

/** Gère les commandes demandées.
 *
 * Fonction à mettre sur la file pour demander une action.
 */
void BACK_SPOT_RIGHT_run_command(queue_id_t queueId, bool_e init);


/**
 * Réinitialise la configuration de l'AX12
 */
void BACK_SPOT_RIGHT_reset_config();

#endif	/* BACK_SPOT_RIGHT_H */

#endif
