/*  Club Robot ESEO 2015 - 2016
 *	BIG
 *
 *	Fichier : pendulum.h
 *	Package : Carte actionneur
 *	Description : Gestion de l'actionneur RX24 PENDULUM
 *  Auteur : Valentin
 *  Version 2016
 *  Robot : BIG
 */

#ifndef PENDULUM_H
	#define	PENDULUM_H

	#include "../QS/QS_all.h"
	#include "../QS/QS_CANmsgList.h"

	#include "../queue.h"

	/** Initialisation du gestionnaire de PENDULUM.
	 *
	 * Configure l'asservissement du RX24
	 */
	void PENDULUM_init();

	/** Initialisation en position du gestionnaire de PENDULUM.
	 *
	 * Initialise la position du RX24
	 */
	void PENDULUM_init_pos();

	/** Stoppe l'actionneur.
	 *
	 * Cette fonction est appelée en fin de match
	 */
	void PENDULUM_stop();

	/** Gère les messages CAN liés à la PENDULUM.
	 *
	 * Cette fonction s'occupe toute seule de gérer la queue et d'agir en conséquence.
	 * @param msg le message CAN
	 * @return TRUE si le message CAN a été géré par cet actionneur, FALSE sinon
	 */
	bool_e PENDULUM_CAN_process_msg(CAN_msg_t* msg);

	/** Gère les commandes demandées.
	 *
	 * Fonction à mettre sur la file pour demander une action.
	 */
	void PENDULUM_run_command(queue_id_t queueId, bool_e init);


	/**
	 * Réinitialise la configuration du RX24
	 */
	void PENDULUM_reset_config();

#endif	/* PENDULUM_H */

