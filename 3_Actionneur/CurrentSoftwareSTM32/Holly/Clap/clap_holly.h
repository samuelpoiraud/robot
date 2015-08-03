/*  Club Robot ESEO 2014 - 2015
 *	HOLLY - WOOD
 *
 *	Fichier : clap_holly.h
 *	Package : Carte actionneur
 *	Description : Gestion de l'actionneur clap
 *  Auteur : Arnaud
 *  Version 20130219
 *  Robot : HOLLY
 */

#ifndef CLAP_HOLLY_H
	#define	CLAP_HOLLY_H

	#include "../QS/QS_all.h"
	#include "../QS/QS_CANmsgList.h"

	#ifdef I_AM_ROBOT_BIG
		#include "../queue.h"

		// Initialisation de l'CLAP_HOLLY.
		void CLAP_HOLLY_init();

		// Initialisation en position de l'CLAP_HOLLY.
		void CLAP_HOLLY_init_pos();

		// Stoppe l'actionneur.
		void CLAP_HOLLY_stop();

		/** Gère les messages CAN liés a la CLAP_HOLLY.
		 *
		 * Cette fonction s'occupe toute seule de gérer la queue et d'agir en conséquence.
		 * @param msg le message CAN
		 * @return TRUE si le message CAN a été géré par cet actionneur, FALSE sinon
		 */
		bool_e CLAP_HOLLY_CAN_process_msg(CAN_msg_t* msg);

		/** Gère les commandes demandées.
		 *
		 * Fonction à mettre sur la file pour demander une action.
		 */
		void CLAP_HOLLY_run_command(queue_id_t queueId, bool_e init);

		void CLAP_HOLLY_reset_config();

		Sint16 CLAP_HOLLY_get_position();

		void CLAP_process_it();

	#endif	/* CLAP_HOLLY_H */

#endif
