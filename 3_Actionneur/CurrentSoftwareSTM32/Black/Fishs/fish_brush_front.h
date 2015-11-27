/*  Club Robot ESEO 2015 - 2016
 *	BIG
 *
 *	Fichier : FISH_BRUSH_FRONT_front.h
 *	Package : Carte actionneur
 *	Description : Gestion de l'actionneur FISH_BRUSH_FRONT
 *  Auteur : Cailyn
 *  Version 2016
 *  Robot : BIG
 */

#ifndef FISH_BRUSH_FRONT_H
	#define	FISH_BRUSH_FRONT_H

	#include "../QS/QS_all.h"
	#include "../QS/QS_CANmsgList.h"

	#include "../queue.h"

	/** Initialisation du gestionnaire de FISH_BRUSH_FRONT.
	 *
	 * Configure l'asservissement du RX24
	 */
	void FISH_BRUSH_FRONT_init();

	/** Initialisation en position du gestionnaire de FISH_BRUSH_FRONT.
	 *
	 * Initialise la position du RX24
	 */
	void FISH_BRUSH_FRONT_init_pos();

	/** Stoppe l'actionneur.
	 *
	 * Cette fonction est appelée en fin de match
	 */
	void FISH_BRUSH_FRONT_stop();

	/** Gère les messages CAN liés à la FISH_BRUSH_FRONT.
	 *
	 * Cette fonction s'occupe toute seule de gérer la queue et d'agir en conséquence.
	 * @param msg le message CAN
	 * @return TRUE si le message CAN a été géré par cet actionneur, FALSE sinon
	 */
	bool_e FISH_BRUSH_FRONT_CAN_process_msg(CAN_msg_t* msg);

	/** Gère les commandes demandées.
	 *
	 * Fonction à mettre sur la file pour demander une action.
	 */
	void FISH_BRUSH_FRONT_run_command(queue_id_t queueId, bool_e init);


	/**
	 * Réinitialise la configuration du RX24
	 */
	void FISH_BRUSH_FRONT_reset_config();

#endif	/* FISH_BRUSH_FRONT_H */

