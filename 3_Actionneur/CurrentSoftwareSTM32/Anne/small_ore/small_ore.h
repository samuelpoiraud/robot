/*  Club Robot ESEO 2016 - 2017
 *
 *	Fichier : SMALL_ORE_ax12.h
 *	Package : Carte actionneur
 *	Description : Gestion de l'actionneur AX12 SMALL_ORE
 *  Auteur :
 *  Version 2017
 *  Robot : BIG
 */


#ifndef SMALL_ORE_H
	#define	SMALL_ORE_H

	#include "../QS/QS_all.h"
	#include "../QS/QS_CANmsgList.h"

	#include "../queue.h"

	/** Initialisation du gestionnaire de SMALL_ORE.
	 *
	 * Configure l'asservissement de l'AX12
	 */
	void SMALL_ORE_init();

	/** Initialisation en position du gestionnaire de SMALL_ORE.
	 *
	 * Initialise la position de l'AX12
	 */
	void SMALL_ORE_init_pos();

	/** Stoppe l'actionneur.
	 *
	 * Cette fonction est appelée en fin de match
	 */
	void SMALL_ORE_stop();

	/** Gère les messages CAN liés à la SMALL_ORE.
	 *
	 * Cette fonction s'occupe toute seule de gérer la queue et d'agir en conséquence.
	 * @param msg le message CAN
	 * @return TRUE si le message CAN a été géré par cet actionneur, FALSE sinon
	 */
	bool_e SMALL_ORE_CAN_process_msg(CAN_msg_t* msg);

	/** Gère les commandes demandées.
	 *
	 * Fonction à mettre sur la file pour demander une action.
	 */
	void SMALL_ORE_run_command(queue_id_t queueId, bool_e init);


	/**
	 * Réinitialise la configuration de l'AX12
	 */
	void SMALL_ORE_reset_config();

#endif	/* SMALL_ORE_H */

