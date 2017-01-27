/*  Club Robot ESEO 2016 - 2017
*
*	Fichier : ORE_WALL_rx24.h
*	Package : Carte actionneur
*	Description : Gestion de l'actionneur RX24 ORE_WALL
*	Auteur :
*	Version 2017
*	Robot : BIG
*/

#ifndef ORE_WALL_H
	#define	ORE_WALL_H

	#include "../QS/QS_all.h"
	#include "../QS/QS_CANmsgList.h"

	#include "../queue.h"

	/** Initialisation du gestionnaire de ORE_WALL.
	 *
	 * Configure l'asservissement du RX24
	 */
	void ORE_WALL_init();

	/** Initialisation en position du gestionnaire de ORE_WALL.
	 *
	 * Initialise la position du RX24
	 */
	void ORE_WALL_init_pos();

	/** Stoppe l'actionneur.
	 *
	 * Cette fonction est appelée en fin de match
	 */
	void ORE_WALL_stop();

	/** Gère les messages CAN liés à la ORE_WALL.
	 *
	 * Cette fonction s'occupe toute seule de gérer la queue et d'agir en conséquence.
	 * @param msg le message CAN
	 * @return TRUE si le message CAN a été géré par cet actionneur, FALSE sinon
	 */
	bool_e ORE_WALL_CAN_process_msg(CAN_msg_t* msg);

	/** Gère les commandes demandées.
	 *
	 * Fonction à mettre sur la file pour demander une action.
	 */
	void ORE_WALL_run_command(queue_id_t queueId, bool_e init);


	/**
	 * Réinitialise la configuration du RX24
	 */
	void ORE_WALL_reset_config();

#endif	/* ORE_WALL_H */

