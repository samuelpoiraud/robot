/*  Club Robot ESEO 2015 - 2016
 *	BIG
 *
 *	Fichier : shovel_dune.h
 *	Package : Carte actionneur
 *	Description : Gestion de l'actionneur RX24 SHOVEL_DUNE
 *  Auteur : Arnaud
 *  Version 2016
 *  Robot : BIG
 */

#ifndef SHOVEL_DUNE_H
	#define	SHOVEL_DUNE_H

	#include "../QS/QS_all.h"
	#include "../QS/QS_CANmsgList.h"

	#include "../queue.h"

	/** Initialisation du gestionnaire de SHOVEL_DUNE.
	 *
	 * Configure l'asservissement du RX24
	 */
	void SHOVEL_DUNE_init();

	/** Initialisation en position du gestionnaire de SHOVEL_DUNE.
	 *
	 * Initialise la position du RX24
	 */
	void SHOVEL_DUNE_init_pos();

	/** Stoppe l'actionneur.
	 *
	 * Cette fonction est appel�e en fin de match
	 */
	void SHOVEL_DUNE_stop();

	/** G�re les messages CAN li�s � la SHOVEL_DUNE.
	 *
	 * Cette fonction s'occupe toute seule de g�rer la queue et d'agir en cons�quence.
	 * @param msg le message CAN
	 * @return TRUE si le message CAN a �t� g�r� par cet actionneur, FALSE sinon
	 */
	bool_e SHOVEL_DUNE_CAN_process_msg(CAN_msg_t* msg);

	/**
	 * R�initialise la configuration du RX24
	 */
	void SHOVEL_DUNE_reset_config();

#endif	/* SHOVEL_DUNE_H */

