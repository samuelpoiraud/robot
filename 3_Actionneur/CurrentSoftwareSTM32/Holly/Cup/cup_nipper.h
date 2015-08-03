/*  Club Robot ESEO 2014 - 2015
 *	HOLLY - WOOD
 *
 *	Fichier : cup_nipper.h
 *	Package : Carte actionneur
 *	Description : Gestion de la pince à gobelet
 *  Auteur : Arnaud
 *  Version 20130219
 *  Robot : HOLLY
 */

#ifndef CUP_NIPPER_H
	#define	CUP_NIPPER_H

	#include "../QS/QS_all.h"
	#include "../QS/QS_CANmsgList.h"

	#ifdef I_AM_ROBOT_BIG
		#include "../queue.h"

		// Initialisation de l'CUP_NIPPER.
		void CUP_NIPPER_init();

		// Initialisation en position de l'CUP_NIPPER.
		void CUP_NIPPER_init_pos();

		// Stoppe l'actionneur.
		void CUP_NIPPER_stop();

		/** Gère les messages CAN liés a la CUP_NIPPER.
		 *
		 * Cette fonction s'occupe toute seule de gérer la queue et d'agir en conséquence.
		 * @param msg le message CAN
		 * @return TRUE si le message CAN a été géré par cet actionneur, FALSE sinon
		 */
		bool_e CUP_NIPPER_CAN_process_msg(CAN_msg_t* msg);

		/** Gère les commandes demandées.
		 *
		 * Fonction à mettre sur la file pour demander une action.
		 */
		void CUP_NIPPER_run_command(queue_id_t queueId, bool_e init);

		void CUP_NIPPER_state_machine();

		void CUP_NIPPER_process_it();

		void CUP_NIPPER_reset_config();

	#endif	/* CUP_NIPPER_H */

#endif

