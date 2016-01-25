/*  Club Robot ESEO 2015 - 2016
 *
 *	Fichier : pompe_black_front_left.c
 *	Package : Carte actionneur
 *	Description : Gestion de la pompe BLACK_FRONT_LEFT
 *  Auteur : Valentin
 *  Version 2016
 *  Robot : BIG
 */


#ifndef POMPE_BLACK_FRONT_LEFT_H
	#define	POMPE_BLACK_FRONT_LEFT_H

	#include "../QS/QS_all.h"
	#include "../QS/QS_CANmsgList.h"

	#ifdef I_AM_ROBOT_BIG

		#include "../queue.h"

		/** Initialisation du gestionnaire de POMPE_BLACK_FRONT_LEFT.
		 *
		 * Configure du moteur de la POMPE_BLACK_FRONT_LEFT
		 */
		void POMPE_BLACK_FRONT_LEFT_init();

		/** Initialisation en position du gestionnaire de POMPE_BLACK_FRONT_LEFT.
		 *
		 * Initialise de la position de la POMPE_BLACK_FRONT_LEFT (stopé)
		 */
		void POMPE_BLACK_FRONT_LEFT_init_pos();

		/** Stoppe l'actionneur.
		 *
		 * Cette fonction est appelée en fin de match
		 */
		void POMPE_BLACK_FRONT_LEFT_stop();

		/** Gère les messages CAN liés au POMPE_BLACK_FRONT_LEFT.
		 *
		 * Cette fonction s'occupe toute seule de gérer la queue et d'agir en conséquence.
		 * @param msg le message CAN
		 * @return TRUE si le message CAN a été géré par cet actionneur, FALSE sinon
		 */
		bool_e POMPE_BLACK_FRONT_LEFT_CAN_process_msg(CAN_msg_t* msg);

		/** Gère les commandes demandées.
		 *
		 * Fonction à mettre sur la file pour demander une action.
		 */
		void POMPE_BLACK_FRONT_LEFT_run_command(queue_id_t queueId, bool_e init);

		/**
		 * Réinitialise la configuration de la pompe
		 */
		void POMPE_BLACK_FRONT_LEFT_reset_config();

	#endif
#endif	/* POMPE_BLACK_FRONT_LEFT_H */

