/*  Club Robot ESEO 2016 - 2017
 *
 *	Fichier : ORE_CATCH_ax12.h
 *	Package : Carte actionneur
 *	Description : Gestion de l'actionneur AX12 ORE_CATCH
 *  Auteur :
 *  Version 2017
 *  Robot : BIG
 */


#ifndef ORE_CATCH_H
	#define	ORE_CATCH_H

	#include "../QS/QS_all.h"
	#include "../QS/QS_CANmsgList.h"

	#include "../queue.h"

	/** Initialisation du gestionnaire de ORE_CATCH.
	 *
	 * Configure l'asservissement de l'AX12
	 */
	void ORE_CATCH_init();

	/** Initialisation en position du gestionnaire de ORE_CATCH.
	 *
	 * Initialise la position de l'AX12
	 */
	void ORE_CATCH_init_pos();

	/** Stoppe l'actionneur.
	 *
	 * Cette fonction est appelée en fin de match
	 */
	void ORE_CATCH_stop();

	/** Gère les messages CAN liés à la ORE_CATCH.
	 *
	 * Cette fonction s'occupe toute seule de gérer la queue et d'agir en conséquence.
	 * @param msg le message CAN
	 * @return TRUE si le message CAN a été géré par cet actionneur, FALSE sinon
	 */
	bool_e ORE_CATCH_CAN_process_msg(CAN_msg_t* msg);

	/** Gère les commandes demandées.
	 *
	 * Fonction à mettre sur la file pour demander une action.
	 */
	void ORE_CATCH_run_command(queue_id_t queueId, bool_e init);


	/**
	 * Réinitialise la configuration de l'AX12
	 */
	void ORE_CATCH_reset_config();

#endif	/* ORE_CATCH_H */

