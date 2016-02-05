/*  Club Robot ESEO 2015 - 2016
 *	BIG
 *
 *	Fichier : bottom_dune_left.h
 *	Package : Carte actionneur
 *	Description : Gestion de l'actionneur RX24 BOTTOM_DUNE_LEFT
 *  Auteur : Julien
 *  Version 2016
 *  Robot : BIG
 */

#ifndef BOTTOM_DUNE_LEFT_H
	#define	BOTTOM_DUNE_LEFT_H

	#include "../QS/QS_all.h"
	#include "../QS/QS_CANmsgList.h"

	#include "../queue.h"

	/** Initialisation du gestionnaire de BOTTOM_DUNE_LEFT.
	 *
	 * Configure l'asservissement du RX24
	 */
	void BOTTOM_DUNE_LEFT_init();

	/** Initialisation en position du gestionnaire de BOTTOM_DUNE_LEFT.
	 *
	 * Initialise la position du RX24
	 */
	void BOTTOM_DUNE_LEFT_init_pos();

	/** Stoppe l'actionneur.
	 *
	 * Cette fonction est appel�e en fin de match
	 */
	void BOTTOM_DUNE_LEFT_stop();

	/** G�re les messages CAN li�s � la BOTTOM_DUNE_LEFT.
	 *
	 * Cette fonction s'occupe toute seule de g�rer la queue et d'agir en cons�quence.
	 * @param msg le message CAN
	 * @return TRUE si le message CAN a �t� g�r� par cet actionneur, FALSE sinon
	 */
	bool_e BOTTOM_DUNE_LEFT_CAN_process_msg(CAN_msg_t* msg);

	/** G�re les commandes demand�es.
	 *
	 * Fonction � mettre sur la file pour demander une action.
	 */
	void BOTTOM_DUNE_LEFT_run_command(queue_id_t queueId, bool_e init);


	/**
	 * R�initialise la configuration du RX24
	 */
	void BOTTOM_DUNE_LEFT_reset_config();

#endif	/* BOTTOM_DUNE_LEFT_H */
