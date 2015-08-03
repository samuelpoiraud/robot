/*  Club Robot ESEO 2014 - 2015
 *	HOLLY - WOOD
 *
 *	Fichier : stock_RX24.h
 *	Package : Carte actionneur
 *	Description : Gestion de l'ensemble des RX24 du stock de Holly
 *  Auteur : Arnaud
 *  Version 20130219
 *  Robot : HOLLY
 */

#ifndef STOCK_RX24_H
	#define	STOCK_RX24_H

	#include "../QS/QS_all.h"
	#include "../QS/QS_CANmsgList.h"

	#ifdef I_AM_ROBOT_BIG
		#include "../queue.h"

		/** Initialisation du gestionnaire de la STOCK.
		 *
		 * Configure l'asservissement de l'AX12
		 */
		void STOCK_init();

		/** Initialisation en position du gestionnaire de la STOCK.
		 *
		 * Initialise la position de l'AX12
		 */
		void STOCK_init_pos();

		/** Stoppe l'actionneur.
		 *
		 * Cette fonction est appel�e en fin de match
		 */
		void STOCK_stop();

		/** G�re les messages CAN li�s a la STOCK.
		 *
		 * Cette fonction s'occupe toute seule de g�rer la queue et d'agir en cons�quence.
		 * @param msg le message CAN
		 * @return TRUE si le message CAN a �t� g�r� par cet actionneur, FALSE sinon
		 */
		bool_e STOCK_CAN_process_msg(CAN_msg_t* msg);

		/** G�re les commandes demand�es.
		 *
		 * Fonction � mettre sur la file pour demander une action.
		 */
		void STOCK_run_command(queue_id_t queueId, bool_e init);

		/**
		 * R�initialise la configuration de l'AX12
		 */
		void STOCK_reset_config();

	#endif	/* PINCEMI_RX24_H */

#endif

