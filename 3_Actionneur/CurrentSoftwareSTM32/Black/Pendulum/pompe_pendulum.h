/*  Club Robot ESEO 2015 - 2016
 *
 *	Fichier : pompe_pendulum.c
 *	Package : Carte actionneur
 *	Description : Gestion de la pompe PENDULUM
 *  Auteur : Valentin
 *  Version 2016
 *  Robot : BIG
 */


#ifndef POMPE_PENDULUM_H
	#define	POMPE_PENDULUM_H

	#include "../QS/QS_all.h"
	#include "../QS/QS_CANmsgList.h"

	#ifdef I_AM_ROBOT_BIG

		#include "../queue.h"

		/** Initialisation du gestionnaire de POMPE_PENDULUM.
		 *
		 * Configure du moteur de la POMPE_PENDULUM
		 */
		void POMPE_PENDULUM_init();

		/** Initialisation en position du gestionnaire de POMPE_PENDULUM.
		 *
		 * Initialise de la position de la POMPE_PENDULUM (stopé)
		 */
		void POMPE_PENDULUM_init_pos();

		/** Stoppe l'actionneur.
		 *
		 * Cette fonction est appelée en fin de match
		 */
		void POMPE_PENDULUM_stop();

		/** Gère les messages CAN liés au POMPE_PENDULUM.
		 *
		 * Cette fonction s'occupe toute seule de gérer la queue et d'agir en conséquence.
		 * @param msg le message CAN
		 * @return TRUE si le message CAN a été géré par cet actionneur, FALSE sinon
		 */
		bool_e POMPE_PENDULUM_CAN_process_msg(CAN_msg_t* msg);

		/** Gère les commandes demandées.
		 *
		 * Fonction à mettre sur la file pour demander une action.
		 */
		void POMPE_PENDULUM_run_command(queue_id_t queueId, bool_e init);

		/**
		 * Réinitialise la configuration de la pompe
		 */
		void POMPE_PENDULUM_reset_config();

	#endif
#endif	/* POMPE_PENDULUM_H */

