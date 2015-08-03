/*  Club Robot ESEO 2014 - 2015
 *	HOLLY - WOOD
 *
 *	Fichier : Pop_drop_left.h
 *	Package : Carte actionneur
 *	Description : Gestion de l'ax12 de la depose des popcorns gauche
 *  Auteur : Anthony
 *  Version 20130219
 *  Robot : HOLLY
 */

#ifndef POP_DROP_LEFT_H
	#define	POP_DROP_LEFT_H

	#include "../QS/QS_all.h"
	#include "../QS/QS_CANmsgList.h"

	#ifdef I_AM_ROBOT_BIG
		#include "../queue.h"

		/** Initialisation du gestionnaire de la POP_DROP_LEFT.
		 *
		 * Configure l'asservissement de l'AX12
		 */
		void POP_DROP_LEFT_init();

		/** Initialisation en position du gestionnaire de la POP_DROP_LEFT.
		 *
		 * Initialise la position de l'AX12
		 */
		void POP_DROP_LEFT_init_pos();

		/** Stoppe l'actionneur.
		 *
		 * Cette fonction est appel�e en fin de match
		 */
		void POP_DROP_LEFT_stop();

		/** G�re les messages CAN li�s a la POP_DROP_LEFT.
		 *
		 * Cette fonction s'occupe toute seule de g�rer la queue et d'agir en cons�quence.
		 * @param msg le message CAN
		 * @return TRUE si le message CAN a �t� g�r� par cet actionneur, FALSE sinon
		 */
		bool_e POP_DROP_LEFT_CAN_process_msg(CAN_msg_t* msg);

		/** G�re les commandes demand�es.
		 *
		 * Fonction � mettre sur la file pour demander une action.
		 */
		void POP_DROP_LEFT_run_command(queue_id_t queueId, bool_e init);


		/**
		 * R�initialise la configuration de l'AX12
		 */
		void POP_DROP_LEFT_reset_config();

	#endif	/* PEXEMPLE_H */

#endif
