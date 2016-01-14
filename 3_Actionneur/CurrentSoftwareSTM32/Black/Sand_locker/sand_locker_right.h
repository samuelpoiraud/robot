/*  Club Robot ESEO 2015 - 2016
 *	BIG
 *
 *	Fichier : sand_locker_right.h
 *	Package : Carte actionneur
 *	Description : Gestion de l'actionneur RX24 SAND_LOCKER_RIGHT
 *  Auteur : Valentin
 *  Version 2016
 *  Robot : BIG
 */

#ifndef SAND_LOCKER_RIGHT_H
	#define	SAND_LOCKER_RIGHT_H

	#include "../QS/QS_all.h"
	#include "../QS/QS_CANmsgList.h"

	#include "../queue.h"

	/** Initialisation du gestionnaire de SAND_LOCKER_RIGHT.
	 *
	 * Configure l'asservissement du RX24
	 */
	void SAND_LOCKER_RIGHT_init();

	/** Initialisation en position du gestionnaire de SAND_LOCKER_RIGHT.
	 *
	 * Initialise la position du RX24
	 */
	void SAND_LOCKER_RIGHT_init_pos();

	/** Stoppe l'actionneur.
	 *
	 * Cette fonction est appelée en fin de match
	 */
	void SAND_LOCKER_RIGHT_stop();

	/** Gère les messages CAN liés à la SAND_LOCKER_RIGHT.
	 *
	 * Cette fonction s'occupe toute seule de gérer la queue et d'agir en conséquence.
	 * @param msg le message CAN
	 * @return TRUE si le message CAN a été géré par cet actionneur, FALSE sinon
	 */
	bool_e SAND_LOCKER_RIGHT_CAN_process_msg(CAN_msg_t* msg);

	/** Gère les commandes demandées.
	 *
	 * Fonction à mettre sur la file pour demander une action.
	 */
	void SAND_LOCKER_RIGHT_run_command(queue_id_t queueId, bool_e init);


	/**
	 * Réinitialise la configuration du RX24
	 */
	void SAND_LOCKER_RIGHT_reset_config();

#endif	/* SAND_LOCKER_RIGHT_H */

