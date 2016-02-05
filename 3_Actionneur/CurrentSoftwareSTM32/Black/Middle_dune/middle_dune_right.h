/*  Club Robot ESEO 2015 - 2016
 *	BIG
 *
 *	Fichier : middle_dune_right.h
 *	Package : Carte actionneur
 *	Description : Gestion de l'actionneur RX24 MIDDLE_DUNE_RIGHT
 *  Auteur : Valentin
 *  Version 2016
 *  Robot : BIG
 */

#ifndef MIDDLE_DUNE_RIGHT_H
	#define	MIDDLE_DUNE_RIGHT_H

	#include "../QS/QS_all.h"
	#include "../QS/QS_CANmsgList.h"

	#include "../queue.h"

	/** Initialisation du gestionnaire de MIDDLE_DUNE_RIGHT.
	 *
	 * Configure l'asservissement du RX24
	 */
	void MIDDLE_DUNE_RIGHT_init();

	/** Initialisation en position du gestionnaire de MIDDLE_DUNE_RIGHT.
	 *
	 * Initialise la position du RX24
	 */
	void MIDDLE_DUNE_RIGHT_init_pos();

	/** Stoppe l'actionneur.
	 *
	 * Cette fonction est appel�e en fin de match
	 */
	void MIDDLE_DUNE_RIGHT_stop();

	/** G�re les messages CAN li�s � la MIDDLE_DUNE_RIGHT.
	 *
	 * Cette fonction s'occupe toute seule de g�rer la queue et d'agir en cons�quence.
	 * @param msg le message CAN
	 * @return TRUE si le message CAN a �t� g�r� par cet actionneur, FALSE sinon
	 */
	bool_e MIDDLE_DUNE_RIGHT_CAN_process_msg(CAN_msg_t* msg);

	/** G�re les commandes demand�es.
	 *
	 * Fonction � mettre sur la file pour demander une action.
	 */
	void MIDDLE_DUNE_RIGHT_run_command(queue_id_t queueId, bool_e init);


	/**
	 * R�initialise la configuration du RX24
	 */
	void MIDDLE_DUNE_RIGHT_reset_config();

#endif	/* MIDDLE_DUNE_RIGHT_H */
