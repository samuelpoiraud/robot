/*  Club Robot ESEO 2016 - 2017
 *
 *	Fichier : CYLINDER_SLOPE_RIGHT_ax12.h
 *	Package : Carte actionneur
 *	Description : Gestion de l'actionneur AX12 CYLINDER_SLOPE_RIGHT
 *  Auteur :
 *  Version 2017
 *  Robot : BIG
 */


#ifndef CYLINDER_SLOPE_RIGHT_H
	#define	CYLINDER_SLOPE_RIGHT_H

	#include "../QS/QS_all.h"
	#include "../QS/QS_CANmsgList.h"

	#include "../queue.h"

	/** Initialisation du gestionnaire de CYLINDER_SLOPE_RIGHT.
	 *
	 * Configure l'asservissement de l'AX12
	 */
	void CYLINDER_SLOPE_RIGHT_init();

	/** Initialisation en position du gestionnaire de CYLINDER_SLOPE_RIGHT.
	 *
	 * Initialise la position de l'AX12
	 */
	void CYLINDER_SLOPE_RIGHT_init_pos();

	/** Stoppe l'actionneur.
	 *
	 * Cette fonction est appelée en fin de match
	 */
	void CYLINDER_SLOPE_RIGHT_stop();

	/** Gère les messages CAN liés à la CYLINDER_SLOPE_RIGHT.
	 *
	 * Cette fonction s'occupe toute seule de gérer la queue et d'agir en conséquence.
	 * @param msg le message CAN
	 * @return TRUE si le message CAN a été géré par cet actionneur, FALSE sinon
	 */
	bool_e CYLINDER_SLOPE_RIGHT_CAN_process_msg(CAN_msg_t* msg);

	/** Gère les commandes demandées.
	 *
	 * Fonction à mettre sur la file pour demander une action.
	 */
	void CYLINDER_SLOPE_RIGHT_run_command(queue_id_t queueId, bool_e init);


	/**
	 * Réinitialise la configuration de l'AX12
	 */
	void CYLINDER_SLOPE_RIGHT_reset_config();

#endif	/* CYLINDER_SLOPE_RIGHT_H */

