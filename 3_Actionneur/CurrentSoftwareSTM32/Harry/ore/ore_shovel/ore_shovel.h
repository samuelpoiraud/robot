/*  Club Robot ESEO 2016 - 2017
*
*	Fichier : ORE_SHOVEL_rx24.h
*	Package : Carte actionneur
*	Description : Gestion de l'actionneur RX24 ORE_SHOVEL
*	Auteur :
*	Version 2017
*	Robot : BIG
*/

#ifndef ORE_SHOVEL_H
	#define	ORE_SHOVEL_H

	#include "../QS/QS_all.h"
	#include "../QS/QS_CANmsgList.h"

	#include "../queue.h"

	/** Initialisation du gestionnaire de ORE_SHOVEL.
	 *
	 * Configure l'asservissement du RX24
	 */
	void ORE_SHOVEL_init();

	/** Initialisation en position du gestionnaire de ORE_SHOVEL.
	 *
	 * Initialise la position du RX24
	 */
	void ORE_SHOVEL_init_pos();

	/** Stoppe l'actionneur.
	 *
	 * Cette fonction est appelée en fin de match
	 */
	void ORE_SHOVEL_stop();

	/** Gère les messages CAN liés à la ORE_SHOVEL.
	 *
	 * Cette fonction s'occupe toute seule de gérer la queue et d'agir en conséquence.
	 * @param msg le message CAN
	 * @return TRUE si le message CAN a été géré par cet actionneur, FALSE sinon
	 */
	bool_e ORE_SHOVEL_CAN_process_msg(CAN_msg_t* msg);

	/** Gère les commandes demandées.
	 *
	 * Fonction à mettre sur la file pour demander une action.
	 */
	void ORE_SHOVEL_run_command(queue_id_t queueId, bool_e init);


	/**
	 * Réinitialise la configuration du RX24
	 */
	void ORE_SHOVEL_reset_config();

#endif	/* ORE_SHOVEL_H */

