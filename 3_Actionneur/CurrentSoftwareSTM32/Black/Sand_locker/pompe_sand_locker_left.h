/*  Club Robot ESEO 2015 - 2016
 *
 *	Fichier : pompe_SAND_LOCKER_LEFT.c
 *	Package : Carte actionneur
 *	Description : Gestion de la pompe SAND_LOCKER_LEFT
 *  Auteur : Valentin
 *  Version 2016
 *  Robot : BIG
 */


#ifndef POMPE_SAND_LOCKER_LEFT_H
	#define	POMPE_SAND_LOCKER_LEFT_H

	#include "../QS/QS_all.h"
	#include "../QS/QS_CANmsgList.h"

	#ifdef I_AM_ROBOT_BIG

		#include "../queue.h"

		/** Initialisation du gestionnaire de POMPE_SAND_LOCKER_LEFT.
		 *
		 * Configure du moteur de la POMPE_SAND_LOCKER_LEFT
		 */
		void POMPE_SAND_LOCKER_LEFT_init();

		/** Initialisation en position du gestionnaire de POMPE_SAND_LOCKER_LEFT.
		 *
		 * Initialise de la position de la POMPE_SAND_LOCKER_LEFT (stop�)
		 */
		void POMPE_SAND_LOCKER_LEFT_init_pos();

		/** Stoppe l'actionneur.
		 *
		 * Cette fonction est appel�e en fin de match
		 */
		void POMPE_SAND_LOCKER_LEFT_stop();

		/** G�re les messages CAN li�s au POMPE_SAND_LOCKER_LEFT.
		 *
		 * Cette fonction s'occupe toute seule de g�rer la queue et d'agir en cons�quence.
		 * @param msg le message CAN
		 * @return TRUE si le message CAN a �t� g�r� par cet actionneur, FALSE sinon
		 */
		bool_e POMPE_SAND_LOCKER_LEFT_CAN_process_msg(CAN_msg_t* msg);

		/** G�re les commandes demand�es.
		 *
		 * Fonction � mettre sur la file pour demander une action.
		 */
		void POMPE_SAND_LOCKER_LEFT_run_command(queue_id_t queueId, bool_e init);

		/**
		 * R�initialise la configuration de la pompe
		 */
		void POMPE_SAND_LOCKER_LEFT_reset_config();

	#endif
#endif	/* POMPE_SAND_LOCKER_LEFT_H */

