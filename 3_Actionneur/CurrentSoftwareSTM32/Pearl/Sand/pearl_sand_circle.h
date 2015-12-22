/*  Club Robot ESEO 2015 - 2016
 *	BIG
 *
 *	Fichier : pearl_sand_circle.h
 *	Package : Carte actionneur
 *	Description : Gestion de l'actionneur PEARL_SAND_CIRCLE
 *  Auteur :
 *  Version 2016
 *  Robot : BIG
 */

#ifndef PEARL_SAND_CIRCLE_H
	#define	PEARL_SAND_CIRCLE_H

	#include "../QS/QS_all.h"
	#include "../QS/QS_CANmsgList.h"

	#include "../queue.h"

	/** Initialisation du gestionnaire de PEARL_SAND_CIRCLE.
	 *
	 * Configure l'asservissement de l'AX12
	 */
	void PEARL_SAND_CIRCLE_init();

	/** Initialisation en position du gestionnaire de PEARL_SAND_CIRCLE.
	 *
	 * Initialise la position de l'AX12
	 */
	void PEARL_SAND_CIRCLE_init_pos();

	/** Stoppe l'actionneur.
	 *
	 * Cette fonction est appelée en fin de match
	 */
	void PEARL_SAND_CIRCLE_stop();

	/** Gère les messages CAN liés à la PEARL_SAND_CIRCLE.
	 *
	 * Cette fonction s'occupe toute seule de gérer la queue et d'agir en conséquence.
	 * @param msg le message CAN
	 * @return TRUE si le message CAN a été géré par cet actionneur, FALSE sinon
	 */
	bool_e PEARL_SAND_CIRCLE_CAN_process_msg(CAN_msg_t* msg);

	/** Gère les commandes demandées.
	 *
	 * Fonction à mettre sur la file pour demander une action.
	 */
	void PEARL_SAND_CIRCLE_run_command(queue_id_t queueId, bool_e init);


	/**
	 * Réinitialise la configuration de l'AX12
	 */
	void PEARL_SAND_CIRCLE_reset_config();

#endif	/* PEARL_SAND_CIRCLE_H */

