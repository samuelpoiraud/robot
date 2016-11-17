/*  Club Robot ESEO 2016 - 2017
*
*	Fichier : exemple_rx24.h
*	Package : Carte actionneur
*	Description : Gestion de l'actionneur RX24 EXEMPLE
*	Auteur :
*	Version 2017
*	Robot : BIG
*/

#ifndef EXEMPLE_H
	#define	EXEMPLE_H

	#include "../QS/QS_all.h"
	#include "../QS/QS_CANmsgList.h"

	#include "../queue.h"

	/** Initialisation du gestionnaire de EXEMPLE.
	 *
	 * Configure l'asservissement du RX24
	 */
	void EXEMPLE_init();

	/** Initialisation en position du gestionnaire de EXEMPLE.
	 *
	 * Initialise la position du RX24
	 */
	void EXEMPLE_init_pos();

	/** Stoppe l'actionneur.
	 *
	 * Cette fonction est appelée en fin de match
	 */
	void EXEMPLE_stop();

	/** Gère les messages CAN liés à la EXEMPLE.
	 *
	 * Cette fonction s'occupe toute seule de gérer la queue et d'agir en conséquence.
	 * @param msg le message CAN
	 * @return TRUE si le message CAN a été géré par cet actionneur, FALSE sinon
	 */
	bool_e EXEMPLE_CAN_process_msg(CAN_msg_t* msg);

	/** Gère les commandes demandées.
	 *
	 * Fonction à mettre sur la file pour demander une action.
	 */
	void EXEMPLE_run_command(queue_id_t queueId, bool_e init);


	/**
	 * Réinitialise la configuration du RX24
	 */
	void EXEMPLE_reset_config();

#endif	/* EXEMPLE_H */

