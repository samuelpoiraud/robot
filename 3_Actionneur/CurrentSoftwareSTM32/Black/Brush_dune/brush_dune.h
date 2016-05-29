/*  Club Robot ESEO 2015 - 2016
 *	BIG
 *
 *	Fichier : brush_dune.h
 *	Package : Carte actionneur
 *	Description : Gestion de l'actionneur RX24 BRUSH_DUNE
 *  Auteur : Arnaud
 *  Version 2016
 *  Robot : BIG
 */

#ifndef BRUSH_DUNE_H
	#define	BRUSH_DUNE_H

	#include "../QS/QS_all.h"
	#include "../QS/QS_CANmsgList.h"

	#include "../queue.h"

	/** Initialisation du gestionnaire de BRUSH_DUNE.
	 *
	 * Configure l'asservissement du RX24
	 */
	void BRUSH_DUNE_init();

	/** Initialisation en position du gestionnaire de BRUSH_DUNE.
	 *
	 * Initialise la position du RX24
	 */
	void BRUSH_DUNE_init_pos();

	/** Stoppe l'actionneur.
	 *
	 * Cette fonction est appel�e en fin de match
	 */
	void BRUSH_DUNE_stop();

	/** G�re les messages CAN li�s � la BRUSH_DUNE.
	 *
	 * Cette fonction s'occupe toute seule de g�rer la queue et d'agir en cons�quence.
	 * @param msg le message CAN
	 * @return TRUE si le message CAN a �t� g�r� par cet actionneur, FALSE sinon
	 */
	bool_e BRUSH_DUNE_CAN_process_msg(CAN_msg_t* msg);

	/** G�re les commandes demand�es.
	 *
	 * Fonction � mettre sur la file pour demander une action.
	 */
	void BRUSH_DUNE_run_command(queue_id_t queueId, bool_e init);


	/**
	 * R�initialise la configuration du RX24
	 */
	void BRUSH_DUNE_reset_config();

#endif	/* BRUSH_DUNE_H */

