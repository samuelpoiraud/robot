/*  Club Robot ESEO 2015 - 2016
 *	BIG
 *
 *	Fichier : fish_braush_back.h
 *	Package : Carte actionneur
 *	Description : Gestion de l'actionneur FISH_BRUSH_BACK
 *  Auteur : Valentin
 *  Version 2016
 *  Robot : BIG
 */

#ifndef FISH_BRUSH_BACK_H
	#define	FISH_BRUSH_BACK_H

	#include "../QS/QS_all.h"
	#include "../QS/QS_CANmsgList.h"

	#include "../queue.h"

	/** Initialisation du gestionnaire de FISH_BRUSH_BACK.
	 *
	 * Configure l'asservissement du RX24
	 */
	void FISH_BRUSH_BACK_init();

	/** Initialisation en position du gestionnaire de FISH_BRUSH_BACK.
	 *
	 * Initialise la position du RX24
	 */
	void FISH_BRUSH_BACK_init_pos();

	/** Stoppe l'actionneur.
	 *
	 * Cette fonction est appelée en fin de match
	 */
	void FISH_BRUSH_BACK_stop();

	/** Gère les messages CAN liés à la FISH_BRUSH_BACK.
	 *
	 * Cette fonction s'occupe toute seule de gérer la queue et d'agir en conséquence.
	 * @param msg le message CAN
	 * @return TRUE si le message CAN a été géré par cet actionneur, FALSE sinon
	 */
	bool_e FISH_BRUSH_BACK_CAN_process_msg(CAN_msg_t* msg);

	/** Gère les commandes demandées.
	 *
	 * Fonction à mettre sur la file pour demander une action.
	 */
	void FISH_BRUSH_BACK_run_command(queue_id_t queueId, bool_e init);


	/**
	 * Réinitialise la configuration du RX24
	 */
	void FISH_BRUSH_BACK_reset_config();

#endif	/* FISH_BRUSH_BACK_H */

