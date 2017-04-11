/*  Club Robot ESEO 2016 - 2017
*
*	Fichier : SMALL_POMPE_DISPOSE.h
*	Package : Carte actionneur
*	Description : Gestion de la pompe EXEMPLE
*	Auteur :
*	Version 2017
*	Robot : BIG
*/


#ifndef SMALL_POMPE_DISPOSE_H
	#define	SMALL_POMPE_DISPOSE_H

	#include "../QS/QS_all.h"
	#include "../QS/QS_CANmsgList.h"

	#ifdef I_AM_ROBOT_SMALL

		#include "../queue.h"

		/** Initialisation du gestionnaire de SMALL_POMPE_DISPOSE.
		 *
		 * Configure du moteur de la SMALL_POMPE_DISPOSE
		 */
		void SMALL_POMPE_DISPOSE_init();

		/** Initialisation en position du gestionnaire de SMALL_POMPE_DISPOSE.
		 *
		 * Initialise de la position de la SMALL_POMPE_DISPOSE (stop�)
		 */
		void SMALL_POMPE_DISPOSE_init_pos();

		/** Stoppe l'actionneur.
		 *
		 * Cette fonction est appel�e en fin de match
		 */
		void SMALL_POMPE_DISPOSE_stop();

		/** G�re les messages CAN li�s au SMALL_POMPE_DISPOSE.
		 *
		 * Cette fonction s'occupe toute seule de g�rer la queue et d'agir en cons�quence.
		 * @param msg le message CAN
		 * @return TRUE si le message CAN a �t� g�r� par cet actionneur, FALSE sinon
		 */
		bool_e SMALL_POMPE_DISPOSE_CAN_process_msg(CAN_msg_t* msg);

		/** G�re les commandes demand�es.
		 *
		 * Fonction � mettre sur la file pour demander une action.
		 */
		void SMALL_POMPE_DISPOSE_run_command(queue_id_t queueId, bool_e init);

		/**
		 * R�initialise la configuration de la pompe
		 */
		void SMALL_POMPE_DISPOSE_reset_config();

	#endif
#endif	/* SMALL_POMPE_DISPOSE_H */

