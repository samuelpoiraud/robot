/*  Club Robot ESEO 2016 - 2017
*
*	Fichier : pompe_exemple.h
*	Package : Carte actionneur
*	Description : Gestion de la pompe EXEMPLE
*	Auteur :
*	Version 2017
*	Robot : BIG
*/


#ifndef POMPE_EXEMPLE_H
	#define	POMPE_EXEMPLE_H

	#include "../QS/QS_all.h"
	#include "../QS/QS_CANmsgList.h"

	#ifdef I_AM_ROBOT_BIG

		#include "../queue.h"

		/** Initialisation du gestionnaire de POMPE_EXEMPLE.
		 *
		 * Configure du moteur de la POMPE_EXEMPLE
		 */
		void POMPE_EXEMPLE_init();

		/** Initialisation en position du gestionnaire de POMPE_EXEMPLE.
		 *
		 * Initialise de la position de la POMPE_EXEMPLE (stop�)
		 */
		void POMPE_EXEMPLE_init_pos();

		/** Stoppe l'actionneur.
		 *
		 * Cette fonction est appel�e en fin de match
		 */
		void POMPE_EXEMPLE_stop();

		/** G�re les messages CAN li�s au POMPE_EXEMPLE.
		 *
		 * Cette fonction s'occupe toute seule de g�rer la queue et d'agir en cons�quence.
		 * @param msg le message CAN
		 * @return TRUE si le message CAN a �t� g�r� par cet actionneur, FALSE sinon
		 */
		bool_e POMPE_EXEMPLE_CAN_process_msg(CAN_msg_t* msg);

		/** G�re les commandes demand�es.
		 *
		 * Fonction � mettre sur la file pour demander une action.
		 */
		void POMPE_EXEMPLE_run_command(queue_id_t queueId, bool_e init);

		/**
		 * R�initialise la configuration de la pompe
		 */
		void POMPE_EXEMPLE_reset_config();

	#endif
#endif	/* POMPE_EXEMPLE_H */

