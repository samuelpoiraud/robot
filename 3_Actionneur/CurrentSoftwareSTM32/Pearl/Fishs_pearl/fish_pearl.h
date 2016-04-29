/*  Club Robot ESEO 2015 - 2016
 *	BIG
 *
 *	Fichier : FISH_PEARL.h
 *	Package : Carte actionneur
 *	Description : Gestion de l'actionneur FISH_PEARL
 *  Auteur :
 *  Version 2016
 *  Robot : BIG
 */

#ifndef FISH_PEARL_H
	#define	FISH_PEARL_H

	#include "../QS/QS_all.h"
	#include "../QS/QS_CANmsgList.h"

	#include "../queue.h"

	/** Initialisation du gestionnaire de FISH_PEARL.
	 *
	 * Configure l'asservissement de l'AX12
	 */
	void FISH_PEARL_init();

	/** Initialisation en position du gestionnaire de FISH_PEARL.
	 *
	 * Initialise la position de l'AX12
	 */
	void FISH_PEARL_init_pos();

	/** Stoppe l'actionneur.
	 *
	 * Cette fonction est appel�e en fin de match
	 */
	void FISH_PEARL_stop();

	/** G�re les messages CAN li�s � la FISH_PEARL.
	 *
	 * Cette fonction s'occupe toute seule de g�rer la queue et d'agir en cons�quence.
	 * @param msg le message CAN
	 * @return TRUE si le message CAN a �t� g�r� par cet actionneur, FALSE sinon
	 */
	bool_e FISH_PEARL_CAN_process_msg(CAN_msg_t* msg);

	/** G�re les commandes demand�es.
	 *
	 * Fonction � mettre sur la file pour demander une action.
	 */
	void FISH_PEARL_run_command(queue_id_t queueId, bool_e init);


	/**
	 * R�initialise la configuration de l'AX12
	 */
	void FISH_PEARL_reset_config();

#endif	/* FISH_PEARL_H */

