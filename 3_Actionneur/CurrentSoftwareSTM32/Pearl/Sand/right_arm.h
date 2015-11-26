/*  Club Robot ESEO 2015 - 2016
 *	SMALL
 *
 *	Fichier : right_arm.h
 *	Package : Carte actionneur
 *	Description : Gestion de l'actionneur RIGHT_ARM
 *  Auteur : Julien
 *  Version 2016
 *  Robot : SMALL
 */

#ifndef RIGHT_ARM_H
	#define	RIGHT_ARM_H

	#include "../QS/QS_all.h"
	#include "../QS/QS_CANmsgList.h"

	#include "../queue.h"

	/** Initialisation du gestionnaire de RIGHT_ARM.
	 *
	 * Configure l'asservissement de l'AX12
	 */
	void RIGHT_ARM_init();

	/** Initialisation en position du gestionnaire de RIGHT_ARM.
	 *
	 * Initialise la position de l'AX12
	 */
	void RIGHT_ARM_init_pos();

	/** Stoppe l'actionneur.
	 *
	 * Cette fonction est appelée en fin de match
	 */
	void RIGHT_ARM_stop();

	/** Gère les messages CAN liés à la RIGHT_ARM.
	 *
	 * Cette fonction s'occupe toute seule de gérer la queue et d'agir en conséquence.
	 * @param msg le message CAN
	 * @return TRUE si le message CAN a été géré par cet actionneur, FALSE sinon
	 */
	bool_e RIGHT_ARM_CAN_process_msg(CAN_msg_t* msg);

	/** Gère les commandes demandées.
	 *
	 * Fonction à mettre sur la file pour demander une action.
	 */
	void RIGHT_ARM_run_command(queue_id_t queueId, bool_e init);


	/**
	 * Réinitialise la configuration de l'AX12
	 */
	void RIGHT_ARM_reset_config();

#endif	/* RIGHT_ARM_H */

