/*  Club Robot ESEO 2015 - 2016
 *	BIG
 *
 *	Fichier : fish_braush_back.h
 *	Package : Carte actionneur
 *	Description : Gestion de l'actionneur RX24 TOP_PLATE_RIGHT
 *  Auteur : Julien
 *  Version 2016
 *  Robot : BIG
 */

#ifndef TOP_PLATE_RIGHT_H
	#define	TOP_PLATE_RIGHT_H

	#include "../QS/QS_all.h"
	#include "../QS/QS_CANmsgList.h"

	#include "../queue.h"

	/** Initialisation du gestionnaire de TOP_PLATE_RIGHT.
	 *
	 * Configure l'asservissement du RX24
	 */
	void TOP_PLATE_RIGHT_init();

	/** Initialisation en position du gestionnaire de TOP_PLATE_RIGHT.
	 *
	 * Initialise la position du RX24
	 */
	void TOP_PLATE_RIGHT_init_pos();

	/** Stoppe l'actionneur.
	 *
	 * Cette fonction est appelée en fin de match
	 */
	void TOP_PLATE_RIGHT_stop();

	/** Gère les messages CAN liés à la TOP_PLATE_RIGHT.
	 *
	 * Cette fonction s'occupe toute seule de gérer la queue et d'agir en conséquence.
	 * @param msg le message CAN
	 * @return TRUE si le message CAN a été géré par cet actionneur, FALSE sinon
	 */
	bool_e TOP_PLATE_RIGHT_CAN_process_msg(CAN_msg_t* msg);

	/** Gère les commandes demandées.
	 *
	 * Fonction à mettre sur la file pour demander une action.
	 */
	void TOP_PLATE_RIGHT_run_command(queue_id_t queueId, bool_e init);


	/**
	 * Réinitialise la configuration du RX24
	 */
	void TOP_PLATE_RIGHT_reset_config();

#endif	/* TOP_PLATE_RIGHT_H */

