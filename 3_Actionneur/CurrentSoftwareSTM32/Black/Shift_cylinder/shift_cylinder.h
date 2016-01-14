/*  Club Robot ESEO 2015 - 2016
 *	BIG
 *
 *	Fichier : shift_cylinder.h
 *	Package : Carte actionneur
 *	Description : Gestion de l'actionneur RX24 SHIFT_CYLINDER
 *  Auteur : Valentin
 *  Version 2016
 *  Robot : BIG
 */

#ifndef SHIFT_CYLINDER_H
	#define	SHIFT_CYLINDER_H

	#include "../QS/QS_all.h"
	#include "../QS/QS_CANmsgList.h"

	#include "../queue.h"

	/** Initialisation du gestionnaire de SHIFT_CYLINDER.
	 *
	 * Configure l'asservissement du RX24
	 */
	void SHIFT_CYLINDER_init();

	/** Initialisation en position du gestionnaire de SHIFT_CYLINDER.
	 *
	 * Initialise la position du RX24
	 */
	void SHIFT_CYLINDER_init_pos();

	/** Stoppe l'actionneur.
	 *
	 * Cette fonction est appelée en fin de match
	 */
	void SHIFT_CYLINDER_stop();

	/** Gère les messages CAN liés à la SHIFT_CYLINDER.
	 *
	 * Cette fonction s'occupe toute seule de gérer la queue et d'agir en conséquence.
	 * @param msg le message CAN
	 * @return TRUE si le message CAN a été géré par cet actionneur, FALSE sinon
	 */
	bool_e SHIFT_CYLINDER_CAN_process_msg(CAN_msg_t* msg);

	/** Gère les commandes demandées.
	 *
	 * Fonction à mettre sur la file pour demander une action.
	 */
	void SHIFT_CYLINDER_run_command(queue_id_t queueId, bool_e init);


	/**
	 * Réinitialise la configuration du RX24
	 */
	void SHIFT_CYLINDER_reset_config();

#endif	/* SHIFT_CYLINDER_H */

