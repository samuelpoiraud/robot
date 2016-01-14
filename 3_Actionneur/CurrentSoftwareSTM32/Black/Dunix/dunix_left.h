/*  Club Robot ESEO 2015 - 2016
 *	BIG
 *
 *	Fichier : dunix_left.h
 *	Package : Carte actionneur
 *	Description : Gestion de l'actionneur RX24 DUNIX_LEFT
 *  Auteur : Valentin
 *  Version 2016
 *  Robot : BIG
 */

#ifndef DUNIX_LEFT_H
	#define	DUNIX_LEFT_H

	#include "../QS/QS_all.h"
	#include "../QS/QS_CANmsgList.h"

	#include "../queue.h"

	/** Initialisation du gestionnaire de DUNIX_LEFT.
	 *
	 * Configure l'asservissement du RX24
	 */
	void DUNIX_LEFT_init();

	/** Initialisation en position du gestionnaire de DUNIX_LEFT.
	 *
	 * Initialise la position du RX24
	 */
	void DUNIX_LEFT_init_pos();

	/** Stoppe l'actionneur.
	 *
	 * Cette fonction est appelée en fin de match
	 */
	void DUNIX_LEFT_stop();

	/** Gère les messages CAN liés à la DUNIX_LEFT.
	 *
	 * Cette fonction s'occupe toute seule de gérer la queue et d'agir en conséquence.
	 * @param msg le message CAN
	 * @return TRUE si le message CAN a été géré par cet actionneur, FALSE sinon
	 */
	bool_e DUNIX_LEFT_CAN_process_msg(CAN_msg_t* msg);

	/** Gère les commandes demandées.
	 *
	 * Fonction à mettre sur la file pour demander une action.
	 */
	void DUNIX_LEFT_run_command(queue_id_t queueId, bool_e init);


	/**
	 * Réinitialise la configuration du RX24
	 */
	void DUNIX_LEFT_reset_config();

#endif	/* DUNIX_LEFT_H */

