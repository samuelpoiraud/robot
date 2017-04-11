/*  Club Robot ESEO 2016 - 2017
*
*	Fichier : SMALL_POMPE_PRISE.h
*	Package : Carte actionneur
*	Description : Gestion de la pompe EXEMPLE
*	Auteur :
*	Version 2017
*	Robot : BIG
*/


#ifndef SMALL_POMPE_PRISE_H
	#define	SMALL_POMPE_PRISE_H

	#include "../QS/QS_all.h"
	#include "../QS/QS_CANmsgList.h"

	#ifdef I_AM_ROBOT_SMALL

		#include "../queue.h"

		/** Initialisation du gestionnaire de SMALL_POMPE_PRISE.
		 *
		 * Configure du moteur de la SMALL_POMPE_PRISE
		 */
		void SMALL_POMPE_PRISE_init();

		/** Initialisation en position du gestionnaire de SMALL_POMPE_PRISE.
		 *
		 * Initialise de la position de la SMALL_POMPE_PRISE (stopé)
		 */
		void SMALL_POMPE_PRISE_init_pos();

		/** Stoppe l'actionneur.
		 *
		 * Cette fonction est appelée en fin de match
		 */
		void SMALL_POMPE_PRISE_stop();

		/** Gère les messages CAN liés au SMALL_POMPE_PRISE.
		 *
		 * Cette fonction s'occupe toute seule de gérer la queue et d'agir en conséquence.
		 * @param msg le message CAN
		 * @return TRUE si le message CAN a été géré par cet actionneur, FALSE sinon
		 */
		bool_e SMALL_POMPE_PRISE_CAN_process_msg(CAN_msg_t* msg);

		/** Gère les commandes demandées.
		 *
		 * Fonction à mettre sur la file pour demander une action.
		 */
		void SMALL_POMPE_PRISE_run_command(queue_id_t queueId, bool_e init);

		/**
		 * Réinitialise la configuration de la pompe
		 */
		void SMALL_POMPE_PRISE_reset_config();

	#endif
#endif	/* SMALL_POMPE_PRISE_H */

