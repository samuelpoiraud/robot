/*  Club Robot ESEO 2014 - 2015
 *	HOLLY - WOOD
 *
 *	Fichier : carpet_launcher_left.h
 *	Package : Carte actionneur
 *	Description : Gestion de l'ax12 du lanceur de tapis gauche
 *  Auteur : Arnaud
 *  Version 20130219
 *  Robot : HOLLY
 */

#ifndef CARPET_LAUNCHER_LEFT_H
	#define	CARPET_LAUNCHER_LEFT_H

	#include "../QS/QS_all.h"
	#include "../QS/QS_CANmsgList.h"

	#ifdef I_AM_ROBOT_BIG
		#include "../queue.h"

		/** Initialisation du gestionnaire de la CARPET_LAUNCHER_LEFT.
		 *
		 * Configure l'asservissement de l'AX12
		 */
		void CARPET_LAUNCHER_LEFT_init();

		/** Initialisation en position du gestionnaire de la CARPET_LAUNCHER_LEFT.
		 *
		 * Initialise la position de l'AX12
		 */
		void CARPET_LAUNCHER_LEFT_init_pos();

		/** Stoppe l'actionneur.
		 *
		 * Cette fonction est appelée en fin de match
		 */
		void CARPET_LAUNCHER_LEFT_stop();

		/** Gère les messages CAN liés a la CARPET_LAUNCHER_LEFT.
		 *
		 * Cette fonction s'occupe toute seule de gérer la queue et d'agir en conséquence.
		 * @param msg le message CAN
		 * @return TRUE si le message CAN a été géré par cet actionneur, FALSE sinon
		 */
		bool_e CARPET_LAUNCHER_LEFT_CAN_process_msg(CAN_msg_t* msg);

		/** Gère les commandes demandées.
		 *
		 * Fonction à mettre sur la file pour demander une action.
		 */
		void CARPET_LAUNCHER_LEFT_run_command(queue_id_t queueId, bool_e init);


		/**
		 * Réinitialise la configuration de l'AX12
		 */
		void CARPET_LAUNCHER_LEFT_reset_config();

	#endif	/* CARPET_LAUNCHER_LEFT_H */

#endif
